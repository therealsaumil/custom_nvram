/* common_hooks.c
 *
 * Hooks functions such as system, fopen, open, mknod for observing
 * from within the process. strace and ltrace don't provide all the
 * information I need, and I haven't figure out how to use dtrace
 * properly yet.
 *
 * by Saumil Shah
 * @therealsaumil
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>

// ./buildroot-2013.05/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc [-DQUIET] -shared -fPIC -o nighthawk_hooks.so nighthawk_hooks.c common_hooks.c

// function declarations
char *get_process_name_by_pid();
void print_caller_and_address();
static int (*real_system)(const char *command) = NULL;
static FILE *(*real_fopen)(const char *filename, const char *mode) = NULL;
static int (*real_open)(const char *pathname, int flags) = NULL;
static int (*real_mknod)(const char *pathname, mode_t mode, dev_t dev) = NULL;

char *get_process_name_by_pid()
{
   static char name[1024];

   // ugly approach, but oh well, it works.
   sprintf(name, "/proc/%d/cmdline", getpid());

   if(!real_fopen) {
      real_fopen = dlsym(RTLD_NEXT, "fopen");
   }
   FILE *f = real_fopen(name, "r");

   if(f) {
      size_t size;
      size = fread(name, sizeof(char), 1024, f);
      if(size > 0){
         if('\n' == name[size - 1])
         name[size - 1] = '\0';
      }
      fclose(f);
   }
   return name;
}

void print_caller_and_address()
{
   printf("%s [0x%08x] ", get_process_name_by_pid(), __builtin_return_address(0));  // get caller's address
}

// hook system()
int system(const char *command)
{
   int r;
#ifdef VERBOSE
   print_caller_and_address();
#endif
   if(!real_system) {
      real_system = dlsym(RTLD_NEXT, "system");
   }
   r = real_system(command);
#ifdef VERBOSE
   printf("system('%s') = %d\n", command, r);
#endif
   return(r);
}

// hook fopen()
FILE *fopen(const char *filename, const char *mode)
{
   FILE *fp;
#ifdef VERBOSE
   print_caller_and_address();
#endif
   if(!real_fopen) {
      real_fopen = dlsym(RTLD_NEXT, "fopen");
   }
   fp = real_fopen(filename, mode);
#ifdef VERBOSE
   printf("fopen('%s', '%s') = 0x%08x\n", filename, mode, (unsigned int) fp);
#endif
   return(fp);
}

// hook open()
int open(const char *pathname, int flags)
{
   int r;
#ifdef VERBOSE
   print_caller_and_address();
#endif
   if(!real_open) {
      real_open = dlsym(RTLD_NEXT, "open");
   }
   r = real_open(pathname, flags);
#ifdef VERBOSE
   printf("open('%s', %d) = %d\n", pathname, flags, r);
#endif
   return(r);
}

// hook mknod()
int mknod(const char *pathname, mode_t mode, dev_t dev)
{
   int r;

#ifdef VERBOSE
   print_caller_and_address();
#endif

   // we want to override creation of /dev/nvram
   if(strcmp(pathname, "/dev/nvram") == 0) {
#ifdef VERBOSE
      printf("mknod('%s', %d, %d) = SKIPPED\n", pathname, mode, dev);
#endif
      return(0);
   }
   if(!real_mknod) {
      real_mknod = dlsym(RTLD_NEXT, "mknod");
   }
   r = real_mknod(pathname, mode, dev);
#ifdef VERBOSE
   printf("mknod('%s', %d, %d) = %d\n", pathname, mode, dev, r);
#endif
   return(r);
}
