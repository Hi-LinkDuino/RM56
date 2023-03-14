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

#include "softbus_utils.h"

#include <ctype.h>
#include <stdlib.h>

#include "securec.h"
#include "softbus_adapter_crypto.h"
#include "softbus_adapter_mem.h"
#include "softbus_adapter_thread.h"
#include "softbus_adapter_timer.h"
#include "softbus_common.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_type_def.h"

#define MAC_BIT_ZERO 0
#define MAC_BIT_ONE 1
#define MAC_BIT_TWO 2
#define MAC_BIT_THREE 3
#define MAC_BIT_FOUR 4
#define MAC_BIT_FIVE 5

#define BT_ADDR_LEN 6
#define BT_ADDR_DELIMITER ":"
#define BT_ADDR_BASE 16

static void *g_timerId = NULL;
static TimerFunCallback g_timerFunList[SOFTBUS_MAX_TIMER_FUN_NUM] = {0};

SoftBusList *CreateSoftBusList(void)
{
    SoftBusList *list = (SoftBusList *)SoftBusMalloc(sizeof(SoftBusList));
    if (list == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "malloc failed");
        return NULL;
    }
    (void)memset_s(list, sizeof(SoftBusList), 0, sizeof(SoftBusList));

    SoftBusMutexAttr mutexAttr;
    mutexAttr.type = SOFTBUS_MUTEX_RECURSIVE;
    if (SoftBusMutexInit(&list->lock, &mutexAttr) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "init lock failed");
        SoftBusFree(list);
        return NULL;
    }

    ListInit(&list->list);
    return list;
}

void DestroySoftBusList(SoftBusList *list)
{
    ListDelInit(&list->list);
    SoftBusMutexDestroy(&list->lock);
    SoftBusFree(list);
    return;
}

int32_t RegisterTimeoutCallback(int32_t timerFunId, TimerFunCallback callback)
{
    if (callback == NULL || timerFunId >= SOFTBUS_MAX_TIMER_FUN_NUM ||
        timerFunId < SOFTBUS_CONN_TIMER_FUN) {
        return SOFTBUS_ERR;
    }

    if (g_timerFunList[timerFunId] != NULL) {
        return SOFTBUS_OK;
    }

    g_timerFunList[timerFunId] = callback;
    return SOFTBUS_OK;
}

static void HandleTimeoutFun(void)
{
    int32_t i;
    for (i = 0; i < SOFTBUS_MAX_TIMER_FUN_NUM; i++) {
        if (g_timerFunList[i] != NULL) {
            g_timerFunList[i]();
        }
    }
}

int32_t SoftBusTimerInit(void)
{
    if (g_timerId != NULL) {
        return SOFTBUS_OK;
    }
    g_timerId = SoftBusCreateTimer(&g_timerId, (void *)HandleTimeoutFun, TIMER_TYPE_PERIOD);
    if (SoftBusStartTimer(g_timerId, TIMER_TIMEOUT) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "start timer failed.");
        (void)SoftBusDeleteTimer(g_timerId);
        g_timerId = NULL;
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void SoftBusTimerDeInit(void)
{
    if (g_timerId != NULL) {
        (void)SoftBusDeleteTimer(g_timerId);
        g_timerId = NULL;
    }
}

int32_t ConvertHexStringToBytes(unsigned char *outBuf, uint32_t outBufLen, const char *inBuf, uint32_t inLen)
{
    (void)outBufLen;

    if ((outBuf == NULL) || (inBuf == NULL) || (inLen % HEXIFY_UNIT_LEN != 0)) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "invalid param");
        return SOFTBUS_ERR;
    }

    uint32_t outLen = UN_HEXIFY_LEN(inLen);
    uint32_t i = 0;
    while (i < outLen) {
        unsigned char c = *inBuf++;
        if ((c >= '0') && (c <= '9')) {
            c -= '0';
        } else if ((c >= 'a') && (c <= 'f')) {
            c -= 'a' - DEC_MAX_NUM;
        } else if ((c >= 'A') && (c <= 'F')) {
            c -= 'A' - DEC_MAX_NUM;
        } else {
            SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "HexToString Error! %c", c);
            return SOFTBUS_ERR;
        }

        unsigned char c2 = *inBuf++;
        if ((c2 >= '0') && (c2 <= '9')) {
            c2 -= '0';
        } else if ((c2 >= 'a') && (c2 <= 'f')) {
            c2 -= 'a' - DEC_MAX_NUM;
        } else if ((c2 >= 'A') && (c2 <= 'F')) {
            c2 -= 'A' - DEC_MAX_NUM;
        } else {
            SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "HexToString Error! %c2", c2);
            return SOFTBUS_ERR;
        }

        *outBuf++ = (c << HEX_MAX_BIT_NUM) | c2;
        i++;
    }
    return SOFTBUS_OK;
}

