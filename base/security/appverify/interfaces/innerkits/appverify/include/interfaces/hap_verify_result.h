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
#ifndef HAP_VERIFY_RESULT_H
#define HAP_VERIFY_RESULT_H

#include <string>
#include <vector>

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "provision/provision_info.h"

namespace OHOS {
namespace Security {
namespace Verify {
enum HapVerifyResultCode {
    VERIFY_SUCCESS = 0,
    FILE_PATH_INVALID = -1,
    OPEN_FILE_ERROR = -2,
    SIGNATURE_NOT_FOUND = -3,
    VERIFY_APP_PKCS7_FAIL = -4,
    PROFILE_PARSE_FAIL = -5,
    APP_SOURCE_NOT_TRUSTED = -6,
    GET_DIGEST_FAIL = -7,
    VERIFY_INTEGRITY_FAIL = -8,
    FILE_SIZE_TOO_LARGE = -9,
    GET_PUBLICKEY_FAIL = -10,
    GET_SIGNATURE_FAIL = -11,
    NO_PROFILE_BLOCK_FAIL = -12,
    VERIFY_SIGNATURE_FAIL = -13,
    VERIFY_SOURCE_INIT_FAIL = -14,
};

enum GetOptionalBlockResultCode {
    GET_SUCCESS = 0,
    NO_THIS_BLOCK_IN_PACKAGE = 1,
};

struct OptionalBlock {
    int optionalType;
    HapByteBuffer optionalBlockValue;
};

class HapVerifyResult {
public:
    DLL_EXPORT HapVerifyResult();
    DLL_EXPORT ~HapVerifyResult();
    DLL_EXPORT int GetVersion() const;
    DLL_EXPORT void SetVersion(int signatureVersion);
    DLL_EXPORT void SetPkcs7SignBlock(const HapByteBuffer& pkcs7);
    DLL_EXPORT void SetPkcs7ProfileBlock(const HapByteBuffer& pkcs7);
    DLL_EXPORT void SetOptionalBlocks(const std::vector<OptionalBlock>& option);
    DLL_EXPORT void SetProvisionInfo(const ProvisionInfo& info);
    DLL_EXPORT int GetProperty(std::string& property) const;
    DLL_EXPORT ProvisionInfo GetProvisionInfo() const;
    DLL_EXPORT std::vector<std::string> GetPublicKey() const;
    DLL_EXPORT std::vector<std::string> GetSignature() const;
    void SetPublicKey(const std::vector<std::string>& inputPubkeys);
    void SetSignature(const std::vector<std::string>& inputSignatures);

private:
    DLL_EXPORT int GetBlockFromOptionalBlocks(int blockType, std::string& block) const;

private:
    int version;
    std::vector<std::string> publicKeys;
    std::vector<std::string> signatures;
    HapByteBuffer pkcs7SignBlock;
    HapByteBuffer pkcs7ProfileBlock;
    std::vector<OptionalBlock> optionalBlocks;
    ProvisionInfo provisionInfo;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_VERIFY_RESULT_H
