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

#include "hap_crl_manager_test.h"

#include <fstream>
#include <gtest/gtest.h>

#include "init/hap_crl_manager.h"
#include "util/hap_cert_verify_openssl_utils.h"

#include "hap_cert_verify_openssl_utils_test.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace OHOS {
namespace Security {
namespace Verify {
const std::string HAP_CRL_TEST_BACKUP_FILE_PATH = "/data/system/hap_crl_backup_test";

enum CopyFileState {
    NO_SOURCE_FILE = 0,
    COPY_FAIL,
    COPY_SUC,
};

CopyFileState CopyFile(const std::string& sourceFilePath, const std::string& destFilePath)
{
    std::ifstream source;
    std::ofstream dest;
    source.open(sourceFilePath.c_str(), std::ios::in);
    if (!source.is_open()) {
        return NO_SOURCE_FILE;
    }
    dest.open(destFilePath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!dest.is_open()) {
        source.close();
        return COPY_FAIL;
    }
    dest << source.rdbuf();
    source.close();
    dest.close();
    return COPY_SUC;
}

bool ClearFile(const std::string& filePath)
{
    std::ofstream fileStream;
    fileStream.open(filePath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!fileStream.is_open()) {
        return false;
    }
    fileStream.close();
    return true;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS

namespace {
class HapCrlManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapCrlManagerTest::SetUpTestCase(void)
{
}

void HapCrlManagerTest::TearDownTestCase(void)
{
}

void HapCrlManagerTest::SetUp()
{
}

void HapCrlManagerTest::TearDown()
{
}

/**
 * @tc.name: Test Write and Read Crl function
 * @tc.desc: The static function will return whether file written can be read;
 * @tc.type: FUNC
 */
HWTEST_F(HapCrlManagerTest, WriteAndReadCrlsFileTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. backup crl file
     * @tc.expected: step1. ret will be success if crl file is exist, or ret will be NO_SOURCE_FILE.
     */
    CopyFileState ret = CopyFile(HapCrlManager::HAP_CRL_FILE_PATH, HAP_CRL_TEST_BACKUP_FILE_PATH);
    ASSERT_TRUE(ret != COPY_FAIL);
    /*
     * @tc.steps: step2. update a crl to crl manager and write crls to file
     * @tc.expected: step2. binary code of crls is written to file.
     */
    HapByteBuffer crlBuffer;
    ASSERT_TRUE(Base64StringDecode(BASE64_CRL, crlBuffer));
    X509_CRL* crlX509 = HapCertVerifyOpensslUtils::GetX509CrlFromDerBuffer(crlBuffer, 0, crlBuffer.GetCapacity());
    ASSERT_TRUE(crlX509 != nullptr);
    std::string crlIssuer;
    ASSERT_TRUE(HapCertVerifyOpensslUtils::GetIssuerFromX509Crl(crlX509, crlIssuer));
    HapCrlManager& hapCrlManager = HapCrlManager::GetInstance();
    hapCrlManager.UpdateCrlByIssuer(crlIssuer, nullptr);
    hapCrlManager.UpdateCrlByIssuer(crlIssuer, crlX509);
    hapCrlManager.WriteCrlsToFile();
    X509_CRL_free(crlX509);
    /*
     * @tc.steps: step3. read crl file.
     * @tc.expected: step3. crl file can be read and parse rightly.
     */
    HapByteBuffer crlsBuffer;
    ASSERT_TRUE(hapCrlManager.ReadCrls(crlsBuffer));
    ASSERT_TRUE(crlsBuffer.HasRemaining());
    ASSERT_TRUE(hapCrlManager.ParseCrls(crlsBuffer));
    /*
     * @tc.steps: step4. recovery crl file.
     * @tc.expected: step4. if backup result is NO_SOURCE_FILE, crl file will be clear.
     */
    if (ret == NO_SOURCE_FILE) {
        ASSERT_TRUE(ClearFile(HapCrlManager::HAP_CRL_FILE_PATH));
    } else {
        ASSERT_TRUE(CopyFile(HAP_CRL_TEST_BACKUP_FILE_PATH, HapCrlManager::HAP_CRL_FILE_PATH) == COPY_SUC);
    }
}
}
