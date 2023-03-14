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

#include "smp.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "btstack.h"
#include "l2cap_le_if.h"
#include "packet.h"

#include "platform/include/alarm.h"
#include "platform/include/allocator.h"
#include "platform/include/event.h"
#include "platform/include/list.h"
#include "platform/include/module.h"
#include "platform/include/mutex.h"
#include "platform/include/random.h"

#include "btm/btm_thread.h"
#include "hci/hci.h"

#include "smp_aes_encryption.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_def.h"
#include "smp_hci_event.h"
#include "smp_legacy.h"
#include "smp_oob.h"
#include "smp_privacy.h"
#include "smp_receive.h"
#include "smp_sc_accepter.h"
#include "smp_sc_initiator.h"
#include "smp_send.h"
#include "smp_tool.h"

static const uint8_t SALT[SMP_ENCRYPT_KEY_LEN] = {
    0x6C, 0x88, 0x83, 0x91, 0xAA, 0xF5, 0xA5, 0x38, 0x60, 0x37, 0x0B, 0xDB, 0x5A, 0x60, 0x83, 0xBE
};  // The key of an encryption algorithm.

static SMP_PairMng g_smpPairMng = {0x00};
static SMP_Callback_t g_smpCallBack = {0x00};
static bool g_smpSecureConnOnlyMode = false;

static int SMP_AuthReqReplyProcessMaster(
    bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue);
static int SMP_AuthReqReplyJudgeExceptionMaster(bool accept, uint8_t rejectReason);
static int SMP_AuthReqReplyStepTransMaster(uint8_t pairMethod);
static int SMP_AuthReqReplyProcessSlave(
    bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue);
static int SMP_AuthReqReplyJudgeExceptionSlave(bool accept, uint8_t rejectReason);
static int SMP_AuthReqReplyStepTransSlave(uint8_t pairMethod);
static int SMP_AuthReqReplyNumericSlave();
static int SMP_AuthReqReplyPasskeyEntrySlave();
static int SMP_AuthReqReplyOobSlave();
static void SMP_AuthReqReplyObtainEntryValue(uint8_t pairMethod, const uint8_t *entryValue);
static int SMP_PairReqReplyJudgeException(
    uint16_t handle, bool accept, uint8_t rejectReason, const SMP_PairParam *param);
static int SMP_PairReqReplyLegacyPair();
static int SMP_PairReqReplyScPair();
static int SMP_PairReqReplyScPairOob();
static int SMP_PairRspReplyJudgeException(uint16_t handle, bool accept, uint8_t rejectReason);
static int SMP_PairRspReplyLegacyPair();
static int SMP_PairRspReplyScPair();
static int SMP_PairRspReplyScPairOob();
static int SMP_LtkReqReplyNormal(uint16_t handle, bool accept, const uint8_t *key);
static int SMP_LtkReqReplyException(uint16_t handle);
static void SMP_GenPairRetNormal(uint16_t handle, uint8_t status);
static void SMP_GenPairRetException(uint16_t handle, uint8_t status, uint8_t failedReason, Alarm *cancelTimer);
static void SMP_L2cifLeRegisterFixChannelCallback(uint16_t cid, int result);
static void SMP_L2cifLeDeregisterFixChannelCallback(uint16_t cid, int result);

static void SMP_StartupTask(void *context);
static void SMP_ShutdownTask(void *context);
static void SMP_SetIrkTask(void *context);
static void SMP_SetIdentAddrTask(void *context);
static void SMP_AsyncResoRpaTask(void *context);
static void SMP_GenRpaTask(void *context);
static void SMP_SetSecConnOnlyModeTask(void *context);
static void SMP_SendSecReqToRemoteTask(void *context);
static void SMP_GenSignTask(void *context);
static void SMP_StartEncTask(void *context);
static void SMP_StartPairTask(void *context);
static void SMP_AuthReqReplyTask(void *context);
static void SMP_RemotePairReqReplyTask(void *context);
static void SMP_RemotePairRspReplyTask(void *context);
static void SMP_RemoteSecReqReplyTask(void *context);
static void SMP_LongTermKeyReqReplyTask(void *context);
static void SMP_CancelPairTask(void *context);
static void SMP_RegCbTask(void *context);
static void SMP_UnregCbTask(void *context);
static void SMP_GenScOobDataTask(void *context);
static void SMP_PairTimeoutTask(void *context);

static HciEventCallbacks g_smpHciEventCallbacks = {
    .leRandComplete = SMP_OnLERandComplete,
    .leEncryptComplete = SMP_OnLEEncryptComplete,
    .encryptionChange = SMP_OnLEEncryptionChange,
    .encryptionKeyRefreshComplete = SMP_OnLEEncryptionKeyRefreshComplete,
    .leLongTermKeyRequest = SMP_OnLELongTermKeyRequest,
    .leLongTermKeyRequestReplyComplete = SMP_OnLELongTermKeyRequestReplyComplete,
    .leReadLocalP256PublicKeyComplete = SMP_OnLEReadLocalP256PublicKeyComplete,
    .leGenerateDHKeyComplete = SMP_OnLEGenerateDHKeyComplete,
};

static void SMP_Initialize(int traceLevel)
{
    LOG_INFO("%{public}s", __FUNCTION__);
}

static void SMP_Finalize(void)
{
    LOG_INFO("%{public}s", __FUNCTION__);
}

static void SMP_Startup()
{
    int ret;
    LOG_INFO("%{public}s", __FUNCTION__);
    Event *ctx = EventCreate(true);
    ret = BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_SMP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);
    if (ret != SMP_SUCCESS) {
        EventDelete(ctx);
        return;
    }
    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_StartupTask, ctx);
    if (ret != SMP_SUCCESS) {
        EventDelete(ctx);
        return;
    }
    EventWait(ctx, SMP_INIT_WAIT_TIME);
    EventDelete(ctx);
}

static void SMP_StartupTask(void *context)
{
    Event *ctx = context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    L2capLeFixChannel channel;
    g_smpSecureConnOnlyMode = false;
    SMP_ClearScOobData(true);
    SMP_ClearPairState(&g_smpPairMng);
    SMP_SetEncCmdList(SMP_ListCreate(SMP_FreeEncCmd));
    channel.cid = L2CAP_LE_SMP_CHANNEL;
    channel.leConnected = SMP_Connected;
    channel.leDisconnected = SMP_Disconnected;
    channel.recvLeData = SMP_ReceiveData;
    int ret = L2CIF_LeRegisterFixChannel(L2CAP_LE_SMP_CHANNEL, &channel, SMP_L2cifLeRegisterFixChannelCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("RegisterFixChannel error %{public}d", ret);
    }
    HCI_RegisterEventCallbacks(&g_smpHciEventCallbacks);
    if (ctx != NULL) {
        EventSet(ctx);
    }
}

static void SMP_Shutdown()
{
    int ret;
    LOG_INFO("%{public}s", __FUNCTION__);
    Event *ctx = EventCreate(true);
    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_ShutdownTask, ctx);
    if (ret != SMP_SUCCESS) {
        EventDelete(ctx);
        return;
    }
    EventWait(ctx, SMP_INIT_WAIT_TIME);
    EventDelete(ctx);
}

static void SMP_ShutdownTask(void *context)
{
    Event *ctx = context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    L2CIF_LeDeregisterFixChannel(L2CAP_LE_SMP_CHANNEL, SMP_L2cifLeDeregisterFixChannelCallback);
    HCI_DeregisterEventCallbacks(&g_smpHciEventCallbacks);
    g_smpSecureConnOnlyMode = false;
    SMP_ClearScOobData(true);
    SMP_ClearPairState(&g_smpPairMng);
    SMP_ListDelete(SMP_GetEncCmdList());
    SMP_SetEncCmdList(NULL);
    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_SMP);
    if (ctx != NULL) {
        EventSet(ctx);
    }
}

