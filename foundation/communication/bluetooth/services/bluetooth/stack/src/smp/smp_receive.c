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

#include "smp_receive.h"

#include <string.h>

#include "btm/btm_thread.h"
#include "log.h"
#include "platform/include/allocator.h"
#include "smp.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_def.h"
#include "smp_legacy.h"
#include "smp_oob.h"
#include "smp_sc_accepter.h"
#include "smp_sc_initiator.h"
#include "smp_send.h"
#include "smp_tool.h"

static void SMP_ReceivePairingRequest(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingResponse(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingConfirm(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingRandom(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveEncryptionInformation(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveMasterIdentification(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveIdentityInformation(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveIdentityAddressInformation(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveSigningInformation(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingFailed(uint16_t handle, const Buffer *buffer);
static void SMP_ReceiveSecurityRequest(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingPublicKey(uint16_t handle, const Buffer *buffer);
static void SMP_ReceivePairingDHKeyCheck(uint16_t handle, const Buffer *buffer);
static void SMP_RecvEncInfoProcessMaster(const Buffer *buffer);
static void SMP_RecvEncInfoProcessSlave(const Buffer *buffer);
static void SMP_RecvMasterIdentProcessMaster(const Buffer *buffer);
static void SMP_RecvMasterIdentProcessSlave(const Buffer *buffer);
static void SMP_RecvIdentInfoProcessMaster(const Buffer *buffer);
static void SMP_RecvIdentInfoProcessSlave(const Buffer *buffer);
static void SMP_RecvIdentAddrInfoProcessMaster(const Buffer *buffer);
static void SMP_RecvIdentAddrInfoProcessSlave(const Buffer *buffer);
static void SMP_RecvSignInfoProcessMaster(const Buffer *buffer);
static void SMP_RecvSignInfoProcessSlave(const Buffer *buffer);
static void SMP_RecvDataCmdDistribution(uint16_t handle, uint8_t code, const Buffer *buffer);
static int SMP_RecvPairReqJudgeException(uint16_t handle, const SMP_PairParam *pairParam);
static int SMP_RecvPairRspJudgeException(uint16_t handle, const SMP_PairParam *pairParam);
static void SMP_RecvPairCfmStepDistribution(uint16_t step, const SMP_StepParam *param);
static void SMP_RecvPairRandStepDistribution(uint16_t step, const SMP_StepParam *param);
static void SMP_RecvPairPubKeyStepDistribution(uint16_t step, const SMP_StepParam *param);

static void SMP_RecvDataTask(void *context);
static void SMP_DisconnectedTask(void *context);

void SMP_ReceiveData(uint16_t handle, const Packet *pkt)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    Packet *pktPtr = PacketRefMalloc(pkt);
    SMP_RecvDataTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_RecvDataTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_RecvDataTask_t), 0x00, sizeof(SMP_RecvDataTask_t));
    ctx->handle = handle;
    ctx->pkt = pktPtr;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_RecvDataTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        PacketFree(ctx->pkt);
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_RecvDataTask(void *context)
{
    uint8_t code = 0x00;
    Buffer *buffer = NULL;
    SMP_RecvDataTask_t *param = (SMP_RecvDataTask_t *)context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    PacketExtractHead(param->pkt, &code, sizeof(code));
    buffer = PacketContinuousPayload(param->pkt);
    SMP_RecvDataCmdDistribution(param->handle, code, buffer);
    PacketFree(param->pkt);
    MEM_MALLOC.free(param);
}

void SMP_Connected(const BtAddr *addr, uint16_t handle, uint8_t role, uint8_t status)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    (void)addr;
    (void)handle;
    (void)role;
    (void)status;
}

void SMP_Disconnected(uint16_t handle, uint8_t role, uint8_t reason)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_DisconnectedTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_DisconnectedTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_DisconnectedTask_t), 0x00, sizeof(SMP_DisconnectedTask_t));
    ctx->handle = handle;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_DisconnectedTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
    (void)role;
    (void)reason;
}

static void SMP_DisconnectedTask(void *context)
{
    SMP_DisconnectedTask_t *param = (SMP_DisconnectedTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) && (SMP_GetPairMng()->handle == param->handle)) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, 0x00, SMP_GetPairMng()->alarm);
        MEM_MALLOC.free(param);
        return;
    }
    MEM_MALLOC.free(param);
}

static void SMP_RecvDataCmdDistribution(uint16_t handle, uint8_t code, const Buffer *buffer)
{
    switch (code) {
        case SMP_CODE_PAIRING_REQ:
            SMP_ReceivePairingRequest(handle, buffer);
            break;
        case SMP_CODE_PAIRING_RSP:
            SMP_ReceivePairingResponse(handle, buffer);
            break;
        case SMP_CODE_PAIRING_CFM:
            SMP_ReceivePairingConfirm(handle, buffer);
            break;
        case SMP_CODE_PAIRING_RAND:
            SMP_ReceivePairingRandom(handle, buffer);
            break;
        case SMP_CODE_ENCRYPTION_INFO:
            SMP_ReceiveEncryptionInformation(handle, buffer);
            break;
        case SMP_CODE_MASTER_IDENTITY:
            SMP_ReceiveMasterIdentification(handle, buffer);
            break;
        case SMP_CODE_IDENTITY_INFO:
            SMP_ReceiveIdentityInformation(handle, buffer);
            break;
        case SMP_CODE_IDENTITY_ADDR_INFO:
            SMP_ReceiveIdentityAddressInformation(handle, buffer);
            break;
        case SMP_CODE_SIGNING_INFO:
            SMP_ReceiveSigningInformation(handle, buffer);
            break;
        case SMP_CODE_PAIRING_FAIL:
            SMP_ReceivePairingFailed(handle, buffer);
            break;
        case SMP_CODE_SECURITY_REQ:
            SMP_ReceiveSecurityRequest(handle, buffer);
            break;
        case SMP_CODE_PAIRING_PUBLIC_KEY:
            SMP_ReceivePairingPublicKey(handle, buffer);
            break;
        case SMP_CODE_PAIRING_DHKEY_CHECK:
            SMP_ReceivePairingDHKeyCheck(handle, buffer);
            break;
        default:
            break;
    }
}

static void SMP_ReceivePairingRequest(uint16_t handle, const Buffer *buffer)
{
    uint8_t *pData = NULL;
    SMP_PairParam pairParamTemp;

    LOG_INFO("%{public}s", __FUNCTION__);
    pData = (uint8_t *)BufferPtr(buffer);
    int offset = 0;
    pairParamTemp.ioCapability = pData[offset];
    offset += sizeof(pairParamTemp.ioCapability);
    pairParamTemp.oobDataFlag = pData[offset];
    offset += sizeof(pairParamTemp.oobDataFlag);
    pairParamTemp.authReq = pData[offset];
    offset += sizeof(pairParamTemp.authReq);
    pairParamTemp.maxEncKeySize = pData[offset];
    offset += sizeof(pairParamTemp.maxEncKeySize);
    pairParamTemp.initKeyDist = pData[offset];
    offset += sizeof(pairParamTemp.initKeyDist);
    pairParamTemp.respKeyDist = pData[offset];
    if (SMP_RecvPairReqJudgeException(handle, &pairParamTemp)) {
        return;
    }
    (void)memset_s(SMP_GetPairMng(), sizeof(SMP_PairMng), 0x00, sizeof(SMP_PairMng));
    SMP_GetPairMng()->state = SMP_STATE_PAIRING;
    SMP_GetPairMng()->handle = handle;
    SMP_GetPairMng()->role = SMP_ROLE_SLAVE;
    SMP_GetPairMng()->alarm = AlarmCreate("", false);
    (void)memcpy_s(&SMP_GetPairMng()->peer.pairParam, sizeof(SMP_PairParam), &pairParamTemp, sizeof(SMP_PairParam));
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_1;
    LOG_INFO("SMP_LEGACY_PAIR_SLAVE_STEP_1 started.");
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbPairReq(handle, &pairParamTemp);
}

static int SMP_RecvPairReqJudgeException(uint16_t handle, const SMP_PairParam *pairParam)
{
    int ret = SMP_SUCCESS;
    uint8_t reason = 0x00;

    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) ||
        (SMP_GetScOobMng()->state == SMP_STATE_SC_OOB_DATA_GENERATING)) {
        LOG_ERROR("It's already pairing state or SC OOB data generating state.");
        if (handle == SMP_GetPairMng()->handle) {
            SMP_SendPairingFailed(handle, SMP_PAIR_FAILED_REPAETED_ATTEMPTS, SMP_SendPairingFailedCallback);
        } else {
            SMP_SendPairingFailed(handle, SMP_PAIR_FAILED_PAIRING_NOT_SUPPORTED, SMP_SendPairingFailedCallback);
        }
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    if (SMP_CheckRemotePairParam(pairParam, &reason)) {
        LOG_ERROR("Check Remote Pair Param failed.");
        SMP_SendPairingFailed(handle, reason, SMP_SendPairingFailedCallback);
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }

    if (SMP_GetSecureConnOnlyMode() && (!(pairParam->authReq & SMP_AUTH_REQ_BIT_SC))) {
        LOG_ERROR("SC bit is not set in authReq.");
        SMP_SendPairingFailed(handle, SMP_PAIR_FAILED_AUTH_REQ, SMP_SendPairingFailedCallback);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    return ret;
}

static void SMP_ReceivePairingResponse(uint16_t handle, const Buffer *buffer)
{
    SMP_PairParam pairParamTemp;

    LOG_INFO("%{public}s", __FUNCTION__);
    if (memcpy_s(&pairParamTemp, sizeof(SMP_PairParam), (uint8_t *)BufferPtr(buffer), sizeof(SMP_PairParam)) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] pairParamTemp memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    if (SMP_RecvPairRspJudgeException(handle, &pairParamTemp)) {
        return;
    }
    AlarmCancel(SMP_GetPairMng()->alarm);
    if (memcpy_s(&SMP_GetPairMng()->peer.pairParam,
        sizeof(SMP_PairParam), (uint8_t *)BufferPtr(buffer), sizeof(SMP_PairParam)) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] peer.pairParam memcpy_s fail.", __FUNCTION__, __LINE__);
            return;
        }
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbPairRsp(handle, &pairParamTemp);
}

