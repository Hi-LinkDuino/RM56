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

#include "util/hap_cert_verify_openssl_utils.h"

#include <cmath>
#include <fstream>

#include "openssl/pem.h"

#include "common/hap_verify_log.h"
#include "init/hap_crl_manager.h"
#include "init/trusted_root_ca.h"
#include "util/hap_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
const unsigned int HapCertVerifyOpensslUtils::MIN_CERT_CHAIN_LEN_NEED_VERIFY_CRL = 2;
const int HapCertVerifyOpensslUtils::OPENSSL_READ_CRL_MAX_TIME = 1048576; // 1024 * 1024
const int HapCertVerifyOpensslUtils::OPENSSL_READ_CRL_LEN_EACH_TIME = 1024;
const int HapCertVerifyOpensslUtils::BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA = 4;
const int HapCertVerifyOpensslUtils::BASE64_ENCODE_PACKET_LEN = 3;

X509* HapCertVerifyOpensslUtils::GetX509CertFromPemString(const std::string& pemString)
{
    BIO* pemBio = BIO_new(BIO_s_mem());
    if (pemBio == nullptr) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "BIO_new failed");
        return nullptr;
    }
    int strLen = static_cast<int>(pemString.size());
    if (BIO_write(pemBio, pemString.c_str(), strLen) != strLen) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "BIO_write failed");
        BIO_free_all(pemBio);
        return nullptr;
    }
    X509* cert = PEM_read_bio_X509(pemBio, nullptr, nullptr, nullptr);
    BIO_free_all(pemBio);
    return cert;
}

X509* HapCertVerifyOpensslUtils::GetX509CertFromBase64String(const std::string& base64String)
{
    std::unique_ptr<unsigned char[]> decodeBuffer = std::make_unique<unsigned char[]>(base64String.size());
    if (decodeBuffer == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "make_unique failed");
        return nullptr;
    }

    const unsigned char* input = reinterpret_cast<const unsigned char *>(base64String.c_str());
    int len = EVP_DecodeBlock(decodeBuffer.get(), input, base64String.size());
    if (len <= 0) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "base64Decode failed, len: %{public}d", len);
        return nullptr;
    }

    const unsigned char* derBits = decodeBuffer.get();
    X509* cert = d2i_X509(nullptr, &derBits, len);
    return cert;
}

bool HapCertVerifyOpensslUtils::GetPublickeyBase64FromPemCert(const std::string& certStr, std::string& publicKey)
{
    X509* cert = GetX509CertFromPemString(certStr);
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetX509CertFromPemString failed");
        return false;
    }

    if (!GetPublickeyBase64(cert, publicKey)) {
        HAPVERIFY_LOG_ERROR(LABEL, "X509_get_pubkey failed");
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        X509_free(cert);
        return false;
    }
    X509_free(cert);
    return true;
}

bool HapCertVerifyOpensslUtils::GetPublickeyBase64(const X509* cert, std::string& publicKey)
{
    EVP_PKEY *pkey = X509_get0_pubkey(cert);
    if (pkey == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "X509_get0_pubkey failed");
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        return false;
    }

    int keyLen = i2d_PublicKey(pkey, nullptr);
    if (keyLen <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "keyLen %{public}d, i2d_PublicKey failed", keyLen);
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        return false;
    }
    std::unique_ptr<unsigned char[]> derPublicKey = std::make_unique<unsigned char[]>(keyLen);
    int base64KeyLen = CalculateLenAfterBase64Encode(keyLen);
    std::unique_ptr<unsigned char[]> base64PublicKey = std::make_unique<unsigned char[]>(base64KeyLen);
    if (derPublicKey == nullptr || base64PublicKey == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "make_unique failed");
        return false;
    }
    unsigned char* derCertificateBackup = derPublicKey.get();
    if (i2d_PublicKey(pkey, &derCertificateBackup) <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "i2d_PublicKey failed");
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        return false;
    }

    int outLen = EVP_EncodeBlock(base64PublicKey.get(), derPublicKey.get(), keyLen);
    publicKey = std::string(reinterpret_cast<char*>(base64PublicKey.get()), outLen);
    return true;
}

/*
 * The length after Base64 encoding is 4/3 of the length before encoding,
 * and openssl function will add '\0' to the encoded string.
 * So len_after_encode = len_before_encode * 4/3 + 1
 */
int HapCertVerifyOpensslUtils::CalculateLenAfterBase64Encode(int len)
{
    return static_cast<int>(ceil(static_cast<long double>(len) / BASE64_ENCODE_PACKET_LEN) *
        BASE64_ENCODE_LEN_OF_EACH_GROUP_DATA + 1);
}

