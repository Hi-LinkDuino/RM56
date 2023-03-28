#include "flash.h"

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
    [HAL_PARTITION_BOOTLOADER] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "boot1",
            .partition_start_addr       = 0x0,
            .partition_length           = BOOT1_SIZE, //64k bytes
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_BOOT1] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "boot2A",
            .partition_start_addr       = BOOT1_SIZE, 
            .partition_length           = BOOT2_SIZE, //64k bytes
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_BOOT1_REDUND] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "boot2B",
            .partition_start_addr       = BOOT1_SIZE+BOOT2_SIZE, 
            .partition_length           = BOOT2_SIZE, //64k bytes
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "boot_info",
            .partition_start_addr       = BOOT1_SIZE+BOOT2_SIZE+BOOT2_SIZE, //boot information need protect
            .partition_length           = BOOTINFO_SIZE,  //32k bytes
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_RTOSA] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "RTOSA",
            .partition_start_addr       = OTA_CODE_OFFSET,
            .partition_length           = OTA_CODE_SIZE,    //12m
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_RTOSB] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "RTOSB",
            .partition_start_addr       = (OTA_CODE_OFFSET+OTA_CODE_SIZE),
            .partition_length           = OTA_CODE_SIZE,
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#if defined(LFS_FLASH_BASE_ADDR) && defined(LFS_FLASH_TOTAL_SIZE)	
    [HAL_PARTITION_LITTLEFS] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "littleFS", //for little fs module
            .partition_start_addr       = LFS_FLASH_BASE_ADDR,
            .partition_length           = LFS_FLASH_TOTAL_SIZE,
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif   	
    [HAL_PARTITION_ENV] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "factory",
            .partition_start_addr       = RESERVED_SECTION_OFFSET,
            .partition_length           = RESERVED_SECTION_SIZE,
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_ENV_REDUND] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "factory_backup",
            .partition_start_addr       = FACTORY_SECTION_OFFSET,
            .partition_length           = FACTORY_SECTION_SIZE,
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

