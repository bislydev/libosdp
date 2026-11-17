---
lastUpdated: 2026-10-24
description: "LibOSDP peripheral device (PD) C API — implement an OSDP PD, advertise capabilities, respond to control panel commands, and report events."
---
# Peripheral Device

The following functions are used when OSDP is used in PD mode. The library
returns a single opaque pointer of type `osdp_t` where it maintains all
internal data. All applications consuming this library must pass this context
pointer to all API calls.

## Device Lifecycle Management

The PD is described to `osdp_pd_setup` with an [`osdp_pd_info_t`](/api/pd-info)
structure.

<!-- doxygen:typedef:osdp_t -->

<!-- doxygen:function:osdp_pd_setup -->

<!-- doxygen:function:osdp_pd_refresh -->

<!-- doxygen:function:osdp_pd_teardown -->

## PD Capabilities

See [PD capabilities](/protocol/pd-capabilities) for the full list of function
codes and compliance levels a PD can advertise.

<!-- doxygen:struct:osdp_pd_cap:members -->

<!-- doxygen:enum:osdp_pd_cap_function_code_e -->

<!-- doxygen:function:osdp_pd_set_capabilities -->

## Commands

<!-- doxygen:typedef:pd_command_callback_t -->

<!-- doxygen:function:osdp_pd_set_command_callback -->

Refer to the [command structure](/api/command-structure) document for more
information on how the `cmd` structure is framed.

::: info
For `CMD_MFG`, the callback's return value picks the immediate reply: negative
→ NAK, zero → ACK, and positive → `REPLY_MFGREP` carrying the `mfg` payload the
callback wrote back into `cmd`. A manufacturer-specific reply can also be sent
later by submitting an `OSDP_EVENT_MFGREP` event with `osdp_pd_submit_event`.
:::

## Events

When a PD app has an event (card read, key press, etc.) to report to the CP,
it creates the corresponding event structure and calls `osdp_pd_submit_event`
to deliver it to the CP on the next `osdp_POLL` command.

<!-- doxygen:function:osdp_pd_submit_event -->

By default LibOSDP copies an accepted event into its queue, so the `event` you
pass is yours again as soon as the call returns.

<!-- doxygen:function:osdp_pd_flush_events -->

### App-owned Event Queue

A build configured with `--app-owned-queue-data` (CMake option
`OPT_OSDP_APP_OWNED_QUEUE_DATA`) queues events **by reference** instead of
copying them. The `event` must then stay alive and unmodified until its
completion callback fires; register that callback with
`osdp_pd_set_event_completion_callback`, which only exists in such a build. The
outcome is reported as an
[`osdp_completion_status`](/api/control-panel#app-owned-command-queue).

<!-- doxygen:typedef:pd_event_completion_callback_t -->

Refer to the [event structure](/api/event-structure) document for more
information on how to populate the `event` structure.

### Deprecated

`osdp_pd_notify_event` is retained for backward compatibility; new code should
use `osdp_pd_submit_event` instead.

<!-- doxygen:function:osdp_pd_notify_event -->
