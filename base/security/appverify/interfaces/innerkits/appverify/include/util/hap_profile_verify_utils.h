/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef HAP_PROFILE_VERIFY_UTILS_H
#define HAP_PROFILE_VERIFY_UTILS_H

#include <string>
#include <vector>

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "util/hap_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
class HapProfileVerifyUtils {
public:
    DLL_EXPORT static bool ParseProfile(Pkcs7Context& profilePkcs7Context, const Pkcs7Context& hapPkcs7Context,
        const HapByteBuffer& pkcs7ProfileBlock, std::string& profile);
    DLL_EXPORT static bool VerifyProfile(Pkcs7Context& pkcs7Context);
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_PROFILE_VERIFY_UTILS_H
