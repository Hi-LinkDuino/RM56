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

#include "smp_send.h"

#include <string.h>

#include "btm/btm_thread.h"
#include "log.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "smp.h"
#include "smp_aes_encryption.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_def.h"
#include "smp_hci_event.h"
#include "smp_legacy.h"
#include "smp_oob.h"
#include "smp_privacy.h"
#include "smp_sc_accepter.h"
#include "smp_tool.h"

static List *g_smpEncCmdList = NULL;

static void SMP_SendKeysToRemoteCbProcess(uint16_t aclHandle, int result, uint8_t keyDistCmdFlag);
static int SMP_SendDistKeysToRemoteEncKey();
static int SMP_SendDistKeysToRemoteIdKey();
static int SMP_SendDistKeysToRemoteSignKey();
static void SMP_SendKeysCbProcException(uint16_t aclHandle);
static void SMP_EncCmdTimeoutGenSign();
static void SMP_EncCmdTimeoutGenRpa();
static void SMP_EncCmdTimeoutResoRpa(const SMP_EncCmd *pEncCmd);
static void SMP_EncCmdTimeoutGenScOobData();
static void SMP_EncCmdTimeoutDefault();
static int SMP_Aes128Hardware(const HciLeEncryptParam *pEncryptParam, SMP_EncCmd *encCmd);
static int SMP_Aes128Software(const HciLeEncryptParam *pEncryptParam, SMP_EncCmd *encCmd);

static void SMP_EncCmdTimeoutTask(void *context);
static void SMP_SendDataCbTask(void *context);
static void SMP_SendPairRspCbTask(void *context);
static void SMP_SendEncInfoCbTask(void *context);
static void SMP_SendMasterIdentCbTask(void *context);
static void SMP_SendIdentInfoCbTask(void *context);
static void SMP_SendIdentAddrInfoCbTask(void *context);
static void SMP_SendSignInfoCbTask(void *context);

int SMP_SendDistributionKeysToRemote()
{
    int ret = SMP_SUCCESS;

    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY) {
        ret = SMP_SendDistKeysToRemoteEncKey();
        if (ret != SMP_SUCCESS) {
            return ret;
        }
    }

    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        ret = SMP_SendDistKeysToRemoteIdKey();
        if (ret != SMP_SUCCESS) {
            return ret;
        }
    }

    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        ret = SMP_SendDistKeysToRemoteSignKey();
        if (ret != SMP_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

static int SMP_SendDistKeysToRemoteEncKey()
{
    uint8_t ltkTemp[SMP_LTK_LEN] = {0x00};
    uint8_t masterIdRandTemp[SMP_MASTER_RAND_LEN] = {0x00};

    LOG_INFO("Local need to send ENC_KEY.");

    (void)memcpy_s(ltkTemp, SMP_LTK_LEN, SMP_GetPairMng()->local.LTK, SMP_LTK_LEN);
    int ret = SMP_SendEncryptionInformation(SMP_GetPairMng()->handle, ltkTemp, SMP_SendEncryptionInformationCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Encryption Information failed.");
        ret = SMP_ERR_REMOTE_ACTION;
        return ret;
    }

    SMP_MemoryReverseCopy(masterIdRandTemp, SMP_GetPairMng()->local.masterIdRand, SMP_MASTER_RAND_LEN);
    ret = SMP_SendMasterIdentification(SMP_GetPairMng()->handle,
        SMP_GetPairMng()->local.masterIdEdiv,
        masterIdRandTemp,
        SMP_SendMasterIdentificationCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Master Identification failed.");
        ret = SMP_ERR_REMOTE_ACTION;
        return ret;
    }

    return ret;
}

static int SMP_SendDistKeysToRemoteIdKey()
{
    uint8_t irkTemp[SMP_IRK_LEN] = {0x00};
    BtAddr identAddrTemp;

    LOG_INFO("Local need to send ID_KEY.");

    SMP_GetLocalIrk(SMP_GetPairMng()->local.IRK, sizeof(SMP_GetPairMng()->local.IRK));
    (void)memcpy_s(irkTemp, SMP_IRK_LEN, SMP_GetPairMng()->local.IRK, SMP_IRK_LEN);
    int ret = SMP_SendIdentityInformation(SMP_GetPairMng()->handle, irkTemp, SMP_SendIdentityInformationCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Identity Information failed.");
        ret = SMP_ERR_REMOTE_ACTION;
        return ret;
    }

    SMP_GetLocalIdentAddr(&SMP_GetPairMng()->local.identityAddr);
    identAddrTemp.type = SMP_GetPairMng()->local.identityAddr.type;
    SMP_MemoryReverseCopy(identAddrTemp.addr, SMP_GetPairMng()->local.identityAddr.addr, BT_ADDRESS_SIZE);
    ret = SMP_SendIdentityAddressInformation(
        SMP_GetPairMng()->handle, &identAddrTemp, SMP_SendIdentityAddressInformationCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Identity Address Information failed.");
        ret = SMP_ERR_REMOTE_ACTION;
        return ret;
    }

    return ret;
}

static int SMP_SendDistKeysToRemoteSignKey()
{
    uint8_t csrkTemp[SMP_CSRK_LEN] = {0x00};

    LOG_INFO("Local need to send SIGN_KEY.");

    (void)memcpy_s(csrkTemp, SMP_CSRK_LEN, SMP_GetPairMng()->local.CSRK, SMP_CSRK_LEN);
    int ret = SMP_SendSigningInformation(SMP_GetPairMng()->handle, csrkTemp, SMP_SendSigningInformationCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Signing Information failed.");
        ret = SMP_ERR_REMOTE_ACTION;
        return ret;
    }

    return ret;
}

int SMP_SendHciLeRandCmd(uint16_t step)
{
    if (step) {
        SMP_GetPairMng()->step = step;
    }
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeRand();
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeRand failed.");
        AlarmCancel(SMP_GetPairMng()->alarm);
    }

    return ret;
}

