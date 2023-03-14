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

#ifndef SMP_OOB_H
#define SMP_OOB_H

#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void SMP_GenerateScOobDataStep1(const SMP_StepParam *param);
void SMP_GenerateScOobDataStep2(const SMP_StepParam *param);
void SMP_GenerateScOobDataStep3(const SMP_StepParam *param);
void SMP_GenerateScOobDataStep4(const SMP_StepParam *param);
void SMP_GenerateScOobDataTimeout(void *parameter);
void SMP_ClearScOobData(bool isCancelTimer);
SMP_ScOobMng *SMP_GetScOobMng();

#ifdef __cplusplus
}
#endif

#endif