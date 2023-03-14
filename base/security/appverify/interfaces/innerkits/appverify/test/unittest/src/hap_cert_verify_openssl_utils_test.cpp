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

#include "hap_cert_verify_openssl_utils_test.h"

#include <fstream>
#include <gtest/gtest.h>

#include "openssl/asn1.h"
#include "openssl/x509.h"

#include "init/trusted_root_ca.h"
#include "interfaces/hap_verify.h"
#include "util/hap_cert_verify_openssl_utils.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace OHOS {
namespace Security {
namespace Verify {
bool Base64StringDecode(const std::string& base64String, HapByteBuffer& output)
{
    std::unique_ptr<char[]> decodeBuffer = std::make_unique<char[]>(base64String.size());
    if (decodeBuffer == nullptr) {
        return false;
    }

    const unsigned char* input = reinterpret_cast<const unsigned char*>(base64String.c_str());
    int len = EVP_DecodeBlock(reinterpret_cast<unsigned char*>(decodeBuffer.get()), input, base64String.size());
    if (len <= 0) {
        return false;
    }

    output.SetCapacity(len);
    output.PutData(0, decodeBuffer.get(), len);
    decodeBuffer.reset(nullptr);
    return true;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS

namespace {
class HapCertVerifyOpensslUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapCertVerifyOpensslUtilsTest::SetUpTestCase(void)
{
}

void HapCertVerifyOpensslUtilsTest::TearDownTestCase(void)
{
}
void HapCertVerifyOpensslUtilsTest::SetUp()
{
    EnableDebugMode();
}

void HapCertVerifyOpensslUtilsTest::TearDown()
{
}

/**
 * @tc.name: Test GetSubjectFromX509 and GetIssuerFromX509 functions.
 * @tc.desc: Check whether GetSubjectFromX509 and GetIssuerFromX509 can be used to obtain the certificate information.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, GetCertInfo001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input an ECC self signed cert with PEM encoded.
     * @tc.expected: step1. The return is an X509 object.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(UAT_SOFT_SIGN_CA);
    ASSERT_TRUE(certX509 != nullptr);
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    /*
     * @tc.steps: step2. Find the CA certificate that signed certX509.
     * @tc.expected: step2. The return is an X509 object.
     */
    X509* issuerCert = rootCertsObj.FindMatchedRoot(certX509);
    /*
     * @tc.steps: step3. Get subject of rootCertsObj and get issuer of certX509.
     * @tc.expected: step3. The subject is same as issuer.
     */
    std::string subject;
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetSubjectFromX509(issuerCert, subject));
    std::string issuer;
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetIssuerFromX509(certX509, issuer));
    ASSERT_TRUE(subject.compare(issuer) == 0);
    X509_free(certX509);
    X509_free(issuerCert);
}

/**
 * @tc.name: Test CertVerify function
 * @tc.desc: Check whether the CertVerify function can be used to verify a certificate.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, CertVerify001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input an ECC self signed cert with PEM encoded.
     * @tc.expected: step1. The return is an X509 object.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    /*
     * @tc.steps: step2. Use all possible input to test CertVerify.
     * @tc.expected: step2. The return is verify result.
     */
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CertVerify(nullptr, certX509));
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CertVerify(certX509, nullptr));
    ASSERT_TRUE(HapCertVerifyOpensslUtils::CertVerify(certX509, certX509));
    X509_free(certX509);
}

/**
 * @tc.name: Test GetX509CertFromBase64String function
 * @tc.desc: Verify whether the GetX509CertFromBase64String function can get an X509 cert from a Base64S string.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, Base64NotUrlSafeCertTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input string of ECC cert with base64 decode
     * @tc.expected: step1. The return is an X509 object.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromBase64String(BASE64_NOT_URL_SAFE_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    std::string subject;
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetSubjectFromX509(certX509, subject));
    X509_free(certX509);
    certX509 = nullptr;
    /*
     * @tc.steps: step2. Use the subject as an error input.
     * @tc.expected: step2. The return is nullptr.
     */
    certX509 = HapCertVerifyOpensslUtils::GetX509CertFromBase64String(subject);
    ASSERT_TRUE(certX509 == nullptr);
}

/**
 * @tc.name: Test GetX509CrlFromDerBuffer function
 * @tc.desc: Verify whether the GetX509CrlFromDerBuffer function can get an X509 Crl from DerData.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, GetX509CrlFromDerBufferTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input invalid data.
     * @tc.expected: step1. The return is nullptr.
     */
    HapByteBuffer crlBuffer;
    X509_CRL* crlX509 = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlBuffer, 0, 0);
    ASSERT_TRUE(crlX509 == nullptr);
    crlBuffer.SetCapacity(TEST_HAPBYTEBUFFER_LENGTH);
    crlX509 = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlBuffer, 0, 0);
    ASSERT_TRUE(crlX509 == nullptr);
    /*
     * @tc.steps: step2. Input a string of crl encoded by base64.
     * @tc.expected: step2. Ihe return is a crl object.
     */
    ASSERT_TRUE(Base64StringDecode(BASE64_CRL, crlBuffer));
    crlX509 = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlBuffer, 0, crlBuffer.GetCapacity());
    std::string crlIssuer;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetIssuerFromX509Crl(nullptr, crlIssuer));
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetIssuerFromX509Crl(crlX509, crlIssuer));
    ASSERT_TRUE(crlX509 != nullptr);
    X509_CRL_free(crlX509);
}

