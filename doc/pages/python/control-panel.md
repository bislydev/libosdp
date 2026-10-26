---
lastUpdated: 2026-10-25
description: "LibOSDP 3.2 Python control panel (CP) reference — the ControlPanel class, its lifecycle, event and command-completion handlers, sending commands, status, and file transfer."
---
# Python Control Panel

`osdp.ControlPanel` drives one or more PDs on a single channel. Each PD is
described by a [`PDInfo`](/python/api#pdinfo), and almost every method takes
the PD's **address** to say which one it means.

## Lifecycle

```python
cp = ControlPanel(pd_info_list, log_level=LogLevel.Info)
cp.start()          # spawn the refresh thread (calls osdp_cp_refresh every 20ms)
cp.sc_wait_all()    # optional: wait for secure channel on every PD
...
cp.teardown()       # stop the thread and release the context
```

`start()` must be called before any method that talks to the library
(`submit_command`, `status`, `get_pd_id`, ...): they share a lock with the
refresh thread that `start()` creates. `stop()` ends the thread without
releasing the context; `teardown()` does both.

## ControlPanel

```python
ControlPanel(pd_info_list, log_level=LogLevel.Info,
             event_handler=None, command_completion_handler=None)
```

| Argument | Meaning |
|---|---|
| `pd_info_list` | A list of [`PDInfo`](/python/api#pdinfo), one per PD on the bus. |
| `log_level` | A [`LogLevel`](/python/api#loglevel). The level is process-wide. |
| `event_handler` | Optional `handler(pd, event)`, see [Events](#events). |
| `command_completion_handler` | Optional `handler(address, command, status)`, see [Command completion](#command-completion). |

## Events

Every event a PD reports is queued per PD. Poll it with:

```python
event = cp.get_event(address, timeout=5)   # dict, or None on timeout
```

A negative `timeout` waits forever. To react as events arrive instead, pass
`event_handler` (or call `set_event_handler`). It is called from the refresh
thread as `handler(pd, event)`; its return value is ignored, and the event is
queued for `get_event` either way. The `event` dict layouts are on the
[Events](/python/events) page.

## Command completion

```python
from osdp import CompletionStatus

def on_command_complete(address, command, status):
    if status != CompletionStatus.Ok:
        print(f"command to PD {address} ended with {status}: {command}")

cp.set_command_completion_handler(on_command_complete)
```

The handler runs once for every command that was accepted, with one of
`CompletionStatus.Ok`, `Failed`, `Flushed` (dropped by `flush_commands`) or
`Aborted` (dropped by `teardown`). It is available in the PyPI wheels; see
[Build and Install](/python/build-and-install#build-time-options) for builds
that leave it out.

## Commands

```python
cp.submit_command(address, command)   # True if queued, False otherwise
cp.flush_commands(address)            # drop queued commands; returns how many
```

`command` is a dict whose `'command'` key selects the type; the remaining keys
are listed on the [Commands](/python/commands) page. A dict with a missing or
mistyped key raises an exception instead of being sent. A `True` return only
means the command was queued — whether the PD accepted it is reported through
the completion handler or, with `LibFlag.EnableNotification`, a
[notification event](/python/events#notification).

::: warning A NAK takes the PD offline
When an online PD NAKs a command — because it lacks the capability, or its
handler refused it — the 3.2 control panel treats that as a link failure: the
completion status is `Failed`, the secure channel is dropped, and the PD stays
offline for five minutes before the CP tries again.
:::

`send_command` is the old name of `submit_command`; it still works but warns
with a `DeprecationWarning`.

## Status and secure channel

| Method | Returns |
|---|---|
| `is_online(address)` | `True` when the PD is online. |
| `get_num_online()` | Number of PDs online. |
| `status()` | Bitmask of online PDs, bit *n* for the PD at index *n*. |
| `is_sc_active(address)` | `True` when a secure channel is active with the PD. |
| `get_num_sc_active()` | Number of PDs with an active secure channel. |
| `sc_status()` | Bitmask of PDs with an active secure channel. |

Blocking helpers poll every 500ms and return `True` on success, `False` on
timeout: `online_wait(address, timeout=8)`, `online_wait_all(timeout=10)`,
`offline_wait(address, timeout=8)`, `sc_wait(address, timeout=5)` and
`sc_wait_all(timeout=5)`.

## Capabilities, IDs, and flags

```python
level, count = cp.check_capability(address, Capability.LEDControl)
pd_id = cp.get_pd_id(address)       # PdId: version, model, vendor_code, serial_number, firmware_version
cp.set_flag(address, LibFlag.EnableNotification)
cp.clear_flag(address, LibFlag.EnableNotification)
```

`check_capability` returns the `(compliance_level, num_items)` the PD reported
for a [`Capability`](/python/api#capability). Only some flags can be changed at
runtime; see [`LibFlag`](/python/api#libflag).

A PD can be taken off the polling loop without tearing the CP down:
`disable_pd(address)` and `enable_pd(address)` return `True` when the request
is accepted, and the change shows in `is_pd_enabled(address)` once the refresh
thread has applied it.

## File transfer

```python
cp.register_file_ops(address, fops)
cp.submit_command(address, {'command': Command.FileTransfer, 'id': 1, 'flags': 0})
cp.get_file_tx_status(address)      # {'size': ..., 'offset': ...} while a transfer runs
```

`fops` is the dict of callbacks described under
[File transfer](/python/api#file-transfer). To stop a transfer, submit the same
command with `'flags': CommandFileTxFlags.Cancel`.

## See also

- [Peripheral Device](/python/peripheral-device) for the other end of the link.
- [Control Panel C API](/api/control-panel), which these methods wrap.
