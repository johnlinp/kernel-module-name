# Kernel Module Name

This is a small tool to get the kernel module name of a `*.ko` file.

Inspired by [this Stack Overflow question](https://stackoverflow.com/questions/44277243/how-to-get-default-kernel-module-name-from-ko-files).


## Build

```
$ make
```

## Use

```
$ ./get-64 /lib/modules/$(uname -r)/kernel/crypto/cryptd.ko
cryptd
$ cp /lib/modules/$(uname -r)/kernel/crypto/cryptd.ko /tmp/foo.ko
$ ./get-64 /tmp/foo.ko
cryptd
```
