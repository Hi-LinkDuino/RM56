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

#ifndef HKS_ECDH_AGREE_TEST_COMMON_H
#define HKS_ECDH_AGREE_TEST_COMMON_H

#include <string>
#include "hks_test_common.h"
namespace Unittest::EcdhAgree {
static const std::string g_inData = "Hks_ECDH_Agree_Test_000000000000000000000000000000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000"
                                    "0000000000000000000000000000000000000000000000000000000000000000000000000_string";
static const uint32_t ECDH_COMMON_SIZE = 1024;

int32_t HksEcdhAgreeFinish(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet, const struct HksParamSet *finishParamSet, struct HksBlob *outData);
int32_t HksEcdhAgreeAbort(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet);
int32_t HksEcdhAgreeExport(const struct HksBlob *keyAlias1, const struct HksBlob *keyAlias2,
    struct HksBlob *publicKey1, struct HksBlob *publicKey2, const struct HksParamSet *genParamSet);
void HksEcdhAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3);
void HksEcdhAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3, struct HksParamSet *paramSet4, struct HksParamSet *paramSet5);
void HksEcdhAgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2);
void HksEcdhAgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2,
    struct HksBlob *blob3, struct HksBlob *blob4);
} // namespace Unittest::EcdhAgree
#endif // HKS_ECDH_AGREE_TEST_COMMON_H