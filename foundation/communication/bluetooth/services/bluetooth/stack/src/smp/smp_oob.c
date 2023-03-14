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

#include "smp_oob.h"

#include <string.h>

#include "btm/btm_thread.h"
#include "log.h"
#include "platform/include/random.h"
#include "smp.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_def.h"
#include "smp_send.h"
#include "smp_tool.h"

static SMP_ScOobMng g_smpScOobMng = {0x00};

static void SMP_GenScOobDataTimeoutTask(void *context);

void SMP_GenerateScOobDataStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint32_t number;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    uint8_t offset = 0x00;
    HciLeEncryptParam encryptParam;
    const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam =
        (HciLeReadLocalP256PublicKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(g_smpScOobMng.publicKey, SMP_PUBLICKEY_LEN, eventParam->localP256PublicKey, SMP_PUBLICKEY_LEN);
    number = RandomGenerate();
    (void)memcpy_s(g_smpScOobMng.random, SMP_RANDOM_DATA_LEN, &number, sizeof(number));
    offset += sizeof(number);
    number = RandomGenerate();
    (void)memcpy_s(g_smpScOobMng.random + offset, SMP_RANDOM_DATA_LEN, &number, sizeof(number));
    offset += sizeof(number);
    number = RandomGenerate();
    (void)memcpy_s(g_smpScOobMng.random + offset, SMP_RANDOM_DATA_LEN, &number, sizeof(number));
    offset += sizeof(number);
    number = RandomGenerate();
    (void)memcpy_s(g_smpScOobMng.random + offset, SMP_RANDOM_DATA_LEN, &number, sizeof(number));

    (void)memset_s(cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN, 0x00, CRYPT_AESCMAC_Z_LEN);
    (void)memcpy_s(encryptParam.key, sizeof(encryptParam.key), g_smpScOobMng.random, SMP_RANDOM_DATA_LEN);
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_GENERATE_SC_OOB_DATA_STEP_2 started.");
    int ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_GENERATE_SC_OOB_DATA_STEP_2, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_GenerateScOobDataStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    int ret;
    SMP_CryptF4Param cryptF4Param;
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep3Param cryptAescmacStep3Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpScOobMng.state != SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's not SC OOB data generating state.");
        return;
    }
    if (encData->encRetParam->status) {
        LOG_ERROR("returnParam->status = %hhu.", encData->encRetParam->status);
        ret = SMP_ERR_INVAL_PARAM;
    } else {
        SMP_GetPublicKeyX(g_smpScOobMng.publicKey, cryptF4Param.U, sizeof(cryptF4Param.U));
        SMP_GetPublicKeyX(g_smpScOobMng.publicKey, cryptF4Param.V, sizeof(cryptF4Param.V));
        cryptF4Param.Z[0x00] = 0x00;
        SMP_CryptographicF4(&cryptF4Param);
        SMP_ConstituteAesCmacStep3Param(encData, cryptF4Param.output, CRYPT_F4_OUT_LEN, &cryptAescmacStep3Param);
        cryptAescmacStep3Param.stepA = SMP_GENERATE_SC_OOB_DATA_STEP_3;
        cryptAescmacStep3Param.stepB = SMP_GENERATE_SC_OOB_DATA_STEP_4;
        ret = SMP_CryptographicAesCmacStep3(&cryptAescmacStep3Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
    }
    (void)memset_s(cryptAescmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_GenerateScOobDataStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpScOobMng.state != SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's not SC OOB data generating state");
        return;
    }
    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu", returnParam->status);
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
        return;
    }
    cryptAesCmacStep4Param.stepA = SMP_GENERATE_SC_OOB_DATA_STEP_3;
    cryptAesCmacStep4Param.stepB = SMP_GENERATE_SC_OOB_DATA_STEP_4;
    cryptAesCmacStep4Param.pEncCmdData = encData->encCmd;
    (void)memcpy_s(cryptAesCmacStep4Param.X, CRYPT_AESCMAC_X_LEN, returnParam->encryptedData, CRYPT_AESCMAC_X_LEN);
    cryptAesCmacStep4Param.isUsingHwAes128 = SMP_USING_HW_AES128_PAIR;
    int ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    if (ret != SMP_SUCCESS) {
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
    }
}

void SMP_GenerateScOobDataStep4(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t confirmTmp[SMP_CONFIRM_DATA_LEN];
    uint8_t randomTmp[SMP_RANDOM_DATA_LEN];

    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpScOobMng.state != SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's not SC OOB data generating state.");
        return;
    }
    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu.", returnParam->status);
        SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
        return;
    }
    g_smpScOobMng.step = 0x00;
    g_smpScOobMng.state = SMP_STATE_SC_OOB_DATA_GENERATED;
    if (g_smpScOobMng.alarm != NULL) {
        AlarmDelete(g_smpScOobMng.alarm);
        g_smpScOobMng.alarm = NULL;
    }
    (void)memcpy_s(g_smpScOobMng.confirm, SMP_CONFIRM_DATA_LEN, returnParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(randomTmp, g_smpScOobMng.random, SMP_RANDOM_DATA_LEN);
    SMP_MemoryReverseCopy(confirmTmp, g_smpScOobMng.confirm, SMP_CONFIRM_DATA_LEN);
    SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_SUCCESS, randomTmp, confirmTmp);
}

void SMP_GenerateScOobDataTimeout(void *parameter)
{
    LOG_INFO("%{public}s", __FUNCTION__);
    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SMP, SMP_GenScOobDataTimeoutTask, NULL);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)parameter;
}

static void SMP_GenScOobDataTimeoutTask(void *context)
{
    LOG_DEBUG("%{public}s", __FUNCTION__);
    if (g_smpScOobMng.state != SMP_STATE_SC_OOB_DATA_GENERATING) {
        LOG_ERROR("It's not SC OOB data generating state.");
        return;
    }
    LOG_DEBUG("step:%hu.", g_smpScOobMng.step);
    SMP_ClearScOobData(false);
    SMP_NotifyCbGenScOobData(SMP_GENERATE_SC_OOB_DATA_FAILED, NULL, NULL);
}

void SMP_ClearScOobData(bool isCancelTimer)
{
    if (g_smpScOobMng.alarm != NULL) {
        if (isCancelTimer) {
            AlarmCancel(g_smpScOobMng.alarm);
        }
        AlarmDelete(g_smpScOobMng.alarm);
    }
    (void)memset_s(&g_smpScOobMng, sizeof(SMP_ScOobMng), 0x00, sizeof(SMP_ScOobMng));
}

SMP_ScOobMng *SMP_GetScOobMng()
{
    return &g_smpScOobMng;
}