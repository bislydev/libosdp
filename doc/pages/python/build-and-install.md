---
lastUpdated: 2026-10-25
description: "Install the LibOSDP 3.2 Python bindings — pre-built PyPI wheels, installing from GitHub, and building from a source checkout."
---
# Python Build and Install

## From PyPI (recommended)

```sh
pip install libosdp
```

Wheels are published for CPython 3.10 to 3.14 on Linux (x86_64 and aarch64,
glibc and musl), macOS (x86_64, arm64 and universal2), and Windows (amd64 and
arm64). Python 3.10 is the oldest supported version.

## From GitHub

To install the tip of the 3.2 maintenance branch instead of a release:

```sh
pip install "git+https://github.com/osdp-dev/libosdp@release_v3.2#egg=libosdp&subdirectory=python"
```

This builds from source, so it needs a C compiler.

## From a source checkout

```sh
git clone --branch release_v3.2 --recurse-submodules https://github.com/osdp-dev/libosdp
cd libosdp/python
pip install .
```

The build compiles the C library together with the `osdp_sys` extension
module, so a C toolchain is required: `gcc` or `clang` on Linux and macOS, the
Visual Studio Build Tools on Windows.

### Build-time options

The package is built with packet tracing (`OPT_OSDP_PACKET_TRACE`) and with
app-owned command and event queues (`OPT_OSDP_APP_OWNED_QUEUE_DATA`), which is
what makes the [completion handlers](/python/control-panel#command-completion)
available. The binding keeps its own copy of every submitted command or event,
so Python code never manages their lifetime.

To build without app-owned queues, set the environment variable before
building:

```sh
OPT_OSDP_APP_OWNED_QUEUE_DATA=off pip install .
```

In such a build the completion handlers are accepted but never called.

## Verify the install

```sh
python3 -c "import osdp, osdp_sys; print(osdp.ControlPanel, osdp.Command.LED)"
```
