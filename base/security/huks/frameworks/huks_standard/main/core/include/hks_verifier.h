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

#ifndef HKS_VERIFIER_H
#define HKS_VERIFIER_H

#include "hks_type_inner.h"

#define HKS_DEFAULT_CERT_CHAIN_CNT 4
#define MAX_ATTEST_EXTENSION_BUF_LEN 10240 // The value needs to be confirmed
#define MAX_ATTEST_CLAIM_BUF_LEN 10240 // The value needs to be confirmed

enum HksCertFormat {
    HKS_CERT_PEM,
    HKS_CERT_DER,
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksClientValidateCertChain(const struct HksCertChain *certChain, struct HksParamSet *paramSetOut);

#ifdef __cplusplus
}
#endif

#endif
