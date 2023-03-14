/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HKS_RESPONSE_H
#define HKS_RESPONSE_H

#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

void HksSendResponse(const uint8_t *context, int32_t result, const struct HksBlob *response);

int32_t HksGetProcessNameForIPC(const uint8_t *context, struct HksBlob *processName);

int32_t HksGetProcessInfoForIPC(const uint8_t *context, struct HksProcessInfo *processInfo);

int32_t Apl3Check();

#ifdef __cplusplus
}
#endif

#endif
