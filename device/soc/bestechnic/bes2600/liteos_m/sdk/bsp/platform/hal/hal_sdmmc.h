/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __HAL_SDMMC_H__
#define __HAL_SDMMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "plat_addr_map.h"

enum HAL_SDMMC_ID_T {
    HAL_SDMMC_ID_0 = 0,
#ifdef SDMMC1_BASE
    HAL_SDMMC_ID_1,
#endif

    HAL_SDMMC_ID_NUM,
};

enum HAL_SDMMC_ERR {
    HAL_SDMMC_ERR_NONE              = 0,
    HAL_SDMMC_RESET_FAIL            = 1,
    HAL_SDMMC_DMA_IN_USE            = 2,
    HAL_SDMMC_COMM_TIMEOUT          = 3,
    HAL_SDMMC_NOT_DATA_MSG          = 4,    /* EBADMSG, Not a data message */
    HAL_SDMMC_NO_SUCH_DEVICE        = 5,    /* ENODEV, No such device */
    HAL_SDMMC_INVALID_SYS_CALL      = 6,
    HAL_SDMMC_WAIT_DAT0_TIMEOUT     = 7,
    HAL_SDMMC_IO_ERR                = 8,    /* EIO, I/O error */
    HAL_SDMMC_COMM_ERR              = 9,    /* ECOMM, Communication error on send*/
    HAL_SDMMC_OUT_OF_MEMORY         = 10,   /* ENOMEM, Out of memory */
    HAL_SDMMC_NO_MEDIUM_FOUND       = 11,   /* ENOMEDIUM, No medium found */
    HAL_SDMMC_MEDIUM_TYPE_ERR       = 12,   /* EMEDIUMTYPE, Wrong medium type*/
    HAL_SDMMC_OP_NOT_PERMITTED      = 13,   /* EPERM, Operation not permitted */
    HAL_SDMMC_OP_NOT_SUPPORTED      = 14,   /* ENOTSUPP, Operation is not supported */
    HAL_SDMMC_OP_NOT_SUPPORTED_EP   = 15,   /* EOPNOTSUPP, Operation not supported on transport endpoint */
    HAL_SDMMC_RESPONSE_ERR          = 16,
    HAL_SDMMC_STATE_MACHINE_BUSY    = 17,
    HAL_SDMMC_DATA0_BUSY            = 18,
    HAL_SDMMC_CMD_SEND_TIMEOUT      = 19,
    HAL_SDMMC_CMD_START_TIMEOUT     = 20,
    HAL_SDMMC_INVALID_PARAMETER     = 21,
    HAL_SDMMC_READ_DATA_ERR         = 22,
    HAL_SDMMC_FIFO_OVERFLOW         = 23,
    HAL_SDMMC_CARD_NOT_SUPPORT      = 24,
    HAL_SDMMC_ALLOCATION_UNIT_ERR   = 25,
    HAL_SDMMC_S_A_TIMEOUT_ERR       = 26,

    //mmc_bread/mmc_berase/mmc_bwrite special code
    HAL_SDMMC_BLK_CNT_ERR           = 0xFFFFFFE0,
    HAL_SDMMC_NO_DEVICE             = 0xFFFFFFE1,
    HAL_SDMMC_SWITCH_PART_ERR       = 0xFFFFFFE2,
    HAL_SDMMC_EXCEED_MAX_CAPACITY   = 0xFFFFFFE3,
    HAL_SDMMC_SET_BLK_LEN_ERR       = 0xFFFFFFE4,
    HAL_SDMMC_READ_BLK_ERR          = 0xFFFFFFE5,
    HAL_SDMMC_ERASE_TIMEOUT         = 0xFFFFFFE6,
    HAL_SDMMC_WRITE_BLK_ERR         = 0xFFFFFFE7,
};

enum HAL_SDMMC_HOST_ERR {
    HAL_SDMMC_HOST_RESPONSE_ERR                 = 100,
    HAL_SDMMC_HOST_RESPONSE_CRC_ERR             = 101,
    HAL_SDMMC_HOST_DATA_CRC_ERR                 = 102,
    HAL_SDMMC_HOST_RESPONSE_TIMEOUT_BAR         = 103,
    HAL_SDMMC_HOST_DATA_READ_TIMEOUT_BDS        = 104,
    HAL_SDMMC_HOST_STARVATION_TIMEOUT           = 105,
    HAL_SDMMC_HOST_FIFO_ERR                     = 106,
    HAL_SDMMC_HOST_HARDWARE_LOCKED_WRITE_ERR    = 107,
    HAL_SDMMC_HOST_START_BIT_ERR                = 108,
    HAL_SDMMC_HOST_READ_END_BIT_ERR_WRITE_NOCRC = 109,
};

