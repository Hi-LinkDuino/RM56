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

#include "trans_lane_manager.h"

#include <securec.h>
#include <unistd.h>
#include "common_list.h"
#include "softbus_adapter_mem.h"
#include "softbus_adapter_thread.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_utils.h"

typedef struct {
    ListNode node;
    int32_t channelId;
    int32_t channelType;
    char pkgName[PKG_NAME_SIZE_MAX];
    LnnLanesObject *lanesObj;
} TransLaneInfo;

static SoftBusList *g_channelLaneList = NULL;

int32_t TransLaneMgrInit(void)
{
    if (g_channelLaneList != NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "trans lane info manager hasn't initialized.");
        return SOFTBUS_OK;
    }
    g_channelLaneList = CreateSoftBusList();
    if (g_channelLaneList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane info manager init failed.");
        return SOFTBUS_MALLOC_ERR;
    }
    return SOFTBUS_OK;
}

void TransLaneMgrDeinit(void)
{
    if (g_channelLaneList == NULL) {
        return;
    }

    if (SoftBusMutexLock(&g_channelLaneList->lock) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock failed");
        return;
    }
    TransLaneInfo *laneItem = NULL;
    TransLaneInfo *nextLaneItem = NULL;
    LIST_FOR_EACH_ENTRY_SAFE(laneItem, nextLaneItem, &g_channelLaneList->list, TransLaneInfo, node) {
        ListDelete(&(laneItem->node));
        LnnReleaseLanesObject(laneItem->lanesObj);
        SoftBusFree(laneItem);
    }
    (void)SoftBusMutexUnlock(&g_channelLaneList->lock);
    DestroySoftBusList(g_channelLaneList);
    g_channelLaneList = NULL;
}

int32_t TransLaneMgrAddLane(int32_t channelId, int32_t channelType, LnnLanesObject *lanesObj, const char *pkgName)
{
    if (g_channelLaneList == NULL) {
        return SOFTBUS_ERR;
    }

    TransLaneInfo *newLane = (TransLaneInfo *)SoftBusCalloc(sizeof(TransLaneInfo));
    if (newLane == NULL) {
        return SOFTBUS_MEM_ERR;
    }
    newLane->channelId = channelId;
    newLane->channelType = channelType;
    newLane->lanesObj = lanesObj;
    if (strcpy_s(newLane->pkgName, sizeof(newLane->pkgName), pkgName) != EOK) {
        SoftBusFree(newLane);
        return SOFTBUS_ERR;
    }
    if (SoftBusMutexLock(&(g_channelLaneList->lock)) != 0) {
        SoftBusFree(newLane);
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock failed");
        return SOFTBUS_LOCK_ERR;
    }

    TransLaneInfo *laneItem = NULL;
    LIST_FOR_EACH_ENTRY(laneItem, &(g_channelLaneList->list), TransLaneInfo, node) {
        if (laneItem->channelId == channelId && laneItem->channelType == channelType) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO,
                "trans lane info has exited.[channelId = %d, channelType = %d]", channelId, channelType);
            SoftBusFree(newLane);
            (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
            return SOFTBUS_ERR;
        }
    }
    ListInit(&(newLane->node));
    ListAdd(&(g_channelLaneList->list), &(newLane->node));
    g_channelLaneList->cnt++;
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "lane num is %d", g_channelLaneList->cnt);
    (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
    return SOFTBUS_OK;
}

int32_t TransLaneMgrDelLane(int32_t channelId, int32_t channelType)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "del trans land mgr.[chanid=%d][type=%d]", channelId, channelType);
    if (g_channelLaneList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane manager hasn't initialized.");
        return SOFTBUS_ERR;
    }
    if (SoftBusMutexLock(&(g_channelLaneList->lock)) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock failed");
        return SOFTBUS_LOCK_ERR;
    }
    TransLaneInfo *laneItem = NULL;
    TransLaneInfo *next = NULL;
    LIST_FOR_EACH_ENTRY_SAFE(laneItem, next, &(g_channelLaneList->list), TransLaneInfo, node) {
        if (laneItem->channelId == channelId && laneItem->channelType == channelType) {
            ListDelete(&(laneItem->node));
            g_channelLaneList->cnt--;
            (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
            LnnReleaseLanesObject(laneItem->lanesObj);
            SoftBusFree(laneItem);
            return SOFTBUS_OK;
        }
    }
    (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane not found.[channelId = %d, channelType = %d]",
        channelId, channelType);
    return SOFTBUS_ERR;
}

void TransLaneMgrDeathCallback(const char *pkgName)
{
    if (g_channelLaneList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane manager hasn't initialized.");
        return;
    }
    if (SoftBusMutexLock(&(g_channelLaneList->lock)) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock failed");
        return;
    }
    TransLaneInfo *laneItem = NULL;
    TransLaneInfo *next = NULL;
    LIST_FOR_EACH_ENTRY_SAFE(laneItem, next, &(g_channelLaneList->list), TransLaneInfo, node) {
        if (strcmp(laneItem->pkgName, pkgName) == 0) {
            ListDelete(&(laneItem->node));
            g_channelLaneList->cnt--;
            (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "%s death del lane[id=%d, type = %d]",
                pkgName, laneItem->channelId, laneItem->channelType);
            LnnReleaseLanesObject(laneItem->lanesObj);
            SoftBusFree(laneItem);
            return;
        }
    }
    (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
    return;
}

LnnLanesObject *TransLaneMgrGetLane(int32_t channelId, int32_t channelType)
{
    if (g_channelLaneList == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane manager hasn't initialized.");
        return NULL;
    }
    if (SoftBusMutexLock(&(g_channelLaneList->lock)) != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "lock failed");
        return NULL;
    }

    TransLaneInfo *laneItem = NULL;
    LIST_FOR_EACH_ENTRY(laneItem, &(g_channelLaneList->list), TransLaneInfo, node) {
        if (laneItem->channelId == channelId && laneItem->channelType == channelType) {
            (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
            return laneItem->lanesObj;
        }
    }
    (void)SoftBusMutexUnlock(&(g_channelLaneList->lock));
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans lane not found.[channelId = %d, channelType = %d]",
        channelId, channelType);
    return NULL;
}