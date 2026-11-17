---
lastUpdated: 2026-10-25
description: "LibOSDP 3.2 Python API reference — PDInfo, the PD capabilities model, the Channel transport, KeyStore, file transfer callbacks, and the constants in osdp.constants."
---
# Python API Reference

Everything below is importable from `osdp`.

## Describing a PD

### PDInfo

```python
PDInfo(address, channel, scbk=None, name=None, flags=[], id=None)
```

| Argument | Meaning |
|---|---|
| `address` | The PD's OSDP address, 0–126. |
| `channel` | The [`Channel`](#channel) to talk over. |
| `scbk` | 16-byte secure channel base key. On a PD, `None` starts it in install mode; on a CP, `None` runs that PD without a secure channel. |
| `name` | Name used in log messages; defaults to `PD-<address>`. |
| `flags` | A list of [`LibFlag`](#libflag) values. |
| `id` | A [`PdId`](#pdid); used only by a PD, to answer the CP's ID request. |

### PdId

```python
PdId(version, model, vendor_code, serial_number, firmware_version)
```

The identity a PD reports, and what `ControlPanel.get_pd_id` returns. A
`PDInfo` without an `id` reports placeholder values.

## The capabilities model

A PD declares what it can do with a list of
`(Capability, compliance_level, num_items)` tuples:

```python
pd_cap = PDCapabilities([
    (Capability.OutputControl, 1, 2),     # two outputs
    (Capability.LEDControl, 2, 1),
    (Capability.TextOutput, 1, 1),
])
```

A CP reads them back with `check_capability`. The function codes and what each
compliance level means are described in [PD Capabilities](/protocol/pd-capabilities).

## Channel

Subclass `osdp.Channel` to give LibOSDP a transport; see
[Getting Started](/python/getting-started#a-channel-to-talk-over) for a serial
example.

| Method | Contract |
|---|---|
| `read(max_bytes) -> bytes` | Return up to `max_bytes` that are available now, or `b''`; never block. |
| `write(buf) -> int` | Send as much of `buf` as possible and return how many bytes were sent. |
| `flush() -> None` | Flush the underlying stream if it supports that. |

## KeyStore

A small helper for SCBKs kept as hex files in a directory (a temporary one when
none is given).

| Method | Does |
|---|---|
| `KeyStore.gen_key(key_len=16)` | Return random key bytes (static method). |
| `new_key(name, key_len=16)` | Generate and remember a key under `name`. |
| `get_key(name)`, `update_key(name, key)` | Read or replace a remembered key. |
| `commit_key(name)` | Write a remembered key to its file. |
| `load_key(name, key_len=16)` | Read a key back from its file. |

`gen_key` uses Python's `random` module, which is not a cryptographic source:
generate production keys with `secrets.token_bytes(16)` instead.

## File transfer

Both ends of a [file transfer](/python/commands#file-transfer) register a dict
of callbacks with `register_file_ops`:

```python
fops = {
    'open':  lambda file_id, size: ...,   # -> int
    'read':  lambda size, offset: ...,    # -> bytes
    'write': lambda data, offset: ...,    # -> int
    'close': lambda file_id: ...,
}
```

| Callback | Contract |
|---|---|
| `open(file_id, size)` | Return the file's size: on a CP the size of the file to send, on a PD the incoming `size` it agrees to receive. Raise to refuse. |
| `read(size, offset)` | CP: return at most `size` bytes from `offset`. |
| `write(data, offset)` | PD: store `data` at `offset` and return how many bytes were written. |
| `close(file_id)` | The transfer ended. |

## Constants

These are defined in `osdp.constants` and re-exported by `osdp`.

### Capability

`ContactStatusMonitoring`, `OutputControl`, `CardDataFormat`, `LEDControl`,
`AudibleControl`, `TextOutput`, `TimeKeeping`, `CheckCharacter`,
`CommunicationSecurity`, `ReceiveBufferSize`, `CombinedMessageSize`,
`SmartCard`, `Readers`, `Biometrics`.

### LogLevel

`Emergency`, `Alert`, `Critical`, `Error`, `Warning`, `Notice`, `Info`,
`Debug` — most to least severe.

### LibFlag

| Flag | Effect |
|---|---|
| `EnforceSecure` | Refuse the default key and fail rather than run without a secure channel. Recommended in production. |
| `InstallMode` | PD: allow one secure channel session with the default key, so a CP can set a new SCBK. |
| `IgnoreUnsolicited` | CP: do not fail when the PD answers a poll with an unknown, unsolicited reply. |
| `EnableNotification` | CP: report link changes as [notification events](/python/events#notification). |
| `CapturePackets` | Write the traffic to a `.pcap` file, finalized on `teardown()`. Set it in `PDInfo`; it cannot be changed later. |
| `AllowEmptyEncryptedDataBlock` | PD: accept secure-channel messages with an empty encrypted data block, which some non-conforming CPs send. |

All flags except `CapturePackets` can also be changed at runtime with
`ControlPanel.set_flag` and `clear_flag`.

### CompletionStatus

`Ok`, `Failed`, `Flushed`, `Aborted` — passed to the command and event
completion handlers.

### Command, Event and friends

| Constant | Values | Used in |
|---|---|---|
| `Command` | `Output`, `LED`, `Buzzer`, `Text`, `Comset`, `ComsetDone`, `Keyset`, `Manufacturer`, `FileTransfer`, `Status` | [Commands](/python/commands) |
| `CommandLEDColor` | `Black`, `Red`, `Green`, `Amber`, `Blue`, `Magenta`, `Cyan`, `White` | [LED](/python/commands#led) |
| `CommandFileTxFlags` | `Cancel` | [File Transfer](/python/commands#file-transfer) |
| `StatusReportType` | `Local`, `Input`, `Output`, `Remote` | [Status](/python/commands#status) |
| `Event` | `CardRead`, `KeyPress`, `ManufacturerReply`, `Status`, `Notification` | [Events](/python/events) |
| `CardFormat` | `Unspecified`, `Wiegand`, `ASCII` (dropped by a 3.2 PD) | [CardRead](/python/events#cardread) |
| `EventNotification` | `Command`, `SecureChannelStatus`, `PeripheralDeviceStatus` | [Notification](/python/events#notification) |

## See also

- [Getting Started](/python/getting-started) for complete CP and PD programs.
- [Miscellaneous C API](/api/miscellaneous) for the underlying file and status functions.
