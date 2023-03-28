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

#include "console.h"
#include "at.h"
#include "cmsis.h"

#include "hal_uart.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "kfifo.h"
#include "cmsis_os.h"

#ifndef SERV_CONSOLE_SZ
#define SERV_CONSOLE_SZ (24*1024)
#endif

typedef struct console_sema {
	osSemaphoreId	 sema_id;
	osSemaphoreDef_t def;
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
	uint32_t		 data[2];
#endif
#ifdef osCMSIS_RHINO
	ksem_t	sem;
#endif

} console_sema_t;

typedef struct bwifi_console {
	uint8_t 	   cmd_cnt;
	osThreadId	   thread_id;
	console_sema_t console_sema;
} bwifi_console_t;

bwifi_console_t p_console;

#ifdef CONSOLE_DUMP_STACK_EN
static char pre_char = '0';
static char pre_pre_char = '0';
#endif

#ifdef HAL_TRACE_RX_ENABLE
#define CONSOLE_USE_HAL_TRACE_RX
#endif

//static const enum HAL_UART_ID_T console_uart = HAL_UART_ID_0;

extern char console_buffer[CMD_CBSIZE];
extern char lastcommand[CMD_CBSIZE];

extern struct kfifo console_kfifo;

void console_task_start(void)
{
	p_console.cmd_cnt++;
	osSemaphoreRelease(p_console.console_sema.sema_id);
}

void console_putc(char c) {
	cmd_printf(NULL, "%c", c);
	//serial_putc(&stdio_uart, c);
}

void console_puts(const char *s) {
	cmd_printf(NULL, "%s", s);
	//while(*s) {
		//serial_putc(&stdio_uart, *s++);
	//}
}
#ifdef _WIFI_RF_TEST_MODULE_ 
extern int rf_parse_command(char *argstr);
extern int btrf_parse_command(char *argstr);
#endif

static unsigned char g_rf_test = 0;

void console_cmd_exec(void) {
	uint8_t  ch;
	int16_t len;
	uint32_t lock;

	while(kfifo_get(&console_kfifo, &ch, 1)) {
		len = handle_char(ch, 0);
		if (len >= 0) {
			strcpy(lastcommand, console_buffer);
			lock = int_lock();
			p_console.cmd_cnt--;
			int_unlock(lock);
			if (len > 0) {
#if defined(_WIFI_RF_TEST_MODULE_) && !defined(__RF_TRACE_BY_SPRINTF__)
				if (strstr(lastcommand,  "besphy") != NULL) {
					if (rf_parse_command(lastcommand) < 0) {
						cmd_printf(NULL, "command fail\r\n");
					}
					if(!g_rf_test) {
						g_rf_test = 1;
					}
				}
#if !defined __BT_CLOSE__ || defined BT_RF_TEST
				else if(strstr(lastcommand,  "btrf") != NULL) {
					if (btrf_parse_command(lastcommand) < 0) {
						cmd_printf(NULL, "bt rf command parse fail\r\n");
					}
					if (!g_rf_test) {
						g_rf_test = 1;
					}
				}
#endif
#endif
				if (!g_rf_test && run_command(NULL, lastcommand) < 0) {
					cmd_printf(NULL, "command fail\r\n");
				}
			}
			handle_char('0', "r");//'r' means reset
		}
		g_rf_test = 0;
	}
}

#ifdef CONSOLE_DUMP_STACK_EN
//#include "core_cmFunc.h"
static void console_stack_dump(void)
{
	uint8_t i = 0;
	uint32_t curr_psp = __get_PSP();
	uint32_t *tmp = (uint32_t *)curr_psp;
	cmd_printf(NULL, "\r\nPSP:%08X:", curr_psp);
	for(i=0; i<64; i++) {
		if (i % 8 == 0)
			cmd_printf(NULL, "\r\n[0x%08x-0x%08x]:", (uint32_t)(tmp + i), (uint32_t)(tmp + i + 7));
		cmd_printf(NULL, "0x%08x,",tmp[i]);
	}
	curr_psp = __get_MSP();
	tmp = (uint32_t *)curr_psp;
	cmd_printf(NULL, "\r\nMSP:%08X:", curr_psp);
	for(i=0; i<64; i++) {
		if (i % 8 == 0)
			cmd_printf(NULL, "\r\n[0x%08x-0x%08x]:", (uint32_t)(tmp + i), (uint32_t)(tmp + i + 7));
		cmd_printf(NULL, "0x%08x,",tmp[i]);
	}
	cmd_printf(NULL, "\r\n");
	ASSERT(0," ");
}
#endif
static unsigned int console_callback(unsigned char *buf, unsigned int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		char *c = (char *)buf + i;
		//console_putc(*c);
#ifdef CONSOLE_DUMP_STACK_EN
		if(pre_pre_char == '#' && pre_char == '*' ) {
			if(*c == '#') {
				console_stack_dump();
				pre_char = '0';
				pre_pre_char = '0';
			} else {
				pre_pre_char = pre_char;
				pre_char = *c;
			}
		} else {
				pre_pre_char = pre_char;
				pre_char = *c;
		}
#endif
		if((*c == '\r') || (*c == '\n') || (*c == 0x03)) {
			p_console.cmd_cnt++;
		}
	}
	kfifo_put(&console_kfifo, buf, len);
	if(p_console.cmd_cnt > 0)
		osSemaphoreRelease(p_console.console_sema.sema_id);
    return 0;
}

