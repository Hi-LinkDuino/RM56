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

#ifndef SMP_COMMON_H
#define SMP_COMMON_H

#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "smp.h"
#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void SMP_PairTimeout(void *parameter);
void SMP_GeneratePairResult(uint16_t handle, uint8_t status, uint8_t failedReason, Alarm *cancelTimer);
int SMP_EncryptCompleteJudgeException(uint8_t status, uint8_t role);
void SMP_GenerateSignatureStep1(const SMP_StepParam *param);
void SMP_GenerateSignatureStep2(const SMP_StepParam *param);
void SMP_GenerateSignatureStep3(const SMP_StepParam *param);
bool SMP_GetSecureConnOnlyMode();
SMP_PairMng *SMP_GetPairMng();
void SMP_NotifyCbAuthReq(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue);
void SMP_NotifyCbPairRet(uint16_t handle, uint8_t status, const SMP_PairResult *result);
void SMP_NotifyCbPairReq(uint16_t handle, const SMP_PairParam *param);
void SMP_NotifyCbPairRsp(uint16_t handle, const SMP_PairParam *param);
void SMP_NotifyCbSecReq(uint16_t handle, uint8_t authReq);
void SMP_NotifyCbLtkReq(uint16_t handle, const uint8_t *random, uint16_t ediv);
void SMP_NotifyCbEncComp(uint16_t handle, uint8_t status);
void SMP_NotifyCbGenSign(uint8_t status, const uint8_t *sign);
void SMP_NotifyCbGenRpa(uint8_t status, const uint8_t *addr);
void SMP_NotifyCbResoRpa(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk);
void SMP_NotifyCbGenScOobData(uint8_t status, const uint8_t *random, const uint8_t *confirm);

int SMP_ParamIsNULL(const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif