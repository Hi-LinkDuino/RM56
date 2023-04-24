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
#ifndef __SYS_API_PROGRAMMER_H__
#define __SYS_API_PROGRAMMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tool_msg.h"
#include "stdint.h"

#ifdef _WIN32
#include "stdio.h"

#ifndef TRACE
#define TRACE(n, str, ...)                  do { printf("[%u] %s/" str "\n", get_current_time(), __FUNCTION__, __VA_ARGS__); } while (0)
#endif
#ifndef ASSERT
#define ASSERT(cond, str, ...)              \
    do { if (!(cond)) { printf("[%u] [ASSERT] %s/" str "\n", get_current_time(), __FUNCTION__, __VA_ARGS__); while (1); } } while (0)
#endif
#ifndef TRACE_TIME
#define TRACE_TIME(n, str, ...)             TRACE(n, str, __VA_ARGS__)
#endif
#define CUST_CMD_INIT_TBL_LOC
#define CUST_CMD_HDLR_TBL_LOC

int write_flash_data(const unsigned char *data, unsigned int len);
#else
#include "hal_trace.h"
#include "hal_timer.h"

#define TRACE_TIME(n, str, ...)             TRACE(n, "[%05u] " str, TICKS_TO_MS(hal_sys_timer_get()), ##__VA_ARGS__)
#define CUST_CMD_INIT_TBL_LOC               __attribute__((section(".cust_cmd_init_tbl"), used))
#define CUST_CMD_HDLR_TBL_LOC               __attribute__((section(".cust_cmd_hldr_tbl"), used))
#endif

enum PGM_DOWNLOAD_TIMEOUT {
    PGM_TIMEOUT_RECV_SHORT,
    PGM_TIMEOUT_RECV_IDLE,
    PGM_TIMEOUT_RECV_4K_DATA,
    PGM_TIMEOUT_SEND,
    PGM_TIMEOUT_SYNC,
};

enum CUST_CMD_STAGE {
    CUST_CMD_STAGE_HEADER,
    CUST_CMD_STAGE_DATA,
    CUST_CMD_STAGE_EXTRA,

    CUST_CMD_STAGE_QTY
};

struct CUST_CMD_PARAM {
    enum CUST_CMD_STAGE stage;
    const struct message_t *msg;
    int extra;
    unsigned char *buf;
    size_t expect;
    size_t size;
    unsigned int timeout;
};

typedef struct
{
    unsigned short magic;
    unsigned short version;
    unsigned int crc;
} section_wifi_head_t;

typedef struct
{
    unsigned char mac_addr[6];
    unsigned short freq_cal;
    unsigned int iQ_cal;
    unsigned int iQ_offset;
/*
	tx_power_type 0 - save bgn 1,7,13 power
	tx_power_type 1 - save bgn 1-13 power
	tx_power_type 0xff - not calibration
*/
    unsigned char tx_power_type;

/*
	freq_cal_flags 0 - chip not calibrated
	freq_cal_flags 1 - chip has been calibrated
*/
    unsigned char freq_cal_flags;

/*
	 when freq_cal_flags value is 0
	 index 0-8
			0-2:11b ch1, 11b ch7, 11b ch13
			3-5:11g ch1, 11g ch7, 11g ch13
			6-8:11n ch1, 11n ch7, 11n ch13

	  when freq_cal_flags value is 1
     index 0-41
            0-13:11b ch1, 11b ch2 ......11b ch14
           14-27:11g ch1, 11g ch2 ......11g ch14
           28-41:11n ch1, 11n ch2 ......11n ch14
*/
    unsigned short tx_power_ch[42];
    unsigned short temperature;
    unsigned char band; //0:2.4 1:5 2:dual band
    unsigned char country[3];
    unsigned int bt_tx_power[4];

    /*
        5G power calibration 0-12 save the 5G calib power,13-15 reserve
        5G power calibration 0-12 save the 5G calib power,13-15 reserve
        11g
        0   36~40£»1    44~48£»2    52~56£»3    60~64;
        4   100~104£»5    108~112£»6    116~120;
        7   124~128£»8    132~136£»9    140~144
        11    149~153; 12   157~161£»13   165~169

        11n
        0   36~40£»1    44~48£»2    52~56£»3    60~64;
        4   100~104£»5    108~112£»6    116~120;
        7   124~128£»8    132~136£»9    140~144
        11    149~153; 12   157~161£»13   165~169

    */
#if 1 //def __DUAL_BAND__
        unsigned short tx_power_ch_5G[32];
    /*
        0- it means that power not calib
        1- it means that power have clibrated
    */
        unsigned short tx_power_flags_5G;
    /*
        The temperature after 5G clibrating.
    */
        unsigned short temperature_5G;
#endif
} factory_section_wifi_data_t;