/**
 * @tc.name: Test GetCertsChain function
 * @tc.desc: Verify whether the GetCertsChain function can get a trusted cert chain from certs.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, GetCertsChainTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use a certVisitSign included a self signed cert and an empty certsChain as input.
     * @tc.expected: step1. The return is false due to invalid input.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    CertChain certsChain;
    CertSign certVisitSign;
    certVisitSign[certX509] = false;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetCertsChain(certsChain, certVisitSign));
    /*
     * @tc.steps: step2. Push a self signed cert to certChain.
     * @tc.expected: step2. The return is false due to can not verify by root ca.
     */
    certsChain.push_back(certX509);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetCertsChain(certsChain, certVisitSign));
    certVisitSign[certX509] = true;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetCertsChain(certsChain, certVisitSign));
    X509_free(certX509);
}

/**
 * @tc.name: Test GetSubjectFromX509 and GetIssuerFromX509 functions.
 * @tc.desc: The static function will return false due to invalid input;
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, GetIssuerAndSubjectTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use nullptr as input to test GetSubjectFromX509.
     * @tc.expected: step1. The return is false.
     */
    std::string subject;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetSubjectFromX509(nullptr, subject));
    /*
     * @tc.steps: step2. Use nullptr as input to test GetIssuerFromX509.
     * @tc.expected: step2. The return is false.
     */
    std::string issuer;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetIssuerFromX509(nullptr, issuer));
}

/**
 * @tc.name: Test GetSerialNumberFromX509 function.
 * @tc.desc: Verify whether the GetSerialNumberFromX509 function can get the SerialNumber from an X509 certificate.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, GetSerialNumberFromX509Test001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use nullptr as input to test GetSerialNumberFromX509.
     * @tc.expected: step1. The return is false.
     */
    long long certSerialNumber;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::GetSerialNumberFromX509(nullptr, certSerialNumber));
    /*
     * @tc.steps: step2. Use real cert to test GetX509CertFromPemString.
     * @tc.expected: step2. the return is true.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetSerialNumberFromX509(certX509, certSerialNumber));
    X509_free(certX509);
}

/**
 * @tc.name: Test VerifyCertChainPeriodOfValidity function.
 * @tc.desc: Verify whether the VerifyCertChainPeriodOfValidity function can verify validity
 *           period of a certificate chain.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, VerifyCertChainPeriodOfValidityTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input an empty certChain.
     * @tc.expected: step1. The return is false.
     */
    CertChain certsChain;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(certsChain, nullptr));
    /*
     * @tc.steps: step2. Input a nullptr signTime and a certChain with two cert.
     * @tc.expected: step2. The return is false.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromBase64String(BASE64_NOT_URL_SAFE_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    certsChain.push_back(certX509);
    certsChain.push_back(certX509);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(certsChain, nullptr));
    /*
     * @tc.steps: step3. Input a signTime which out of period of validity.
     * @tc.expected: step3. The return is false.
     */
    ASN1_OCTET_STRING* asnString = ASN1_OCTET_STRING_new();
    ASSERT_TRUE(asnString != nullptr);
    ASSERT_TRUE(ASN1_OCTET_STRING_set(asnString, reinterpret_cast<const unsigned char*>(TEST_ANS_TIME.c_str()),
        static_cast<int>(TEST_ANS_TIME.size())));
    ASN1_TYPE* time = ASN1_TYPE_new();
    ASSERT_TRUE(time != nullptr);
    ASN1_TYPE_set(time, V_ASN1_UTCTIME, asnString);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(certsChain, time));
    /*
     * @tc.steps: step4. Input a certChain with two nullptr.
     * @tc.expected: step4. The return iis false.
     */
    certsChain.clear();
    certsChain.push_back(nullptr);
    certsChain.push_back(nullptr);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCertChainPeriodOfValidity(certsChain, time));
    X509_free(certX509);
    ASN1_TYPE_free(time);
}

