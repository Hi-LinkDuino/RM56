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

#include "smp_hci_event.h"
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
#include "smp_privacy.h"
#include "smp_sc_accepter.h"
#include "smp_sc_initiator.h"
#include "smp_send.h"
#include "smp_tool.h"

static SMP_StepFunc g_genOOB[] = {
    SMP_GenerateScOobDataStep1,
    SMP_GenerateScOobDataStep2,
    SMP_GenerateScOobDataStep3,
    SMP_GenerateScOobDataStep4,
};
static SMP_StepFunc g_genSignature[] = {
    SMP_GenerateSignatureStep1,
    SMP_GenerateSignatureStep2,
    SMP_GenerateSignatureStep3,
};

static SMP_StepFunc g_masLegacy[] = {
    SMP_LegacyPairMasterStep1,
    SMP_LegacyPairMasterStep2,
    SMP_LegacyPairMasterStep3,
    SMP_LegacyPairMasterStep4,
    SMP_LegacyPairMasterStep5,
    NULL,
    NULL,
    SMP_LegacyPairMasterStep8,
    SMP_LegacyPairMasterStep9,
    SMP_LegacyPairMasterStep10,
    NULL,
    NULL,
    SMP_LegacyPairMasterStep13,
    SMP_LegacyPairMasterStep14,
    SMP_LegacyPairMasterStep15,
    SMP_LegacyPairMasterStep16,
    SMP_LegacyPairMasterStep17,
    SMP_LegacyPairMasterStep18,
};

static SMP_StepFunc g_slaLegacy[] = {
    SMP_LegacyPairSlaveStep1,
    NULL,
    NULL,
    SMP_LegacyPairSlaveStep4,
    SMP_LegacyPairSlaveStep5,
    SMP_LegacyPairSlaveStep6,
    SMP_LegacyPairSlaveStep7,
    NULL,
    SMP_LegacyPairSlaveStep9,
    SMP_LegacyPairSlaveStep10,
    SMP_LegacyPairSlaveStep11,
    NULL,
    NULL,
    SMP_LegacyPairSlaveStep14,
    SMP_LegacyPairSlaveStep15,
    SMP_LegacyPairSlaveStep16,
    SMP_LegacyPairSlaveStep17,
    SMP_LegacyPairSlaveStep18,
    SMP_LegacyPairSlaveStep19,
    NULL,
};

static SMP_StepFunc g_masScjw[] = {
    SMP_ScPairJustworkOrNumericMasterStep1,
    SMP_ScPairJustworkOrNumericMasterStep2,
    SMP_ScPairJustworkOrNumericMasterStep3,
    SMP_ScPairJustworkOrNumericMasterStep4,
    SMP_ScPairJustworkOrNumericMasterStep5,
    SMP_ScPairJustworkOrNumericMasterStep6,
    SMP_ScPairJustworkOrNumericMasterStep7,
    SMP_ScPairJustworkOrNumericMasterStep8,
    SMP_ScPairJustworkOrNumericMasterStep9,
    SMP_ScPairJustworkOrNumericMasterStep10,
    SMP_ScPairJustworkOrNumericMasterStep11,
    SMP_ScPairJustworkOrNumericMasterStep12,
    SMP_ScPairJustworkOrNumericMasterStep13,
    NULL,
};

static SMP_StepFunc g_masScpk[] = {
    SMP_ScPairPasskeyEntryMasterStep1,
    SMP_ScPairPasskeyEntryMasterStep2,
    SMP_ScPairPasskeyEntryMasterStep3,
    SMP_ScPairPasskeyEntryMasterStep4,
    NULL,
    SMP_ScPairPasskeyEntryMasterStep6,
    SMP_ScPairPasskeyEntryMasterStep7,
    SMP_ScPairPasskeyEntryMasterStep8,
    SMP_ScPairPasskeyEntryMasterStep9,
    SMP_ScPairPasskeyEntryMasterStep10,
    SMP_ScPairPasskeyEntryMasterStep11,
    SMP_ScPairPasskeyEntryMasterStep12,
    SMP_ScPairPasskeyEntryMasterStep13,
    SMP_ScPairPasskeyEntryMasterStep14,
    SMP_ScPairPasskeyEntryMasterStep15,
};