int SMP_RegisterCallback(const SMP_Callback_t *cb)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_RegCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_RegCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_RegCbTask_t), 0x00, sizeof(SMP_RegCbTask_t));
    (void)memcpy_s(&ctx->cb, sizeof(SMP_Callback_t), cb, sizeof(ctx->cb));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_RegCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_RegCbTask(void *context)
{
    SMP_RegCbTask_t *param = (SMP_RegCbTask_t *)context;
    if (param->cb.SMP_CallbackAuthenticationRequest != NULL) {
        g_smpCallBack.SMP_CallbackAuthenticationRequest = param->cb.SMP_CallbackAuthenticationRequest;
    }
    if (param->cb.SMP_CallbackEncryptionComplete != NULL) {
        g_smpCallBack.SMP_CallbackEncryptionComplete = param->cb.SMP_CallbackEncryptionComplete;
    }
    if (param->cb.SMP_CallbackLongTermKeyRequest != NULL) {
        g_smpCallBack.SMP_CallbackLongTermKeyRequest = param->cb.SMP_CallbackLongTermKeyRequest;
    }
    if (param->cb.SMP_CallbackPairResult != NULL) {
        g_smpCallBack.SMP_CallbackPairResult = param->cb.SMP_CallbackPairResult;
    }
    if (param->cb.SMP_CallbackRemotePairRequest != NULL) {
        g_smpCallBack.SMP_CallbackRemotePairRequest = param->cb.SMP_CallbackRemotePairRequest;
    }
    if (param->cb.SMP_CallbackRemotePairResponse != NULL) {
        g_smpCallBack.SMP_CallbackRemotePairResponse = param->cb.SMP_CallbackRemotePairResponse;
    }
    if (param->cb.SMP_CallbackRemoteSecurityRequest != NULL) {
        g_smpCallBack.SMP_CallbackRemoteSecurityRequest = param->cb.SMP_CallbackRemoteSecurityRequest;
    }
    if (param->cb.SMP_CallbackGenerateSignatureResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateSignatureResult = param->cb.SMP_CallbackGenerateSignatureResult;
    }
    if (param->cb.SMP_CallbackGenerateRPAResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateRPAResult = param->cb.SMP_CallbackGenerateRPAResult;
    }
    if (param->cb.SMP_CallbackResolveRPAResult != NULL) {
        g_smpCallBack.SMP_CallbackResolveRPAResult = param->cb.SMP_CallbackResolveRPAResult;
    }
    if (param->cb.SMP_CallbackGenerateScOobDataResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateScOobDataResult = param->cb.SMP_CallbackGenerateScOobDataResult;
    }
    MEM_MALLOC.free(param);
}

int SMP_UnregisterCallback()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_UnregCbTask, NULL);
    if (ret != SMP_SUCCESS) {
        return ret;
    }
    return ret;
}

static void SMP_UnregCbTask(void *context)
{
    g_smpCallBack.SMP_CallbackAuthenticationRequest = NULL;
    g_smpCallBack.SMP_CallbackEncryptionComplete = NULL;
    g_smpCallBack.SMP_CallbackLongTermKeyRequest = NULL;
    g_smpCallBack.SMP_CallbackPairResult = NULL;
    g_smpCallBack.SMP_CallbackRemotePairRequest = NULL;
    g_smpCallBack.SMP_CallbackRemotePairResponse = NULL;
    g_smpCallBack.SMP_CallbackRemoteSecurityRequest = NULL;
    g_smpCallBack.SMP_CallbackGenerateSignatureResult = NULL;
    g_smpCallBack.SMP_CallbackGenerateRPAResult = NULL;
    g_smpCallBack.SMP_CallbackResolveRPAResult = NULL;
    g_smpCallBack.SMP_CallbackGenerateScOobDataResult = NULL;
    (void)context;
}

int SMP_StartPair(uint16_t handle, const BtAddr *localAddr, const BtAddr *peerAddr, const SMP_PairParam *param)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_StartPairTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_StartPairTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_StartPairTask_t), 0x00, sizeof(SMP_StartPairTask_t));
    ctx->handle = handle;
    (void)memcpy_s(&ctx->localAddr, sizeof(BtAddr), localAddr, sizeof(ctx->localAddr));
    (void)memcpy_s(&ctx->peerAddr, sizeof(BtAddr), peerAddr, sizeof(ctx->peerAddr));
    (void)memcpy_s(&ctx->param, sizeof(SMP_PairParam), param, sizeof(ctx->param));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_StartPairTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_StartPairTask(void *context)
{
    int ret = SMP_SUCCESS;
    SMP_StartPairTask_t *param = (SMP_StartPairTask_t *)context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.state == SMP_STATE_PAIRING) {
        LOG_ERROR("It's already pairing state.");
    } else if (SMP_GetScOobMng()->state == SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's already SC OOB data generating state.");
        ret = SMP_ERR_INVAL_STATE;
    } else if (SMP_GetSecureConnOnlyMode() && (!(param->param.authReq & SMP_AUTH_REQ_BIT_SC))) {
        LOG_ERROR("SC bit is not set in authReq.");
        ret = SMP_ERR_INVAL_PARAM;
    } else {
        (void)memset_s(&g_smpPairMng, sizeof(g_smpPairMng), 0x00, sizeof(g_smpPairMng));
        g_smpPairMng.state = SMP_STATE_PAIRING;
        g_smpPairMng.handle = param->handle;
        g_smpPairMng.role = SMP_ROLE_MASTER;
        g_smpPairMng.alarm = AlarmCreate("", false);
        g_smpPairMng.local.addr.type = param->localAddr.type;
        g_smpPairMng.peer.addr.type = param->peerAddr.type;
        SMP_MemoryReverseCopy(
            g_smpPairMng.local.addr.addr, param->localAddr.addr, sizeof(g_smpPairMng.local.addr.addr));
        SMP_MemoryReverseCopy(g_smpPairMng.peer.addr.addr, param->peerAddr.addr, sizeof(g_smpPairMng.peer.addr.addr));
        (void)memcpy_s(&g_smpPairMng.local.pairParam, sizeof(SMP_PairParam), &param->param, sizeof(SMP_PairParam));
        g_smpPairMng.step = SMP_LEGACY_PAIR_MASTER_STEP_1;
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_1 started.");
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = SMP_SendPairingRequest(param->handle, &param->param, SMP_SendDataCallback);
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("Send pairing request failed.");
            AlarmCancel(g_smpPairMng.alarm);
        }
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(param->handle, SMP_PAIR_STATUS_FAILED, 0x00, NULL);
    }
    MEM_MALLOC.free(param);
}

int SMP_CancelPair(uint16_t handle)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_CancelPairTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_CancelPairTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_CancelPairTask_t), 0x00, sizeof(SMP_CancelPairTask_t));
    ctx->handle = handle;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_CancelPairTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_CancelPairTask(void *context)
{
    SMP_CancelPairTask_t *param = (SMP_CancelPairTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state. ");
        MEM_MALLOC.free(param);
        return;
    }
    if (param->handle != g_smpPairMng.handle) {
        LOG_ERROR("Connection handle error. ");
        MEM_MALLOC.free(param);
        return;
    }
    SMP_GeneratePairResult(
        param->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, g_smpPairMng.alarm);
    MEM_MALLOC.free(param);
}

int SMP_AuthenticationRequestReply(
    uint16_t handle, bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_AuthReqReplyTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_AuthReqReplyTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_AuthReqReplyTask_t), 0x00, sizeof(SMP_AuthReqReplyTask_t));
    ctx->handle = handle;
    ctx->accept = accept;
    ctx->rejectReason = rejectReason;
    ctx->pairMethod = pairMethod;
    if (accept && SMP_IsPasskeyEntryPair(pairMethod)) {
        (void)memcpy_s(ctx->entryValue, SMP_ENTRY_VALUE_LEN, entryValue, SMP_PASSKEY_LEN);
    } else if (accept && (pairMethod == SMP_PAIR_METHOD_OOB_LEGACY)) {
        (void)memcpy_s(ctx->entryValue, SMP_ENTRY_VALUE_LEN, entryValue, SMP_LEGACY_OOB_LEN);
    } else if ((accept && (pairMethod == SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV)) ||
               (accept && (pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND))) {
        (void)memcpy_s(ctx->entryValue, SMP_ENTRY_VALUE_LEN, entryValue, SMP_SC_OOB_LEN);
    }
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_AuthReqReplyTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_AuthReqReplyTask(void *context)
{
    SMP_AuthReqReplyTask_t *param = (SMP_AuthReqReplyTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state");
        MEM_MALLOC.free(param);
        return;
    }
    if (g_smpPairMng.handle != param->handle) {
        LOG_ERROR("Connection handle error");
        MEM_MALLOC.free(param);
        return;
    }
    if (g_smpPairMng.role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role.");
        SMP_AuthReqReplyProcessMaster(param->accept, param->rejectReason, param->pairMethod, param->entryValue);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_AuthReqReplyProcessSlave(param->accept, param->rejectReason, param->pairMethod, param->entryValue);
    }
    MEM_MALLOC.free(param);
}

