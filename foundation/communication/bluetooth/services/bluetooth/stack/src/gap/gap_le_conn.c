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

#include "gap_le.h"
#include "gap_internal.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "btm.h"
#include "btm/btm_controller.h"
#include "btm/btm_thread.h"

#ifdef GAP_LE_SUPPORT

typedef struct {
    GapLeConnCallback callback;
    void *context;
} LeConnUpdateCallback;

static LeConnUpdateCallback g_leConnUpdateCallback;

void GapWriteAuthenticatedPayloadTimeoutComplete(const HciWriteAuthenticatedPayloadTimeoutReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:handle:0x%04x status:0x%02x", __FUNCTION__, param->connectionHandle, param->status);
    }
}

void GapOnAuthenticatedPayloadTimeoutExpiredEvent(const HciAuthenticatedPayloadTimeoutExpiredEventParam *eventParam)
{
    LOG_WARN("%{public}s:handle:0x%04x", __FUNCTION__, eventParam->connectionHandle);
}

int GAP_RegisterLeConnCallback(const GapLeConnCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_leConnUpdateCallback.callback,
            sizeof(g_leConnUpdateCallback.callback),
            0x00,
            sizeof(g_leConnUpdateCallback.callback));
    } else {
        g_leConnUpdateCallback.callback = *callback;
    }
    g_leConnUpdateCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterLeConnCallback(void)
{
    (void)memset_s(&g_leConnUpdateCallback.callback,
        sizeof(g_leConnUpdateCallback.callback),
        0x00,
        sizeof(g_leConnUpdateCallback.callback));
    g_leConnUpdateCallback.context = NULL;
    return GAP_SUCCESS;
}

static int GapLeConnectionUpdate(uint16_t handle, const GapLeConnectionParameter *connParam)
{
    HciLeConnectionUpdateParam hciCmdParam = {
        .connectionHandle = handle,
        .connIntervalMin = connParam->connIntervalMin,
        .connIntervalMax = connParam->connIntervalMax,
        .connLatency = connParam->connLatency,
        .supervisionTimeout = connParam->timeout,
        .minimumCELength = connParam->minCeLen,
        .maximumCELength = connParam->maxCeLen,
    };

    return HCI_LeConnectionUpdate(&hciCmdParam);
}

void GapOnLeConnectionUpdateCompleteEvent(const HciLeConnectionUpdateCompleteEventParam *eventParam)
{
    BtAddr addr = {0};

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(
        connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, (void *)&eventParam->connectionHandle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (deviceInfo->paramUpdateReq != NULL) {
            MEM_MALLOC.free(deviceInfo->paramUpdateReq);
            deviceInfo->paramUpdateReq = NULL;
        }
    }

    if (deviceInfo != NULL) {
        if (g_leConnUpdateCallback.callback.leConnectionUpdateComplete) {
            g_leConnUpdateCallback.callback.leConnectionUpdateComplete(eventParam->status,
                &addr,
                eventParam->connInterval,
                eventParam->connLatency,
                eventParam->supervisionTimeout,
                g_leConnUpdateCallback.context);
        }
    }
}

int GAP_LeConnParamUpdate(const BtAddr *addr, const GapLeConnectionParameter *connParam)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret = GAP_SUCCESS;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_CENTRAL | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
        LeDeviceInfo *deviceInfo = NULL;
        deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
        if (deviceInfo != NULL) {
            if (deviceInfo->role == LE_CONNECTION_ROLE_MASTER) {
                ret = GapLeConnectionUpdate(deviceInfo->handle, connParam);
            } else if (BTM_IsControllerSupportConnectionParametersRequestProcedure() && false) {
                ret = GapLeConnectionUpdate(deviceInfo->handle, connParam);
            } else {
                L2capLeConnectionParameter parameter = {
                    .connIntervalMax = connParam->connIntervalMax,
                    .connIntervalMin = connParam->connIntervalMin,
                    .connLatency = connParam->connLatency,
                    .supervisionTimeout = connParam->timeout,
                };
                GapLeConnectionParameterUpdateReq(deviceInfo->handle, &parameter);
            }
        } else {
            ret = GAP_ERR_INVAL_STATE;
        }
    }

    return ret;
}

static int GapLeRemoteConnectionParameterRequestNegativeReply(uint16_t handle, uint8_t reason)
{
    HciLeRemoteConnectionParameterRequestNegativeReplyParam hciCmdParam = {
        .connectionHandle = handle,
        .reason = reason,
    };

    return HCI_LeRemoteConnectionParameterRequestNegativeReply(&hciCmdParam);
}

void GapLeRemoteConnectionParameterRequestNegativeReplyComplete(
    const HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam *param)
{
    if (param->status) {
        LOG_ERROR("%{public}s:status:%02x, handle:%04x", __FUNCTION__, param->status, param->connectionHandle);
    }
}