static int SMP_RecvPairRspJudgeException(uint16_t handle, const SMP_PairParam *pairParam)
{
    int ret = SMP_SUCCESS;
    uint8_t reason = 0x00;

    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state.");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection handle error.");
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }
    if (SMP_GetPairMng()->role != SMP_ROLE_MASTER) {
        LOG_INFO("It's not initiator role.");
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }
    if (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_1) {
        LOG_INFO("It's not SMP_LEGACY_PAIR_MASTER_STEP_1.");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    if (SMP_CheckRemotePairParam(pairParam, &reason)) {
        LOG_ERROR("Check Remote Pair Param failed.");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, reason, SMP_GetPairMng()->alarm);
        ret = SMP_ERR_INVAL_PARAM;
        return ret;
    }
    if (SMP_GetSecureConnOnlyMode() && (!(pairParam->authReq & SMP_AUTH_REQ_BIT_SC))) {
        LOG_ERROR("SC bit is not set in authReq.");
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_AUTH_REQ, SMP_GetPairMng()->alarm);
        ret = SMP_ERR_INVAL_STATE;
        return ret;
    }

    return ret;
}

static void SMP_ReceivePairingConfirm(uint16_t handle, const Buffer *buffer)
{
    SMP_StepParam param = {.data = (void *)BufferPtr(buffer)};
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's Not Pairing State");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle Error");
        return;
    }
    SMP_RecvPairCfmStepDistribution(SMP_GetPairMng()->step, &param);
}

