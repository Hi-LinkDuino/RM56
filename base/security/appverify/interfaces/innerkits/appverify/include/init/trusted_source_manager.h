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
#ifndef HAP_TRUSTED_SOURCE_MANAGER_H
#define HAP_TRUSTED_SOURCE_MANAGER_H

#include <string>
#include <unordered_map>

#include "common/export_define.h"
#include "init/json_parser_utils.h"
#include "init/matching_result.h"
#include "util/hap_signing_block_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
struct HapAppSourceInfo {
    TrustedSources source;
    std::string sourceName;
    std::string appSigningCert;
    std::string profileSigningCertificate;
    std::string profileDebugSigningCertificate;
    std::string issuer;
    int maxCertsPath;
    StringVec critialcalCertExtension;
};

using SourceInfoVec = std::vector<HapAppSourceInfo>;

class TrustedSourceManager {
public:
    DLL_EXPORT static TrustedSourceManager& GetInstance();
    DLL_EXPORT bool Init();
    DLL_EXPORT void Recovery();
    DLL_EXPORT bool EnableDebug();
    DLL_EXPORT void DisableDebug();
    DLL_EXPORT MatchingResult IsTrustedSource(const std::string& certSubject, const std::string& certIssuer,
        HapBlobType blobType, int certListPath) const;

private:
    TrustedSourceManager();
    ~TrustedSourceManager();

    /* Forbid external replication constructs and external replication */
    TrustedSourceManager(const TrustedSourceManager& trustedSource) = delete;
    TrustedSourceManager& operator = (const TrustedSourceManager& trustedSource) = delete;

    bool GetAppTrustedSources(SourceInfoVec& trustedAppSources, std::string& souucesVersion,
        std::string& souucesReleaseTime, const std::string& filePath);
    bool ParseTrustedAppSourceJson(SourceInfoVec& trustedAppSources, const JsonObjVec& trustedAppSourceJson);
    std::string EncapTrustedAppSourceString(const HapAppSourceInfo& appSourceInfo);
    MatchingResult MatchTrustedSource(const SourceInfoVec& trustedAppSources, const std::string& certSubject,
        const std::string& certIssuer, HapBlobType blobType, int certListPath) const;
    MatchingStates TrustedSourceListCompare(const std::string& certSubject, const std::string& certIssuer,
        const HapAppSourceInfo& appSource, HapBlobType blobType) const;
    TrustedSources GetTrustedSource(std::string& sourceName);
    bool MatchSubjectAndIssuer(const std::string& trustedSource, const std::string& certSubjectOrIssuer) const;

private:
    static const std::string APP_TRUSTED_SOURCE_FILE_PATH;
    static const std::string APP_TRUSTED_SOURCE_TEST_FILE_PATH;
    static const std::string KEY_OF_APP_TRUSTED_SOURCE;
    static const std::string KEY_OF_APP_TRUSTED_SOURCE_VERSION;
    static const std::string KEY_OF_APP_TRUSTED_SOURCE_RELEASETIME;
    static const std::string KEY_OF_SOURCE_NAME;
    static const std::string KEY_OF_APP_SIGNING_CERT;
    static const std::string KEY_OF_PROFILE_SIGNING_CERTIFICATE;
    static const std::string KEY_OF_PROFILE_DEBUG_SIGNING_CERTIFICATE;
    static const std::string KEY_OF_ISSUER;
    static const std::string KEY_OF_MAX_CERTS_PATH;
    static const std::string KEY_OF_CRITIALCAL_CERT_EXTENSION;
    static const std::string APP_GALLERY_SOURCE_NAME;
    static const std::string APP_SYSTEM_SOURCE_NAME;
    static const std::string APP_THIRD_PARTY_PRELOAD_SOURCE_NAME;
    SourceInfoVec appTrustedSources;
    SourceInfoVec appTrustedSourcesForTest;
    std::string version;
    std::string versionForTest;
    std::string releaseTime;
    std::string releaseTimeForTest;
    bool isInit;
    bool isDebug;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_TRUSTED_SOURCE_MANAGER_H
