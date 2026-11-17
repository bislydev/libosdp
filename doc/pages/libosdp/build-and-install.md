---
lastUpdated: 2026-10-24
description: "Build and install LibOSDP from source with CMake or the configure.sh/make build, pick compile-time options, and consume it from your own CMake project."
---
# Build and Install

To build LibOSDP you must have cmake-3.0 (or above) and a C compiler installed.
This repository produces a `libosdpstatic.a` and `libosdp.so`. You can link
these with your application as needed (`-losdp` or `-losdpstatic`). Have a look
at `examples/` for details on how to consume this library.

This project supports two kinds of build systems: cmake and make.

## CMake Build

CMake is the recommended way of building LibOSDP.

```sh
cmake -B build
cmake --build build --parallel
```

Optionally, you could install LibOSDP using,

```sh
cmake --build build --target install
```

## Make Build

The make build is provided as a reference for people who wish to write their
own Makefile to be chained in some other build system. Please see
`configure.sh --help` for different configuration options.

```sh
./configure.sh
make
```

## Running the Test Suite

If you are adding features to LibOSDP, you might want to kick off the test
suite to ensure that your changes are valid and have not regressed any other
features. To do this run,

```sh
./scripts/run_tests.sh
```

## Compile-time Configuration Options

LibOSDP can be configured to enable/disable certain features by passing the
`-DOPT_XXX=ON/OFF` flag to cmake. The following table lists all such config
switches. For instance, to build with packet tracing, pass
`-DOPT_OSDP_PACKET_TRACE=ON` to cmake.

| configure.sh flag        | CMake option                    | Default | Description                                         |
|--------------------------|---------------------------------|---------|-----------------------------------------------------|
| `--packet-trace`         | `OPT_OSDP_PACKET_TRACE`         | OFF     | Enable raw packet trace for diagnostics             |
| `--data-trace`           | `OPT_OSDP_DATA_TRACE`           | OFF     | Enable command/reply data buffer tracing            |
| `--skip-mark`            | `OPT_OSDP_SKIP_MARK_BYTE`       | OFF     | Don't send the leading mark byte (0xFF)             |
| `--zero-copy`            | `OPT_OSDP_RX_ZERO_COPY`         | OFF     | Zero-copy RX buffers (needs `recv_pkt`/`release_pkt`) |
| `--app-owned-queue-data` | `OPT_OSDP_APP_OWNED_QUEUE_DATA` | OFF     | Queue app-owned command/event payload references    |
| `--no-colours`           | `OPT_DISABLE_PRETTY_LOGGING`    | OFF     | Don't colourize log outputs                         |
| `--static-pd`            | `OPT_OSDP_STATIC_PD`            | OFF     | Set up a single PD statically                       |
| `--lib-only`             | `OPT_OSDP_LIB_ONLY`             | OFF     | Only build the library                              |
| N/A                      | `OPT_BUILD_SANITIZER`           | OFF     | Enable different sanitizers during build            |
| N/A                      | `OPT_BUILD_SHARED`              | ON      | Build shared library                                |
| N/A                      | `OPT_BUILD_STATIC`              | ON      | Build static library                                |
| N/A                      | `OPT_BUILD_BARE_METAL`          | OFF     | Build library for bare metal targets                |

When `OPT_OSDP_APP_OWNED_QUEUE_DATA` is enabled, LibOSDP keeps only references
to submitted command/event payloads in its internal queues. In this mode the
internal slab-based command/event payload storage is not built/used, and the
application must keep submitted objects valid until their completion callbacks
fire.

## Add LibOSDP to Your CMake Project

### find_package

Build and install LibOSDP to some standard location such as `/usr/local/` and
then use cmake's `find_package()` method.

### External Project

Start by adding the following to your `CMakeLists.txt`:

```cmake
include(ExternalProject)
ExternalProject_Add(ext_libosdp
    GIT_REPOSITORY    https://github.com/osdp-dev/libosdp.git
    GIT_TAG           v3.2.7 # update this to the latest v3.2 release
    SOURCE_DIR        ${CMAKE_BINARY_DIR}/libosdp/src
    BINARY_DIR        ${CMAKE_BINARY_DIR}/libosdp/build
    CONFIGURE_COMMAND cmake ${CMAKE_BINARY_DIR}/libosdp/src
    BUILD_COMMAND     make
    INSTALL_COMMAND   make install DESTDIR=${CMAKE_BINARY_DIR}/libosdp/install
)
include_directories("${CMAKE_BINARY_DIR}/libosdp/install/usr/local/include")
link_directories("${CMAKE_BINARY_DIR}/libosdp/install/usr/local/lib")
```

Next, you must add `ext_libosdp` as a dependency to your target. That's it! Now
you can link your application to the osdp library. The following example shows
how you can do this.

```cmake
set(OSDP_APP osdp-app)
list(APPEND OSDP_APP_SRC
    "src/main.c"
    "src/more_source_files.c"
    ...
)
add_executable(${OSDP_APP} ${OSDP_APP_SRC})
add_dependencies(${OSDP_APP} ext_libosdp)
target_link_libraries(${OSDP_APP} osdp)
```

### Using pkg-config

If you are familiar with pkg-config based dependency resolution methods, LibOSDP
provides a `libosdp.pc` file which is installed along with the library.

### Using vcpkg

vcpkg is a free and open-source C/C++ package manager maintained by Microsoft
and the C++ community. A port for LibOSDP has been merged upstream, so you can
consume LibOSDP directly from vcpkg and use all the generators it supports.

Follow the
[getting started document](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started)
from Microsoft to set up vcpkg. After that, create a new application and pull in
LibOSDP as a dependency.

```shell
mkdir osdp_app && cd osdp_app
vcpkg new --application
vcpkg add port libosdp
```

After that, you can add your app sources, find the libosdp package and link it
to your target with,

```cmake
find_package(LibOSDP CONFIG REQUIRED)
target_link_libraries(
    main
    PRIVATE $<IF:$<TARGET_EXISTS:libosdp::osdp>,libosdp::osdp,libosdp::osdpstatic>
)
```

To build the project, you must set the `CMAKE_TOOLCHAIN_FILE` to the one
provided by vcpkg for the dependencies to be pulled in correctly.

```shell
cmake -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -B build .
cmake --build build
```

::: tip
vcpkg is the recommended way to consume LibOSDP on Windows.
:::
