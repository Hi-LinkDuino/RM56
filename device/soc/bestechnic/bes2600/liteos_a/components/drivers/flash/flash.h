/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PAR_OPT_READ_POS (0)
#define PAR_OPT_WRITE_POS (1)

#define PAR_OPT_READ_MASK (0x1u << PAR_OPT_READ_POS)
#define PAR_OPT_WRITE_MASK (0x1u << PAR_OPT_WRITE_POS)

#define PAR_OPT_READ_DIS (0x0u << PAR_OPT_READ_POS)
#define PAR_OPT_READ_EN (0x1u << PAR_OPT_READ_POS)
#define PAR_OPT_WRITE_DIS (0x0u << PAR_OPT_WRITE_POS)
#define PAR_OPT_WRITE_EN (0x1u << PAR_OPT_WRITE_POS)

typedef enum {
    HAL_PARTITION_ERROR = -1,
    HAL_PARTITION_BOOTLOADER = 2,
    HAL_PARTITION_BOOT2A = 3,
    HAL_PARTITION_BOOT2B = 4,
    HAL_PARTITION_TRUSTZONEA = 5,
    HAL_PARTITION_TRUSTZONEB = 6,
    HAL_PARTITION_TZ_INFO = 7,
    HAL_PARTITION_CM33_MAIN = 8,
    HAL_PARTITION_SYSTEM_MINI = 9,
    HAL_PARTITION_RESOURCE = 10,
    HAL_PARTITION_LOG = 11,
    HAL_PARTITION_DATA = 12,
    HAL_PARTITION_MISC = 13,
    HAL_PARTITION_USERDATA = 14,
    HAL_PARTITION_ENV = 15,
    HAL_PARTITION_ENV_REDUND = 16,
    HAL_PARTITION_MAX,
} hal_partition_t;

typedef enum {
    HAL_FLASH_EMBEDDED,
    HAL_FLASH_SPI,
    HAL_FLASH_QSPI,
    HAL_FLASH_MAX,
    HAL_FLASH_NONE,
} hal_flash_t;

typedef struct {
    hal_flash_t partition_owner;
    const char *partition_description;
    uint32_t partition_start_addr;
    uint32_t partition_length;
    uint32_t partition_options;
} hal_logic_partition_t;

#define BOOT_INFO_A_ADDR 0xFD0000
#define BOOT_INFO_A_B_SIZE 0x6000
#define BOOT_INFO_B_ADDR (BOOT_INFO_A_ADDR + BOOT_INFO_A_B_SIZE)
#define SECURE_CHECK_MAX_TIME 3
#define EXCEPTION_REBOOT_COUNT_MAX 3
#define CMU_BOOTMODE_WATCHDOG_BIT (1 << 0)
#define OTA_BOOT_INFO_HEAD_LEN (4 + 4)
#define OTA_BOOT_INFO_BODY_LEN 20
#define RD_DATA_LEN_MAX 200
#define RD_SIGN_LEN_MAX 384
#define MISC_HEADER_MAGIC 0x6564636A

typedef struct {
    int rdDataLen;                /* 研发模式明文长度 */
    char rdData[RD_DATA_LEN_MAX]; /* 研发模式明文 */
    char rdSign[RD_SIGN_LEN_MAX]; /* 研发模式签名 */
} RdModeInfo;

typedef struct {
    int headerMagic;    /* Header魔术字 */
    int crcVal;         /* CRC值 */
    int len;            /* 数据长度 */
    int curbootArea;    /* 当前启动区 */
    int upgMode;        /* 升级模式 */
    int quietUpgFlg;    /* 静默升级标记 */
    int timerRebootFlg; /* 定时重启标记 */
    int bootTimes;      /* 当前分区异常启动次数 */
    RdModeInfo rdMode;  /* 研发模式 */
} MiscDataInfo;

typedef enum {
    BOOT_AREA_A = 0,
    BOOT_AREA_B = 1,
} BootAreaVal;

typedef enum {
    UPG_MODE_NORMAL = 0, /* normal模式 */
    UPG_MODE_OTA = 1,    /* OTA升级模式 */
} UpgModeVal;

typedef enum {
    NORMAL_OTA = 0,         /* 非静默升级 */
    QUIET_FIRST_STAGE = 1,  /* 静默升级第一阶段 */
    QUIET_SECOND_STAGE = 2, /* 静默升级第二阶段 */
} OtaUpgTypeVal;

typedef enum {
    NORMAL_REBOOT = 0, /* 非定时重启 */
    TIMER_REBOOT = 1,  /* 定时重启 */
} RebootTypeVal;

typedef enum {
    MISC_CRC_VALUE = 0,
    MISC_DATA_LENGTH,
    MISC_CUR_BOOT_AREA,
    MISC_UPG_MODE,
    MISC_QUIET_UPG_FLAG,
    MISC_TIMER_REBOOT_FLAG,
    MISC_BOOT_TIMES,
    MISC_RD_MODE_INFO,
    MISC_DATA_MAX,
} MiscDataType;

typedef enum {
    BOOTINFO_ORIGIN,
    BOOTINFO_BACKUP,
    BOOTINFO_INVALID
} bootinfo_block;

typedef enum {
    BOOTINFO_ZONE_0,
    BOOTINFO_ZONE_1,
    BOOTINFO_ZONE_2,
    BOOTINFO_ZONE_3,
    BOOTINFO_ZONE_4,
    BOOTINFO_ZONE_5,
    BOOTINFO_ZONE_MAX
} bootinfo_zone;

typedef enum {
    SECURE_CHECK_BOOT2A,
    SECURE_CHECK_BOOT2B,
    SECURE_CHECK_CM33_MAIN,
    SECURE_CHECK_CM33_MINI,
    SECURE_CHECK_MAX
} secure_check_zone;

typedef enum {
    LINK_BOOT2A,
    LINK_BOOT2B,
    LINK_CM33_MAIN,
    LINK_CM33_MINI,
    LINK_MAX
} link_entry;

typedef enum {
    /** No error, everything OK.   */
    ERR_OK = 0,
    /** parameter error.           */
    ERR_PARAMETER = -1,
    /** function return error.     */
    ERR_RETURN = -2,
    /** software exception error.  */
    ERR_SWEXCEPTION = -3,
} err_enum;

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition);

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size);

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len);

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len);

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len);

/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);

/**
 * Convert physical address to logic partition id and offset in partition
 *
 * @param[out]  in_partition Point to the logic partition id
 * @param[out]  off_set      Point to the offset in logic partition
 * @param[in]   addr         The physical address
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr);

int SetMiscData(MiscDataType type, const void *data, uint32_t dataLen);

int GetMiscData(MiscDataType type, void *data, uint32_t dataLen);

int ota_flash_read(const hal_partition_t partition, const uint32_t addr, uint8_t *dst, const uint32_t size);

int ota_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size);

int32_t GetFlashBadBlock(hal_partition_t *partition, uint32_t *offset, uint32_t *blockNum);

int32_t GetFlashType(int32_t *factoryID, int32_t *deviceID);

int32_t GetFlashStatisticInfo(hal_partition_t partition, uint32_t blockNum, uint32_t op, uint32_t *blockTimes);

int32_t GetFlashRewriteCycle(hal_partition_t partition, uint32_t blockNum, uint32_t op, uint32_t *times);

int32_t GetFlashBadBlockNum(hal_partition_t partition, uint32_t *blockNum);

#ifdef __cplusplus
}
#endif

#endif /* HAL_FLASH_H */
