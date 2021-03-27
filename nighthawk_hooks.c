/* nighthawk_hooks.c
 *
 * Fakes some functions from libacos_shared.so and libnat.so
 * for the Netgear Nighthawk line of routers otherwise the
 * R6250/R6400's webserver doesn't start
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

int nvram_match(char *uParm1, char *pcParm2); // match
int nvram_invmatch(char *uParm1, char *pcParm2); // match
char *nvram_get_ex2(char *uParm1, char *pcParm2, int sParm3); // read
int nvram_set(char *uParm1, char *pcParm2); // set
char *nvram_get(char *uParm1); // get
int nvram_unset(char *uParm1); // unset

// ./buildroot-2013.05/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc [-DQUIET] -shared -fPIC -o nighthawk_hooks.so nighthawk_hooks.c

static int counter = 0;

// function declarations
static int (*real_system)(const char *command) = NULL;
static FILE *(*real_fopen)(const char *filename, const char *mode) = NULL;
static int (*real_open)(const char *pathname, int flags) = NULL;
static int (*real_mknod)(const char *pathname, mode_t mode, dev_t dev) = NULL;

// hook system()
int system(const char *command)
{
   int r;
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif
   real_system = dlsym(RTLD_NEXT, "system");
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
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif
   real_fopen = dlsym(RTLD_NEXT, "fopen");
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
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif
   real_open = dlsym(RTLD_NEXT, "open");
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
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif
   real_mknod = dlsym(RTLD_NEXT, "mknod");
   r = real_mknod(pathname, mode, dev);
#ifdef VERBOSE
   printf("mknod('%s', %d, %d) = %d\n", pathname, mode, dev, r);
#endif
   return(r);
}

/* acosNvram functions - call counterparts in libnvram.so */
char *acosNvramConfig_get(char *k)
{
   char *v = "";

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   v = nvram_get(k);

   if(v == 0)
      v = "";

#ifdef VERBOSE
   printf("acosNvramConfig_get('%s') = '%s'\n", k, v);
#endif
   return(v);
}

char *acosNvramConfig_exist(char *k)
{
   char *v = "";

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   v = nvram_get(k);

   if(v == 0)
      v = "";

#ifdef VERBOSE
   printf("acosNvramConfig_exist('%s') = '%s'\n", k, v);
#endif
   return(v);
}

int acosNvramConfig_set(char *k, char *v) {
   int i;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   i = nvram_set(k, v);

#ifdef VERBOSE
   printf("[nvram %d] acosNvramConfig_set('%s', '%s')\n", i, k, v);
#endif
   return(0);
}

void acosNvramConfig_read(char *k, char *r, int len) {
   char* v = "";

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   //v = nvram_get(k);
   //
   //strncpy(r, v, len);
   nvram_get_ex2(k, r, len);
#ifdef VERBOSE
   printf("acosNvramConfig_read('%s', '%s', %d)\n", k, r, len);
#endif
}

int acosNvramConfig_unset(char *k) {
   int i;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   i = nvram_unset(k);

#ifdef VERBOSE
   printf("[nvram %d] acosNvramConfig_unset('%s')\n", i, k);
#endif
   return(i);
}

int acosNvramConfig_match(char *k, char *v) {
   // return 0 (False) by default
   int r = 0;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   r = nvram_match(k, v);

#ifdef VERBOSE
   printf("acosNvramConfig_match('%s', '%s') = %d\n", k, v, r);
#endif
   return(r);
}

int acosNvramConfig_invmatch(char *k, char *v) {
   // return 0 (False) by default
   int r = 0;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   r = nvram_invmatch(k, v);

#ifdef VERBOSE
   printf("acosNvramConfig_invmatch('%s', '%s') = %d\n", k, v, r);
#endif
   return(r);
}

int acosNvramConfig_save(void) {
   // nvram_commit
   // return 1 (true)
   return(1);
}

// built up from Ghidra's decompiler
int acosNvramConfig_readAsInt(char *k, int *r) {
   char *v = "";
   int n;

   if(k == (char *) 0 || r == (int *) 0) {
      return(1);
   }
   v = nvram_get(k);
   if(v == (char *) 0) {
      v = (char *) 0x18920;
   }
   n = atoi(v);
   *r = n;
   return(0);
}

/* intercepted other libacos_shared.so functions */

int agApi_fwServiceAdd(char *k, int a, int b, int c) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif
   counter++;
#ifdef VERBOSE
   printf("agApi_fwServiceAdd('%s', %d, %d, %d) = %d\n", k, a, b, c, counter);
#endif
   return(counter);
}

int agApi_fwURLFilterEnableTmSch_Session2(int x) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwURLFilterEnableTmSch_Session2(%d) = 0\n", x);
#endif
   return(0);
}

int agApi_fwURLFilterEnable_Session2(int x) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwURLFilterEnable_Session2(%d) = 0\n", x);
#endif
   return(0);
}

int agApi_tmschDelConf(char *k) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_tmschDelConf('%s') = 0\n", k);
#endif
   return(0);
}

int agApi_tmschAddConf(char *a, char *b, char *c, char *d, char *e, int f, int g, int h) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_tmschAddConf('%s', '%s', '%s', '%s', '%s', %d, %d, %d)\n", a, b, c, d, e, f, g, h);
#endif
   return(0);
}

int agApi_tmschDelConf_Session2(char *k) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_tmschDelConf_Session2('%s') = 0\n", k);
#endif
   return(0);
}

int agApi_tmschAddConf_Session2(char *a, char *b, char *c, char *d, char *e, int f, int g, int h) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_tmschAddConf_Session2('%s', '%s', '%s', '%s', '%s', %d, %d, %d)\n", a, b, c, d, e, f, g, h);
#endif
   return(0);
}

int agApi_fwBlkServModAction(char *k) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwBlkServModAction('%s') = 0\n", k);
#endif
   return(0);
}

int agApi_fwBlkServModAction_Session2(char *k) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwBlkServModAction('%s') = 0\n", k);
#endif
   return(0);
}

int agApi_fwEchoRespSet(int x) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwEchoRespSet(%d) = 1\n", x);
#endif
   return(1);
}

int agApi_fwURLFilterEnable(int x) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwURLFilterEnable(%d) = 0\n", x);
#endif
   return(0);
}

int agApi_fwURLFilterEnableTmSch() {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwURLFilterEnableTmSch() = 0\n");
#endif
   return(0);
}

int agApi_fwGetAllServices(char *k, int a) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwGetAllServices('%s', %d) = %d\n", k, a, counter);
#endif
   return(counter);
}

void agApi_fwDelTriggerConf2(char *k) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwDelTriggerConf2('%s')\n", k);
#endif
}

int agApi_fwGetNextTriggerConf(int a) {
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("agApi_fwGetNextTriggerConf(0x%08x) = 1\n", a);
#endif
   return(1);
}
