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

#include "init/hap_crl_manager.h"

#include <fstream>

#include "common/hap_verify_log.h"
#include "common/random_access_file.h"
#include "util/hap_cert_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
#ifndef STANDARD_SYSTEM
const std::string HapCrlManager::HAP_CRL_FILE_PATH = "/data/sadata/hap_crl";
#else
const std::string HapCrlManager::HAP_CRL_FILE_PATH = "/data/hap_crl";
#endif

HapCrlManager& HapCrlManager::GetInstance()
{
    static HapCrlManager hapCrlManager;
    return hapCrlManager;
}

HapCrlManager::HapCrlManager() : crlMtx(), crlsMap(), isInit(false)
{
}

HapCrlManager::~HapCrlManager()
{
    for (auto crlPair : crlsMap) {
        X509_CRL_free(crlPair.second);
    }
    crlsMap.clear();
}

void HapCrlManager::Init()
{
    if (isInit) {
        return;
    }

    crlMtx.lock();
    HapByteBuffer crlsBuffer;
    bool ret = ReadCrls(crlsBuffer);
    if (ret && crlsBuffer.HasRemaining()) {
        ParseCrls(crlsBuffer);
        isInit = true;
    }
    crlMtx.unlock();
}

bool HapCrlManager::ParseCrls(HapByteBuffer& crlsBuffer)
{
    unsigned int numOfCrl;
    if (!crlsBuffer.GetUInt32(0, numOfCrl)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get numOfCrl failed");
        return false;
    }

    int hasUsed = static_cast<int>(sizeof(numOfCrl));
    crlsBuffer.SetPosition(hasUsed);
    HAPVERIFY_LOG_DEBUG(LABEL, "total crl num: %{public}u", numOfCrl);
    while (numOfCrl && hasUsed <= crlsBuffer.GetCapacity()) {
        int crlLen;
        if (!crlsBuffer.GetInt32(crlLen)) {
            HAPVERIFY_LOG_ERROR(LABEL, "get crlLen failed");
            return false;
        }
        hasUsed += static_cast<int>(sizeof(crlLen));
        X509_CRL* crl = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlsBuffer, hasUsed, crlLen);
        if (crl == nullptr) {
            HAPVERIFY_LOG_WARN(LABEL, "crl file is destroyed");
            return false;
        }

        std::string crlIssuer;
        if (!HapCertVerifyOpensslUtils::GetIssuerFromX509Crl(crl, crlIssuer)) {
            X509_CRL_free(crl);
            HAPVERIFY_LOG_WARN(LABEL, "GetIssuerFromX509Crl failed");
            return false;
        }

        if (crlsMap.find(crlIssuer) != crlsMap.end()) {
            X509_CRL_free(crlsMap[crlIssuer]);
        }
        crlsMap[crlIssuer] = crl;

        hasUsed += crlLen;
        crlsBuffer.SetPosition(hasUsed);
        HAPVERIFY_LOG_INFO(LABEL, "get %{public}ust crl's Issuer: %{public}s", numOfCrl, crlIssuer.c_str());
        numOfCrl--;
    }
    return true;
}

bool HapCrlManager::ReadCrls(HapByteBuffer& crlsBuffer)
{
    RandomAccessFile crlRandomAccess;
    crlRandomAccess.Init(HAP_CRL_FILE_PATH);
    long long fileLen = crlRandomAccess.GetLength();
    if (fileLen <= 0) {
        HAPVERIFY_LOG_WARN(LABEL, "crl fileLen: %{public}lld", fileLen);
        return true;
    }
    crlsBuffer.SetCapacity(fileLen);
    long long readLen = crlRandomAccess.ReadFileFullyFromOffset(crlsBuffer, 0);
    if (readLen != fileLen) {
        HAPVERIFY_LOG_ERROR(LABEL, "read file len: %{public}lld is not same as %{public}lld", readLen, fileLen);
        return false;
    }
    return true;
}

