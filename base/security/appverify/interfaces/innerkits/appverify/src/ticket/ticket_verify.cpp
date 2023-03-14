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

#include "ticket/ticket_verify.h"

#include <algorithm>
#include <regex>

#include "nlohmann/json.hpp"

#ifndef STANDARD_SYSTEM
#include "ohos_account_kits.h"
#else
#include "parameter.h"
#include "sysparam_errno.h"
#endif // STANDARD_SYSTEM

#include "common/hap_byte_buffer.h"
#include "common/hap_verify_log.h"
#include "common/random_access_file.h"
#include "init/trusted_ticket_manager.h"
#include "util/hap_cert_verify_openssl_utils.h"
#include "util/hap_verify_openssl_utils.h"
#include "util/pkcs7_context.h"

namespace {
const int MAXIMUM_DEVICES = 100;
const int TICKET_MAX_SIZE = 18432;
const std::string TICKET_FILE_PATH = "/data/update/ticket/";
const std::string VALUE_DEVICE_TYPE_UDID = "udid";
} // namespace

namespace OHOS {
namespace Security {
namespace Verify {
bool CheckTicketFilePath(const std::string& filePath, std::string& standardFilePath)
{
    char path[PATH_MAX + 1] = { 0x00 };
    if (filePath.size() > PATH_MAX || realpath(filePath.c_str(), path) == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "filePath is not a standard path");
        return false;
    }
    standardFilePath = std::string(path);
    return true;
}

bool CheckPermissions(std::vector<std::string> ticketPermissions, std::vector<std::string> profilePermissions)
{
    for (auto ticket : ticketPermissions) {
        auto iter = find(profilePermissions.begin(), profilePermissions.end(), ticket);
        if (iter == profilePermissions.end()) {
            return false;
        }
    }
    return true;
}

inline bool CheckDevice(const std::vector<std::string>& deviceIds, const std::string& deviceId)
{
    auto iter = find(deviceIds.begin(), deviceIds.end(), deviceId);
    if (iter == deviceIds.end()) {
        return false;
    }
    return true;
}

AppProvisionVerifyResult CheckDevice(ProvisionInfo& info)
{
    // Checking device ids
    if (info.debugInfo.deviceIds.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "device-id list is empty.");
        return PROVISION_DEVICE_UNAUTHORIZED;
    }

    if (info.debugInfo.deviceIds.size() > MAXIMUM_DEVICES) {
        HAPVERIFY_LOG_ERROR(LABEL, "No. of device IDs in list exceed maximum number %{public}d", MAXIMUM_DEVICES);
        return PROVISION_NUM_DEVICE_EXCEEDED;
    }

    if (info.debugInfo.deviceIdType != VALUE_DEVICE_TYPE_UDID) {
        HAPVERIFY_LOG_ERROR(LABEL, "type of device ID is not supported.");
        return PROVISION_UNSUPPORTED_DEVICE_TYPE;
    }

    std::string deviceId;
#ifndef STANDARD_SYSTEM
    int32_t ret = OHOS::AccountSA::OhosAccountKits::GetInstance().GetUdid(deviceId);
    if (ret != 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "obtaining current device id failed (%{public}d).", ret);
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
#else
    char udid[DEV_UUID_LEN] = {0};
    int ret = GetDevUdid(udid, sizeof(udid));
    if (ret != EC_SUCCESS) {
        HAPVERIFY_LOG_ERROR(LABEL, "obtaining current device id failed (%{public}d).", static_cast<int>(ret));
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
    deviceId = std::string(udid, sizeof(udid) - 1);
#endif // STANDARD_SYSTEM
    if (deviceId.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "device-id of current device is empty.");
        return PROVISION_DEVICE_UNAUTHORIZED;
    }