static SMP_StepFunc g_masScob[] = {
    SMP_ScPairOobMasterStep1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    SMP_ScPairOobMasterStep8,
    SMP_ScPairOobMasterStep9,
    SMP_ScPairOobMasterStep10,
    SMP_ScPairOobMasterStep11,
    SMP_ScPairOobMasterStep12,
    SMP_ScPairOobMasterStep13,
    SMP_ScPairOobMasterStep14,
    SMP_ScPairOobMasterStep15,
};

static SMP_StepFunc g_masScCommon[] = {
    SMP_ScPairCommonMasterStep1,
    SMP_ScPairCommonMasterStep2,
    SMP_ScPairCommonMasterStep3,
    SMP_ScPairCommonMasterStep4,
    SMP_ScPairCommonMasterStep5,
    SMP_ScPairCommonMasterStep6,
    SMP_ScPairCommonMasterStep7,
    SMP_ScPairCommonMasterStep8,
    SMP_ScPairCommonMasterStep9,
    SMP_ScPairCommonMasterStep10,
    SMP_ScPairCommonMasterStep11,
    SMP_ScPairCommonMasterStep12,
    SMP_ScPairCommonMasterStep13,
    SMP_ScPairCommonMasterStep14,
    SMP_ScPairCommonMasterStep15,
    SMP_ScPairCommonMasterStep16,
    NULL,
    NULL,
    SMP_ScPairCommonMasterStep19,
    SMP_ScPairCommonMasterStep20,
};

static SMP_StepFunc g_slaScjw[] = {
    SMP_ScPairJustworkOrNumericSlaveStep1,
    SMP_ScPairJustworkOrNumericSlaveStep2,
    SMP_ScPairJustworkOrNumericSlaveStep3,
    SMP_ScPairJustworkOrNumericSlaveStep4,
    SMP_ScPairJustworkOrNumericSlaveStep5,
    SMP_ScPairJustworkOrNumericSlaveStep6,
    SMP_ScPairJustworkOrNumericSlaveStep7,
    SMP_ScPairJustworkOrNumericSlaveStep8,
    SMP_ScPairJustworkOrNumericSlaveStep9,
    SMP_ScPairJustworkOrNumericSlaveStep10,
    SMP_ScPairJustworkOrNumericSlaveStep11,
    SMP_ScPairJustworkOrNumericSlaveStep12,
    SMP_ScPairJustworkOrNumericSlaveStep13,
    NULL,
};

static SMP_StepFunc g_slaScpk[] = {
    SMP_ScPairPasskeyEntrySlaveStep1,
    SMP_ScPairPasskeyEntrySlaveStep2,
    SMP_ScPairPasskeyEntrySlaveStep3,
    SMP_ScPairPasskeyEntrySlaveStep4,
    NULL,
    SMP_ScPairPasskeyEntrySlaveStep6,
    SMP_ScPairPasskeyEntrySlaveStep7,
    SMP_ScPairPasskeyEntrySlaveStep8,
    SMP_ScPairPasskeyEntrySlaveStep9,
    SMP_ScPairPasskeyEntrySlaveStep10,
    SMP_ScPairPasskeyEntrySlaveStep11,
    SMP_ScPairPasskeyEntrySlaveStep12,
    SMP_ScPairPasskeyEntrySlaveStep13,
    SMP_ScPairPasskeyEntrySlaveStep14,
    SMP_ScPairPasskeyEntrySlaveStep15,
};

static SMP_StepFunc g_slaScob[] = {
    SMP_ScPairOobSlaveStep1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    SMP_ScPairOobSlaveStep8,
    SMP_ScPairOobSlaveStep9,
    SMP_ScPairOobSlaveStep10,
    SMP_ScPairOobSlaveStep11,
    SMP_ScPairOobSlaveStep12,
    SMP_ScPairOobSlaveStep13,
    SMP_ScPairOobSlaveStep14,
    SMP_ScPairOobSlaveStep15,
};

