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

#include "util/hap_verify_openssl_utils.h"

#include "openssl/err.h"
#include "openssl/pem.h"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
using Pkcs7SignerInfoStack = STACK_OF(PKCS7_SIGNER_INFO);
using X509AttributeStack = STACK_OF(X509_ATTRIBUTE);

const int HapVerifyOpensslUtils::OPENSSL_PKCS7_VERIFY_SUCCESS = 1;
const int HapVerifyOpensslUtils::OPENSSL_ERR_MESSAGE_MAX_LEN = 1024;

/*
 * OPENSSL_READ_DATA_MAX_TIME * OPENSSL_READ_DATA_LEN_EACH_TIME < 2GBytes.
 * make the maximum size of data that can be read each time be 1 KBytes,
 * so the maximum times of read data is 1024 * 1024 * 2 = 2097152;
 */
const int HapVerifyOpensslUtils::OPENSSL_READ_DATA_MAX_TIME = 2097152;
const int HapVerifyOpensslUtils::OPENSSL_READ_DATA_LEN_EACH_TIME = 1024;

/* Signature algorithm OID for extended PKCS7 */
const std::string HapVerifyOpensslUtils::PKCS7_EXT_SHAWITHRSA_PSS = "1.2.840.113549.1.1.10";
const int HapVerifyOpensslUtils::MAX_OID_LENGTH = 128;

bool HapVerifyOpensslUtils::ParsePkcs7Package(const unsigned char packageData[],
    unsigned int packageLen, Pkcs7Context& pkcs7Context)
{
    if (packageData == nullptr || packageLen == 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input");
        return false;
    }

    pkcs7Context.p7 = d2i_PKCS7(nullptr, &packageData, packageLen);
    if (!CheckPkcs7SignedDataIsValid(pkcs7Context.p7)) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "p7 is invalid");
        return false;
    }
    if (!GetContentInfo(pkcs7Context.p7->d.sign->contents, pkcs7Context.content)) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get content from pkcs7 failed");
        return false;
    }
    return true;
}

bool HapVerifyOpensslUtils::GetCertChains(PKCS7* p7, Pkcs7Context& pkcs7Context)
{
    if (!CheckPkcs7SignedDataIsValid(p7)) {
        HAPVERIFY_LOG_ERROR(LABEL, "p7 is invalid");
        return false;
    }

    CertSign certVisitSign;
    HapCertVerifyOpensslUtils::GenerateCertSignFromCertStack(p7->d.sign->cert, certVisitSign);

    Pkcs7SignerInfoStack* signerInfoStack = PKCS7_get_signer_info(p7);
    if (signerInfoStack == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "get signerInfoStack error");
        GetOpensslErrorMessage();
        return false;
    }
    int signCount = sk_PKCS7_SIGNER_INFO_num(signerInfoStack);
    if (signCount <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "can not find signinfo");
        return false;
    }

    for (int i = 0; i < signCount; i++) {
        /* get ith signInfo */
        PKCS7_SIGNER_INFO* signInfo = sk_PKCS7_SIGNER_INFO_value(signerInfoStack, i);
        if (signInfo == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "signInfo %{public}dst is nullptr", i);
            return false;
        }
        /* GET X509 certificate */
        X509* cert = PKCS7_cert_from_signer_info(p7, signInfo);
        if (cert == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "get cert for %{public}dst signInfo failed", i);
            return false;
        }
        CertChain certChain;
        pkcs7Context.certChains.push_back(certChain);
        pkcs7Context.certChains[i].push_back(X509_dup(cert));
        HapCertVerifyOpensslUtils::ClearCertVisitSign(certVisitSign);
        certVisitSign[cert] = true;
        if (!VerifyCertChain(pkcs7Context.certChains[i], p7, signInfo, pkcs7Context, certVisitSign)) {
            HAPVERIFY_LOG_ERROR(LABEL, "verify %{public}dst certchain failed", i);
            return false;
        }
    }
    return true;
}