typedef struct{
    section_wifi_head_t head;
    factory_section_wifi_data_t data;
}factory_section_wifi_t;

#define nvrec_mini_version          1
#define nvrec_dev_magic             0xba80
#define nvrec_current_version       2
#define FACTORY_SECTION_OFFSET      0x0
#define FACTORY_SECTION_WIFI_OFFSET 0x800 /* offset 2k */
#define TOTAL_LOGIC_PARTITION_NUM   2
#define PAR_OPT_READ_EN             ( 0x1u << 0 )
#define PAR_OPT_WRITE_EN            ( 0x1u << 1 )

typedef struct {
    char         *partition_description;
    uint32_t     partition_start_addr;
    uint32_t     partition_length;
    uint32_t     partition_options;
} hal_logic_partition_t;

typedef struct {
    uint16_t    remain_data_len;
    uint8_t     seq_num;
    uint8_t     data[255];
} more_data_t;

typedef void (*CUST_CMD_INIT_T)(void);
typedef enum ERR_CODE (*CUST_CMD_HANDLER_T)(struct CUST_CMD_PARAM *param);

void init_download_context(void);

void init_cust_cmd(void);
enum ERR_CODE handle_cust_cmd(struct CUST_CMD_PARAM *param);

void reset_transport(void);

void set_recv_timeout(unsigned int timeout);
void set_send_timeout(unsigned int timeout);

unsigned int get_pgm_timeout(enum PGM_DOWNLOAD_TIMEOUT timeout);
unsigned int get_current_time(void);

int send_data(const unsigned char *buf, size_t len);
int recv_data_ex(unsigned char *buf, size_t len, size_t expect, size_t *rlen);
int handle_error(void);
int cancel_input(void);
int cancel_output(void);

int debug_read_enabled(void);
int debug_write_enabled(void);

void system_reboot(void);
void system_shutdown(void);
void system_flash_boot(void);
void system_set_bootmode(unsigned int bootmode);
void system_clear_bootmode(unsigned int bootmode);
unsigned int system_get_bootmode(void);
void system_set_download_rate(unsigned int rate);
int get_sector_info(unsigned int addr, unsigned int *sector_addr, unsigned int *sector_len);
int get_flash_id(unsigned char id, unsigned char cs, unsigned char *buf, unsigned int len);
int get_flash_unique_id(unsigned char id, unsigned char cs, unsigned char *buf, unsigned int len);
int get_flash_size(unsigned char id, unsigned char cs, unsigned int *size);
int erase_sector(unsigned int sector_addr, unsigned int sector_len);
int erase_chip(unsigned char id, unsigned char cs);
int burn_data(unsigned int addr, const unsigned char *data, unsigned int len);
int verify_flash_data(unsigned int addr, const unsigned char *data, unsigned int len);

int enable_flash_remap(unsigned int addr, unsigned int len, unsigned int offset, int burn_both);
int disable_flash_remap(void);

int wait_data_buf_finished(void);
int wait_all_data_buf_finished(void);
int abort_all_data_buf(void);

int read_security_register(unsigned int addr, unsigned char *data, unsigned int len);
int erase_security_register(unsigned int addr, unsigned int len);
int get_security_register_lock_status(unsigned int addr, unsigned int len);
int lock_security_register(unsigned int addr, unsigned int reg_len);
int burn_security_register(unsigned int addr, const unsigned char *data, unsigned int len);
int verify_security_register_data(unsigned int addr, unsigned char *load_buf, const unsigned char *data, unsigned int len);

int send_debug_event(const void *buf, size_t len);

void programmer_watchdog_init(void);
void programmer_watchdog_ping(void);

void copy_new_image(void);
void programmer_copy_new_image(uint32_t srcFlashOffset, uint32_t dstFlashOffset);

#ifdef __cplusplus
}
#endif

#endif

