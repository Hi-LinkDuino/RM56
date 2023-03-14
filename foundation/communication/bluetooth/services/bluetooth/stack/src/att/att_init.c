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

/**
 * @file att_init.c
 *
 * @brief implement initialize function to be called.
 *
 */

#include <string.h>

#include "att_connect.h"
#include "att_common.h"
#include "att_receive.h"

#include "alarm.h"
#include "btstack.h"
#include "module.h"
#include "packet.h"

#include "log.h"

#include "../btm/btm_thread.h"

static void AttInstructListInit();
static void AttListDataFree(const void *data);
static void AttAlarmListInit();

static void L2cifLeRisterFixChannelCallback(uint16_t cid, int result);

static void L2cifRegisterServiceCallback(uint16_t lpsm, int result);
static void AttDelete(AttConnectInfo *connectInfo, AttConnectingInfo *connectingInfo);

static void ATT_Init(const int traceLevel);

static void ATT_StartUp();
static void ATT_StartUpAsync(const void *context);
static void ATT_StartUpAsyncDestroy(const void *context);

static void ATT_ShutDown();
static void ATT_ShutDownAsync(const void *context);

static void ATT_Clearup();

/**
 * @brief att instructions list initialization.
 *
 */
static void AttInstructListInit()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    AttConnectInfo *connectInfoPtr = NULL;

    connectInfoPtr = AttGetConnectStart();

    for (; index < MAXCONNECT; ++index) {
        connectInfoPtr[index].instruct = ListCreate((void *)AttListDataFree);
    }

    return;
}

/**
 * @brief att list data free.
 *
 * @param data Indicates the pointer to dta.
 */
static void AttListDataFree(const void *data)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    Packet *packet = (Packet *)data;
    PacketFree(packet);

    return;
}

/**
 * @brief att alarm list initialization.
 *
 */
static void AttAlarmListInit()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    uint16_t increaseIndex;
    AttConnectInfo *connectInfo = NULL;
    AttConnectingInfo *connectingInfo = NULL;

    connectInfo = AttGetConnectStart();
    connectingInfo = AttGetConnectingStart();

    for (; index < MAXCONNECT; ++index) {
        connectInfo[index].alarm = AlarmCreate((char *)&index, 0);
    }

    for (increaseIndex = MAXCONNECT, index = 0; (increaseIndex < STEP_TWO * MAXCONNECT) && (index < MAXCONNECT);
         ++increaseIndex, ++index) {
        connectingInfo[index].bredrAlarm = AlarmCreate((char *)&increaseIndex, 0);
    }

    for (increaseIndex = STEP_TWO * MAXCONNECT, index = 0;
         (increaseIndex < STEP_THREE * MAXCONNECT) && (index < MAXCONNECT);
         ++increaseIndex, ++index) {
        connectingInfo[index].leAlarm = AlarmCreate((char *)&increaseIndex, 0);
    }

    return;
}

/**
 * @brief att start up async.
 *
 * @param context Indicates the pointer to context.
 */
static void ATT_StartUpAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttInstructListInit();
    AttAlarmListInit();
    FunctionListInit();

    L2capLeFixChannel lel2capLeFixChannelObj;
    lel2capLeFixChannelObj.cid = LE_CID;
    lel2capLeFixChannelObj.leConnected = AttLeConnected;
    lel2capLeFixChannelObj.leDisconnected = AttLeDisconnected;
    lel2capLeFixChannelObj.recvLeData = AttRecvLeData;
    L2CIF_LeRegisterFixChannel(LE_CID, &lel2capLeFixChannelObj, L2cifLeRisterFixChannelCallback);

    return;
}

/**
 * @brief att start up async destroy.
 *
 * @param context Indicates the pointer to context.
 */
static void ATT_StartUpAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief att start up.
 *
 */
static void ATT_StartUp()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int result;

    result = BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_ATT, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);
    if (result != BT_NO_ERROR) {
        return;
    }

    AttAsyncProcess(ATT_StartUpAsync, ATT_StartUpAsyncDestroy, NULL);

    return;
}

/**
 * @brief att initialization.
 *
 * @param trace_level Indicates the level.
 */
static void ATT_Init(const int traceLevel)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief att shutdown async.
 *
 */