static int GapLeRemoteConnectionParameterRequestReply(uint16_t handle, const GapLeConnectionParameter *connParam)
{
    HciLeRemoteConnectionParameterRequestReplyParam hciCmdParam = {
        .connectionHandle = handle,
        .intervalMin = connParam->connIntervalMin,
        .intervalMax = connParam->connIntervalMax,
        .latency = connParam->connLatency,
        .timeout = connParam->timeout,
        .minimumCELength = connParam->minCeLen,
        .maximumCELength = connParam->maxCeLen,
    };

    return HCI_LeRemoteConnectionParameterRequestReply(&hciCmdParam);
}

void GapLeRemoteConnectionParameterRequestReplyComplete(
    const HciLeRemoteConnectionParameterRequestReplyReturnParam *param)
{
    if (param->status) {
        LOG_WARN("%{public}s:status:%02x, handle:%04x", __FUNCTION__, param->status, param->connectionHandle);
        GapLeRemoteConnectionParameterRequestNegativeReply(param->connectionHandle, HCI_UNSPECIFIED_ERROR);
    }
}

void GapOnLeRemoteConnectionParameterRequestEvent(const HciLeRemoteConnectionParameterRequestEventParam *eventParam)
{
    BtAddr addr = {0};
    bool doReject = false;

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(
        connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, (void *)&eventParam->connectionHandle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (deviceInfo->paramUpdateReq != NULL) {
            doReject = true;
        } else {
            deviceInfo->paramUpdateReq = MEM_MALLOC.alloc(sizeof(LeConnParamUpdateReq));
            if (deviceInfo->paramUpdateReq != NULL) {
                deviceInfo->paramUpdateReq->status = GAP_LE_CONN_PARAM_UPDATE_RECV_HCI;
                deviceInfo->paramUpdateReq->id = 0x00;
            } else {
                doReject = true;
            }
        }
    }

    if (doReject) {
        GapLeRemoteConnectionParameterRequestNegativeReply(eventParam->connectionHandle, HCI_UNSPECIFIED_ERROR);
    } else {
        if (g_leConnUpdateCallback.callback.leConnectionParameterReq) {
            g_leConnUpdateCallback.callback.leConnectionParameterReq(&addr,
                eventParam->intervalMin,
                eventParam->IntervalMax,
                eventParam->latency,
                eventParam->timeout,
                g_leConnUpdateCallback.context);
        } else {
            GapLeRemoteConnectionParameterRequestNegativeReply(eventParam->connectionHandle, HCI_UNSPECIFIED_ERROR);
        }
    }
}

static int GapAcceptConnectionParameterUpdate(const LeDeviceInfo *deviceInfo, const GapLeConnectionParameter *connParam)
{
    int ret;

    if (deviceInfo->paramUpdateReq->status == GAP_LE_CONN_PARAM_UPDATE_RECV_HCI) {
        ret = GapLeRemoteConnectionParameterRequestReply(deviceInfo->handle, connParam);
    } else {
        GapLeConnectionParameterUpdateRsp(
            deviceInfo->handle, deviceInfo->paramUpdateReq->id, L2CAP_LE_CONNECTION_PARAMETERS_ACCEPTED);
        ret = GapLeConnectionUpdate(deviceInfo->handle, connParam);
    }

    return ret;
}

static int GapRejectConnectionParameterUpdate(const LeDeviceInfo *deviceInfo)
{
    int ret = GAP_SUCCESS;

    if (deviceInfo->paramUpdateReq->status == GAP_LE_CONN_PARAM_UPDATE_RECV_HCI) {
        ret = GapLeRemoteConnectionParameterRequestNegativeReply(
            deviceInfo->handle, HCI_UNSUPPORTED_LMP_PARAMETER_VALUE_OR_UNSUPPORTED_LL_PARAMETER_VALUE);
    } else {
        GapLeConnectionParameterUpdateRsp(
            deviceInfo->handle, deviceInfo->paramUpdateReq->id, L2CAP_LE_CONNECTION_PARAMETERS_REJECTED);
    }

    return ret;
}

int GAP_LeConnectionParameterRsp(const BtAddr *addr, uint8_t accept, const GapLeConnectionParameter *connParam)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_CENTRAL | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
        LeDeviceInfo *deviceInfo = NULL;
        deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
        if (deviceInfo != NULL && deviceInfo->paramUpdateReq != NULL) {
            if (accept == GAP_ACCEPT) {
                ret = GapAcceptConnectionParameterUpdate(deviceInfo, connParam);
            } else if (accept == GAP_NOT_ACCEPT) {
                ret = GapRejectConnectionParameterUpdate(deviceInfo);
            } else {
                ret = GAP_ERR_INVAL_PARAM;
            }
        } else {
            ret = GAP_ERR_INVAL_STATE;
        }
    }

    return ret;
}

