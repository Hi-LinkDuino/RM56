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

#include "init/trusted_ticket_manager.h"

#include <regex>

#include "nlohmann/json.hpp"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
const std::string TrustedTicketManager::TICKET_TRUSTED_SOURCE_FILE_PATH =
    "/system/etc/security/trusted_tickets_sources.json";
const std::string TrustedTicketManager::KEY_OF_TICKET_TRUSTED_SOURCE = "trust-app-source";
const std::string TrustedTicketManager::KEY_OF_TICKET_TRUSTED_SOURCE_VERSION = "version";
const std::string TrustedTicketManager::KEY_OF_TICKET_TRUSTED_SOURCE_RELEASETIME = "release-time";
const std::string TrustedTicketManager::KEY_OF_SOURCE_NAME = "name";
const std::string TrustedTicketManager::KEY_OF_TICKET_SIGNING_CERT = "ticket-signing-cert";
const std::string TrustedTicketManager::KEY_OF_ISSUER = "issuer-ca";
const std::string TrustedTicketManager::KEY_OF_MAX_CERTS_PATH = "max-certs-path";
const std::string TrustedTicketManager::KEY_OF_CRITIALCAL_CERT_EXTENSION = "critialcal-cert-extension";

TrustedTicketManager& TrustedTicketManager::GetInstance()
{
    static TrustedTicketManager singleTrustedTicketManager;
    return singleTrustedTicketManager;
}

TrustedTicketManager::TrustedTicketManager()
    : TicketTrustedSources(), version(), releaseTime(), isInit(false)
{
}

TrustedTicketManager::~TrustedTicketManager()
{
}

bool TrustedTicketManager::Init()
{
    if (isInit) {
        return true;
    }

    isInit = GetTicketTrustedSources(TicketTrustedSources, version, releaseTime, TICKET_TRUSTED_SOURCE_FILE_PATH);
    if (isInit) {
        HAPVERIFY_LOG_INFO(LABEL, "trusted ticket source version: %{public}s, releaseTime: %{public}s, Size:"
            " %{public}zu", version.c_str(), releaseTime.c_str(), TicketTrustedSources.size());
    }
    return isInit;
}

void TrustedTicketManager::Recovery()
{
    isInit = false;
    TicketTrustedSources.clear();
}

bool TrustedTicketManager::GetTicketTrustedSources(TicketSourceInfoVec& trustedTicketSources,
    std::string& sourcesVersion, std::string& sourcesReleaseTime, const std::string& filePath)
{
    nlohmann::json trustedSourceJson;
    std::string errorInfo;
    if (!JsonParserUtils::ReadTrustedRootCAFromJson(trustedSourceJson, filePath, errorInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get jsonObj from %{public}s failed, because %{public}s",
            filePath.c_str(), errorInfo.c_str());
        return false;
    }
    if (!JsonParserUtils::GetJsonString(trustedSourceJson, KEY_OF_TICKET_TRUSTED_SOURCE_VERSION, sourcesVersion)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get version failed");
        return false;
    }
    if (!JsonParserUtils::GetJsonString(trustedSourceJson,
        KEY_OF_TICKET_TRUSTED_SOURCE_RELEASETIME, sourcesReleaseTime)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get releaseTime failed");
        return false;
    }
    JsonObjVec trustedTicketJson;
    if (!JsonParserUtils::ParseJsonToObjVec(trustedSourceJson, KEY_OF_TICKET_TRUSTED_SOURCE, trustedTicketJson)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get JsonObjVec failed");
        return false;
    }
    if (!ParseTrustedTicketSourceJson(trustedTicketSources, trustedTicketJson)) {
        HAPVERIFY_LOG_ERROR(LABEL, "parse JsonObjVec failed");
        return false;
    }
    if (trustedTicketSources.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "no ticket trusted source");
        return false;
    }
    return true;
}

