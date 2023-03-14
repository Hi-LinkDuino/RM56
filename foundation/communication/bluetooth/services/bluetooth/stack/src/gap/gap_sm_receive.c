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

#include "gap_internal.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "smp/smp.h"

typedef struct {
    uint16_t handle;
    uint8_t pairMethod;
    uint8_t *displayValue;
} GapLeAuthenticationRequestParam;

typedef struct {
    uint16_t handle;
    uint8_t status;
    SMP_PairResult result;
} GapLePairResultParam;

typedef struct {
    uint16_t handle;
    SMP_PairParam param;
} GapLeRemotePairRequestParam;

typedef struct {
    uint16_t handle;
    SMP_PairParam param;
} GapLeRemotePairResponseParam;

typedef struct {
    uint16_t handle;
    uint8_t authReq;
} GapLeRemoteSecurityRequestParam;

typedef struct {
    uint16_t handle;
    uint64_t random;
    uint16_t ediv;
} GapLeLongTermKeyRequestParam;

typedef struct {
    uint8_t status;
    uint8_t *sign;
} GapLeGenerateSignatureResultParam;

typedef struct {
    uint8_t status;
    uint8_t *addr;
} GapGenerateRPAResultParam;

typedef struct {
    uint8_t status;
    bool result;
    uint8_t addr[BT_ADDRESS_SIZE];
    uint8_t irk[GAP_IRK_SIZE];
} GapResolveRPAResultParam;

static void GapLeAuthenticationRequestTask(void *ctx)
{
    GapLeAuthenticationRequestParam *param = ctx;
    GapLeAuthenticationRequest(param->handle, param->pairMethod, param->displayValue);
}

static void GapFreeLeAuthenticationRequest(void *ctx)
{
    GapLeAuthenticationRequestParam *param = ctx;

    if (param->displayValue != NULL) {
        MEM_MALLOC.free(param->displayValue);
    }
}

static uint8_t *GapAllocLeAuthenticationRequestValue(uint8_t pairMethod, const uint8_t *displayValue)
{
    uint8_t valueLength;
    uint8_t *value = NULL;

    switch (pairMethod) {
        case SMP_PAIR_METHOD_PASSKEY_DISPLAY:
        case SMP_PAIR_METHOD_NUMERIC_COMPARISON:
            valueLength = sizeof(uint32_t);
            break;
        case SMP_PAIR_METHOD_OOB_LEGACY:
            valueLength = GAP_OOB_DATA_SIZE;
            break;
        case SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV:
        case SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV:
        case SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND:
            valueLength = BT_ADDRESS_SIZE + GAP_OOB_DATA_CONFIRM_SIZE + GAP_OOB_DATA_RANDOM_SIZE;
            break;
        case SMP_PAIR_METHOD_JUST_WORK:
        case SMP_PAIR_METHOD_PASSKEY_ENTRY:
        default:
            return NULL;
    }

    value = MEM_MALLOC.alloc(valueLength);
    if (value != NULL) {
        (void)memcpy_s(value, valueLength, displayValue, valueLength);
    }

    return value;
}

