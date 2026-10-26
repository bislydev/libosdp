---
lastUpdated: 2026-10-24
description: "LibOSDP command structure reference — the osdp_cmd data structures and enums used to send OSDP commands such as LED, buzzer, output, text, and key-set to a PD."
---
# Application Commands

LibOSDP exposes the following structures through `osdp.h`. All commands are
carried in a single union wrapper:

<!-- doxygen:struct:osdp_cmd:members -->

The `id` field selects the active union member and is one of `osdp_cmd_e`:

<!-- doxygen:enum:osdp_cmd_e -->

<!-- doxygen:define:OSDP_CMD_FLAG_BROADCAST -->

Below are the structures for each command type. They are submitted to a PD with
[`osdp_cp_submit_command`](/api/control-panel).

## LED Command

<!-- doxygen:struct:osdp_cmd_led_params:members -->

<!-- doxygen:struct:osdp_cmd_led:members -->

<!-- doxygen:enum:osdp_led_color_e -->

## Buzzer Command

<!-- doxygen:struct:osdp_cmd_buzzer:members -->

## Text Command

<!-- doxygen:struct:osdp_cmd_text:members -->

<!-- doxygen:define:OSDP_CMD_TEXT_MAX_LEN -->

## Output Command

<!-- doxygen:struct:osdp_cmd_output:members -->

## Comset Command

<!-- doxygen:struct:osdp_cmd_comset:members -->

## Keyset Command

This command sets the SCBK on a PD; see [Secure Channel](/libosdp/secure-channel)
for how keying and install mode work.

<!-- doxygen:struct:osdp_cmd_keyset:members -->

<!-- doxygen:define:OSDP_CMD_KEYSET_KEY_MAX_LEN -->

## Manufacturer Specific Command

<!-- doxygen:struct:osdp_cmd_mfg:members -->

<!-- doxygen:define:OSDP_CMD_MFG_MAX_DATALEN -->

## File Transfer Command

<!-- doxygen:struct:osdp_cmd_file_tx:members -->

<!-- doxygen:define:OSDP_CMD_FILE_TX_FLAG_CANCEL -->

## Status Report Command

<!-- doxygen:struct:osdp_status_report:members -->

<!-- doxygen:enum:osdp_status_report_type -->

<!-- doxygen:define:OSDP_STATUS_REPORT_MAX_LEN -->

## NAK Codes

When a PD declines a command it replies with a NAK carrying one of these reason
codes.

<!-- doxygen:enum:osdp_pd_nak_code_e -->
