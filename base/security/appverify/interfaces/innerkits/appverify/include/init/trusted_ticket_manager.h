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
#ifndef HAP_TRUSTED_TICKET_MANAGER_H
#define HAP_TRUSTED_TICKET_MANAGER_H

#include <string>
#include <unordered_map>

#include "common/export_define.h"
#include "init/json_parser_utils.h"
#include "init/matching_result.h"

namespace OHOS {
namespace Security {
namespace Verify {
struct HapTicketSourceInfo {
    TrustedSources source;
    std::string sourceName;
    std::string ticketSigningCert;
    std::string issuer;
    int maxCertsPath;
    StringVec critialcalCertExtension;
};

using TicketSourceInfoVec = std::vector<HapTicketSourceInfo>;

class TrustedTicketManager {
public:
    DLL_EXPORT static TrustedTicketManager& GetInstance();
    DLL_EXPORT bool Init();
    DLL_EXPORT void Recovery();
    DLL_EXPORT MatchingResult IsTrustedSource(const std::string& certSubject, const std::string& certIssuer,
        int certListPath) const;

private:
    TrustedTicketManager();
    ~TrustedTicketManager();

    /* Forbid external replication constructs and external replication */
    TrustedTicketManager(const TrustedTicketManager& trustedSource) = delete;
    TrustedTicketManager& operator =(const TrustedTicketManager& trustedSource) = delete;

    bool GetTicketTrustedSources(TicketSourceInfoVec& trustedTicketSources, std::string& sourcesVersion,
        std::string& sourcesReleaseTime, const std::string& filePath);
    bool ParseTrustedTicketSourceJson(TicketSourceInfoVec& trustedTicketSources,
        const JsonObjVec& trustedTicketJson);
    std::string EncapTrustedTicketSourceString(const HapTicketSourceInfo& ticketSourceInfo);
    MatchingResult MatchTrustedSource(const TicketSourceInfoVec& trustedTicketSources, const std::string& certSubject,
        const std::string& certIssuer, int certListPath) const;
    MatchingStates TrustedSourceListCompare(const std::string& certSubject, const std::string& certIssuer,
        const HapTicketSourceInfo& TicketSource) const;
    bool MatchSubject(const std::string& trustedSource, const std::string& certSubject) const;
    bool MatchIssuer(const std::string& trustedSource, const std::string& certIssuer) const;

private:
    static const std::string TICKET_TRUSTED_SOURCE_FILE_PATH;
    static const std::string KEY_OF_TICKET_TRUSTED_SOURCE;
    static const std::string KEY_OF_TICKET_TRUSTED_SOURCE_VERSION;
    static const std::string KEY_OF_TICKET_TRUSTED_SOURCE_RELEASETIME;
    static const std::string KEY_OF_SOURCE_NAME;
    static const std::string KEY_OF_TICKET_SIGNING_CERT;
    static const std::string KEY_OF_ISSUER;
    static const std::string KEY_OF_MAX_CERTS_PATH;
    static const std::string KEY_OF_CRITIALCAL_CERT_EXTENSION;
    TicketSourceInfoVec TicketTrustedSources;
    std::string version;
    std::string releaseTime;
    bool isInit;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_TRUSTED_TICKET_MANAGER_H