bool HapVerifyOpensslUtils::VerifyCertChain(CertChain& certsChain, PKCS7* p7,
    PKCS7_SIGNER_INFO* signInfo, Pkcs7Context& pkcs7Context, CertSign& certVisitSign)
{
    if (!HapCertVerifyOpensslUtils::GetCertsChain(certsChain, certVisitSign)) {
        HAPVERIFY_LOG_ERROR(LABEL, "get cert chain for signInfo failed");
        return false;
    }
    ASN1_TYPE* signTime = PKCS7_get_signed_attribute(signInfo, NID_pkcs9_signingTime);
    if (!HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(certsChain, signTime)) {
        HAPVERIFY_LOG_ERROR(LABEL, "VerifyCertChainPeriodOfValidity for signInfo failed");
        return false;
    }
    if (!HapCertVerifyOpensslUtils::VerifyCrl(certsChain, p7->d.sign->crl, pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "VerifyCrl for signInfo failed");
        return false;
    }
    return true;
}

bool HapVerifyOpensslUtils::CheckPkcs7SignedDataIsValid(const PKCS7* p7)
{
    if (p7 == nullptr || !PKCS7_type_is_signed(p7) || p7->d.sign == nullptr) {
        return false;
    }
    return true;
}

bool HapVerifyOpensslUtils::VerifyPkcs7(Pkcs7Context& pkcs7Context)
{
    if (!CheckPkcs7SignedDataIsValid(pkcs7Context.p7)) {
        HAPVERIFY_LOG_ERROR(LABEL, "p7 type is invalid signed_data_pkcs7");
        return false;
    }

    if (!VerifyPkcs7SignedData(pkcs7Context)) {
        HAPVERIFY_LOG_ERROR(LABEL, "verify p7 error");
        return false;
    }
    return true;
}

bool HapVerifyOpensslUtils::VerifyPkcs7SignedData(Pkcs7Context& pkcs7Context)
{
    /* get signed data which was used to be signed */
    BIO* p7Bio = PKCS7_dataDecode(pkcs7Context.p7, nullptr, nullptr, nullptr);
    if (p7Bio == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "get p7bio error");
        GetOpensslErrorMessage();
        return false;
    }
    char buf[OPENSSL_READ_DATA_LEN_EACH_TIME] = {0};
    int readLen = BIO_read(p7Bio, buf, sizeof(buf));
    int readTime = 0;
    while ((readLen > 0) && (++readTime < OPENSSL_READ_DATA_MAX_TIME)) {
        readLen = BIO_read(p7Bio, buf, sizeof(buf));
    }
    Pkcs7SignerInfoStack* signerInfoStack = PKCS7_get_signer_info(pkcs7Context.p7);
    if (signerInfoStack == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "get signerInfoStack error");
        BIO_free_all(p7Bio);
        GetOpensslErrorMessage();
        return false;
    }
    /* get the num of signInfo */
    int signCount = sk_PKCS7_SIGNER_INFO_num(signerInfoStack);
    if (signCount <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "can not find signinfo");
        BIO_free_all(p7Bio);
        return false;
    }
    for (int i = 0; i < signCount; i++) {
        if (!VerifySignInfo(signerInfoStack, p7Bio, i, pkcs7Context)) {
            HAPVERIFY_LOG_ERROR(LABEL, "Verify %{public}dst signInfo failed", i);
            BIO_free_all(p7Bio);
            return false;
        }
    }
    BIO_free_all(p7Bio);
    return true;
}

bool HapVerifyOpensslUtils::VerifySignInfo(STACK_OF(PKCS7_SIGNER_INFO)* signerInfoStack,
    BIO* p7Bio, int signInfoNum, Pkcs7Context& pkcs7Context) {
    if (signerInfoStack == nullptr || p7Bio == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "invalid input");
        return false;
    }
    /* get signInfo */
    PKCS7_SIGNER_INFO* signInfo = sk_PKCS7_SIGNER_INFO_value(signerInfoStack, signInfoNum);
    if (signInfo == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "signInfo %{public}dst is nullptr", signInfoNum);
        return false;
    }
    /* GET X509 certificate */
    X509* cert = pkcs7Context.certChains[signInfoNum][0];
    bool isShaWithRsaPss = IsEnablePss(signInfo);
    if (isShaWithRsaPss) {
        EVP_PKEY* pkey = X509_get0_pubkey(cert);
        if (pkey == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "signInfo %{public}dst X509_get_pubkey failed", signInfoNum);
            return false;
        }
        HAPVERIFY_LOG_DEBUG(LABEL, "use RSA/pss");
        if (!VerifyShaWithRsaPss(signInfo, p7Bio, pkey, isShaWithRsaPss)) {
            HAPVERIFY_LOG_ERROR(LABEL, "VerifyShaWithRsaPss %{public}dst signInfo failed", signInfoNum);
            return false;
        }
    } else {
        if (PKCS7_signatureVerify(p7Bio, pkcs7Context.p7, signInfo, cert) <= 0) {
            HAPVERIFY_LOG_ERROR(LABEL, "PKCS7_signatureVerify %{public}dst signInfo failed", signInfoNum);
            GetOpensslErrorMessage();
            return false;
        }
    }
    return true;
}

