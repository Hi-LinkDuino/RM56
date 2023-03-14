/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hc_file_iot_flash_test.h"

#include <hctest.h>
#include <iot_errno.h>
#include <iot_flash.h>
#include <securec.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if FLASH_START_ADDRESS_HICHAIN

#define FLASH_SIZE_4_K 4096

#define TOTAL_STORAGE_SIZE FLASH_SIZE_4_K // 4K Bytes
#define GROUP_OFFSET_ADDR 0x0100          // sizeof(struct FlashHeader)
#define TCIS_OFFSET_ADDR 0x0BC0
#define KEY_OFFSET_ADDR 0x0DC0
#define PART2_OFFSET_ADDR 0x0EC0
#define PART3_OFFSET_ADDR 0x0F00
#define SALT_OFFSET_ADDR 0x0F40
#define MK_OFFSET_ADDR 0x0F80
#define MAGIC_NUM 0x07DECADE
#define DEFAULT_VERSION 0x01

typedef enum FileIdEnumT {
    FILE_ID_GROUP = 0,
    FILE_ID_TCIS_DATA,
    FILE_ID_KEYCONTENT,
    FILE_ID_PART2,
    FILE_ID_PART3,
    FILE_ID_SALT,
    FILE_ID_MK,
    FILE_ID_LAST,
} FileIdEnum;

static uint32_t g_startAddr = FLASH_START_ADDRESS_HICHAIN;

static const struct FlashHeader {
    struct VersionHeader {
        uint64_t magic;
        uint16_t version;
        uint8_t reserved[54]; // 54: Reserved bytes for expansion
    } versionHeader;
    struct FileHeader {
        uint32_t start;
        uint32_t size;
        uint32_t end;
    } fileHeaders[FILE_ID_LAST];
    uint8_t reserved[108]; // 108: Reserved bytes for expansion
} g_flashHeader = {
    .versionHeader = {
        .magic = MAGIC_NUM,
        .version = DEFAULT_VERSION,
    },
    .fileHeaders[FILE_ID_GROUP] = {
        .start = GROUP_OFFSET_ADDR,
        .size = TCIS_OFFSET_ADDR - GROUP_OFFSET_ADDR,
        .end = TCIS_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_TCIS_DATA] = {
        .start = TCIS_OFFSET_ADDR,
        .size = KEY_OFFSET_ADDR - TCIS_OFFSET_ADDR,
        .end = KEY_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_KEYCONTENT] = {
        .start = KEY_OFFSET_ADDR,
        .size = PART2_OFFSET_ADDR - KEY_OFFSET_ADDR,
        .end = PART2_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_PART2] = {
        .start = PART2_OFFSET_ADDR,
        .size = PART3_OFFSET_ADDR - PART2_OFFSET_ADDR,
        .end = PART3_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_PART3] = {
        .start = PART3_OFFSET_ADDR,
        .size = SALT_OFFSET_ADDR - PART3_OFFSET_ADDR,
        .end = SALT_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_SALT] = {
        .start = SALT_OFFSET_ADDR,
        .size = MK_OFFSET_ADDR - SALT_OFFSET_ADDR,
        .end = MK_OFFSET_ADDR,
    },
    .fileHeaders[FILE_ID_MK] = {
        .start = MK_OFFSET_ADDR,
        .size = TOTAL_STORAGE_SIZE - MK_OFFSET_ADDR,
        .end = TOTAL_STORAGE_SIZE,
    },
};

enum {
    RANDOM_READ_TIMES = 20,
};

static void ReadFlash(uint32_t offset, uint8_t *buffer, uint32_t size)
{
    int res;
    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashInit());
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashRead(g_startAddr + offset, size, buffer));
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashDeinit());
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);
}

static void WriteFlash(uint32_t offset, const uint8_t *buffer, uint32_t size)
{
    int res;
    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashInit());
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);

    LOGI("begin write, g_startAddr + offset = %lu, size = %lu, buffer = %p",
        (unsigned long)(g_startAddr + offset), (unsigned long)(size), buffer);
    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashWrite(g_startAddr + offset, size, buffer, true));
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, IoTFlashDeinit());
    TEST_ASSERT_EQUAL(IOT_SUCCESS, res);
}

static void TestIotFlashWrite(const uint8_t *totalDataContent, uint32_t sz)
{
    LOGI("begin to test writing flash");
    WriteFlash(0, totalDataContent, sz);
    LOGI("test writing flash done");
}

static void TestIotFlashReadWholeBlock(
    uint8_t *totalData, uint32_t sz, const uint8_t *totalDataContent, uint32_t contentSz)
{
    int res = memset_s(totalData, sz, 0, sz);
    TEST_ASSERT_EQUAL(EOK, res);
    LOGI("begin to test read whole flash");
    ReadFlash(0, totalData, sz);
    LOGI("test reading whole flash done");

    TEST_ASSERT_EQUAL(sz, contentSz);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(totalData, totalDataContent, sz);
}

static void TestIotFlashReadFlashHeader(
    struct FlashHeader *flashHeader, const struct FlashHeader *flashHeaderCompare)
{
    int res = memset_s(flashHeader, sizeof(*flashHeader), 0, sizeof(*flashHeader));
    TEST_ASSERT_EQUAL(EOK, res);
    LOGI("begin reading header");
    ReadFlash(0, (uint8_t *)flashHeader, sizeof(struct FlashHeader));
    LOGI("end reading header");

