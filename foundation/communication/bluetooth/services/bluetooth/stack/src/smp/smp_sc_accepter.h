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

#ifndef SMP_SC_ACCEPTER_H
#define SMP_SC_ACCEPTER_H

#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_1 0x00A1
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_2 0x00A2
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_3 0x00A3
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_4 0x00A4
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_5 0x00A5
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_6 0x00A6
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_7 0x00A7
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_8 0x00A8
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_9 0x00A9
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_10 0x00AA
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_11 0x00AB
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_12 0x00AC
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_13 0x00AD
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_SLAVE_STEP_14 0x00AE
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_1 0x00C1
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_2 0x00C2
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_3 0x00C3
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_4 0x00C4
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_5 0x00C5
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_6 0x00C6
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_7 0x00C7
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_8 0x00C8
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_9 0x00C9
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_10 0x00CA
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_11 0x00CB
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_12 0x00CC
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_13 0x00CD
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_14 0x00CE
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_15 0x00CF
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_16 0x00D0
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_17 0x00D1
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_18 0x00D2
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_19 0x00D3
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_20 0x00D4
#define SMP_SC_PAIR_COMMON_SLAVE_STEP_21 0x00D5

#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_1 0x00E1
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_2 0x00E2
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_3 0x00E3
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_4 0x00E4
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_5 0x00E5
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_6 0x00E6
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_7 0x00E7
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_8 0x00E8
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_9 0x00E9
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_10 0x00EA
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_11 0x00EB
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_12 0x00EC
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_13 0x00ED
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_14 0x00EE
#define SMP_SC_PAIR_PASSKEYENTRY_SLAVE_STEP_15 0x00EF

#define SMP_SC_PAIR_OOB_SLAVE_STEP_1 0x0121
#define SMP_SC_PAIR_OOB_SLAVE_STEP_2 0x0122
#define SMP_SC_PAIR_OOB_SLAVE_STEP_3 0x0123
#define SMP_SC_PAIR_OOB_SLAVE_STEP_4 0x0124
#define SMP_SC_PAIR_OOB_SLAVE_STEP_5 0x0125
#define SMP_SC_PAIR_OOB_SLAVE_STEP_6 0x0126
#define SMP_SC_PAIR_OOB_SLAVE_STEP_7 0x0127
#define SMP_SC_PAIR_OOB_SLAVE_STEP_8 0x0128
#define SMP_SC_PAIR_OOB_SLAVE_STEP_9 0x0129
#define SMP_SC_PAIR_OOB_SLAVE_STEP_10 0x012A
#define SMP_SC_PAIR_OOB_SLAVE_STEP_11 0x012B
#define SMP_SC_PAIR_OOB_SLAVE_STEP_12 0x012C
#define SMP_SC_PAIR_OOB_SLAVE_STEP_13 0x012D
#define SMP_SC_PAIR_OOB_SLAVE_STEP_14 0x012E
#define SMP_SC_PAIR_OOB_SLAVE_STEP_15 0x012F
#define SMP_PAIR_STEP_END 0x012F

void SMP_ScPairJustworkOrNumericSlaveStep1(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep2(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep3(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep4(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep5(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep6(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep7(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep8(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep9(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep10(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep11(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep12(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericSlaveStep13(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep1(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep2(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep3(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep4(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep5(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep6(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep7(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep8(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep9(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep10(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep11(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep12(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep13(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep14(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntrySlaveStep15(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep1(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep7(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep8(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep9(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep10(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep11(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep12(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep13(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep14(const SMP_StepParam *param);
void SMP_ScPairOobSlaveStep15(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep1(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep2(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep3(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep4(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep5(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep6(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep7(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep8(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep9(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep10(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep11(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep12(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep13(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep14(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep15(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep16(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep19(const SMP_StepParam *param);
void SMP_ScPairCommonSlaveStep20(const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif