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
#include "flash.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif

/* Logic partition on flash devices */
hal_logic_partition_t g_halPartitions[] = {
    [HAL_PARTITION_BOOTLOADER] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "boot1",
        .partition_start_addr = 0x0,
#ifdef BOOT1_SIZE
        .partition_length = BOOT1_SIZE, //64KB
#else
        .partition_length = 0x10000, //64KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_BOOT2A] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "boot2A",
#ifdef BOOT1_SIZE
        .partition_start_addr = BOOT1_SIZE,
        .partition_length = BOOT2_SIZE, //64KB
#else
        .partition_start_addr = 0x10000,
        .partition_length = 0x10000, //64KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_BOOT2B] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "boot2B",
#ifdef BOOT1_SIZE
        .partition_start_addr = BOOT1_SIZE+BOOT2_SIZE,
        .partition_length = BOOT2_SIZE, //64KB
#else
        .partition_start_addr = 0x20000,
        .partition_length = 0x10000, //64KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#if defined(SECURE_BOOT)
    [HAL_PARTITION_BOOT2B+1] =
    {
            .partition_owner            = HAL_FLASH_EMBEDDED,
            .partition_description      = "boot_info",
            .partition_start_addr       = BOOT1_SIZE+BOOT2_SIZE+BOOT2_SIZE, //boot information need protect
            .partition_length           = BOOTINFO_SIZE,  //32k bytes
            .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#else
    [HAL_PARTITION_TRUSTZONEA] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "trustzoneA",
#ifdef TZ_SIZE
        .partition_start_addr = BOOT1_SIZE+BOOT2_SIZE+BOOT2_SIZE,
        .partition_length = TZ_SIZE, //152KB
#else
        .partition_start_addr = 0x30000,
        .partition_length = 0x26000, //152KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_TRUSTZONEB] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "trustzoneB",
#ifdef TZ_SIZE
        .partition_start_addr = BOOT1_SIZE+BOOT2_SIZE+BOOT2_SIZE+TZ_SIZE,
        .partition_length = TZ_SIZE, //152KB
#else
        .partition_start_addr = 0x56000,
        .partition_length = 0x26000, //152KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_TZ_INFO] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "trustzoneInfo",
#ifdef TZ_SIZE
        .partition_start_addr = BOOT1_SIZE+BOOT2_SIZE+BOOT2_SIZE+TZ_SIZE+TZ_SIZE,
        .partition_length = TZ_INFO_SIZE, //152KB
#else
        .partition_start_addr = 0x7C000,
        .partition_length = 0x4000, //16KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_CM33_MAIN] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "cm33_main",
#ifdef OTA_CODE_OFFSET
        .partition_start_addr = OTA_CODE_OFFSET,
        .partition_length = OTA_CODE_SIZE, //8064KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x80000,
        .partition_length = 0x600000, //6MB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0x80000,
        .partition_length = 0x600000, //6MB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },

    [HAL_PARTITION_SYSTEM_MINI] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "system_mini",
#ifdef FW_MINI_CODE_SIZE
        .partition_start_addr = FW_MINI_CODE_OFFSET,
        .partition_length = FW_MINI_CODE_SIZE, //3072KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x680000,
        .partition_length = 0x600000, //6MB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0x680000,
        .partition_length = 0x600000, //6MB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_RESOURCE] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "littlefs",
#if defined(LFS_FLASH_BASE_ADDR) && defined(LFS_SIZE)
        .partition_start_addr = LFS_FLASH_BASE_ADDR,
        .partition_length = LFS_SIZE,
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0xC80000,
        .partition_length = 0x400000, //4MB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0xC80000,
        .partition_length = 0x300000, //3MB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_LOG] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "log",
#ifdef LOG_DUMP_SECTION_SIZE
        .partition_start_addr = LOG_DUMP_SECTION_OFFSET,
        .partition_length = LOG_DUMP_SECTION_SIZE, //192KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x1CC0000,
        .partition_length = 0x30000, //192KB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0xF60000,
        .partition_length = 0x30000, //192KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#if 0
#if (FLASH_SIZE == 0x2000000)
    [HAL_PARTITION_DATA] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "data",
        .partition_start_addr = 0x1DC0000,
        .partition_length = 0x40000, //256KB
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_MISC] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "misc",
        .partition_start_addr = 0x1FC0000,
        .partition_length = 0x8000, //32KB
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#elif (FLASH_SIZE == 0x1000000)
    [HAL_PARTITION_DATA] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "data",
        .partition_start_addr = 0xF90000,
        .partition_length = 0x40000, //256KB
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_MISC] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "misc",
        .partition_start_addr = 0xFD0000,
        .partition_length = 0x8000, //32KB
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
#endif
    [HAL_PARTITION_USERDATA] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "userdata",
#ifdef USERDATA_SECTION_SIZE
        .partition_start_addr = USERDATA_SECTION_OFFSET,
        .partition_length = USERDATA_SECTION_SIZE*2, //16KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x1FD0000,
        .partition_length = 0x4000, //16KB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0xFDC000,
        .partition_length = 0x4000, //16KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_ENV] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "factory",
