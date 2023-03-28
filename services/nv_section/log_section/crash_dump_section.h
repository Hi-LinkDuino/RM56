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
#ifndef __CRASH_DUMP_SECTION_H__
#define __CRASH_DUMP_SECTION_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
#include "plat_types.h"

#define CRASH_DUMP_MAGIC_CODE	  0x504D5544
#define CRASH_DUMP_ASSERT_CODE    0xEB6D0924
#define CRASH_DUMP_EXCEPTION_CODE 0xC78BA332
/*
 * Total number of core registers stored
 */
#define CRASH_DUMP_REGISTERS_NUM   17
#define CRASH_DUMP_REGISTERS_NUM_BYTES   ((CRASH_DUMP_REGISTERS_NUM)*4)
/*
 * Number bytes to store from stack
 *   - this is total, not per PSP/MSP
 */
#define CRASH_DUMP_STACK_NUM_BYTES 384
#define CRASH_DUMP_FRAME_SIZE (1024*16)

typedef struct {
  uint32_t magicCode; // must be CRASH_DUMP_MAGIC_CODE
  int32_t  seqnum;  // increased by 1 per crash
  uint32_t partnum;   // maximum crash log count saved in the flash
  uint32_t reserved;
  uint32_t bufpos;  // valid content in the following buf array
  uint32_t liteDumpOffset;
  uint32_t crashCode;
} CRASH_OUTPUT_BUF_HEADER_T;

/**
 * This structure MUST be a total size
 * of 0x4000
 */
struct CRASH_OUTPUT_BUF_T {
  CRASH_OUTPUT_BUF_HEADER_T hdr;
  uint8_t buf[CRASH_DUMP_FRAME_SIZE - sizeof(CRASH_OUTPUT_BUF_HEADER_T)];
};

/** Tell whether the system reboots because of a fresh assert or hardfault. */
bool crashdump_is_crash_happened(void);
void crashdump_set_existing_flag(uint8_t isExisting);
/**
 * Tell the flash offset of the latest crash dump
 *  -1 means there is no crash dump in the flash.
 *  Other values are the valid flash offset of the latest happend crash dump log.
 */
int32_t crashdump_get_latest_flash_offset(void);
/** Tell whether there is any crash dump logs in the flash. */
bool crashdump_is_crash_dump_existing(void);
void crashdump_set_crash_happened_flag(uint8_t isHappened);
void crashdump_dumptoflash(void);
void crashdump_init_section_info(void);
#else

#include "hal_trace.h"

#define CRASH_DUMP_PREFIX     "__CRASH_DUMP:"
#define CRASH_DUMP_TRACE(num,fmt, ...)   TRACE(num,fmt, ##__VA_ARGS__)
#define CRASH_LOG_ALIGN(x,a)  (uint32_t)(((x + a - 1)/a) * a)


#define CRASH_DUMP_SECTOR_SIZE             0x1000
#define CRASH_DUMP_BUFFER_LEN              (CRASH_DUMP_SECTOR_SIZE*2)
#define CRASH_DUMP_NORFALSH_BUFFER_LEN     (CRASH_DUMP_BUFFER_LEN)

#define DATA_BUFFER_STATE_FREE           0
#define DATA_BUFFER_STATE_WRITTING       0x1
#define DATA_BUFFER_STATE_WRITTEN        0x2

#define CRASH_DUMP_MAGIC_CODE              0x504D5544
#define CRASH_DUMP_VERSION                 0x00010001

#define CRASH_DUMP_ASSERT_CODE    0xEB6D0924
#define CRASH_DUMP_EXCEPTION_CODE 0xC78BA332

typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t seqnum;
    uint8_t reseved[4];
}CRASH_DUMP_HEADER_T;


#define CRASH_DUMP_FRAME_SIZE (CRASH_DUMP_BUFFER_LEN)
typedef struct{
    uint32_t offset;
    uint8_t *buffer;
}CRASH_DATA_BUFFER;

void crash_dump_init(void);
void crash_dump_init_buffer(void);
int32_t crash_dump_read(uint32_t addr,uint8_t* ptr,uint32_t len);
int32_t crash_dump_write(uint32_t addr,uint8_t* ptr,uint32_t len);
void crash_dump_notify_handler(enum HAL_TRACE_STATE_T state);
void crash_dump_output_handler(const unsigned char *buf, unsigned int buf_len);
void crash_dump_fault_handler(const unsigned char *buf, unsigned int buf_len);
void crash_dump_set_flag(uint8_t is_happend);
void crash_dump_register(HAL_TRACE_APP_NOTIFY_T notify_cb,HAL_TRACE_APP_OUTPUT_T crash_output_cb,HAL_TRACE_APP_OUTPUT_T crash_fault_cb);
CRASH_DATA_BUFFER* crash_dump_get_buffer(void);
uint32_t crash_dump_get_type(void);

#ifdef TOTA_CRASH_DUMP_TOOL_ENABLE
void crash_dump_get_info(void);
int32_t crash_dump_get_assert_content(uint8_t** ptr,uint32_t* bytenum, uint8_t id,uint8_t type);

void crash_dump_generate_crash_dump_rsp(uint8_t**rsp,uint8_t* len);
uint32_t crash_dump_check_param_is_vaild(uint8_t crash_id,uint8_t role);
uint8_t crash_dump_update_crash_info(uint32_t trace_len, uint32_t crash_len);
void crash_dump_record_peer_crash_info(uint8_t**peer_info,uint8_t length);
uint8_t* crash_dump_get_local_crash_info(uint16_t* len);
void crash_dump_get_crash_id(uint8_t* crash_id,uint8_t* role,uint8_t* type);
void crash_dump_set_readtype(uint8_t type);
uint32_t crash_dump_slave_get_totoal_len(uint8_t id);
uint8_t crash_dump_erase_crash_info(void);
void crash_dump_reset_assert_offset(void);

#endif


// --gsound crash dump -------------
typedef struct {
    uint32_t magicCode;  // must be CRASH_DUMP_MAGIC_CODE
    uint32_t seqnum;     // increased by 1 per crash
    uint32_t partnum;    // maximum crash log count saved in the flash
    uint32_t reserved;
    uint32_t bufpos;  // valid content in the following buf array
    uint32_t liteDumpOffset;
    uint32_t crashCode;
} CRASH_OUTPUT_BUF_HEADER_T;

/**
 * This structure MUST be a total size
 * of 0x1000
 */

struct CRASH_OUTPUT_BUF_T {
    CRASH_OUTPUT_BUF_HEADER_T hdr;
    uint8_t buf[CRASH_DUMP_FRAME_SIZE - sizeof(CRASH_OUTPUT_BUF_HEADER_T)];
};

// --gsound crash dump end  -------------

#endif
#ifdef __cplusplus
}
#endif

#endif

