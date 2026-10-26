/*
 * Copyright (c) 2019-2026 Siddharth Chandrasekaran <sidcha.dev@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>

#include <osdp.h>
#include "test.h"

extern int (*test_state_update)(struct osdp_pd *);
extern int (*test_cp_phy_state_update)(struct osdp_pd *);

struct retry_capture {
	uint8_t packet[OSDP_PACKET_BUF_SIZE];
	int length;
	int sends;
	bool identical;
};

static int capture_retry(void *data, uint8_t *buf, int len)
{
	struct retry_capture *capture = data;

	if (len <= 0 || len > (int)sizeof(capture->packet)) {
		return -1;
	}
	if (capture->sends == 0) {
		memcpy(capture->packet, buf, len);
		capture->length = len;
	} else if (len != capture->length || memcmp(capture->packet, buf, len)) {
		capture->identical = false;
	}
	capture->sends++;
	return len;
}

static int no_reply(void *data, uint8_t *buf, int len)
{
	ARG_UNUSED(data);
	ARG_UNUSED(buf);
	ARG_UNUSED(len);
	return 0;
}

static void run_retry_recovery_tests(struct test *t)
{
	struct retry_capture capture = { 0 };
	osdp_pd_info_t info = {
		.address = 1,
		.baud_rate = 38400,
		.channel.data = &capture,
		.channel.send = capture_retry,
		.channel.recv = no_reply,
	};
	struct osdp *ctx = (struct osdp *)osdp_cp_setup(1, &info);
	bool retries_ok = true, jitter_ok = true;

	if (!ctx) {
		TEST_REPORT(t, false);
		return;
	}
	SET_CURRENT_PD(ctx, 0);
	struct osdp_pd *pd = GET_CURRENT_PD(ctx);
	/* Include sequence wrap and repeated offline/recovery cycles. */
	for (int cycle = 0; cycle < 32; cycle++) {
		capture = (struct retry_capture) { .identical = true };
		osdp_phy_state_reset(pd, true);
		pd->state = OSDP_CP_STATE_ONLINE;
		pd->seq_number = cycle % 4;
		pd->cmd_id = CMD_POLL;
		pd->phy_state = OSDP_CP_PHY_STATE_SEND_CMD;
		test_cp_phy_state_update(pd);
		for (int retry = 0; retry < OSDP_CMD_MAX_RETRIES; retry++) {
			pd->phy_tstamp = osdp_millis_now() - OSDP_RESP_TOUT_MS - 1;
			test_cp_phy_state_update(pd);
			retries_ok &= pd->phy_state == OSDP_CP_PHY_STATE_WAIT;
			pd->phy_tstamp = osdp_millis_now() - OSDP_CMD_RETRY_WAIT_MS;
			test_cp_phy_state_update(pd);
		}
		pd->phy_tstamp = osdp_millis_now() - OSDP_RESP_TOUT_MS - 1;
		test_cp_phy_state_update(pd);
		retries_ok &= pd->phy_state == OSDP_CP_PHY_STATE_ERR &&
			capture.sends == 1 + OSDP_CMD_MAX_RETRIES && capture.identical;
		test_state_update(pd);
		jitter_ok &= pd->state == OSDP_CP_STATE_OFFLINE &&
			pd->wait_ms >= OSDP_ONLINE_RETRY_WAIT_MAX_MS - 900 &&
			pd->wait_ms <= OSDP_ONLINE_RETRY_WAIT_MAX_MS &&
			(OSDP_ONLINE_RETRY_WAIT_MAX_MS - pd->wait_ms) % 100 == 0;
		pd->tstamp = osdp_millis_now() - pd->wait_ms + 100;
		test_state_update(pd);
		jitter_ok &= pd->state == OSDP_CP_STATE_OFFLINE;
		pd->tstamp = osdp_millis_now() - pd->wait_ms - 1;
		test_state_update(pd);
		jitter_ok &= pd->state == OSDP_CP_STATE_INIT;
	}
	printf(SUB_1 "Bounded byte-identical retries: %s\n", retries_ok ? "OK" : "FAIL");
	TEST_REPORT(t, retries_ok);
	printf(SUB_1 "Offline jitter and recovery: %s\n", jitter_ok ? "OK" : "FAIL");
	TEST_REPORT(t, jitter_ok);
	osdp_cp_teardown(ctx);
}