static int GapLeSetHostChannelClassification(uint64_t channelMap)
{
    HciLeSetHostChannelClassificationParam hciCmdParam;
    (void)memcpy_s(hciCmdParam.channelMap, sizeof(hciCmdParam.channelMap), &channelMap, sizeof(hciCmdParam.channelMap));

    return HCI_LeSetHostChannelClassification(&hciCmdParam);
}

void GapLeSetHostChannelClassificationComplete(const HciLeSetHostChannelClassificationReturnParam *param)
{
    if (g_leConnUpdateCallback.callback.GapleSetHostChannelClassificationResult) {
        g_leConnUpdateCallback.callback.GapleSetHostChannelClassificationResult(
            param->status, g_leConnUpdateCallback.context);
    }
}

int GAP_LeSetHostChannelClassification(uint64_t channelMap)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_CENTRAL | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetHostChannelClassification(channelMap);
    }

    return ret;
}

void GapLeReadChannelMapComplete(const HciLeReadChannelMapReturnParam *param)
{
    BtAddr addr = {0};
    uint64_t channelMap = 0;

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(
        connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, (void *)&param->connectionHandle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        (void)memcpy_s(&channelMap, sizeof(channelMap), param->channelMap, sizeof(param->channelMap));
    }

    if (deviceInfo != NULL) {
        if (g_leConnUpdateCallback.callback.GapleReadChannelMapResult) {
            g_leConnUpdateCallback.callback.GapleReadChannelMapResult(
                param->status, &addr, channelMap, g_leConnUpdateCallback.context);
        }
    }
}

int GAP_LeSetBondMode(uint8_t mode)
{
    LOG_INFO("%{public}s: mode[%hhu]", __FUNCTION__, mode);
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    LeLocalInfo *localInfo = GapGetLeLocalInfo();
    localInfo->bondableMode = mode;

    return GAP_SUCCESS;
}

int GAP_LeSetSecurityMode(GAP_LeSecMode1Level mode1Level, GAP_LeSecMode2Level mode2Level)
{
    LOG_INFO("%{public}s: mode1Level[%{public}d], mode2Level[%{public}d]", __FUNCTION__, mode1Level, mode2Level);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }
    LeLocalInfo *localInfo = GapGetLeLocalInfo();
    localInfo->mode1Level = mode1Level;
    localInfo->mode2Level = mode2Level;
    if (mode1Level == LE_MODE_1_LEVEL_4) {
        BtmLocalVersionInformation version;
        ret = BTM_GetLocalVersionInformation(&version);
        if ((ret == BT_NO_ERROR) && (version.hciVersion >= BLUETOOTH_CORE_SPECIFICATION_4_2)) {
            ret = SMP_SetSecureConnOnlyMode(true);
        } else {
            ret = GAP_ERR_NOT_SUPPORT;
        }
    } else {
        ret = SMP_SetSecureConnOnlyMode(false);
    }
    return ret;
}

int GAP_LeGetSecurityStatus(const BtAddr *addr, GAP_LeSecurityStatus *status, uint8_t *encKeySize)
{
    int ret = GAP_SUCCESS;
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        *status = deviceInfo->encryptionStatus;
        *encKeySize = deviceInfo->keySize;
        LOG_INFO("%{public}s:" BT_ADDR_FMT " status:%{public}d, keySize:%hhu",
            __FUNCTION__,
            BT_ADDR_FMT_OUTPUT(addr->addr),
            *status,
            *encKeySize);
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

bool GapLeDeviceNeedBond(const LeDeviceInfo *deviceInfo)
{
    LeBondBlock *leBondBlock = GapGetLeBondBlock();
    return (leBondBlock->isPairing == true) && (GapAddrCompare(&deviceInfo->addr, &leBondBlock->addr));
}

static void GapWaitExAdvTerminatedTimeoutTask(void *ctx)
{
    LeDeviceInfo *deviceInfo = ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindCmpListData, ctx);
    if (deviceInfo != NULL) {
        deviceInfo->ownAddrUpdated = true;
        GapLeRequestSecurityProcess(deviceInfo);
        if (GapLeDeviceNeedBond(deviceInfo)) {
            GapLeDoPair(&deviceInfo->addr);
        }
    }
}

void GapWaitExAdvTerminatedTimeout(void *dev)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    ctx->pointer = dev;

    int ret = GapRunTaskUnBlockProcess(GapWaitExAdvTerminatedTimeoutTask, ctx, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

void GapLeConnectionComplete(uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context)
{
    LeConnectionInfoBlock *leConnectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    leConnectionInfoBlock = GapGetLeConnectionInfoBlock();
    if (status == HCI_SUCCESS) {
        deviceInfo = MEM_MALLOC.alloc(sizeof(LeDeviceInfo));
        if (deviceInfo != NULL) {
            (void)memset_s(deviceInfo, sizeof(LeDeviceInfo), 0x00, sizeof(LeDeviceInfo));
            (void)memcpy_s(&deviceInfo->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
            deviceInfo->handle = connectionHandle;
            BTM_GetLeConnectionAddress(connectionHandle, &deviceInfo->ownAddr, &deviceInfo->peerAddr);
            LOG_DEBUG("%{public}s: own:" BT_ADDR_FMT " %hhu",
                __FUNCTION__,
                BT_ADDR_FMT_OUTPUT(deviceInfo->ownAddr.addr),
                deviceInfo->ownAddr.type);
            LOG_DEBUG("%{public}s: peer:" BT_ADDR_FMT " %hhu",
                __FUNCTION__,
                BT_ADDR_FMT_OUTPUT(deviceInfo->peerAddr.addr),
                deviceInfo->peerAddr.type);
            deviceInfo->role = role;
            deviceInfo->alarm = AlarmCreate("LEWaitExAdvTerminated", false);
            ListAddLast(leConnectionInfoBlock->deviceList, deviceInfo);
            if (deviceInfo->role == HCI_ROLE_SLAVE) {
                AlarmSet(deviceInfo->alarm, GAP_WAIT_EX_ADV_TERMINATED, GapWaitExAdvTerminatedTimeout, deviceInfo);
            } else if (deviceInfo->role == HCI_ROLE_MASTER && GapLeDeviceNeedBond(deviceInfo)) {
                GapLeDoPair(&deviceInfo->addr);
            } else {
                deviceInfo->ownAddrUpdated = true;
            }
        } else {
            LOG_ERROR("%{public}s:alloc failed.", __FUNCTION__);
        }
    } else {
        LeBondBlock *leBondBlock = GapGetLeBondBlock();
        if (leBondBlock->isPairing == true && GapAddrCompare(addr, &leBondBlock->addr)) {
            GapDoPairResultCallback(addr, status);
        }
        GapClearPairingStatus(addr);
    }

    if (status == HCI_SUCCESS) {
        GapRequestSigningAlgorithmInfo(addr);
    }
}

void GapLeDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    LeConnectionInfoBlock *leConnectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    leConnectionInfoBlock = GapGetLeConnectionInfoBlock();
    if (status == HCI_SUCCESS) {
        deviceInfo =
            ListForEachData(leConnectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &connectionHandle);
        if (deviceInfo != NULL) {
            ListRemoveNode(leConnectionInfoBlock->deviceList, deviceInfo);
        }
    }
}

void GapReceiveL2capParameterUpdateReq(
    uint16_t aclHandle, uint8_t id, const L2capLeConnectionParameter *param, void *ctx)
{
    BtAddr addr = {0};
    bool doReject = false;

    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByHandle, &aclHandle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (deviceInfo->paramUpdateReq != NULL) {
            doReject = true;
        } else {
            deviceInfo->paramUpdateReq = MEM_MALLOC.alloc(sizeof(LeConnParamUpdateReq));
            if (deviceInfo->paramUpdateReq != NULL) {
                deviceInfo->paramUpdateReq->status = GAP_LE_CONN_PARAM_UPDATE_RECV_L2CAP;
                deviceInfo->paramUpdateReq->id = id;
            } else {
                doReject = true;
            }
        }
    }

    if (doReject) {
        GapLeConnectionParameterUpdateRsp(aclHandle, id, L2CAP_LE_CONNECTION_PARAMETERS_REJECTED);
    } else {
        if (g_leConnUpdateCallback.callback.leConnectionParameterReq) {
            g_leConnUpdateCallback.callback.leConnectionParameterReq(&addr,
                param->connIntervalMin,
                param->connIntervalMax,
                param->connLatency,
                param->supervisionTimeout,
                g_leConnUpdateCallback.context);
        } else {
            GapLeConnectionParameterUpdateRsp(aclHandle, id, L2CAP_LE_CONNECTION_PARAMETERS_REJECTED);
        }
    }
}

void GapReceiveL2capParameterUpdateRsp(uint16_t aclHandle, uint16_t result, void *ctx)
{
    BtAddr addr = {0};

    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByHandle, &aclHandle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
    }

    if (result == L2CAP_LE_CONNECTION_PARAMETERS_REJECTED) {
        if (g_leConnUpdateCallback.callback.leConnectionUpdateComplete) {
            g_leConnUpdateCallback.callback.leConnectionUpdateComplete(
                GAP_STATUS_FAILED, &addr, 0, 0, 0, g_leConnUpdateCallback.context);
        }
    }
}

#endif