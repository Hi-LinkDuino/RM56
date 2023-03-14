/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "audio_internal.h"
#include "hdf_log.h"
#include "pnp_message_report.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_STUB

#define PNP_REPORT_MSG_FIELD_NUM 5

static int32_t AudioPnpDevPlugMsgDeSerialize(uint8_t *msgStr, struct PnpReportDevPlugMsg *devPlugMsg)
{
    int i;
    int32_t ret = -1;
    char *stringTepm = NULL;
    uint8_t buf[PNP_REPORT_MSG_FIELD_NUM - 1] = {0};

    if (msgStr == NULL) {
        HDF_LOGE("[%{public}s]: Parameter error!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    char strTemp[PNP_REPORT_MSG_LEN_MAX] = {0};
    ret = memcpy_s(strTemp, PNP_REPORT_MSG_LEN_MAX, (char *)msgStr, strlen((char *)msgStr));
    if (ret != EOK) {
        HDF_LOGE("[%{public}s]: memcpy_s fail!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    stringTepm = strtok((char*)strTemp, ";");
    if (stringTepm != NULL) {
        devPlugMsg->eventType = (uint8_t)atoi(stringTepm);
        if (devPlugMsg->eventType != DEVICE_PULG) {
            HDF_LOGE("[%{public}s]: PnpReportType error!", __func__);

            return AUDIO_HAL_ERR_INVALID_PARAM;
        }
    }
    for (i = 1; i < PNP_REPORT_MSG_FIELD_NUM; i++) {
        stringTepm = strtok(NULL, ";");
        if (stringTepm != NULL) {
            buf[i - 1] = (uint8_t)atoi(stringTepm);
        } else {
            HDF_LOGE("[%{public}s]: Parse error!", __func__);
            return AUDIO_HAL_ERR_NOT_SUPPORT;
        }
    }
    devPlugMsg->state = buf[0];
    devPlugMsg->deviceType = buf[1];
    devPlugMsg->deviceCap = buf[2]; // 2 is deviceCap
    devPlugMsg->id = buf[3]; // 3 is id

    return AUDIO_HAL_SUCCESS;
}

static int32_t AudioPnpDevEventMsgDeSerialize(uint8_t *msgStr, struct PnpReportEventMsg *eventMsg)
{
    int i;
    int32_t ret = -1;
    char *stringTepm = NULL;
    uint8_t buf[PNP_REPORT_MSG_FIELD_NUM - 1] = {0};

    if (msgStr == NULL || eventMsg == NULL) {
        HDF_LOGE("[%{public}s]: Parameter error!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    char strTemp[PNP_REPORT_MSG_LEN_MAX] = {0};
    ret = memcpy_s(strTemp, PNP_REPORT_MSG_LEN_MAX, (char *)msgStr, strlen((char *)msgStr));
    if (ret != EOK) {
        HDF_LOGE("[%{public}s]: memcpy_s fail!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }

    stringTepm = strtok((char *)strTemp, ";");
    if (stringTepm != NULL) {
        eventMsg->eventType = (uint8_t)atoi(stringTepm);
        if (eventMsg->eventType != EVENT_REPORT) {
            HDF_LOGE("[%{public}s]: PnpReportType error!", __func__);
            return AUDIO_HAL_ERR_INVALID_PARAM;
        }
    }
    for (i = 1; i < PNP_REPORT_MSG_FIELD_NUM; i++) {
        stringTepm = strtok(NULL, ";");
        if (stringTepm != NULL) {
            buf[i - 1] = (uint8_t)atoi(stringTepm);
        } else {
            HDF_LOGE("[%{public}s]: Parse error!", __func__);
            return AUDIO_HAL_ERR_NOT_SUPPORT;
        }
    }

    eventMsg->eventId = buf[0];
    eventMsg->eventValue = buf[1];
    eventMsg->deviceType = buf[2]; // 2 is deviceType
    eventMsg->reserve = buf[3]; // 3 is reserve
    
    return AUDIO_HAL_SUCCESS;
}

int32_t PnpReportMsgDeSerialize(uint8_t *msgStr, enum PnpReportType msgType,
    struct PnpReportMsg *pnpReportMsg)
{
    int32_t ret;
    int len;
    if (pnpReportMsg == NULL || msgStr == NULL) {
        HDF_LOGE("[%{public}s]: Parameter error!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    len = strlen((const char *)msgStr);
    if (len == 0 || len > PNP_REPORT_MSG_LEN_MAX) {
        HDF_LOGE("[%{public}s]: Parameter error!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    switch (msgType) {
        case DEVICE_PULG:
            pnpReportMsg->reportType = DEVICE_PULG;
            ret = AudioPnpDevPlugMsgDeSerialize(msgStr, &pnpReportMsg->devPlugMsg);
            if (ret != 0) {
                HDF_LOGE("[%{public}s]: PnpDevPlugMsgPrase error!", __func__);
                return AUDIO_HAL_ERR_INTERNAL;
            }
            break;
        case EVENT_REPORT:
            pnpReportMsg->reportType = EVENT_REPORT;
            ret = AudioPnpDevEventMsgDeSerialize(msgStr, &pnpReportMsg->eventMsg);
            if (ret != 0) {
                HDF_LOGE("[%{public}s]: PnpDevEventMsgPrase error!", __func__);
                return AUDIO_HAL_ERR_INTERNAL;
            }
            break;
        default:
            HDF_LOGE("[%{public}s]: Unknown message type!", __func__);
            return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    return AUDIO_HAL_SUCCESS;
}

char *PnpReportMsgSerialize(struct PnpReportMsg *pnpReportMsg)
{
    int ret = 0;
    char *msgBuf = NULL;
    if (pnpReportMsg == NULL) {
        HDF_LOGE("[%{public}s]: Parameter error!", __func__);
        return NULL;
    }
    msgBuf = OsalMemCalloc(PNP_REPORT_MSG_LEN_MAX);
    if (msgBuf == NULL) {
        HDF_LOGE("[%{public}s]: Malloc memory failed!", __func__);
        return NULL;
    }
    
    switch (pnpReportMsg->reportType) {
        case DEVICE_PULG:
            ret = snprintf_s(msgBuf, PNP_REPORT_MSG_LEN_MAX, PNP_REPORT_MSG_LEN_MAX - 1, "%d;%d;%d;%d;%d",
                             pnpReportMsg->devPlugMsg.eventType, pnpReportMsg->devPlugMsg.state,
                             pnpReportMsg->devPlugMsg.deviceType, pnpReportMsg->devPlugMsg.deviceCap,
                             pnpReportMsg->devPlugMsg.id);
            break;
        case EVENT_REPORT:
            ret = snprintf_s(msgBuf, PNP_REPORT_MSG_LEN_MAX, PNP_REPORT_MSG_LEN_MAX - 1, "%d;%d;%d;%d;%d",
                             pnpReportMsg->eventMsg.eventType, pnpReportMsg->eventMsg.eventId,
                             pnpReportMsg->eventMsg.eventValue, pnpReportMsg->eventMsg.deviceType,
                             pnpReportMsg->eventMsg.reserve);
            break;
        default:
            HDF_LOGE("[%{public}s]: Unknown message type!", __func__);
            OsalMemFree(msgBuf);
            return NULL;
    }
    if (ret < 0) {
        HDF_LOGE("[%{public}s]: snprintf_s failed!", __func__);
        OsalMemFree(msgBuf);
        return NULL;
    }
    return msgBuf;
}

