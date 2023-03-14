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

#include "smp_sc_accepter.h"

#include <string.h>

#include "log.h"
#include "smp.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_send.h"
#include "smp_tool.h"

static const uint8_t SALT[SMP_ENCRYPT_KEY_LEN] = {
    0x6C, 0x88, 0x83, 0x91, 0xAA, 0xF5, 0xA5, 0x38, 0x60, 0x37, 0x0B, 0xDB, 0x5A, 0x60, 0x83, 0xBE
};  // The key of an encryption algorithm.

void SMP_ScPairJustworkOrNumericSlaveStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    (void)memcpy_s(SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN, (uint8_t *)param->data, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_2 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_2;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeReadLocalP256PublicKey();
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeReadLocalP256PublicKey failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeGenerateDHKeyParam DHKeyParam;
    const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam =
        (HciLeReadLocalP256PublicKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.publicKey, SMP_PUBLICKEY_LEN, eventParam->localP256PublicKey, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_3 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_3;
    (void)memcpy_s(
        DHKeyParam.remoteP256PublicKey, SMP_PUBLICKEY_LEN, SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN);
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeGenerateDHKey(&DHKeyParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeGenerateDHKey failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeGenerateDHKeyCompleteEventParam *eventParam = (HciLeGenerateDHKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->DHKey, eventParam->DHKey, SMP_DHKEY_LEN);
    int ret =
        SMP_SendPairingPublicKey(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.publicKey, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send pairing public key failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    } else {
        LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_4 started.");
        ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_4);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep4(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_5 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_5);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep5(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeEncryptParam encryptParam;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random + SMP_RAND_NUM_LEN,
        SMP_RAND_NUM_LEN,
        returnParam->randomNumber,
        SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_6 started");
    SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    int ret =
        SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_6, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairJustworkOrNumericSlaveStep6(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF4Param cryptF4Param;
    bool isPasskey = false;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_GetPublicKeyX(SMP_GetPairMng()->local.publicKey, cryptF4Param.U, sizeof(cryptF4Param.U));
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptF4Param.V, sizeof(cryptF4Param.V));
    SMP_ConstituteF4Param(isPasskey, &cryptF4Param);
    SMP_CryptographicF4(&cryptF4Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF4Param.output, CRYPT_F4_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_7;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_8;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairJustworkOrNumericSlaveStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_7,
        SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_8,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t tmpConfirm[SMP_CONFIRM_DATA_LEN] = {0x00};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(SMP_GetPairMng()->local.confirm, encData->encRetParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(tmpConfirm, SMP_GetPairMng()->local.confirm, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_9 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_9;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingConfirm(SMP_GetPairMng()->handle, tmpConfirm, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing Confirm failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    int ret;
    HciLeEncryptParam encryptParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.random, (uint8_t *)param->data, SMP_RANDOM_DATA_LEN);
    if (SMP_GetPairMng()->local.pairMethod == SMP_PAIR_METHOD_JUST_WORK) {
        uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};
        SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
        SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_1;
        LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_1 started.");
        AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
            LOG_ERROR("Send pairing random error.");
        }
    } else {
        SMP_GetPairMng()->slaveDHKeyCheckRecvFlag = SMP_SLAVE_DHKEYCHECK_RECV_FLAG_NO;
        SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->peer.random, sizeof(encryptParam.key));
        uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
        (void)memcpy_s(
            encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
        LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_10 started.");
        ret = SMP_SendLeEncryptCmd(
            &encryptParam, SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_10, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairJustworkOrNumericSlaveStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptG2Param cryptG2Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptG2Param.U, sizeof(cryptG2Param.U));
    SMP_GetPublicKeyX(SMP_GetPairMng()->local.publicKey, cryptG2Param.V, sizeof(cryptG2Param.V));
    (void)memcpy_s(cryptG2Param.Y, sizeof(cryptG2Param.Y), SMP_GetPairMng()->local.random, sizeof(cryptG2Param.Y));
    SMP_CryptographicG2(&cryptG2Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptG2Param.output, CRYPT_G2_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_11;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_12;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairJustworkOrNumericSlaveStep11(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_11,
        SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_12,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep12(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint16_t handle = SMP_GetPairMng()->handle;
    uint8_t pairMethod = SMP_GetPairMng()->local.pairMethod;
    uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_13 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_13;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send pairing random error.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    } else {
        uint32_t numeric = *((uint32_t *)encData->encRetParam->encryptedData);
        numeric = numeric % (SMP_MAX_NUMERIC_VALUE + 0x01);
        SMP_NotifyCbAuthReq(handle, pairMethod, (uint8_t *)&numeric);
    }
}

void SMP_ScPairJustworkOrNumericSlaveStep13(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->peer.DHKeyCheck, SMP_DHKEY_CHECK_LEN, (uint8_t *)param->data, SMP_DHKEY_CHECK_LEN);
    SMP_GetPairMng()->slaveDHKeyCheckRecvFlag = SMP_SLAVE_DHKEYCHECK_RECV_FLAG_YES;
}

void SMP_ScPairPasskeyEntrySlaveStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    (void)memcpy_s(SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN, (uint8_t *)param->data, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_2 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_2;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeReadLocalP256PublicKey();
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeReadLocalP256PublicKey failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeGenerateDHKeyParam DHKeyParam;
    const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam =
        (HciLeReadLocalP256PublicKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.publicKey, SMP_PUBLICKEY_LEN, eventParam->localP256PublicKey, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_3 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_3;
    (void)memcpy_s(
        DHKeyParam.remoteP256PublicKey, SMP_PUBLICKEY_LEN, SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN);
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeGenerateDHKey(&DHKeyParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeGenerateDHKey failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint16_t handle = SMP_GetPairMng()->handle;
    uint8_t pairMethod = SMP_GetPairMng()->local.pairMethod;
    const HciLeGenerateDHKeyCompleteEventParam *eventParam = (HciLeGenerateDHKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->DHKey, eventParam->DHKey, SMP_DHKEY_LEN);
    int ret =
        SMP_SendPairingPublicKey(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.publicKey, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send pairing public key failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    } else {
        if (SMP_GetPairMng()->local.pairMethod == SMP_PAIR_METHOD_PASSKEY_DISPLAY) {
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_4 started.");
            SMP_GetPairMng()->scConfirmCheckCounter = 0x00;
            ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_4);
            if (ret != SMP_SUCCESS) {
                SMP_GeneratePairResult(
                    SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
            }
        } else {
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5 started.");
            SMP_GetPairMng()->scConfirmCheckCounter = 0x00;
            SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5;
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
            SMP_NotifyCbAuthReq(handle, pairMethod, NULL);
        }
    }
}

void SMP_ScPairPasskeyEntrySlaveStep4(const SMP_StepParam *param)
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
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbAuthReq(handle, pairMethod, (uint8_t *)&passkey);
}

void SMP_ScPairPasskeyEntrySlaveStep5(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_GetPairMng()->slaveConfirmRecvFlag = SMP_SLAVE_CONFIRM_RECV_FLAG_YES;
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.confirm, (uint8_t *)param->data, SMP_CONFIRM_DATA_LEN);
}

void SMP_ScPairPasskeyEntrySlaveStep6(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.confirm, (uint8_t *)param->data, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_7 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_7);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_8 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_8);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeEncryptParam encryptParam;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random + SMP_RAND_NUM_LEN,
        SMP_RAND_NUM_LEN,
        returnParam->randomNumber,
        SMP_RAND_NUM_LEN);
    SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_9 started");
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    int ret =
        SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_9, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairPasskeyEntrySlaveStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF4Param cryptF4Param;
    bool isPasskey = true;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_GetPublicKeyX(SMP_GetPairMng()->local.publicKey, cryptF4Param.U, sizeof(cryptF4Param.U));
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptF4Param.V, sizeof(cryptF4Param.V));
    SMP_ConstituteF4Param(isPasskey, &cryptF4Param);
    SMP_CryptographicF4(&cryptF4Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF4Param.output, CRYPT_F4_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_10;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_11;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairPasskeyEntrySlaveStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_10,
        SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_11,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep11(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t confirmTmp[SMP_CONFIRM_DATA_LEN] = {0x00};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(SMP_GetPairMng()->local.confirm, encData->encRetParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    SMP_MemoryReverseCopy(confirmTmp, SMP_GetPairMng()->local.confirm, SMP_CONFIRM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_12 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_12;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingConfirm(SMP_GetPairMng()->handle, confirmTmp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing Confirm failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep12(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    HciLeEncryptParam encryptParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.random, (uint8_t *)param->data, SMP_RANDOM_DATA_LEN);
    SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->peer.random, SMP_RANDOM_DATA_LEN);
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_13 started.");
    int ret =
        SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_13, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairPasskeyEntrySlaveStep13(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF4Param cryptF4Param;
    bool isPasskey = true;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptF4Param.U, sizeof(cryptF4Param.U));
    SMP_GetPublicKeyX(SMP_GetPairMng()->local.publicKey, cryptF4Param.V, sizeof(cryptF4Param.V));
    SMP_ConstituteF4Param(isPasskey, &cryptF4Param);
    SMP_CryptographicF4(&cryptF4Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF4Param.output, CRYPT_F4_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_14;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_15;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairPasskeyEntrySlaveStep14(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_14,
        SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_15,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairPasskeyEntrySlaveStep15(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t confirmTmp[SMP_CONFIRM_DATA_LEN] = {0x00};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(confirmTmp, encData->encRetParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    if (memcmp(confirmTmp, SMP_GetPairMng()->peer.confirm, SMP_CONFIRM_DATA_LEN) != 0x00) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_CONFIRM_VALUE, NULL);
        LOG_ERROR("Confirm check failed");
    } else {
        LOG_INFO("Confirm check success");
        uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};
        SMP_GetPairMng()->scConfirmCheckCounter++;
        if (SMP_GetPairMng()->scConfirmCheckCounter == 0x14) {
            SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
            SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_1;
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
            LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_1 started.");
            ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
        } else {
            SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
            LOG_DEBUG("SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6 started.");
            SMP_GetPairMng()->step = SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6;
            AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
            ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
        }
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("Send pairing random error.");
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
        }
    }
}

void SMP_ScPairOobSlaveStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint16_t handle = SMP_GetPairMng()->handle;
    uint8_t pairMethod = SMP_GetPairMng()->local.pairMethod;
    const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam =
        (HciLeReadLocalP256PublicKeyCompleteEventParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.publicKey, SMP_PUBLICKEY_LEN, eventParam->localP256PublicKey, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_7 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_OOB_SLAVE_STEP_7;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_NotifyCbAuthReq(handle, pairMethod, NULL);
}

void SMP_ScPairOobSlaveStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN, (uint8_t *)param->data, SMP_PUBLICKEY_LEN);
    SMP_GetPairMng()->slavePubKeyRecvFlag = SMP_SLAVE_PUBLICKEY_RECV_FLAG_YES;
}

void SMP_ScPairOobSlaveStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    HciLeGenerateDHKeyParam DHKeyParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    (void)memcpy_s(SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN, (uint8_t *)param->data, SMP_PUBLICKEY_LEN);
    SMP_GetPairMng()->step = SMP_SC_PAIR_OOB_SLAVE_STEP_9;
    (void)memcpy_s(
        DHKeyParam.remoteP256PublicKey, SMP_PUBLICKEY_LEN, SMP_GetPairMng()->peer.publicKey, SMP_PUBLICKEY_LEN);
    LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_9 started");
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    int ret = HCI_LeGenerateDHKey(&DHKeyParam);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("HCI_LeGenerateDHKey failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairOobSlaveStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    int ret;
    const HciLeGenerateDHKeyCompleteEventParam *eventParam = (HciLeGenerateDHKeyCompleteEventParam *)param->data;
    HciLeEncryptParam encryptParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->DHKey, eventParam->DHKey, SMP_DHKEY_LEN);
    if ((SMP_GetPairMng()->local.pairMethod == SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV) ||
        (SMP_GetPairMng()->local.pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND)) {
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_10 started");
        SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->peer.oobRand, SMP_RANDOM_DATA_LEN);
        uint8_t tmpZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
        (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), tmpZ, sizeof(tmpZ));
        ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_OOB_SLAVE_STEP_10, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    } else {
        SMP_GetPairMng()->step = SMP_SC_PAIR_OOB_SLAVE_STEP_13;
        AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_13 started.");
        ret =
            SMP_SendPairingPublicKey(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.publicKey, SMP_SendDataCallback);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
            LOG_ERROR("Send pairing public key failed.");
        }
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairOobSlaveStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF4Param cryptF4Param;
    bool isPasskey = false;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptF4Param.U, sizeof(cryptF4Param.U));
    SMP_GetPublicKeyX(SMP_GetPairMng()->peer.publicKey, cryptF4Param.V, sizeof(cryptF4Param.V));
    SMP_ConstituteF4Param(isPasskey, &cryptF4Param);
    SMP_CryptographicF4(&cryptF4Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF4Param.output, CRYPT_F4_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_OOB_SLAVE_STEP_11;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_OOB_SLAVE_STEP_12;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairOobSlaveStep11(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(
        encData, SMP_SC_PAIR_OOB_SLAVE_STEP_11, SMP_SC_PAIR_OOB_SLAVE_STEP_12, SMP_ROLE_SLAVE, &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairOobSlaveStep12(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t tmpConfirm[SMP_CONFIRM_DATA_LEN] = {0x00};
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(tmpConfirm, SMP_CONFIRM_DATA_LEN, encData->encRetParam->encryptedData, SMP_CONFIRM_DATA_LEN);
    if (memcmp(tmpConfirm, SMP_GetPairMng()->peer.confirm, SMP_CONFIRM_DATA_LEN) != 0x00) {
        LOG_ERROR("Confirm Check Failed.");
        SMP_SendPairingPublicKey(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.publicKey, SMP_SendDataCallback);
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_CONFIRM_VALUE, NULL);
    } else {
        LOG_INFO("Confirm Check Success.");
        LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_13 started.");
        SMP_GetPairMng()->step = SMP_SC_PAIR_OOB_SLAVE_STEP_13;
        AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
        ret =
            SMP_SendPairingPublicKey(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.publicKey, SMP_SendDataCallback);
        if (ret != SMP_SUCCESS) {
            LOG_ERROR("Send pairing public key failed.");
            SMP_GeneratePairResult(SMP_GetPairMng()->handle,
                SMP_PAIR_STATUS_FAILED,
                SMP_PAIR_FAILED_UNSPECIFIED_REASION,
                SMP_GetPairMng()->alarm);
        }
    }
}

void SMP_ScPairOobSlaveStep13(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_MemoryReverseCopy(SMP_GetPairMng()->peer.random, (uint8_t *)param->data, SMP_RANDOM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_14 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_OOB_SLAVE_STEP_14);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairOobSlaveStep14(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_SC_PAIR_OOB_SLAVE_STEP_15 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_OOB_SLAVE_STEP_15);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairOobSlaveStep15(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint8_t randomTemp[SMP_RANDOM_DATA_LEN] = {0x00};
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.random + SMP_RAND_NUM_LEN,
        SMP_RAND_NUM_LEN,
        returnParam->randomNumber,
        SMP_RAND_NUM_LEN);
    SMP_MemoryReverseCopy(randomTemp, SMP_GetPairMng()->local.random, SMP_RANDOM_DATA_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_1 started.");
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_1;
    int ret = SMP_SendPairingRandom(SMP_GetPairMng()->handle, randomTemp, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send pairing random error.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairCommonSlaveStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    HciLeEncryptParam encryptParam;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    AlarmCancel(SMP_GetPairMng()->alarm);
    (void)memcpy_s(SMP_GetPairMng()->peer.DHKeyCheck, SMP_DHKEY_CHECK_LEN, (uint8_t *)param->data, SMP_DHKEY_CHECK_LEN);
    SMP_MemoryReverseCopy(encryptParam.key, SALT, sizeof(encryptParam.key));
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_2 started.");
    int ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_2, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep2(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(
        encData, SMP_GetPairMng()->DHKey, sizeof(SMP_GetPairMng()->DHKey), &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_COMMON_SLAVE_STEP_3;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_COMMON_SLAVE_STEP_4;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep3(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_3,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_4,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep4(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    HciLeEncryptParam encryptParam;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(encryptParam.key, sizeof(encryptParam.key), returnParam->encryptedData, sizeof(encryptParam.key));
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_5 started.");
    ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_5, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep5(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF5Param cryptF5Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_ConstituteF5Param(SMP_ROLE_SLAVE, &cryptF5Param);
    SMP_CryptographicF5(&cryptF5Param);
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF5Param.output, CRYPT_F5_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_COMMON_SLAVE_STEP_6;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_COMMON_SLAVE_STEP_7;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep6(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_6,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_7,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep7(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    HciLeEncryptParam encryptParam;
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    SMP_EncCmd *pEncCmdData = encData->encCmd;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_MemoryReverseCopy(SMP_GetPairMng()->macKey, encData->encRetParam->encryptedData, SMP_MACKEY_LEN);
    (void)memcpy_s(encryptParam.key, sizeof(encryptParam.key), pEncCmdData->key, sizeof(encryptParam.key));
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_8 started.");
    ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_8, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep8(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptF5Param cryptF5Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_ConstituteF5Param(SMP_ROLE_SLAVE, &cryptF5Param);
    SMP_CryptographicF5(&cryptF5Param);
    cryptF5Param.output[0x00] = 0x01;
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    SMP_ConstituteAesCmacStep3Param(encData, cryptF5Param.output, CRYPT_F5_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_COMMON_SLAVE_STEP_9;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_COMMON_SLAVE_STEP_10;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(cryptAesCmacStep3Param.key, CRYPT_AESCMAC_KEY_LEN, 0x00, CRYPT_AESCMAC_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep9(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_9,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_10,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep10(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;
    uint8_t tmpLTK[SMP_LTK_LEN] = {0x00};
    uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
    HciLeEncryptParam encryptParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(tmpLTK, SMP_LTK_LEN, returnParam->encryptedData, SMP_LTK_LEN);
    SMP_LongTermKeyCopy(SMP_GetPairMng()->local.LTK, tmpLTK, SMP_GetPairMng()->encKeySize);
    SMP_LongTermKeyCopy(SMP_GetPairMng()->peer.LTK, tmpLTK, SMP_GetPairMng()->encKeySize);
    SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->macKey, SMP_MACKEY_LEN);
    (void)memcpy_s(encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_11 started.");
    ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_11, NULL, SMP_USING_HW_AES128_PAIR);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep11(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_CryptF6Param cryptF6Param;
    bool isCalculatePeer = true;
    SMP_ConstituteF6Param(isCalculatePeer, &cryptF6Param);
    SMP_CryptographicF6(&cryptF6Param);
    SMP_ConstituteAesCmacStep3Param(encData, cryptF6Param.output, CRYPT_F6_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_COMMON_SLAVE_STEP_12;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_COMMON_SLAVE_STEP_13;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep12(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    LOG_DEBUG("%{public}s", __FUNCTION__);
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_12,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_13,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep13(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    uint8_t dhkeyCheckTmp[SMP_DHKEY_CHECK_LEN] = {0x00};
    HciLeEncryptParam encryptParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(dhkeyCheckTmp, SMP_DHKEY_CHECK_LEN, encData->encRetParam->encryptedData, SMP_DHKEY_CHECK_LEN);
    if (memcmp(dhkeyCheckTmp, SMP_GetPairMng()->peer.DHKeyCheck, SMP_DHKEY_CHECK_LEN) != 0x00) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_DHKEY_CHECK, NULL);
        LOG_ERROR("DHKey check failed");
    } else {
        LOG_INFO("DHKey check success");
        SMP_MemoryReverseCopy(encryptParam.key, SMP_GetPairMng()->macKey, SMP_MACKEY_LEN);
        uint8_t cryptAesCmacZ[CRYPT_AESCMAC_Z_LEN] = {0x00};
        (void)memcpy_s(
            encryptParam.plaintextData, sizeof(encryptParam.plaintextData), cryptAesCmacZ, CRYPT_AESCMAC_Z_LEN);
        LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_14 started.");
        ret = SMP_SendLeEncryptCmd(&encryptParam, SMP_SC_PAIR_COMMON_SLAVE_STEP_14, NULL, SMP_USING_HW_AES128_PAIR);
        if (ret != SMP_SUCCESS) {
            SMP_GeneratePairResult(
                SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
        }
    }
    (void)memset_s(encryptParam.key, SMP_ENCRYPT_KEY_LEN, 0x00, SMP_ENCRYPT_KEY_LEN);
}

void SMP_ScPairCommonSlaveStep14(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep3Param cryptAesCmacStep3Param;
    int ret = SMP_EncryptCompleteJudgeException(encData->encRetParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    SMP_CryptF6Param cryptF6Param;
    bool isCalculatePeer = false;
    SMP_ConstituteF6Param(isCalculatePeer, &cryptF6Param);
    SMP_CryptographicF6(&cryptF6Param);
    SMP_ConstituteAesCmacStep3Param(encData, cryptF6Param.output, CRYPT_F6_OUT_LEN, &cryptAesCmacStep3Param);
    cryptAesCmacStep3Param.stepA = SMP_SC_PAIR_COMMON_SLAVE_STEP_15;
    cryptAesCmacStep3Param.stepB = SMP_SC_PAIR_COMMON_SLAVE_STEP_16;
    ret = SMP_CryptographicAesCmacStep3(&cryptAesCmacStep3Param);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep15(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_CryptAesCmacStep4Param cryptAesCmacStep4Param;
    int ret = SMP_ConstituteAesCmacStep4Param(encData,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_15,
        SMP_SC_PAIR_COMMON_SLAVE_STEP_16,
        SMP_ROLE_SLAVE,
        &cryptAesCmacStep4Param);
    if (!ret) {
        ret = SMP_CryptographicAesCmacStep4(&cryptAesCmacStep4Param);
    }
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep16(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    int ret = SMP_EncryptCompleteJudgeException(returnParam->status, SMP_ROLE_SLAVE);
    if (ret != SMP_SUCCESS) {
        return;
    }
    (void)memcpy_s(
        SMP_GetPairMng()->local.DHKeyCheck, SMP_DHKEY_CHECK_LEN, returnParam->encryptedData, SMP_DHKEY_CHECK_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_17 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_17;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    ret = SMP_SendPairingDHKeyCheck(SMP_GetPairMng()->handle, SMP_GetPairMng()->local.DHKeyCheck, SMP_SendDataCallback);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send Pairing DHKey Check failed.");
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}

void SMP_ScPairCommonSlaveStep19(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(SMP_GetPairMng()->local.CSRK, SMP_CSRK_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_20 started.");
    int ret = SMP_SendHciLeRandCmd(SMP_SC_PAIR_COMMON_SLAVE_STEP_20);
    if (ret != SMP_SUCCESS) {
        SMP_GeneratePairResult(
            SMP_GetPairMng()->handle, SMP_PAIR_STATUS_FAILED, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL);
    }
}

void SMP_ScPairCommonSlaveStep20(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    const HciLeRandReturnParam *returnParam = (HciLeRandReturnParam *)param->data;

    LOG_DEBUG("%{public}s", __FUNCTION__);
    (void)memcpy_s(
        SMP_GetPairMng()->local.CSRK + SMP_RAND_NUM_LEN, SMP_RAND_NUM_LEN, returnParam->randomNumber, SMP_RAND_NUM_LEN);
    LOG_INFO("Send keys to remote.");
    LOG_DEBUG("SMP_SC_PAIR_COMMON_SLAVE_STEP_21 started.");
    SMP_GetPairMng()->step = SMP_SC_PAIR_COMMON_SLAVE_STEP_21;
    AlarmSet(SMP_GetPairMng()->alarm, SMP_PAIR_WAIT_TIME, SMP_PairTimeout, NULL);
    if (SMP_SendDistributionKeysToRemote()) {
        SMP_GeneratePairResult(SMP_GetPairMng()->handle,
            SMP_PAIR_STATUS_FAILED,
            SMP_PAIR_FAILED_UNSPECIFIED_REASION,
            SMP_GetPairMng()->alarm);
    }
}