int SMP_SendLeEncryptCmd(
    const HciLeEncryptParam *pEncryptParam, uint16_t step, const SMP_EncCmd *pEncCmdData, bool isUsingHw)
{
    int ret;
    SMP_EncCmd *encCmd = SMP_AllocEncCmd();
    if (encCmd == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }

    if (step) {
        if ((step >= SMP_PAIR_STEP_START) && (step <= SMP_PAIR_STEP_END)) {
            SMP_GetPairMng()->step = step;
            encCmd->step = step;
        } else {
            encCmd->step = step;
        }
    }

    if (pEncCmdData != NULL) {
        (void)memcpy_s(encCmd->X, sizeof(encCmd->X), pEncCmdData->X, sizeof(encCmd->X));
        (void)memcpy_s(encCmd->Y, sizeof(encCmd->Y), pEncCmdData->Y, sizeof(encCmd->Y));
        if (pEncCmdData->length != 0) {
            encCmd->length = pEncCmdData->length;
            encCmd->M = MEM_MALLOC.alloc(encCmd->length);
            (void)memcpy_s(encCmd->M, encCmd->length, pEncCmdData->M, encCmd->length);
        }
        (void)memcpy_s(encCmd->M_last, sizeof(encCmd->M_last), pEncCmdData->M_last, sizeof(encCmd->M_last));
        (void)memcpy_s(encCmd->key, sizeof(encCmd->key), pEncCmdData->key, sizeof(encCmd->key));
        encCmd->n = pEncCmdData->n;
        encCmd->i = pEncCmdData->i;
        encCmd->signDataLen = pEncCmdData->signDataLen;
        encCmd->signCounter = pEncCmdData->signCounter;
        (void)memcpy_s(encCmd->address, sizeof(encCmd->address), pEncCmdData->address, sizeof(encCmd->address));
    } else {
        (void)memcpy_s(encCmd->key, sizeof(encCmd->key), pEncryptParam->key, sizeof(encCmd->key));
    }

    SMP_ListAddLast(g_smpEncCmdList, encCmd);
    AlarmSet(encCmd->timeoutTimer, SMP_PAIR_WAIT_TIME, SMP_EncCmdTimeout, encCmd);
    if (isUsingHw) {
        ret = SMP_Aes128Hardware(pEncryptParam, encCmd);
    } else {
        ret = SMP_Aes128Software(pEncryptParam, encCmd);
    }

    return ret;
}

void SMP_SendDataCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendDataCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendDataCbTask(void *context)
{
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (param->result) {
        LOG_ERROR("Send Data failed result = %{public}d", param->result);
        SMP_GeneratePairResult(
            param->aclHandle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, SMP_GetPairMng()->alarm);
        MEM_MALLOC.free(param);
        return;
    }
    MEM_MALLOC.free(param);
}

void SMP_SendPairingResponseCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendPairRspCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendPairRspCbTask(void *context)
{
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (param->result) {
        LOG_ERROR("Send Data failed result = %{public}d", param->result);
        SMP_GeneratePairResult(param->aclHandle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        MEM_MALLOC.free(param);
        return;
    }
    MEM_MALLOC.free(param);
}

void SMP_SendPairingFailedCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);

    if (result) {
        LOG_ERROR("Send Data failed result = %{public}d", result);
    }
    (void)aclHandle;
}

void SMP_SendSecurityRequestCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);

    if (result) {
        LOG_ERROR("Send Data failed result = %{public}d", result);
    }
    (void)aclHandle;
}

void SMP_SendEncryptionInformationCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendEncInfoCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendEncInfoCbTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;
    SMP_SendKeysToRemoteCbProcess(param->aclHandle, param->result, SMP_KEY_DIST_CMD_FLAG_BIT_ENC_INFO);
    MEM_MALLOC.free(param);
}

