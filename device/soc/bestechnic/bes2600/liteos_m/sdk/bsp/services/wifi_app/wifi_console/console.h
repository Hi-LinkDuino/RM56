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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define CONSOLE_DUMP_STACK_EN

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_CBSIZE       1024
#define CMD_PROMPT       "best_wifi> "
#define CMD_MAXARGS      16
#define CMD_LIST_COUNT   256
#define CONSOLE_RING_BUFFER_SIZE  1024// mast be 2^n
//2 TODO: This value should be equal to CONSOLE cmd_list elements number!!!
#ifdef __WIFI_NVR_SUPPORT__
#define MAX_CMD_ITEMS       (10)
#else
#define MAX_CMD_ITEMS       (8)
#endif

typedef struct cmd_tbl_s {
	char	*name;									   /* Command Name */
	int 	maxargs;								   /* maximum number of arguments */
	int 	(*cmd)(struct cmd_tbl_s *, int, char *[], void *handler);
	char	*usage;                                    /* Usage message(short)*/
} cmd_tbl_t;

extern cmd_tbl_t cmd_list[CMD_LIST_COUNT];
extern unsigned int cmd_cntr;

#define console_strtoul     simple_strtoul
#define console_cmd_usage   show_cmd_usage
#define console_cmd_add     add_cmd_to_list

extern void console_init(void);
extern void console_puts(const char *s);
extern void console_putc(char c);
extern void console_wakeup(void);
extern void console_sleep(void);
extern void console_onoff(uint8_t onoff);

extern void nts_free(void *rmem);
extern void *nts_malloc(size_t size);
extern void *nts_calloc(size_t count, size_t size);

extern void init_console_irq_buffer(void);
extern void uninit_console_irq_buffer(void);
extern int run_command (void *handler, char *cmd);
extern int handle_char (const char c, char *prompt);
extern uint32_t simple_strtoul(const char *cp,char **endp,unsigned int base);
extern void show_cmd_usage(const cmd_tbl_t *cmd);
extern int add_cmd_to_list(const cmd_tbl_t *cmd);
int do_help(cmd_tbl_t *cmd, int argc, char *argv[], void *handler);
extern int cmd_get_data_size(const char *arg, int default_size);
int get_at_cmd_echo();
void console_task_start(void);
#ifdef __cplusplus
}
#endif


#endif

