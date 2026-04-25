/*
 * Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha.dev@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OSDP_FILE_H_
#define _OSDP_FILE_H_

#include "osdp_common.h"

#define TO_FILE(pd) (pd)->file

#define OSDP_FILE_TX_STATE_IDLE         0
#define OSDP_FILE_TX_STATE_PENDING      1
#define OSDP_FILE_TX_STATE_ERROR       -1
#define OSDP_FILE_TX_STATE_WAIT        -2

/**
 * @brief OSDP specified file transfer command payload.
 */
PACK(struct osdp_cmd_file_xfer {
	uint8_t type;   /**< File transfer type: 1=opaque, 2..127 reserved, 128..255 private use. */
	uint32_t size;  /**< File size in little-endian format. */
	uint32_t offset; /**< Offset in the file for the current message. */
	uint16_t length; /**< Length of the data section in this command. */
	uint8_t data[]; /**< Variable-length file contents. */
});

/**
 * @brief OSDP specified file transfer status payload.
 */
PACK(struct osdp_cmd_file_stat {
	uint8_t control; /**< Control flags: bit-0 interleave, bit-1 leave secure channel, bit-2 separate poll response available. */
	uint16_t delay; /**< Requested delay in milliseconds before the next CMD_FILETRANSFER message. */
	int16_t status; /**< Transfer status: 0 proceed, 1 processed, 2 rebooting, 3 finishing, -1 abort, -2 unrecognized, -3 malformed. */
	uint16_t rx_size; /**< Alternate maximum CMD_FILETRANSFER size, or 0 to keep the current value. */
});

enum file_tx_state_e {
	OSDP_FILE_IDLE,
	OSDP_FILE_INPROG,
	OSDP_FILE_DONE,
	OSDP_FILE_KEEP_ALIVE,
};

struct osdp_file {
	uint32_t flags;
	int file_id;
	enum file_tx_state_e state;
	int length;
	uint32_t size;
	uint32_t offset;
	int errors;
	bool cancel_req;
	int64_t tstamp;
	uint32_t wait_time_ms;
	struct osdp_file_ops ops;
};

int osdp_file_cmd_tx_build(struct osdp_pd *pd, uint8_t *buf, int max_len);
int osdp_file_cmd_tx_decode(struct osdp_pd *pd, uint8_t *buf, int len);
int osdp_file_cmd_stat_decode(struct osdp_pd *pd, uint8_t *buf, int len);
int osdp_file_cmd_stat_build(struct osdp_pd *pd, uint8_t *buf, int max_len);
int osdp_file_tx_command(struct osdp_pd *pd, int file_id, uint32_t flags);
int osdp_file_tx_get_command(struct osdp_pd *pd);
void osdp_file_tx_abort(struct osdp_pd *pd);

#endif /* _OSDP_FILE_H_ */
