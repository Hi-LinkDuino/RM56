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

#ifndef MBEDTLS_HASH_TO_POINT_H
#define MBEDTLS_HASH_TO_POINT_H

#include <stdint.h>
#include "string_util.h"

#define HASH2POINT_PARA_PREPRO 0xc0
#define BYTE_LENGTH_CURVE_25519 32

#ifdef __cplusplus
extern "C" {
#endif

int32_t MbedtlsHashToPoint(const Uint8Buff *hash, Uint8Buff *outEcPoint);

#ifdef __cplusplus
}
#endif
#endif