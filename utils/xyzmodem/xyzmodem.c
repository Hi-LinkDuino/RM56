/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "hal_trace.h"
#include "hal_uart.h"
#include "string.h"
#include "hal_timer.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25
#define TRANSMIT_XMODEM_1K

/* CRC16 implementation acording to CCITT standards */

static const unsigned short crc16tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

static unsigned short crc16_ccitt(const void *buf, int len)
{
	register int counter;
	register unsigned short crc = 0;
	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
	return crc;
}

static int check(int crc_check, const uint8_t *buf, int sz) __attribute__((unused));
static int check(int crc_check, const uint8_t *buf, int sz)
{
	if (crc_check) {
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
			return 1;
	}
	else {
		int i;
		unsigned char cks = 0;
		for (i = 0; i < sz; ++i) {
			cks += buf[i];
		}
		if (cks == buf[sz])
		return 1;
	}

	return 0;
}

static enum HAL_UART_ID_T trace_uart;

static uint8_t _inbyte(unsigned int delay)
{
    return hal_uart_blocked_getc(trace_uart);
}

static int _outbyte(uint8_t c)
{
    return hal_uart_blocked_putc(trace_uart, c);
}

static void flushinput(void)
{
	while (_inbyte(((DLY_1S)*3)>>1) >= 0)
		;
}

static unsigned char packet_no = 1;
static unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
static uint32_t packet_size  = 1024;
static int crc_enabled;
static int packet_idx;

static int xmodem_fill_packet(uint8_t *buff, uint8_t packetno, int len, int crc)
{
    int c = len;
    int bufsz;
    int i;

#ifdef TRANSMIT_XMODEM_1K
    buff[0] = STX; bufsz = 1024;
#else
    buff[0] = SOH; bufsz = 128;
#endif
    buff[1] = packetno;
    buff[2] = ~packetno;

    //TRACE_IMM(4,"buff %p, packetno %d, len %d, crc %d", buff, packetno, len, crc);
    if (c < bufsz)
        memset(&buff[3 + c], CTRLZ, bufsz);

    if (crc) {
        unsigned short ccrc = crc16_ccitt(&buff[3], bufsz);
        buff[bufsz+3] = (ccrc>>8) & 0xFF;
        buff[bufsz+4] = ccrc & 0xFF;
    }
    else {
        unsigned char ccks = 0;
        for (i = 3; i < bufsz+3; ++i) {
            ccks += buff[i];
        }
        buff[bufsz+3] = ccks;
    }
    return 0;
}

static int xmodem_send_one_packet(void)
{
    int retry = 0;
    int bufsz;
    int crc = 1;
    int i;
    uint8_t c;

#ifdef TRANSMIT_XMODEM_1K
    bufsz = 1024;
#else
    bufsz = 128;
#endif
    packet_size  = bufsz;
    for (retry = 0; retry < MAXRETRANS; ++retry) {
        for (i = 0; i < bufsz+4+(crc?1:0); ++i) {
            _outbyte(xbuff[i]);
        }
        if ((c = _inbyte(DLY_1S)) >= 0 ) {
            switch (c) {
            case ACK:
                return 0;
            case CAN:
                if ((c = _inbyte(DLY_1S)) == CAN) {
                    _outbyte(ACK);
                    flushinput();
                    return -1; /* canceled by remote */
                }
                break;
            case NAK:
            default:
                break;
            }
        }
    }
    _outbyte(CAN);
    _outbyte(CAN);
    _outbyte(CAN);
    flushinput();
    return -4; /* xmit error */
}

static int xmodem_connect_host(void)
{
    uint8_t c;
    int retry;

    for( retry = 0; retry < 16; ++retry) {
        if ((c = _inbyte((DLY_1S)<<1)) >= 0) {
            switch (c) {
            case 'C':
                crc_enabled = 1;
                return 0;
            case NAK:
                crc_enabled = 0;
                return 0;
            case CAN:
                if ((c = _inbyte(DLY_1S)) == CAN) {
                    _outbyte(ACK);
                    flushinput();
                    return -1; /* canceled by remote */
                }
                break;
            default:
                break;
            }
        }
    }
    return -2; /* no sync */
}

static uint32_t filled_one_payload(uint8_t *dst, const uint8_t *data, size_t len, int ascii)
{
    static const char hexToASCII[] = "0123456789ABCDEF";
    int i;
    uint8_t byte;
    int c;

    if (ascii) {
        for( i = 0; i < len; i++) {
            c = *(data + i);
            byte = c >> 4;
            dst[i * 2] = hexToASCII[byte];
            byte = c & 0x0F;
            dst[i * 2 + 1] = hexToASCII[byte];
        }
    } else {
        for( i = 0; i < len; i++) {
            byte = *(data + i);
            dst[i] = byte;
        }
    }

    return len << ascii;
}

