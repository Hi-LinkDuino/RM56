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
#ifndef HAP_VERIFY_HAP_V2_H
#define HAP_VERIFY_HAP_V2_H

#include <string>

#include "common/hap_byte_buffer.h"
#include "common/random_access_file.h"
#include "interfaces/hap_verify_result.h"
#include "provision/provision_verify.h"
#include "util/hap_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
class HapVerifyV2 {
public:
    int Verify(const std::string& filePath, HapVerifyResult& hapVerifyV1Result);

private:
    int Verify(RandomAccessFile& hapFile, HapVerifyResult& hapVerifyV1Result);
    bool VerifyAppPkc7AndParseProfile(Pkcs7Context& pkcs7Context, HapByteBuffer& hapProfileBlock,
        HapVerifyResult& hapVerifyV1Result);
    bool VerifyAppSourceAndParseProfile(Pkcs7Context& pkcs7Context, const HapByteBuffer& hapProfileBlock,
        HapVerifyResult& hapVerifyV1Result, bool& profileNeadWriteCrl);
    bool VerifyAppPkcs7(Pkcs7Context& pkcs7Context, const HapByteBuffer& hapSignatureBlock);
    bool VerifyAppSource(Pkcs7Context& pkcs7Context, Pkcs7Context& profileContext);
    DLL_EXPORT bool GetDigestAndAlgorithm(Pkcs7Context& digest);
    DLL_EXPORT bool CheckFilePath(const std::string& filePath, std::string& standardFilePath);
    void WriteCrlIfNeed(const Pkcs7Context& pkcs7Context, const bool& profileNeedWriteCrl);
    DLL_EXPORT bool ParseAndVerifyProfileIfNeed(const std::string& profile, ProvisionInfo& provisionInfo,
        bool isCallParseAndVerify);
    bool IsAppDistributedTypeAllowInstall(const AppDistType& type, const ProvisionInfo& provisionInfo) const;
    DLL_EXPORT bool VerifyProfileInfo(const Pkcs7Context& pkcs7Context, const Pkcs7Context& profileContext,
        ProvisionInfo& provisionInfo);
    bool CheckProfileSignatureIsRight(const MatchingStates& matchState, const ProvisionType& type);
    DLL_EXPORT bool GenerateAppId(ProvisionInfo& provisionInfo);
    bool VerifyProfileSignature(const Pkcs7Context& pkcs7Context, Pkcs7Context& profileContext);

private:
    static const int HEX_PRINT_LENGTH;
    static const int DIGEST_BLOCK_LEN_OFFSET;
    static const int DIGEST_ALGORITHM_OFFSET;
    static const int DIGEST_LEN_OFFSET;
    static const int DIGEST_OFFSET_IN_CONTENT;
    static const std::string HAP_APP_PATTERN;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_VERIFY_HAP_V2_H
