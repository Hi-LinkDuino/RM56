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

#ifndef HKS_AUTH_H
#define HKS_AUTH_H

#include "hks_keyblob.h"
#include "hks_type_inner.h"

#include "hks_keynode.h"

enum HksAuthId {
    HKS_AUTH_ID_SIGN_VERIFY_RSA = 0x1,
    HKS_AUTH_ID_SIGN_VERIFY_ECC = 0x2,
    HKS_AUTH_ID_SYM_CIPHER = 0x3,
    HKS_AUTH_ID_ASYM_CIPHER = 0x4,
    HKS_AUTH_ID_MAC = 0x5,
    HKS_AUTH_ID_DERIVE = 0x6,
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksAuth(uint32_t authId, const struct HksKeyNode *keyNode, const struct HksParamSet *paramSet);

int32_t HksThreeStageAuth(uint32_t authId, const struct HuksKeyNode *keyNode);

#ifdef __cplusplus
}
#endif

#endif /* HKS_AUTH_H */

