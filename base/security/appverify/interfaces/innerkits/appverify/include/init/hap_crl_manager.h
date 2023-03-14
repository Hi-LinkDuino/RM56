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
#ifndef HAP_CRL_MANAGER_H
#define HAP_CRL_MANAGER_H

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

#include "openssl/x509.h"

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "util/pkcs7_context.h"

namespace OHOS {
namespace Security {
namespace Verify {
using IssuerCrlMap = std::unordered_map<std::string, X509_CRL*>;

class HapCrlManager {
public:
    DLL_EXPORT static HapCrlManager& GetInstance();
    DLL_EXPORT void Init();
    DLL_EXPORT bool CrlCheck(X509* cert, X509_CRL* targetCrl, Pkcs7Context& pkcs7Context);
    DLL_EXPORT void WriteCrlsToFile();

private:
    HapCrlManager();
    ~HapCrlManager();

    /* Forbid external replication constructs and external replication */
    HapCrlManager(const HapCrlManager& hapCrlManager) = delete;
    HapCrlManager& operator = (const HapCrlManager& hapCrlManager) = delete;

    DLL_EXPORT X509_CRL* GetFinalCrl(X509_CRL* crlInPackage, Pkcs7Context& pkcs7Context);
    DLL_EXPORT X509_CRL* GetCrlByIssuer(const std::string& issuer);
    DLL_EXPORT void UpdateCrlByIssuer(const std::string& issuer, X509_CRL* crl);
    DLL_EXPORT bool ReadCrls(HapByteBuffer& crlsBuffer);
    DLL_EXPORT bool ParseCrls(HapByteBuffer& crlsBuffer);

private:
    DLL_EXPORT static const std::string HAP_CRL_FILE_PATH;
    std::mutex crlMtx;
    IssuerCrlMap crlsMap;
    bool isInit;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_CRL_MANAGER_H