static void SMP_RecvPairCfmStepDistribution(uint16_t step, const SMP_StepParam *param)
{
    switch (step) {
        case SMP_LEGACY_PAIR_MASTER_STEP_6:
            SMP_LegacyPairMasterStep6(param);
            break;
        case SMP_LEGACY_PAIR_SLAVE_STEP_1:
        case SMP_LEGACY_PAIR_SLAVE_STEP_2:
            SMP_LegacyPairSlaveStep2(param);
            break;
        case SMP_LEGACY_PAIR_SLAVE_STEP_3:
            SMP_LegacyPairSlaveStep3(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_3:
            SMP_ScPairJustworkOrNumericMasterStep3(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_4:
            SMP_ScPairJustworkOrNumericMasterStep4(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_11:
            SMP_ScPairPasskeyEntryMasterStep11(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5:
            SMP_ScPairPasskeyEntrySlaveStep5(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6:
            SMP_ScPairPasskeyEntrySlaveStep6(param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", step);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

static void SMP_ReceivePairingRandom(uint16_t handle, const Buffer *buffer)
{
    SMP_StepParam param = {.data = (void *)BufferPtr(buffer)};
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It is not pairing state.");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection handle is error.");
        return;
    }
    AlarmCancel(SMP_GetPairMng()->alarm);
    SMP_RecvPairRandStepDistribution(SMP_GetPairMng()->step, &param);
}

static void SMP_RecvPairRandStepDistribution(uint16_t step, const SMP_StepParam *param)
{
    switch (step) {
        case SMP_LEGACY_PAIR_MASTER_STEP_7:
            SMP_LegacyPairMasterStep7(param);
            break;
        case SMP_LEGACY_PAIR_SLAVE_STEP_8:
            SMP_LegacyPairSlaveStep8(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_7:
            SMP_ScPairJustworkOrNumericMasterStep7(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_12:
            SMP_ScPairPasskeyEntryMasterStep12(param);
            break;
        case SMP_SC_PAIR_OOB_MASTER_STEP_15:
            SMP_ScPairOobMasterStep15(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_9:
            SMP_ScPairJustworkOrNumericSlaveStep9(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_12:
            SMP_ScPairPasskeyEntrySlaveStep12(param);
            break;
        case SMP_SC_PAIR_OOB_SLAVE_STEP_13:
            SMP_ScPairOobSlaveStep13(param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", step);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

static void SMP_ReceiveEncryptionInformation(uint16_t handle, const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection handle error");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role");
        SMP_RecvEncInfoProcessMaster(buffer);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_RecvEncInfoProcessSlave(buffer);
    }
}

static void SMP_ReceiveMasterIdentification(uint16_t handle, const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's Not pairing state");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle error");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role");
        SMP_RecvMasterIdentProcessMaster(buffer);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_RecvMasterIdentProcessSlave(buffer);
    }
}

static void SMP_ReceiveIdentityInformation(uint16_t handle, const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's Not pairing state.");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle error.");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role.");
        SMP_RecvIdentInfoProcessMaster(buffer);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_RecvIdentInfoProcessSlave(buffer);
    }
}

static void SMP_ReceiveIdentityAddressInformation(uint16_t handle, const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state!");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection handle error!");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role!");
        SMP_RecvIdentAddrInfoProcessMaster(buffer);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_RecvIdentAddrInfoProcessSlave(buffer);
    }
}

static void SMP_ReceiveSigningInformation(uint16_t handle, const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state.");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection handle error.");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_INFO("It's initiator role.");
        SMP_RecvSignInfoProcessMaster(buffer);
    } else {
        LOG_INFO("It's accepter role.");
        SMP_RecvSignInfoProcessSlave(buffer);
    }
}

static void SMP_ReceivePairingFailed(uint16_t handle, const Buffer *buffer)
{
    uint8_t reason = *((uint8_t *)BufferPtr(buffer));

    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It Is Not Pairing State!");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle Is Error!");
        return;
    }
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_ERROR("Initiator, step :%hu.", SMP_GetPairMng()->step);
    } else {
        LOG_ERROR("Accepter, step :%hu.", SMP_GetPairMng()->step);
    }
    if (reason != SMP_PAIR_FAILED_REPAETED_ATTEMPTS) {
        SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, 0x00, NULL);
    }
}

static void SMP_ReceiveSecurityRequest(uint16_t handle, const Buffer *buffer)
{
    uint8_t authReq = *((uint8_t *)BufferPtr(buffer));

    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) && (SMP_GetPairMng()->handle == handle)) {
        return;
    }
    if (SMP_GetSecureConnOnlyMode() && (!(authReq & SMP_AUTH_REQ_BIT_SC))) {
        LOG_ERROR("SC bit is not set in authReq.");
        SMP_SendPairingFailed(handle, SMP_PAIR_FAILED_AUTH_REQ, SMP_SendPairingFailedCallback);
        return;
    }
    SMP_NotifyCbSecReq(handle, authReq);
}