static SMP_StepFunc g_slaScCommon[] = {
    SMP_ScPairCommonSlaveStep1,
    SMP_ScPairCommonSlaveStep2,
    SMP_ScPairCommonSlaveStep3,
    SMP_ScPairCommonSlaveStep4,
    SMP_ScPairCommonSlaveStep5,
    SMP_ScPairCommonSlaveStep6,
    SMP_ScPairCommonSlaveStep7,
    SMP_ScPairCommonSlaveStep8,
    SMP_ScPairCommonSlaveStep9,
    SMP_ScPairCommonSlaveStep10,
    SMP_ScPairCommonSlaveStep11,
    SMP_ScPairCommonSlaveStep12,
    SMP_ScPairCommonSlaveStep13,
    SMP_ScPairCommonSlaveStep14,
    SMP_ScPairCommonSlaveStep15,
    SMP_ScPairCommonSlaveStep16,
    NULL,
    NULL,
    SMP_ScPairCommonSlaveStep19,
    SMP_ScPairCommonSlaveStep20,
    NULL,
};

SMP_StepFunc GetPairTable(uint16_t step)
{
    if (step >= SMP_LEGACY_PAIR_MASTER_STEP_1 && step <= SMP_LEGACY_PAIR_MASTER_STEP_18) {
        return g_masLegacy[step - SMP_LEGACY_PAIR_MASTER_STEP_1];
    } else if (step >= SMP_LEGACY_PAIR_SLAVE_STEP_1 && step <= SMP_LEGACY_PAIR_SLAVE_STEP_20) {
        return g_slaLegacy[step - SMP_LEGACY_PAIR_SLAVE_STEP_1];
    } else if (step >= SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1 &&
        step <= SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_14) {
        return g_masScjw[step - SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1];
    } else if (step >= SMP_SC_PAIR_COMMON_MASTER_STEP_1 && step <= SMP_SC_PAIR_COMMON_MASTER_STEP_20) {
        return g_masScCommon[step - SMP_SC_PAIR_COMMON_MASTER_STEP_1];
    } else if (step >= SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1 && step <= SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_15) {
        return g_masScpk[step - SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1];
    } else if (step >= SMP_SC_PAIR_OOB_MASTER_STEP_1 && step <= SMP_SC_PAIR_OOB_MASTER_STEP_15) {
        return g_masScob[step - SMP_SC_PAIR_OOB_MASTER_STEP_1];
    } else if (step >= SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1 &&
        step <= SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_14) {
        return g_slaScjw[step - SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1];
    } else if (step >= SMP_SC_PAIR_COMMON_SLAVE_STEP_1 && step <= SMP_SC_PAIR_COMMON_SLAVE_STEP_21) {
        return g_slaScCommon[step - SMP_SC_PAIR_COMMON_SLAVE_STEP_1];
    } else if (step >= SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1 && step <= SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_15) {
        return g_slaScpk[step - SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1];
    } else if (step >= SMP_SC_PAIR_OOB_SLAVE_STEP_1 && step <= SMP_SC_PAIR_OOB_SLAVE_STEP_15) {
        return g_slaScob[step - SMP_SC_PAIR_OOB_SLAVE_STEP_1];
    } else if (step >= SMP_GENERATE_SC_OOB_DATA_STEP_1 && step <= SMP_GENERATE_SC_OOB_DATA_STEP_4) {
        return g_genOOB[step - SMP_GENERATE_SC_OOB_DATA_STEP_1];
    } else if (step >= SMP_GENERATE_SIGNATURE_STEP_1 && step <= SMP_GENERATE_SIGNATURE_STEP_3) {
        return g_genSignature[step - SMP_GENERATE_SIGNATURE_STEP_1];
    } else {
        return NULL;
    }
}

void SMP_ExecuteStepFunc(uint16_t step, const SMP_StepParam *param)
{
    SMP_StepFunc nextStepFunc;
    if (step == SMP_RESOLVE_RPA_STEP_1) {
        SMP_ResolveRpaStep1(param);
    } else if (step == SMP_GENERATE_RPA_STEP_1) {
        SMP_GenerateRpaStep1(param);
    } else {
        nextStepFunc = GetPairTable(step);
        if (nextStepFunc != NULL) {
            nextStepFunc(param);
        } else {
            LOG_ERROR("It's invalid step:%hu.", step);
            return;
        }
    }
}

static void SMP_LinkLayerEncryptionComplete(uint16_t handle, uint8_t status);
static void SMP_LinkLayerEncCompMaster();
static void SMP_LinkLayerEncCompSlave();
static void SMP_OnLtkReqLegacyPair();
static void SMP_OnLtkReqScPair();
static void SMP_OnLtkReqEncProc(const HciLeLongTermKeyRequestEventParam *eventParam);
static void SMP_OnGenDHKeyCompScJwOrNumMasterStep3(const HciLeGenerateDHKeyCompleteEventParam *eventParam);
static void SMP_OnGenDHKeyCompScJwOrNumMasterStep4(const HciLeGenerateDHKeyCompleteEventParam *eventParam);
static void SMP_OnReadLocalPubKeyCompStepDistribution(uint16_t step, const SMP_StepParam *param);
static void SMP_OnGenDhKeyCompStepDistribution(uint16_t step, const SMP_StepParam *param);

static void SMP_OnLeRandCompTask(void *context);
static void SMP_OnLeEncChangeTask(void *context);
static void SMP_OnLeEncKeyRefCompTask(void *context);
static void SMP_OnLeLongTermKeyReqTask(void *context);
static void SMP_OnLeLongTermKeyReqReplyCompTask(void *context);
static void SMP_OnLeReadLocalP256PubKeyCompTask(void *context);
static void SMP_OnLeGenDhKeyCompTask(void *context);

void SMP_OnLERandComplete(const HciLeRandReturnParam *returnParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeRandReturnParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeRandReturnParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(HciLeRandReturnParam), 0x00, sizeof(HciLeRandReturnParam));
    (void)memcpy_s(ctx, sizeof(HciLeRandReturnParam), returnParam, sizeof(HciLeRandReturnParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeRandCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeRandCompTask(void *context)
{
    HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)context;
    SMP_StepParam param = {.data = (void *)returnParam};

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's  not  pairing  state.");
        MEM_MALLOC.free(returnParam);
        return;
    }
    AlarmCancel(SMP_GetPairMng()->alarm);
    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu.", returnParam->status);
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    } else {
        SMP_ExecuteStepFunc(SMP_GetPairMng()->step, &param);
    }
    MEM_MALLOC.free(returnParam);
}

void SMP_OnLEEncryptComplete(const HciLeEncryptReturnParam *returnParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeEncryptReturnParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeEncryptReturnParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(HciLeEncryptReturnParam), 0x00, sizeof(HciLeEncryptReturnParam));
    (void)memcpy_s(ctx, sizeof(HciLeEncryptReturnParam), returnParam, sizeof(HciLeEncryptReturnParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeEncCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

void SMP_OnLeEncCompTask(void *context)
{
    SMP_EncCmd *pEncCmdData = NULL;
    ListNode *listNode = NULL;
    HciLeEncryptReturnParam *returnParam = (HciLeEncryptReturnParam *)context;
    SMP_EncData encData = {
        .encRetParam = returnParam,
        .encCmd = NULL,
    };
    SMP_StepParam param = {.data = (void *)&encData};

    LOG_DEBUG("%{public}s", __FUNCTION__);
    listNode = SMP_ListGetFirstNode(SMP_GetEncCmdList());
    if (listNode == NULL) {
        LOG_ERROR("g_smpEncCmdList is NULL.");
        MEM_MALLOC.free(returnParam);
        return;
    }
    pEncCmdData = SMP_ListGetNodeData(listNode);
    if (pEncCmdData == NULL) {
        LOG_ERROR("pEncCmdData is NULL.");
        MEM_MALLOC.free(returnParam);
        return;
    }
    encData.encCmd = pEncCmdData;
    AlarmCancel(pEncCmdData->timeoutTimer);
    SMP_ExecuteStepFunc(pEncCmdData->step, &param);
    SMP_ListRemoveNode(SMP_GetEncCmdList(), pEncCmdData);
    MEM_MALLOC.free(returnParam);
}

void SMP_OnLEEncryptionChange(const HciEncryptionChangeEventParam *eventParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciEncryptionChangeEventParam *ctx = MEM_MALLOC.alloc(sizeof(HciEncryptionChangeEventParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(HciEncryptionChangeEventParam), 0x00, sizeof(HciEncryptionChangeEventParam));
    (void)memcpy_s(ctx, sizeof(HciEncryptionChangeEventParam), eventParam, sizeof(HciEncryptionChangeEventParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeEncChangeTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeEncChangeTask(void *context)
{
    HciEncryptionChangeEventParam *eventParam = (HciEncryptionChangeEventParam *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_LinkLayerEncryptionComplete(eventParam->connectionHandle, eventParam->status);
    MEM_MALLOC.free(eventParam);
}

void SMP_OnLEEncryptionKeyRefreshComplete(const HciEncryptionKeyRefreshCompleteEventParam *eventParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciEncryptionKeyRefreshCompleteEventParam *ctx =
        MEM_MALLOC.alloc(sizeof(HciEncryptionKeyRefreshCompleteEventParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx,
        sizeof(HciEncryptionKeyRefreshCompleteEventParam),
        0x00,
        sizeof(HciEncryptionKeyRefreshCompleteEventParam));
    (void)memcpy_s(ctx,
        sizeof(HciEncryptionKeyRefreshCompleteEventParam),
        eventParam,
        sizeof(HciEncryptionKeyRefreshCompleteEventParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeEncKeyRefCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeEncKeyRefCompTask(void *context)
{
    HciEncryptionKeyRefreshCompleteEventParam *eventParam = (HciEncryptionKeyRefreshCompleteEventParam *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_LinkLayerEncryptionComplete(eventParam->connectionHandle, eventParam->status);
    MEM_MALLOC.free(eventParam);
}

void SMP_OnLELongTermKeyRequest(const HciLeLongTermKeyRequestEventParam *eventParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeLongTermKeyRequestEventParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeLongTermKeyRequestEventParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx, sizeof(HciLeLongTermKeyRequestEventParam), 0x00, sizeof(HciLeLongTermKeyRequestEventParam));
    (void)memcpy_s(
        ctx, sizeof(HciLeLongTermKeyRequestEventParam), eventParam, sizeof(HciLeLongTermKeyRequestEventParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeLongTermKeyReqTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeLongTermKeyReqTask(void *context)
{
    HciLeLongTermKeyRequestNegativeReplyParam negativeReplyParam;
    HciLeLongTermKeyRequestEventParam *eventParam = (HciLeLongTermKeyRequestEventParam *)context;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) && (SMP_GetPairMng()->handle == eventParam->connectionHandle)) {
        LOG_INFO("Pairing process.");
        if (SMP_GetPairMng()->role != SMP_ROLE_SLAVE) {
            LOG_ERROR("It's not accepter role.");
            negativeReplyParam.connectionHandle = SMP_GetPairMng()->handle;
            HCI_LeLongTermKeyRequestNegativeReply(&negativeReplyParam);
            MEM_MALLOC.free(eventParam);
            return;
        }
        if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_12) &&
            (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_17)) {
            LOG_ERROR("It's invalid step:%hu.", SMP_GetPairMng()->step);
            negativeReplyParam.connectionHandle = SMP_GetPairMng()->handle;
            HCI_LeLongTermKeyRequestNegativeReply(&negativeReplyParam);
            MEM_MALLOC.free(eventParam);
            return;
        }
        AlarmCancel(SMP_GetPairMng()->alarm);
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            SMP_OnLtkReqLegacyPair();
        } else {
            SMP_OnLtkReqScPair();
        }
    } else {
        LOG_INFO("Encryption process.");
        SMP_OnLtkReqEncProc(eventParam);
    }
    MEM_MALLOC.free(eventParam);
}

void SMP_OnLELongTermKeyRequestReplyComplete(const HciLeLongTermKeyRequestReplyReturnParam *returnParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeLongTermKeyRequestReplyReturnParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeLongTermKeyRequestReplyReturnParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(
        ctx, sizeof(HciLeLongTermKeyRequestReplyReturnParam), 0x00, sizeof(HciLeLongTermKeyRequestReplyReturnParam));
    (void)memcpy_s(ctx,
        sizeof(HciLeLongTermKeyRequestReplyReturnParam),
        returnParam,
        sizeof(HciLeLongTermKeyRequestReplyReturnParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeLongTermKeyReqReplyCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeLongTermKeyReqReplyCompTask(void *context)
{
    HciLeLongTermKeyRequestReplyReturnParam *returnParam = (HciLeLongTermKeyRequestReplyReturnParam *)context;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) && (SMP_GetPairMng()->handle == returnParam->connectionHandle)) {
        LOG_INFO("Pairing process.");
        if (SMP_GetPairMng()->role != SMP_ROLE_SLAVE) {
            LOG_ERROR("It's not accepter role.");
            MEM_MALLOC.free(returnParam);
            return;
        }
        if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_13) &&
            (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_18)) {
            LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_13.");
            MEM_MALLOC.free(returnParam);
            return;
        }
        if (returnParam->status) {
            LOG_ERROR("eventParam->status = %hhu.", returnParam->status);
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
        }
    } else {
        LOG_INFO("Encryption process.");
        if (returnParam->status) {
            LOG_ERROR("eventParam->status = %hhu.", returnParam->status);
            LOG_ERROR("Encryption failed.");
            SMP_NotifyCbEncComp(returnParam->connectionHandle, SMP_ENCRYPT_STATUS_FAILED);
        }
    }
    MEM_MALLOC.free(returnParam);
}

void SMP_OnLEReadLocalP256PublicKeyComplete(const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeReadLocalP256PublicKeyCompleteEventParam *ctx =
        MEM_MALLOC.alloc(sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(ctx,
        sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam),
        0x00,
        sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam));
    (void)memcpy_s(ctx,
        sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam),
        eventParam,
        sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeReadLocalP256PubKeyCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeReadLocalP256PubKeyCompTask(void *context)
{
    HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam =
        (HciLeReadLocalP256PublicKeyCompleteEventParam *)context;
    SMP_StepParam param = {.data = (void *)eventParam};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state == SMP_STATE_PAIRING) {
        LOG_INFO("It's Pairing State.");
        AlarmCancel(SMP_GetPairMng()->alarm);
        if (eventParam->status) {
            LOG_ERROR("returnParam->status = %hhu.", eventParam->status);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        } else {
            SMP_OnReadLocalPubKeyCompStepDistribution(SMP_GetPairMng()->step, &param);
        }
    } else if (SMP_GetScOobMng()->state == SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_INFO("It's SC OOB data generating state.");
        if (SMP_GetScOobMng()->step == SMP_GENERATE_SC_OOB_DATA_STEP_1) {
            SMP_GenerateScOobDataStep1(&param);
        } else {
            LOG_ERROR("It's invalid step:%hu.", SMP_GetPairMng()->step);
        }
    } else {
        LOG_ERROR("It's invalid state.");
    }
    MEM_MALLOC.free(eventParam);
}

static void SMP_OnReadLocalPubKeyCompStepDistribution(uint16_t step, const SMP_StepParam *param)
{
    switch (step) {
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1:
            SMP_ScPairJustworkOrNumericMasterStep1(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1:
            SMP_ScPairPasskeyEntryMasterStep1(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_2:
            SMP_ScPairJustworkOrNumericSlaveStep2(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_2:
            SMP_ScPairPasskeyEntrySlaveStep2(param);
            break;
        case SMP_SC_PAIR_OOB_MASTER_STEP_1:
            SMP_ScPairOobMasterStep1(param);
            break;
        case SMP_SC_PAIR_OOB_SLAVE_STEP_1:
            SMP_ScPairOobSlaveStep1(param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", step);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

void SMP_OnLEGenerateDHKeyComplete(const HciLeGenerateDHKeyCompleteEventParam *eventParam)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    HciLeGenerateDHKeyCompleteEventParam *ctx = MEM_MALLOC.alloc(sizeof(HciLeGenerateDHKeyCompleteEventParam));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }
    (void)memset_s(
        ctx, sizeof(HciLeGenerateDHKeyCompleteEventParam), 0x00, sizeof(HciLeGenerateDHKeyCompleteEventParam));
    (void)memcpy_s(
        ctx, sizeof(HciLeGenerateDHKeyCompleteEventParam), eventParam, sizeof(HciLeGenerateDHKeyCompleteEventParam));
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_OnLeGenDhKeyCompTask, (void *)ctx);
    if (ret != SMP_SUCCESS) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SMP_OnLeGenDhKeyCompTask(void *context)
{
    HciLeGenerateDHKeyCompleteEventParam *eventParam = (HciLeGenerateDHKeyCompleteEventParam *)context;
    SMP_StepParam param = {.data = (void *)eventParam};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (SMP_GetPairMng()->state != SMP_STATE_PAIRING) {
        LOG_ERROR("It's not pairing state.");
        MEM_MALLOC.free(eventParam);
        return;
    }
    AlarmCancel(SMP_GetPairMng()->alarm);
    if (eventParam->status) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        LOG_ERROR("returnParam->status = %hhu.", eventParam->status);
    } else {
        SMP_OnGenDhKeyCompStepDistribution(SMP_GetPairMng()->step, &param);
    }
    MEM_MALLOC.free(eventParam);
}

static void SMP_OnGenDhKeyCompStepDistribution(uint16_t step, const SMP_StepParam *param)
{
    switch (step) {
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_3:
            SMP_OnGenDHKeyCompScJwOrNumMasterStep3((const HciLeGenerateDHKeyCompleteEventParam *)param->data);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_4:
            SMP_OnGenDHKeyCompScJwOrNumMasterStep4((const HciLeGenerateDHKeyCompleteEventParam *)param->data);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_3:
            SMP_ScPairPasskeyEntryMasterStep3(param);
            break;
        case SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_3:
            SMP_ScPairJustworkOrNumericSlaveStep3(param);
            break;
        case SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_3:
            SMP_ScPairPasskeyEntrySlaveStep3(param);
            break;
        case SMP_SC_PAIR_OOB_MASTER_STEP_9:
            SMP_ScPairOobMasterStep9(param);
            break;
        case SMP_SC_PAIR_OOB_SLAVE_STEP_9:
            SMP_ScPairOobSlaveStep9(param);
            break;
        default:
            LOG_ERROR("It's invalid step:%hu.", step);
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            break;
    }
}

static void SMP_OnGenDHKeyCompScJwOrNumMasterStep3(const HciLeGenerateDHKeyCompleteEventParam *eventParam)
{
    SMP_MemoryReverseCopy(SMP_GetPairMng()->DHKey, eventParam->DHKey, SMP_DHKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_4 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_4;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
}

static void SMP_OnGenDHKeyCompScJwOrNumMasterStep4(const HciLeGenerateDHKeyCompleteEventParam *eventParam)
{
    SMP_MemoryReverseCopy(SMP_GetPairMng()->DHKey, eventParam->DHKey, SMP_DHKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_5 started.");
    if (SMP_SendHciLeRandCmd(SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_5)) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

static void SMP_LinkLayerEncryptionComplete(uint16_t handle, uint8_t status)
{
    if ((SMP_GetPairMng()->state == SMP_STATE_PAIRING) && (SMP_GetPairMng()->handle == handle)) {
        LOG_INFO("Pairing process.");
        if (SMP_GetPairMng()->role == SMP_ROLE_MASTER) {
            LOG_INFO("It's initiator role.");
            if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_MASTER_STEP_11) &&
                (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_MASTER_STEP_17)) {
                LOG_ERROR("It's not SMP_LEGACY_PAIR_MASTER_STEP_11.");
                LOG_ERROR("and It's not SMP_SC_PAIR_COMMON_MASTER_STEP_17.");
                return;
            }
            AlarmCancel(SMP_GetPairMng()->alarm);
            if (status) {
                SMP_GeneratePairResult(
                    SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
                LOG_ERROR("eventParam->status = %hhu.", status);
                return;
            }
            SMP_GetPairMng()->masterEncryptedFlag = SMP_MASTER_ENCRYPTED_FLAG_YES;
            SMP_LinkLayerEncCompMaster();
        } else {
            LOG_INFO("It's accepter role.");
            if ((SMP_GetPairMng()->step != SMP_LEGACY_PAIR_SLAVE_STEP_13) &&
                (SMP_GetPairMng()->step != SMP_SC_PAIR_COMMON_SLAVE_STEP_18)) {
                LOG_ERROR("It's not SMP_LEGACY_PAIR_SLAVE_STEP_13.");
                LOG_ERROR("and It's not SMP_SC_PAIR_COMMON_SLAVE_STEP_18.");
                return;
            }
            AlarmCancel(SMP_GetPairMng()->alarm);
            if (status) {
                LOG_ERROR("eventParam->status = %hhu.", status);
                SMP_GeneratePairResult(
                    SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
                return;
            }
            SMP_LinkLayerEncCompSlave();
        }
    }
}

static void SMP_LinkLayerEncCompMaster()
{
    int ret = SMP_SUCCESS;
    if (SMP_GetPairMng()->peer.keyDistCmdFlag != 0x00) {
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_12 started.");
            SMP_GetPairMng()->step = SMP_LEGACY_PAIR_MASTER_STEP_12;
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        } else {
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_18 started.");
            SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_MASTER_STEP_18;
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        }
        return;
    }
    LOG_INFO("g_smpPairMng.peer.keyDistCmdFlag is 0.");
    if ((SMP_GetPairMng()->pairType) == SMP_PAIR_TYPE_LEGACY &&
        (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY)) {
        LOG_INFO("Local  need  to  send  ENC_KEY.");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_13 Started.");
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_13);
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local  Need  to  send  SIGN_KEY.");
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17 Started.");
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        } else {
            LOG_DEBUG("SMP_SC_PAIR_COMMON_MASTER_STEP_19 Started.");
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_MASTER_STEP_19);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        ret = SMP_SendDistributionKeysToRemote();
        LOG_INFO("Send  Keys To Remote");
    } else {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        return;
    }
    if (ret != SMP_SUCCESS) {
        LOG_INFO("Pair  Failed.");
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

static void SMP_LinkLayerEncCompSlave()
{
    int ret = SMP_SUCCESS;
    if ((SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) &&
        (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY)) {
        ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_14);
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_18);
        } else {
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_SLAVE_STEP_19);
        }
    } else if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY) {
        if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
            SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_20;
        } else {
            SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_21;
        }
        AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = SMP_SendDistributionKeysToRemote();
        if (ret != SMP_SUCCESS) {
            AlarmCancel(SMP_GetPairMng()->alarm);
        }
    } else {
        if (SMP_GetPairMng()->peer.keyDist == 0x00) {
            LOG_INFO("g_smpPairMng.peer.keyDist  is  0.");
            SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
            return;
        } else {
            if (SMP_GetPairMng()->pairType == SMP_PAIR_TYPE_LEGACY) {
                SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_20;
            } else {
                SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_21;
            }
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        }
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

static void SMP_OnLtkReqLegacyPair()
{
    HciLeLongTermKeyRequestReplyParam replycmdParam;

    replycmdParam.connectionHandle = SMP_GetPairMng()->handle;
    (void)memcpy_s(replycmdParam.longTermKey, SMP_LTK_LEN, SMP_GetPairMng()->STK, SMP_STK_LEN);
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_13;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeLongTermKeyRequestReply(&replycmdParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeLongTermKeyRequestReply failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

static void SMP_OnLtkReqScPair()
{
    HciLeLongTermKeyRequestReplyParam replycmdParam;

    replycmdParam.connectionHandle = SMP_GetPairMng()->handle;
    (void)memcpy_s(replycmdParam.longTermKey, SMP_LTK_LEN, SMP_GetPairMng()->local.LTK, SMP_LTK_LEN);
    SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_18;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeLongTermKeyRequestReply(&replycmdParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeLongTermKeyRequestReply failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

static void SMP_OnLtkReqEncProc(const HciLeLongTermKeyRequestEventParam *eventParam)
{
    uint16_t ediv;
    uint8_t edivTemp[sizeof(ediv)] = {0x00};
    uint8_t randomTemp[SMP_MASTER_RAND_LEN] = {0x00};

    edivTemp[0x00] = ((uint8_t)((eventParam->encryptedDiversifier) & 0xFF));
    edivTemp[0x01] = ((uint8_t)(((eventParam->encryptedDiversifier) >> 0x08) & 0xFF));
    ediv = *((uint16_t *)edivTemp);
    SMP_MemoryReverseCopy(randomTemp, eventParam->randomNumber, SMP_MASTER_RAND_LEN);
    SMP_NotifyCbLtkReq(eventParam->connectionHandle, randomTemp, ediv);
}