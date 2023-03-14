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

#include "init/trusted_source_manager.h"

#include "nlohmann/json.hpp"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
const std::string TrustedSourceManager::APP_TRUSTED_SOURCE_FILE_PATH =
    "/system/etc/security/trusted_apps_sources.json";
const std::string TrustedSourceManager::APP_TRUSTED_SOURCE_TEST_FILE_PATH =
    "/system/etc/security/trusted_apps_sources_test.json";
const std::string TrustedSourceManager::KEY_OF_APP_TRUSTED_SOURCE = "trust-app-source";
const std::string TrustedSourceManager::KEY_OF_APP_TRUSTED_SOURCE_VERSION = "version";
const std::string TrustedSourceManager::KEY_OF_APP_TRUSTED_SOURCE_RELEASETIME = "release-time";
const std::string TrustedSourceManager::KEY_OF_SOURCE_NAME = "name";
const std::string TrustedSourceManager::KEY_OF_APP_SIGNING_CERT = "app-signing-cert";
const std::string TrustedSourceManager::KEY_OF_PROFILE_SIGNING_CERTIFICATE = "profile-signing-certificate";
const std::string TrustedSourceManager::KEY_OF_PROFILE_DEBUG_SIGNING_CERTIFICATE = "profile-debug-signing-certificate";
const std::string TrustedSourceManager::KEY_OF_ISSUER = "issuer-ca";
const std::string TrustedSourceManager::KEY_OF_MAX_CERTS_PATH = "max-certs-path";
const std::string TrustedSourceManager::KEY_OF_CRITIALCAL_CERT_EXTENSION = "critialcal-cert-extension";
const std::string TrustedSourceManager::APP_GALLERY_SOURCE_NAME = "huawei app gallery";
const std::string TrustedSourceManager::APP_SYSTEM_SOURCE_NAME = "huawei system apps";
const std::string TrustedSourceManager::APP_THIRD_PARTY_PRELOAD_SOURCE_NAME = "third_party app preload";

TrustedSourceManager& TrustedSourceManager::GetInstance()
{
    static TrustedSourceManager singleTrustedSourceManager;
    return singleTrustedSourceManager;
}

TrustedSourceManager::TrustedSourceManager()
    : appTrustedSources(), appTrustedSourcesForTest(), version(), versionForTest(), releaseTime(),
      releaseTimeForTest(), isInit(false), isDebug(false)
{
}

TrustedSourceManager::~TrustedSourceManager()
{
}

bool TrustedSourceManager::EnableDebug()
{
    if (isDebug) {
        return true;
    }

    isDebug = GetAppTrustedSources(appTrustedSourcesForTest, versionForTest,
        releaseTimeForTest, APP_TRUSTED_SOURCE_TEST_FILE_PATH);
    if (isDebug) {
        HAPVERIFY_LOG_INFO(LABEL, "trusted app source test version: %{public}s, releaseTime: %{public}s, Size:"
            " %{public}zu", versionForTest.c_str(), releaseTimeForTest.c_str(), appTrustedSourcesForTest.size());
    }
    return isDebug;
}

void TrustedSourceManager::DisableDebug()
{
    isDebug = false;
    appTrustedSourcesForTest.clear();
}

bool TrustedSourceManager::Init()
{
    if (isInit) {
        return true;
    }

    isInit = GetAppTrustedSources(appTrustedSources, version, releaseTime, APP_TRUSTED_SOURCE_FILE_PATH);
    if (isInit) {
        HAPVERIFY_LOG_INFO(LABEL, "trusted app source version: %{public}s, releaseTime: %{public}s, Size:"
            " %{public}zu", version.c_str(), releaseTime.c_str(), appTrustedSources.size());
    }
    return isInit;
}

void TrustedSourceManager::Recovery()
{
    appTrustedSources.clear();
    isInit = false;
}

bool TrustedSourceManager::GetAppTrustedSources(SourceInfoVec& trustedAppSources, std::string& souucesVersion,
    std::string& souucesReleaseTime, const std::string& filePath)
{
    nlohmann::json trustedSourceJson;
    std::string errorInfo;
    if (!JsonParserUtils::ReadTrustedRootCAFromJson(trustedSourceJson, filePath, errorInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get jsonObj from %{public}s failed, because %{public}s",
            filePath.c_str(), errorInfo.c_str());
        return false;
    }
    if (!JsonParserUtils::GetJsonString(trustedSourceJson, KEY_OF_APP_TRUSTED_SOURCE_VERSION, souucesVersion)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get version failed");
        return false;
    }
    if (!JsonParserUtils::GetJsonString(trustedSourceJson,
        KEY_OF_APP_TRUSTED_SOURCE_RELEASETIME, souucesReleaseTime)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get releaseTime failed");
        return false;
    }
    JsonObjVec trustedAppSourceJson;
    if (!JsonParserUtils::ParseJsonToObjVec(trustedSourceJson, KEY_OF_APP_TRUSTED_SOURCE, trustedAppSourceJson)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get JsonObjVec failed");
        return false;
    }
    if (!ParseTrustedAppSourceJson(trustedAppSources, trustedAppSourceJson)) {
        HAPVERIFY_LOG_ERROR(LABEL, "parse JsonObjVec failed");
        return false;
    }
    if (trustedAppSources.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "no app trusted source");
        return false;
    }
    return true;
}

