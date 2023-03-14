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

#ifndef SMP_SEND_H
#define SMP_SEND_H

#include <stdint.h>

#include "hci/hci.h"
#include "platform/include/list.h"
#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

int SMP_SendDistributionKeysToRemote();
int SMP_SendHciLeRandCmd(uint16_t step);
int SMP_SendLeEncryptCmd(
    const HciLeEncryptParam *pEncryptParam, uint16_t step, const SMP_EncCmd *pEncCmdData, bool isUsingHw);
void SMP_SendDataCallback(uint16_t aclHandle, int result);
void SMP_SendPairingResponseCallback(uint16_t aclHandle, int result);
void SMP_SendPairingFailedCallback(uint16_t aclHandle, int result);
void SMP_SendSecurityRequestCallback(uint16_t aclHandle, int result);
void SMP_SendEncryptionInformationCallback(uint16_t aclHandle, int result);
void SMP_SendMasterIdentificationCallback(uint16_t aclHandle, int result);
void SMP_SendIdentityInformationCallback(uint16_t aclHandle, int result);
void SMP_SendIdentityAddressInformationCallback(uint16_t aclHandle, int result);
void SMP_SendSigningInformationCallback(uint16_t aclHandle, int result);
void SMP_FreeEncCmd(void *encCmd);
SMP_EncCmd *SMP_AllocEncCmd();
void SMP_EncCmdTimeout(void *param);
List *SMP_GetEncCmdList();
void SMP_SetEncCmdList(List *list);

#ifdef __cplusplus
}
#endif

#endif