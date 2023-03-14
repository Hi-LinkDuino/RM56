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

#ifndef SMP_LEGACY_H
#define SMP_LEGACY_H

#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMP_PAIR_STEP_START 0x0001
#define SMP_LEGACY_PAIR_MASTER_STEP_1 0x0001
#define SMP_LEGACY_PAIR_MASTER_STEP_2 0x0002
#define SMP_LEGACY_PAIR_MASTER_STEP_3 0x0003
#define SMP_LEGACY_PAIR_MASTER_STEP_4 0x0004
#define SMP_LEGACY_PAIR_MASTER_STEP_5 0x0005
#define SMP_LEGACY_PAIR_MASTER_STEP_6 0x0006
#define SMP_LEGACY_PAIR_MASTER_STEP_7 0x0007
#define SMP_LEGACY_PAIR_MASTER_STEP_8 0x0008
#define SMP_LEGACY_PAIR_MASTER_STEP_9 0x0009
#define SMP_LEGACY_PAIR_MASTER_STEP_10 0x000A
#define SMP_LEGACY_PAIR_MASTER_STEP_11 0x000
#define SMP_LEGACY_PAIR_MASTER_STEP_12 0x000C
#define SMP_LEGACY_PAIR_MASTER_STEP_13 0x000D
#define SMP_LEGACY_PAIR_MASTER_STEP_14 0x000E
#define SMP_LEGACY_PAIR_MASTER_STEP_15 0x000F
#define SMP_LEGACY_PAIR_MASTER_STEP_16 0x0010
#define SMP_LEGACY_PAIR_MASTER_STEP_17 0x0011
#define SMP_LEGACY_PAIR_MASTER_STEP_18 0x0012

void SMP_LegacyPairMasterStep1(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep2(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep3(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep4(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep5(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep6(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep7(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep8(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep9(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep10(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep13(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep14(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep15(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep16(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep17(const SMP_StepParam *param);
void SMP_LegacyPairMasterStep18(const SMP_StepParam *param);

#define SMP_LEGACY_PAIR_SLAVE_STEP_1 0x0021
#define SMP_LEGACY_PAIR_SLAVE_STEP_2 0x0022
#define SMP_LEGACY_PAIR_SLAVE_STEP_3 0x0023
#define SMP_LEGACY_PAIR_SLAVE_STEP_4 0x0024
#define SMP_LEGACY_PAIR_SLAVE_STEP_5 0x0025
#define SMP_LEGACY_PAIR_SLAVE_STEP_6 0x0026
#define SMP_LEGACY_PAIR_SLAVE_STEP_7 0x0027
#define SMP_LEGACY_PAIR_SLAVE_STEP_8 0x0028
#define SMP_LEGACY_PAIR_SLAVE_STEP_9 0x0029
#define SMP_LEGACY_PAIR_SLAVE_STEP_10 0x002A
#define SMP_LEGACY_PAIR_SLAVE_STEP_11 0x002B
#define SMP_LEGACY_PAIR_SLAVE_STEP_12 0x002C
#define SMP_LEGACY_PAIR_SLAVE_STEP_13 0x002D
#define SMP_LEGACY_PAIR_SLAVE_STEP_14 0x002E
#define SMP_LEGACY_PAIR_SLAVE_STEP_15 0x002F
#define SMP_LEGACY_PAIR_SLAVE_STEP_16 0x0030
#define SMP_LEGACY_PAIR_SLAVE_STEP_17 0x0031
#define SMP_LEGACY_PAIR_SLAVE_STEP_18 0x0032
#define SMP_LEGACY_PAIR_SLAVE_STEP_19 0x0033
#define SMP_LEGACY_PAIR_SLAVE_STEP_20 0x0034

void SMP_LegacyPairSlaveStep1(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep2(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep3(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep4(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep5(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep6(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep7(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep8(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep9(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep10(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep11(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep14(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep15(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep16(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep17(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep18(const SMP_StepParam *param);
void SMP_LegacyPairSlaveStep19(const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif