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

#ifndef HKS_DSA_SIGN_VERIFY_TEST_H
#define HKS_DSA_SIGN_VERIFY_TEST_H

#include <string>
#include "hks_test_common.h"
namespace Unittest::DsaSignVerify {
static const uint32_t DSA_COMMON_SIZE = 1024;
static const std::string g_inData = "Hks_DSA_Sign_Verify_Test_0000000000000000000000000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000"
                                    "0000000000000000000000000000000000000000000000000000000000000000000000000_string";
}
#endif // HKS_DSA_SIGN_VERIFY_TEST_H