bool HapVerifyOpensslUtils::IsEnablePss(const PKCS7_SIGNER_INFO* signInfo)
{
    char oId[MAX_OID_LENGTH];
    if (signInfo->digest_enc_alg == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "signInfo->digest_enc_alg is nullptr");
        return false;
    }
    int len = OBJ_obj2txt(oId, sizeof(oId), signInfo->digest_enc_alg->algorithm, 1);
    if (len < 0 || len >= MAX_OID_LENGTH) {
        HAPVERIFY_LOG_ERROR(LABEL, "Get length of oId failed");
        return false;
    }
    return PKCS7_EXT_SHAWITHRSA_PSS.compare(0, PKCS7_EXT_SHAWITHRSA_PSS.size(), oId, len) == 0;
}

bool HapVerifyOpensslUtils::VerifyShaWithRsaPss(const PKCS7_SIGNER_INFO* signInfo,
    BIO* p7Bio, EVP_PKEY* pkey, bool isPss)
{
    if (signInfo->digest_alg == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "signInfo->digest_alg is nullptr");
        return false;
    }
    int mdType = OBJ_obj2nid(signInfo->digest_alg->algorithm);
    const EVP_MD_CTX* mdCtx = FindMdCtxInBio(p7Bio, mdType);
    EVP_MD_CTX* mdCtxTmp = EVP_MD_CTX_new();
    if (mdCtxTmp == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_MD_CTX_new failed");
        return false;
    }
    if (!EVP_MD_CTX_copy_ex(mdCtxTmp, mdCtx)) {
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_MD_CTX_copy_ex failed");
        EVP_MD_CTX_free(mdCtxTmp);
        return false;
    }
    if (!VerifyPkcs7AuthAttributes(signInfo, mdCtxTmp, mdType)) {
        HAPVERIFY_LOG_ERROR(LABEL, "VerifyPkcs7AuthAttributes failed");
        EVP_MD_CTX_free(mdCtxTmp);
        return false;
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen;
    if (EVP_DigestFinal_ex(mdCtxTmp, digest, &digestLen) <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "Digest content failed");
        GetOpensslErrorMessage();
        EVP_MD_CTX_free(mdCtxTmp);
        return false;
    }
    EVP_MD_CTX_free(mdCtxTmp);

    if (!VerifyShaWithRsaPss(signInfo, pkey, isPss, digest, digestLen)) {
        HAPVERIFY_LOG_ERROR(LABEL, "VerifyShaWithRsaPss failed");
        GetOpensslErrorMessage();
        return false;
    }
    return true;
}

const EVP_MD_CTX* HapVerifyOpensslUtils::FindMdCtxInBio(BIO* p7Bio, int mdType)
{
    EVP_MD_CTX* mdCtx = nullptr;
    while (p7Bio) {
        BIO_get_md_ctx(p7Bio, &mdCtx);
        if (mdCtx == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "Get null from bio");
            return nullptr;
        }
        if ((EVP_MD_CTX_type(mdCtx) == mdType) || (EVP_MD_pkey_type(EVP_MD_CTX_md(mdCtx)) == mdType)) {
            break;
        }
        p7Bio = BIO_next(p7Bio);
    }
    return mdCtx;
}

