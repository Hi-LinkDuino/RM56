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

#include "verify/hap_verify_v2.h"

#include <climits>
#include <cstdlib>
#include <regex>

#include "securec.h"

#include "common/hap_verify_log.h"
#include "init/hap_crl_manager.h"
#include "init/trusted_source_manager.h"
#include "ticket/ticket_verify.h"
#include "util/hap_profile_verify_utils.h"
#include "util/hap_signing_block_utils.h"
#include "util/signature_info.h"

namespace OHOS {
namespace Security {
namespace Verify {
const int HapVerifyV2::HEX_PRINT_LENGTH = 3;
const int HapVerifyV2::DIGEST_BLOCK_LEN_OFFSET = 8;
const int HapVerifyV2::DIGEST_ALGORITHM_OFFSET = 12;
const int HapVerifyV2::DIGEST_LEN_OFFSET = 16;
const int HapVerifyV2::DIGEST_OFFSET_IN_CONTENT = 20;
const std::string HapVerifyV2::HAP_APP_PATTERN = "[^]*.hap$";

int HapVerifyV2::Verify(const std::string& filePath, HapVerifyResult& hapVerifyV1Result)
{
    HAPVERIFY_LOG_DEBUG(LABEL, "Start Verify");
    std::string standardFilePath;
    if (!CheckFilePath(filePath, standardFilePath)) {
        return FILE_PATH_INVALID;
    }

    RandomAccessFile hapFile;
    if (!hapFile.Init(standardFilePath)) {
        HAPVERIFY_LOG_ERROR(LABEL, "open standard file failed");
        return OPEN_FILE_ERROR;
    }

    int resultCode = Verify(hapFile, hapVerifyV1Result);
    return resultCode;
}

bool HapVerifyV2::CheckFilePath(const std::string& filePath, std::string& standardFilePath)
{
    char path[PATH_MAX + 1] = { 0x00 };
    if (filePath.size() > PATH_MAX || realpath(filePath.c_str(), path) == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "filePath is not a standard path");
        return false;
    }
    standardFilePath = std::string(path);
    if (!std::regex_match(standardFilePath, std::regex(HAP_APP_PATTERN))) {
        HAPVERIFY_LOG_ERROR(LABEL, "file is not hap package");
        return false;
    }
    return true;
}

int HapVerifyV2::Verify(RandomAccessFile& hapFile, HapVerifyResult& hapVerifyV1Result)
{
    SignatureInfo hapSignInfo;
    if (!HapSigningBlockUtils::FindHapSignature(hapFile, hapSignInfo)) {
        return SIGNATURE_NOT_FOUND;
    }
    hapVerifyV1Result.SetVersion(hapSignInfo.version);
    hapVerifyV1Result.SetPkcs7SignBlock(hapSignInfo.hapSignatureBlock);
    hapVerifyV1Result.SetPkcs7ProfileBlock(hapSignInfo.hapSignatureBlock);
    hapVerifyV1Result.SetOptionalBlocks(hapSignInfo.optionBlocks);
    Pkcs7Context pkcs7Context;
    if (!VerifyAppPkcs7(pkcs7Context, hapSignInfo.hapSignatureBlock)) {
        return VERIFY_APP_PKCS7_FAIL;
    }
    int profileIndex = 0;
    if (!HapSigningBlockUtils::GetOptionalBlockIndex(hapSignInfo.optionBlocks, PROFILE_BLOB, profileIndex)) {
        return NO_PROFILE_BLOCK_FAIL;
    }
    bool profileNeedWriteCrl = false;
    if (!VerifyAppSourceAndParseProfile(pkcs7Context, hapSignInfo.optionBlocks[profileIndex].optionalBlockValue,
        hapVerifyV1Result, profileNeedWriteCrl)) {
        HAPVERIFY_LOG_ERROR(LABEL, "APP source is not trusted");
        return APP_SOURCE_NOT_TRUSTED;
    }
    if (!GetDigestAndAlgorithm(pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get digest failed");
        return GET_DIGEST_FAIL;
    }
    std::vector<std::string> publicKeys;
    if (!HapVerifyOpensslUtils::GetPublickeys(pkcs7Context.certChains[0], publicKeys)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get publicKeys failed");
        return GET_PUBLICKEY_FAIL;
    }
    hapVerifyV1Result.SetPublicKey(publicKeys);
    std::vector<std::string> certSignatures;
    if (!HapVerifyOpensslUtils::GetSignatures(pkcs7Context.certChains[0], certSignatures)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get sianatures failed");
        return GET_SIGNATURE_FAIL;
    }
    hapVerifyV1Result.SetSignature(certSignatures);
    if (!HapSigningBlockUtils::VerifyHapIntegrity(pkcs7Context, hapFile, hapSignInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Verify Integrity failed");
        return VERIFY_INTEGRITY_FAIL;
    }
    WriteCrlIfNeed(pkcs7Context, profileNeedWriteCrl);
    return VERIFY_SUCCESS;
}

bool HapVerifyV2::VerifyAppPkcs7(Pkcs7Context& pkcs7Context, const HapByteBuffer& hapSignatureBlock)
{
    const unsigned char* pkcs7Block = reinterpret_cast<const unsigned char*>(hapSignatureBlock.GetBufferPtr());
    unsigned int pkcs7Len = static_cast<unsigned int>(hapSignatureBlock.GetCapacity());
    if (!HapVerifyOpensslUtils::ParsePkcs7Package(pkcs7Block, pkcs7Len, pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "parse pkcs7 failed");
        return false;
    }
    if (!HapVerifyOpensslUtils::GetCertChains(pkcs7Context.p7, pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetCertChains from pkcs7 failed");
        return false;
    }
    if (!HapVerifyOpensslUtils::VerifyPkcs7(pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "verify signature failed");
        return false;
    }
    return true;
}

bool HapVerifyV2::VerifyAppSourceAndParseProfile(Pkcs7Context& pkcs7Context,
    const HapByteBuffer& hapProfileBlock, HapVerifyResult& hapVerifyV1Result, bool& profileNeadWriteCrl)
{
    std::string certSubject;
    if (!HapCertVerifyOpensslUtils::GetSubjectFromX509(pkcs7Context.certChains[0][0], certSubject)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get info of sign cert failed");
        return false;
    }
    HAPVERIFY_LOG_INFO(LABEL, "App signature subject: %{public}s, issuer: %{public}s",
        certSubject.c_str(), pkcs7Context.certIssuer.c_str());

    TrustedSourceManager& trustedSourceManager = TrustedSourceManager::GetInstance();
    pkcs7Context.matchResult = trustedSourceManager.IsTrustedSource(certSubject, pkcs7Context.certIssuer,
        HAP_SIGN_BLOB, pkcs7Context.certChains[0].size());

    Pkcs7Context profileContext;
    std::string profile;
    if (!HapProfileVerifyUtils::ParseProfile(profileContext, pkcs7Context, hapProfileBlock, profile)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Parse profile pkcs7 failed");
        return false;
    }

    if (!VerifyProfileSignature(pkcs7Context, profileContext)) {
        HAPVERIFY_LOG_ERROR(LABEL, "VerifyProfileSignature failed");
        return false;
    }
    /*
     * If app source is not trusted, verify profile.
     * If profile is debug, check whether app signed cert is same as the debug cert in profile.
     * If profile is release, do not allow installation of this app.
     */
    bool isCallParseAndVerify = false;
    ProvisionInfo provisionInfo;
    if (pkcs7Context.matchResult.matchState == DO_NOT_MATCH) {
        if (!HapProfileVerifyUtils::VerifyProfile(profileContext)) {
            HAPVERIFY_LOG_ERROR(LABEL, "profile verify failed");
            return false;
        }
        AppProvisionVerifyResult profileRet = ParseAndVerify(profile, provisionInfo);
        if (profileRet != PROVISION_OK) {
            HAPVERIFY_LOG_ERROR(LABEL, "profile parsing failed, error: %{public}d", static_cast<int>(profileRet));
            return false;
        }
        if (!VerifyProfileInfo(pkcs7Context, profileContext, provisionInfo)) {
            HAPVERIFY_LOG_ERROR(LABEL, "VerifyProfileInfo failed");
            return false;
        }
        isCallParseAndVerify = true;
    }

    if (!ParseAndVerifyProfileIfNeed(profile, provisionInfo, isCallParseAndVerify)) {
        return false;
    }

    if (!GenerateAppId(provisionInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GenerateAppId failed");
        return false;
    }

    hapVerifyV1Result.SetProvisionInfo(provisionInfo);
    profileNeadWriteCrl = profileContext.needWriteCrl;
    return true;
}

bool HapVerifyV2::VerifyProfileSignature(const Pkcs7Context& pkcs7Context, Pkcs7Context& profileContext)
{
    if (pkcs7Context.matchResult.matchState == MATCH_WITH_SIGN &&
        pkcs7Context.matchResult.source == APP_THIRD_PARTY_PRELOAD) {
        if (!HapProfileVerifyUtils::VerifyProfile(profileContext)) {
            HAPVERIFY_LOG_ERROR(LABEL, "profile verify failed");
            return false;
        }
    }
    return true;
}

bool HapVerifyV2::GenerateAppId(ProvisionInfo& provisionInfo)
{
    std::string& certInProfile = provisionInfo.bundleInfo.distributionCertificate;
    if (provisionInfo.bundleInfo.distributionCertificate.empty()) {
        certInProfile = provisionInfo.bundleInfo.developmentCertificate;
        HAPVERIFY_LOG_DEBUG(LABEL, "use development Certificate");
    }
    std::string publicKey;
    if (!HapCertVerifyOpensslUtils::GetPublickeyBase64FromPemCert(certInProfile, publicKey)) {
        return false;
    }
    provisionInfo.appId = publicKey;
    HAPVERIFY_LOG_DEBUG(LABEL, "provisionInfo.appId: %{public}s", provisionInfo.appId.c_str());
    return true;
}

bool HapVerifyV2::VerifyProfileInfo(const Pkcs7Context& pkcs7Context, const Pkcs7Context& profileContext,
    ProvisionInfo& provisionInfo)
{
    if (!CheckProfileSignatureIsRight(profileContext.matchResult.matchState, provisionInfo.type)) {
        return false;
    }
    std::string& certInProfile = provisionInfo.bundleInfo.developmentCertificate;
    if (provisionInfo.type == ProvisionType::RELEASE) {
        if (!IsAppDistributedTypeAllowInstall(provisionInfo.distributionType, provisionInfo)) {
            HAPVERIFY_LOG_ERROR(LABEL, "untrusted source app with release profile distributionType: %{public}d",
                static_cast<int>(provisionInfo.distributionType));
            return false;
        }
        certInProfile = provisionInfo.bundleInfo.distributionCertificate;
        HAPVERIFY_LOG_DEBUG(LABEL, "allow install app with release profile distributionType: %{public}d",
            static_cast<int>(provisionInfo.distributionType));
    }
    HAPVERIFY_LOG_DEBUG(LABEL, "provisionInfo.type: %{public}d", static_cast<int>(provisionInfo.type));
    if (!HapCertVerifyOpensslUtils::CompareX509Cert(pkcs7Context.certChains[0][0], certInProfile)) {
        HAPVERIFY_LOG_ERROR(LABEL, "developed cert is not same as signed cert");
        return false;
    }
    return true;
}

bool HapVerifyV2::IsAppDistributedTypeAllowInstall(const AppDistType& type, const ProvisionInfo& provisionInfo) const
{
    switch (type) {
        case AppDistType::NONE_TYPE:
            return false;
        case AppDistType::APP_GALLERY:
            if (CheckTicketSource(provisionInfo)) {
                HAPVERIFY_LOG_INFO(LABEL, "current device is allowed to install opentest application");
                return true;
            }
            return false;
        case AppDistType::ENTERPRISE:
        case AppDistType::OS_INTEGRATION:
            return true;
        default:
            return false;
    }
}

bool HapVerifyV2::CheckProfileSignatureIsRight(const MatchingStates& matchState, const ProvisionType& type)
{
    if (matchState == MATCH_WITH_PROFILE && type == ProvisionType::RELEASE) {
        return true;
    } else if (matchState == MATCH_WITH_PROFILE_DEBUG && type == ProvisionType::DEBUG) {
        return true;
    }
    HAPVERIFY_LOG_ERROR(LABEL, "isTrustedSource: %{public}d is not match with profile type: %{public}d",
        static_cast<int>(matchState), static_cast<int>(type));
    return false;
}

void HapVerifyV2::WriteCrlIfNeed(const Pkcs7Context& pkcs7Context, const bool& profileNeedWriteCrl)
{
    if (!pkcs7Context.needWriteCrl && !profileNeedWriteCrl) {
        return;
    }
    HapCrlManager& hapCrlManager = HapCrlManager::GetInstance();
    hapCrlManager.WriteCrlsToFile();
}

bool HapVerifyV2::ParseAndVerifyProfileIfNeed(const std::string& profile,
    ProvisionInfo& provisionInfo, bool isCallParseAndVerify)
{
    if (isCallParseAndVerify) {
        return isCallParseAndVerify;
    }
    AppProvisionVerifyResult profileRet = ParseAndVerify(profile, provisionInfo);
    if (profileRet != PROVISION_OK) {
        HAPVERIFY_LOG_ERROR(LABEL, "profile parse failed, error: %{public}d", static_cast<int>(profileRet));
        return false;
    }
    return true;
}

bool HapVerifyV2::GetDigestAndAlgorithm(Pkcs7Context& digest)
{
    /*
     * contentinfo format:
     * int: version
     * int: block number
     * digest blocks:
     * each digest block format:
     * int: length of sizeof(digestblock) - 4
     * int: Algorithm ID
     * int: length of digest
     * byte[]: digest
     */
    /* length of sizeof(digestblock - 4) */
    int digestBlockLen;
    if (!digest.content.GetInt32(DIGEST_BLOCK_LEN_OFFSET, digestBlockLen)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get digestBlockLen failed");
        return false;
    }
    /* Algorithm ID */
    if (!digest.content.GetInt32(DIGEST_ALGORITHM_OFFSET, digest.digestAlgorithm)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get digestAlgorithm failed");
        return false;
    }
    /* length of digest */
    int digestlen;
    if (!digest.content.GetInt32(DIGEST_LEN_OFFSET, digestlen)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get digestlen failed");
        return false;
    }

    int sum = sizeof(digestlen) + sizeof(digest.digestAlgorithm) + digestlen;
    if (sum != digestBlockLen) {
        HAPVERIFY_LOG_ERROR(LABEL, "digestBlockLen: %{public}d is not equal to sum: %{public}d",
            digestBlockLen, sum);
        return false;
    }
    /* set position to the digest start point */
    digest.content.SetPosition(DIGEST_OFFSET_IN_CONTENT);
    /* set limit to the digest end point */
    digest.content.SetLimit(DIGEST_OFFSET_IN_CONTENT + digestlen);
    digest.content.Slice();
    return true;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
