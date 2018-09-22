#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
//#include <dlfcn.h>
//
// ../buildroot-2016.11.2/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc -shared -fPIC -o nvram.so nvram.c 

#define NVRAM_FILE      "/tmp/nvram.ini"
#define NVRAM_ENTRIES   2000
#define NVRAM_KEYLEN    128
#define NVRAM_LINE      256

static int counter = 0;
static int nvram_init = 0;
static int nvram_entries = 0;

// inefficient and crude key value pair array
static char key[NVRAM_ENTRIES][NVRAM_KEYLEN];
static char value[NVRAM_ENTRIES][NVRAM_KEYLEN];

static int custom_nvram_init();
int read_nvram();

int main(int argc, char *argv[])
{
   custom_nvram_init();
}

// function will be called only once when any of the acosNvram_* functions get
// invoked
static int custom_nvram_init()
{
   nvram_init = 1;
   printf("custom_nvram initialised\n");
   nvram_entries = read_nvram();
   printf("Read %d lines from %s\n", nvram_entries, NVRAM_FILE);
}

// function to read the nvram.ini file into
// a global array
int read_nvram()
{
   int i = 0;
   FILE *fp;
   char line[NVRAM_LINE], *k, *v;

   fp = fopen(NVRAM_FILE, "r");
   if(fp == (FILE *) NULL) {
      printf("Cannot open %s\n", NVRAM_FILE);
      exit(-1);
   }

   while(!feof(fp)) {
      fgets(line, NVRAM_LINE, fp);
      k = strtok(line, "=");
      v = strtok(NULL, "\n");
      memset(key[i], '\0', NVRAM_KEYLEN);
      memset(value[i], '\0', NVRAM_KEYLEN);
      if(k != NULL)
         strncpy(key[i], k, NVRAM_KEYLEN - 1);
      if(v != NULL)
         strncpy(value[i], v, NVRAM_KEYLEN - 1);

      printf("[%d] %s = %s\n", i, key[i], value[i]);
      i++;

      if(i >= NVRAM_ENTRIES) {
         printf("** WARNING: nvram entries exceeds %d\n", NVRAM_ENTRIES);
         break;
      }
   }

   fclose(fp);
   return(i);
}

