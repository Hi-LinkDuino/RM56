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

#include "init/trusted_root_ca.h"

#include "nlohmann/json.hpp"

#include "common/hap_verify_log.h"
#include "util/hap_cert_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
const std::string TrustedRootCa::TRUSTED_ROOT_CA_FILE_PATH = "/system/etc/security/trusted_root_ca.json";
const std::string TrustedRootCa::TRUSTED_ROOT_CA_TEST_FILE_PATH = "/system/etc/security/trusted_root_ca_test.json";

TrustedRootCa& TrustedRootCa::GetInstance()
{
    static TrustedRootCa singleTrustedRoot;
    return singleTrustedRoot;
}

TrustedRootCa::TrustedRootCa() : rootCerts(), rootCertsForTest(), isInit(false), isDebug(false)
{
}

TrustedRootCa::~TrustedRootCa()
{
    for (auto rootCert : rootCerts) {
        X509_free(rootCert.second);
    }
    for (auto rootCert : rootCertsForTest) {
        X509_free(rootCert.second);
    }
}

bool TrustedRootCa::EnableDebug()
{
    if (isDebug) {
        return true;
    }

    isDebug = GetTrustedRootCAFromJson(rootCertsForTest, TRUSTED_ROOT_CA_TEST_FILE_PATH);
    if (isDebug) {
        HAPVERIFY_LOG_INFO(LABEL, "parse root certs test success, certs num: %{public}zu", rootCertsForTest.size());
    }
    return isDebug;
}

void TrustedRootCa::DisableDebug()
{
    isDebug = false;
    for (auto& rootCert : rootCertsForTest) {
        X509_free(rootCert.second);
    }
    rootCertsForTest.clear();
}

bool TrustedRootCa::Init()
{
    if (isInit) {
        return true;
    }

    isInit = GetTrustedRootCAFromJson(rootCerts, TRUSTED_ROOT_CA_FILE_PATH);
    if (isInit) {
        HAPVERIFY_LOG_INFO(LABEL, "parse root certs success, certs num: %{public}zu", rootCerts.size());
    }
    return isInit;
}

void TrustedRootCa::Recovery()
{
    for (auto& rootCert : rootCerts) {
        X509_free(rootCert.second);
    }
    rootCerts.clear();
    isInit = false;
}

bool TrustedRootCa::GetTrustedRootCAFromJson(StringCertMap& rootCertMap, const std::string& filePath)
{
    nlohmann::json trustedRootCAJson;
    std::string errorInfo;
    if (!JsonParserUtils::ReadTrustedRootCAFromJson(trustedRootCAJson, filePath, errorInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get jsonObj from %{public}s failed, because %{public}s",
            filePath.c_str(), errorInfo.c_str());
        return false;
    }

    JsonMap trustedRootCAJsonMap;
    JsonParserUtils::ParseJsonToMap(trustedRootCAJson, trustedRootCAJsonMap);
    for (auto jsonPair : trustedRootCAJsonMap) {
        HAPVERIFY_LOG_INFO(LABEL, "parse cert: %{public}s", jsonPair.second.c_str());
        X509* cert = HapCertVerifyOpensslUtils::GetX509CertFromPemString(jsonPair.second);
        if (cert == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "GetX509CertFromPemString failed, key: %{public}s value: %{public}s",
                jsonPair.first.c_str(), jsonPair.second.c_str());
            return false;
        }
        rootCertMap[jsonPair.first] = cert;
    }

    if (rootCertMap.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "no root cert");
        return false;
    }
    return true;
}

X509* TrustedRootCa::FindMatchedRoot(X509* caCert)
{
    if (caCert == nullptr) {
        return nullptr;
    }

    X509* root = FindMatchedRoot(rootCerts, caCert);
    if (root != nullptr) {
        return root;
    }

    if (isDebug) {
        HAPVERIFY_LOG_INFO(LABEL, "try to match with test root");
        root = FindMatchedRoot(rootCertsForTest, caCert);
    }
    return root;
}

X509* TrustedRootCa::FindMatchedRoot(const StringCertMap& rootCertMap, X509* caCert)
{
    for (auto root : rootCertMap) {
        if (HapCertVerifyOpensslUtils::X509NameCompare(X509_get_subject_name(root.second),
            X509_get_issuer_name(caCert)) &&
            HapCertVerifyOpensslUtils::CertVerify(caCert, root.second)) {
            return X509_dup(root.second);
        }
    }
    return nullptr;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
