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

#ifndef PAKE_MESSAGE_UTIL_H
#define PAKE_MESSAGE_UTIL_H

#include "json_utils.h"
#include "pake_base_cur_task.h"

int32_t PackagePakeRequestData(const PakeParams *params, CJson *payload);
int32_t ParsePakeRequestMessage(PakeParams *params, const CJson *in);
int32_t PackagePakeResponseData(const PakeParams *params, CJson *payload);
int32_t ParsePakeResponseMessage(PakeParams *params, const CJson *in);
int32_t PackagePakeClientConfirmData(const PakeParams *params, CJson *payload);
int32_t ParsePakeClientConfirmMessage(PakeParams *params, const CJson *in);
int32_t PackagePakeServerConfirmData(const PakeParams *params, CJson *payload);
int32_t ParsePakeServerConfirmMessage(PakeParams *params, const CJson *in);

#endif