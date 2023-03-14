/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OPENSSL_HMAC_HELPER_H
#define OPENSSL_HMAC_HELPER_H

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT_NUM_OF_UINT8 8
#define HMAC_KEY_SIZE 512
#define HMAC_MESSAGE_SIZE 64
#define HMAC_FAILED 1
#define HMAC_SUCCESS 0
#define OUT_PARAMSET_SIZE 2048

int32_t HmacGenerateKey(int keyLen, struct HksBlob *key);

int32_t HmacHmac(const struct HksBlob *key, uint32_t digestAlg, const struct HksBlob *msg, struct HksBlob *mac);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CIPHER_H