bool HapCertVerifyOpensslUtils::CompareX509Cert(const X509* certA, const std::string& base64Cert)
{
    if (certA == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "certA is nullptr");
        return false;
    }

    X509* certB = GetX509CertFromPemString(base64Cert);
    if (certB == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "generate certB failed");
        return false;
    }
    bool ret = (X509_cmp(certA, certB) == 0);
    X509_free(certB);
    return ret;
}

X509_CRL* HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(const HapByteBuffer& crlBuffer, int offset, int len)
{
    if (crlBuffer.GetBufferPtr() == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input, crlbuffer is null");
        return nullptr;
    }
    if ((len <= 0) || (offset < 0) || (crlBuffer.GetCapacity() - len < offset)) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input, offset: %{public}d, len: %{public}d", offset, len);
        return nullptr;
    }

    BIO* derBio = BIO_new(BIO_s_mem());
    if (derBio == nullptr) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "BIO_new failed");
        return nullptr;
    }
    if (BIO_write(derBio, crlBuffer.GetBufferPtr() + offset, len) != len) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "BIO_write failed");
        BIO_free_all(derBio);
        return nullptr;
    }
    X509_CRL* crl = d2i_X509_CRL_bio(derBio, nullptr);
    BIO_free_all(derBio);
    return crl;
}

void HapCertVerifyOpensslUtils::WriteX509CrlToStream(std::ofstream& crlFile, X509_CRL* crl)
{
    if (!crlFile.is_open()) {
        HAPVERIFY_LOG_ERROR(LABEL, "fill is not open");
        return;
    }

    BIO* derBio = BIO_new(BIO_s_mem());
    if (derBio == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "BIO_new failed");
        return;
    }
    if (crl == nullptr || i2d_X509_CRL_bio(derBio, crl) == 0) {
        BIO_free_all(derBio);
        HAPVERIFY_LOG_ERROR(LABEL, "i2d_X509_CRL_bio failed");
        return;
    }

    int totalLen = 0;
    long long posStart = crlFile.tellp();
    crlFile.seekp(posStart + sizeof(totalLen));
    char buf[OPENSSL_READ_CRL_LEN_EACH_TIME];
    int readLen = BIO_read(derBio, buf, sizeof(buf));
    int readTime = 0;
    while (readLen > 0 && (++readTime < OPENSSL_READ_CRL_MAX_TIME)) {
        crlFile.write(buf, readLen);
        totalLen += readLen;
        readLen = BIO_read(derBio, buf, sizeof(buf));
    }
    BIO_free_all(derBio);
    long long posEnd = crlFile.tellp();
    crlFile.seekp(posStart);
    /* write crl data len */
    crlFile.write(reinterpret_cast<char*>(&totalLen), sizeof(totalLen));
    crlFile.seekp(posEnd);
}

void HapCertVerifyOpensslUtils::GenerateCertSignFromCertStack(STACK_OF(X509)* certs, CertSign& certVisitSign)
{
    if (certs == nullptr) {
        return;
    }

    for (int i = 0; i < sk_X509_num(certs); i++) {
        X509* cert = sk_X509_value(certs, i);
        if (cert == nullptr) {
            continue;
        }
        certVisitSign[cert] = false;
    }
}

void HapCertVerifyOpensslUtils::ClearCertVisitSign(CertSign& certVisitSign)
{
    for (auto& certPair : certVisitSign) {
        certPair.second = false;
    }
}

bool HapCertVerifyOpensslUtils::GetCertsChain(CertChain& certsChain, CertSign& certVisitSign)
{
    if (certsChain.empty() || certVisitSign.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "input is invalid");
        return false;
    }

    X509* issuerCert;
    X509* cert = certsChain[0];
    while ((issuerCert = FindCertOfIssuer(cert, certVisitSign)) != nullptr) {
        certsChain.push_back(X509_dup(issuerCert));
        certVisitSign[issuerCert] = true;
        cert = issuerCert;
    }

    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    issuerCert = rootCertsObj.FindMatchedRoot(certsChain[certsChain.size() - 1]);
    if (issuerCert == nullptr) {
        std::string caIssuer;
        GetIssuerFromX509(certsChain[certsChain.size() - 1], caIssuer);
        HAPVERIFY_LOG_ERROR(LABEL, "it do not come from trusted root, issuer: %{public}s", caIssuer.c_str());
        return false;
    }
    if (X509_cmp(issuerCert, certsChain[certsChain.size() - 1]) == 0) {
        X509_free(issuerCert);
    } else {
        certsChain.push_back(issuerCert);
    }
    return true;
}