void HapCrlManager::WriteCrlsToFile()
{
    crlMtx.lock();
    std::ofstream crlFile(HAP_CRL_FILE_PATH, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    if (!crlFile.is_open()) {
        HAPVERIFY_LOG_ERROR(LABEL, "open %{public}s failed", HAP_CRL_FILE_PATH.c_str());
        crlMtx.unlock();
        return;
    }
    unsigned int numOfCrl = crlsMap.size();
    crlFile.write(reinterpret_cast<char*>(&numOfCrl), sizeof(numOfCrl));
    for (auto crlPair : crlsMap) {
        HapCertVerifyOpensslUtils::WriteX509CrlToStream(crlFile, crlPair.second);
    }
    HAPVERIFY_LOG_INFO(LABEL, "Write %{public}u crls to file done", numOfCrl);
    crlFile.close();
    crlMtx.unlock();
}

X509_CRL* HapCrlManager::GetCrlByIssuer(const std::string& issuer)
{
    if (crlsMap.find(issuer) == crlsMap.end()) {
        return nullptr;
    }
    return crlsMap[issuer];
}

void HapCrlManager::UpdateCrlByIssuer(const std::string& issuer, X509_CRL* crl)
{
    if (crl == nullptr) {
        return;
    }

    if (crlsMap.find(issuer) != crlsMap.end()) {
        X509_CRL_free(crlsMap[issuer]);
    }
    crlsMap[issuer] = X509_CRL_dup(crl);
}

bool HapCrlManager::CrlCheck(X509* cert, X509_CRL* targetCrl, Pkcs7Context& pkcs7Context)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input");
        return false;
    }

    crlMtx.lock();
    /* crl in package compare with local crl, and decide which one to use */
    targetCrl = GetFinalCrl(targetCrl, pkcs7Context);
    if (targetCrl == nullptr) {
        HAPVERIFY_LOG_INFO(LABEL, "no crl");
        crlMtx.unlock();
        return true;
    }
    X509_REVOKED* revoked = nullptr;
    if (X509_CRL_get0_by_cert(targetCrl, &revoked, cert) != 0) {
        long long certNumber;
        std::string certSuject;
        HapCertVerifyOpensslUtils::GetSerialNumberFromX509(cert, certNumber);
        HapCertVerifyOpensslUtils::GetSubjectFromX509(cert, certSuject);
        HAPVERIFY_LOG_ERROR(LABEL, "cert(issuer: %{public}s, subject: %{public}s, number:%{public}lld) is revoked",
            pkcs7Context.certIssuer.c_str(), certSuject.c_str(), certNumber);
        crlMtx.unlock();
        return false;
    }
    crlMtx.unlock();
    return true;
}

X509_CRL* HapCrlManager::GetFinalCrl(X509_CRL* crlInPackage, Pkcs7Context& pkcs7Context)
{
    X509_CRL* localCrl = GetCrlByIssuer(pkcs7Context.certIssuer);
    /* if no crl in package, use local crl */
    if (crlInPackage == nullptr) {
        return localCrl;
    }

    /* if no local crl, use crl in package */
    if (localCrl == nullptr) {
        /* no local crl, update crl in package to local */
        UpdateCrlByIssuer(pkcs7Context.certIssuer, crlInPackage);
        pkcs7Context.needWriteCrl = true;
        return crlInPackage;
    }

    /* if both are present, use which one recently updated */
    const ASN1_TIME* localCrlUpdateTime = X509_CRL_get0_lastUpdate(localCrl);
    const ASN1_TIME* packageCrlUpdateTime = X509_CRL_get0_lastUpdate(crlInPackage);
    if (localCrlUpdateTime == nullptr || packageCrlUpdateTime == nullptr) {
        HAPVERIFY_LOG_INFO(LABEL, "crl no update time");
        return nullptr;
    }
    if (ASN1_TIME_compare(localCrlUpdateTime, packageCrlUpdateTime) >= 0) {
        return localCrl;
    }
    /* if crl in package is newer version, use it to update local crl */
    UpdateCrlByIssuer(pkcs7Context.certIssuer, crlInPackage);
    pkcs7Context.needWriteCrl = true;
    return crlInPackage;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