#if defined(FACTORY_SECTION_SIZE) && defined(FACTORY_SECTION_OFFSET)
        .partition_start_addr = FACTORY_SECTION_OFFSET,
        .partition_length = FACTORY_SECTION_SIZE/2, //64KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x1FFE000,
        .partition_length = 0x1000, //4KB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0xFFE000,
        .partition_length = 0x1000, //4KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_ENV_REDUND] = {
        .partition_owner = HAL_FLASH_EMBEDDED,
        .partition_description = "factory_backup",
#if defined(FACTORY_SECTION_SIZE) && defined(FACTORY_SECTION_OFFSET)
        .partition_start_addr = FACTORY_SECTION_OFFSET+FACTORY_SECTION_SIZE/2,
        .partition_length = FACTORY_SECTION_SIZE/2, //64KB
#elif (FLASH_SIZE == 0x2000000)
        .partition_start_addr = 0x1FFF000,
        .partition_length = 0x1000, //4KB
#elif (FLASH_SIZE == 0x1000000)
        .partition_start_addr = 0xFFF000,
        .partition_length = 0x1000, //4KB
#endif
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define DISPLAY_MISC_STORAGE_FLASH_CONFIG HCS_NODE(HCS_NODE(HCS_NODE(HCS_ROOT, misc), storage_config), flash_config)
static uint32_t FlashGetResource()
{
    int32_t num = HCS_ARRAYS_SIZE(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, partitions));
    if (num < 0 || num > HAL_PARTITION_MAX) {
        HDF_LOGE("%s: invalid partitions num %d", __func__, num);
        return HDF_FAILURE;
    }
    uint32_t partition;
    uint32_t partitions[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, partitions));
    uint32_t partition_owner[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, owner));
    char * description[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, description));
    uint32_t start_addr[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, start_addr));
    uint32_t length[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, length));
    uint32_t options[] = HCS_ARRAYS(HCS_NODE(DISPLAY_MISC_STORAGE_FLASH_CONFIG, options));
    for (int32_t i = 0; i < num; i++) {
        partition = partitions[i];
        if (partition >= HAL_PARTITION_MAX) {
            HDF_LOGE("%s: invalid partition %u", __func__, partition);
            return HDF_FAILURE;
        }
        g_halPartitions[partition].partition_owner = partition_owner[i];
        g_halPartitions[partition].partition_description = description[i];
        g_halPartitions[partition].partition_start_addr = start_addr[i];
        g_halPartitions[partition].partition_length = length[i];
        g_halPartitions[partition].partition_options = options[i];
        HDF_LOGD("%s: partition[%u] owner=%u, description=%s, start_addr=0x%x, length=0x%x, options=%u", __func__, partition,
                 g_halPartitions[partition].partition_owner, g_halPartitions[partition].partition_description,
                 g_halPartitions[partition].partition_start_addr, g_halPartitions[partition].partition_length,
                 g_halPartitions[partition].partition_options);
    }
    return HDF_SUCCESS;
}
#else
static uint32_t FlashGetResource(const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *resource = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (resource == NULL) {
        HDF_LOGE("Invalid DeviceResourceIface");
        return HDF_FAILURE;
    }
    int32_t num = resource->GetElemNum(resourceNode, "partitions");
    if (num < 0 || num > HAL_PARTITION_MAX) {
        HDF_LOGE("%s: invalid partitions num %d", __func__, num);
        return HDF_FAILURE;
    }
    for (int32_t i = 0; i < num; i++) {
        uint32_t partition = 0;
        if (resource->GetUint32ArrayElem(resourceNode, "partitions", i, &partition, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get partitions", __func__);
            return HDF_FAILURE;
        }
        if (partition >= HAL_PARTITION_MAX) {
            HDF_LOGE("%s: invalid partition %u", __func__, partition);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "owner", i, (uint32_t *)&g_halPartitions[partition].partition_owner, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get owner", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetStringArrayElem(resourceNode, "description", i, &g_halPartitions[partition].partition_description, NULL) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get mount_points", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "start_addr", i, &g_halPartitions[partition].partition_start_addr, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get block_size", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "length", i, &g_halPartitions[partition].partition_length, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get block_count", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "options", i, &g_halPartitions[partition].partition_options, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get block_count", __func__);
            return HDF_FAILURE;
        }
        HDF_LOGD("%s: partition[%u] owner=%u, description=%s, start_addr=0x%x, length=0x%x, options=%u", __func__, partition,
                 g_halPartitions[partition].partition_owner, g_halPartitions[partition].partition_description,
                 g_halPartitions[partition].partition_start_addr, g_halPartitions[partition].partition_length,
                 g_halPartitions[partition].partition_options);
    }
    return HDF_SUCCESS;
}
#endif
static int32_t FlashDriverInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        return HDF_FAILURE;
    }
#if !defined(LFS_FLASH_BASE_ADDR)
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (FlashGetResource() != HDF_SUCCESS) {
        HDF_LOGE("%s: FlashGetResource failed", __func__);
        return HDF_FAILURE;
    }
#else
    if (object->property) {
        if (FlashGetResource(object->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: FlashGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }
#endif
#endif
    return HDF_SUCCESS;
}

static int32_t FlashDriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void FlashDriverRelease(struct HdfDeviceObject *device)
{
    (void)device;
}

static struct HdfDriverEntry g_FlashDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_STORAGE_FLASH",
    .Bind = FlashDriverBind,
    .Init = FlashDriverInit,
    .Release = FlashDriverRelease,
};

HDF_INIT(g_FlashDriverEntry);