/**
 * @tc.name: Test VerifyCrl function.
 * @tc.desc: Verify whether the VerifyCrl function can verify a crl.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, VerifyCrlTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Input an empty certChain.
     * @tc.expected: step1. The return is false.
     */
    CertChain certsChain;
    Pkcs7Context pkcs7Context;
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCrl(certsChain, nullptr, pkcs7Context));
    /*
     * @tc.steps: step2. Input a certChain with two nullptr.
     * @tc.expected: step2. the return is false.
     */
    STACK_OF(X509_CRL)* crls = sk_X509_CRL_new_null();
    ASSERT_TRUE(crls != nullptr);
    certsChain.push_back(nullptr);
    certsChain.push_back(nullptr);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCrl(certsChain, crls, pkcs7Context));
    /*
     * @tc.steps: step3. Input valid certChain and null.
     * @tc.expected: step3. The return is true due to crl is optional.
     */
    certsChain.clear();
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    certsChain.push_back(certX509);
    certsChain.push_back(certX509);
    ASSERT_TRUE(HapCertVerifyOpensslUtils::VerifyCrl(certsChain, crls, pkcs7Context));
    /*
     * @tc.steps: step4. Input error certChain and crls.
     * @tc.expected: step4. The return is false.
     */
    certsChain.clear();
    X509* root = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ROOTCERT);
    ASSERT_TRUE(root != nullptr);
    certsChain.push_back(root);
    certsChain.push_back(certX509);
    HapByteBuffer crlBuffer;
    ASSERT_TRUE(Base64StringDecode(BASE64_CRL, crlBuffer));
    X509_CRL* crlX509 = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlBuffer, 0, crlBuffer.GetCapacity());
    ASSERT_TRUE(crlX509 != nullptr);
    sk_X509_CRL_push(crls, crlX509);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::VerifyCrl(certsChain, crls, pkcs7Context));
    /*
     * @tc.steps: step5. Input right certChain and crls.
     * @tc.expected: step5. The return is true.
     */
    certsChain.clear();
    certsChain.push_back(root);
    certsChain.push_back(root);
    ASSERT_TRUE(HapCertVerifyOpensslUtils::VerifyCrl(certsChain, crls, pkcs7Context));
    X509_free(certX509);
    X509_free(root);
    sk_X509_CRL_pop_free(crls, X509_CRL_free);
}

/**
 * @tc.name: Test CompareX509Cert function
 * @tc.desc: Verify whether the CompareX509Cert function can compare two certificates.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, CompareX509CertTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use nullptr as input to test CompareX509Cert.
     * @tc.expected: step1. The return is false.
     */
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CompareX509Cert(nullptr, ECC_TEST_KEY));
    /*
     * @tc.steps: step2. The second cert is not a cert string.
     * @tc.expected: step2. the return is false.
     */
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CompareX509Cert(certX509, ECC_TEST_KEY));
    /*
     * @tc.steps: step3. Input two same cert.
     * @tc.expected: step3. The return is true.
     */
    ASSERT_TRUE(HapCertVerifyOpensslUtils::CompareX509Cert(certX509, ECC_TEST_CERT));
    X509_free(certX509);
}

/**
 * @tc.name: Test WriteX509CrlToStream function.
 * @tc.desc: Verify whether the WriteX509CrlToStream function can write crl to a file.
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, WriteX509CrlToStreamTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use invalid input.
     * @tc.expected: step1. The file length is zero.
     */
    std::ofstream crlFile;
    HapCertVerifyOpensslUtils::WriteX509CrlToStream(crlFile, nullptr);
    std::string filePath = "./test_crl";
    crlFile.open(filePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    HapCertVerifyOpensslUtils::WriteX509CrlToStream(crlFile, nullptr);
    ASSERT_TRUE(crlFile.tellp() == 0);
    if (crlFile.is_open()) {
        crlFile.close();
    }
}

/**
 * @tc.name: Test private function
 * @tc.desc: The static function will return result of invalid input;
 * @tc.type: FUNC
 */
HWTEST_F(HapCertVerifyOpensslUtilsTest, PrivateFuncInvalidInputTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. Use invalid input.
     * @tc.expected: step1. The return is false.
     */
    CertSign certVisitSign;
    HapCertVerifyOpensslUtils::GenerateCertSignFromCertStack(nullptr, certVisitSign);
    ASSERT_TRUE(HapCertVerifyOpensslUtils::FindCertOfIssuer(nullptr, certVisitSign) == nullptr);
    std::string str = HapCertVerifyOpensslUtils::GetDnToString(nullptr);
    ASSERT_FALSE(str.size() > 0);
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetCrlBySignedCertIssuer(nullptr, nullptr) == nullptr);
    ASSERT_FALSE(HapCertVerifyOpensslUtils::X509NameCompare(nullptr, nullptr));
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CheckSignTimeInValidPeriod(nullptr, nullptr, nullptr));
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CheckAsn1TimeIsValid(nullptr));
    ASSERT_FALSE(HapCertVerifyOpensslUtils::CheckAsn1TypeIsValid(nullptr));
}
}