    if (!CheckDevice(info.debugInfo.deviceIds, deviceId)) {
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
    return PROVISION_OK;
}

int CompareTicketAndProfile(const ProvisionInfo& ticketInfo, const ProvisionInfo& profileInfo)
{
    if (ticketInfo.bundleInfo.bundleName != profileInfo.bundleInfo.bundleName) {
        HAPVERIFY_LOG_ERROR(LABEL, "ticket bundlename doesn't match");
        return TICKET_NOT_MATCH;
    }

    if (ticketInfo.type != profileInfo.type) {
        return TICKET_NOT_MATCH;
    }

    if (ticketInfo.type == DEBUG) {
        if (ticketInfo.bundleInfo.developmentCertificate != profileInfo.bundleInfo.developmentCertificate) {
            HAPVERIFY_LOG_ERROR(LABEL, "ticket developmentCertificate doesn't match");
            return TICKET_NOT_MATCH;
        }
    } else {
        if (ticketInfo.bundleInfo.distributionCertificate != profileInfo.bundleInfo.distributionCertificate) {
            HAPVERIFY_LOG_ERROR(LABEL, "ticket distributionCertificate doesn't match");
            return TICKET_NOT_MATCH;
        }
    }

    if (!ticketInfo.permissions.restrictedCapabilities.empty()) {
        if (!CheckPermissions(ticketInfo.permissions.restrictedCapabilities,
            profileInfo.permissions.restrictedCapabilities)) {
            HAPVERIFY_LOG_ERROR(LABEL, "ticket restrictedCapabilities doesn't match");
            return TICKET_PERMISSION_ERROR;
        }
    }

    if (!ticketInfo.permissions.restrictedPermissions.empty()) {
        if (!CheckPermissions(ticketInfo.permissions.restrictedPermissions,
            profileInfo.permissions.restrictedPermissions)) {
            HAPVERIFY_LOG_ERROR(LABEL, "ticket restrictedPermissions doesn't match");
            return TICKET_PERMISSION_ERROR;
        }
    }
    return TICKET_OK;
}

bool VerifyTicketSignature(HapByteBuffer& ticketBlock, Pkcs7Context& pkcs7Context, std::string& ticket)
{
    const unsigned char* pkcs7Block = reinterpret_cast<const unsigned char*>(ticketBlock.GetBufferPtr());
    unsigned int pkcs7Len = static_cast<unsigned int>(ticketBlock.GetCapacity());
    if (!HapVerifyOpensslUtils::ParsePkcs7Package(pkcs7Block, pkcs7Len, pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Parse ticket pkcs7 failed");
        return false;
    }
    ticket = std::string(pkcs7Context.content.GetBufferPtr(), pkcs7Context.content.GetCapacity());

    if (!HapVerifyOpensslUtils::GetCertChains(pkcs7Context.p7, pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetCertChains from ticket pkcs7 failed");
        return false;
    }

    if (!HapVerifyOpensslUtils::VerifyPkcs7(pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "verify ticket signature failed");
        return false;
    }

    std::string certSubject;
    if (!HapCertVerifyOpensslUtils::GetSubjectFromX509(pkcs7Context.certChains[0][0], certSubject)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get info of sign cert from ticket failed");
        return false;
    }

    TrustedTicketManager& trustedTicketSourceManager = TrustedTicketManager::GetInstance();
    pkcs7Context.matchResult = trustedTicketSourceManager.IsTrustedSource(certSubject, pkcs7Context.certIssuer,
        pkcs7Context.certChains[0].size());
    if (pkcs7Context.matchResult.matchState == DO_NOT_MATCH) {
        HAPVERIFY_LOG_ERROR(LABEL, "Ticket signature is not trusted source, subject: %{public}s, issuer: %{public}s",
            certSubject.c_str(), pkcs7Context.certIssuer.c_str());
        return false;
    }
    HAPVERIFY_LOG_INFO(LABEL, "Ticket subject: %{public}s, issuer: %{public}s",
        certSubject.c_str(), pkcs7Context.certIssuer.c_str());
    return true;
}

int TicketParseAndVerify(const std::string& ticket, ProvisionInfo& ticketInfo,
    const ProvisionInfo& profileInfo)
{
    if (ParseProvision(ticket, ticketInfo) != PROVISION_OK) {
        return TICKET_PARSE_FAIL;
    }
    int ret = CompareTicketAndProfile(ticketInfo, profileInfo);
    if (ret != TICKET_OK) {
        return ret;
    }
    if (CheckDevice(ticketInfo) != PROVISION_OK) {
        return TICKET_DEVICE_INVALID;
    }
    return TICKET_OK;
}

int VerifyTicket(const std::string& filePath, const ProvisionInfo& profileInfo)
{
    HAPVERIFY_LOG_DEBUG(LABEL, "Enter Ticket Verify");
    RandomAccessFile ticketFile;
    if (!ticketFile.Init(filePath)) {
        HAPVERIFY_LOG_ERROR(LABEL, "open %{public}s failed", filePath.c_str());
        return OPEN_TICKET_FILE_ERROR;
    }
    long long fileLength = ticketFile.GetLength();
    if (fileLength > TICKET_MAX_SIZE) {
        HAPVERIFY_LOG_ERROR(LABEL, "file length %{public}lld is too larger", fileLength);
        return OPEN_TICKET_FILE_ERROR;
    }
    int fileLen = static_cast<int>(fileLength);
    HapByteBuffer ticketBlock(fileLen);
    long long ret = ticketFile.ReadFileFullyFromOffset(ticketBlock, 0);
    if (ret < 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "read data from ticket error: %{public}lld", ret);
        return TICKET_READ_FAIL;
    }

    Pkcs7Context pkcs7Context;
    std::string ticket;
    if (!VerifyTicketSignature(ticketBlock, pkcs7Context, ticket)) {
        HAPVERIFY_LOG_ERROR(LABEL, "verify ticket signature failed");
        return SIGNATURE_VERIFY_FAIL;
    }

    ProvisionInfo ticketInfo;
    int ticketRet = TicketParseAndVerify(ticket, ticketInfo, profileInfo);
    if (ticketRet != TICKET_OK) {
        HAPVERIFY_LOG_ERROR(LABEL, "ticket parse failed, error: %{public}d", static_cast<int>(ticketRet));
        return ticketRet;
    }
    HAPVERIFY_LOG_DEBUG(LABEL, "Leave Ticket Verify");
    return TICKET_VERIFY_SUCCESS;
}

bool CheckTicketSource(const ProvisionInfo& profileInfo)
{
    std::string ticketfilepath = TICKET_FILE_PATH + profileInfo.bundleInfo.bundleName + ".p7b";
    std::string standardFilePath;
    if (!CheckTicketFilePath(ticketfilepath, standardFilePath)) {
        return false;
    }

    int ret = VerifyTicket(standardFilePath, profileInfo);
    if (ret != TICKET_VERIFY_SUCCESS) {
        HAPVERIFY_LOG_ERROR(LABEL, "ticket verify failed, result: %{public}d", ret);
        return false;
    }
    HAPVERIFY_LOG_INFO(LABEL, "Ticket verify success");
    return true;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
