---
lastUpdated: 2026-10-25
description: "Get started with the LibOSDP 3.2 Python bindings — install the package and build a minimal OSDP control panel or peripheral device in a few lines of Python."
---
# Python Getting Started

The `libosdp` Python package wraps the LibOSDP C library so you can build and
test OSDP devices without writing any C. In the 3.2 releases, commands and
events are plain `dict`s whose keys are listed on the
[Commands](/python/commands) and [Events](/python/events) pages, and the
constants they use come from `osdp` (defined in `osdp.constants`).

::: info Which API is this?
These pages describe the 3.2 releases published on PyPI. The `osdp.commands` and
`osdp.events` modules with typed dataclasses, shown in the docs for the latest
development version, are not part of any 3.2 release.
:::

## Install

Pre-built wheels for CPython 3.10 to 3.14 are published on PyPI for Linux,
macOS, and Windows:

```sh
pip install libosdp
```

If no wheel matches your platform, pip builds from source, which needs a C
compiler — see [Build and Install](/python/build-and-install).

## A channel to talk over

LibOSDP does not own the transport. You implement a [`Channel`](/python/api#channel)
that moves bytes over your wire (serial, TCP, etc.) and hand it to the device.
A `pyserial`-backed channel looks like this:

```python
import serial
from osdp import Channel

class SerialChannel(Channel):
    def __init__(self, device: str, speed: int):
        self.dev = serial.Serial(device, speed, timeout=0)

    def read(self, max_read: int):
        return self.dev.read(max_read)

    def write(self, data: bytes):
        return self.dev.write(data)

    def flush(self):
        self.dev.flush()
```

## Control Panel mode

```python
from osdp import ControlPanel, PDInfo, KeyStore, Command, CommandLEDColor, LogLevel

channel = SerialChannel("/dev/ttyUSB0", 115200)
pd_info = [PDInfo(101, channel, scbk=KeyStore.gen_key())]

cp = ControlPanel(pd_info, log_level=LogLevel.Info)
cp.start()
cp.sc_wait_all()

led_cmd = {
    'command': Command.LED,
    'reader': 0,
    'led_number': 0,
    'control_code': 2,      # temporary state: set it and start the timer
    'on_count': 10,
    'off_count': 10,
    'on_color': CommandLEDColor.Red,
    'off_color': CommandLEDColor.Black,
    'timer_count': 10,
    'temporary': True,
}
cp.submit_command(101, led_cmd)

event = cp.get_event(101, timeout=2)
if event:
    print(f"CP: received event {event}")

cp.teardown()
```

Commands are addressed by the PD's address (`101` here), not by its position
in the list.

::: warning A NAK takes the PD offline
In 3.2 a control panel treats every NAK from an online PD as a link failure: it
drops the secure channel and takes the PD offline for five minutes. A PD NAKs a
command it has not declared the capability for, and any command its handler
refuses — so declare every capability you use, and keep NAKs for real faults.
:::

## Peripheral Device mode

```python
from osdp import PeripheralDevice, PDInfo, PDCapabilities, Capability, Event, CardFormat

channel = SerialChannel("/dev/ttyUSB0", 115200)
pd_info = PDInfo(101, channel, scbk=None)   # no SCBK: start in install mode
pd_cap = PDCapabilities([
    (Capability.LEDControl, 2, 1),
    (Capability.AudibleControl, 1, 1),
])

pd = PeripheralDevice(pd_info, pd_cap)
pd.start()
pd.sc_wait()

pd.submit_event({                           # a 26-bit Wiegand credential
    'event': Event.CardRead,
    'reader_no': 0,
    'direction': 0,
    'format': CardFormat.Wiegand,
    'length': 26,
    'data': bytes([0x5a, 0x3c, 0x81, 0x40]),
})

cmd = pd.get_command(timeout=5)
if cmd:
    print(f"PD: received command {cmd}")

pd.teardown()
```

## Full examples

Runnable CP and PD applications for 3.2 live in the repository at
[`examples/python`](https://github.com/osdp-dev/libosdp/tree/release_v3.2/examples/python).

## Where to next

- [Control Panel](/python/control-panel) and [Peripheral Device](/python/peripheral-device)
  for each class's methods and handlers.
- [Commands](/python/commands) and [Events](/python/events) for every dict key.
- [API Reference](/python/api) for `PDInfo`, capabilities, `Channel`,
  `KeyStore`, file transfer, and the constants.