static void SMP_ReceivePairingPublicKey(uint16_t handle, const Buffer *buffer)
{
    SMP_StepParam param = {.data = (void *)BufferPtr(buffer)};
    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It is Not Pairing state!");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle is error!");
        return;
    }
    SMP_RecvPairPubKeyStepDistribution(SMP_GetPairMng()->step, &param);
}

static void SMP_RecvPairPubKeyStepDistribution(uint16_t step, const SMP_StepParam *param)
{
    switch (step) {
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_2:
            SMP_ScPairJustworkOrNumericMasterStep2(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_2:
            SMP_ScPairPasskeyEntryMasterStep2(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1:
            SMP_ScPairJustworkOrNumericSlaveStep1(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1:
            SMP_ScPairPasskeyEntrySlaveStep1(param);
            break;
        case SMP_SC_PAIR_OOB_MASTER_STEP_8:
            SMP_ScPairOobMasterStep8(param);
            break;
        case SMP_SC_PAIR_OOB_SLAVE_STEP_7:
            SMP_ScPairOobSlaveStep7(param);
            break;
        case SMP_SC_PAIR_OOB_SLAVE_STEP_8:
            SMP_ScPairOobSlaveStep8(param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", step);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

static void SMP_ReceivePairingDHKeyCheck(uint16_t handle, const Buffer *buffer)
{
    SMP_StepParam param = {.data = (void *)BufferPtr(buffer)};

    LOG_INFO("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It Is Not Pairing State");
        return;
    }
    if (SMP_GetPairMng()->handle != handle) {
        LOG_ERROR("Connection Handle Is Error");
        return;
    }
    switch (SMP_GetPairMng()->step) {
        case SMP_SC_PAIR_COMMON_MASTER_STEP_13:
            SMP_ScPairCommonMasterStep13(&param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_13:
            SMP_ScPairJustworkOrNumericSlaveStep13(&param);
            break;
        case SMP_SC_PAIR_COMMON_SLAVE_STEP_1:
            SMP_ScPairCommonSlaveStep1(&param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", SMP_GetPairMng()->step);
            SMP_GeneratePairResult(handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

static void SMP_RecvEncInfoProcessMaster(const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
        (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_12)) {
        LOG_ERROR("It's invalid step.");
        return;
    }
    if (memcpy_s(SMP_GetPairMng()->peer.LTK, SMP_LTK_LEN, (uint8_t *)BufferPtr(buffer), SMP_LTK_LEN) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_ENC_INFO);
    if ((SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) ||
        (SMP_GetPairMng()->masterEncryptedFlag == SMP_MASTER_ENCRYPTED_FLAG_NO)) {
        return;
    }
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0");
    AlarmCancel(SMP_GetPairMng()->alarm);
    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY) {
        LOG_INFO("Local need to send ENC_KEY");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 started");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local need to send SIGN_KEY");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 started");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        LOG_INFO("Send keys to remote");
        if (SMP_SendDistributionKeysToRemote()) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            LOG_INFO("Pair failed.");
        }
    } else {
        LOG_INFO("Pair success.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvEncInfoProcessSlave(const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);

    if (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_20) {
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_20.");
        return;
    }

    if (memcpy_s(SMP_GetPairMng()->peer.LTK, SMP_LTK_LEN, (uint8_t *)BufferPtr(buffer), SMP_LTK_LEN) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_ENC_INFO);
    if (SMP_GetPairMng()->peer.keyDistCmdFlag == 0x00) {
        LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
        AlarmCancel(SMP_GetPairMng()->alarm);
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvMasterIdentProcessMaster(const Buffer *buffer)
{
    uint8_t *pData = (uint8_t *)BufferPtr(buffer);
    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
        (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_12)) {
        LOG_ERROR("It's invalid step.");
        return;
    }
    SMP_GetPairMng()->peer.masterIdEdiv = *(uint16_t *)pData;
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.masterIdRand, &(pData[0x02]), SMP_MASTER_RAND_LEN);
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_MASTER_IDENT);
    if ((SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) ||
        (SMP_GetPairMng()->masterEncryptedFlag == SMP_MASTER_ENCRYPTED_FLAG_NO)) {
        return;
    }
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0!");
    AlarmCancel(SMP_GetPairMng()->alarm);
    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY) {
        LOG_INFO("Local need to send ENC_KEY!");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 started!");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local need to send SIGN_KEY!");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 started!");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        LOG_INFO("Send Keys to remote.");
        if (SMP_SendDistributionKeysToRemote()) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            LOG_INFO("Pair is failed.");
        }
    } else {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvMasterIdentProcessSlave(const Buffer *buffer)
{
    uint8_t *pData = NULL;

    LOG_INFO("%{public}s", __FUNCTION__);

    if (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_20) {
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_20.");
        return;
    }

    pData = (uint8_t *)BufferPtr(buffer);
    SMP_GetPairMng()->peer.masterIdEdiv = *(uint16_t *)pData;
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.masterIdRand, &(pData[0x02]), SMP_MASTER_RAND_LEN);
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_MASTER_IDENT);
    if (SMP_GetPairMng()->peer.keyDistCmdFlag == 0x00) {
        LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
        AlarmCancel(SMP_GetPairMng()->alarm);
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvIdentInfoProcessMaster(const Buffer *buffer)
{
    int ret = SMP_SUCCESS;
    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
        (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_12) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_17) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_18)) {
        LOG_ERROR("It's invalid step.");
        return;
    }
    if (memcpy_s(SMP_GetPairMng()->peer.IRK, SMP_IRK_LEN, (uint8_t *)BufferPtr(buffer), SMP_IRK_LEN) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_INFO);
    if ((SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) ||
        (SMP_GetPairMng()->masterEncryptedFlag == SMP_MASTER_ENCRYPTED_FLAG_NO)) {
        return;
    }
    AlarmCancel(SMP_GetPairMng()->alarm);
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
    if ((SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) &&
        (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY)) {
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 is started.");
        LOG_INFO("Local is need to send ENC_KEY.");
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local is need to send SIGN_KEY.");
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 is started.");
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        } else {
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_19 is started.");
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_MASTER_STEP_19);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        LOG_INFO("Send keys to Remote.");
        ret = SMP_SendDistributionKeysToRemote();
    } else {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        LOG_INFO("Pair success!");
        return;
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        LOG_INFO("Pair failed!");
    }
}

static void SMP_RecvIdentInfoProcessSlave(const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);

    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_20) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_21)) {
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_20");
        LOG_ERROR("and It's not SMP_SC_PAIR_COMMON_SLAVE_STEP_21");
        return;
    }

    if (memcpy_s(SMP_GetPairMng()->peer.IRK, SMP_IRK_LEN, (uint8_t *)BufferPtr(buffer), SMP_IRK_LEN) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_INFO);
    if (SMP_GetPairMng()->peer.keyDistCmdFlag == 0x00) {
        LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
        AlarmCancel(SMP_GetPairMng()->alarm);
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvIdentAddrInfoProcessMaster(const Buffer *buffer)
{
    int ret = SMP_SUCCESS;
    uint8_t *pData1 = (uint8_t *)BufferPtr(buffer);
    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
        (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_12) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_17) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_18)) {
        LOG_ERROR("It's invalid step.");
        return;
    }
    SMP_GetPairMng()->peer.identityAddr.type = pData1[0x00];
    if (memcpy_s(SMP_GetPairMng()->peer.identityAddr.addr, BT_ADDRESS_SIZE, &(pData1[0x01]), BT_ADDRESS_SIZE) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_ADDR);
    if ((SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) ||
        (SMP_GetPairMng()->masterEncryptedFlag == SMP_MASTER_ENCRYPTED_FLAG_NO)) {
        return;
    }
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0");
    AlarmCancel(SMP_GetPairMng()->alarm);
    if ((SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) &&
        (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY)) {
        LOG_INFO("Local is need to send ENC_KEY!");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 is started!");
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local is need to send SIGN_KEY!");
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 is started!");
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        } else {
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_19 is started!");
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_MASTER_STEP_19);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        LOG_INFO("Send Keys to Remote.");
        ret = SMP_SendDistributionKeysToRemote();
    } else {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        LOG_INFO("Pair Success");
        return;
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        LOG_INFO("Pair Failed");
    }
}