int32_t ConvertBytesToHexString(char *outBuf, uint32_t outBufLen, const unsigned char *inBuf, uint32_t inLen)
{
    if ((outBuf == NULL) || (inBuf == NULL) || (outBufLen < HEXIFY_LEN(inLen))) {
        return SOFTBUS_ERR;
    }

    while (inLen > 0) {
        unsigned char h = *inBuf / HEX_MAX_NUM;
        unsigned char l = *inBuf % HEX_MAX_NUM;

        if (h < DEC_MAX_NUM) {
            *outBuf++ = '0' + h;
        } else {
            *outBuf++ = 'a' + h - DEC_MAX_NUM;
        }

        if (l < DEC_MAX_NUM) {
            *outBuf++ = '0' + l;
        } else {
            *outBuf++ = 'a' + l - DEC_MAX_NUM;
        }

        ++inBuf;
        inLen--;
    }
    return SOFTBUS_OK;
}

int32_t GenerateRandomStr(char *str, uint32_t len)
{
    if ((str == NULL) ||  (len < HEXIFY_UNIT_LEN)) {
        return SOFTBUS_INVALID_PARAM;
    }

    uint32_t hexLen = len / HEXIFY_UNIT_LEN;
    unsigned char *hexAuthId = (unsigned char *)SoftBusMalloc(hexLen);
    if (hexAuthId == NULL) {
        return SOFTBUS_MEM_ERR;
    }
    (void)memset_s(hexAuthId, hexLen, 0, hexLen);

    if (SoftBusGenerateRandomArray(hexAuthId, hexLen) != SOFTBUS_OK) {
        SoftBusFree(hexAuthId);
        return SOFTBUS_ERR;
    }

    if (ConvertBytesToHexString(str, len, hexAuthId, hexLen) != SOFTBUS_OK) {
        SoftBusFree(hexAuthId);
        return SOFTBUS_ERR;
    }

    SoftBusFree(hexAuthId);
    return SOFTBUS_OK;
}

bool IsValidString(const char *input, uint32_t maxLen)
{
    if (input == NULL) {
        return false;
    }

    uint32_t len = strlen(input);
    if (len >= maxLen) {
        return false;
    }

    return true;
}

int32_t ConvertBtMacToBinary(const char *strMac, uint32_t strMacLen, uint8_t *binMac, uint32_t binMacLen)
{
    if (strMac == NULL || strMacLen < BT_MAC_LEN || binMac == NULL || binMacLen < BT_ADDR_LEN) {
        return SOFTBUS_INVALID_PARAM;
    }
    char *tmpMac = (char *)SoftBusMalloc(strMacLen * sizeof(char));
    if (tmpMac == NULL) {
        return SOFTBUS_MALLOC_ERR;
    }
    if (memcpy_s(tmpMac, strMacLen, strMac, strMacLen) != EOK) {
        SoftBusFree(tmpMac);
        return SOFTBUS_MEM_ERR;
    }
    char *nextTokenPtr = NULL;
    char *token = strtok_r((char *)tmpMac, BT_ADDR_DELIMITER, &nextTokenPtr);
    char *endptr = NULL;
    for (int i = 0; i < BT_ADDR_LEN; i++) {
        if (token == NULL) {
            SoftBusFree(tmpMac);
            return SOFTBUS_ERR;
        }
        binMac[i] = strtoul(token, &endptr, BT_ADDR_BASE);
        token = strtok_r(NULL, BT_ADDR_DELIMITER, &nextTokenPtr);
    }
    SoftBusFree(tmpMac);
    return SOFTBUS_OK;
}

int32_t ConvertBtMacToStr(char *strMac, uint32_t strMacLen, const uint8_t *binMac, uint32_t binMacLen)
{
    int32_t ret;

    if (strMac == NULL || strMacLen < BT_MAC_LEN || binMac == NULL || binMacLen < BT_ADDR_LEN) {
        return SOFTBUS_INVALID_PARAM;
    }
    ret = snprintf_s(strMac, strMacLen, strMacLen - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
        binMac[MAC_BIT_ZERO], binMac[MAC_BIT_ONE], binMac[MAC_BIT_TWO],
        binMac[MAC_BIT_THREE], binMac[MAC_BIT_FOUR], binMac[MAC_BIT_FIVE]);
    if (ret < 0) {
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t Strnicmp(const char *src1, const char *src2, uint32_t len)
{
    if (src1 == NULL || src2 == NULL ||
        strlen(src1) + 1 < len || strlen(src2) + 1 < len) {
        return SOFTBUS_ERR;
    }
    char *tmpSrc1 = (char *)src1;
    char *tmpSrc2 = (char *)src2;
    int32_t ca;
    int32_t cb;
    uint32_t i = len;
    do {
        ca = (int32_t)(*tmpSrc1++);
        cb = (int32_t)(*tmpSrc2++);
        ca = toupper(ca);
        cb = toupper(cb);
        i--;
    } while (ca == cb && i > 0);
    return ca - cb;
}