enum mmc_voltage {
    MMC_SIGNAL_VOLTAGE_000 = 0,
    MMC_SIGNAL_VOLTAGE_120 = 1,
    MMC_SIGNAL_VOLTAGE_180 = 2,
    MMC_SIGNAL_VOLTAGE_330 = 4,
};

struct HAL_SDMMC_CB_T {
    void (*hal_sdmmc_signal_voltage_switch)(enum mmc_voltage voltage);
    void (*hal_sdmmc_host_error)(enum HAL_SDMMC_HOST_ERR error);
    void (*hal_sdmmc_txrx_done)(void);
};

enum HAL_SDMMC_STATUS {
    HAL_SDMMC_STATUS_UN_INIT = 0,
    HAL_SDMMC_STATUS_HAS_INIT,
    HAL_SDMMC_STATUS_UNKNOWN,
};

enum HAL_SDMMC_BUS_WIDTH {
    HAL_SDMMC_BUS_WIDTH_1 = 1,
    HAL_SDMMC_BUS_WIDTH_4 = 4,
    HAL_SDMMC_BUS_WIDTH_8 = 8,
};

enum HAL_SDMMC_DEVICE_TYPE {
    HAL_SDMMC_DEVICE_TYPE_EMMC = 0,
    HAL_SDMMC_DEVICE_TYPE_SD,
};

struct HAL_SDMMC_CONFIG_T {
    bool yield;
    bool ddr_mode;
    bool volt_switch;   /* Only effective for SD card */
    uint32_t bus_speed;
    enum HAL_SDMMC_BUS_WIDTH bus_width;
    enum HAL_SDMMC_DEVICE_TYPE device_type;
};

struct HAL_SDMMC_CID_T {
    uint8_t   mid;                  // Manufacturer ID
    uint8_t   cbx;                  // Card/BGA
    uint16_t  oid;                  // OEM/Application ID
    uint8_t   pnm[6];               // Product Name
    uint8_t   prv;                  // Product Revision
    uint32_t  psn;                  // Product Serial Number
    uint16_t  mdt;                  // Manufacturing date
};

struct HAL_SDMMC_CSD_T {
    uint8_t  csd_structure;         // CSD Structure
    uint8_t  spec_vers;             // System specification version
    uint8_t  taac;                  // data read access-time - 1
    uint8_t  nsac;                  // data read access-time - 2 in CLK cycles (nsac * 100)
    uint8_t  tran_speed;            // max. data transfer rate
    uint16_t ccc;                   // card command classes
    uint8_t  read_bl_len;           // max. read data block length
    uint8_t  read_bl_partial;       // partial blocks for read allowed
    uint8_t  write_blk_misalign;    // write block misalignment
    uint8_t  read_blk_misalign;     // read block misalignment
    uint8_t  dsr_imp;               // DSR implemented
    uint32_t c_size;                // device size
    uint8_t  vdd_r_curr_min;        // max. read current @ VDD min
    uint8_t  vdd_r_curr_max;        // max. read current @ VDD max
    uint8_t  vdd_w_curr_min;        // max. write current @ VDD min
    uint8_t  vdd_w_curr_max;        // max. write current @ VDD max
    uint8_t  c_size_mult;           // device size multiplier
    uint8_t  erase_grp_size;        // erase sector size
    uint8_t  erase_grp_mult;        // erase group size
    uint8_t  erase_blk_en;          // erase single block enable
    uint8_t  sector_size;           // erase sector size
    uint8_t  wp_grp_size;           // write protect group size
    uint8_t  wp_grp_enable;         // write protect group enable
    uint8_t  default_ecc;           // manufacturer default ECC
    uint8_t  r2w_factor;            // write speed factor
    uint8_t  write_bl_len;          // max. write data block length
    uint8_t  write_bl_partial;      // partial blocks for write allowed
    uint8_t  content_prot_app;      // Content protection application
    uint8_t  file_format_grp;       // File format group
    uint8_t  copy;                  // copy flag (OTP)
    uint8_t  perm_write_protect;    // permanent write protection
    uint8_t  tmp_write_protect;     // temporary write protection
    uint8_t  file_format;           // File format
    uint8_t  ecc;                   // ECC code
};

