/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_CLIENT_SERVICE_ADAPTER_H
#define HKS_CLIENT_SERVICE_ADAPTER_H

#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t TranslateToX509PublicKey(const struct HksBlob *publicKey, struct HksBlob *x509Key);

int32_t TranslateFromX509PublicKey(const struct HksBlob *x509Key, struct HksBlob *publicKey);

int32_t TranslateToInnerCurve25519Format(const uint32_t alg, const struct HksBlob *key, struct HksBlob *publicKey);

int32_t TranslateToInnerAesFormat(const struct HksBlob *key, struct HksBlob *outKey);

#ifdef __cplusplus
}
#endif

#endif /* HKS_CLIENT_SERVICE_ADAPTER_H */