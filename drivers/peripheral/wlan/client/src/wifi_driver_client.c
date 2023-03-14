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

#include <string.h>
#include <stdlib.h>
#include "wifi_common_cmd.h"
#include "hilog/log.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef EOK
#define EOK 0
#endif

#define MAX_CALL_BACK_COUNT 10
static struct CallbackEvent *g_callbackEventMap[MAX_CALL_BACK_COUNT] = {NULL};

void WifiEventReport(const char *ifName, uint32_t event, void *data)
{
    uint32_t i;

    for (i = 0; i < MAX_CALL_BACK_COUNT; i++) {
        if (g_callbackEventMap[i] != NULL && (strcmp(g_callbackEventMap[i]->ifName, ifName) == 0) &&
            (((1 << event) & g_callbackEventMap[i]->eventType) != 0)) {
            HILOG_INFO(LOG_DOMAIN, "%s: WifiEventReport send event = %u, ifName = %s",
                __FUNCTION__, event, ifName);
            g_callbackEventMap[i]->onRecFunc(event, data, ifName);
        }
    }
}

int32_t WifiRegisterEventCallback(OnReceiveFunc onRecFunc, uint32_t eventType, const char *ifName)
{
    uint32_t i;
    struct CallbackEvent *callbackEvent = NULL;

    if (onRecFunc == NULL || ifName == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return RET_CODE_INVALID_PARAM;
    }
    for (i = 0; i < MAX_CALL_BACK_COUNT; i++) {
        if (g_callbackEventMap[i] != NULL && g_callbackEventMap[i]->eventType == eventType &&
            (strcmp(g_callbackEventMap[i]->ifName, ifName) == 0) && g_callbackEventMap[i]->onRecFunc == onRecFunc) {
            HILOG_INFO(LOG_DOMAIN, "%s the onRecFunc has been registered!", __FUNCTION__);
            return RET_CODE_SUCCESS;
        }
    }
    callbackEvent = (struct CallbackEvent *)malloc(sizeof(struct CallbackEvent));
    if (callbackEvent == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s fail: malloc fail!", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    callbackEvent->eventType = eventType;
    if (strcpy_s(callbackEvent->ifName, IFNAMSIZ, ifName) != RET_CODE_SUCCESS) {
        free(callbackEvent);
        return RET_CODE_FAILURE;
    }
    callbackEvent->onRecFunc = onRecFunc;
    for (i = 0; i < MAX_CALL_BACK_COUNT; i++) {
        if (g_callbackEventMap[i] == NULL) {
            g_callbackEventMap[i] = callbackEvent;
            return RET_CODE_SUCCESS;
        }
    }
    free(callbackEvent);
    HILOG_ERROR(LOG_DOMAIN, "%s fail: register onRecFunc num more than %d!", __FUNCTION__, MAX_CALL_BACK_COUNT);
    return RET_CODE_FAILURE;
}

void WifiUnregisterEventCallback(OnReceiveFunc onRecFunc, uint32_t eventType, const char *ifName)
{
    uint32_t i;

    if (onRecFunc == NULL || ifName == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    for (i = 0; i < MAX_CALL_BACK_COUNT; i++) {
        if (g_callbackEventMap[i] != NULL && g_callbackEventMap[i]->eventType == eventType &&
            (strcmp(g_callbackEventMap[i]->ifName, ifName) == 0) && g_callbackEventMap[i]->onRecFunc == onRecFunc) {
            g_callbackEventMap[i]->onRecFunc = NULL;
            free(g_callbackEventMap[i]);
            g_callbackEventMap[i] = NULL;
            return;
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
