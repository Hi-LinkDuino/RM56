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

#ifndef SOFTBUS_UTILS_H
#define SOFTBUS_UTILS_H

#include "softbus_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HEXIFY_UNIT_LEN 2
#define UN_HEXIFY_LEN(len) ((len) / HEXIFY_UNIT_LEN)
#define HEXIFY_LEN(len) ((len) * HEXIFY_UNIT_LEN + 1)
#define HEX_MAX_NUM 16
#define DEC_MAX_NUM 10
#define HEX_MAX_BIT_NUM 4
#define MAX_QUERY_LEN 64

#define TIMER_TIMEOUT 1000 // 1s

typedef void (*TimerFunCallback)(void);

typedef enum {
    SOFTBUS_CONN_TIMER_FUN,
    SOFTBUS_AUTHEN_TIMER_FUN,
    SOFTBUS_SESSION_TIMER_FUN,
    SOFTBUS_PROXYCHANNEL_TIMER_FUN,
    SOFTBUS_PROXYSLICE_TIMER_FUN,
    SOFTBUS_TCP_DIRECTCHANNEL_TIMER_FUN,
    SOFTBUS_UDP_CHANNEL_TIMER_FUN,
    SOFTBUS_TIME_SYNC_TIMER_FUN,
    SOFTBUS_PROXY_SENDFILE_TIMER_FUN,
    SOFTBUS_MAX_TIMER_FUN_NUM
} SoftBusTimerFunEnum;

int32_t RegisterTimeoutCallback(int32_t timerFunId, TimerFunCallback callback);

int32_t SoftBusTimerInit(void);

void SoftBusTimerDeInit(void);
SoftBusList *CreateSoftBusList(void);

void DestroySoftBusList(SoftBusList *list);

int32_t ConvertBytesToHexString(char *outBuf, uint32_t outBufLen, const unsigned char *inBuf, uint32_t inLen);

int32_t ConvertHexStringToBytes(unsigned char *outBuf, uint32_t outBufLen, const char *inBuf, uint32_t inLen);

int32_t GenerateRandomStr(char *str, uint32_t size);

bool IsValidString(const char *input, uint32_t maxLen);

int32_t ConvertBtMacToBinary(const char *strMac, uint32_t strMacLen, uint8_t *binMac, uint32_t binMacLen);

int32_t ConvertBtMacToStr(char *strMac, uint32_t strMacLen, const uint8_t *binMac, uint32_t binMacLen);

int32_t Strnicmp(const char *src1, const char *src2, uint32_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* SOFTBUS_UTILS_H */