/*
 * Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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
#include <string.h>
#include <stdlib.h>
#include "flash.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "cmsis.h"
#include "pmu.h"
#include "crc32_c.h"
#include "app_flash_api.h"
#include "hal_location.h"
//extern void watchdog_ping(void);

//#define APP_FLASH_API_USED
//#define BES_HAL_DEBUG 1

#if BES_HAL_DEBUG
#define ENTER_FUNCTION() printf("%s enter ->\n", __FUNCTION__)
#define LEAVE_FUNCTION() printf("%s <- leave\n", __FUNCTION__)
#define FOOTPRINT() printf("%s:%d\n", __FUNCTION__, __LINE__)
#define TRACEME(str, ...) printf("%s:%d " str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define ENTER_FUNCTION()
#define LEAVE_FUNCTION()
#define FOOTPRINT()
#define TRACEME(str, ...)
#endif
#define FAIL_FUNCTION() printf("%s:%d fail!\n", __FUNCTION__, __LINE__)

#define FLASH_PAGE_SIZE_IN_BYTES 256
#define OTA_MALLOC_BUF_SIZE      FLASH_BLOCK_SIZE_IN_BYTES
#define ONE_BLOCK_SIZE           (64 * 1024)
#define BLOCK_MAX_INFO           80
#define RW_MAX_BLOCK             80
#define RW_MAX_PARTITION         6
#define MAX_FLASH_OPTIONS        3
#define MAGIC_CHECK_ADDRESS0     0x1c
#define MAGIC_CHECK_ADDRESS1     0xec
#define MAGIC_CHECK_ADDRESS2     0x57
#define MAGIC_CHECK_ADDRESS3     0xbe
#define ERASE_BLOCK              4096
extern const hal_logic_partition_t g_halPartitions[];

static bootinfo_zone cur_bootinfo_zone = BOOTINFO_ZONE_MAX;
static bootinfo_zone cur_bootinfoBK_zone = BOOTINFO_ZONE_MAX;

enum FLASH_OPTIONS {
    FLASH_READ  = 0,
    FLASH_WRITE = 1,
    FLASH_ERASE = 2,
};

struct RW_BLOCK_INFO {
    bool flag;
    uint32_t blocknum;
    uint64_t count[MAX_FLASH_OPTIONS];
};

struct HAL_FLASH_BAD_INFO {
    bool flag;
    enum FLASH_OPTIONS option;
    hal_partition_t partition;
    uint32_t offset;
    uint32_t blocknum;
};

struct HAL_FLASH_RW_INFO {
    hal_partition_t partition;
    struct RW_BLOCK_INFO blockInfo[RW_MAX_BLOCK];
};

typedef struct {
    char *bin_name;
    hal_partition_t partition;
    enum NORFLASH_API_MODULE_ID_T module;
} user_writeable_flash_info;

user_writeable_flash_info user_writeable_partitions[] = {
    {"boot", HAL_PARTITION_BOOTLOADER, NORFLASH_API_MODULE_ID_CUSTOM},
    {"data", HAL_PARTITION_DATA, NORFLASH_API_MODULE_ID_CUSTOM1},
    {"log", HAL_PARTITION_LOG, NORFLASH_API_MODULE_ID_CUSTOM2},
    {"littlefs", HAL_PARTITION_RESOURCE, NORFLASH_API_MODULE_ID_CUSTOM3},
    {"secure", HAL_PARTITION_TRUSTZONEA, NORFLASH_API_MODULE_ID_CUSTOM4},
    {"misc", HAL_PARTITION_MISC, NORFLASH_API_MODULE_ID_CUSTOM5},
    {"factory", HAL_PARTITION_ENV, NORFLASH_API_MODULE_ID_CUSTOM6},
    {"factory_backup", HAL_PARTITION_ENV_REDUND, NORFLASH_API_MODULE_ID_CUSTOM7},
    {"system_mini", HAL_PARTITION_SYSTEM_MINI, NORFLASH_API_MODULE_ID_CUSTOM8},
};

struct HAL_FLASH_BAD_INFO g_flashBadInfo[BLOCK_MAX_INFO];
struct HAL_FLASH_BAD_INFO g_normalFlashBadInfo;
static bool g_rwFlashInitFlag = false;
struct HAL_FLASH_RW_INFO g_flashRwInfo[RW_MAX_PARTITION] = {
    {
        .partition = HAL_PARTITION_DATA,
    },
    {
        .partition = HAL_PARTITION_LOG,
    },
    {
        .partition = HAL_PARTITION_RESOURCE,
    },
    {
        .partition = HAL_PARTITION_MISC,
    },
    {
        .partition = HAL_PARTITION_USERDATA,
    },
};

static void SRAM_TEXT_LOC FlashOptionStructInit(void)
{
    for (size_t i = 0; i < RW_MAX_PARTITION; i++) {
        for (size_t j = 0; j < RW_MAX_BLOCK; j++) {
            g_flashRwInfo[i].blockInfo[j].blocknum = 0;
            g_flashRwInfo[i].blockInfo[j].flag = false;
            for (size_t k = 0; k < MAX_FLASH_OPTIONS; k++) {
                g_flashRwInfo[i].blockInfo[j].count[k] = 0;
            }
        }
    }

    g_rwFlashInitFlag = true;
}

#ifndef APP_FLASH_API_USED
int SRAM_TEXT_LOC bes_check_user_write_flash_addr(const uint32_t addr, const uint32_t size)
{
    hal_logic_partition_t info;
    int32_t ret = -1;
    int32_t index = 0;

    for (index = 0; index < sizeof(user_writeable_partitions) / sizeof(user_writeable_partitions[0]); index++) {
        if (hal_flash_info_get(user_writeable_partitions[index].partition, &info) != 0) {
            break;
        }

        if ((info.partition_start_addr <= addr) && ((addr + size) <= (info.partition_start_addr + info.partition_length))) {
            ret = 0;
            break;
        }
    }
    return ret;
}
#else
int SRAM_TEXT_LOC bes_check_user_write_flash_addr(const uint32_t addr, const uint32_t size,
	enum NORFLASH_API_MODULE_ID_T *module, uint32_t *module_offset)
{
    hal_logic_partition_t info;
    int32_t ret = -1;
    int32_t index = 0;

    for (index = 0; index < sizeof(user_writeable_partitions) / sizeof(user_writeable_partitions[0]); index++) {
        if (hal_flash_info_get(user_writeable_partitions[index].partition, &info) != 0) {
            TRACE(0, "hal_flash_info_get fail");
            continue;
        }

        if ((info.partition_start_addr <= addr) && ((addr + size) <= (info.partition_start_addr + info.partition_length))) {
            if (module)
                *module = user_writeable_partitions[index].module;
            if (module_offset)
                *module_offset = addr - info.partition_start_addr;
            ret = 0;
            break;
        }
    }

    return ret;
}
#endif

int SRAM_TEXT_LOC CheckUseAccessable(unsigned char *partitionName, unsigned int *partition, hal_logic_partition_t *info)
{
    int32_t index;
    int32_t ret = -1;

    if ((partitionName == NULL) || (partition == NULL) || (info == NULL)) {
        return -1;
    }

    for (index = 1; index < sizeof(user_writeable_partitions) / sizeof(user_writeable_partitions[0]); index++) {
        if (strcmp(user_writeable_partitions[index].bin_name, partitionName) != 0) {
            continue;
        }
        if (hal_flash_info_get(user_writeable_partitions[index].partition, info) != 0) {
            TRACE(0, "hal_flash_info_get fail");
            return ret;
        } else {
            *partition = user_writeable_partitions[index].partition;
            return 0;
        }
    }

    return ret;
}

int SRAM_TEXT_LOC GetUseAccessableName(char **partitionName, unsigned int index)
{
    if (partitionName == NULL) {
        return -1;
    }

    if (index < sizeof(user_writeable_partitions) / sizeof(user_writeable_partitions[0])) {
        *partitionName = user_writeable_partitions[index].bin_name;
        return 0;
    }

    return -1;
}

osMutexId FlashMutex = NULL;
osMutexDef_t os_mutex_def_flash;
static void SRAM_TEXT_LOC FlashosMutexWait(void)
{
    if (FlashMutex == NULL) {
        FlashMutex = osMutexCreate(&os_mutex_def_flash);
    }

    osMutexWait(FlashMutex, osWaitForever);
}

void *my_memcpy(void *dst, void *src, size_t num)
{
    int offset1 = (4 - ((uint32_t)dst & 3)) & 3;
    int offset2 = (4 - ((uint32_t)src & 3)) & 3;

    if (offset1 != offset2) {
        return memcpy(dst, src, num);
    }

    int wordnum = num > offset1 ? (num - offset1) / 8 : 0;
    int slice = num > offset1 ? (num - offset1) % 8 : 0;
    char *pdst = (char *)dst;
    char *psrc = (char *)src;
    long long *pintsrc = NULL;
    long long *pintdst = NULL;

    while (offset1--) {
        *pdst++ = *psrc++;
    }

    pintdst = (long long *)pdst;
    pintsrc = (long long *)psrc;
    while (wordnum--) {
        *pintdst++ = *pintsrc++;
    }

    pdst = (char *)pintdst;
    psrc = (char *)pintsrc;
    while (slice--) {
        *pdst++ = *psrc++;
    }

    return dst;
}

#ifdef APP_FLASH_API_USED
static bool g_flashApiInited = false;
static void SRAM_TEXT_LOC flash_api_init(void)
{
    hal_logic_partition_t info;
    int32_t index = 0;

    if (g_flashApiInited == true) {
        return ;
    } else {
        g_flashApiInited = true;
    }

    for (index = 0; index < sizeof(user_writeable_partitions) / sizeof(user_writeable_partitions[0]); index++) {
        if (hal_flash_info_get(user_writeable_partitions[index].partition, &info) != 0) {
            TRACE(0, "hal_flash_info_get fail");
            continue;
        }
        app_flash_register_module(user_writeable_partitions[index].module,
            TO_FLASH_NC_ADDR(info.partition_start_addr), info.partition_length, 0);
    }
}
#endif

#ifndef APP_FLASH_API_USED
int SRAM_TEXT_LOC flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;

    if (dst == NULL) {
        ret = -1;
        goto RETURN;
    }

    FlashosMutexWait();
    pmu_flash_write_config();
    lock = int_lock_global();
    ret = NORFLASH_API_WRAP(hal_norflash_read)(HAL_FLASH_ID_0, addr, dst, size);
    int_unlock_global(lock);
    pmu_flash_read_config();
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

int SRAM_TEXT_LOC flash_write(const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = 0;
    uint32_t lock;

    ret = bes_check_user_write_flash_addr(addr, size);
    if (ret) {
        return ret;
    }

    FlashosMutexWait();
    pmu_flash_write_config();
    lock = int_lock_global();
    ret = NORFLASH_API_WRAP(hal_norflash_write)(HAL_FLASH_ID_0, addr, src, size);
    int_unlock_global(lock);
    pmu_flash_read_config();
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    osMutexRelease(FlashMutex);

    return ret;
}

int SRAM_TEXT_LOC flash_erase(const uint32_t addr, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;

    ret = bes_check_user_write_flash_addr(addr, size);

    if (ret) {
        return ret;
    }

    FlashosMutexWait();
    pmu_flash_write_config();
    lock = int_lock_global();
    ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, addr, size);
    int_unlock_global(lock);

    pmu_flash_read_config();
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    osMutexRelease(FlashMutex);

    return ret;
}
#else
int SRAM_TEXT_LOC flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    enum NORFLASH_API_MODULE_ID_T module;
    uint32_t module_offset;

    ret = bes_check_user_write_flash_addr(addr, size, &module, &module_offset);
    if (ret) {
        TRACE(0, "fatal error %s %d, addr:0x%x, size:0x%x", __func__, __LINE__, addr, size);
        return ret;
    }

    if (dst == NULL) {
        ret = -1;
        goto RETURN;
    }

    flash_api_init();
    FlashosMutexWait();
    app_flash_read(module, module_offset, dst, size);
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

int SRAM_TEXT_LOC flash_write(const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = 0;
    enum NORFLASH_API_MODULE_ID_T module;
    uint32_t module_offset;

    ret = bes_check_user_write_flash_addr(addr, size, &module, &module_offset);
    if (ret) {
        return ret;
    }

    if (src == NULL) {
        return -1;
    }

    flash_api_init();
    FlashosMutexWait();
    app_flash_program(module, module_offset, src, size, false);
    app_flash_flush_pending_op(module, NORFLASH_API_ALL);
    osMutexRelease(FlashMutex);

    return ret;
}

int SRAM_TEXT_LOC flash_erase(const uint32_t addr, const uint32_t size)
{
    int ret = 0;
    enum NORFLASH_API_MODULE_ID_T module;
    uint32_t module_offset;

    if ((addr % ERASE_BLOCK != 0) || (size % ERASE_BLOCK != 0)) {
        TRACE(0, "%s error, addr:0x%x size:0x%x", __func__, addr, size);
        return -1;
    }

    ret = bes_check_user_write_flash_addr(addr, size, &module, &module_offset);
    if (ret) {
        return ret;
    }

    flash_api_init();
    FlashosMutexWait();
    app_flash_erase(module, module_offset, size);
    osMutexRelease(FlashMutex);

    return ret;
}
#endif

static int32_t SRAM_TEXT_LOC SetFlashOptionInfo(hal_partition_t partition, uint32_t size, uint32_t option)
{
    uint32_t blockNum;

    if (((partition != HAL_PARTITION_DATA) && (partition != HAL_PARTITION_LOG) && (partition != HAL_PARTITION_RESOURCE)) || (option >= MAX_FLASH_OPTIONS)) {
        TRACE(0, "%s---%d----%d-----%d\r\n", __FUNCTION__, partition, size, option);
        return -1;
    }

    if (!g_rwFlashInitFlag) {
        FlashOptionStructInit();
    }

    blockNum = (size / ONE_BLOCK_SIZE);

    for (size_t i = 0; i < RW_MAX_PARTITION; i++) {
        if (g_flashRwInfo[i].partition == partition) {
            for (size_t j = 0; j < RW_MAX_BLOCK; j++) {
                if (g_flashRwInfo[i].blockInfo[j].flag) {
                    if (g_flashRwInfo[i].blockInfo[j].blocknum == blockNum) {
                        g_flashRwInfo[i].blockInfo[j].count[option]++;
                        break;
                    } else {
                        continue;
                    }
                } else {
                    g_flashRwInfo[i].blockInfo[j].blocknum = blockNum;
                    g_flashRwInfo[i].blockInfo[j].count[option]++;
                    g_flashRwInfo[i].blockInfo[j].flag = true;
                    break;
                }
            }
        }
    }

    return 0;
}

static int32_t SRAM_TEXT_LOC SetFlashBadOptionInfo(hal_partition_t partition, uint32_t size, uint32_t option)
{
    if (((partition != HAL_PARTITION_DATA) && (partition != HAL_PARTITION_LOG) && (partition != HAL_PARTITION_RESOURCE)) || (option >= MAX_FLASH_OPTIONS)) {
        return -1;
    }

    struct HAL_FLASH_BAD_INFO *flashBadInfo = &g_normalFlashBadInfo;
    flashBadInfo->option = option;
    flashBadInfo->partition = partition;

    if ((size % ONE_BLOCK_SIZE) > 0) {
        flashBadInfo->blocknum = (size / ONE_BLOCK_SIZE) + 1;
    } else {
        flashBadInfo->blocknum = (size / ONE_BLOCK_SIZE);
    }

    for (size_t i = 0; i < BLOCK_MAX_INFO; i++) {
        if (g_flashBadInfo[i].flag) {
            continue;
        }

        g_flashBadInfo[i] = g_normalFlashBadInfo;
        g_flashBadInfo[i].flag = true;
    }

    return 0;
}

static struct HAL_FLASH_BAD_INFO *GetFlashBadOptionInfo(void)
{
    return &g_normalFlashBadInfo;
}

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t SRAM_TEXT_LOC hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    if (in_partition >= HAL_PARTITION_MAX || partition == NULL) {
        return -1;
    }

    ENTER_FUNCTION();
    logic_partition = (hal_logic_partition_t *)&g_halPartitions[in_partition];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));
    LEAVE_FUNCTION();

    return 0;
}

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
int32_t SRAM_TEXT_LOC hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;
    int32_t ret = 0;

    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACE(0, "hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + off_set;
    partition_end = info.partition_start_addr + info.partition_length;
    if ((start_addr + size) > partition_end) {
        TRACE(0, "flash over partition, off_set:0x%x size:0x%x\n", off_set, size);
        ret = -1;
        goto RETURN;
    }
    ret = SetFlashOptionInfo(in_partition, start_addr, FLASH_ERASE);
    if (ret < 0) {
        TRACE(0, "SetFlashOptionInfo FAIL\r\n");
    }

    ret = flash_erase(start_addr, size);
    if (!ret) {
        goto RETURN;
    } else {
        SetFlashBadOptionInfo(in_partition, start_addr, FLASH_ERASE);
    }

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

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
int32_t SRAM_TEXT_LOC hal_flash_write(hal_partition_t in_partition, uint32_t off_set, const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;

    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACE(0, "hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + off_set;
    partition_end = info.partition_start_addr + info.partition_length;
    if (start_addr >= partition_end) {
        TRACE(0, "flash over partition\r\n");
        ret = -1;
        goto RETURN;
    }

    if ((start_addr + in_buf_len) > partition_end) {
        TRACE(0, "flash over partition, off_set:0x%x in_buf_len:0x%x\r\n", off_set, in_buf_len);
        ret = -1;
        goto RETURN;
    }

    ret = SetFlashOptionInfo(in_partition, start_addr, FLASH_WRITE);
    if (ret < 0) {
        TRACE(0, "SetFlashOptionInfo FAIL\r\n");
    }

    ret = flash_write(start_addr, in_buf, in_buf_len);
    if (ret) {
        SetFlashBadOptionInfo(in_partition, start_addr, FLASH_WRITE);
    }

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

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
int32_t SRAM_TEXT_LOC hal_flash_erase_write(hal_partition_t in_partition, uint32_t off_set, const void *in_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;

    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACE(0, "hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + off_set;
    partition_end = info.partition_start_addr + info.partition_length;

    if (start_addr >= partition_end) {
        TRACE(0, "flash over partition\r\n");
        ret = -1;
        goto RETURN;
    }

    if ((start_addr + in_buf_len) > partition_end) {
        TRACE(0, "flash over partition, off_set:0x%x in_buf_len:0x%x\r\n", off_set, in_buf_len);
        ret = -1;
        goto RETURN;
    }

    ret = SetFlashOptionInfo(in_partition, start_addr, FLASH_ERASE);
    if (ret < 0) {
        TRACE(0, "SetFlashOptionInfo FAIL\r\n");
    }

    ret = flash_erase(start_addr, in_buf_len);
    if (ret) {
        TRACE(0, "flash erase fail\r\n");
        ret = -1;
        SetFlashBadOptionInfo(in_partition, start_addr, FLASH_ERASE);
        goto RETURN;
    }

    ret = SetFlashOptionInfo(in_partition, start_addr, FLASH_WRITE);
    if (ret < 0) {
        TRACE(0, "SetFlashOptionInfo FAIL\r\n");
    }

    ret = flash_write(start_addr, in_buf, in_buf_len);
    if (ret) {
        SetFlashBadOptionInfo(in_partition, start_addr, FLASH_WRITE);
    }

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

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
int32_t SRAM_TEXT_LOC hal_flash_read(hal_partition_t in_partition, uint32_t off_set, void *out_buf, uint32_t in_buf_len)
{
    int32_t ret = 0;
    uint32_t start_addr;
    uint32_t partition_end;
    hal_logic_partition_t info;

    ENTER_FUNCTION();
    if (hal_flash_info_get(in_partition, &info) != 0) {
        TRACE(0, "hal_flash_info_get fail\n");
        ret = -1;
        goto RETURN;
    }

    start_addr = info.partition_start_addr + off_set;
    partition_end = info.partition_start_addr + info.partition_length;
    if (start_addr >= partition_end) {
        TRACE(0, "flash over partition\r\n");
        ret = -1;
        goto RETURN;
    }

    if ((start_addr + in_buf_len) > partition_end) {
        TRACE(0, "flash over partition, off_set:0x%x in_buf_len:0x%x\r\n", off_set, in_buf_len);
        ret = -1;
        goto RETURN;
    }

    ret = SetFlashOptionInfo(in_partition, start_addr, FLASH_READ);
    if (ret < 0) {
        TRACE(0, "SetFlashOptionInfo FAIL\r\n");
    }

    ret = flash_read(start_addr, out_buf, in_buf_len);
    if (ret) {
        SetFlashBadOptionInfo(in_partition, start_addr, FLASH_READ);
    }

RETURN:
    LEAVE_FUNCTION();
    return ret;
}

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
int32_t SRAM_TEXT_LOC hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    FAIL_FUNCTION();
    return 0;
}

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
int32_t SRAM_TEXT_LOC hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    FAIL_FUNCTION();
    return 0;
}

/**
 * Convert physical address to logic partition id and offset in partition
 *
 * @param[out]  in_partition Point to the logic partition id
 * @param[out]  off_set      Point to the offset in logic partition
 * @param[in]   addr         The physical address
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t SRAM_TEXT_LOC hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr)
{
    FAIL_FUNCTION();
    return 0;
}

int SRAM_TEXT_LOC ota_set_bootinfo_zone_num(bootinfo_block block, bootinfo_zone zone)
{
    if (block >= BOOTINFO_INVALID || zone >= BOOTINFO_ZONE_MAX) {
        return BOOTINFO_ZONE_MAX;
    }

    if (block == BOOTINFO_ORIGIN) {
        cur_bootinfo_zone = zone;
    } else if (block == BOOTINFO_BACKUP) {
        cur_bootinfoBK_zone = zone;
    }

    return 0;
}

bootinfo_zone SRAM_TEXT_LOC ota_get_bootinfo_zone_num(bootinfo_block block)
{
    if (block == BOOTINFO_ORIGIN) {
        return cur_bootinfo_zone;
    } else if (block == BOOTINFO_BACKUP) {
        return cur_bootinfoBK_zone;
    } else {
        return BOOTINFO_ZONE_MAX;
    }
}

int SRAM_TEXT_LOC ota_set_bootinfo_crc32value(MiscDataInfo *info)
{
    uint32_t crc32_value = 0;
    uint8_t *flash_pointer = NULL;

    flash_pointer = (uint8_t *)(info);
    crc32_value = crc32_c(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->len);
    info->crcVal = crc32_value;

    return ERR_OK;
}

uint32_t SRAM_TEXT_LOC ota_get_partition_start_addr(hal_partition_t partition)
{
    int ret = 0;
    hal_logic_partition_t partition_info;

    ret = hal_flash_info_get(partition, &partition_info);
    if (ret) {
        return ERR_RETURN;
    } else {
        return partition_info.partition_start_addr;
    }
}

int SRAM_TEXT_LOC ota_write_bootinfo_to_flash(MiscDataInfo *info, bootinfo_block block, bootinfo_zone zone)
{
    int ret = 0;
    uint32_t lock = 0;
    bootinfo_zone cur_zone = 0, next_zone = 0;
    uint32_t cur_addr = 0;
    uint32_t next_addr = 0;
    uint8_t buffer[FLASH_SECTOR_SIZE_IN_BYTES] = {0};
    uint32_t bootinfo_a_addr;
    uint32_t bootinfo_b_addr;

    if (block >= BOOTINFO_INVALID) {
        return ERR_PARAMETER;
    }

    bootinfo_a_addr = ota_get_partition_start_addr(HAL_PARTITION_MISC);
    if (bootinfo_a_addr == ERR_RETURN) {
        return ERR_RETURN;
    }
    bootinfo_b_addr = bootinfo_a_addr + BOOT_INFO_A_B_SIZE;

    ota_set_bootinfo_crc32value(info);

    cur_zone = zone;
    next_zone = (cur_zone + 1) % (BOOT_INFO_A_B_SIZE / FLASH_SECTOR_SIZE_IN_BYTES);
    if (block == BOOTINFO_ORIGIN) {
        cur_addr = bootinfo_a_addr + cur_zone * FLASH_SECTOR_SIZE_IN_BYTES;
        next_addr = bootinfo_a_addr + next_zone * FLASH_SECTOR_SIZE_IN_BYTES;
    } else if (block == BOOTINFO_BACKUP) {
        cur_addr = bootinfo_b_addr + cur_zone * FLASH_SECTOR_SIZE_IN_BYTES;
        next_addr = bootinfo_b_addr + next_zone * FLASH_SECTOR_SIZE_IN_BYTES;
    }

    pmu_flash_write_config();
    memcpy(buffer, (uint8_t *)info, sizeof(MiscDataInfo));

    //erase current sector
    lock = int_lock_global();
    norflash_api_flush_all();
    ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, cur_addr, FLASH_SECTOR_SIZE_IN_BYTES);
    int_unlock_global(lock);
    if (ret != HAL_NORFLASH_OK) {
        SetFlashBadOptionInfo(HAL_PARTITION_MISC, cur_addr, FLASH_ERASE);
        goto end;
    }

    // erase next sector
    lock = int_lock_global();
    norflash_api_flush_all();
    ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, next_addr, FLASH_SECTOR_SIZE_IN_BYTES);
    int_unlock_global(lock);
    if (ret != HAL_NORFLASH_OK) {
        SetFlashBadOptionInfo(HAL_PARTITION_MISC, next_addr, FLASH_ERASE);
        goto end;
    }

    // write next sector
    lock = int_lock_global();
    norflash_api_flush_all();
    ret = NORFLASH_API_WRAP(hal_norflash_write)(HAL_FLASH_ID_0, next_addr, buffer, FLASH_SECTOR_SIZE_IN_BYTES);
    int_unlock_global(lock);
    if (ret != HAL_NORFLASH_OK) {
        SetFlashBadOptionInfo(HAL_PARTITION_MISC, next_addr, FLASH_WRITE);
        goto end;
    }

    // update bootinfo current zone number
    ret = ota_set_bootinfo_zone_num(block, next_zone);

end:
    pmu_flash_read_config();
    return ret;
}

int SRAM_TEXT_LOC ota_check_bootinfo_block(bootinfo_block block)
{
    MiscDataInfo *info = NULL;
    uint32_t crc32_value = 0;
    uint8_t *flash_pointer = NULL;
    uint32_t bootinfo_a_addr;
    uint32_t bootinfo_b_addr;

    if (block >= BOOTINFO_INVALID) {
        return ERR_PARAMETER;
    }

    bootinfo_a_addr = ota_get_partition_start_addr(HAL_PARTITION_MISC);
    if (bootinfo_a_addr == ERR_RETURN) {
        return ERR_RETURN;
    }
    bootinfo_b_addr = bootinfo_a_addr + BOOT_INFO_A_B_SIZE;

    for (bootinfo_zone num = BOOTINFO_ZONE_0; num < BOOTINFO_ZONE_MAX; num++) {
        if (block == BOOTINFO_ORIGIN) {
            info = (MiscDataInfo *)(FLASH_NC_BASE + bootinfo_a_addr + num * FLASH_SECTOR_SIZE_IN_BYTES);
        } else if (block == BOOTINFO_BACKUP) {
            info = (MiscDataInfo *)(FLASH_NC_BASE + bootinfo_b_addr + num * FLASH_SECTOR_SIZE_IN_BYTES);
        }

        if (info->headerMagic != MISC_HEADER_MAGIC) {
            continue;
        }

        flash_pointer = (uint8_t *)(info);
        crc32_value = crc32_c(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->len);
        if (crc32_value == info->crcVal) {
            if (block == BOOTINFO_ORIGIN) {
                if (ota_get_bootinfo_zone_num(BOOTINFO_ORIGIN) != num) {
                    ota_set_bootinfo_zone_num(BOOTINFO_ORIGIN, num);
                }
                if (ota_get_bootinfo_zone_num(BOOTINFO_BACKUP) != num) {
                    ota_set_bootinfo_zone_num(BOOTINFO_BACKUP, num);
                }
            } else if (block == BOOTINFO_BACKUP) {
                // copy bootinfo from BACKUP zone to ORIGIN zone
                ota_write_bootinfo_to_flash(info, BOOTINFO_ORIGIN,
                                            (num + BOOT_INFO_A_B_SIZE / FLASH_SECTOR_SIZE_IN_BYTES - 1) %
                                                (BOOT_INFO_A_B_SIZE / FLASH_SECTOR_SIZE_IN_BYTES));

                if (ota_get_bootinfo_zone_num(BOOTINFO_ORIGIN) != num) {
                    ota_set_bootinfo_zone_num(BOOTINFO_ORIGIN, num);
                }
                if (ota_get_bootinfo_zone_num(BOOTINFO_BACKUP) != num) {
                    ota_set_bootinfo_zone_num(BOOTINFO_BACKUP, num);
                }
            }
            return 0;
        }
    }

    return ERR_PARAMETER;
}

int SRAM_TEXT_LOC ota_set_bootinfo_to_zoneAB(MiscDataInfo *info)
{
    int ret = 0;

    ret = ota_write_bootinfo_to_flash(info, BOOTINFO_ORIGIN, ota_get_bootinfo_zone_num(BOOTINFO_ORIGIN));
    if (ret) {
        TRACE(0, "ota_write_bootinfo_to_flash fail.%s %d", __func__, __LINE__);
        return ret;
    }

    ret = ota_write_bootinfo_to_flash(info, BOOTINFO_BACKUP, ota_get_bootinfo_zone_num(BOOTINFO_BACKUP));
    if (ret) {
        TRACE(0, "ota_write_bootinfo_to_flash fail.%s %d", __func__, __LINE__);
    }

    return ret;
}

void SRAM_TEXT_LOC ota_set_default_bootinfo_to_zoneAB(void)
{
    MiscDataInfo info;
    int ret = 0;

    info.headerMagic = MISC_HEADER_MAGIC;
    info.len = OTA_BOOT_INFO_BODY_LEN;
    info.curbootArea = BOOT_AREA_A;
    info.upgMode = UPG_MODE_NORMAL;
    info.quietUpgFlg = NORMAL_OTA;
    info.timerRebootFlg = NORMAL_REBOOT;
    info.bootTimes = 0;
    info.rdMode.rdDataLen = 0;

    memset(&info.rdMode.rdData[0], 0, RD_DATA_LEN_MAX);
    memset(&info.rdMode.rdSign[0], 0, RD_SIGN_LEN_MAX);

    ota_set_bootinfo_zone_num(BOOTINFO_ORIGIN, BOOTINFO_ZONE_3);
    ota_set_bootinfo_zone_num(BOOTINFO_BACKUP, BOOTINFO_ZONE_3);

    ret = ota_set_bootinfo_to_zoneAB(&info);
    if (ret) {
        return;
    }
}

bootinfo_block SRAM_TEXT_LOC ota_get_valid_bootinfo_block(void)
{
    int ret = -1;

    ret = ota_check_bootinfo_block(BOOTINFO_ORIGIN);
    if (!ret) {
        return BOOTINFO_ORIGIN;
    } else {
        ret = ota_check_bootinfo_block(BOOTINFO_BACKUP);
        if (!ret) {
            return BOOTINFO_ORIGIN;
        } else {
            // first boot or both boot info bad, set both to default.
            ota_set_default_bootinfo_to_zoneAB();
            return BOOTINFO_ORIGIN;
        }
    }
}

int SRAM_TEXT_LOC ota_get_bootinfo(MiscDataInfo *info, bootinfo_block block, bootinfo_zone zone)
{
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    uint32_t bootinfo_a_addr;
    uint32_t bootinfo_b_addr;

    if (block >= BOOTINFO_INVALID) {
        return ERR_PARAMETER;
    }

    if (zone >= BOOTINFO_ZONE_MAX) {
        return ERR_PARAMETER;
    }

    bootinfo_a_addr = ota_get_partition_start_addr(HAL_PARTITION_MISC);
    if (bootinfo_a_addr == ERR_RETURN) {
        return ERR_RETURN;
    }
    bootinfo_b_addr = bootinfo_a_addr + BOOT_INFO_A_B_SIZE;

    if (block == BOOTINFO_ORIGIN) {
        start_addr = bootinfo_a_addr + zone * FLASH_SECTOR_SIZE_IN_BYTES;
    } else if (block == BOOTINFO_BACKUP) {
        start_addr = bootinfo_b_addr + zone * FLASH_SECTOR_SIZE_IN_BYTES;
    }

    pmu_flash_write_config();
    lock = int_lock_global();
    NORFLASH_API_WRAP(hal_norflash_read)(HAL_FLASH_ID_0, start_addr, (uint8_t *)info, sizeof(MiscDataInfo));
    int_unlock_global(lock);
    pmu_flash_read_config();

    return ERR_OK;
}

int SRAM_TEXT_LOC SetMiscData(MiscDataType type, const void *data, uint32_t dataLen)
{
    MiscDataInfo ctrl;
    int ret = 0;
    int len = 0;
    bootinfo_block block = BOOTINFO_INVALID;

    FlashosMutexWait();
    block = ota_get_valid_bootinfo_block();
    ret = ota_get_bootinfo(&ctrl, block, ota_get_bootinfo_zone_num(block));
    if (ret) {
        osMutexRelease(FlashMutex);
        return ret;
    }

    switch (type) {
    case MISC_CUR_BOOT_AREA:
        memcpy(&ctrl.curbootArea, data, dataLen);
        break;
    case MISC_UPG_MODE:
        memcpy(&ctrl.upgMode, data, dataLen);
        break;
    case MISC_QUIET_UPG_FLAG:
        memcpy(&ctrl.quietUpgFlg, data, dataLen);
        break;
    case MISC_TIMER_REBOOT_FLAG:
        memcpy(&ctrl.timerRebootFlg, data, dataLen);
        break;
    case MISC_BOOT_TIMES:
        memcpy(&ctrl.bootTimes, data, dataLen);
        break;
    case MISC_RD_MODE_INFO:
        memset((char *)&ctrl.rdMode, 0, sizeof(RdModeInfo));
        len = (dataLen > sizeof(RdModeInfo)) ? sizeof(RdModeInfo) : dataLen;
        memcpy((char *)&ctrl.rdMode, data, len);
        if (ctrl.len < len) {
            ctrl.len += len;
        }
        break;
    default:
        osMutexRelease(FlashMutex);
        return ERR_PARAMETER;
    }

    ret = ota_set_bootinfo_to_zoneAB(&ctrl);
    osMutexRelease(FlashMutex);

    return ret;
}

int SRAM_TEXT_LOC GetMiscData(MiscDataType type, void *data, uint32_t dataLen)
{
    MiscDataInfo ctrl;
    int ret = 0;
    int len = 0;
    bootinfo_block block = BOOTINFO_INVALID;

    if (type >= MISC_DATA_MAX) {
        return ERR_PARAMETER;
    }

    FlashosMutexWait();
    block = ota_get_valid_bootinfo_block();
    ret = ota_get_bootinfo(&ctrl, block, ota_get_bootinfo_zone_num(block));
    if (ret) {
        osMutexRelease(FlashMutex);
        return ret;
    }
    osMutexRelease(FlashMutex);

    switch (type) {
    case MISC_CRC_VALUE:
        memcpy(data, &ctrl.crcVal, dataLen);
        break;
    case MISC_DATA_LENGTH:
        memcpy(data, &ctrl.len, dataLen);
        break;
    case MISC_CUR_BOOT_AREA:
        memcpy(data, &ctrl.curbootArea, dataLen);
        break;
    case MISC_UPG_MODE:
        memcpy(data, &ctrl.upgMode, dataLen);
        break;
    case MISC_QUIET_UPG_FLAG:
        memcpy(data, &ctrl.quietUpgFlg, dataLen);
        break;
    case MISC_TIMER_REBOOT_FLAG:
        memcpy(data, &ctrl.timerRebootFlg, dataLen);
        break;
    case MISC_BOOT_TIMES:
        memcpy(data, &ctrl.bootTimes, dataLen);
        break;
    case MISC_RD_MODE_INFO:
        len = (dataLen > sizeof(RdModeInfo)) ? sizeof(RdModeInfo) : dataLen;
        memcpy(data, (char *)&ctrl.rdMode, len);
        break;
    default:
        return ERR_PARAMETER;
    }

    return ERR_OK;
}

int SRAM_TEXT_LOC ota_partition_check_magic(const hal_partition_t partition, const uint32_t addr)
{
    return 0;
}

int SRAM_TEXT_LOC ota_hal_read_flash(const hal_partition_t partition, const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = -1;

    if (partition == HAL_PARTITION_BOOT2A) {
        //ret = NORFLASH_API_WRAP(hal_norflash_read_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0, addr, dst, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        ret = NORFLASH_API_WRAP(hal_norflash_read)(HAL_FLASH_ID_0, addr, dst, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else if (partition == HAL_PARTITION_RTOSA) {
        //ret = NORFLASH_API_WRAP(hal_norflash_read_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2, addr, dst, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        ret = NORFLASH_API_WRAP(hal_norflash_read)(HAL_FLASH_ID_0, addr, dst, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else {
        ret = NORFLASH_API_WRAP(hal_norflash_read)(HAL_FLASH_ID_0, addr, dst, size);
    }

    return ret;
}

int SRAM_TEXT_LOC ota_flash_read(const hal_partition_t partition, const uint32_t addr, uint8_t *dst, const uint32_t size)
{
    int ret = 0;
    uint32_t lock = 0;
    hal_logic_partition_t partitionInfo;
    uint32_t flash_offset = 0;

    if (NULL == dst) {
        ret = -1;
        goto RETURN;
    }

    ret = hal_flash_info_get(partition, &partitionInfo);
    if (ret) {
        return ERR_RETURN;
    }
    flash_offset = addr + partitionInfo.partition_start_addr;

    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    FlashosMutexWait();
    pmu_flash_write_config();
    lock = int_lock_global();
    ret = ota_hal_read_flash(partition, flash_offset, dst, size);
    int_unlock_global(lock);
    pmu_flash_read_config();
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

int SRAM_TEXT_LOC ota_hal_erase_flash(const hal_partition_t partition, const uint32_t addr, const uint32_t size)
{
    int ret = -1;

    if (partition == HAL_PARTITION_BOOT2A) {
        //ret = NORFLASH_API_WRAP(hal_norflash_erase_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0, addr, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, addr, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else if (partition == HAL_PARTITION_RTOSA) {
        //ret = NORFLASH_API_WRAP(hal_norflash_erase_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2, addr, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, addr, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else {
        ret = NORFLASH_API_WRAP(hal_norflash_erase)(HAL_FLASH_ID_0, addr, size);
    }

    return ret;
}

int SRAM_TEXT_LOC ota_hal_write_flash(const hal_partition_t partition, const uint32_t addr, uint8_t *src, const uint32_t size)
{
    int ret = -1;

    if (partition == HAL_PARTITION_BOOT2A) {
        //ret = NORFLASH_API_WRAP(hal_norflash_write_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0, addr, src, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        ret = NORFLASH_API_WRAP(hal_norflash_write)(HAL_FLASH_ID_0, addr, src, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_0);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else if (partition == HAL_PARTITION_RTOSA) {
        //ret = NORFLASH_API_WRAP(hal_norflash_write_remap)(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2, addr, src, size);
        hal_norflash_disable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        ret = NORFLASH_API_WRAP(hal_norflash_write)(HAL_FLASH_ID_0, addr, src, size);
        hal_norflash_enable_remap(HAL_FLASH_ID_0, HAL_NORFLASH_REMAP_ID_2);
        if (ret) {
            TRACE(0, "%s %d hal_norflash_read_remap error!", __func__, __LINE__);
            return -1;
        }
    } else {
        ret = NORFLASH_API_WRAP(hal_norflash_write)(HAL_FLASH_ID_0, addr, src, size);
    }

    return ret;
}

int SRAM_TEXT_LOC ota_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size)
{
    int ret = -1;
    uint32_t lock;
    uint32_t num;
    uint32_t left_len = 0;
    uint32_t align_len = 0;
    uint32_t fill_len = 0;
    uint32_t flash_offset = 0;
    uint32_t lengthToBurn = size;
    uint8_t *ptrSource = (uint8_t *)src;
    volatile char *flashPointer = NULL;
    uint8_t *buf = NULL;
    hal_logic_partition_t partition_info;
    uint8_t ota_partition[] = {
        HAL_PARTITION_BOOT2A, HAL_PARTITION_BOOT2B, HAL_PARTITION_RTOSA, HAL_PARTITION_RTOSB
    };
    uint8_t read_buf[FLASH_SECTOR_SIZE_IN_BYTES] = {0};

    if (NULL == ptrSource) {
        ret = -2;
        goto RETURN;
    }

    ret = hal_flash_info_get(partition, &partition_info);
    if (ret) {
        return ERR_RETURN;
    }
    if ((addr + size) > partition_info.partition_length) {
        TRACE(0, "error addr:0x%x size:0x%x partition_info.partition_length:0x%x", addr, size, partition_info.partition_length);
        return -1;
    }
    flash_offset = addr + partition_info.partition_start_addr;

    //watchdog_ping();

    FlashosMutexWait();
    if (addr % OTA_MALLOC_BUF_SIZE != 0) {
        buf = (uint8_t *)malloc(OTA_MALLOC_BUF_SIZE);
        if (!buf) {
            ret = -1;
            goto end;
        }
    } else {
        if (size % OTA_MALLOC_BUF_SIZE != 0) {
            buf = (uint8_t *)malloc(OTA_MALLOC_BUF_SIZE);
            if (!buf) {
                ret = -1;
                goto end;
            }
        }
    }

    pmu_flash_write_config();
    if (flash_offset % OTA_MALLOC_BUF_SIZE != 0) {
        left_len = OTA_MALLOC_BUF_SIZE - flash_offset % OTA_MALLOC_BUF_SIZE;
        fill_len = (left_len >= lengthToBurn) ? lengthToBurn : left_len;

        align_len = flash_offset;
        align_len -= align_len % OTA_MALLOC_BUF_SIZE;
        memset(buf, 0, OTA_MALLOC_BUF_SIZE);
        lock = int_lock_global();
        ret = ota_hal_read_flash(partition, align_len, buf, OTA_MALLOC_BUF_SIZE);
        int_unlock_global(lock);
        if (ret != HAL_NORFLASH_OK) {
            TRACE(0, "error %s %d, ota_hal_read_flash ret:%d", __func__, __LINE__, ret);
            SetFlashBadOptionInfo(partition, flash_offset, FLASH_READ);
            goto end;
        }

        lock = int_lock_global();
        norflash_api_flush_all();
        ret = ota_hal_erase_flash(partition, align_len, OTA_MALLOC_BUF_SIZE);
        int_unlock_global(lock);
        if (ret != HAL_NORFLASH_OK) {
            TRACE(0, "error %s %d, ota_hal_erase_flash ret:%d", __func__, __LINE__, ret);
            SetFlashBadOptionInfo(partition, flash_offset, FLASH_ERASE);
            goto end;
        }

        memcpy((buf + flash_offset % OTA_MALLOC_BUF_SIZE), ptrSource, fill_len);
        lock = int_lock_global();
        norflash_api_flush_all();
        ret = ota_hal_write_flash(partition, align_len, buf, OTA_MALLOC_BUF_SIZE);
        int_unlock_global(lock);
        if (ret != HAL_NORFLASH_OK) {
            TRACE(0, "error %s %d, ota_hal_write_flash ret:%d", __func__, __LINE__, ret);
            SetFlashBadOptionInfo(partition, flash_offset, FLASH_WRITE);
            goto end;
        }

#ifdef __WATCHER_DOG_RESET__
        //app_wdt_ping();
#endif

        lengthToBurn -= fill_len;
        flash_offset += fill_len;
        ptrSource += fill_len;
    }

    if (lengthToBurn > 0) {
        for (num = 0; num < lengthToBurn / OTA_MALLOC_BUF_SIZE; num++) {
            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_erase_flash(partition, flash_offset, OTA_MALLOC_BUF_SIZE);
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_erase_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_ERASE);
                goto end;
            }

            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_write_flash(partition, flash_offset, ptrSource, OTA_MALLOC_BUF_SIZE);
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_write_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_WRITE);
                goto end;
            }

#ifdef __WATCHER_DOG_RESET__
            //app_wdt_ping();
#endif
            flash_offset += OTA_MALLOC_BUF_SIZE;
            ptrSource += OTA_MALLOC_BUF_SIZE;
        }

        left_len = lengthToBurn % OTA_MALLOC_BUF_SIZE;
        if (left_len) {
            memset(buf, 0, OTA_MALLOC_BUF_SIZE);
            lock = int_lock_global();
            ret = ota_hal_read_flash(partition, flash_offset, buf, OTA_MALLOC_BUF_SIZE);
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_read_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_READ);
                goto end;
            }

            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_erase_flash(partition, flash_offset, OTA_MALLOC_BUF_SIZE);
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_erase_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_ERASE);
                goto end;
            }

            memcpy(buf, ptrSource, left_len);
            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_write_flash(partition, flash_offset, buf, OTA_MALLOC_BUF_SIZE);
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_write_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_WRITE);
                goto end;
            }

#ifdef __WATCHER_DOG_RESET__
            //app_wdt_ping();
#endif
        }
    }

    for (num = 0; num < sizeof(ota_partition); num++) {
        if ((partition == ota_partition[num]) && (addr == 0)) {
            flash_offset = partition_info.partition_start_addr;
            hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
            lock = int_lock_global();
            ret = ota_hal_read_flash(partition, flash_offset, read_buf, sizeof(read_buf));
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_read_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_READ);
                goto end;
            }

            read_buf[0] = MAGIC_CHECK_ADDRESS0;
            read_buf[1] = MAGIC_CHECK_ADDRESS1;
            read_buf[2] = MAGIC_CHECK_ADDRESS2;
            read_buf[3] = MAGIC_CHECK_ADDRESS3;

            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_erase_flash(partition, flash_offset, sizeof(read_buf));
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_erase_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_ERASE);
                goto end;
            }

            lock = int_lock_global();
            norflash_api_flush_all();
            ret = ota_hal_write_flash(partition, flash_offset, read_buf, sizeof(read_buf));
            int_unlock_global(lock);
            if (ret != HAL_NORFLASH_OK) {
                TRACE(0, "error %s %d, ota_hal_write_flash ret:%d", __func__, __LINE__, ret);
                SetFlashBadOptionInfo(partition, flash_offset, FLASH_WRITE);
                goto end;
            }
            break;
        }
    }

end:
    pmu_flash_read_config();
    if (addr % OTA_MALLOC_BUF_SIZE != 0) {
        free(buf);
        buf = NULL;
    } else {
        if (size % OTA_MALLOC_BUF_SIZE != 0) {
            free(buf);
            buf = NULL;
        }
    }
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    osMutexRelease(FlashMutex);

RETURN:
    return ret;
}

/*
 * 获取当前是那个区间：partition
 * 获取具体那个坏块号
 */
int32_t SRAM_TEXT_LOC GetFlashBadBlockNum(hal_partition_t partition, uint32_t *blockNum)
{
    struct HAL_FLASH_BAD_INFO *flashbadinfo = NULL;
    if (blockNum == NULL) {
        return -1;
    }

    for (size_t i = 0; i < BLOCK_MAX_INFO; i++) {
        flashbadinfo = &g_flashBadInfo[i];
        if (flashbadinfo->partition == partition) {
            *blockNum = flashbadinfo->blocknum;
            break;
        }
    }

    return 0;
}

/*
 * 获取当前是那个区间：partition
 * 获取区间上偏移（算出具体物理地址）：offset
 * 获取具体那个坏块号：
 */
int32_t SRAM_TEXT_LOC GetFlashBadBlock(hal_partition_t *partition, uint32_t *offset, uint32_t *blockNum)
{
    struct HAL_FLASH_BAD_INFO *flashbadinfo = NULL;
    if (partition == NULL || offset == NULL || blockNum == NULL) {
        return -1;
    }

    flashbadinfo = GetFlashBadOptionInfo();
    *partition = flashbadinfo->partition;
    *offset = flashbadinfo->offset;
    *blockNum = flashbadinfo->blocknum;

    return 0;
}

/*
 * 获取集成Flash芯片的厂家ID:factoryID
 * 获取集成Flash芯片的设备ID:deviceID
 */
