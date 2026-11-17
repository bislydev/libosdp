---
lastUpdated: 2026-10-24
description: "Reference for OSDP commands and replies — the full message set exchanged between a control panel (CP) and peripheral device (PD) on the OSDP bus."
---
# Command and Reply Codes

The following commands and replies are reserved by the protocol for various
actions. The tables below show their descriptions and support status in this
implementation.

The secure-channel commands — `CMD_KEYSET`, `CMD_CHLNG`, and `CMD_SCRYPT` — are
described in [Secure Channel](/libosdp/secure-channel).

## Legend

The **Support** column in the tables below uses these glyphs:

| Glyph | Meaning |
|-------|---------|
| <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> | Supported by LibOSDP |
| <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> | Not supported |
| <span class="status-glyph status-glyph-na" role="img" aria-label="Not applicable">–</span> | Not applicable — reserved by the protocol, not implemented |
| <span class="status-glyph status-glyph-deprecated" role="img" aria-label="Deprecated">†</span> | Deprecated — retained for compatibility, not used |

## Commands

| Command            | Value  | Description                                           | Support |
|--------------------|--------|-------------------------------------------------------|---------|
| `CMD_POLL`         | `0x60` | Poll                                                  | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_ID`           | `0x61` | ID Report Request                                     | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_CAP`          | `0x62` | Peripheral Device Capabilities Request                | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_DIAG`         | `0x63` | Diagnostic Function                                   | <span class="status-glyph status-glyph-na" role="img" aria-label="Not applicable">–</span> |
| `CMD_LSTAT`        | `0x64` | Local Status Report Request                           | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_ISTAT`        | `0x65` | Input Status Report Request                           | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_OSTAT`        | `0x66` | Output Status Report Request                          | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_RSTAT`        | `0x67` | Reader Status Tamper Report Request                   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_OUT`          | `0x68` | Output Control Command                                | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_LED`          | `0x69` | Reader LED Control Command                            | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_BUZ`          | `0x6A` | Reader Buzzer Control Command                         | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_TEXT`         | `0x6B` | Reader Text Output Command                            | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_RMODE`        | `0x6C` | Reader Mode Set                                       | <span class="status-glyph status-glyph-deprecated" role="img" aria-label="Deprecated">†</span> |
| `CMD_TDSET`        | `0x6D` | Time and Date Set                                     | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_COMSET`       | `0x6E` | Communication Configuration Command                   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_BIOREAD`      | `0x73` | Scan and send biometric data                          | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_BIOMATCH`     | `0x74` | Scan and match biometric data                         | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_KEYSET`       | `0x75` | Encryption Key Set                                    | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_CHLNG`        | `0x76` | Challenge and Secure Session Initialization Request   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_SCRYPT`       | `0x77` | Server's Random Number and Server Cryptogram          | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_ACURXSIZE`    | `0x7B` | Maximum Acceptable Reply Size                         | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_FILETRANSFER` | `0x7C` | File transfer command                                 | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_MFG`          | `0x80` | Manufacturer Specific Command                         | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_XWR`          | `0xA1` | Extended write data (Transparent Reader Support)      | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_ABORT`        | `0xA2` | Abort PD operation                                    | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `CMD_PIVDATA`      | `0xA3` | Get PIV Data                                          | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_GENAUTH`      | `0xA4` | Request Authenticate                                  | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_CRAUTH`       | `0xA5` | Request Crypto Response                               | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `CMD_KEEPACTIVE`   | `0xA7` | Keep secure channel active                            | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |

## Responses

| Response          | Value  | Description                                              | Support |
|-------------------|--------|----------------------------------------------------------|---------|
| `REPLY_ACK`       | `0x40` | General Acknowledge, Nothing to Report                   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_NAK`       | `0x41` | Negative Acknowledge – SIO Comm Handler Error Response   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_PDID`      | `0x45` | Device Identification Report                             | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_PDCAP`     | `0x46` | Device Capabilities Report                               | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_LSTATR`    | `0x48` | Local Status Report                                      | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_ISTATR`    | `0x49` | Input Status Report                                      | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_OSTATR`    | `0x4A` | Output Status Report                                     | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_RSTATR`    | `0x4B` | Reader Status Tamper Report                              | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_RAW`       | `0x50` | Card Data Report, Raw Bit Array                          | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_FMT`       | `0x51` | Card Data Report, Character Array                        | <span class="status-glyph status-glyph-deprecated" role="img" aria-label="Deprecated">†</span> |
| `REPLY_KEYPAD`    | `0x53` | Keypad Data Report                                       | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_COM`       | `0x54` | Communication Configuration Report                       | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_BIOREADR`  | `0x57` | Biometric Data                                           | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_BIOMATCHR` | `0x58` | Biometric Match Result                                   | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_CCRYPT`    | `0x76` | Client's ID and Client's Random Number                   | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_RMAC_I`    | `0x78` | Client Cryptogram Packet and the Initial R-MAC           | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_BUSY`      | `0x79` | PD Is Busy Reply                                         | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_FTSTAT`    | `0x7A` | File transfer status                                     | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_PIVDATAR`  | `0x80` | PIV data reply                                           | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_GENAUTHR`  | `0x81` | Authentication response                                  | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_CRAUTHR`   | `0x82` | Response to challenge                                    | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_MFGSTATR`  | `0x83` | Manufacturer specific status                             | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_MFGERRR`   | `0x84` | Manufacturer specific error                              | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |
| `REPLY_MFGREP`    | `0x90` | Manufacturer specific reply                              | <span class="status-glyph status-glyph-ok" role="img" aria-label="Supported">✓</span> |
| `REPLY_XRD`       | `0xB1` | Extended read response (Transparent Reader Support)      | <span class="status-glyph status-glyph-no" role="img" aria-label="Not supported">✕</span> |

## LibOSDP Behavior Notes

For `CMD_MFG`, the PD's command callback decides the immediate reply: a return
`< 0` yields `REPLY_NAK`, `0` yields `REPLY_ACK`, and `> 0` sends the `mfg`
payload the callback left in the command back to the CP as `REPLY_MFGREP`. A
PD can also report `REPLY_MFGREP` later by submitting an `OSDP_EVENT_MFGREP`
event, which is delivered on a subsequent `CMD_POLL` cycle.

**Deprecated `REPLY_FMT`.** `REPLY_FMT` (Card Data Report, Character Array) was
underspecified by SIA and marked for deprecation. LibOSDP ignores it on receipt
and never emits it; use `REPLY_RAW` for card data instead.

**Deprecated `CMD_RMODE`.** Reader Mode Set (`0x6C`) has been removed from the
OSDP specification. LibOSDP never implemented it and neither issues nor handles
it; the code point is retained only so packet traces can still name it.
