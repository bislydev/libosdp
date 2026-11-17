---
lastUpdated: 2026-10-25
description: "LibOSDP 3.2 Python commands reference — the dict layout of every command a control panel sends to a PD (output, LED, buzzer, text, comset, keyset, manufacturer, file transfer, status)."
---
# Python Commands

In 3.2 a command is a `dict`. The `'command'` key holds a `Command` constant
that selects the type, and the other keys carry its fields. A CP passes the
dict to [`submit_command`](/python/control-panel#commands); a PD receives the
same layout in its [command handler](/python/peripheral-device#the-command-handler-contract)
or from `get_command`.

```python
from osdp import Command, CommandLEDColor

cp.submit_command(101, {
    'command': Command.Buzzer,
    'reader': 0,
    'control_code': 2,
    'on_count': 5,
    'off_count': 5,
    'rep_count': 3,
})
```

The PD must declare the capability a command needs — `OutputControl` for
Output, `LEDControl` for LED, `AudibleControl` for Buzzer, `TextOutput` for
Text — or it NAKs the command, which in 3.2 takes it offline; see
[Control Panel](/python/control-panel#commands).

::: info
Every key listed for a command is required when sending it, unless marked
optional; a missing one raises an exception before anything reaches the bus.
Durations are in units of 100 ms unless stated otherwise. The typed
`osdp.commands` classes shown in the docs for the latest development version
are not available in 3.2.
:::

## Reader output

### Output

`Command.Output` — drive a PD output (relay, lock, etc.).

| Key | Type | Meaning |
|---|---|---|
| `output_no` | int | Output number, 0 for the first. |
| `control_code` | int | 0 no change; 1 / 2 permanent OFF / ON and abort any timed state; 3 / 4 permanent OFF / ON after the timed state completes; 5 / 6 temporary ON / OFF for `timer_count`. |
| `timer_count` | int | Duration of a temporary state. |

### LED

`Command.LED` — set a reader LED, either permanently or for a while.

| Key | Type | Meaning |
|---|---|---|
| `reader` | int | Reader number, 0 for the first. |
| `led_number` | int | LED number on that reader. |
| `temporary` | bool | Optional. `True` to set the temporary state, `False` (the default) for the permanent one. |
| `control_code` | int | Permanent: 0 no change, 1 set. Temporary: 0 no change, 1 cancel and show the permanent state, 2 set and start the timer. |
| `on_color`, `off_color` | int | A `CommandLEDColor`: `Black`, `Red`, `Green`, `Amber`, `Blue`, `Magenta`, `Cyan`, `White`. |
| `on_count`, `off_count` | int | Flash ON and OFF durations. |
| `timer_count` | int | Temporary only: how long the temporary state lasts. |

A PD can receive one more key, `cancel_temporary: True`, when the CP cancels a
temporary state and sets the permanent one in the same command.

### Buzzer

`Command.Buzzer` — sound a reader's buzzer.

| Key | Type | Meaning |
|---|---|---|
| `reader` | int | Reader number. |
| `control_code` | int | 0 no tone, 1 off, 2 default tone. |
| `on_count`, `off_count` | int | Tone ON and OFF durations. |
| `rep_count` | int | Number of ON/OFF cycles; 0 repeats forever. |

### Text

`Command.Text` — show text on a reader's display.

| Key | Type | Meaning |
|---|---|---|
| `reader` | int | Reader number. |
| `control_code` | int | 1 / 2 permanent text without / with wrap; 3 / 4 temporary text without / with wrap. |
| `temp_time` | int | Seconds to show temporary text. |
| `offset_row`, `offset_col` | int | Position of the first character, 1-indexed. |
| `data` | str | The text, at most 32 characters. |

## Configuration

### Comset

`Command.Comset` — change the PD's address and baud rate.

| Key | Type | Meaning |
|---|---|---|
| `address` | int | Address the PD answers on after the change. |
| `baud_rate` | int | One of 9600, 19200, 38400, 115200, 230400. |

A PD's handler sees this command twice: first as `Command.Comset`, which it
may NAK to refuse, and then, once the reply has gone out and the new settings
are about to apply, as `Command.ComsetDone` with the same keys. Persist the new
address and baud rate on `ComsetDone`.

### Keyset

`Command.Keyset` — set a new secure channel base key (SCBK) on the PD.

| Key | Type | Meaning |
|---|---|---|
| `type` | int | Key type; 1 is the SCBK. |
| `data` | bytes | The key, 16 bytes for an SCBK (at most 32). |

A CP only sends a Keyset while a secure channel with the PD is active.

The PD must store the key itself; see
[Install mode and secure channel](/python/peripheral-device#install-mode-and-secure-channel).

## Data and status

### Manufacturer

`Command.Manufacturer` — a vendor-specific command.

| Key | Type | Meaning |
|---|---|---|
| `vendor_code` | int | The vendor's 3-byte OUI, as an integer. |
| `data` | bytes | Payload, at most 64 bytes. |

A PD answers data back with a [Manufacturer reply event](/python/events#manufacturer).

### File Transfer

`Command.FileTransfer` — send a file to the PD. Register file callbacks on both
sides first; see [File transfer](/python/api#file-transfer).

| Key | Type | Meaning |
|---|---|---|
| `id` | int | File ID agreed between CP and PD. |
| `flags` | int | 0 to start; `CommandFileTxFlags.Cancel` to stop a transfer in progress. |

### Status

`Command.Status` — ask the PD for a status report, which arrives as a
[Status event](/python/events#status).

| Key | Type | Meaning |
|---|---|---|
| `type` | int | A `StatusReportType`: `Local`, `Input`, `Output` or `Remote`. |

## See also

- [Events](/python/events) for what the PD reports back.
- [Command Structure](/api/command-structure), the C structures these dicts map to.