static void SMP_RecvIdentAddrInfoProcessSlave(const Buffer *buffer)
{
    uint8_t *pData2 = NULL;

    LOG_INFO("%{public}s", __FUNCTION__);

    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_20) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_21)) {
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_20.");
        LOG_ERROR("and It's not SMP_SC_PAIR_COMMON_SLAVE_STEP_21.");
        return;
    }

    pData2 = (uint8_t *)BufferPtr(buffer);
    SMP_GetPairMng()->peer.identityAddr.type = pData2[0x00];
    if (memcpy_s(SMP_GetPairMng()->peer.identityAddr.addr,
        sizeof(SMP_GetPairMng()->peer.identityAddr.addr),
        &(pData2[0x01]),
        sizeof(SMP_GetPairMng()->peer.identityAddr.addr)) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            return;
        }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_ADDR);
    if (SMP_GetPairMng()->peer.keyDistCmdFlag == 0x00) {
        AlarmCancel(SMP_GetPairMng()->alarm);
        LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

static void SMP_RecvSignInfoProcessMaster(const Buffer *buffer)
{
    int ret = SMP_SUCCESS;
    LOG_INFO("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
        (SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_12) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_17) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_18)) {
        LOG_ERROR("It's invalid step.");
        return;
    }
    if (memcpy_s(SMP_GetPairMng()->peer.CSRK, SMP_CSRK_LEN, (uint8_t *)BufferPtr(buffer), SMP_CSRK_LEN) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_SIGN_INFO);
    if ((SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) ||
        (SMP_GetPairMng()->masterEncryptedFlag == SMP_MASTER_ENCRYPTED_FLAG_NO)) {
        return;
    }
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
    AlarmCancel(SMP_GetPairMng()->alarm);
    if ((SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) &&
        (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY)) {
        LOG_INFO("Local is need to send ENC_KEY");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 is started");
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local is need to send SIGN_KEY");
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 is started");
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        } else {
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_19 is started");
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_MASTER_STEP_19);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        LOG_INFO("Send Keys to Remote!");
        ret = SMP_SendDistributionKeysToRemote();
    } else {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        LOG_INFO("Pair is Success!");
        return;
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        LOG_INFO("Pair is Failed!");
    }
}

static void SMP_RecvSignInfoProcessSlave(const Buffer *buffer)
{
    LOG_INFO("%{public}s", __FUNCTION__);

    if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_20) &&
        (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_21)) {
        LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_20.");
        LOG_ERROR("and It's not SMP_SC_PAIR_COMMON_SLAVE_STEP_21.");
        return;
    }

    if (memcpy_s(SMP_GetPairMng()->peer.CSRK, SMP_CSRK_LEN, (uint8_t *)BufferPtr(buffer), SMP_CSRK_LEN) != EOK) {
        LOG_ERROR("%{public}s, memcpy_s fail", __FUNCTION__);
        return;
    }
    SMP_GetPairMng()->peer.keyDistCmdFlag =
        SMP_GetPairMng()->peer.keyDistCmdFlag & (~SMP_KEY_DIST_CMD_FLAG_BIT_SIGN_INFO);
    if (SMP_GetPairMng()->peer.keyDistCmdFlag == 0x00) {
        LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
        AlarmCancel(SMP_GetPairMng()->alarm);
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}