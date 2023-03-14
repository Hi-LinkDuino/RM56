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

#ifndef SMP_HCI_EVENT_H
#define SMP_HCI_EVENT_H

#include "hci/hci.h"
#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void SMP_OnLERandComplete(const HciLeRandReturnParam *returnParam);
void SMP_OnLEEncryptComplete(const HciLeEncryptReturnParam *returnParam);
void SMP_OnLEEncryptionChange(const HciEncryptionChangeEventParam *eventParam);
void SMP_OnLEEncryptionKeyRefreshComplete(const HciEncryptionKeyRefreshCompleteEventParam *eventParam);
void SMP_OnLELongTermKeyRequest(const HciLeLongTermKeyRequestEventParam *eventParam);
void SMP_OnLELongTermKeyRequestReplyComplete(const HciLeLongTermKeyRequestReplyReturnParam *returnParam);
void SMP_OnLEReadLocalP256PublicKeyComplete(const HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam);
void SMP_OnLEGenerateDHKeyComplete(const HciLeGenerateDHKeyCompleteEventParam *eventParam);
void SMP_OnLeEncCompTask(void *context);
SMP_StepFunc GetPairTable(uint16_t step);
void SMP_ExecuteStepFunc(uint16_t step, const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif