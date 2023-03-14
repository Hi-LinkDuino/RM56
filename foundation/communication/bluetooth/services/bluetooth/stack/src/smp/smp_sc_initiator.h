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

#ifndef SMP_SC_INITIATOR_H
#define SMP_SC_INITIATOR_H

#include "smp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_1 0x0041
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_2 0x0042
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_3 0x0043
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_4 0x0044
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_5 0x0045
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_6 0x0046
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_7 0x0047
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_8 0x0048
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_9 0x0049
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_10 0x004A
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_11 0x004B
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_12 0x004C
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_13 0x004D
#define SMP_SC_PAIR_JUSTWORKORNUMERIC_MASTER_STEP_14 0x004E

#define SMP_SC_PAIR_COMMON_MASTER_STEP_1 0x0061
#define SMP_SC_PAIR_COMMON_MASTER_STEP_2 0x0062
#define SMP_SC_PAIR_COMMON_MASTER_STEP_3 0x0063
#define SMP_SC_PAIR_COMMON_MASTER_STEP_4 0x0064
#define SMP_SC_PAIR_COMMON_MASTER_STEP_5 0x0065
#define SMP_SC_PAIR_COMMON_MASTER_STEP_6 0x0066
#define SMP_SC_PAIR_COMMON_MASTER_STEP_7 0x0067
#define SMP_SC_PAIR_COMMON_MASTER_STEP_8 0x0068
#define SMP_SC_PAIR_COMMON_MASTER_STEP_9 0x0069
#define SMP_SC_PAIR_COMMON_MASTER_STEP_10 0x006A
#define SMP_SC_PAIR_COMMON_MASTER_STEP_11 0x006B
#define SMP_SC_PAIR_COMMON_MASTER_STEP_12 0x006C
#define SMP_SC_PAIR_COMMON_MASTER_STEP_13 0x006D
#define SMP_SC_PAIR_COMMON_MASTER_STEP_14 0x006E
#define SMP_SC_PAIR_COMMON_MASTER_STEP_15 0x006F
#define SMP_SC_PAIR_COMMON_MASTER_STEP_16 0x0070
#define SMP_SC_PAIR_COMMON_MASTER_STEP_17 0x0071
#define SMP_SC_PAIR_COMMON_MASTER_STEP_18 0x0072
#define SMP_SC_PAIR_COMMON_MASTER_STEP_19 0x0073
#define SMP_SC_PAIR_COMMON_MASTER_STEP_20 0x0074

#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_1 0x0081
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_2 0x0082
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_3 0x0083
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_4 0x0084
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_5 0x0085
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_6 0x0086
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_7 0x0087
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_8 0x0088
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_9 0x0089
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_10 0x008A
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_11 0x008B
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_12 0x008C
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_13 0x008D
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_14 0x008E
#define SMP_SC_PAIR_PASSKEYENTRY_MASTER_STEP_15 0x008F

#define SMP_SC_PAIR_OOB_MASTER_STEP_1 0x0101
#define SMP_SC_PAIR_OOB_MASTER_STEP_2 0x0102
#define SMP_SC_PAIR_OOB_MASTER_STEP_3 0x0103
#define SMP_SC_PAIR_OOB_MASTER_STEP_4 0x0104
#define SMP_SC_PAIR_OOB_MASTER_STEP_5 0x0105
#define SMP_SC_PAIR_OOB_MASTER_STEP_6 0x0106
#define SMP_SC_PAIR_OOB_MASTER_STEP_7 0x0107
#define SMP_SC_PAIR_OOB_MASTER_STEP_8 0x0108
#define SMP_SC_PAIR_OOB_MASTER_STEP_9 0x0109
#define SMP_SC_PAIR_OOB_MASTER_STEP_10 0x010A
#define SMP_SC_PAIR_OOB_MASTER_STEP_11 0x010B
#define SMP_SC_PAIR_OOB_MASTER_STEP_12 0x010C
#define SMP_SC_PAIR_OOB_MASTER_STEP_13 0x010D
#define SMP_SC_PAIR_OOB_MASTER_STEP_14 0x010E
#define SMP_SC_PAIR_OOB_MASTER_STEP_15 0x010F

void SMP_ScPairJustworkOrNumericMasterStep1(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep2(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep3(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep4(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep5(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep6(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep7(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep8(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep9(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep10(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep11(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep12(const SMP_StepParam *param);
void SMP_ScPairJustworkOrNumericMasterStep13(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep1(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep2(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep3(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep4(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep6(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep7(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep8(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep9(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep10(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep11(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep12(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep13(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep14(const SMP_StepParam *param);
void SMP_ScPairPasskeyEntryMasterStep15(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep1(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep8(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep9(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep10(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep11(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep12(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep13(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep14(const SMP_StepParam *param);
void SMP_ScPairOobMasterStep15(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep1(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep2(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep3(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep4(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep5(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep6(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep7(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep8(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep9(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep10(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep11(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep12(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep13(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep14(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep15(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep16(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep19(const SMP_StepParam *param);
void SMP_ScPairCommonMasterStep20(const SMP_StepParam *param);

#ifdef __cplusplus
}
#endif

#endif