bool TrustedTicketManager::ParseTrustedTicketSourceJson(TicketSourceInfoVec& trustedTicketSources,
    const JsonObjVec& trustedTicketJson)
{
    for (auto TicketSource : trustedTicketJson) {
        HapTicketSourceInfo hapTicketSource;
        if (!JsonParserUtils::GetJsonString(TicketSource, KEY_OF_SOURCE_NAME, hapTicketSource.sourceName)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get sourceName Failed");
            return false;
        }
        hapTicketSource.source = OTHER_TRUSTED_SOURCE;
        if (!JsonParserUtils::GetJsonString(TicketSource, KEY_OF_TICKET_SIGNING_CERT,
            hapTicketSource.ticketSigningCert)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get ticketSigningCert Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonString(TicketSource, KEY_OF_ISSUER, hapTicketSource.issuer)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get issuer Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonInt(TicketSource, KEY_OF_MAX_CERTS_PATH, hapTicketSource.maxCertsPath)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get maxCertsPath Failed");
            return false;
        }
        if (!JsonParserUtils::GetJsonStringVec(TicketSource, KEY_OF_CRITIALCAL_CERT_EXTENSION,
            hapTicketSource.critialcalCertExtension)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get critialcalCertExtension Failed");
            return false;
        }
        HAPVERIFY_LOG_INFO(LABEL, "trusted ticket source: %{public}s",
            EncapTrustedTicketSourceString(hapTicketSource).c_str());
        trustedTicketSources.push_back(hapTicketSource);
    }
    return true;
}

std::string TrustedTicketManager::EncapTrustedTicketSourceString(const HapTicketSourceInfo& ticketSourceInfo)
{
    std::string info =  "sourceName: " + ticketSourceInfo.sourceName + "\n" +
        "sourceNumber: " + std::to_string(static_cast<int>(ticketSourceInfo.source)) + "\n" +
        "ticketSigningCert: " + ticketSourceInfo.ticketSigningCert + "\n" +
        "issuer: " + ticketSourceInfo.issuer + "\n" +
        "maxCertsPath: " + std::to_string(ticketSourceInfo.maxCertsPath) + "\n" +
        "critialcalCertExtension: ";
    for (auto extension : ticketSourceInfo.critialcalCertExtension) {
        info += extension + ", ";
    }
    return info;
}

MatchingResult TrustedTicketManager::IsTrustedSource(const std::string& certSubject,
    const std::string& certIssuer, int certListPath) const
{
    MatchingResult ret = MatchTrustedSource(TicketTrustedSources, certSubject, certIssuer, certListPath);
    if (ret.matchState != DO_NOT_MATCH) {
        return ret;
    }
    return ret;
}

MatchingResult TrustedTicketManager::MatchTrustedSource(const TicketSourceInfoVec& trustedTicketSources,
    const std::string& certSubject, const std::string& certIssuer, int certListPath) const
{
    MatchingResult ret;
    ret.matchState = DO_NOT_MATCH;
    for (auto TicketSource : trustedTicketSources) {
        if (certListPath == TicketSource.maxCertsPath) {
            ret.matchState = TrustedSourceListCompare(certSubject, certIssuer, TicketSource);
            if (ret.matchState != DO_NOT_MATCH) {
                ret.source = TicketSource.source;
                break;
            }
        }
    }
    return ret;
}

MatchingStates TrustedTicketManager::TrustedSourceListCompare(const std::string& certSubject,
    const std::string& certIssuer, const HapTicketSourceInfo& TicketSource) const
{
    MatchingStates ret = DO_NOT_MATCH;
    if (MatchSubject(TicketSource.ticketSigningCert, certSubject) &&
        MatchIssuer(TicketSource.issuer, certIssuer)) {
        ret = MATCH_WITH_TICKET;
    }
    return ret;
}

bool TrustedTicketManager::MatchSubject(const std::string& trustedSource,
    const std::string& certSubject) const
{
    if (trustedSource.empty()) {
        return false;
    }
    return std::regex_match(certSubject, std::regex(trustedSource));
}

bool TrustedTicketManager::MatchIssuer(const std::string& trustedSource,
    const std::string& certIssuer) const
{
    if (trustedSource.empty()) {
        return false;
    }
    return trustedSource == certIssuer;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
