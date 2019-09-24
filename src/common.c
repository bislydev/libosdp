/**
 *  SPDX-License-Identifier: MIT
 *
 *  Author: Siddharth Chandrasekaran
 *    Date: Sat Sep 14 14:02:33 IST 2019
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>

#include "common.h"

int g_log_level;  /* Note: log level is not contextual */
static unsigned short fcrc_table_is_valid = 0;  /* preset: CRC Table not initialized */
static unsigned short fcrc_table[256];          /* CRC table - working copy */

/* generate the table for POLY == 0x1012 */
static uint16_t fcrc_table_init(uint16_t *pTbl )
{
    int ii, jj;
    uint16_t ww;

    for (ii = 0; ii < 256; ii++) {
        ww = (uint16_t)(ii << 8);
        for (jj = 0; jj< 8; jj++) {
            if ( ww& 0x8000 ) {
                ww = (ww<< 1) ^ 0x1021;
            } else {
                ww = (ww<< 1);
            }
        }
        pTbl[ii] = ww;
    }
    return 1;
}

/* table based CRC - this is the "direct table" mode */
uint16_t compute_crc16(uint8_t *data, int  len)
{
    int i;
    uint16_t crc = 0;

    if (fcrc_table_is_valid == 0)
        fcrc_table_is_valid = fcrc_table_init(fcrc_table);

    for (i = 0, crc = 0x1D0F; i < len; i++)
        crc = (crc << 8) ^ fcrc_table[ ((crc >> 8) ^ data[i]) & 0xFF ];

    return crc;
}

uint8_t compute_checksum(uint8_t *msg, int length)
{
    uint8_t checksum = 0;
    int i, whole_checksum;

    whole_checksum = 0;
    for (i = 0; i < length; i++) {
        whole_checksum += msg[i];
        checksum = ~(0xff & whole_checksum) + 1;
    }
    return checksum;
}

void osdp_set_log_level(int log_level)
{
    g_log_level = log_level;
}

void osdp_log(int log_level, const char *fmt, ...)
{
    va_list args;
    char *buf;
    const char *levels[] = {
        "EMERG", "ALERT", "CRIT ", "ERROR",
        "WARN ", "NOTIC", "INFO ", "DEBUG"
    };

    if (log_level > g_log_level)
        return;

    va_start(args, fmt);
    vasprintf(&buf, fmt, args);
    va_end(args);

    printf("OSDP: %s: %s\n", levels[log_level], buf);
    free(buf);
}

void osdp_dump(const char *head, const uint8_t *data, int len)
{
    int i;
    char str[16+1] = { 0 };

    printf("%s [%d] =>\n    0000  %02x ", head, len, len ? data[0] : 0);
    str[0] = isprint(data[0]) ? data[0] : '.';
    for (i=1; i<len; i++) {
        if ((i&0x0f) == 0) {
            printf(" |%16s|", str);
            printf("\n    %04d  ", i);
        } else if((i&0x07) == 0) {
            printf(" ");
        }
        printf("%02x ", data[i]);
        str[i&0x0f] = isprint(data[i]) ? data[i] : '.';
    }
    if ((i &= 0x0f) != 0) {
        if (i < 8) printf(" ");
        for (; i<16; i++) {
            printf("   ");
            str[i] = ' ';
        }
        printf(" |%16s|", str);
    }
    printf("\n");
}

millis_t millis_now()
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return (millis_t)((tv.tv_sec) * 1000L + (tv.tv_usec) / 1000L);
}

millis_t millis_since(millis_t last)
{
    return millis_now() - last;
}
