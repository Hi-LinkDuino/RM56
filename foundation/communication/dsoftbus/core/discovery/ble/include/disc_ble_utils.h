/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef DISC_BLE_UTILS_H
#define DISC_BLE_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#include "discovery_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ADV_DATA_MAX_LEN 24
#define RESP_DATA_MAX_LEN 26
#define BOARDCAST_MAX_LEN (ADV_DATA_MAX_LEN + RESP_DATA_MAX_LEN)

typedef struct {
    union {
        unsigned char data[BOARDCAST_MAX_LEN];
        struct {
            unsigned char advData[ADV_DATA_MAX_LEN];
            unsigned char rspData[RESP_DATA_MAX_LEN];
        };
    } data;
    unsigned short dataLen;
} BoardcastData;

bool CheckBitMapEmpty(uint32_t capBitMapNum, const uint32_t *capBitMap);
bool CheckCapBitMapExist(uint32_t capBitMapNum, const uint32_t *capBitMap, uint32_t pos);
bool CheckCapBitMapEqual(const uint32_t *srcBitMap, const uint32_t *dstBitMap, uint32_t capBitMapNum);
void SetCapBitMapPos(uint32_t capBitMapNum, uint32_t *capBitMap, uint32_t pos);
void UnsetCapBitMapPos(uint32_t capBitMapNum, uint32_t *capBitMap, uint32_t pos);

int32_t DiscBleGetDeviceUdid(char *devId, uint32_t len);
int32_t DiscBleGetDeviceName(char *deviceName);
int32_t DiscBleGetHwAccount(char *hwAccount);
uint8_t DiscBleGetDeviceType(void);
int32_t DiscBleGetDeviceIdHash(unsigned char *hashStr);
int32_t DiscBleGetShortUserIdHash(unsigned char *hashStr);

int32_t AssembleTLV(BoardcastData *boardcastData, unsigned char dataType, const unsigned char *data, uint32_t dataLen);
int32_t GetDeviceInfoFromDisAdvData(DeviceInfo *info, const unsigned char *data, uint32_t dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif