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

#include "smp_legacy.h"

#include <string.h>

#include "log.h"
#include "smp.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_send.h"
#include "smp_tool.h"

void SMP_LegacyPairMasterStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint32_t passkey;
    uint16_t handle = SMP_GetPairMng()->handle;
    uint8_t pairMethod = SMP_GetPairMng()->local.pairMethod;
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    passkey = *((uint32_t *)returnParam->randomNumber);
    passkey = passkey % (SMP_MAX_PASSKEY_VALUE + 0x01);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_2 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_MASTER_STEP_2;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbAuthReq(handle, pairMethod, (uint8_t *)&passkey);
}

void SMP_LegacyPairMasterStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_3 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_3);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;
    SMP_CryptC1Step1Param cryptC1Step1Param;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random + SMP_RAND_NUM_LEN,
        SMP_RAND_NUM_LEN,
        returnParam->randomNumber,
        SMP_RAND_NUM_LEN);
    SMP_ConstituteC1Step1Param(
        SMP_ROLE_MASTER, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, &cryptC1Step1Param);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_4 started.");
    int ret = SMP_CryptographicC1Step1(SMP_LEGACY_PAIR_MASTER_STEP_4, &cryptC1Step1Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep4(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptC1Step2Param cryptC1Step2Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_ConstituteC1Step2Param(encData, SMP_ROLE_MASTER, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_5 started.");
    ret = SMP_CryptographicC1Step2(SMP_LEGACY_PAIR_MASTER_STEP_5, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep5(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t confirmTemp[SMP_CONFIRM_DATA_LEN] = {0};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_MASTER);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(SMP_GetPairMng()->local.confirm, returnParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(confirmTemp, SMP_GetPairMng()->local.confirm, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_6 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_MASTER_STEP_6;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingConfirm(SMP_GetPairMng()->handle, confirmTemp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing Confirm failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_LegacyPairMasterStep6(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};

    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.confirm, (uint8_t *)param->data, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_MASTER_STEP_7;
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_7 started.");
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing random error");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_LegacyPairMasterStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_CryptC1Step1Param cryptC1Step1Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.random, (uint8_t *)param->data, SMP_RANDOM_DATA_LEN);
    SMP_ConstituteC1Step1Param(SMP_ROLE_MASTER, SMP_GetPairMng()->peer.random, SMP_RANDOM_DATA_LEN, &cryptC1Step1Param);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_8 started.");
    int ret = SMP_CryptographicC1Step1(SMP_LEGACY_PAIR_MASTER_STEP_8, &cryptC1Step1Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptC1Step2Param cryptC1Step2Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_ConstituteC1Step2Param(encData, SMP_ROLE_MASTER, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_9 started.");
    ret = SMP_CryptographicC1Step2(SMP_LEGACY_PAIR_MASTER_STEP_9, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    HciLeEncryptParam encryptParam;
    uint8_t confirmTemp[SMP_CONFIRM_DATA_LEN] = {0x00};
    SMP_CryptS1Param cryptS1Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_MASTER);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(confirmTemp, returnParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    if (memcmp(confirmTemp, SMP_GetPairMng()->peer.confirm, SMP_CONFIRM_DATA_LEN) != 0x00) {
        LOG_ERROR("Confirm check failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_CONFIRM_VALUE, NULL);
    } else {
        LOG_INFO("Confirm  check  success.");
        LOG_INFO("Start  generate  STK.");
        (void)memcpy_s(
            cryptS1Param.r1, sizeof(cryptS1Param.r1), SMP_GetPairMng()->peer.random, sizeof(cryptS1Param.r1));
        (void)memcpy_s(
            cryptS1Param.r2, sizeof(cryptS1Param.r2), SMP_GetPairMng()->local.random, sizeof(cryptS1Param.r2));
        SMP_CryptographicS1(&cryptS1Param);
        SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->TK, SMP_TK_LEN);
        SMP_MemoryReverseCopy(encryptParam.plaintextData, cryptS1Param.output, sizeof(cryptS1Param.output));
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_10 started.");
        ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_LEGACY_PAIR_MASTER_STEP_10, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    }
}

void SMP_LegacyPairMasterStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeStartEncryptionParam startEncParam;
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_MASTER);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(SMP_GetPairMng()->STK, SMP_STK_LEN, returnParam->encryptedData, SMP_STK_LEN);
    if (SMP_GetPairMng()->encKeySize < SMP_STK_LEN) {
        (void)memset_s(SMP_GetPairMng()->STK + SMP_GetPairMng()->encKeySize,
            (SMP_STK_LEN - SMP_GetPairMng()->encKeySize),
            0x00,
            (SMP_STK_LEN - SMP_GetPairMng()->encKeySize));
    }
    startEncParam.connectionHandle = SMP_GetPairMng()->handle;
    startEncParam.encryptDiversifier = 0x00;
    (void)memset_s(startEncParam.randomNumber, SMP_MASTER_RAND_LEN, 0x00, SMP_MASTER_RAND_LEN);
    (void)memcpy_s(startEncParam.longTermKey, SMP_LTK_LEN, SMP_GetPairMng()->STK, SMP_STK_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_11 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_MASTER_STEP_11;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_GetPairMng()->masterEncryptedFlag = SMP_MASTER_ENCRYPTED_FLAG_NO;
    ret = HCI_LeStartEncryption(&startEncParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeStartEncryption failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_LegacyPairMasterStep13(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.LTK, SMP_LTK_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_14 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_14);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep14(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.LTK + SMP_RAND_NUM_LEN, SMP_RAND_NUM_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    SMP_LongTermKeyCopy(SMP_GetPairMng()->local.LTK, SMP_GetPairMng()->local.LTK, SMP_GetPairMng()->encKeySize);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_15 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_15);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep15(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(&SMP_GetPairMng()->local.masterIdEdiv,
        sizeof(SMP_GetPairMng()->local.masterIdEdiv),
        returnParam->randomNumber,
        sizeof(SMP_GetPairMng()->local.masterIdEdiv));
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_16 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_16);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep16(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.masterIdRand, SMP_MASTER_RAND_LEN, returnParam->randomNumber, SMP_MASTER_RAND_LEN);
    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local Need to send  SIGN_KEY.");
        LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_17  started.");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_17);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if ((SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY) ||
               (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY)) {
        LOG_INFO("Local Need to Send ENC_KEY or SIGN_KEY.");
        if (SMP_SendDistributionKeysToRemote()) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        LOG_INFO("Pair  Success.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
    }
}

void SMP_LegacyPairMasterStep17(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.CSRK, SMP_CSRK_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_MASTER_STEP_18 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_MASTER_STEP_18);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairMasterStep18(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.CSRK + SMP_RAND_NUM_LEN, SMP_RAND_NUM_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_INFO("Send Keys to remote.");
    if (SMP_SendDistributionKeysToRemote()) {
        LOG_INFO("Pair  failed");
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint32_t passkey;
    uint16_t handle = SMP_GetPairMng()->handle;
    uint8_t pairMethod = SMP_GetPairMng()->local.pairMethod;
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    passkey = *((uint32_t *)returnParam->randomNumber);
    passkey = passkey % (SMP_MAX_PASSKEY_VALUE + 0x01);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_2 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_2;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbAuthReq(handle, pairMethod, (uint8_t *)&passkey);
}

void SMP_LegacyPairSlaveStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_GetPairMng()->slaveConfirmRecvFlag = SMP_SLAVE_CONFIRM_RECV_FLAG_YES;
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.confirm, (uint8_t *)param->data, SMP_CONFIRM_DATA_LEN);
}

void SMP_LegacyPairSlaveStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }

    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.confirm, (uint8_t *)param->data, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_4 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_4);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep4(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_5 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_5);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep5(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;
    SMP_CryptC1Step1Param cryptC1Step1Param;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random + SMP_RAND_NUM_LEN,
        SMP_RAND_NUM_LEN,
        returnParam->randomNumber,
        SMP_RAND_NUM_LEN);
    SMP_ConstituteC1Step1Param(SMP_ROLE_SLAVE, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, &cryptC1Step1Param);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_6 started.");
    int ret = SMP_CryptographicC1Step1(SMP_LEGACY_PAIR_SLAVE_STEP_6, &cryptC1Step1Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep6(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptC1Step2Param cryptC1Step2Param;
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_7 started.");
    int ret = SMP_ConstituteC1Step2Param(encData, SMP_ROLE_SLAVE, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        return;
    }
    ret = SMP_CryptographicC1Step2(SMP_LEGACY_PAIR_SLAVE_STEP_7, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint8_t confirm[SMP_CONFIRM_DATA_LEN] = {0x00};
    SMP_EncData *encData1 = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData1->encRetParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }

    SMP_MemoryReverseCopy(SMP_GetPairMng()->local.confirm, returnParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(confirm, SMP_GetPairMng()->local.confirm, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_8 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_8;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingConfirm(SMP_GetPairMng()->handle, confirm, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing Confirm failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_LegacyPairSlaveStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_CryptC1Step1Param cryptC1Step1Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.random, (uint8_t *)param->data, SMP_RANDOM_DATA_LEN);
    SMP_ConstituteC1Step1Param(SMP_ROLE_SLAVE, SMP_GetPairMng()->peer.random, SMP_RANDOM_DATA_LEN, &cryptC1Step1Param);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_9 started.");
    int ret = SMP_CryptographicC1Step1(SMP_LEGACY_PAIR_SLAVE_STEP_9, &cryptC1Step1Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptC1Step2Param cryptC1Step2Param;
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_10 started.");
    int ret = SMP_ConstituteC1Step2Param(encData, SMP_ROLE_SLAVE, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        return;
    }
    ret = SMP_CryptographicC1Step2(SMP_LEGACY_PAIR_SLAVE_STEP_10, &cryptC1Step2Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t confirm[SMP_CONFIRM_DATA_LEN] = {0x00};
    HciLeEncryptParam encryptParam;
    SMP_CryptS1Param cryptS1Param;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(confirm, returnParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    if (memcmp(confirm, SMP_GetPairMng()->peer.confirm, SMP_CONFIRM_DATA_LEN)) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_CONFIRM_VALUE, NULL);
        LOG_ERROR("Confirm Check Failed");
    } else {
        LOG_INFO("Confirm Check Success");
        LOG_INFO("Start Generate STK");
        (void)memcpy_s(
            cryptS1Param.r1, sizeof(cryptS1Param.r1), SMP_GetPairMng()->local.random, sizeof(cryptS1Param.r1));
        (void)memcpy_s(
            cryptS1Param.r2, sizeof(cryptS1Param.r2), SMP_GetPairMng()->peer.random, sizeof(cryptS1Param.r2));
        SMP_CryptographicS1(&cryptS1Param);
        SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->TK, SMP_TK_LEN);
        SMP_MemoryReverseCopy(encryptParam.plaintextData, cryptS1Param.output, sizeof(cryptS1Param.output));
        LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_11 started.");
        ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_LEGACY_PAIR_SLAVE_STEP_11, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    }
}

void SMP_LegacyPairSlaveStep11(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }

    (void)memcpy_s(SMP_GetPairMng()->STK, SMP_STK_LEN, returnParam->encryptedData, SMP_STK_LEN);
    if (SMP_GetPairMng()->encKeySize < SMP_STK_LEN) {
        (void)memset_s(SMP_GetPairMng()->STK + SMP_GetPairMng()->encKeySize,
            (SMP_STK_LEN - SMP_GetPairMng()->encKeySize),
            0x00,
            (SMP_STK_LEN - SMP_GetPairMng()->encKeySize));
    }
    SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_12 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_12;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing Random failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_LegacyPairSlaveStep14(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(SMP_GetPairMng()->local.LTK, SMP_LTK_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);

    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_15 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_15);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep15(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(
        SMP_GetPairMng()->local.LTK + SMP_RAND_NUM_LEN, SMP_RAND_NUM_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    SMP_LongTermKeyCopy(SMP_GetPairMng()->local.LTK, SMP_GetPairMng()->local.LTK, SMP_GetPairMng()->encKeySize);

    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_16 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_16);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep16(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(&SMP_GetPairMng()->local.masterIdEdiv,
        sizeof(SMP_GetPairMng()->local.masterIdEdiv),
        returnParam->randomNumber,
        sizeof(SMP_GetPairMng()->local.masterIdEdiv));

    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_17 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_17);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep17(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.masterIdRand, SMP_MASTER_RAND_LEN, returnParam->randomNumber, SMP_MASTER_RAND_LEN);
    if (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
        LOG_INFO("Local need to send SIGN_KEY.");
        LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_18 started.");
        int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_18);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else if ((SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ENC_KEY) ||
               (SMP_GetPairMng()->local.keyDist & SMP_KEY_DIST_BIT_ID_KEY)) {
        LOG_INFO("Local need to send ENC_KEY or SIGN_KEY");
        SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_20;
        AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_20 Started");
        if (SMP_SendDistributionKeysToRemote()) {
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
        }
    } else {
        if (!SMP_GetPairMng()->peer.keyDist) {
            SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_SUCCESS, 0x00, NULL);
        } else {
            SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_20;
            LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_20 started");
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        }
    }
}

void SMP_LegacyPairSlaveStep18(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(SMP_GetPairMng()->local.CSRK, SMP_CSRK_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);

    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_19 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_LEGACY_PAIR_SLAVE_STEP_19);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_LegacyPairSlaveStep19(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    (void)memcpy_s(
        SMP_GetPairMng()->local.CSRK + SMP_RAND_NUM_LEN, SMP_RAND_NUM_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);

    LOG_INFO("Send keys to remote.");
    LOG_DEBUG("SMP_LEGACY_PAIR_SLAVE_STEP_20 started.");
    SMP_GetPairMng()->step = SMP_LEGACY_PAIR_SLAVE_STEP_20;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    if (SMP_SendDistributionKeysToRemote()) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}