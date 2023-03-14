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

#include "smp_privacy.h"

#include <string.h>

#include "log.h"
#include "smp.h"
#include "smp_cmd.h"
#include "smp_common.h"
#include "smp_def.h"
#include "smp_tool.h"

static uint8_t g_smpLocalIRK[SMP_IRK_LEN] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
static BtAddr g_smpLocalIdentAddr = {
    .type = 0x01,
    .addr = {0xFF, 0x01, 0x02, 0x03, 0x04, 0x05},
};

static void SMP_ResoRpaStep1Exception(const SMP_EncCmd *pEncCmdData);
static void SMP_ResoRpaStep1Failed(const SMP_EncCmd *pEncCmdData);
static void SMP_ResoRpaStep1Success(const SMP_EncCmd *pEncCmdData);

void SMP_GenerateRpaStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_EncCmd *pEncCmdData = encData->encCmd;
    uint8_t address[BT_ADDRESS_SIZE] = {0x00};
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;

    LOG_DEBUG("%{public}s.", __FUNCTION__);

    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu", returnParam->status);
        SMP_NotifyCbGenRpa(SMP_GENERATE_RPA_STATUS_FAILED, address);
        return;
    }

    SMP_MemoryReverseCopy(
        pEncCmdData->address + SMP_RPA_HIGH_BIT_LEN, returnParam->encryptedData, SMP_RPA_HIGH_BIT_LEN);
    SMP_MemoryReverseCopy(address, pEncCmdData->address, sizeof(address));
    SMP_NotifyCbGenRpa(SMP_GENERATE_RPA_STATUS_SUCCESS, address);
}

void SMP_ResolveRpaStep1(const SMP_StepParam *param)
{
    if (SMP_ParamIsNULL(param) != SMP_SUCCESS) {
        return;
    }
    SMP_EncData *encData = (SMP_EncData *)param->data;
    SMP_EncCmd *pEncCmdData = encData->encCmd;
    const HciLeEncryptReturnParam *returnParam = encData->encRetParam;

    LOG_DEBUG("%{public}s", __FUNCTION__);

    if (returnParam->status) {
        LOG_ERROR("returnParam->status = %hhu.", returnParam->status);
        SMP_ResoRpaStep1Exception(pEncCmdData);
        return;
    }

    if (memcmp(returnParam->encryptedData, pEncCmdData->address, BT_ADDRESS_SIZE - SMP_RPA_HIGH_BIT_LEN) != 0x00) {
        SMP_ResoRpaStep1Failed(pEncCmdData);
    } else {
        SMP_ResoRpaStep1Success(pEncCmdData);
    }
}

static void SMP_ResoRpaStep1Exception(const SMP_EncCmd *pEncCmdData)
{
    SMP_NotifyCbResoRpa(SMP_RESOLVE_RPA_STATUS_FAILED, false, pEncCmdData->address, pEncCmdData->key);
}

static void SMP_ResoRpaStep1Failed(const SMP_EncCmd *pEncCmdData)
{
    LOG_INFO("Resolve RPA failed.");
    SMP_NotifyCbResoRpa(SMP_RESOLVE_RPA_STATUS_SUCCESS, false, pEncCmdData->address, pEncCmdData->key);
}

static void SMP_ResoRpaStep1Success(const SMP_EncCmd *pEncCmdData)
{
    LOG_INFO("Resolve RPA successfully.");
    SMP_NotifyCbResoRpa(SMP_RESOLVE_RPA_STATUS_SUCCESS, true, pEncCmdData->address, pEncCmdData->key);
}

void SMP_SetLocalIrk(const uint8_t *irk, uint8_t size)
{
    (void)memcpy_s(g_smpLocalIRK, SMP_IRK_LEN, irk, size);
}

void SMP_SetLocalIdentAddr(const BtAddr *addr)
{
    g_smpLocalIdentAddr.type = addr->type;
    SMP_MemoryReverseCopy(g_smpLocalIdentAddr.addr, addr->addr, sizeof(g_smpLocalIdentAddr.addr));
}

void SMP_GetLocalIrk(uint8_t *irk, uint8_t size)
{
    (void)memcpy_s(irk, SMP_IRK_LEN, g_smpLocalIRK, size);
}

void SMP_GetLocalIdentAddr(BtAddr *addr)
{
    (void)memcpy_s(addr, SMP_IRK_LEN, &g_smpLocalIdentAddr, sizeof(BtAddr));
}