void SMP_SendMasterIdentificationCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendMasterIdentCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendMasterIdentCbTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;
    SMP_SendKeysToRemoteCbProcess(param->aclHandle, param->result, SMP_KEY_DIST_CMD_FLAG_BIT_MASTER_IDENT);
    MEM_MALLOC.free(param);
}

void SMP_SendIdentityInformationCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendIdentInfoCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendIdentInfoCbTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;
    SMP_SendKeysToRemoteCbProcess(param->aclHandle, param->result, SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_INFO);
    MEM_MALLOC.free(param);
}

void SMP_SendIdentityAddressInformationCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendIdentAddrInfoCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendIdentAddrInfoCbTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;
    SMP_SendKeysToRemoteCbProcess(param->aclHandle, param->result, SMP_KEY_DIST_CMD_FLAG_BIT_IDENT_ADDR);
    MEM_MALLOC.free(param);
}

void SMP_SendSigningInformationCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *ctx = MEM_MALLOC.alloc(sizeof(SMP_SendL2capDataCbTask_t));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_SendL2capDataCbTask_t), 0x00, sizeof(SMP_SendL2capDataCbTask_t));
    ctx->aclHandle = aclHandle;
    ctx->result = result;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_SendSignInfoCbTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_SendSignInfoCbTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_SendL2capDataCbTask_t *param = (SMP_SendL2capDataCbTask_t *)context;
    SMP_SendKeysToRemoteCbProcess(param->aclHandle, param->result, SMP_KEY_DIST_CMD_FLAG_BIT_SIGN_INFO);
    MEM_MALLOC.free(param);
}

void SMP_FreeEncCmd(void *encCmd)
{
    SMP_EncCmd *pEncCmd = (SMP_EncCmd *)encCmd;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (pEncCmd != NULL) {
        if (pEncCmd->timeoutTimer != NULL) {
            AlarmCancel(pEncCmd->timeoutTimer);
            AlarmDelete(pEncCmd->timeoutTimer);
            pEncCmd->timeoutTimer = NULL;
        }
        if (pEncCmd->M != NULL) {
            MEM_MALLOC.free(pEncCmd->M);
        }
        MEM_MALLOC.free(encCmd);
    }
}

SMP_EncCmd *SMP_AllocEncCmd()
{
    SMP_EncCmd *pEncCmd = MEM_MALLOC.alloc(sizeof(SMP_EncCmd));

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (pEncCmd != NULL) {
        pEncCmd->step = 0x00;
        (void)memset_s(pEncCmd, sizeof(SMP_EncCmd), 0x00, sizeof(SMP_EncCmd));
        pEncCmd->timeoutTimer = AlarmCreate("", false);
    }
    return pEncCmd;
}

