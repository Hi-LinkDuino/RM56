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

#ifndef SMP_PRIVACY_H
#define SMP_PRIVACY_H

#include <stdint.h>

#include "btstack.h"
#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void SMP_GenerateRpaStep1(const SMP_StepParam *param);
void SMP_ResolveRpaStep1(const SMP_StepParam *param);
void SMP_SetLocalIrk(const uint8_t *irk, uint8_t size);
void SMP_SetLocalIdentAddr(const BtAddr *addr);
void SMP_GetLocalIrk(uint8_t *irk, uint8_t size);
void SMP_GetLocalIdentAddr(BtAddr *addr);

#ifdef __cplusplus
}
#endif

#endif