int SMP_RemotePairRequestReply(
    uint16_t handle, uint8_t rejectReason, const BtAddr *localAddr, const BtAddr *peerAddr, const SMP_PairParam *param)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_RemotePairReqReplyTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_RemotePairReqReplyTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_RemotePairReqReplyTask_t), 0x00, sizeof(SMP_RemotePairReqReplyTask_t));
    ctx->handle = handle;
    ctx->rejectReason = rejectReason;
    if (rejectReason == SMP_PAIR_FAILED_NO_FAILED) {
        ctx->accept = true;
    } else {
        ctx->accept = false;
    }
    if (ctx->accept) {
        (void)memcpy_s(&ctx->localAddr, sizeof(BtAddr), localAddr, sizeof(ctx->localAddr));
        (void)memcpy_s(&ctx->peerAddr, sizeof(BtAddr), peerAddr, sizeof(ctx->peerAddr));
        (void)memcpy_s(&ctx->param, sizeof(BtAddr), param, sizeof(ctx->param));
    }
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_RemotePairReqReplyTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_RemotePairReqReplyTask(void *context)
{
    SMP_RemotePairReqReplyTask_t *param = (SMP_RemotePairReqReplyTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_PairReqReplyJudgeException(param->handle, param->accept, param->rejectReason, &param->param);
    if (!ret) {
        g_smpPairMng.local.addr.type = param->localAddr.type;
        g_smpPairMng.peer.addr.type = param->peerAddr.type;
        SMP_MemoryReverseCopy(
            g_smpPairMng.local.addr.addr, param->localAddr.addr, sizeof(g_smpPairMng.local.addr.addr));
        SMP_MemoryReverseCopy(g_smpPairMng.peer.addr.addr, param->peerAddr.addr, sizeof(g_smpPairMng.peer.addr.addr));
        (void)memcpy_s(&g_smpPairMng.local.pairParam, sizeof(SMP_PairParam), &param->param, sizeof(SMP_PairParam));
        SMP_CalculatePairType(&g_smpPairMng);
        SMP_CalculateEncKeySize(&g_smpPairMng);
        SMP_CalculatePairMethod(&g_smpPairMng);
        SMP_CalculateKeyDistribution(&g_smpPairMng);
        ret = SMP_SendPairingResponse(param->handle, &g_smpPairMng.local.pairParam, SMP_SendPairingResponseCallback);
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("Send pairing response failed.");
            SMP_GeneratePairResult(param->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        } else {
            if (g_smpPairMng.pairType == SMP_PAIR_TYPE_LEGACY) {
                LOG_INFO("It's legacy pair.");
                SMP_PairReqReplyLegacyPair();
            } else {
                LOG_INFO("It's SC pair.");
                SMP_PairReqReplyScPair();
            }
        }
    }
    MEM_MALLOC.free(param);
}

int SMP_RemotePairResponseReply(uint16_t handle, bool accept, uint8_t rejectReason)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_RemotePairRspReplyTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_RemotePairRspReplyTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_RemotePairRspReplyTask_t), 0x00, sizeof(SMP_RemotePairRspReplyTask_t));
    ctx->handle = handle;
    ctx->accept = accept;
    ctx->rejectReason = rejectReason;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_RemotePairRspReplyTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_RemotePairRspReplyTask(void *context)
{
    SMP_RemotePairRspReplyTask_t *param = (SMP_RemotePairRspReplyTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_PairRspReplyJudgeException(param->handle, param->accept, param->rejectReason);
    if (!ret) {
        SMP_CalculatePairType(&g_smpPairMng);
        SMP_CalculateEncKeySize(&g_smpPairMng);
        SMP_CalculatePairMethod(&g_smpPairMng);
        SMP_CalculateKeyDistribution(&g_smpPairMng);

        if (g_smpPairMng.pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_INFO("It's legacy pair.");
            SMP_PairRspReplyLegacyPair();
        } else {
            LOG_INFO("It's SC pair.");
            SMP_PairRspReplyScPair();
        }
    }
    MEM_MALLOC.free(param);
}

int SMP_RemoteSecurityRequestReply(uint16_t handle, bool accept, uint8_t rejectReason)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_RemoteSecReqReplyTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_RemoteSecReqReplyTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_RemoteSecReqReplyTask_t), 0x00, sizeof(SMP_RemoteSecReqReplyTask_t));
    ctx->handle = handle;
    ctx->accept = accept;
    ctx->rejectReason = rejectReason;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_RemoteSecReqReplyTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_RemoteSecReqReplyTask(void *context)
{
    SMP_RemoteSecReqReplyTask_t *param = (SMP_RemoteSecReqReplyTask_t *)context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (!param->accept) {
        LOG_ERROR("User rejected.");
        SMP_SendPairingFailed(param->handle, param->rejectReason, SMP_SendPairingFailedCallback);
    }
    MEM_MALLOC.free(param);
}

int SMP_LongTermKeyRequestReply(uint16_t handle, bool accept, const uint8_t *key)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_LongTermKeyReqReplyTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_LongTermKeyReqReplyTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_LongTermKeyReqReplyTask_t), 0x00, sizeof(SMP_LongTermKeyReqReplyTask_t));
    ctx->handle = handle;
    ctx->accept = accept;
    if (key != NULL) {
        (void)memcpy_s(ctx->key, SMP_LTK_LEN, key, SMP_LTK_LEN);
    }
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_LongTermKeyReqReplyTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_LongTermKeyReqReplyTask(void *context)
{
    SMP_LongTermKeyReqReplyTask_t *param = (SMP_LongTermKeyReqReplyTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((g_smpPairMng.state == SMP_STATE_PAIRING) && (g_smpPairMng.handle == param->handle)) {
        SMP_LtkReqReplyException(param->handle);
    } else {
        SMP_LtkReqReplyNormal(param->handle, param->accept, param->key);
    }
    MEM_MALLOC.free(param);
}

static int SMP_LtkReqReplyNormal(uint16_t handle, bool accept, const uint8_t *key)
{
    int ret = SMP_SUCCESS;
    HciLeLongTermKeyRequestReplyParam replycmdParam;
    HciLeLongTermKeyRequestNegativeReplyParam negativeReplyParam;

    if (!accept) {
        LOG_ERROR("User rejected.");
        negativeReplyParam.connectionHandle = handle;
        HCI_LeLongTermKeyRequestNegativeReply(&negativeReplyParam);
        SMP_NotifyCbEncComp(handle, SMP_ENCRYPT_STATUS_FAILED);
    } else {
        replycmdParam.connectionHandle = handle;
        (void)memcpy_s(replycmdParam.longTermKey, SMP_LTK_LEN, key, SMP_LTK_LEN);
        ret = HCI_LeLongTermKeyRequestReply(&replycmdParam);
        if (ret != SMP_SUCCESS) {
            SMP_NotifyCbEncComp(handle, SMP_ENCRYPT_STATUS_FAILED);
        }
    }

    return ret;
}

static int SMP_LtkReqReplyException(uint16_t handle)
{
    int ret = SMP_SUCCESS;
    HciLeLongTermKeyRequestNegativeReplyParam negativeReplyParam;

    LOG_ERROR("Invalid state.");
    negativeReplyParam.connectionHandle = handle;
    HCI_LeLongTermKeyRequestNegativeReply(&negativeReplyParam);
    SMP_NotifyCbEncComp(handle, SMP_ENCRYPT_STATUS_FAILED);

    return ret;
}

int SMP_StartEncryption(uint16_t handle, const uint8_t *random, uint16_t ediv, const uint8_t *key)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_StartEncTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_StartEncTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_StartEncTask_t), 0x00, sizeof(SMP_StartEncTask_t));
    ctx->handle = handle;
    (void)memcpy_s(ctx->random, SMP_MASTER_RAND_LEN, random, SMP_MASTER_RAND_LEN);
    ctx->ediv = ediv;
    (void)memcpy_s(ctx->key, SMP_LTK_LEN, key, SMP_LTK_LEN);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_StartEncTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_StartEncTask(void *context)
{
    int ret;
    HciLeStartEncryptionParam startEncParam;
    SMP_StartEncTask_t *param = (SMP_StartEncTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((g_smpPairMng.state == SMP_STATE_PAIRING) && (g_smpPairMng.handle == param->handle)) {
        LOG_ERROR("Invalid state.");
        ret = SMP_ERR_INVAL_STATE;
    } else {
        uint8_t edivTemp[sizeof(param->ediv)] = {0x00};
        startEncParam.connectionHandle = param->handle;
        edivTemp[0x00] = ((uint8_t)((param->ediv) & 0xFF));
        edivTemp[0x01] = ((uint8_t)(((param->ediv) >> 0x08) & 0xFF));
        (void)memcpy_s(&startEncParam.encryptDiversifier, sizeof(edivTemp), edivTemp, sizeof(edivTemp));
        SMP_MemoryReverseCopy(startEncParam.randomNumber, param->random, SMP_MASTER_RAND_LEN);
        (void)memcpy_s(startEncParam.longTermKey, SMP_LTK_LEN, param->key, SMP_LTK_LEN);
        ret = HCI_LeStartEncryption(&startEncParam);
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("HCI_LeStartEncryption failed.");
        }
    }
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbEncComp(param->handle, SMP_ENCRYPT_STATUS_FAILED);
    }
    MEM_MALLOC.free(param);
}

void SMP_PairTimeout(void *parameter)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_PairTimeoutTask, NULL);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)parameter;
}

