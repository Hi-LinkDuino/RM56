/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "btm_snoop.h"

#include <stdio.h>

#include <securec.h>
#include <sys/time.h>

#include "hci/hci.h"
#include "platform/include/allocator.h"
#include "platform/include/bt_endian.h"
#include "platform/include/mutex.h"
#include "platform/include/queue.h"
#include "platform/include/reactor.h"
#include "platform/include/thread.h"

#include "btm.h"
#include "btm/btm_snoop_filter.h"
#include "log.h"

#define SNOOP_INDENTIFICATION_PATTERN                  \
    {                                                  \
        0x62, 0x74, 0x73, 0x6e, 0x6f, 0x6f, 0x70, 0x00 \
    }
#define SNOOP_VERSION_NUMBER 1
#define SNOOP_DATALINK_TYPE_H4 1002

#define MICROSECOND_1970BASE 62168256000000000

#define H4_HEADER_CMD 0x01
#define H4_HEADER_ACLDATA 0x02
#define H4_HEADER_EVENT 0x04

#define SNOOP_PACKET_FLAG_SENT 0x00
#define SNOOP_PACKET_FLAG_RECEIVED 0x01
#define SNOOP_PACKET_FLAG_DATA 0x00
#define SNOOP_PACKET_FLAG_CMD_EVENT 0x02

#define SNOOP_LAST_FILE_TAIL ".last"

#define MICROSECOND 1000000

#define SNOOP_BLOCK_IOV_COUNT 3

#define HCI_LOG_PATH "./hci.log"

#define HCI_H4_HEADER_LEN 1

#pragma pack(1)
typedef struct {
    uint8_t identificationPattern[8];  // { 0x62, 0x74, 0x73, 0x6e, 0x6f, 0x6f, 0x70, 0x00 }
    uint32_t versionNumber;            // 1
    uint32_t datalinkType;             // 1002
} BtmSnoopFileHeader;

typedef struct {
    uint32_t originalLength;
    uint32_t includedLength;
    uint32_t packetFlags;
    uint32_t cumulativeDrops;
    uint64_t timestamp;  // microseconds
} BtmSnoopPacketHeader;
#pragma pack()

static bool g_output = false;
static char *g_outputPath = NULL;
static FILE *g_outputFile = NULL;
static bool g_hciLogOuput = false;
static Mutex *g_outputMutex = NULL;

static void GetH4HeaderAndPacketFlags(uint8_t type, uint8_t *h4Header, uint32_t *packetFlags)
{
    switch (type) {
        case TRANSMISSON_TYPE_H2C_CMD:
            *h4Header = H4_HEADER_CMD;
            *packetFlags = SNOOP_PACKET_FLAG_SENT | SNOOP_PACKET_FLAG_CMD_EVENT;
            break;
        case TRANSMISSON_TYPE_C2H_EVENT:
            *h4Header = H4_HEADER_EVENT;
            *packetFlags = SNOOP_PACKET_FLAG_RECEIVED | SNOOP_PACKET_FLAG_CMD_EVENT;
            break;
        case TRANSMISSON_TYPE_H2C_DATA:
            *h4Header = H4_HEADER_ACLDATA;
            *packetFlags = SNOOP_PACKET_FLAG_SENT | SNOOP_PACKET_FLAG_DATA;
            break;
        case TRANSMISSON_TYPE_C2H_DATA:
            *h4Header = H4_HEADER_ACLDATA;
            *packetFlags = SNOOP_PACKET_FLAG_RECEIVED | SNOOP_PACKET_FLAG_DATA;
            break;
        default:
            break;
    }
}

static void BtmSnoopOutput(uint8_t type, const uint8_t *data, uint16_t length)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    const uint64_t timestamp = MICROSECOND_1970BASE + tv.tv_sec * MICROSECOND + tv.tv_usec;

    uint8_t h4Header = 0;
    uint32_t packetFlags = 0;

    GetH4HeaderAndPacketFlags(type, &h4Header, &packetFlags);

    uint16_t originalLength = length + 1;
    uint16_t includedLength = length + 1;
    const uint8_t *outputData = data;

    BtmHciFilter(type, &outputData, originalLength, &includedLength);

    BtmSnoopPacketHeader header = {
        .originalLength = H2BE_32(originalLength),
        .includedLength = H2BE_32(includedLength),
        .cumulativeDrops = H2BE_32(0),
        .packetFlags = H2BE_32(packetFlags),
        .timestamp = H2BE_64(timestamp),
    };

    if (g_outputFile == NULL) {
        LOG_ERROR("%{public}s, g_outputFile is NULL", __FUNCTION__);
        return;
    }
    MutexLock(g_outputMutex);

    (void)fwrite(&header, 1, sizeof(BtmSnoopPacketHeader), g_outputFile);
    (void)fwrite(&h4Header, 1, HCI_H4_HEADER_LEN, g_outputFile);
    (void)fwrite(outputData, 1, includedLength - HCI_H4_HEADER_LEN, g_outputFile);

    fflush(g_outputFile);

    if (outputData != data) {
        MEM_MALLOC.free((void *)outputData);
    }

    MutexUnlock(g_outputMutex);
}

