---
lastUpdated: 2026-10-25
description: "LibOSDP 3.2 Python peripheral device (PD) reference — the PeripheralDevice class, its lifecycle, the command-handler contract, reporting events, and install mode."
---
# Python Peripheral Device

`osdp.PeripheralDevice` implements a single PD: it answers the CP's polls,
hands you the commands the CP sends, and delivers the events you submit.

## Lifecycle

```python
pd = PeripheralDevice(pd_info, pd_cap, log_level=LogLevel.Info)
pd.start()          # spawn the refresh thread (calls osdp_pd_refresh every 20ms)
pd.sc_wait()        # optional: wait up to 8s for a secure channel
...
pd.teardown()       # stop the thread and release the context
```

As on the CP, `start()` must be called before `submit_event`,
`register_file_ops` or `get_file_tx_status`; `stop()` ends the thread without
releasing the context and `teardown()` does both.

## PeripheralDevice

```python
PeripheralDevice(pd_info, pd_cap, log_level=LogLevel.Info,
                 command_handler=None, event_completion_handler=None)
```

| Argument | Meaning |
|---|---|
| `pd_info` | A [`PDInfo`](/python/api#pdinfo): address, channel, SCBK and the identity reported to the CP. |
| `pd_cap` | A [`PDCapabilities`](/python/api#the-capabilities-model) listing what this PD can do. |
| `log_level` | A [`LogLevel`](/python/api#loglevel). |
| `command_handler` | Optional, see [The command-handler contract](#the-command-handler-contract). |
| `event_completion_handler` | Optional `handler(event, status)`, see [Reporting events](#reporting-events). |

## The command-handler contract

Every command from the CP is queued, and you can poll it:

```python
cmd = pd.get_command(timeout=5)    # dict, or None on timeout
```

To decide how the PD answers, pass `command_handler` (or call
`set_command_handler`). It is called from the refresh thread with the command
dict and returns a tuple:

```python
def on_command(cmd):
    if cmd['command'] == Command.LED:
        drive_led(cmd)
        return 0, None      # reply with osdp_ACK
    return -1, None         # reply with osdp_NAK
```

- `(0, None)` acknowledges the command.
- A negative first element NAKs it. An exception raised in the handler NAKs it
  too.
- Pass `None` as the second element. To answer a manufacturer-specific command
  with data, submit an [`Event.ManufacturerReply`](/python/events#manufacturer)
  event; it reaches the CP on its next poll.

::: warning A NAK takes the PD offline
A 3.2 control panel treats a NAK from an online PD as a link failure and takes
the PD offline for five minutes. LibOSDP also NAKs, without calling your
handler, any command whose capability the PD did not declare (`OutputControl`
for Output, `LEDControl` for LED, `AudibleControl` for Buzzer, `TextOutput` for
Text). Declare what you handle, and NAK only on a real fault.
:::

The command is queued for `get_command` whether or not a handler is set. Without
a handler every command is acknowledged. The command dict layouts are on the
[Commands](/python/commands) page.

## Reporting events

```python
pd.submit_event({'event': Event.KeyPress, 'reader_no': 0, 'data': b'1234#'})
```

`submit_event` returns `True` once the event is queued for the CP's next poll
and `False` if the queue is full; a dict that does not match its event type
raises `TypeError`. A queued event can still be dropped when it is sent — an
ASCII card read, or a status report of the wrong length; see
[Events](/python/events). `notify_event` is the old name and warns with a
`DeprecationWarning`.

With `event_completion_handler` (or `set_event_completion_handler`) you learn
what became of each queued event: `handler(event, status)`, where `status` is a
[`CompletionStatus`](/python/api#completionstatus) — `Flushed` and `Aborted`
mean it was dropped before reaching the CP.

## Install mode and secure channel

A `PDInfo` with `scbk=None` starts the PD in install mode: it accepts a secure
channel set up with the default key and lets the CP send a new SCBK with a
[Keyset](/python/commands#keyset) command. That command reaches your handler
like any other, so store the new key (for instance with a
[`KeyStore`](/python/api#keystore)) and pass it as `scbk` on the next start;
LibOSDP does not persist it. See [Secure Channel](/libosdp/secure-channel) for
the full model.

`is_online()` and `is_sc_active()` report the link state, and
`sc_wait(timeout=8)` blocks until a secure channel is up, returning `False` on
timeout.

## File transfer

`register_file_ops(fops)` installs the callbacks that receive a file the CP
sends with a [File Transfer](/python/commands#file-transfer) command, and
`get_file_tx_status()` returns `{'size': ..., 'offset': ...}` while one is in
progress. The `fops` dict is described under
[File transfer](/python/api#file-transfer).

## See also

- [Control Panel](/python/control-panel) for the other end of the link.
- [Peripheral Device C API](/api/peripheral-device), which these methods wrap.