static void SMP_PairTimeoutTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state. ");
        return;
    }
    if (g_smpPairMng.role == SMP_ROLE_MASTER) {
        LOG_ERROR("Master, step:%hu. ", g_smpPairMng.step);
    } else {
        LOG_ERROR("Slave, step:%hu. ", g_smpPairMng.step);
    }
    SMP_GeneratePairResult(g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, 0x00, NULL);
    (void)context;
}

int SMP_SendSecurityRequestToRemote(uint16_t handle, uint8_t authReq)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendSecReqToRemoteTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendSecReqToRemoteTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_SendSecReqToRemoteTask_t), 0x00, sizeof(SMP_SendSecReqToRemoteTask_t));
    ctx->handle = handle;
    ctx->authReq = authReq;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendSecReqToRemoteTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_SendSecReqToRemoteTask(void *context)
{
    SMP_SendSecReqToRemoteTask_t *param = (SMP_SendSecReqToRemoteTask_t *)context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendSecurityRequest(param->handle, param->authReq, SMP_SendSecurityRequestCallback);
    MEM_MALLOC.free(param);
}

int SMP_GenerateSignature(const uint8_t *csrk, uint32_t counter, const uint8_t *data, uint16_t dataLen)
{
    int ret;
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_GenSignTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_GenSignTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_GenSignTask_t), 0x00, sizeof(SMP_GenSignTask_t));
    (void)memcpy_s(ctx->csrk, SMP_CSRK_LEN, csrk, SMP_CSRK_LEN);
    ctx->counter = counter;
    ctx->dataLen = dataLen;
    ctx->data = MEM_MALLOC.alloc(dataLen);
    (void)memcpy_s(ctx->data, dataLen, data, dataLen);
    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_GenSignTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_GenSignTask(void *context)
{
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    SMP_EncCmd *encCmd = SMP_AllocEncCmd();
    if (encCmd == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    HciLeEncryptParam encryptParam;
    SMP_GenSignTask_t *param = (SMP_GenSignTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    encCmd->length = param->dataLen + SMP_SIGNCOUNTER_LEN;
    encCmd->M = MEM_MALLOC.alloc(encCmd->length);
    (void)memcpy_s(encCmd->M, param->dataLen, param->data, param->dataLen);
    (void)memcpy_s(encCmd->M + param->dataLen, SMP_SIGNCOUNTER_LEN, (uint8_t *)&param->counter, SMP_SIGNCOUNTER_LEN);
    encCmd->signDataLen = param->dataLen + SMP_SIGNCOUNTER_LEN;
    SMP_ReverseMemoryOrder(encCmd->M, encCmd->signDataLen);
    (void)memcpy_s(encCmd->key, sizeof(encCmd->key), param->csrk, sizeof(encCmd->key));
    encCmd->signCounter = param->counter;
    (void)memcpy_s(encryptParam.key, SMP_CSRK_LEN, param->csrk, SMP_CSRK_LEN);
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_GENERATE_SIGNATURE_STEP_1 started.");
    int ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_GENERATE_SIGNATURE_STEP_1, encCmd, SMP_USING_HW_AES128_SIGN);
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
    }
    SMP_FreeEncCmd(encCmd);
    MEM_MALLOC.free(param->data);
    MEM_MALLOC.free(param);
}

int SMP_ResolveRPA(const uint8_t *addr, const uint8_t *irk)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret;
    uint8_t message[SMP_ENCRYPT_PLAINTEXTDATA_LEN] = {0x00};
    (void)memcpy_s(message, sizeof(message), addr + SMP_RPA_HIGH_BIT_LEN, SMP_RPA_HIGH_BIT_LEN);
    uint8_t encryptedData[SMP_ENCRYPT_PLAINTEXTDATA_LEN] = {0x00};
    ret = SMP_Aes128(irk, SMP_IRK_LEN, message, sizeof(message), encryptedData);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("status = %{public}d.", ret);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (memcmp(encryptedData, addr, SMP_RPA_HIGH_BIT_LEN) != 0x00) {
        LOG_INFO("Resolve RPA failed");
        ret = SMP_RESOLVE_RPA_RESULT_NO;
    } else {
        LOG_INFO("Resolve RPA success");
        ret = SMP_RESOLVE_RPA_RESULT_YES;
    }
    return ret;
}

int SMP_AsyncResolveRPA(const uint8_t *addr, const uint8_t *irk)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_AsyncResoRpaTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_AsyncResoRpaTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_AsyncResoRpaTask_t), 0x00, sizeof(SMP_AsyncResoRpaTask_t));
    (void)memcpy_s(ctx->addr, sizeof(ctx->addr), addr, sizeof(ctx->addr));
    (void)memcpy_s(ctx->irk, sizeof(ctx->irk), irk, SMP_IRK_LEN);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_AsyncResoRpaTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_AsyncResoRpaTask(void *context)
{
    uint8_t message[SMP_ENCRYPT_PLAINTEXTDATA_LEN] = {0x00};
    SMP_EncCmd *encCmd = SMP_AllocEncCmd();
    if (encCmd == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    HciLeEncryptParam encryptParam;
    SMP_AsyncResoRpaTask_t *param = (SMP_AsyncResoRpaTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(message, sizeof(message), param->addr + SMP_RPA_HIGH_BIT_LEN, SMP_RPA_HIGH_BIT_LEN);
    (void)memcpy_s(encCmd->address, sizeof(encCmd->address), param->addr, sizeof(encCmd->address));
    (void)memcpy_s(encCmd->key, sizeof(encCmd->key), param->irk, SMP_IRK_LEN);
    (void)memcpy_s(encryptParam.key, sizeof(encryptParam.key), param->irk, sizeof(encryptParam.key));
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), message, sizeof(message));
    LOG_DEBUG("SMP_RESOLVE_RPA_STEP_1 started.");
    int ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_RESOLVE_RPA_STEP_1, encCmd, SMP_USING_HW_AES128_RPA);
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbResoRpa(SMP_RESOLVE_RPA_STATUS_FAILED, false, param->addr, param->irk);
    }
    SMP_FreeEncCmd(encCmd);
    MEM_MALLOC.free(param);
}

int SMP_GenerateRPA(const uint8_t *irk)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_GenRpaTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_GenRpaTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_GenRpaTask_t), 0x00, sizeof(SMP_GenRpaTask_t));
    (void)memcpy_s(ctx->irk, sizeof(ctx->irk), irk, SMP_IRK_LEN);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_GenRpaTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_GenRpaTask(void *context)
{
    uint32_t number = RandomGenerate();
    uint8_t address[BT_ADDRESS_SIZE] = {0x00};
    uint8_t message[SMP_ENCRYPT_PLAINTEXTDATA_LEN] = {0x00};
    SMP_EncCmd *encCmd = SMP_AllocEncCmd();
    if (encCmd == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    HciLeEncryptParam encryptParam;
    SMP_GenRpaTask_t *param = (SMP_GenRpaTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(address, sizeof(address), (uint8_t *)&number, SMP_RPA_HIGH_BIT_LEN);
    address[0x00] &= 0x7Fu;
    address[0x00] |= 0x40u;
    for (int i = 0; i < (BT_ADDRESS_SIZE - SMP_RPA_HIGH_BIT_LEN); i++) {
        message[sizeof(message) - SMP_RPA_HIGH_BIT_LEN + i] = address[i];
    }
    (void)memcpy_s(encCmd->address, sizeof(encCmd->address), address, sizeof(encCmd->address));
    (void)memcpy_s(encryptParam.key, sizeof(encryptParam.key), param->irk, sizeof(encryptParam.key));
    SMP_MemoryReverseCopy(encryptParam.plaintextData, message, sizeof(message));
    LOG_DEBUG("SMP_GENERATE_RPA_STEP_1 started.");
    int ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_GENERATE_RPA_STEP_1, encCmd, SMP_USING_HW_AES128_RPA);
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenRpa(SMP_GENERATE_RPA_STATUS_FAILED, address);
    }
    SMP_FreeEncCmd(encCmd);
    MEM_MALLOC.free(param);
}

int SMP_SetSecureConnOnlyMode(bool mode)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SetSecConnOnlyModeTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SetSecConnOnlyModeTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_SetSecConnOnlyModeTask_t), 0x00, sizeof(SMP_SetSecConnOnlyModeTask_t));
    ctx->mode = mode;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SetSecConnOnlyModeTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_SetSecConnOnlyModeTask(void *context)
{
    SMP_SetSecConnOnlyModeTask_t *param = (SMP_SetSecConnOnlyModeTask_t *)context;
    g_smpSecureConnOnlyMode = param->mode;
    MEM_MALLOC.free(param);
}

bool SMP_GetSecureConnOnlyMode()
{
    bool mode = false;

    mode = g_smpSecureConnOnlyMode;

    return mode;
}

int SMP_SetIRK(const uint8_t *irk)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SetIrkTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SetIrkTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_SetIrkTask_t), 0x00, sizeof(SMP_SetIrkTask_t));
    (void)memcpy_s(ctx->irk, sizeof(ctx->irk), irk, SMP_IRK_LEN);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SetIrkTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_SetIrkTask(void *context)
{
    SMP_SetIrkTask_t *param = (SMP_SetIrkTask_t *)context;
    SMP_SetLocalIrk(param->irk, SMP_IRK_LEN);
    MEM_MALLOC.free(param);
}

int SMP_SetIdentAddr(const BtAddr *addr)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SetIdentAddrTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SetIdentAddrTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(SMP_SetIdentAddrTask_t), 0x00, sizeof(SMP_SetIdentAddrTask_t));
    (void)memcpy_s(&ctx->addr, sizeof(ctx->addr), addr, sizeof(BtAddr));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SetIdentAddrTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return ret;
    }
    return ret;
}

static void SMP_SetIdentAddrTask(void *context)
{
    SMP_SetIdentAddrTask_t *param = (SMP_SetIdentAddrTask_t *)context;
    SMP_SetLocalIdentAddr(&param->addr);
    MEM_MALLOC.free(param);
}

void SMP_GeneratePairResult(uint16_t handle, uint8_t status, uint8_t failedReason, Alarm *cancelTimer)
{
    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        return;
    }

    if (status) {
        SMP_GenPairRetException(handle, status, failedReason, cancelTimer);
    } else {
        SMP_GenPairRetNormal(handle, status);
    }
}

int SMP_EncryptCompleteJudgeException(uint8_t status, uint8_t role)
{
    int ret = SMP_SUCCESS;

    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state.");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    if (g_smpPairMng.role != role) {
        LOG_ERROR("Invalid role = %hhu.", g_smpPairMng.role);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    if (status) {
        LOG_ERROR("returnParam->status = %hhu.", status);
        SMP_GeneratePairResult(g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }

    return ret;
}

static int SMP_AuthReqReplyProcessMaster(
    bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = SMP_AuthReqReplyJudgeExceptionMaster(accept, rejectReason);
    if (ret != SMP_SUCCESS) {
        return ret;
    }
    SMP_AuthReqReplyObtainEntryValue(pairMethod, entryValue);
    ret = SMP_AuthReqReplyStepTransMaster(pairMethod);

    return ret;
}

static int SMP_AuthReqReplyJudgeExceptionMaster(bool accept, uint8_t rejectReason)
{
    int ret = SMP_SUCCESS;

    if ((g_smpPairMng.step != SMP_LEGACY_PAIR_MASTER_STEP_2) &&
        (g_smpPairMng.step != SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_5) &&
        (g_smpPairMng.step != SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_14) &&
        (g_smpPairMng.step != SMP_SC_PAIR_OOB_MASTER_STEP_7)) {
        ret = SMP_ERR_INVAL_STATE;
        LOG_ERROR("It's invalid step:%hu. ", g_smpPairMng.step);
        return ret;
    }
    AlarmCancel(g_smpPairMng.alarm);
    if (!accept) {
        LOG_ERROR("User rejected. ");
        ret = SMP_ERR_INVAL_STATE;
        SMP_GeneratePairResult(g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, rejectReason, NULL);
        return ret;
    }

    return ret;
}

static int SMP_AuthReqReplyStepTransMaster(uint8_t pairMethod)
{
    int ret = SMP_SUCCESS;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    HciLeEncryptParam encryptParam;
    if (g_smpPairMng.pairType == SMP_PAIR_TYPE_LEGACY) {
        ret = SMP_SendHciLeRandCmd(0x00);
    } else {
        if (SMP_IsPasskeyEntryPair(pairMethod)) {
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_6 started.");
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_6);
        } else if (pairMethod == SMP_PAIR_METHOD_NUMERIC_COMPARISON) {
            SMP_MemoryReverseCopy(encryptParam.key, SALT, sizeof(encryptParam.key));
            (void)memcpy_s(
                encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_1 started.");
            ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_MASTER_STEP_1, NULL, SMP_USING_HW_AES128_PAIR);
        } else if (SMP_IsScOobPair(pairMethod)) {
            LOG_DEBUG("SMP_SC_PAIR_OOB_MASTER_STEP_8 started.");
            g_smpPairMng.step = SMP_SC_PAIR_OOB_MASTER_STEP_8;
            AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
            ret = SMP_SendPairingPublicKey(g_smpPairMng.handle, g_smpPairMng.local.publicKey, SMP_SendDataCallback);
            if (ret != SMP_SUCCESS) {
                LOG_ERROR("Send pairing public key failed.");
                AlarmCancel(g_smpPairMng.alarm);
            }
        }
    }

    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
    return ret;
}

static int SMP_AuthReqReplyProcessSlave(
    bool accept, uint8_t rejectReason, uint8_t pairMethod, const uint8_t *entryValue)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = SMP_AuthReqReplyJudgeExceptionSlave(accept, rejectReason);
    if (ret != SMP_SUCCESS) {
        return ret;
    }
    SMP_AuthReqReplyObtainEntryValue(pairMethod, entryValue);
    ret = SMP_AuthReqReplyStepTransSlave(pairMethod);

    return ret;
}

static int SMP_AuthReqReplyJudgeExceptionSlave(bool accept, uint8_t rejectReason)
{
    int ret = SMP_SUCCESS;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((g_smpPairMng.step != SMP_LEGACY_PAIR_SLAVE_STEP_2) &&
        (g_smpPairMng.step != SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5) &&
        (g_smpPairMng.step != SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_13) &&
        (g_smpPairMng.step != SMP_SC_PAIR_OOB_SLAVE_STEP_7)) {
        ret = SMP_ERR_INVAL_STATE;
        LOG_ERROR("It's invalid step:%hu", g_smpPairMng.step);
        return ret;
    }
    AlarmCancel(g_smpPairMng.alarm);
    if (!accept) {
        ret = SMP_ERR_INVAL_STATE;
        SMP_GeneratePairResult(g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, rejectReason, NULL);
        LOG_ERROR("User rejected");
        return ret;
    }

    return ret;
}

static int SMP_AuthReqReplyStepTransSlave(uint8_t pairMethod)
{
    int ret = SMP_SUCCESS;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.pairType == SMP_PAIR_TYPE_LEGACY) {
        if (g_smpPairMng.slaveConfirmRecvFlag) {
            LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_4 started.");
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_4);
            if (ret != SMP_SUCCESS) {
                SMP_GeneratePairResult(
                    g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            }
        } else {
            LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_3 started.");
            g_smpPairMng.step = SMP_LEGACY_PAIR_SLAVE_STEP_3;
            AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        }
    } else {
        if (SMP_IsPasskeyEntryPair(pairMethod)) {
            ret = SMP_AuthReqReplyPasskeyEntrySlave();
        } else if (pairMethod == SMP_PAIR_METHOD_NUMERIC_COMPARISON) {
            ret = SMP_AuthReqReplyNumericSlave();
        } else if (SMP_IsScOobPair(pairMethod)) {
            ret = SMP_AuthReqReplyOobSlave();
        }
    }

    return ret;
}

static int SMP_AuthReqReplyNumericSlave()
{
    int ret = SMP_SUCCESS;
    HciLeEncryptParam encryptParam;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.slaveDHKeyCheckRecvFlag) {
        uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
        SMP_MemoryReverseCopy(encryptParam.key, SALT, sizeof(encryptParam.key));
        (void)memcpy_s(
            encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
        LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_2 started.");
        ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_2, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_1 started.");
        g_smpPairMng.step = SMP_SC_PAIR_COMMON_SLAVE_STEP_1;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
    return ret;
}

static int SMP_AuthReqReplyPasskeyEntrySlave()
{
    int ret = SMP_SUCCESS;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpPairMng.slaveConfirmRecvFlag) {
        LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_7 started.");
        ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_7);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6 started.");
        g_smpPairMng.step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    }

    return ret;
}

static int SMP_AuthReqReplyOobSlave()
{
    int ret = SMP_SUCCESS;
    HciLeGenerateDHKeyParam DHKeyParam;

    if (g_smpPairMng.slavePubKeyRecvFlag) {
        g_smpPairMng.step = SMP_SC_PAIR_OOB_SLAVE_STEP_9;
        (void)memcpy_s(
            DHKeyParam.remoteP256PublicKey, SMP_PUBLICKEY_LEN, g_smpPairMng.peer.publicKey, SMP_PUBLICKEY_LEN);
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_9 started. ");
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = HCI_LeGenerateDHKey(&DHKeyParam);
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("HCI_LeGenerateDHKey failed. ");
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, g_smpPairMng.alarm);
        }
    } else {
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_8 started. ");
        g_smpPairMng.step = SMP_SC_PAIR_OOB_SLAVE_STEP_8;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    }

    return ret;
}

