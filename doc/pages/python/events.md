---
lastUpdated: 2026-10-25
description: "LibOSDP 3.2 Python events reference — the dict layout of every event a peripheral device reports to a control panel (card read, key press, status, manufacturer reply) and of CP notifications."
---
# Python Events

In 3.2 an event is a `dict`. The `'event'` key holds an `Event` constant that
selects the type, and the other keys carry its fields. A PD passes the dict to
[`submit_event`](/python/peripheral-device#reporting-events); a CP receives the
same layout from `get_event` or its
[event handler](/python/control-panel#events).

```python
from osdp import Event, CardFormat

pd.submit_event({
    'event': Event.CardRead,
    'reader_no': 0,
    'format': CardFormat.Wiegand,
    'direction': 0,
    'length': 26,                            # bits
    'data': bytes([0x5a, 0x3c, 0x81, 0x40]),
})
```

::: info
Every key listed for an event is required when submitting it. The typed
`osdp.events` classes shown in the docs for the latest development version are
not available in 3.2.
:::

## Card and keypad

### CardRead

`Event.CardRead` — a credential was presented.

| Key | Type | Meaning |
|---|---|---|
| `reader_no` | int | Reader number, 0 for the first. |
| `format` | int | A `CardFormat`: `Unspecified` or `Wiegand`. |
| `direction` | int | Read direction: 0 forward, 1 reverse. |
| `length` | int | Number of **bits** in `data`. |
| `data` | bytes | The card bits, packed into `(length + 7) // 8` bytes; at most 64 bytes. |

`CardFormat.ASCII` is still defined, but ASCII card reads are not supported:
the `osdp_FMT` reply they would need is deprecated by the OSDP specification,
so a PD does not send them. Send card data in a raw format.

### KeyPress

`Event.KeyPress` — keys were pressed on a reader's keypad.

| Key | Type | Meaning |
|---|---|---|
| `reader_no` | int | Reader number. |
| `data` | bytes | The key codes, at most 64. |

## Status and manufacturer

### Status

`Event.Status` — a status report, sent on its own or in answer to a
[Status command](/python/commands#status).

| Key | Type | Meaning |
|---|---|---|
| `type` | int | A `StatusReportType`: `Local`, `Input`, `Output` or `Remote`. |
| `report` | bytes | One byte per entry, at most 64. |

For `Input` the report must have exactly as many entries as the `num_items`
declared for `Capability.ContactStatusMonitoring`, and for `Output` as many as
declared for `Capability.OutputControl`; a report of any other length is
dropped when the PD comes to send it.

### Manufacturer

`Event.ManufacturerReply` — vendor-specific data from the PD, also how a PD
answers a [Manufacturer command](/python/commands#manufacturer) with data.

| Key | Type | Meaning |
|---|---|---|
| `vendor_code` | int | The vendor's 3-byte OUI, as an integer. |
| `data` | bytes | Payload, at most 128 bytes. |

## Notification

`Event.Notification` — not sent by a PD: LibOSDP raises it on the CP itself
when that PD's `LibFlag.EnableNotification` is set, to report what happened on
the link.

| Key | Type | Meaning |
|---|---|---|
| `type` | int | An `EventNotification`, see below. |
| `arg0`, `arg1` | int | Depend on `type`. |

| `type` | `arg0` | `arg1` |
|---|---|---|
| `EventNotification.Command` | The command's `Command` value | The command's outcome |
| `EventNotification.SecureChannelStatus` | 1 active, 0 inactive | 0 with the SCBK, 1 with the default key |
| `EventNotification.PeripheralDeviceStatus` | 1 online, 0 offline | — |

To tell whether a command succeeded, use the `CompletionStatus` passed to the
[command completion handler](/python/control-panel#command-completion).

## See also

- [Commands](/python/commands) for what the CP sends.
- [Event Structure](/api/event-structure), the C structures these dicts map to.