void SMP_EncCmdTimeout(void *param)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    SMP_EncTimeoutParam *ctx = MEM_MALLOC.alloc(sizeof(SMP_EncTimeoutParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(SMP_EncTimeoutParam), 0x00, sizeof(SMP_EncTimeoutParam));
    ctx->encCmd = (SMP_EncCmd *)param;
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_EncCmdTimeoutTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_EncCmdTimeoutTask(void *context)
{
    SMP_EncTimeoutParam *timeoutParam = (SMP_EncTimeoutParam *)context;
    SMP_EncCmd *pEncCmd = timeoutParam->encCmd;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    ListNode *node = SMP_ListGetFirstNode(g_smpEncCmdList);
    while (node != NULL) {
        if (pEncCmd == SMP_ListGetNodeData(node)) {
            switch (pEncCmd->step) {
                case SMP_GENERATE_SIGNATURE_STEP_1:
                case SMP_GENERATE_SIGNATURE_STEP_2:
                case SMP_GENERATE_SIGNATURE_STEP_3:
                    SMP_EncCmdTimeoutGenSign();
                    break;
                case SMP_GENERATE_RPA_STEP_1:
                    SMP_EncCmdTimeoutGenRpa();
                    break;
                case SMP_RESOLVE_RPA_STEP_1:
                    SMP_EncCmdTimeoutResoRpa(pEncCmd);
                    break;
                case SMP_GENERATE_SC_OOB_DATA_STEP_2:
                case SMP_GENERATE_SC_OOB_DATA_STEP_3:
                case SMP_GENERATE_SC_OOB_DATA_STEP_4:
                    SMP_EncCmdTimeoutGenScOobData();
                    break;
                default:
                    SMP_EncCmdTimeoutDefault();
                    break;
            }
            ListRemoveNode(g_smpEncCmdList, pEncCmd);
            break;
        }
        node = ListGetNextNode(node);
    }
    MEM_MALLOC.free(timeoutParam);
}

static void SMP_SendKeysToRemoteCbProcess(uint16_t aclHandle, int result, uint8_t keyDistCmdFlag)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (result) {
        LOG_ERROR("Send Data failed result = %{public}d", result);
        SMP_SendKeysCbProcException(aclHandle);
    } else {
        SMP_GetPairMng()->local.keyDistCmdFlag = SMP_GetPairMng()->local.keyDistCmdFlag & (~keyDistCmdFlag);
        if (SMP_GetPairMng()->local.keyDistCmdFlag != 0x00) {
            return;
        }
        LOG_INFO("g_smpPairMng.local.keyDistCmdFlag is 0.");
        if (SMP_ROLE_MASTER == SMP_GetPairMng()->role) {
            SMP_GeneratePairResult(aclHandle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        } else {
            if (!((SMP_GetPairMng()->local.pairParam.authReq & SMP_AUTH_REQ_BONDING) &&
                (SMP_GetPairMng()->peer.pairParam.authReq & SMP_AUTH_REQ_BONDING))) {
                LOG_INFO("It's no bonding.");
                AlarmCancel(SMP_GetPairMng()->alarm);
                SMP_GeneratePairResult(aclHandle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
            } else if (SMP_GetPairMng()->peer.keyDist == 0x00) {
                LOG_INFO("g_smpPairMng.peer.keyDist is 0.");
                AlarmCancel(SMP_GetPairMng()->alarm);
                SMP_GeneratePairResult(aclHandle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
            }
        }
    }
}

static void SMP_SendKeysCbProcException(uint16_t aclHandle)
{
    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        SMP_GeneratePairResult(aclHandle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    } else {
        SMP_GeneratePairResult(
            aclHandle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, SMP_GetPairMng()->alarm);
    }
}

static void SMP_EncCmdTimeoutGenSign()
{
    SMP_NotifyCbGenSign(SMP_GENERATE_SIGN_STATUS_FAILED, NULL);
}

static void SMP_EncCmdTimeoutGenRpa()
{
    SMP_NotifyCbGenRpa(SMP_GENERATE_RPA_STATUS_FAILED, NULL);
}

static void SMP_EncCmdTimeoutResoRpa(const SMP_EncCmd *pEncCmd)
{
    uint8_t key[SMP_ENCRYPT_KEY_LEN] = {0x00};
    uint8_t address[BT_ADDRESS_SIZE] = {0x00};

    SMP_MemoryReverseCopy(address, pEncCmd->address, sizeof(address));
    (void)memcpy_s(key, sizeof(key), pEncCmd->key, sizeof(key));
    SMP_NotifyCbResoRpa(SMP_RESOLVE_RPA_STATUS_FAILED, false, address, key);
}

static void SMP_EncCmdTimeoutGenScOobData()
{
    SMP_ClearScOobData(false);
    SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
}

static void SMP_EncCmdTimeoutDefault()
{
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state");
        return;
    }

    if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
        LOG_ERROR("Initiator, step:%hu", SMP_GetPairMng()->step);
    } else {
        LOG_ERROR("Accepter, step:%hu", SMP_GetPairMng()->step);
    }

    SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
}

static int SMP_Aes128Hardware(const HciLeEncryptParam *pEncryptParam, SMP_EncCmd *encCmd)
{
    int ret = HCI_LeEncrypt(pEncryptParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeEncrypt failed.");
        AlarmCancel(encCmd->timeoutTimer);
        SMP_ListRemoveNode(g_smpEncCmdList, encCmd);
    }

    return ret;
}

static int SMP_Aes128Software(const HciLeEncryptParam *pEncryptParam, SMP_EncCmd *encCmd)
{
    uint8_t encryptedData[SMP_ENCRYPT_PLAINTEXTDATA_LEN] = {0x00};

    int ret = SMP_Aes128(pEncryptParam->key,
        sizeof(pEncryptParam->key),
        pEncryptParam->plaintextData,
        sizeof(pEncryptParam->plaintextData),
        encryptedData);
    HciLeEncryptReturnParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeEncryptReturnParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return SMP_ERR_OUT_OF_RES;
    }
    (void)memset_s(ctx, sizeof(HciLeEncryptReturnParam), 0x00, sizeof(HciLeEncryptReturnParam));
    ctx->status = (uint8_t)ret;
    (void)memcpy_s(ctx->encryptedData, sizeof(ctx->encryptedData), encryptedData, sizeof(ctx->encryptedData));
    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeEncCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("SMP_Aes128 failed.");
        MEM_MALLOC.free(ctx);
        AlarmCancel(encCmd->timeoutTimer);
        SMP_ListRemoveNode(g_smpEncCmdList, encCmd);
    }

    return ret;
}

List *SMP_GetEncCmdList()
{
    return g_smpEncCmdList;
}

void SMP_SetEncCmdList(List *list)
{
    g_smpEncCmdList = list;
}