bool HapVerifyOpensslUtils::VerifyPkcs7AuthAttributes(const PKCS7_SIGNER_INFO* signInfo, EVP_MD_CTX* mdCtx, int mdType)
{
    X509AttributeStack* authAttributes = signInfo->auth_attr;
    if ((authAttributes != nullptr) && (sk_X509_ATTRIBUTE_num(authAttributes) != 0)) {
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digestLen;
        if (EVP_DigestFinal_ex(mdCtx, digest, &digestLen) <= 0) {
            HAPVERIFY_LOG_ERROR(LABEL, "Digest content failed");
            GetOpensslErrorMessage();
            return false;
        }
        ASN1_OCTET_STRING* digestInAttribute = PKCS7_digest_from_attributes(authAttributes);
        if (!AsnStringCmp(digestInAttribute, digest, static_cast<int>(digestLen))) {
            HAPVERIFY_LOG_ERROR(LABEL, "AsnStringCmp failed");
            return false;
        }

        if (!EVP_VerifyInit_ex(mdCtx, EVP_get_digestbynid(mdType), nullptr)) {
            HAPVERIFY_LOG_ERROR(LABEL, "EVP_VerifyInit_ex failed");
            GetOpensslErrorMessage();
            return false;
        }

        unsigned char* attributesData = nullptr;
        int attributesLen = ASN1_item_i2d(reinterpret_cast<ASN1_VALUE*>(authAttributes), &attributesData,
            ASN1_ITEM_rptr(PKCS7_ATTR_VERIFY));
        if (attributesLen <= 0 || attributesData == nullptr) {
            HAPVERIFY_LOG_ERROR(LABEL, "ASN1_item_i2d failed");
            GetOpensslErrorMessage();
            return false;
        }
        if (!EVP_VerifyUpdate(mdCtx, attributesData, attributesLen)) {
            HAPVERIFY_LOG_ERROR(LABEL, "EVP_VerifyUpdate failed");
            GetOpensslErrorMessage();
            OPENSSL_free(attributesData);
            return false;
        }
        OPENSSL_free(attributesData);
    }
    return true;
}

bool HapVerifyOpensslUtils::AsnStringCmp(const ASN1_OCTET_STRING* asnStr, const unsigned char data[], int len)
{
    if (asnStr == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "asnStr is nullptr");
        return false;
    }
    if (asnStr->data == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "asnStr->data is nullptr");
        return false;
    }
    if (data == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "data is nullptr");
        return false;
    }
    if (asnStr->length != len) {
        HAPVERIFY_LOG_ERROR(LABEL, "asnStr->length: %{public}d is not equal to len: %{public}d", asnStr->length, len);
        return false;
    }
    for (int i = 0; i < len; i++) {
        if (asnStr->data[i] != data[i]) {
            HAPVERIFY_LOG_ERROR(LABEL, "%{public}dst data is not equal", i);
            return false;
        }
    }
    return true;
}

bool HapVerifyOpensslUtils::VerifyShaWithRsaPss(const PKCS7_SIGNER_INFO* signInfo, EVP_PKEY* pkey, bool isPss,
    const unsigned char digest[], unsigned int digestLen)
{
    EVP_PKEY_CTX* pkeyCtx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (pkeyCtx == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_PKEY_CTX_new failed");
        GetOpensslErrorMessage();
        return false;
    }
    if (EVP_PKEY_verify_init(pkeyCtx) <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_PKEY_verify_init failed");
        GetOpensslErrorMessage();
        EVP_PKEY_CTX_free(pkeyCtx);
        return false;
    }
    if (signInfo->digest_alg == nullptr || signInfo->enc_digest == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "no digest_alg or enc_digest in signInfo");
        EVP_PKEY_CTX_free(pkeyCtx);
        return false;
    }
    int mdType = OBJ_obj2nid(signInfo->digest_alg->algorithm);
    if ((isPss && EVP_PKEY_CTX_set_rsa_padding(pkeyCtx, RSA_PKCS1_PSS_PADDING) <= 0) ||
        (EVP_PKEY_CTX_set_signature_md(pkeyCtx, EVP_get_digestbynid(mdType)) <= 0)) {
        HAPVERIFY_LOG_ERROR(LABEL, "set rsa_padding or signature_md failed");
        GetOpensslErrorMessage();
        EVP_PKEY_CTX_free(pkeyCtx);
        return false;
    }
    if (EVP_PKEY_verify(pkeyCtx, signInfo->enc_digest->data, signInfo->enc_digest->length, digest, digestLen) <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_PKEY_verify failed");
        GetOpensslErrorMessage();
        EVP_PKEY_CTX_free(pkeyCtx);
        return false;
    }
    EVP_PKEY_CTX_free(pkeyCtx);
    return true;
}

bool HapVerifyOpensslUtils::GetPublickeys(const CertChain& signCertChain,
    std::vector<std::string>& SignatureVec)
{
    for (unsigned int i = 0; i < signCertChain.size(); i++) {
        if (!GetPublickeyFromCertificate(signCertChain[i], SignatureVec)) {
            HAPVERIFY_LOG_ERROR(LABEL, "%{public}ust Get Publickey failed", i);
            return false;
        }
    }
    return !SignatureVec.empty();
}

