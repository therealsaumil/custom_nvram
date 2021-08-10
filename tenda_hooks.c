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
#include <sys/types.h>
#include <sys/stat.h>

int nvram_set(char *uParm1, char *pcParm2); // set
char *nvram_get(char *uParm1); // get
int nvram_unset(char *uParm1); // unset
//static int (*real_get_flash_type)() = NULL;

// function declarations
extern int print_caller_and_address();

/*
// hook GetCfmValue()
int GetCfmValue(char *name, char *value)
{
   int r;
   print_caller_and_address();
   if(!real_GetCfmValue)
      real_GetCfmValue = dlsym(RTLD_NEXT, "GetCfmValue");
   r = real_GetCfmValue(name, value);
   printf("GetCfmValue('%s' -> %s) =%d\n", name, value, r);
   return(r);
}
*/

// Hook just to inspect what get_flash_type() returns
/*
int get_flash_type()
{
   int r;

   if(!real_get_flash_type) {
      real_get_flash_type = dlsym(RTLD_NEXT, "get_flash_type");
   }
   r = real_get_flash_type();
#ifdef VERBOSE
   print_caller_and_address();
   printf("get_flash_type() = %d\n", r);
#endif
   return(r);
}
*/

// bcm_nvram_get --> nvram_get
char *bcm_nvram_get(char *k)
{
   char *v = "";

#ifdef VERBOSE
   print_caller_and_address();
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
   print_caller_and_address();
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
   print_caller_and_address();
#endif

   i = nvram_unset(k);

#ifdef VERBOSE
   printf("[nvram %d] bcm_nvram_unset('%s')\n", i, k);
#endif
   return(i);
}
