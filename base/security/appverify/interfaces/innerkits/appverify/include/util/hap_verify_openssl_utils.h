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
#ifndef HAP_VERIFY_OPENSSL_UTILS_H
#define HAP_VERIFY_OPENSSL_UTILS_H

#include <string>
#include <vector>

#include "openssl/evp.h"

#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "interfaces/hap_verify_result.h"
#include "util/digest_parameter.h"
#include "util/hap_cert_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
enum SignatureAlgorithm {
    ALGORITHM_SHA256_WITH_RSA_PSS = 0x00000101,
    ALGORITHM_SHA384_WITH_RSA_PSS,
    ALGORITHM_SHA512_WITH_RSA_PSS,
    ALGORITHM_SHA256_WITH_RSA_PKCS1_V1_5,
    ALGORITHM_SHA384_WITH_RSA_PKCS1_V1_5,
    ALGORITHM_SHA512_WITH_RSA_PKCS1_V1_5,
    ALGORITHM_SHA256_WITH_ECDSA = 0x00000201,
    ALGORITHM_SHA384_WITH_ECDSA,
    ALGORITHM_SHA512_WITH_ECDSA,
    ALGORITHM_SHA256_WITH_DSA = 0x00000301,
    ALGORITHM_SHA384_WITH_DSA,
    ALGORITHM_SHA512_WITH_DSA,
};

class HapVerifyOpensslUtils {
public:
    DLL_EXPORT static bool ParsePkcs7Package(const unsigned char packageData[],
        unsigned int packageLen, Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool GetCertChains(PKCS7* p7, Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool VerifyPkcs7(Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool GetPublickeys(const CertChain& signCertChain, std::vector<std::string>& SignatureVec);
    DLL_EXPORT static bool GetSignatures(const CertChain& signCertChain, std::vector<std::string>& SignatureVec);
    static int GetDigest(const HapByteBuffer& chunk, const std::vector<OptionalBlock>& optionalBlocks,
        const DigestParameter& digestParameter, unsigned char (&out)[EVP_MAX_MD_SIZE]);
    static bool DigestInit(const DigestParameter& digestParameter);
    static bool DigestUpdate(const DigestParameter& digestParameter, const unsigned char content[], int len);
    static int GetDigest(const DigestParameter& digestParameter, unsigned char (&out)[EVP_MAX_MD_SIZE]);
    static int GetDigestAlgorithmOutputSizeBytes(int nId);
    DLL_EXPORT static int GetDigestAlgorithmId(int signAlgorithm);
    static void GetOpensslErrorMessage();

private:
    DLL_EXPORT static bool VerifyPkcs7SignedData(Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool VerifySignInfo(STACK_OF(PKCS7_SIGNER_INFO)* signerInfoStack,
        BIO* p7Bio, int signInfoNum, Pkcs7Context& pkcs7Context);
    DLL_EXPORT static bool GetPublickeyFromCertificate(const X509* ptrX509, std::vector<std::string>& publicKeyVec);
    DLL_EXPORT static bool GetDerCert(X509* ptrX509, std::vector<std::string>& SignatureVec);
    static bool VerifyShaWithRsaPss(const PKCS7_SIGNER_INFO* signInfo, BIO* p7Bio, EVP_PKEY* pkey, bool isPss);
    DLL_EXPORT static bool VerifyShaWithRsaPss(const PKCS7_SIGNER_INFO* signInfo, EVP_PKEY* pkey, bool isPss,
        const unsigned char digest[], unsigned int digestLen);
    static bool VerifyCertChain(CertChain& certsChain, PKCS7* p7, PKCS7_SIGNER_INFO* signInfo,
        Pkcs7Context& pkcs7Context, CertSign& certVisitSign);
    static bool IsEnablePss(const PKCS7_SIGNER_INFO* signInfo);
    static bool GetContentInfo(const PKCS7* p7ContentInfo, HapByteBuffer& content);
    static bool CheckPkcs7SignedDataIsValid(const PKCS7* p7);
    DLL_EXPORT static bool AsnStringCmp(const ASN1_OCTET_STRING* asnStr, const unsigned char data[], int len);
    static bool VerifyPkcs7AuthAttributes(const PKCS7_SIGNER_INFO* signInfo, EVP_MD_CTX* mdCtx, int mdType);
    static const EVP_MD_CTX* FindMdCtxInBio(BIO* p7Bio, int mdType);
    static bool CheckDigestParameter(const DigestParameter& digestParameter);

private:
    static const int OPENSSL_PKCS7_VERIFY_SUCCESS;
    static const int OPENSSL_ERR_MESSAGE_MAX_LEN;
    static const int OPENSSL_READ_DATA_MAX_TIME;
    static const int OPENSSL_READ_DATA_LEN_EACH_TIME;
    static const int MAX_OID_LENGTH;
    static const std::string PKCS7_EXT_SHAWITHRSA_PSS;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_VERIFY_OPENSSL_UTILS_H
