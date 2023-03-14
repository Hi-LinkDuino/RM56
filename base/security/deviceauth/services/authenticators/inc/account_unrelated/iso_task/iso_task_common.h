/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ISO_TASK_COMMON_H
#define ISO_TASK_COMMON_H

#include "json_utils.h"
#include "iso_base_cur_task.h"

void DestroyIsoParams(IsoParams *params);
int InitIsoParams(IsoParams *params, const CJson *in);

int GenerateSeed(IsoParams *params);

int GenerateKeyAliasInIso(const IsoParams *params, uint8_t *keyAlias, uint32_t keyAliasLen, bool useOpposite);
int GeneratePsk(const CJson *in, IsoParams *params);

int GenerateEncResult(const IsoParams *params, int message, CJson *sendToPeer, const char *aad);
int GenEncResult(IsoParams *params, int message, CJson *out, const char *aad, bool isNeedReturnKey);
int SendResultToFinalSelf(IsoParams *params, CJson *out, bool isNeedReturnKey);
int CheckEncResult(IsoParams *params, const CJson *in, const char *aad);
void DeleteAuthCode(const IsoParams *params);

#endif