X509* HapCertVerifyOpensslUtils::FindCertOfIssuer(X509* cert, CertSign& certVisitSign)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "input is invalid");
        return nullptr;
    }

    X509_NAME* signCertIssuer = X509_get_issuer_name(cert);
    for (auto certPair : certVisitSign) {
        if (certPair.second) {
            continue;
        }

        X509* issuerCert = certPair.first;
        X509_NAME* issuerCertSubject = X509_get_subject_name(issuerCert);
        /* verify sign and issuer */
        if (X509NameCompare(issuerCertSubject, signCertIssuer) &&
            CertVerify(cert, issuerCert)) {
            return issuerCert;
        }
    }
    return nullptr;
}

bool HapCertVerifyOpensslUtils::CertVerify(X509* cert, const X509* issuerCert)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "input is invalid");
        return false;
    }

    EVP_PKEY* caPublicKey = X509_get0_pubkey(issuerCert);
    if (caPublicKey == nullptr) {
        HapVerifyOpensslUtils::GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "get pubkey from caCert failed");
        return false;
    }
    return X509_verify(cert, caPublicKey) > 0;
}

bool HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(CertChain& certsChain, const ASN1_TYPE* signTime)
{
    if (certsChain.empty()) {
        return false;
    }

    for (unsigned int i = 0; i < certsChain.size() - 1; i++) {
        if (certsChain[i] == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "%{public}dst cert is nullptr", i);
            return false;
        }
        const ASN1_TIME* notBefore = X509_get0_notBefore(certsChain[i]);
        const ASN1_TIME* notAfter = X509_get0_notAfter(certsChain[i]);
        if (!CheckSignTimeInValidPeriod(signTime, notBefore, notAfter)) {
            HAPVERIFY_LOG_ERROR(LABEL, "%{public}dst cert is not in period of validity", i);
            return false;
        }
    }
    return true;
}

bool HapCertVerifyOpensslUtils::CheckAsn1TimeIsValid(const ASN1_TIME* asn1Time)
{
    if (asn1Time == nullptr || asn1Time->data == nullptr) {
        return false;
    }
    return true;
}

bool HapCertVerifyOpensslUtils::CheckAsn1TypeIsValid(const ASN1_TYPE* asn1Type)
{
    if (asn1Type == nullptr || asn1Type->value.asn1_string == nullptr ||
        asn1Type->value.asn1_string->data == nullptr) {
        return false;
    }
    return true;
}

bool HapCertVerifyOpensslUtils::CheckSignTimeInValidPeriod(const ASN1_TYPE* signTime,
    const ASN1_TIME* notBefore, const ASN1_TIME* notAfter)
{
    if (!CheckAsn1TimeIsValid(notBefore) || !CheckAsn1TimeIsValid(notAfter)) {
        HAPVERIFY_LOG_ERROR(LABEL, "no valid period");
        return false;
    }
    if (!CheckAsn1TypeIsValid(signTime)) {
        HAPVERIFY_LOG_ERROR(LABEL, "signTime is invalid");
        return false;
    }

    if (ASN1_TIME_compare(notBefore, signTime->value.asn1_string) > 0 ||
        ASN1_TIME_compare(notAfter, signTime->value.asn1_string) < 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "Out of valid period, signTime: %{public}s, notBefore: %{public}s, "
            "notAfter: %{public}s", signTime->value.asn1_string->data, notBefore->data, notAfter->data);
        return false;
    }
    HAPVERIFY_LOG_DEBUG(LABEL, "signTime type: %{public}d, data: %{public}s, notBefore: %{public}s, "
        "notAfter: %{public}s", signTime->type, signTime->value.asn1_string->data, notBefore->data, notAfter->data);
    return true;
}