int test_fsm_resp = 0;

int test_cp_fsm_send(void *data, uint8_t *buf, int len)
{
	ARG_UNUSED(data);

#ifndef OPT_OSDP_SKIP_MARK_BYTE
	int cmd_id_offset = OSDP_CMD_ID_OFFSET + 1;
#else
	int cmd_id_offset = OSDP_CMD_ID_OFFSET;
#endif

	switch (buf[cmd_id_offset]) {
	case 0x60:
		test_fsm_resp = 1;
		break;
	case 0x61:
		test_fsm_resp = 2;
		break;
	case 0x62:
		test_fsm_resp = 3;
		break;
	default:
		printf(SUB_1 "invalid ID:0x%02x\n", buf[cmd_id_offset

		]);
	}
	return len;
}

int test_cp_fsm_receive(void *data, uint8_t *buf, int len)
{
	ARG_UNUSED(data);

	uint8_t resp_id[] = {
#ifndef OPT_OSDP_SKIP_MARK_BYTE
		0xff,
#endif
		0x53, 0xe5, 0x14, 0x00, 0x04, 0x45, 0xa1, 0xa2, 0xa3, 0xb1,
		0xc1, 0xd1, 0xd2, 0xd3, 0xd4, 0xe1, 0xe2, 0xe3, 0xf8, 0xd9
	};
	uint8_t resp_cap[] = {
#ifndef OPT_OSDP_SKIP_MARK_BYTE
		0xff,
#endif
		0x53, 0xe5, 0x0b, 0x00, 0x05, 0x46, 0x04, 0x04, 0x01, 0xb3, 0xec
	};
	uint8_t resp_ack[] = {
#ifndef OPT_OSDP_SKIP_MARK_BYTE
		0xff,
#endif
		0x53, 0xe5, 0x08, 0x00, 0x06, 0x40, 0xb0, 0xf0
	};

	ARG_UNUSED(len);

	switch (test_fsm_resp) {
	case 1:
		memcpy(buf, resp_ack, sizeof(resp_ack));
		return sizeof(resp_ack);
	case 2:
		memcpy(buf, resp_id, sizeof(resp_id));
		return sizeof(resp_id);
	case 3:
		memcpy(buf, resp_cap, sizeof(resp_cap));
		return sizeof(resp_cap);
	}
	return -1;
}

int test_cp_fsm_setup(struct test *t)
{
	/* mock application data */
	osdp_pd_info_t info = {
		.address = 101,
		.baud_rate = 9600,
		.flags = 0,
		.channel.data = NULL,
		.channel.send = test_cp_fsm_send,
		.channel.recv = test_cp_fsm_receive,
		.channel.flush = NULL,
		.scbk = NULL,
	};
	osdp_logger_init("osdp::cp", t->loglevel, NULL);
	struct osdp *ctx = (struct osdp *)osdp_cp_setup(1, &info);
	if (ctx == NULL) {
		printf("   init failed!\n");
		return -1;
	}
	SET_CURRENT_PD(ctx, 0);
	SET_FLAG(GET_CURRENT_PD(ctx), PD_FLAG_SKIP_SEQ_CHECK);
	t->mock_data = (void *)ctx;
	return 0;
}

void test_cp_fsm_teardown(struct test *t)
{
	osdp_cp_teardown(t->mock_data);
}

void run_cp_fsm_tests(struct test *t)
{
	int result = true;
	uint32_t count = 0;
	struct osdp *ctx;

	printf("\nStarting CP Phy state tests\n");

	if (test_cp_fsm_setup(t))
		return;

	ctx = t->mock_data;

	printf(SUB_1 "executing state_update()\n");
	while (1) {
		test_state_update(GET_CURRENT_PD(ctx));

		if (GET_CURRENT_PD(ctx)->state == OSDP_CP_STATE_OFFLINE) {
			printf(SUB_2 "state_update() CP went offline\n");
			result = false;
			break;
		}
		if (count++ > 300)
			break;
		usleep(1000);
	}
	printf(SUB_1 "state_update test %s\n", result ? "succeeded" : "failed");

	TEST_REPORT(t, result);

	test_cp_fsm_teardown(t);
	run_retry_recovery_tests(t);
}

// unnecessary
