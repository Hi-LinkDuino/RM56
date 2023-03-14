/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hap_verify_test.h"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include "app_verify_pub.h"
#include "write_file.h"

using namespace std;
using namespace testing::ext;
namespace {
const int DELAY_TIME = 4;

class HapVerifyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapVerifyTest::SetUpTestCase(void)
{
}

void HapVerifyTest::TearDownTestCase(void)
{
}

void HapVerifyTest::SetUp()
{
}

void HapVerifyTest::TearDown()
{
}

/**
 * @tc.name: test001
 * @tc.desc: 测试签名正确的hap包可以安装并验签通过
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test001, TestSize.Level3)
{
    const char *signedHap = "./business_packet.hap";
    DeleteFile(signedHap);
    CopyFile(GetBusinessHap(), signedHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(signedHap, &verifyResult);
    ASSERT_TRUE(resultCode == V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    DeleteFile(signedHap);
}

/**
 * @tc.name: test002
 * @tc.desc: 安装未签名的应用，校验失败
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test002, TestSize.Level3)
{
    const char *unsignedHap = "./unsigned_packet.hap";
    DeleteFile(unsignedHap);
    CopyFile(GetUnsignedHap(), unsignedHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(unsignedHap, &verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    DeleteFile(unsignedHap);
}

/**
 * @tc.name: test003
 * @tc.desc: udid号与设备不对应的应用安装验签失败
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test003, TestSize.Level3)
{
    APPVERI_SetDebugMode(true);
    APPVERI_SetActsMode(true);
    const char *udidUnmatchHap = "./udid_wrong_test.hap";
    DeleteFile(udidUnmatchHap);
    CopyFile(GetWrongUdidHap(), udidUnmatchHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(udidUnmatchHap, &verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    APPVERI_SetDebugMode(false);
    APPVERI_SetActsMode(false);
    DeleteFile(udidUnmatchHap);
}

/**
 * @tc.name: test005
 * @tc.desc: 被篡改的应用安装验签
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test005, TestSize.Level3)
{
    const char *tamperedHap = "./modified_packet.hap";
    DeleteFile(tamperedHap);
    CopyFile(GetModifiedHap(), tamperedHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(tamperedHap, &verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    DeleteFile(tamperedHap);
}

/**
 * @tc.name: test006
 * @tc.desc: debug模式打开，安装商用版本hap成功，安装测试版本hap成功
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test006, TestSize.Level3)
{
    APPVERI_SetDebugMode(true);
    APPVERI_SetActsMode(true);
    const char *testHap = "./success_test.hap";
    DeleteFile(testHap);
    CopyFile(GetSuccessHap(), testHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(testHap, &verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    APPVERI_SetActsMode(false);
    DeleteFile(testHap);

    const char *businessHap = "./business_packet.hap";
    DeleteFile(businessHap);
    CopyFile(GetBusinessHap(), businessHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult1 = {0};
    resultCode = APPVERI_AppVerify(businessHap, &verifyResult1);
    APPVERI_FreeVerifyRst(&verifyResult1);
    ASSERT_TRUE(resultCode == V_OK);
    APPVERI_SetDebugMode(false);
    DeleteFile(businessHap);
}

/**
 * @tc.name: test007
 * @tc.desc: debug模式关闭，安装商用版本hap成功，安装测试版本hap失败
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test007, TestSize.Level3)
{
    APPVERI_SetDebugMode(false);
    const char *businessHap = "./business_packet.hap";
    DeleteFile(businessHap);
    CopyFile(GetBusinessHap(), businessHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(businessHap, &verifyResult);
    ASSERT_TRUE(resultCode == V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    DeleteFile(businessHap);

    const char *testHap = "./success_test.hap";
    DeleteFile(testHap);
    CopyFile(GetSuccessHap(), testHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult1 = {0};
    APPVERI_SetActsMode(true);
    resultCode = APPVERI_AppVerify(testHap, &verifyResult1);
    APPVERI_FreeVerifyRst(&verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_SetDebugMode(false);
    APPVERI_SetActsMode(false);
    DeleteFile(testHap);
}

/**
 * @tc.name: test008
 * @tc.desc: 使用错误证书的应用安全验签失败
 * @tc.function: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.spec: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.feature: SUB_SEC_AppSec_AppSign_VerifySignature
 * @tc.number: SUB_SEC_AppSec_AppSign_VerifySignature_1500
 * @tc.size: MEDIUM
 * @tc.level: level 3
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, test008, TestSize.Level3)
{
    const char *wrongHap = "./wrong_license.hap";
    DeleteFile(wrongHap);
    CopyFile(GetWrongLiceseHap(), wrongHap);
    sleep(DELAY_TIME);
    VerifyResult verifyResult = {0};
    int32_t resultCode = APPVERI_AppVerify(wrongHap, &verifyResult);
    ASSERT_TRUE(resultCode != V_OK);
    APPVERI_FreeVerifyRst(&verifyResult);
    DeleteFile(wrongHap);
}
}