bool HapVerifyOpensslUtils::GetSignatures(const CertChain& signCertChain,
    std::vector<std::string>& SignatureVec)
{
    for (unsigned int i = 0; i < signCertChain.size(); i++) {
        if (!GetDerCert(signCertChain[i], SignatureVec)) {
            HAPVERIFY_LOG_ERROR(LABEL, "%{public}ust GetDerCert failed", i);
            return false;
        }
    }
    return !SignatureVec.empty();
}

bool HapVerifyOpensslUtils::GetDerCert(X509* ptrX509, std::vector<std::string>& SignatureVec)
{
    if (ptrX509 == nullptr) {
        return false;
    }

    int certLen = i2d_X509(ptrX509, nullptr);
    if (certLen <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "certLen %{public}d, i2d_X509 failed", certLen);
        GetOpensslErrorMessage();
        return false;
    }
    std::unique_ptr<unsigned char[]> derCertificate = std::make_unique<unsigned char[]>(certLen);
    int base64CertLen = HapCertVerifyOpensslUtils::CalculateLenAfterBase64Encode(certLen);
    std::unique_ptr<unsigned char[]> base64Certificate = std::make_unique<unsigned char[]>(base64CertLen);
    if (derCertificate == nullptr || base64Certificate == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "make_unique failed");
        return false;
    }
    unsigned char* derCertificateBackup = derCertificate.get();
    if (i2d_X509(ptrX509, &derCertificateBackup) <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "i2d_X509 failed");
        GetOpensslErrorMessage();
        return false;
    }

    /* base64 encode */
    int len = EVP_EncodeBlock(base64Certificate.get(), derCertificate.get(), certLen);
    SignatureVec.emplace_back(std::string(reinterpret_cast<char*>(base64Certificate.get()), len));
    return true;
}

bool HapVerifyOpensslUtils::GetPublickeyFromCertificate(const X509* ptrX509, std::vector<std::string>& publicKeyVec)
{
    if (ptrX509 == nullptr) {
        return false;
    }

    std::string publicKey;
    if (!HapCertVerifyOpensslUtils::GetPublickeyBase64(ptrX509, publicKey)) {
        HAPVERIFY_LOG_ERROR(LABEL, "GetPublickeyBase64 Failed");
        return false;
    }
    publicKeyVec.emplace_back(publicKey);
    return true;
}

bool HapVerifyOpensslUtils::GetContentInfo(const PKCS7* p7ContentInfo, HapByteBuffer& content)
{
    if ((p7ContentInfo == nullptr) || !PKCS7_type_is_data(p7ContentInfo)) {
        HAPVERIFY_LOG_ERROR(LABEL, "p7ContentInfo is invalid");
        return false;
    }

    ASN1_OCTET_STRING* strContentInfo = p7ContentInfo->d.data;
    if (strContentInfo == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "strContentInfo is invalid");
        return false;
    }

    int strContentInfoLen = strContentInfo->length;
    unsigned char* strContentInfoData = strContentInfo->data;
    if (strContentInfoData == nullptr || strContentInfoLen <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "ASN1_OCTET_STRING is invalid");
        return false;
    }

    content.SetCapacity(strContentInfoLen);
    content.PutData(0, reinterpret_cast<char*>(strContentInfoData), strContentInfoLen);
    HAPVERIFY_LOG_DEBUG(LABEL, "strContentInfoLen: %{public}d", strContentInfoLen);
    return true;
}

int HapVerifyOpensslUtils::GetDigestAlgorithmOutputSizeBytes(int nId)
{
    return EVP_MD_size(EVP_get_digestbynid(nId));
}

bool HapVerifyOpensslUtils::CheckDigestParameter(const DigestParameter& digestParameter)
{
    if (digestParameter.md == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "md is nullptr");
        return false;
    }
    if (digestParameter.ptrCtx == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "ptrCtx is nullptr");
        return false;
    }
    return true;
}

bool HapVerifyOpensslUtils::DigestInit(const DigestParameter& digestParameter)
{
    if (!CheckDigestParameter(digestParameter)) {
        return false;
    }
    if (EVP_DigestInit(digestParameter.ptrCtx, digestParameter.md) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestInit failed");
        return false;
    }
    return true;
}

