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

#ifndef HKS_RSA_COMMON_TEST_COMMON_H
#define HKS_RSA_COMMON_TEST_COMMON_H

#include <string>
#include "hks_test_three_stage_common.h"
namespace Unittest::RsaCipher {
static const uint32_t RSA_COMMON_SIZE = 1024;
static const std::string g_inData_5   = "RSA_5";
static const std::string g_inData_12  = "RSA_12_ttttt";
static const std::string g_inData_14  = "RSA_14_ttttttt";
static const std::string g_inData_20  = "RSA_20_ttttttttttttt";
static const std::string g_inData_32  = "RSA_32_ttttttttttttttttttttttttt";
static const std::string g_inData_64  = "RSA_64_ttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
static const std::string g_inData_96  = "RSA_96_ttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttt";
static const std::string g_inData_128 = "RSA_128_tttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
static const std::string g_inData_256 = "RSA_256_tttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
static const std::string g_inData_384 = "RSA_384_tttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
static const std::string g_inData_512 = "RSA_512_tttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt"
                                        "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";

int32_t HksRsaCipherTestCase(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet, const struct HksBlob *inData);
} // namespace Unittest::RsaCipher
#endif // HKS_RSA_COMMON_TEST_COMMON_H