    TEST_ASSERT_EQUAL_HEX8_ARRAY((const uint8_t *)flashHeader, (const uint8_t *)flashHeaderCompare,
        sizeof(struct FlashHeader));
}

static void TestIotFlashReadSmallFiles(
    uint8_t *totalData, uint32_t sz, const uint8_t *totalDataContent, uint32_t contentSz)
{
    LOGI("begin reading small files");
    TEST_ASSERT_EQUAL((uint32_t)(TOTAL_STORAGE_SIZE), contentSz);
    int res;
    for (int i = 0; i < FILE_ID_LAST; ++i) {
        LOGI("test flash file %d/%d", i, FILE_ID_LAST);
        res = memset_s(totalData, sz, 0, sz);
        TEST_ASSERT_EQUAL(EOK, res);

        LOGI("test read from %lu, size %lu", (unsigned long)(g_flashHeader.fileHeaders[i].start),
            (unsigned long)(g_flashHeader.fileHeaders[i].size));
        ReadFlash(g_flashHeader.fileHeaders[i].start, totalData, g_flashHeader.fileHeaders[i].size);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(totalData, totalDataContent + g_flashHeader.fileHeaders[i].start,
            g_flashHeader.fileHeaders[i].size);
    }
    LOGI("end reading small files");
}

static void TestIotFlashReadRandom(uint8_t *totalData, uint32_t sz, const uint8_t *totalDataContent, uint32_t contentSz)
{
    LOGI("begin reading random bytes");
    TEST_ASSERT_EQUAL((uint32_t)(TOTAL_STORAGE_SIZE), contentSz);
    int res;
    for (int i = 0; i < RANDOM_READ_TIMES; ++i) {
        uint32_t startAddr = rand() % TOTAL_STORAGE_SIZE;
        uint32_t size = rand() % (TOTAL_STORAGE_SIZE - startAddr);
        LOGI("test random read flash %d/%d, startAddr = %lu, size = %lu", i,
            RANDOM_READ_TIMES, (unsigned long)(startAddr), (unsigned long)(size));
        res = memset_s(totalData, sz, 0, sz);
        TEST_ASSERT_EQUAL(EOK, res);
        ReadFlash(startAddr, totalData, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(totalData, totalDataContent + startAddr, size);
    }
    LOGI("end reading random bytes");

    LOGI("begin reading small bytes");
    for (int i = 0; i < RANDOM_READ_TIMES; ++i) {
        uint32_t size = i + 1;
        uint32_t startAddr = rand() % (TOTAL_STORAGE_SIZE - size);
        LOGI("test random read flash %d/%d, startAddr = %lu, size = %lu", i,
            RANDOM_READ_TIMES, (unsigned long)(startAddr), (unsigned long)(size));
        res = memset_s(totalData, sz, 0, sz);
        TEST_ASSERT_EQUAL(EOK, res);
        ReadFlash(startAddr, totalData, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(totalData, totalDataContent + startAddr, size);
    }
    LOGI("end reading small bytes");
}

void TestHcFileIotFlash(void)
{
    uint8_t *totalData = (uint8_t *)malloc(TOTAL_STORAGE_SIZE);
    TEST_ASSERT_NOT_NULL(totalData);
    uint8_t *totalDataContent = (uint8_t *)malloc(TOTAL_STORAGE_SIZE);
    TEST_ASSERT_NOT_NULL(totalDataContent);
    LOGI("malloc memory succeed");

    int res, zeroCount = 0;
    struct FlashHeader flashHeader;
    res = memset_s(&flashHeader, sizeof(flashHeader), 0, sizeof(flashHeader));
    TEST_ASSERT_EQUAL(EOK, res);
    for (uint32_t i = 0; i < TOTAL_STORAGE_SIZE; ++i) {
        totalDataContent[i] = rand() % UINT8_MAX;
        if (totalDataContent[i] == 0) {
            ++zeroCount;
        }
    }
    TEST_ASSERT_NOT_EQUAL(TOTAL_STORAGE_SIZE, zeroCount);

    TestIotFlashWrite(totalDataContent, TOTAL_STORAGE_SIZE);

    TestIotFlashReadWholeBlock(totalData, TOTAL_STORAGE_SIZE, totalDataContent, TOTAL_STORAGE_SIZE);

    TestIotFlashReadFlashHeader(&flashHeader, (struct FlashHeader *)totalDataContent);

    TestIotFlashReadSmallFiles(totalData, TOTAL_STORAGE_SIZE, totalDataContent, TOTAL_STORAGE_SIZE);

    TestIotFlashReadRandom(totalData, TOTAL_STORAGE_SIZE, totalDataContent, TOTAL_STORAGE_SIZE);

    free(totalData);
    free(totalDataContent);
}

#else // FLASH_START_ADDRESS_HICHAIN

void TestHcFileIotFlash(void)
{
    LOGE("no FLASH_START_ADDRESS_HICHAIN, do not test iot flash");
}

#endif // FLASH_START_ADDRESS_HICHAIN

#ifdef __cplusplus
}
#endif