static void SMP_AuthReqReplyObtainEntryValue(uint8_t pairMethod, const uint8_t *entryValue)
{
    if (SMP_IsPasskeyEntryPair(pairMethod)) {
        LOG_INFO("SMP_PAIR_METHOD_PASSKEY_ENTRY.");
        LOG_INFO("passkey is %u.", *((uint32_t *)entryValue));
        (void)memset_s(g_smpPairMng.TK, SMP_TK_LEN, 0x00, sizeof(g_smpPairMng.TK));
        for (int i = 0; i < (int)sizeof(*((uint32_t *)entryValue)); i++) {
            g_smpPairMng.TK[SMP_TK_LEN - 1 - i] = entryValue[i];
        }
    } else if (pairMethod == SMP_PAIR_METHOD_OOB_LEGACY) {
        LOG_INFO("SMP_PAIR_METHOD_OOB_LEGACY.");
        (void)memcpy_s(g_smpPairMng.TK, SMP_TK_LEN, entryValue, SMP_TK_LEN);
    } else if ((pairMethod == SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV) ||
               (pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND)) {
        LOG_INFO("SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV or ");
        LOG_INFO("SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND");
        SMP_MemoryReverseCopy(g_smpPairMng.peer.addr.addr, entryValue, sizeof(g_smpPairMng.peer.addr.addr));
        (void)memcpy_s(g_smpPairMng.peer.oobRand,
            SMP_RANDOM_DATA_LEN,
            entryValue + sizeof(g_smpPairMng.peer.addr.addr),
            SMP_RANDOM_DATA_LEN);
        SMP_MemoryReverseCopy(g_smpPairMng.peer.confirm,
            entryValue + sizeof(g_smpPairMng.peer.addr.addr) + SMP_RANDOM_DATA_LEN,
            SMP_CONFIRM_DATA_LEN);
    }
}

static int SMP_PairReqReplyJudgeException(
    uint16_t handle, bool accept, uint8_t rejectReason, const SMP_PairParam *param)
{
    int ret = SMP_SUCCESS;

    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state. ");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (g_smpPairMng.handle != handle) {
        LOG_ERROR("Connection handle error. ");
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }
    if (g_smpPairMng.role != SMP_ROLE_SLAVE) {
        LOG_ERROR("It's not accepter role.");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (g_smpPairMng.step != SMP_LEGACY_PAIR_SLAVE_STEP_1) {
        ret = SMP_ERR_INVAL_STATE;
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_1. ");
        return ret;
    }
    AlarmCancel(g_smpPairMng.alarm);

    if (!accept) {
        LOG_ERROR("User rejected. ");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, rejectReason, NULL);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (SMP_GetSecureConnOnlyMode() && (!(param->authReq & SMP_AUTH_REQ_BIT_SC))) {
        LOG_ERROR("SC bit is not set in authReq.");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }

    return ret;
}

static int SMP_PairReqReplyLegacyPair()
{
    int ret = SMP_SUCCESS;
    uint16_t handle = g_smpPairMng.handle;
    uint8_t pairMethod = g_smpPairMng.local.pairMethod;

    if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_JUST_WORK) {
        LOG_INFO("SMP_PAIR_METHOD_JUST_WORK.");
        (void)memset_s(g_smpPairMng.TK, SMP_TK_LEN, 0x00, sizeof(g_smpPairMng.TK));
        LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_3 started.");
        g_smpPairMng.step = SMP_LEGACY_PAIR_SLAVE_STEP_3;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    } else if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_PASSKEY_DISPLAY) {
        LOG_INFO("SMP_PAIR_METHOD_PASSKEY_DISPLAY.");
        g_smpPairMng.slaveConfirmRecvFlag = SMP_SLAVE_CONFIRM_RECV_FLAG_NO;
        ret = SMP_SendHciLeRandCmd(0x00);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_2 started.");
        g_smpPairMng.slaveConfirmRecvFlag = SMP_SLAVE_CONFIRM_RECV_FLAG_NO;
        g_smpPairMng.step = SMP_LEGACY_PAIR_SLAVE_STEP_2;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        SMP_NotifyCbAuthReq(handle, pairMethod, NULL);
    }
    return ret;
}

static int SMP_PairReqReplyScPair()
{
    int ret = SMP_SUCCESS;

    if ((SMP_IsPasskeyEntryPair(g_smpPairMng.local.pairMethod)) ||
        (SMP_IsJustworkOrNumericPair(g_smpPairMng.local.pairMethod))) {
        LOG_INFO("It's not OOB");
        if (SMP_IsJustworkOrNumericPair(g_smpPairMng.local.pairMethod)) {
            LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1 started.");
            g_smpPairMng.step = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1;
            AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        } else {
            g_smpPairMng.scConfirmCheckCounter = 0x00;
            g_smpPairMng.slaveConfirmRecvFlag = SMP_SLAVE_CONFIRM_RECV_FLAG_NO;
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1 started.");
            g_smpPairMng.step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1;
            AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        }
    } else {
        LOG_INFO("It's OOB.");
        ret = SMP_PairReqReplyScPairOob();
    }

    return ret;
}

static int SMP_PairReqReplyScPairOob()
{
    int ret = SMP_SUCCESS;

    if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV) {
        (void)memset_s(g_smpPairMng.peer.oobRand, SMP_RANDOM_DATA_LEN, 0x00, sizeof(g_smpPairMng.peer.oobRand));
    } else if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND) {
        (void)memset_s(g_smpPairMng.local.oobRand, SMP_RANDOM_DATA_LEN, 0x00, sizeof(g_smpPairMng.local.oobRand));
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_1 started");
        g_smpPairMng.step = SMP_SC_PAIR_OOB_SLAVE_STEP_1;
        g_smpPairMng.slavePubKeyRecvFlag = SMP_SLAVE_PUBLICKEY_RECV_FLAG_NO;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = HCI_LeReadLocalP256PublicKey();
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, g_smpPairMng.alarm);
            LOG_ERROR("HCI_LeReadLocalP256PublicKey failed");
        }
    }

    if (g_smpPairMng.local.pairMethod != SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND) {
        uint8_t scOobData[SMP_SC_OOB_LEN] = {0x00};
        SMP_MemoryReverseCopy(g_smpPairMng.local.oobRand, SMP_GetScOobMng()->random, SMP_RANDOM_DATA_LEN);
        SMP_MemoryReverseCopy(scOobData, g_smpPairMng.local.addr.addr, sizeof(g_smpPairMng.local.addr.addr));
        SMP_MemoryReverseCopy(
            scOobData + sizeof(g_smpPairMng.local.addr.addr), SMP_GetScOobMng()->random, SMP_RANDOM_DATA_LEN);
        SMP_MemoryReverseCopy(scOobData + sizeof(g_smpPairMng.local.addr.addr) + SMP_RANDOM_DATA_LEN,
            SMP_GetScOobMng()->confirm,
            SMP_CONFIRM_DATA_LEN);
        (void)memcpy_s(
            g_smpPairMng.local.publicKey, SMP_PUBLICKEY_LEN, SMP_GetScOobMng()->publicKey, SMP_PUBLICKEY_LEN);
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_7 started.");
        g_smpPairMng.step = SMP_SC_PAIR_OOB_SLAVE_STEP_7;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        SMP_NotifyCbAuthReq(g_smpPairMng.handle, g_smpPairMng.local.pairMethod, scOobData);
    }

    return ret;
}

static int SMP_PairRspReplyJudgeException(uint16_t handle, bool accept, uint8_t rejectReason)
{
    int ret = SMP_SUCCESS;

    if (g_smpPairMng.state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (g_smpPairMng.handle != handle) {
        LOG_ERROR("Connection handle error");
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }
    if (g_smpPairMng.role != SMP_ROLE_MASTER) {
        LOG_ERROR("It's not initiator role.");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (g_smpPairMng.step != SMP_LEGACY_PAIR_MASTER_STEP_1) {
        ret = SMP_ERR_INVAL_STATE;
        LOG_ERROR("It's not SMP_LEGACY_PAIR_MASTER_STEP_1");
        return ret;
    }
    AlarmCancel(g_smpPairMng.alarm);

    if (!accept) {
        LOG_ERROR("User rejected");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, rejectReason, NULL);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    return ret;
}

static int SMP_PairRspReplyLegacyPair()
{
    int ret = SMP_SUCCESS;
    uint16_t handle = g_smpPairMng.handle;
    uint8_t pairMethod = g_smpPairMng.local.pairMethod;

    if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_JUST_WORK) {
        LOG_INFO("SMP_PAIR_METHOD_JUST_WORK.");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_2 started.");
        (void)memset_s(g_smpPairMng.TK, SMP_TK_LEN, 0x00, sizeof(g_smpPairMng.TK));
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_2);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_PASSKEY_DISPLAY) {
        LOG_INFO("SMP_PAIR_METHOD_PASSKEY_DISPLAY.");
        ret = SMP_SendHciLeRandCmd(0x00);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        LOG_INFO("Other pair method.");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_2 started.");
        g_smpPairMng.step = SMP_LEGACY_PAIR_MASTER_STEP_2;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        SMP_NotifyCbAuthReq(handle, pairMethod, NULL);
    }

    return ret;
}