void cmd_send(const char *str) {
	if(!str || *str == 0)return;
	const char *p = str;
	unsigned int count = 0;
	while(*p){
		char c = *p++;
		count++;
		//console_putc(*c);
#ifdef CONSOLE_DUMP_STACK_EN
		if(pre_pre_char == '#' && pre_char == '*' ) {
			if(c == '#') {
				console_stack_dump();
				pre_char = '0';
				pre_pre_char = '0';
			} else {
				pre_pre_char = pre_char;
				pre_char = c;
			}
		} else {
				pre_pre_char = pre_char;
				pre_char = c;
		}
#endif
		if((c == '\r') || (c == '\n') || (c == 0x03)) {
			p_console.cmd_cnt++;
		}
	}
	kfifo_put(&console_kfifo, str, count);
	p--;
	if(*p != '\r' && *p != '\n'){
		kfifo_put(&console_kfifo, "\n", 1);
		p_console.cmd_cnt++;
	}
	osSemaphoreRelease(p_console.console_sema.sema_id);
}

#ifdef CONSOLE_USE_HAL_TRACE_RX
static unsigned int console_hal_trace_callback(unsigned char *buf, unsigned int len)
{
	buf[len] = '\n';
	return console_callback(buf, len+1);
}
#else
#if 0
static void console_rxisr_callback(void) {
	while(hal_uart_readable(console_uart))
	{
		unsigned char c = hal_uart_getc(console_uart);
		console_putc(c);
#ifdef CONSOLE_DUMP_STACK_EN
		if(pre_pre_char == '#' && pre_char == '*' ) {
			if(c == '#') {
				console_stack_dump();
				pre_char = '0';
				pre_pre_char = '0';
			} else {
				pre_pre_char = pre_char;
				pre_char = c;
			}
		} else {
				pre_pre_char = pre_char;
				pre_char = c;
		}
#endif
		{
			kfifo_put(&console_kfifo, &c, 1);
			if((c == '\r') || (c == '\n') || (c == 0x03)) {
				p_console.cmd_cnt++;
			}
		}
	}
	if(p_console.cmd_cnt > 0)
		osSemaphoreRelease(p_console.console_sema.sema_id);
}
#endif
#ifdef DEBUG
#if (DEBUG_PORT == 1)
static enum HAL_UART_ID_T trace_uart = HAL_UART_ID_0;
#elif (DEBUG_PORT == 2)
static enum HAL_UART_ID_T trace_uart = HAL_UART_ID_1;
#elif (DEBUG_PORT == 3)
static enum HAL_UART_ID_T trace_uart = HAL_UART_ID_2;
#else
#error "DEBUG_PORT error"
#endif
#else
static enum HAL_UART_ID_T trace_uart = HAL_UART_ID_QTY;
#endif
SRAM_BSS_LOC unsigned char console_hal_trace_buf[CONSOLE_RING_BUFFER_SIZE];
static void console_hal_trace_rx_start(void)
{
	struct HAL_DMA_DESC_T dma_desc_rx;
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.RX = 1;
    mask.RT = 1;

    hal_uart_dma_recv_mask(trace_uart, console_hal_trace_buf, CONSOLE_RING_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}
static void console_uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
	console_callback(console_hal_trace_buf, xfer_size);
	console_hal_trace_rx_start();
}
#endif
static void console_task(const void *arg) {
	init_console_irq_buffer();
	while (1) {
		osSemaphoreWait(p_console.console_sema.sema_id, osWaitForever);
		console_cmd_exec();
	}
	uninit_console_irq_buffer();

	osThreadExit();
}

__attribute__((unused)) static void uart_break_handler(void)
{
	AT_PRINT("****** Handle break ******");
	//cancel_xfer = true;
	//hal_uart_stop_dma_recv(console_uart);
	//hal_uart_stop_dma_send(console_uart);
	//uart_dma_rx_state = UART_DMA_ERROR;
}

osThreadDef(console_task, osPriorityNormal, 1, SERV_CONSOLE_SZ, "wifi_console");
void console_init(void)
{
#ifdef DEBUG
#ifdef CONSOLE_USE_HAL_TRACE_RX
    hal_trace_rx_register("AT", (HAL_TRACE_RX_CALLBACK_T)console_hal_trace_callback);
#else
	struct HAL_UART_CFG_T uart_cfg = {
	    .dma_rx = true,
		.dma_tx = true,
		.dma_rx_stop_on_err = false,
	};
	hal_uart_reopen(trace_uart, &uart_cfg);
    hal_uart_irq_set_dma_handler(trace_uart, console_uart_dma_rx_handler, NULL);
    console_hal_trace_rx_start();
#endif
	p_console.cmd_cnt = 0;
#ifdef CMSIS_OS_RTX
#if (osCMSIS < 0x20000U)
	memset(p_console.console_sema.data, 0, sizeof(p_console.console_sema.data));
	p_console.console_sema.def.semaphore = p_console.console_sema.data;
#else
    memset(&p_console.console_sema.def, 0, sizeof(osSemaphoreDef_t));
#endif
#endif
#ifdef osCMSIS_RHINO
	p_console.console_sema.def.sem = &p_console.console_sema.sem;
	p_console.console_sema.def.name = "console";
#endif
	p_console.console_sema.sema_id = osSemaphoreCreate(&p_console.console_sema.def, 0);
	ASSERT(p_console.console_sema.sema_id != NULL, "Failed to create console semaphore");

	p_console.thread_id = osThreadCreate(osThread(console_task), NULL);
	ASSERT(p_console.thread_id != NULL, "Failed to create console Thread");
#endif
}