static void ATT_ShutDownAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;
    AttConnectInfo *connectInfo = AttGetConnectStart();
    AttConnectingInfo *connectingInfo = AttGetConnectingStart();

    for (; index < MAXCONNECT; ++index) {
        AttShutDownClearConnectInfo(&connectInfo[index]);
        ListClear(connectInfo[index].instruct);
        if (connectInfo[index].alarm) {
            AlarmCancel(connectInfo[index].alarm);
        }
        if (connectingInfo[index].bredrAlarm) {
            AlarmCancel(connectingInfo[index].bredrAlarm);
        }
        if (connectingInfo[index].leAlarm) {
            AlarmCancel(connectingInfo[index].leAlarm);
        }
    }

    AttDelete(AttGetConnectStart(), AttGetConnectingStart());

    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_ATT);

    return;
}

/**
 * @brief att shutdown.
 *
 */
static void ATT_ShutDown()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttAsyncProcess(ATT_ShutDownAsync, NULL, NULL);

    return;
}

/**
 * @brief att clear up.
 *
 */
static void ATT_Clearup()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief callback of att register l2caple.
 *
 * @param1 cid Indicates the cid.
 * @param2 result Indicates the result of callback.
 */
static void L2cifLeRisterFixChannelCallback(uint16_t cid, int result)
{
    LOG_INFO("%{public}s enter, cid = %hu, result = %{public}d", __FUNCTION__, cid, result);

    L2capService bredrl2capServiceTObj;
    bredrl2capServiceTObj.recvConnectionReq =
        (void (*)(uint16_t, uint8_t, const L2capConnectionInfo *, uint16_t, void *))AttReceiveConnectionReq;
    bredrl2capServiceTObj.recvConnectionRsp =
        (void (*)(uint16_t, const L2capConnectionInfo *, uint16_t, uint16_t, void *))AttReceiveConnectionRsp;
    bredrl2capServiceTObj.recvConfigReq =
        (void (*)(uint16_t, uint8_t, const L2capConfigInfo *, void *))AttReceiveConfigReq;
    bredrl2capServiceTObj.recvConfigRsp =
        (void (*)(uint16_t, const L2capConfigInfo *, uint16_t, void *))AttReceiveConfigRsp;
    bredrl2capServiceTObj.recvDisconnectionReq = (void (*)(uint16_t, uint8_t, void *))AttReceiveDisconnectionReq;
    bredrl2capServiceTObj.recvDisconnectionRsp = (void (*)(uint16_t, void *))AttRecvDisconnectionRsp;
    bredrl2capServiceTObj.disconnectAbnormal = (void (*)(uint16_t, uint8_t, void *))AttDisconnectAbnormal;
    bredrl2capServiceTObj.recvData = (void (*)(uint16_t, Packet *, void *))AttRecvData;

    L2CIF_RegisterService(BT_PSM_ATT, &bredrl2capServiceTObj, NULL, L2cifRegisterServiceCallback);

    return;
}

/**
 * @brief callback of att register l2capbredr.
 *
 * @param1 lpsm Indicates the lpsm.
 * @param2 result Indicates the result of callback.
 */
void L2cifRegisterServiceCallback(uint16_t lpsm, int result)
{
    LOG_INFO("%{public}s enter, lpsm = %hu, result = %{public}d", __FUNCTION__, lpsm, result);

    return;
}

/**
 * @brief att delete information.
 *
 * @param1 connectInfo Indicates the pointer of AttConnectInfo.
 * @param2 connectingInfo Indicates the pointer of AttConnectingInfo.
 */
static void AttDelete(AttConnectInfo *connectInfo, AttConnectingInfo *connectingInfo)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        ListDelete(connectInfo[index].instruct);
        connectInfo[index].instruct = NULL;
        if (connectInfo[index].alarm) {
            AlarmDelete(connectInfo[index].alarm);
            connectInfo[index].alarm = NULL;
        }
        if (connectingInfo[index].bredrAlarm) {
            AlarmDelete(connectingInfo[index].bredrAlarm);
            connectingInfo[index].bredrAlarm = NULL;
        }
        if (connectingInfo[index].leAlarm) {
            AlarmDelete(connectingInfo[index].leAlarm);
            connectingInfo[index].leAlarm = NULL;
        }
    }

    return;
}

static Module g_att = {
    .name = MODULE_NAME_ATT,
    .init = ATT_Init,
    .startup = ATT_StartUp,
    .shutdown = ATT_ShutDown,
    .cleanup = ATT_Clearup,
    .dependencies = {MODULE_NAME_L2CAP},
};

MODULE_DECL(g_att);