struct HAL_SDMMC_CARD_INFO_T {
    bool cardsort;                  // card sort,0:mmc card, 1:sd card
    bool high_capacity;             // high capacity card
    uint32_t cardtype;              // card Type
    uint32_t class;                 // class of the card class
    uint32_t relcardadd;            // Relative Card Address
    uint32_t blocknbr;              // Card Capacity in blocks
    uint32_t blocksize;             // one block size in bytes
    uint32_t logblocknbr;           // Card logical Capacity in blocks
    uint32_t logllocksize;          // logical block size in bytes
    uint32_t ext_csd_rev;           // extended CSD revision
    uint32_t firmware_version_h32;  // firmware version[261:258]
    uint32_t firmware_version;      // firmware version[257:254]
    uint32_t device_version;        // device version
    uint32_t cache_size;            // emmc cache size
    uint32_t cache_ctrl;            // indicates whether the cache of emmc is open
    uint32_t cache_flush_policy;    // cache flush policy (read only)
    uint64_t capacity;
    uint64_t capacity_user;
    uint64_t capacity_boot;
    uint64_t capacity_rpmb;
    struct HAL_SDMMC_CID_T cid;
    struct HAL_SDMMC_CSD_T csd;
};

typedef void (*HAL_SDMMC_DELAY_FUNC)(uint32_t ms);
typedef void (*HAL_SDMMC_DELAY_US_FUNC)(uint32_t us);
HAL_SDMMC_DELAY_FUNC hal_sdmmc_set_delay_func(HAL_SDMMC_DELAY_FUNC new_func);
HAL_SDMMC_DELAY_US_FUNC hal_sdmmc_set_delay_us_func(HAL_SDMMC_DELAY_US_FUNC new_func);

enum HAL_SDMMC_ERR hal_sdmmc_open(enum HAL_SDMMC_ID_T id);
enum HAL_SDMMC_ERR hal_sdmmc_open_ext(enum HAL_SDMMC_ID_T id, const struct HAL_SDMMC_CONFIG_T *cfg, struct HAL_SDMMC_CB_T *callback);
void hal_sdmmc_close(enum HAL_SDMMC_ID_T id);

uint32_t hal_sdmmc_read_blocks(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *dest);
uint32_t hal_sdmmc_write_blocks(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *src);

uint32_t hal_sdmmc_read_blocks_dma(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *dest);
uint32_t hal_sdmmc_write_blocks_dma(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *src);
uint32_t hal_sdmmc_read_blocks_polling(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *dest);
uint32_t hal_sdmmc_write_blocks_polling(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count, uint8_t *src);

uint32_t hal_sdmmc_erase(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count);
uint32_t hal_sdmmc_erase_dma(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count);
uint32_t hal_sdmmc_erase_polling(enum HAL_SDMMC_ID_T id, uint32_t start_block, uint32_t block_count);

enum HAL_SDMMC_STATUS hal_sdmmc_get_status(enum HAL_SDMMC_ID_T id);
uint32_t hal_sdmmc_get_bus_speed(enum HAL_SDMMC_ID_T id);

void hal_sdmmc_dump(enum HAL_SDMMC_ID_T id);
void hal_sdmmc_info(enum HAL_SDMMC_ID_T id, uint32_t *sector_count, uint32_t *sector_size);
enum HAL_SDMMC_ERR hal_sdmmc_get_card_info(enum HAL_SDMMC_ID_T id, struct HAL_SDMMC_CARD_INFO_T *card_info);
void hal_sdmmc_dump_card_info(struct HAL_SDMMC_CARD_INFO_T *card_info);

//eMMC special function
enum HAL_SDMMC_ERR hal_sdmmc_cache_control(enum HAL_SDMMC_ID_T id, uint8_t enable_disable);//enable_disable : 0 close cache, 1 open cache
enum HAL_SDMMC_ERR hal_sdmmc_cache_flush(enum HAL_SDMMC_ID_T id);

enum HAL_SDMMC_ERR hal_sdmmc_select_card(enum HAL_SDMMC_ID_T id);
enum HAL_SDMMC_ERR hal_sdmmc_deselect_card(enum HAL_SDMMC_ID_T id);
enum HAL_SDMMC_ERR hal_sdmmc_sleep_card(enum HAL_SDMMC_ID_T id);
enum HAL_SDMMC_ERR hal_sdmmc_awake_card(enum HAL_SDMMC_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