bool TrustedSourceManager::ParseTrustedAppSourceJson(SourceInfoVec& trustedAppSources,
    const JsonObjVec& trustedAppSourceJson)
{
    for (auto appSource : trustedAppSourceJson) {
        HapAppSourceInfo hapAppSource;
        if (!JsonParserUtils::GetJsonString(appSource, KEY_OF_SOURCE_NAME, hapAppSource.sourceName)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get sourceName Failed");
            return false;
        }
        hapAppSource.source = GetTrustedSource(hapAppSource.sourceName);
        if (!JsonParserUtils::GetJsonString(appSource, KEY_OF_APP_SIGNING_CERT, hapAppSource.appSigningCert)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get appSigningCert Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonString(appSource, KEY_OF_PROFILE_SIGNING_CERTIFICATE,
            hapAppSource.profileSigningCertificate)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get profileSigningCertificate Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonString(appSource, KEY_OF_PROFILE_DEBUG_SIGNING_CERTIFICATE,
            hapAppSource.profileDebugSigningCertificate)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get profileDebugSigningCertificate Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonString(appSource, KEY_OF_ISSUER, hapAppSource.issuer)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get issuer Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonInt(appSource, KEY_OF_MAX_CERTS_PATH, hapAppSource.maxCertsPath)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get maxCertsPath Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonStringVec(appSource, KEY_OF_CRITIALCAL_CERT_EXTENSION,
            hapAppSource.critialcalCertExtension)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get critialcalCertExtension Failed");
            return false;
        }
        HAPVERIFY_LOG_INFO(LABEL, "trusted app source: %{public}s", EncapTrustedAppSourceString(hapAppSource).c_str());
        trustedAppSources.push_back(hapAppSource);
    }
    return true;
}

std::string TrustedSourceManager::EncapTrustedAppSourceString(const HapAppSourceInfo& appSourceInfo)
{
    std::string info =  "sourceName: " + appSourceInfo.sourceName + "\n" +
        "sourceNumber: " + std::to_string(static_cast<int>(appSourceInfo.source)) + "\n" +
        "appSigningCert: " + appSourceInfo.appSigningCert + "\n" +
        "profileSigningCertificate: " + appSourceInfo.profileSigningCertificate + "\n" +
        "profileDebugSigningCertificate: " + appSourceInfo.profileDebugSigningCertificate + "\n" +
        "issuer: " + appSourceInfo.issuer + "\n" +
        "maxCertsPath: " + std::to_string(appSourceInfo.maxCertsPath) + "\n" +
        "critialcalCertExtension: ";
    for (auto extension : appSourceInfo.critialcalCertExtension) {
        info += extension + ", ";
    }
    return info;
}

MatchingResult TrustedSourceManager::IsTrustedSource(const std::string& certSubject,
    const std::string& certIssuer, HapBlobType blobType, int certListPath) const
{
    MatchingResult ret = MatchTrustedSource(appTrustedSources, certSubject, certIssuer, blobType, certListPath);
    if (ret.matchState != DO_NOT_MATCH) {
        return ret;
    }

    if (isDebug) {
        return MatchTrustedSource(appTrustedSourcesForTest, certSubject, certIssuer, blobType, certListPath);
    }
    return ret;
}

MatchingResult TrustedSourceManager::MatchTrustedSource(const SourceInfoVec& trustedAppSources,
    const std::string& certSubject, const std::string& certIssuer, HapBlobType blobType, int certListPath) const
{
    MatchingResult ret;
    ret.matchState = DO_NOT_MATCH;
    for (auto appSource : trustedAppSources) {
        if (certListPath == appSource.maxCertsPath) {
            ret.matchState = TrustedSourceListCompare(certSubject, certIssuer, appSource, blobType);
            if (ret.matchState != DO_NOT_MATCH) {
                ret.source = appSource.source;
                break;
            }
        }
    }
    return ret;
}

MatchingStates TrustedSourceManager::TrustedSourceListCompare(const std::string& certSubject,
    const std::string& certIssuer, const HapAppSourceInfo& appSource, HapBlobType blobType) const
{
    MatchingStates ret = DO_NOT_MATCH;
    switch (blobType) {
        case HAP_SIGN_BLOB: {
            if (MatchSubjectAndIssuer(appSource.appSigningCert, certSubject) &&
                MatchSubjectAndIssuer(appSource.issuer, certIssuer)) {
                ret = MATCH_WITH_SIGN;
            }
            break;
        }
        case PROFILE_BLOB: {
            if (MatchSubjectAndIssuer(appSource.issuer, certIssuer)) {
                if (MatchSubjectAndIssuer(appSource.profileSigningCertificate, certSubject)) {
                    ret = MATCH_WITH_PROFILE;
                } else if (MatchSubjectAndIssuer(appSource.profileDebugSigningCertificate, certSubject)) {
                    ret = MATCH_WITH_PROFILE_DEBUG;
                }
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

TrustedSources TrustedSourceManager::GetTrustedSource(std::string& sourceName)
{
    if (APP_GALLERY_SOURCE_NAME == sourceName) {
        return APP_GALLARY;
    }

    if (APP_SYSTEM_SOURCE_NAME == sourceName) {
        return APP_SYSTEM;
    }

    if (APP_THIRD_PARTY_PRELOAD_SOURCE_NAME == sourceName) {
        return APP_THIRD_PARTY_PRELOAD;
    }
    return OTHER_TRUSTED_SOURCE;
}

bool TrustedSourceManager::MatchSubjectAndIssuer(const std::string& trustedSource,
                                                 const std::string& certSubjectOrIssuer) const
{
    if (trustedSource.empty()) {
        return false;
    }

    return trustedSource == certSubjectOrIssuer;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