int32_t SRAM_TEXT_LOC GetFlashType(int32_t *factoryID, int32_t *deviceID)
{
    uint8_t flash_id[HAL_NORFLASH_DEVICE_ID_LEN];
    int32_t a, b;

    NORFLASH_API_WRAP(hal_norflash_get_id)(HAL_FLASH_ID_0, flash_id, ARRAY_SIZE(flash_id));

    TRACE(3, "FLASH_ID: %02X-%02X-%02X", flash_id[0], flash_id[1], flash_id[2]);
    *factoryID = flash_id[0];

    a = 0xffff00ff | (flash_id[1] << 8);
    b = 0xffffff00 | flash_id[2];

    *deviceID = a & b;

    return 0;
}

/*
 * 当前是区间：partition
 * 当前是块号：blockNum
 * 读或者写：op
 * 获取该块号的写或者擦的次数：times
 */
int32_t SRAM_TEXT_LOC GetFlashRewriteCycle(hal_partition_t partition, uint32_t blockNum, uint32_t op, uint32_t *times)
{
    struct HAL_FLASH_RW_INFO *FlashRwInfo = NULL;

    if (times == NULL || op >= MAX_FLASH_OPTIONS) {
        return -1;
    }

    for (size_t i = 0; i < RW_MAX_PARTITION; i++) {
        FlashRwInfo = &g_flashRwInfo[i];
        if (FlashRwInfo->partition == partition) {
            for (size_t j = 0; j < RW_MAX_BLOCK; j++) {
                if (FlashRwInfo->blockInfo[j].blocknum == blockNum) {
                    *times = FlashRwInfo->blockInfo[j].count[op];
                    break;
                }
            }
        }
    }

    return 0;
}

