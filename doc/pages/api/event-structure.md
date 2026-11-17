---
lastUpdated: 2026-10-24
description: "LibOSDP event structure reference — the osdp_event data structures and enums a peripheral device uses to report card reads, key presses, status, and notifications to the CP."
---
# Application Events

LibOSDP exposes the following structures through `osdp.h`. All events are
carried in a single union wrapper:

<!-- doxygen:struct:osdp_event:members -->

The `type` field selects the active union member and is one of `osdp_event_type`:

<!-- doxygen:enum:osdp_event_type -->

Below are the structures for each event type. They are delivered to the CP with
[`osdp_pd_submit_event`](/api/peripheral-device).

## Key Press Event

<!-- doxygen:struct:osdp_event_keypress:members -->

<!-- doxygen:define:OSDP_EVENT_KEYPRESS_MAX_DATALEN -->

## Card Read Event

<!-- doxygen:struct:osdp_event_cardread:members -->

<!-- doxygen:enum:osdp_event_cardread_format_e -->

<!-- doxygen:define:OSDP_EVENT_CARDREAD_MAX_DATALEN -->

## Manufacturer Specific Reply Event

<!-- doxygen:struct:osdp_event_mfgrep:members -->

<!-- doxygen:define:OSDP_EVENT_MFGREP_MAX_DATALEN -->

## Status Report Request Event

<!-- doxygen:struct:osdp_status_report:members -->

<!-- doxygen:enum:osdp_status_report_type -->

## Notifications

When a CP is set up with `OSDP_FLAG_ENABLE_NOTIFICATION`, LibOSDP reports
internal state changes to the application as `OSDP_EVENT_NOTIFICATION` events
carrying this structure.

<!-- doxygen:struct:osdp_event_notification:members -->

<!-- doxygen:enum:osdp_event_notification_type -->