static void BtmOnHciTransmission(uint8_t type, const uint8_t *data, uint16_t length)
{
    BtmSnoopOutput(type, data, length);
}

static void BtmWriteSnoopFileHeader(void)
{
    BtmSnoopFileHeader header = {
        .identificationPattern = SNOOP_INDENTIFICATION_PATTERN,
        .versionNumber = H2BE_32(SNOOP_VERSION_NUMBER),
        .datalinkType = H2BE_32(SNOOP_DATALINK_TYPE_H4),
    };

    if (g_outputFile == NULL) {
        LOG_ERROR("%{public}s, g_outputFile is NULL", __FUNCTION__);
        return;
    }
    MutexLock(g_outputMutex);

    (void)fwrite(&header, 1, sizeof(BtmSnoopFileHeader), g_outputFile);

    fflush(g_outputFile);

    MutexUnlock(g_outputMutex);
}

static bool BtmIsFileExists(const char *path)
{
    bool exists = false;
    FILE *file = fopen(path, "r");
    if (file != NULL) {
        exists = true;
        fclose(file);
    }
    return exists;
}

static void BtmPrepareSnoopFile(void)
{
    if (g_hciLogOuput) {
        bool exists = BtmIsFileExists(HCI_LOG_PATH);
        if (exists) {
            g_outputFile = fopen(HCI_LOG_PATH, "a");
            if (g_outputFile == NULL) {
                return;
            }
        } else {
            g_outputFile = fopen(HCI_LOG_PATH, "w");
            if (g_outputFile == NULL) {
                return;
            }

            BtmWriteSnoopFileHeader();
        }
    } else {
        bool exists = BtmIsFileExists(g_outputPath);
        if (exists) {
            const int length = strlen(g_outputPath) + strlen(SNOOP_LAST_FILE_TAIL) + 1;
            char *bakPath = MEM_CALLOC.alloc(length);
            if (bakPath == NULL) {
                return;
            }
            if (strcpy_s(bakPath, length, g_outputPath) != EOK) {
                return;
            }
            (void)strcat_s(bakPath, length, SNOOP_LAST_FILE_TAIL);
            rename(g_outputPath, bakPath);

            MEM_CALLOC.free(bakPath);
        }

        g_outputFile = fopen(g_outputPath, "w");
        if (g_outputFile == NULL) {
            LOG_ERROR("%{public}s, g_outputFile is NULL:%{public}s", __FUNCTION__, strerror(errno));
            return;
        }

        BtmWriteSnoopFileHeader();
    }
}

static void BtmCloseSnoopFile(void)
{
    if (g_outputFile != NULL) {
        fclose(g_outputFile);
        g_outputFile = NULL;
    }
}

int BTM_SetSnoopFilePath(const char *path, uint16_t length)
{
    g_outputPath = (char *)MEM_MALLOC.alloc(length + 1);
    if (g_outputPath == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(g_outputPath, length + 1, path, length);
    g_outputPath[length] = '\0';
    return BT_NO_ERROR;
}

int BTM_EnableSnoopFileOutput(bool filter)
{
    g_output = true;
    if (filter) {
        BtmEnableSnoopFilter();
    } else {
        BtmDisableSnoopFilter();
    }
    return BT_NO_ERROR;
}

int BTM_DisableSnoopFileOutput(void)
{
    if (g_output) {
        BtmDisableSnoopFilter();
    }
    g_output = false;
    return BT_NO_ERROR;
}

void BtmStartSnoopOutput(void)
{
    if (g_hciLogOuput || (g_output && g_outputPath != NULL)) {
        BtmPrepareSnoopFile();

        HCI_SetTransmissionCaptureCallback(BtmOnHciTransmission);
        HCI_EnableTransmissionCapture();
    }
}

void BtmStopSnoopOutput(void)
{
    HCI_DisableTransmissionCapture();

    BtmCloseSnoopFile();
}

void BtmInitSnoop(void)
{
    g_outputMutex = MutexCreate();
    BtmInitSnoopFilter();
}

void BtmCloseSnoop(void)
{
    g_output = false;
    g_hciLogOuput = false;

    BtmCloseSnoopFilter();

    if (g_outputMutex != NULL) {
        MutexDelete(g_outputMutex);
        g_outputMutex = NULL;
    }

    if (g_outputPath != NULL) {
        MEM_MALLOC.free(g_outputPath);
        g_outputPath = NULL;
    }
}

int BTM_EnableHciLogOutput(bool filter)
{
    g_hciLogOuput = true;
    if (filter) {
        BtmEnableSnoopFilter();
    } else {
        BtmDisableSnoopFilter();
    }
    return BT_NO_ERROR;
}

int BTM_DisableHciLogOutput(void)
{
    if (g_hciLogOuput) {
        BtmDisableSnoopFilter();
    }
    g_hciLogOuput = false;
    return BT_NO_ERROR;
}