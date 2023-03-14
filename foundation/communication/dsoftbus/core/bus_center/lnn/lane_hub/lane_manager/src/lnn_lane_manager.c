/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "lnn_lane_manager.h"

#include <securec.h>

#include "bus_center_manager.h"
#include "lnn_distributed_net_ledger.h"
#include "lnn_lane_info.h"
#include "lnn_lane_link.h"
#include "lnn_smart_communication.h"
#include "softbus_adapter_mem.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_utils.h"

typedef struct {
    ListNode node;
    LnnLanesObject *object;
    LnnLaneQosObserverNotify notify;
} LaneObserverListItem;

static SoftBusList *g_laneQosObserver;

LnnLanesObject *LnnRequestLanesObject(const char *netWorkId, int32_t pid, LnnLaneProperty prop,
    const LnnPreferredLinkList *linkList, uint32_t laneNum)
{
    if (prop < LNN_MESSAGE_LANE || prop >= LNN_LANE_PROPERTY_BUTT || netWorkId == NULL ||
        laneNum == 0 || laneNum > LNN_REQUEST_MAX_LANE_NUM) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param error, prop = %d, laneNum = %u", prop, laneNum);
        return NULL;
    }
    uint32_t memLen = sizeof(LnnLanesObject) + sizeof(int32_t) * laneNum;
    LnnLanesObject *lanesObject = (LnnLanesObject *)SoftBusMalloc(memLen);
    if (lanesObject == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftBusMalloc error.");
        return NULL;
    }
    (void)memset_s(lanesObject, memLen, 0, memLen);
    lanesObject->prop = prop;
    lanesObject->laneNum = laneNum;

    for (uint32_t i = 0; i < laneNum; i++) {
        int32_t laneId = LnnGetRightLane(netWorkId, pid, prop, linkList);
        if (laneId < 0) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetRightLane error. laneId = %d", laneId);
            lanesObject->laneNum = i;
            LnnReleaseLanesObject(lanesObject);
            return NULL;
        }
        lanesObject->laneId[i] = laneId;
        (void)LnnSetLaneCount(laneId, 1); // laneCount add 1
    }
    return lanesObject;
}

void LnnReleaseLanesObject(LnnLanesObject *lanesObject)
{
    if (lanesObject == NULL) {
        return;
    }
    for (uint32_t i = 0; i < lanesObject->laneNum; i++) {
        (void)LnnSetLaneCount(lanesObject->laneId[i], -1); // laneCount subtract 1
        LnnReleaseLane(lanesObject->laneId[i]);
    }
    SoftBusFree(lanesObject);
}

int32_t LnnGetLaneId(LnnLanesObject *lanesObject, uint32_t num)
{
    if (lanesObject == NULL || num >= lanesObject->laneNum) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param error. num = %u", num);
        return SOFTBUS_ERR;
    }
    return lanesObject->laneId[num];
}

uint32_t LnnGetLaneNum(LnnLanesObject *lanesObject)
{
    if (lanesObject == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param error");
        return SOFTBUS_ERR;
    }
    return lanesObject->laneNum;
}

static int32_t AddLaneQosObserverItem(LnnLanesObject *object, LnnLaneQosObserverNotify notify)
{
    LaneObserverListItem *item = NULL;
    SoftBusList *list = g_laneQosObserver;
    item = (LaneObserverListItem *)SoftBusMalloc(sizeof(LaneObserverListItem));
    if (item == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "fail: malloc LaneQosObserver list item");
        return SOFTBUS_MALLOC_ERR;
    }
    ListInit(&item->node);
    item->object = object;
    item->notify = notify;
    ListAdd(&list->list, &item->node);
    list->cnt++;
    return SOFTBUS_OK;
}

static void ClearLaneQosObserverList(LnnLanesObject *object)
{
    LaneObserverListItem *item = NULL;
    LaneObserverListItem *next = NULL;
    ListNode *list = &g_laneQosObserver->list;
    LIST_FOR_EACH_ENTRY_SAFE(item, next, list, LaneObserverListItem, node) {
        if (object != item->object) {
            continue;
        }
        ListDelete(&item->node);
        if (g_laneQosObserver->cnt > 0) {
            g_laneQosObserver->cnt--;
        }
        SoftBusFree(item);
    }
}

int32_t LnnLaneQosObserverAttach(LnnLanesObject *object, LnnLaneQosObserverNotify notify)
{
    if (object == NULL || notify == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param error");
        return SOFTBUS_ERR;
    }
    if (AddLaneQosObserverItem(object, notify) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "AddLaneQosObserverItem failed");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void LnnLaneQosObserverDetach(LnnLanesObject *object)
{
    if (object == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param error");
    }
    ClearLaneQosObserverList(object);
}

static void LaneMonitorCallback(int32_t laneId, int32_t socre)
{
    LaneObserverListItem *item = NULL;
    SoftBusList *list = g_laneQosObserver;
    LIST_FOR_EACH_ENTRY(item, &list->list, LaneObserverListItem, node) {
        for (uint32_t i = 0; i < item->object->laneNum; i++) {
            if (laneId == item->object->laneId[i]) {
                item->notify(laneId, socre);
            }
        }
    }
}

int32_t LnnInitLaneManager(void)
{
    if (LnnLanesInit() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnLanesInit error");
        return SOFTBUS_ERR;
    }
    if (g_laneQosObserver == NULL) {
        g_laneQosObserver = CreateSoftBusList();
        if (g_laneQosObserver == NULL) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "CreateSoftBusList error");
            return SOFTBUS_ERR;
        }
    }
    if (LnnRegisterLaneMonitor(LaneMonitorCallback) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnRegisterLaneMonitor error");
        DestroySoftBusList(g_laneQosObserver);
        g_laneQosObserver = NULL;
        return SOFTBUS_ERR;
    }
    if (LnnLanePendingInit() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnLanePendingInit error");
        DestroySoftBusList(g_laneQosObserver);
        g_laneQosObserver = NULL;
        return SOFTBUS_ERR;
    }
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "InitLaneManager success");
    return SOFTBUS_OK;
}