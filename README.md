# custom_nvram - nvram hooks for devices emulated by [ARMX][armx]

ARMX's `libnvram-armx.so` uses shared memory to store `nvram` data and extend
nvram support to emulated devices. Some of the key functions exported by this
library are:

```
char *nvram_get(char *uParm1);
int nvram_set(char *uParm1, char *pcParm2);
int nvram_unset(char *uParm1);
int nvram_match(char *uParm1, char *pcParm2);
int nvram_invmatch(char *uParm1, char *pcParm2);
char *nvram_get_ex2(char *uParm1, char *pcParm2, int sParm3);
```

Devices emulated in ARMX that need `nvram` support can `LD_PRELOAD` this
library to provide wrappers for basic `nvram` manipulation functionality.

## nighthawk_hooks.so - Netgear Nighthawk R6250 and R6400

nvram functions present in in Netgear's libnvram.so and libacos_shared.so:

```
int acosNvramConfig_match(char *k, char *v)
void acosNvramConfig_read(char *k, char *r, int len)
int acosNvramConfig_set(char *k, char *v)
char *acosNvramConfig_get(char *k)
char *acosNvramConfig_exist(char *k)
char *acosNvramConfig_invmatch(undefined4 uParm1,char *pcParm2)
acosNvramConfig_save(void)
int acosNvramConfig_readAsInt(char *k, int *r)
```

Function mapping:

```
acosNvramConfig_get      - nvram_get
acosNvramConfig_set      - nvram_set
acosNvramConfig_unset    - nvram_unset
acosNvramConfig_match    - nvram_match
acosNvramConfig_read     - nvram_get_ex2
acosNvramConfig_exist    - nvram_get
acosNvramConfig_invmatch - nvram_invmatch
acosNvramConfig_save     - nvram_commit
```

Compile:

```
CC="<buildroot>/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc"
FLAGS="-shared -fpie -ldl"
$CC $FLAGS -o nighthawk_hooks.so nighthawk_hooks.c
$CC $FLAGS -DVERBOSE -o nighthawk_hooks_verbose.so nighthawk_hooks.c
```

## tenda_hooks.so - Tenda AC15 (and other AC series routers)

nvram functions present in in Tenda's libCfm.so:

```
int bcm_nvram_unset(char *name)
int bcm_nvram_set(char *name,char *value)
char * bcm_nvram_get(char *name)
int bcm_nvram_commit(void)
```

Function mapping:

```
bcm_nvram_get    - nvram_get
bcm_nvram_set    - nvram_set
bcm_nvram_unset  - nvram_unset
bcm_nvram_commit - didn't hook :)
```

Compile:

```
CC="<buildroot>/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc"
FLAGS="-shared -fpie -ldl"
$CC $FLAGS -o tenda_hooks.so tenda_hooks.c
$CC $FLAGS -DVERBOSE -o tenda_hooks_verbose.so tenda_hooks.c
```


# custom_nvram.so [OLD - superceded by [ARMX][armx]'s approach]

Note: These files are not maintained actively.
Shared Library to intercept nvram get/set/match calls for emulating libnvram.so
used by many IoT firmware software. I first wrote this library to intercept
nvram calls made by software running on the Netgear Nighthawk R6250 and R6400
routers, for emulating them under QEMU.

Loosely based on Craig Heffner [@devttyS0](https://twitter.com/@devttyS0) and
Zachary Cutlip [@zcutlip](https://twitter.com/zcutlip)'s work.

[armx]: https://github.com/therealsaumil/armx/

## custom_nvram.so for Netgear R6250 Nighthawk

To compile `custom_nvram_r6250.so` from `custom_nvram_r6250.c`, we need `buildroot`.

```
buildroot/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc -shared -fPIC -o custom_nvram_r6250.so custom_nvram_r6250.c
```

Invocation

```
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
     LD_PRELOAD=/tmp/custom_nvram_r6250.so
     /usr/sbin/httpd -S -E /usr/sbin/ca.pem /usr/sbin/httpsd.pem
```

`custom_nvram_r6250.so` will look for a hardcoded file `/tmp/nvram.ini`
containing the configuration data.

References:

-	https://github.com/zcutlip/nvram-faker
-	http://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html
-	https://stackoverflow.com/questions/9759880/automatically-executed-functions-when-loading-shared-libraries
-	https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html