static int SMP_PairRspReplyScPair()
{
    int ret;

    if ((SMP_IsJustworkOrNumericPair(g_smpPairMng.local.pairMethod)) ||
        (SMP_IsPasskeyEntryPair(g_smpPairMng.local.pairMethod))) {
        LOG_INFO("It's not OOB. ");
        if (SMP_IsJustworkOrNumericPair(g_smpPairMng.local.pairMethod)) {
            LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1 started.");
            g_smpPairMng.step = SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1;
        } else {
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1 started.");
            g_smpPairMng.step = SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1;
        }
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = HCI_LeReadLocalP256PublicKey();
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("HCI_LeReadLocalP256PublicKey failed.");
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, g_smpPairMng.alarm);
        }
    } else {
        LOG_INFO("It's OOB.");
        ret = SMP_PairRspReplyScPairOob();
    }

    return ret;
}

static int SMP_PairRspReplyScPairOob()
{
    int ret = SMP_SUCCESS;

    if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV) {
        (void)memset_s(g_smpPairMng.peer.oobRand, SMP_RANDOM_DATA_LEN, 0x00, SMP_RANDOM_DATA_LEN);
    } else if (g_smpPairMng.local.pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND) {
        (void)memset_s(g_smpPairMng.local.oobRand, SMP_RANDOM_DATA_LEN, 0x00, SMP_RANDOM_DATA_LEN);
        LOG_DEBUG("SMP_SC_PAIR_OOB_MASTER_STEP_1 started.");
        g_smpPairMng.step = SMP_SC_PAIR_OOB_MASTER_STEP_1;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = HCI_LeReadLocalP256PublicKey();
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("HCI_LeReadLocalP256PublicKey failed.");
            SMP_GeneratePairResult(
                g_smpPairMng.handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, g_smpPairMng.alarm);
        }
    }

    if (g_smpPairMng.local.pairMethod != SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND) {
        uint8_t scOOBData[SMP_SC_OOB_LEN] = {0x00};
        SMP_MemoryReverseCopy(g_smpPairMng.local.oobRand, SMP_GetScOobMng()->random, SMP_RANDOM_DATA_LEN);
        SMP_MemoryReverseCopy(scOOBData, g_smpPairMng.local.addr.addr, sizeof(g_smpPairMng.local.addr.addr));
        SMP_MemoryReverseCopy(
            scOOBData + sizeof(g_smpPairMng.local.addr.addr), SMP_GetScOobMng()->random, SMP_RANDOM_DATA_LEN);
        SMP_MemoryReverseCopy(scOOBData + sizeof(g_smpPairMng.local.addr.addr) + SMP_RANDOM_DATA_LEN,
            SMP_GetScOobMng()->confirm,
            SMP_CONFIRM_DATA_LEN);
        (void)memcpy_s(
            g_smpPairMng.local.publicKey, SMP_PUBLICKEY_LEN, SMP_GetScOobMng()->publicKey, SMP_PUBLICKEY_LEN);
        LOG_DEBUG("SMP_SC_PAIR_OOB_MASTER_STEP_7 started.");
        g_smpPairMng.step = SMP_SC_PAIR_OOB_MASTER_STEP_7;
        AlarmSet(g_smpPairMng.alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        SMP_NotifyCbAuthReq(g_smpPairMng.handle, g_smpPairMng.local.pairMethod, scOOBData);
    }

    return ret;
}

static void SMP_GenPairRetNormal(uint16_t handle, uint8_t status)
{
    SMP_PairResult pairResult;

    (void)memset_s(&pairResult, sizeof(SMP_PairResult), 0x00, sizeof(pairResult));
    pairResult.pairType = g_smpPairMng.pairType;

    if ((g_smpPairMng.local.pairParam.authReq & SMP_AUTH_REQ_BONDING) &&
        (g_smpPairMng.peer.pairParam.authReq & SMP_AUTH_REQ_BONDING)) {
        pairResult.bondedFlag = SMP_BONDED_FLAG_YES;
        if (g_smpPairMng.local.pairMethod != SMP_PAIR_METHOD_JUST_WORK) {
            pairResult.authFlag = SMP_AUTH_FLAG_YES;
        } else {
            pairResult.authFlag = SMP_AUTH_FLAG_NO;
        }
        pairResult.localKeyDist = g_smpPairMng.local.keyDist;
        pairResult.peerKeyDist = g_smpPairMng.peer.keyDist;
        (void)memcpy_s(pairResult.localLTK, SMP_LTK_LEN, g_smpPairMng.local.LTK, SMP_LTK_LEN);
        pairResult.localEdiv = g_smpPairMng.local.masterIdEdiv;
        (void)memcpy_s(
            pairResult.localRandom, SMP_MASTER_RAND_LEN, g_smpPairMng.local.masterIdRand, SMP_MASTER_RAND_LEN);
        (void)memcpy_s(pairResult.peerLTK, SMP_LTK_LEN, g_smpPairMng.peer.LTK, SMP_LTK_LEN);
        pairResult.peerEdiv = g_smpPairMng.peer.masterIdEdiv;
        (void)memcpy_s(pairResult.peerRandom, SMP_MASTER_RAND_LEN, g_smpPairMng.peer.masterIdRand, SMP_MASTER_RAND_LEN);
        (void)memcpy_s(pairResult.localIRK, SMP_IRK_LEN, g_smpPairMng.local.IRK, SMP_IRK_LEN);
        (void)memcpy_s(&pairResult.localIdentAddr, sizeof(BtAddr), &g_smpPairMng.local.identityAddr, sizeof(BtAddr));
        (void)memcpy_s(pairResult.peerIRK, SMP_IRK_LEN, g_smpPairMng.peer.IRK, SMP_IRK_LEN);
        (void)memcpy_s(&pairResult.peerIdentAddr, sizeof(BtAddr), &g_smpPairMng.peer.identityAddr, sizeof(BtAddr));
        (void)memcpy_s(pairResult.localCSRK, SMP_CSRK_LEN, g_smpPairMng.local.CSRK, SMP_CSRK_LEN);
        (void)memcpy_s(pairResult.peerCSRK, SMP_CSRK_LEN, g_smpPairMng.peer.CSRK, SMP_CSRK_LEN);
        pairResult.encKeySize = g_smpPairMng.encKeySize;
    } else {
        pairResult.bondedFlag = SMP_BONDED_FLAG_NO;
    }

    AlarmDelete(g_smpPairMng.alarm);
    (void)memset_s(&g_smpPairMng, sizeof(g_smpPairMng), 0x00, sizeof(g_smpPairMng));
    SMP_NotifyCbPairRet(handle, status, &pairResult);
}

static void SMP_GenPairRetException(uint16_t handle, uint8_t status, uint8_t failedReason, Alarm *cancelTimer)
{
    SMP_PairResult pairResult;
    (void)memset_s(&pairResult, sizeof(SMP_PairResult), 0x00, sizeof(pairResult));
    if (cancelTimer != NULL) {
        AlarmCancel(cancelTimer);
    }
    if (failedReason) {
        SMP_SendPairingFailed(handle, failedReason, SMP_SendPairingFailedCallback);
    }
    AlarmDelete(g_smpPairMng.alarm);
    (void)memset_s(&g_smpPairMng, sizeof(g_smpPairMng), 0x00, sizeof(g_smpPairMng));
    SMP_NotifyCbPairRet(handle, status, &pairResult);
}

static void SMP_L2cifLeRegisterFixChannelCallback(uint16_t cid, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (result) {
        LOG_ERROR("L2CIF_LeRegisterFixChannel failed result = %{public}d", result);
    }
    (void)cid;
    (void)result;
}

static void SMP_L2cifLeDeregisterFixChannelCallback(uint16_t cid, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (result) {
        LOG_ERROR("L2CIF_LeDeregisterFixChannel failed result = %{public}d", result);
    }
    (void)cid;
    (void)result;
}

