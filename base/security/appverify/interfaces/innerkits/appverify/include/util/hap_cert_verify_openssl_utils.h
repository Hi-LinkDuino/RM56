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
#ifndef HAP_CERT_VERIFY_OPENSSL_UTILS_H
#define HAP_CERT_VERIFY_OPENSSL_UTILS_H

#include <string>
#include <unordered_map>

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "util/pkcs7_context.h"

namespace OHOS {
namespace Security {
namespace Verify {
using CertSign = std::unordered_map<X509*, bool>;

class HapCertVerifyOpensslUtils {
public:
    DLL_EXPORT static X509* GetX509CertFromPemString(const std::string& pemString);
    DLL_EXPORT static X509* GetX509CertFromBase64String(const std::string& base64String);
    DLL_EXPORT static X509_CRL* GetX509CrlFromDerBuffer(const HapByteBuffer& crlBuffer, int offset, int len);
    DLL_EXPORT static void GenerateCertSignFromCertStack(STACK_OF(X509)* certs, CertSign& certVisitSign);
    DLL_EXPORT static void ClearCertVisitSign(CertSign& certVisitSign);
    DLL_EXPORT static bool GetCertsChain(CertChain& certsChain, CertSign& certVisitSign);
    DLL_EXPORT static bool CertVerify(X509* cert, const X509* issuerCert);
    DLL_EXPORT static bool GetSubjectFromX509(const X509* cert, std::string& subject);
    DLL_EXPORT static bool GetIssuerFromX509(const X509* cert, std::string& issuer);
    DLL_EXPORT static bool GetSerialNumberFromX509(const X509* cert, long long& certNumber);
    DLL_EXPORT static bool GetIssuerFromX509Crl(const X509_CRL* crl, std::string& issuer);
    DLL_EXPORT static bool VerifyCertChainPeriodOfValidity(CertChain& certsChain, const ASN1_TYPE* signTime);
    DLL_EXPORT static bool VerifyCrl(CertChain& certsChain, STACK_OF(X509_CRL)* crls, Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool CompareX509Cert(const X509* certA, const std::string& base64Cert);
    DLL_EXPORT static void WriteX509CrlToStream(std::ofstream& crlFile, X509_CRL* crl);
    DLL_EXPORT static bool GetPublickeyBase64FromPemCert(const std::string& certStr, std::string& publicKey);
    DLL_EXPORT static bool X509NameCompare(const X509_NAME* a, const X509_NAME* b);
    DLL_EXPORT static bool GetPublickeyBase64(const X509* cert, std::string& publicKey);
    DLL_EXPORT static int CalculateLenAfterBase64Encode(int len);

private:
    DLL_EXPORT static X509* FindCertOfIssuer(X509* cert, CertSign& certVisitSign);
    DLL_EXPORT static std::string GetDnToString(X509_NAME* name);
    DLL_EXPORT static void GetTextFromX509Name(X509_NAME* name, int nId, std::string& text);
    DLL_EXPORT static X509_CRL* GetCrlBySignedCertIssuer(STACK_OF(X509_CRL)* crls, const X509* cert);
    DLL_EXPORT static bool CheckSignTimeInValidPeriod(const ASN1_TYPE* signTime,
        const ASN1_TIME* notBefore, const ASN1_TIME* notAfter);
    DLL_EXPORT static bool CheckAsn1TimeIsValid(const ASN1_TIME* asn1Time);
    DLL_EXPORT static bool CheckAsn1TypeIsValid(const ASN1_TYPE* asn1Type);

private:
    static const unsigned int MIN_CERT_CHAIN_LEN_NEED_VERIFY_CRL;
    static const int OPENSSL_READ_CRL_MAX_TIME;
    static const int OPENSSL_READ_CRL_LEN_EACH_TIME;
    static const int BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA;
    static const int BASE64_ENCODE_PACKET_LEN;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_CERT_VERIFY_OPENSSL_UTILS_H
