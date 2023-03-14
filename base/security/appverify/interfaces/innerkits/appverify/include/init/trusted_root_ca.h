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
#ifndef HAP_TRUSTED_ROOT_CA_H
#define HAP_TRUSTED_ROOT_CA_H

#include <string>
#include <unordered_map>

#include "openssl/x509.h"

#include "common/export_define.h"
#include "init/json_parser_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
using StringCertMap = std::unordered_map<std::string, X509*>;

class TrustedRootCa {
public:
    DLL_EXPORT static TrustedRootCa& GetInstance();
    DLL_EXPORT bool Init();
    DLL_EXPORT void Recovery();
    DLL_EXPORT bool EnableDebug();
    DLL_EXPORT void DisableDebug();
    DLL_EXPORT X509* FindMatchedRoot(X509* caCert);

private:
    TrustedRootCa();
    ~TrustedRootCa();

    /* Forbid external replication constructs and external replication */
    TrustedRootCa(const TrustedRootCa& trustedRoot) = delete;
    TrustedRootCa& operator = (const TrustedRootCa& trustedRoot) = delete;

    DLL_EXPORT bool GetTrustedRootCAFromJson(StringCertMap& rootCertMap, const std::string& filePath);
    X509* FindMatchedRoot(const StringCertMap& rootCertMap, X509* caCert);

private:
    static const std::string TRUSTED_ROOT_CA_FILE_PATH;
    static const std::string TRUSTED_ROOT_CA_TEST_FILE_PATH;
    StringCertMap rootCerts;
    StringCertMap rootCertsForTest;
    bool isInit;
    bool isDebug;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_TRUSTED_ROOT_CA_H