void SMP_GenerateSignatureStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_EncCmd *pEncCmdData = encData->encCmd;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    SMP_CryptAesCmacStep2Param cryptAesCmacstep2Param;
    SMP_CryptAesCmacStep3Param cryptAesCmacstep3Param;
    SMP_CryptAesCmacStep1Param cryptAesCmacStep1parm;

    LOG_INFO("%{public}s ", __FUNCTION__);
    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu", returnParam->status);
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
        return;
    }
    SMP_MemoryReverseCopy(cryptAesCmacStep1parm.input, returnParam->encryptedData, sizeof(cryptAesCmacStep1parm.input));
    SMP_CryptographicAesCmacStep1(&cryptAesCmacStep1parm);
    (void)memcpy_s(
        cryptAesCmacstep2Param.input1, CRYPT_AESCMAC_IN_LEN, cryptAesCmacStep1parm.output1, CRYPT_AESCMAC_IN_LEN);
    (void)memcpy_s(
        cryptAesCmacstep2Param.input2, CRYPT_AESCMAC_IN_LEN, cryptAesCmacStep1parm.output2, CRYPT_AESCMAC_IN_LEN);
    cryptAesCmacstep2Param.input3 = pEncCmdData->M;
    cryptAesCmacstep2Param.length = pEncCmdData->signDataLen;
    SMP_CryptographicAesCmacStep2(&cryptAesCmacstep2Param);
    cryptAesCmacstep3Param.stepA = SMP_GENERATE_SIGNATURE_STEP_2;
    cryptAesCmacstep3Param.stepB = SMP_GENERATE_SIGNATURE_STEP_3;
    cryptAesCmacstep3Param.message = pEncCmdData->M;
    cryptAesCmacstep3Param.messageSize = pEncCmdData->signDataLen;
    (void)memcpy_s(cryptAesCmacstep3Param.aesCmacOutput,
        CRYPT_AESCMAC_OUT_LEN,
        cryptAesCmacstep2Param.output,
        CRYPT_AESCMAC_OUT_LEN);
    (void)memcpy_s(cryptAesCmacstep3Param.key, CRYPT_AESCMAC_KEY_LEN, pEncCmdData->key, CRYPT_AESCMAC_KEY_LEN);
    cryptAesCmacstep3Param.n = cryptAesCmacstep2Param.n;
    cryptAesCmacstep3Param.signCounter = pEncCmdData->signCounter;
    cryptAesCmacstep3Param.isUsingHwAes128 = SMP_USING_HW_AES128_SIGN;
    int ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacstep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
    }
    (void)memset_s(cryptAesCmacstep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_GenerateSignatureStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;

    LOG_INFO("%{public}s.", __FUNCTION__);

    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu ", returnParam->status);
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
        return;
    }

    cryptAesCmacStep4Param.stepA = SMP_GENERATE_SIGNATURE_STEP_2;
    cryptAesCmacStep4Param.stepB = SMP_GENERATE_SIGNATURE_STEP_3;
    cryptAesCmacStep4Param.pEncCmdData = encData->encCmd;
    (void)memcpy_s(cryptAesCmacStep4Param.X,
        sizeof(cryptAesCmacStep4Param.X),
        returnParam->encryptedData,
        sizeof(cryptAesCmacStep4Param.X));
    cryptAesCmacStep4Param.isUsingHwAes128 = SMP_USING_HW_AES128_SIGN;
    int ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
    }
}

void SMP_GenerateSignatureStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_EncCmd *pEncCmdData = encData->encCmd;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t signature[SMP_SIGNATURE_LEN] = {0};

    LOG_INFO("%{public}s", __FUNCTION__);

    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu. ", returnParam->status);
        SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
        return;
    }

    (void)memcpy_s(signature, sizeof(signature), (uint8_t *)&pEncCmdData->signCounter, SMP_SIGNCOUNTER_LEN);
    (void)memcpy_s(signature + SMP_SIGNCOUNTER_LEN,
        (SMP_SIGNATURE_LEN - SMP_SIGNCOUNTER_LEN),
        &returnParam->encryptedData[SMP_SIGNATURE_LEN - SMP_SIGNCOUNTER_LEN],
        (SMP_SIGNATURE_LEN - SMP_SIGNCOUNTER_LEN));
    SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_SUCCESS, signature);
}

void SMP_NotifyCbAuthReq(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue)
{
    if (g_smpCallBack.SMP_CallbackAuthenticationRequest != NULL) {
        g_smpCallBack.SMP_CallbackAuthenticationRequest(handle, pairMethod, displayValue);
    }
}

void SMP_NotifyCbPairRet(uint16_t handle, uint8_t status, const SMP_PairResult *result)
{
    if (g_smpCallBack.SMP_CallbackPairResult != NULL) {
        g_smpCallBack.SMP_CallbackPairResult(handle, status, result);
    }
}

void SMP_NotifyCbPairReq(uint16_t handle, const SMP_PairParam *param)
{
    if (g_smpCallBack.SMP_CallbackRemotePairRequest != NULL) {
        g_smpCallBack.SMP_CallbackRemotePairRequest(handle, param);
    }
}

void SMP_NotifyCbPairRsp(uint16_t handle, const SMP_PairParam *param)
{
    if (g_smpCallBack.SMP_CallbackRemotePairResponse != NULL) {
        g_smpCallBack.SMP_CallbackRemotePairResponse(handle, param);
    }
}

void SMP_NotifyCbSecReq(uint16_t handle, uint8_t authReq)
{
    if (g_smpCallBack.SMP_CallbackRemoteSecurityRequest != NULL) {
        g_smpCallBack.SMP_CallbackRemoteSecurityRequest(handle, authReq);
    }
}

void SMP_NotifyCbLtkReq(uint16_t handle, const uint8_t *random, uint16_t ediv)
{
    if (g_smpCallBack.SMP_CallbackLongTermKeyRequest != NULL) {
        g_smpCallBack.SMP_CallbackLongTermKeyRequest(handle, random, ediv);
    }
}

void SMP_NotifyCbEncComp(uint16_t handle, uint8_t status)
{
    if (g_smpCallBack.SMP_CallbackEncryptionComplete != NULL) {
        g_smpCallBack.SMP_CallbackEncryptionComplete(handle, status);
    }
}

void SMP_NotifyCbGenSign(uint8_t status, const uint8_t *sign)
{
    if (g_smpCallBack.SMP_CallbackGenerateSignatureResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateSignatureResult(status, sign);
    }
}

void SMP_NotifyCbGenRpa(uint8_t status, const uint8_t *addr)
{
    if (g_smpCallBack.SMP_CallbackGenerateRPAResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateRPAResult(status, addr);
    }
}

void SMP_NotifyCbResoRpa(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk)
{
    if (g_smpCallBack.SMP_CallbackResolveRPAResult != NULL) {
        g_smpCallBack.SMP_CallbackResolveRPAResult(status, result, addr, irk);
    }
}

void SMP_NotifyCbGenScOobData(uint8_t status, const uint8_t *random, const uint8_t *confirm)
{
    if (g_smpCallBack.SMP_CallbackGenerateScOobDataResult != NULL) {
        g_smpCallBack.SMP_CallbackGenerateScOobDataResult(status, random, confirm);
    }
}

int SMP_ParamIsNULL(const SMP_StepParam *param)
{
    if (param == NULL) {
        return SMP_ERR_INVAL_PARAM;
    }
    if (param->data == NULL) {
        return SMP_ERR_INVAL_PARAM;
    }
    return SMP_SUCCESS;
}

int SMP_GenerateScOobData()
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_GenScOobDataTask, NULL);
    if (ret != SMP_SUCCESS) {
        return ret;
    }
    return ret;
}

static void SMP_GenScOobDataTask(void *context)
{
    int ret = SMP_SUCCESS;

    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetScOobMng()->state == SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's already SC OOB data generating state.");
    } else if (g_smpPairMng.state == SMP_STATE_PAIRING) {
        LOG_ERROR("It's already pairing state.");
        ret = SMP_ERR_INVAL_STATE;
    } else {
        SMP_ClearScOobData(false);
        SMP_GetScOobMng()->state = SMP_STATE_SC_OOB_DATA_GENERATING;
        SMP_GetScOobMng()->alarm = AlarmCreate("", false);
        LOG_DEBUG("SMP_GENERATE_SC_OOB_DATA_STEP_1 started.");
        SMP_GetScOobMng()->step = SMP_GENERATE_SC_OOB_DATA_STEP_1;
        AlarmSet(SMP_GetScOobMng()->alarm, SMP_GEN_SC_OOB_DATA_WAIT_TIME, SMP_GenerateScOobDataTimeout, NULL);
        ret = HCI_LeReadLocalP256PublicKey();
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("HCI_LeReadLocalP256PublicKey failed.");
            SMP_ClearScOobData(true);
        }
    }
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
    }
    (void)context;
}

SMP_PairMng *SMP_GetPairMng()
{
    return &g_smpPairMng;
}

static Module g_smp = {
    .name = MODULE_NAME_SMP,
    .init = SMP_Initialize,
    .startup = SMP_Startup,
    .shutdown = SMP_Shutdown,
    .cleanup = SMP_Finalize,
    .dependencies = {MODULE_NAME_L2CAP},
};

MODULE_DECL(g_smp)