bool HapCertVerifyOpensslUtils::VerifyCrl(CertChain& certsChain, STACK_OF(X509_CRL)* crls, Pkcs7Context& pkcs7Context)
{
    if (certsChain.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "cert chain is null");
        return false;
    }

    /* get signed cert's issuer and then it will be used to find local crl */
    if (!GetIssuerFromX509(certsChain[0], pkcs7Context.certIssuer)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get issuer of signed cert failed");
        return false;
    }
    X509_CRL* targetCrl = GetCrlBySignedCertIssuer(crls, certsChain[0]);
    /* crl is optional */
    if (targetCrl != nullptr && certsChain.size() >= MIN_CERT_CHAIN_LEN_NEED_VERIFY_CRL) {
        /* if it include crl, it must be verified by ca cert */
        if (X509_CRL_verify(targetCrl, X509_get0_pubkey(certsChain[1])) <= 0) {
            HapVerifyOpensslUtils::GetOpensslErrorMessage();
            HAPVERIFY_LOG_ERROR(LABEL, "verify crlInPackage failed");
            return false;
        }
    }
    HapCrlManager& hapCrlManager = HapCrlManager::GetInstance();
    return hapCrlManager.CrlCheck(certsChain[0], targetCrl, pkcs7Context);
}

X509_CRL* HapCertVerifyOpensslUtils::GetCrlBySignedCertIssuer(STACK_OF(X509_CRL)* crls, const X509* cert)
{
    if (crls == nullptr || cert == nullptr) {
        return nullptr;
    }

    X509_NAME* certIssuer = X509_get_issuer_name(cert);
    for (int i = 0; i < sk_X509_CRL_num(crls); i++) {
        X509_CRL* crl = sk_X509_CRL_value(crls, i);
        if (crl == nullptr) {
            continue;
        }

        X509_NAME* crlIssuer = X509_CRL_get_issuer(crl);
        if (X509NameCompare(crlIssuer, certIssuer)) {
            return crl;
        }
    }
    return nullptr;
}

bool HapCertVerifyOpensslUtils::X509NameCompare(const X509_NAME* a, const X509_NAME* b)
{
    if (a == nullptr || b == nullptr) {
        return false;
    }

    return X509_NAME_cmp(a, b) == 0;
}

bool HapCertVerifyOpensslUtils::GetSubjectFromX509(const X509* cert, std::string& subject)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "cert is nullptr");
        return false;
    }

    X509_NAME* name = X509_get_subject_name(cert);
    subject = GetDnToString(name);
    HAPVERIFY_LOG_DEBUG(LABEL, "subject = %{public}s", subject.c_str());
    return true;
}

bool HapCertVerifyOpensslUtils::GetIssuerFromX509(const X509* cert, std::string& issuer)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "cert is nullptr");
        return false;
    }

    X509_NAME* name = X509_get_issuer_name(cert);
    issuer = GetDnToString(name);
    HAPVERIFY_LOG_DEBUG(LABEL, "cert issuer = %{public}s", issuer.c_str());
    return true;
}

bool HapCertVerifyOpensslUtils::GetSerialNumberFromX509(const X509* cert, long long& certNumber)
{
    if (cert == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "cert is nullptr");
        return false;
    }

    const ASN1_INTEGER* certSN = X509_get0_serialNumber(cert);
    certNumber = ASN1_INTEGER_get(certSN);
    HAPVERIFY_LOG_DEBUG(LABEL, "cert number = %{public}lld", certNumber);
    return true;
}

bool HapCertVerifyOpensslUtils::GetIssuerFromX509Crl(const X509_CRL* crl, std::string& issuer)
{
    if (crl == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "clr is nullptr");
        return false;
    }

    X509_NAME* name = X509_CRL_get_issuer(crl);
    if (name == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "crl issuer nullptr");
        return false;
    }
    issuer = GetDnToString(name);
    return true;
}

std::string HapCertVerifyOpensslUtils::GetDnToString(X509_NAME* name)
{
    if (name == nullptr) {
        return "";
    }

    std::string countryName;
    GetTextFromX509Name(name, NID_countryName, countryName);
    std::string organizationName;
    GetTextFromX509Name(name, NID_organizationName, organizationName);
    std::string organizationalUnitName;
    GetTextFromX509Name(name, NID_organizationalUnitName, organizationalUnitName);
    std::string commonName;
    GetTextFromX509Name(name, NID_commonName, commonName);
    return "C=" + countryName + ", O=" + organizationName + ", OU=" + organizationalUnitName +
        ", CN=" + commonName;
}

void HapCertVerifyOpensslUtils::GetTextFromX509Name(X509_NAME* name, int nId, std::string& text)
{
    int textLen = X509_NAME_get_text_by_NID(name, nId, nullptr, 0);
    if (textLen <= 0) {
        return;
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(textLen + 1);
    if (buffer == nullptr) {
        return;
    }

    if (X509_NAME_get_text_by_NID(name, nId, buffer.get(), textLen + 1) != textLen) {
        return;
    }
    text = std::string(buffer.get());
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