/* the caller must ensure that EVP_DigestInit was called before calling this function */
bool HapVerifyOpensslUtils::DigestUpdate(const DigestParameter& digestParameter,
    const unsigned char content[], int len)
{
    if (content == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "content is nullptr");
        return false;
    }
    if (!CheckDigestParameter(digestParameter)) {
        return false;
    }
    if (EVP_DigestUpdate(digestParameter.ptrCtx, content, len) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestUpdate chunk failed");
        return false;
    }
    return true;
}

int HapVerifyOpensslUtils::GetDigest(const DigestParameter& digestParameter, unsigned char (&out)[EVP_MAX_MD_SIZE])
{
    unsigned int outLen = 0;
    if (!CheckDigestParameter(digestParameter)) {
        return outLen;
    }
    if (EVP_DigestFinal(digestParameter.ptrCtx, out, &outLen) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestFinal failed");
        outLen = 0;
    }
    return outLen;
}

int HapVerifyOpensslUtils::GetDigest(const HapByteBuffer& chunk, const std::vector<OptionalBlock>& optionalBlocks,
    const DigestParameter& digestParameter, unsigned char (&out)[EVP_MAX_MD_SIZE])
{
    int chunkLen = chunk.Remaining();
    unsigned int outLen = 0;
    if (digestParameter.md == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "md is nullprt");
        return outLen;
    }
    if (digestParameter.ptrCtx == nullptr) {
        HAPVERIFY_LOG_ERROR(LABEL, "ptrCtx is nullprt");
        return outLen;
    }

    if (EVP_DigestInit(digestParameter.ptrCtx, digestParameter.md) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestInit failed");
        return outLen;
    }

    if (EVP_DigestUpdate(digestParameter.ptrCtx, chunk.GetBufferPtr(), chunkLen) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestUpdate chunk failed");
        return outLen;
    }
    for (int i = 0; i < static_cast<int>(optionalBlocks.size()); i++) {
        chunkLen = optionalBlocks[i].optionalBlockValue.GetCapacity();
        if (EVP_DigestUpdate(digestParameter.ptrCtx, optionalBlocks[i].optionalBlockValue.GetBufferPtr(),
            chunkLen) <= 0) {
            GetOpensslErrorMessage();
            HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestUpdate %{public}dst optional block failed", i);
            return outLen;
        }
    }

    if (EVP_DigestFinal(digestParameter.ptrCtx, out, &outLen) <= 0) {
        GetOpensslErrorMessage();
        HAPVERIFY_LOG_ERROR(LABEL, "EVP_DigestFinal failed");
        outLen = 0;
    }
    return outLen;
}

void HapVerifyOpensslUtils::GetOpensslErrorMessage()
{
    unsigned long retOpenssl;
    char errOpenssl[OPENSSL_ERR_MESSAGE_MAX_LEN];
    while ((retOpenssl = ERR_get_error()) != 0) {
        ERR_error_string(retOpenssl, errOpenssl);
        HAPVERIFY_LOG_ERROR(LABEL, "openssl err: %{public}lu, message: %{public}s", retOpenssl, errOpenssl);
    }
}

int HapVerifyOpensslUtils::GetDigestAlgorithmId(int signAlgorithm)
{
    switch (signAlgorithm) {
        case ALGORITHM_SHA256_WITH_RSA_PSS:
        case ALGORITHM_SHA256_WITH_RSA_PKCS1_V1_5:
        case ALGORITHM_SHA256_WITH_ECDSA:
        case ALGORITHM_SHA256_WITH_DSA:
            return NID_sha256;
        case ALGORITHM_SHA384_WITH_RSA_PSS:
        case ALGORITHM_SHA384_WITH_RSA_PKCS1_V1_5:
        case ALGORITHM_SHA384_WITH_ECDSA:
        case ALGORITHM_SHA384_WITH_DSA:
            return NID_sha384;
        case ALGORITHM_SHA512_WITH_RSA_PSS:
        case ALGORITHM_SHA512_WITH_RSA_PKCS1_V1_5:
        case ALGORITHM_SHA512_WITH_ECDSA:
        case ALGORITHM_SHA512_WITH_DSA:
            return NID_sha512;
        default:
            HAPVERIFY_LOG_ERROR(LABEL, "signAlgorithm: %{public}d error", signAlgorithm);
            return NID_undef;
    }
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