static void GapRecvLeAuthenticationRequest(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue)
{
    LOG_INFO("%{public}s: handle:0x%04x pairMethod:%hhu", __FUNCTION__, handle, pairMethod);
    GapLeAuthenticationRequestParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeAuthenticationRequestParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->pairMethod = pairMethod;
    smParam->displayValue = GapAllocLeAuthenticationRequestValue(pairMethod, displayValue);
    if (smParam->displayValue == NULL && displayValue != NULL) {
        MEM_MALLOC.free(smParam);
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    int ret = GapRunTaskUnBlockProcess(GapLeAuthenticationRequestTask, smParam, GapFreeLeAuthenticationRequest);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLePairResultTask(void *ctx)
{
    GapLePairResultParam *param = ctx;
    GapLePairResult(param->handle, param->status, &param->result);
}

static void GapRecvLePairResult(uint16_t handle, uint8_t status, const SMP_PairResult *result)
{
    LOG_INFO("%{public}s: handle:0x%04x status:%hhu", __FUNCTION__, handle, status);
    if (result != NULL) {
        LOG_INFO("pairType:%hhu bondedFlag:%hhu authFlag:0x%02x encKeySize:%hhu localKeyDist:0x%02x peerKeyDist:0x%02x",
            result->pairType,
            result->bondedFlag,
            result->authFlag,
            result->encKeySize,
            result->localKeyDist,
            result->peerKeyDist);
    } else {
        return;
    }

    GapLePairResultParam *smParam = MEM_MALLOC.alloc(sizeof(GapLePairResultParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->status = status;
    smParam->result = *result;

    int ret = GapRunTaskUnBlockProcess(GapLePairResultTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeRemotePairRequestTask(void *ctx)
{
    GapLeRemotePairRequestParam *param = ctx;
    GapLeRemotePairRequest(param->handle, &param->param);
}

static void GapRecvLeRemotePairRequest(uint16_t handle, const SMP_PairParam *param)
{
    LOG_INFO("%{public}s: handle:0x%04x ioCapability:%hhu authReq:0x%02x initKeyDist:0x%02x respKeyDist:0x%02x",
        __FUNCTION__,
        handle,
        param->ioCapability,
        param->authReq,
        param->initKeyDist,
        param->respKeyDist);
    GapLeRemotePairRequestParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeRemotePairRequestParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->param = *param;

    int ret = GapRunTaskUnBlockProcess(GapLeRemotePairRequestTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeRemotePairResponseTask(void *ctx)
{
    GapLeRemotePairRequestParam *param = ctx;
    GapLeRemotePairResponse(param->handle, &param->param);
}

static void GapRecvLeRemotePairResponse(uint16_t handle, const SMP_PairParam *param)
{
    LOG_INFO("%{public}s: handle:0x%04x ioCapability:%hhu authReq:0x%02x initKeyDist:0x%02x respKeyDist:0x%02x",
        __FUNCTION__,
        handle,
        param->ioCapability,
        param->authReq,
        param->initKeyDist,
        param->respKeyDist);
    GapLeRemotePairResponseParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeRemotePairResponseParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->param = *param;

    int ret = GapRunTaskUnBlockProcess(GapLeRemotePairResponseTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeRemoteSecurityRequestTask(void *ctx)
{
    GapLeRemoteSecurityRequestParam *param = ctx;
    GapLeRemoteSecurityRequest(param->handle, param->authReq);
}

static void GapRecvLeRemoteSecurityRequest(uint16_t handle, uint8_t authReq)
{
    LOG_INFO("%{public}s: handle:0x%04x authReq:0x%02x", __FUNCTION__, handle, authReq);
    GapLeRemoteSecurityRequestParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeRemoteSecurityRequestParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->authReq = authReq;

    int ret = GapRunTaskUnBlockProcess(GapLeRemoteSecurityRequestTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeLongTermKeyRequestTask(void *ctx)
{
    GapLeLongTermKeyRequestParam *param = ctx;
    GapLeLongTermKeyRequest(param->handle, (uint8_t *)&param->random, param->ediv);
}

static void GapRecvLeLongTermKeyRequest(uint16_t handle, const uint8_t *random, uint16_t ediv)
{
    LOG_INFO("%{public}s: handle:0x%04x", __FUNCTION__, handle);
    GapLeLongTermKeyRequestParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeLongTermKeyRequestParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->handle = handle;
    smParam->random = *(uint64_t *)random;
    smParam->ediv = ediv;

    int ret = GapRunTaskUnBlockProcess(GapLeLongTermKeyRequestTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeGenerateSignatureResultTask(void *ctx)
{
    GapLeGenerateSignatureResultParam *param = ctx;
    GapLeGenerateSignatureResult(param->status, param->sign);
}

static void GapFreeLeGenerateSignatureResult(void *ctx)
{
    GapLeGenerateSignatureResultParam *param = ctx;

    MEM_MALLOC.free(param->sign);
}

static void GapRecvLeGenerateSignatureResult(uint8_t status, const uint8_t *sign)
{
    LOG_INFO("%{public}s: status:%hhu", __FUNCTION__, status);
    GapLeGenerateSignatureResultParam *smParam = MEM_MALLOC.alloc(sizeof(GapLeGenerateSignatureResultParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->status = status;
    if (status == SMP_GENERATE_SIGN_STATUS_SUCCESS) {
        smParam->sign = MEM_MALLOC.alloc(GAP_SIGNATURE_SIZE);
        if (smParam->sign == NULL) {
            MEM_MALLOC.free(smParam);
            LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
            return;
        }

        (void)memcpy_s(smParam->sign, GAP_SIGNATURE_SIZE, sign, GAP_SIGNATURE_SIZE);
    } else {
        smParam->sign = NULL;
    }

    int ret = GapRunTaskUnBlockProcess(GapLeGenerateSignatureResultTask, smParam, GapFreeLeGenerateSignatureResult);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapGenerateRPAResultTask(void *ctx)
{
    GapGenerateRPAResultParam *param = ctx;
    GapGenerateRPAResult(param->status, param->addr);
}

static void GapFreeGenerateRPAResult(void *ctx)
{
    GapGenerateRPAResultParam *param = ctx;

    MEM_MALLOC.free(param->addr);
}

static void GapRecvGenerateRPAResult(uint8_t status, const uint8_t *addr)
{
    LOG_INFO("%{public}s: status:%hhu", __FUNCTION__, status);
    GapGenerateRPAResultParam *smParam = MEM_MALLOC.alloc(sizeof(GapGenerateRPAResultParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->status = status;
    if (status == SMP_GENERATE_RPA_STATUS_SUCCESS) {
        smParam->addr = MEM_MALLOC.alloc(BT_ADDRESS_SIZE);
        if (smParam->addr == NULL) {
            MEM_MALLOC.free(smParam);
            LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
            return;
        }

        (void)memcpy_s(smParam->addr, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
    } else {
        smParam->addr = NULL;
    }

    int ret = GapRunTaskUnBlockProcess(GapGenerateRPAResultTask, smParam, GapFreeGenerateRPAResult);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapResolveRPAResultTask(void *ctx)
{
    GapResolveRPAResultParam *param = ctx;
    GapResolveRPAResult(param->status, param->result, param->addr, param->irk);
}

static void GapRecvResolveRPAResult(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk)
{
    LOG_INFO("%{public}s: status:%hhu result:%{public}d", __FUNCTION__, status, result);
    GapResolveRPAResultParam *smParam = MEM_MALLOC.alloc(sizeof(GapResolveRPAResultParam));
    if (smParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    smParam->status = status;
    smParam->result = result;
    (void)memcpy_s(smParam->addr, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
    (void)memcpy_s(smParam->irk, GAP_IRK_SIZE, irk, GAP_IRK_SIZE);

    int ret = GapRunTaskUnBlockProcess(GapResolveRPAResultTask, smParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static SMP_Callback_t g_smCallback = {
    .SMP_CallbackAuthenticationRequest = GapRecvLeAuthenticationRequest,
    .SMP_CallbackPairResult = GapRecvLePairResult,
    .SMP_CallbackRemotePairRequest = GapRecvLeRemotePairRequest,
    .SMP_CallbackRemotePairResponse = GapRecvLeRemotePairResponse,
    .SMP_CallbackRemoteSecurityRequest = GapRecvLeRemoteSecurityRequest,
    .SMP_CallbackLongTermKeyRequest = GapRecvLeLongTermKeyRequest,
    .SMP_CallbackGenerateSignatureResult = GapRecvLeGenerateSignatureResult,
    .SMP_CallbackGenerateRPAResult = GapRecvGenerateRPAResult,
    .SMP_CallbackResolveRPAResult = GapRecvResolveRPAResult,
};

void GapRegisterSmCallbacks(void)
{
    SMP_RegisterCallback(&g_smCallback);
}

void GapDeregisterSmCallbacks(void)
{
    SMP_UnregisterCallback();
}