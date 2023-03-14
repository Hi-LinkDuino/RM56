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

#ifndef KEY_AGREE_H
#define KEY_AGREE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t RequestAuthKeyAgree(int64_t requestId, const char *appId, const char *agreeParams);
int32_t RequestProcessKeyAgreeData(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen);

#ifdef __cplusplus
}
#endif
#endif
