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

void __attribute__ ((constructor)) my_init(void);
void __attribute__ ((destructor)) my_fini(void);

// function declarations
int print_caller_and_address();
char *get_process_name_by_pid();
static int (*real_mknod)(const char *pathname, mode_t mode, dev_t dev) = NULL;

#ifdef VERBOSE
static int (*real_system)(const char *command) = NULL;
static FILE *(*real_fopen)(const char *filename, const char *mode) = NULL;
static int (*real_open)(const char *pathname, int flags) = NULL;
#endif

void my_init(void) {
#ifdef VERBOSE
   if(!real_fopen) {
      real_fopen = dlsym(RTLD_NEXT, "fopen");
   }
   if(!real_system) {
      real_system = dlsym(RTLD_NEXT, "system");
   }
   if(!real_open) {
      real_open = dlsym(RTLD_NEXT, "open");
   }
#endif
   if(!real_mknod) {
      real_mknod = dlsym(RTLD_NEXT, "mknod");
   }
}

void my_fini(void) {
}

char *get_process_name_by_pid()
{
   static char name[1024];

   // ugly approach, but oh well, it works.
   sprintf(name, "/proc/%d/cmdline", getpid());

   // real_fopen must be initialised by the constructor
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

int print_caller_and_address()
{
   printf("%s [0x%08x] ", get_process_name_by_pid(), __builtin_return_address(0));  // get caller's address
}

#ifdef VERBOSE
// hook system()
// do this only if we are performing VERBOSE traces
// otherwise it conflicts with libuClibc's system()
// symbol
int system(const char *command)
{
   int r;
   print_caller_and_address();
   r = real_system(command);
   printf("system('%s') = %d\n", command, r);
   return(r);
}
#endif

#ifdef VERBOSE
// hook fopen()
// do this only if we are performing VERBOSE traces
FILE *fopen(const char *filename, const char *mode)
{
   FILE *fp;
   print_caller_and_address();
   fp = real_fopen(filename, mode);
   printf("fopen('%s', '%s') = 0x%08x\n", filename, mode, (unsigned int) fp);
   return(fp);
}
#endif

#ifdef VERBOSE
// hook open()
// do this only if we are performing VERBOSE traces
int open(const char *pathname, int flags)
{
   int r;
   print_caller_and_address();
   r = real_open(pathname, flags);
   printf("open('%s', %d) = %d\n", pathname, flags, r);
   return(r);
}
#endif

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
   r = real_mknod(pathname, mode, dev);
#ifdef VERBOSE
   printf("mknod('%s', %d, %d) = %d\n", pathname, mode, dev, r);
#endif
   return(r);
}
