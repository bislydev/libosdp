---
lastUpdated: 2026-10-24
description: "Cross-compile LibOSDP for embedded targets with a CMake toolchain file or the configure.sh/make build."
---
# Cross Compiling

LibOSDP is written in C and does not depend on any other libraries. You can
compile it for pretty much any platform (even Windows). Follow the cross
compilation best practice for your platform. This document gives you some ideas
on how this can be done but is in no way conclusive.

## Using CMake

LibOSDP can be compiled with your cross compiler by passing a toolchain file to
cmake. This can be done by invoking cmake with the command line argument
`-DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain-file.cmake`.

If your toolchain is installed in `/opt/toolchain/armv8l-linux-gnueabihf/` and
the sysroot is present in `/opt/toolchain/armv8l-linux-gnueabihf/sysroot`, the
`toolchain-file.cmake` file should look like this:

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# specify the cross compiler and sysroot
set(TOOLCHAIN_INST_PATH /opt/toolchain/armv8l-linux-gnueabihf)
set(CMAKE_C_COMPILER    ${TOOLCHAIN_INST_PATH}/bin/armv8l-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_INST_PATH}/bin/armv8l-linux-gnueabihf-g++)
set(CMAKE_SYSROOT       ${TOOLCHAIN_INST_PATH}/sysroot)

# don't search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search for libraries and headers in the target directories only
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
```

For convenience, the `toolchain-file.cmake` file can be placed in a common path
(probably where the toolchain is installed) and referenced from your build
directory.

```sh
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/toolchain/armv8l-linux-gnueabihf/toolchain-file.cmake ..
make
```

## Using Make

You could use the `--cross-compile` flag in `configure.sh` and then invoke make
to build the library.

```sh
./configure.sh --cross-compile arm-none-
make
make DESTDIR=/opt/arm-none-sysroot/ install
```
