custom_nvram.so
===============

Shared Library to intercept nvram get/set/match calls for emulating libnvram.so used by many IoT firmware software. I first wrote this library to intercept nvram calls made by software running on the Netgear Nighthawk R6250 and R6400 routers, for emulating them under QEMU.

Loosely based on Craig Heffner [@devttyS0](https://twitter.com/@devttyS0) and Zachary Cutlip [@zcutlip](https://twitter.com/zcutlip)'s work.

Compiling custom_nvram.so
-------------------------

To compile `custom_nvram.c` from `custom_nvram.c`, we need `buildroot`.

```
buildroot/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-gcc -shared -fPIC -o custom_nvram.so custom_nvram.c
```

Invocation (for the Netgear Nighthawk R6250 router)

```
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
     LD_PRELOAD=/tmp/custom_nvram.so
     /usr/sbin/httpd -S -E /usr/sbin/ca.pem /usr/sbin/httpsd.pem
```

`custom_nvram.so` will look for a hardcoded file `/tmp/nvram.ini` containing the configuration data.

References:

-	https://github.com/zcutlip/nvram-faker
-	http://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html
-	https://stackoverflow.com/questions/9759880/automatically-executed-functions-when-loading-shared-libraries
-	https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html
