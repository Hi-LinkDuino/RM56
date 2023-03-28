
#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PAR_OPT_READ_POS  ( 0 )
#define PAR_OPT_WRITE_POS ( 1 )

#define PAR_OPT_READ_MASK  ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_MASK ( 0x1u << PAR_OPT_WRITE_POS )

#define PAR_OPT_READ_DIS  ( 0x0u << PAR_OPT_READ_POS )
#define PAR_OPT_READ_EN   ( 0x1u << PAR_OPT_READ_POS )
#define PAR_OPT_WRITE_DIS ( 0x0u << PAR_OPT_WRITE_POS )
#define PAR_OPT_WRITE_EN  ( 0x1u << PAR_OPT_WRITE_POS )

typedef enum {
    HAL_PARTITION_ERROR = -1,
    HAL_PARTITION_BOOTLOADER,
    HAL_PARTITION_APPLICATION,
    HAL_PARTITION_ATE,
    HAL_PARTITION_OTA_TEMP,
    HAL_PARTITION_RF_FIRMWARE,
    HAL_PARTITION_PARAMETER_1,
    HAL_PARTITION_PARAMETER_2,
    HAL_PARTITION_PARAMETER_3,
    HAL_PARTITION_PARAMETER_4,
    HAL_PARTITION_BT_FIRMWARE,
    HAL_PARTITION_SPIFFS,
    HAL_PARTITION_CUSTOM_1,
    HAL_PARTITION_CUSTOM_2,
    HAL_PARTITION_2ND_BOOT,
    HAL_PARTITION_MBINS_APP,
    HAL_PARTITION_MBINS_KERNEL,
    HAL_PARTITION_GPT,
    HAL_PARTITION_ENV,
    HAL_PARTITION_ENV_REDUND,
    HAL_PARTITION_RTOSA,
    HAL_PARTITION_RTOSB,
    HAL_PARTITION_BOOT1,
    HAL_PARTITION_BOOT1_REDUND,
    HAL_PARTITION_LITTLEFS,
    HAL_PARTITION_MAX,
    HAL_PARTITION_NONE,
} hal_partition_t;

typedef enum {
    HAL_FLASH_EMBEDDED,
    HAL_FLASH_SPI,
    HAL_FLASH_QSPI,
    HAL_FLASH_MAX,
    HAL_FLASH_NONE,
} hal_flash_t;

typedef struct {
    hal_flash_t  partition_owner;
    const char  *partition_description;
    uint32_t     partition_start_addr;
    uint32_t     partition_length;
    uint32_t     partition_options;
} hal_logic_partition_t;

typedef struct
{
    char *bin_name;
    hal_partition_t partition;
} ota_bin_partition;

enum ota_link {
    OTA_LINK_ERR = -1,
    OTA_LINK_BOOT2A,
    OTA_LINK_BOOT2B,
    OTA_LINK_RTOSA,
    OTA_LINK_RTOSB,
    OTA_LINK_MAX
};

#define BOOT_INFO_A_B_SIZE         0x4000
#define SECURE_CHECK_MAX_TIME      3
#define EXCEPTION_REBOOT_COUNT_MAX 3
#define CMU_BOOTMODE_WATCHDOG_BIT  (1 << 0)
#define OTA_BOOT_INFO_HEAD_LEN     (4 + 4)
#define OTA_BOOT_INFO_BODY_LEN     24
#define RD_DATA_LEN_MAX            200
#define RD_SIGN_LEN_MAX            384
#define MISC_HEADER_MAGIC          0x6564636A
#define FLASH_WRITE_RETRY_MAX      3
#define OTA_MALLOC_BUF_SIZE        FLASH_BLOCK_SIZE_IN_BYTES
#define MAGIC_CHECK_ADDRESS0       0x1c
#define MAGIC_CHECK_ADDRESS1       0xec
#define MAGIC_CHECK_ADDRESS2       0x57
#define MAGIC_CHECK_ADDRESS3       0xbe

#define OTA_OK                      0
#define OTA_FAILE                   1

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
    BOOTINFO_ZONE_MAX
} bootinfo_zone;

typedef enum {
    /** parameter error.           */
    ERR_PARAMETER = -1,
    /** function return error.     */
    ERR_RETURN = -2,
    /** software exception error.  */
    ERR_SWEXCEPTION = -3,
} err_enum;

typedef enum {
    /** No error, everything OK.   */
      OTA_ERR_OK         = 0,
    /** parameter error.           */
      OTA_ERR_PARAMETER  = -1,
    /** function return error.     */
      OTA_ERR_RETURN     = -2,
    /** software exception error.  */
      OTA_ERR_SWEXCEPTION  = -3,
}ota_err_enum;

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

int ota_flash_read(const hal_partition_t partition, const uint32_t addr, uint8_t *dst, const uint32_t size);

int ota_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size);

int tg_ota_upgrade_slice(const char *image_name,        uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, uint8_t *buffer);

int ota_upgrade_end(void);

int ota_flash_api_test(void);

uint32_t ota_get_partition_start_addr(hal_partition_t partition);

#ifdef __cplusplus
}
#endif

#endif /* HAL_FLASH_H */