int xmodem_send_stream(const uint8_t *data, size_t size, int ascii)
{
    uint32_t left;
    uint8_t *payload;
    uint32_t filled;
    int packet_room;
    int xfer;
    int ret;
    uint8_t *dst;
    const uint8_t *src;

    left = size;

    payload = &xbuff[3];

    if ((ascii == 0) && (size % 2 != 0)) {
        /*
         * if don't need convert to ascii, the size should be multi of 2,
         * because the packet should include ascii and binary at same time;
         */
        return -1;
    }
    src = data;
    while (left > 0) {
        packet_room = packet_size - packet_idx;
        if (left > (packet_room >> ascii))
            xfer = packet_room >> ascii;
        else
            xfer = left;

        /*
        TRACE_IMM(2,"left %d packet room %d", left, packet_room);
        TRACE_IMM(1,"xfer %d", xfer);
        */
        dst = payload + packet_idx;
        filled = filled_one_payload(dst, src, xfer, ascii);
        src += xfer;
        if ((filled  + packet_idx) == packet_size ) {
            xmodem_fill_packet(xbuff, packet_no, 1024, crc_enabled);
            ret = xmodem_send_one_packet();
            //ret = 0;
            if (ret == 0)
                packet_no++;
            memset(xbuff, 0, sizeof(xbuff));
            packet_idx  = 0;
        } else {
            packet_idx += filled;
        }

        left -= xfer;
    }

    return size;
}

int xmodem_start_xfer(uint32_t wait)
{
    int ret;
    int delay = wait;
    enum HAL_TRACE_TRANSPORT_T transport;
    uint8_t input;
    uint32_t baud;
    struct HAL_UART_CFG_T uart_cfg = {
        .parity = HAL_UART_PARITY_NONE,
        .stop = HAL_UART_STOP_BITS_1,
        .data = HAL_UART_DATA_BITS_8,
        .flow = HAL_UART_FLOW_CONTROL_NONE,//HAL_UART_FLOW_CONTROL_RTSCTS,
        .tx_level = HAL_UART_FIFO_LEVEL_1_2,
        .rx_level = HAL_UART_FIFO_LEVEL_1_2,
        .baud = 921600,
        .dma_rx = false,
        .dma_tx = true,
        .dma_rx_stop_on_err = false,
    };

    transport = hal_trace_get_transport();
    if (transport == HAL_TRACE_TRANSPORT_UART0) {
        trace_uart = HAL_UART_ID_0;
#if (CHIP_HAS_UART >= 2)
    } else if (transport == HAL_TRACE_TRANSPORT_UART1) {
        trace_uart = HAL_UART_ID_1;
#endif
#if (CHIP_HAS_UART >= 3)
    } else if (transport == HAL_TRACE_TRANSPORT_UART2) {
        trace_uart = HAL_UART_ID_2;
#endif
    } else {
        return -1;
    }

    baud = hal_trace_get_baudrate();
    hal_trace_flush_output();
    hal_uart_close(trace_uart);

    uart_cfg.baud = baud;
    uart_cfg.dma_rx = false;
    uart_cfg.dma_tx = true;
    hal_uart_open(trace_uart, &uart_cfg);

    R_TR_INFO(TR_ATTR_IMM, "Press 'ESC' to skip, 'd' to dump on terminal, or any other key to enable XMODEM");

    while (!hal_uart_readable(trace_uart) && (delay > 0)) {
        hal_sys_timer_delay(MS_TO_HWTICKS(1000));
        delay--;
        R_TR_INFO(TR_ATTR_IMM | TR_ATTR_NO_LF, "\b\b\b%3d", delay);
        REL_LOG_FLUSH();
    }
    R_TR_INFO(TR_ATTR_IMM, " ");
    R_TR_INFO(TR_ATTR_IMM, " ");

    if (delay == 0) {
        R_TR_INFO(TR_ATTR_IMM, "time out, cannot start XMODEM....");
        return -2;
    }

    input = hal_uart_getc(trace_uart);
    if (input == '\x1B') {
        return -3;
    } else if (input == 'd') {
        return 1;
    }

    R_TR_INFO(TR_ATTR_IMM, "XMODEM start ....");
    hal_sys_timer_delay(MS_TO_HWTICKS(30));

    hal_uart_close(trace_uart);
    uart_cfg.dma_tx = false;
    hal_uart_open(trace_uart, &uart_cfg);

    ret = xmodem_connect_host();
    if (ret != 0) {
		_outbyte(CAN);
		_outbyte(CAN);
		_outbyte(CAN);
		flushinput();
        return -3;
    }
    return 0;
}

int xmodem_stop_xfer(void)
{
    int ret = -1;
    int retry;
    int c;

    xmodem_fill_packet(xbuff, packet_no, packet_idx, crc_enabled);
    ret = xmodem_send_one_packet();
    if (!ret) {
        for (retry = 0; retry < 10; ++retry) {
            _outbyte(EOT);
            if ((c = _inbyte((DLY_1S)<<1)) == ACK)
                return 0;
        }
        flushinput();
    }
    return ret;
}

