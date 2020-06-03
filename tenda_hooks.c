/* tenda_hooks.c
 *
 * Fakes some functions from libcfm.so
 * for the Tenda AC15/ACx line of routers otherwise the
 * webserver doesn't start
 *
 * by Saumil Shah
 * @therealsaumil
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

int nvram_set(char *uParm1, char *pcParm2); // set
char *nvram_get(char *uParm1); // get
int nvram_unset(char *uParm1); // unset

#ifdef VERBOSE
// function declarations for hooking.
// we have no real use for hooking, other than just observation.
// if we don't enable verbose mode, don't even hook these functions.
static int (*real_system)(const char *command) = NULL;
//static int (*real_GetCfmValue)(char *name, char *value) = NULL;

// hook system()
int system(const char *command)
{
   int r;
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   if(!real_system)
      real_system = dlsym(RTLD_NEXT, "system");
   r = real_system(command);
   printf("system('%s') = %d\n", command, r);
   return(r);
}
#endif

/*
// hook GetCfmValue()
int GetCfmValue(char *name, char *value)
{
   int r;
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   if(!real_GetCfmValue)
      real_GetCfmValue = dlsym(RTLD_NEXT, "GetCfmValue");
   r = real_GetCfmValue(name, value);
   printf("GetCfmValue('%s' -> %s) =%d\n", name, value, r);
   return(r);
}
*/

// This is the only key function that we have to fake
int get_flash_type()
{
#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
   printf("get_flash_type() = 4\n");
#endif
   return(4);
}

// bcm_nvram_get --> nvram_get
char *bcm_nvram_get(char *k)
{
   char *v = "";

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   v = nvram_get(k);

   if(v == 0)
      v = "";

#ifdef VERBOSE
   printf("bcm_nvram_get('%s') = '%s'\n", k, v);
#endif
   return(v);
}

// bcm_nvram_set --> nvram_set
int bcm_nvram_set(char *k, char *v)
{
   int i;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   i = nvram_set(k, v);

#ifdef VERBOSE
   printf("[nvram %d] bcm_nvram_set('%s', '%s')\n", i, k, v);
#endif
   return(0);
}

// bcm_nvram_unset --> nvram_unset
int bcm_nvram_unset(char *k)
{
   int i;

#ifdef VERBOSE
   printf("[0x%08x] ", __builtin_return_address(0));  // get caller's address
#endif

   i = nvram_unset(k);

#ifdef VERBOSE
   printf("[nvram %d] bcm_nvram_unset('%s')\n", i, k);
#endif
   return(i);
}