int32_t SRAM_TEXT_LOC GetFlashRewriteCycleCount(hal_partition_t partition, uint32_t op, uint32_t *count)
{
    uint32_t tempTimes = 0;
    uint32_t countTimes = 0;
    uint32_t partitionStartBlockNum;
    uint32_t partitionEndBlockNum;
    hal_logic_partition_t info;

    if (count == NULL) {
        TRACE(0, "count is NULL\r\n");
        return -1;
    }

    if (hal_flash_info_get(partition, &info) != 0) {
        TRACE(0, "hal_flash_info_get fail\r\n");
        return -1;
    }

    partitionStartBlockNum = info.partition_start_addr / ONE_BLOCK_SIZE;
    partitionEndBlockNum = (info.partition_start_addr + info.partition_length) / ONE_BLOCK_SIZE;

    for (size_t blockNum = partitionStartBlockNum; blockNum < partitionEndBlockNum; blockNum++) {
        GetFlashRewriteCycle(partition, blockNum, op, &tempTimes);
        countTimes += tempTimes;
    }

    *count = countTimes;

    return 0;
}

/*
 * 当前是区间：partition
 * 当前是块号：blockNum
 * 读或者写：op
 * 获取该块号的写或者擦的次数：blockTimes
 */
int32_t SRAM_TEXT_LOC GetFlashStatisticInfo(hal_partition_t partition, uint32_t blockNum, uint32_t op, uint32_t *blockTimes)
{
    struct HAL_FLASH_BAD_INFO *flashBadInfo = NULL;
    uint32_t blockcount = 0;

    if (blockTimes == NULL) {
        TRACE(0, "blockTimes is NULL\r\n");
        return -1;
    }

    for (size_t i = 0; i < BLOCK_MAX_INFO; i++) {
        flashBadInfo = &g_flashBadInfo[i];
        if ((flashBadInfo->partition == partition) && (flashBadInfo->blocknum <= blockNum) && (flashBadInfo->partition == op)) {
            blockcount++;
        }
    }

    *blockTimes = blockcount;

    return 0;
}
