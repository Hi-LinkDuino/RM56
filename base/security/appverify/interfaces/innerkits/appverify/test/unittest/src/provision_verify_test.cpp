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
#include "provision_verify_test.h"

#include <gtest/gtest.h>

#ifndef STANDARD_SYSTEM
#include "ohos_account_kits.h"
#else
#include "parameter.h"
#include "sysparam_errno.h"
#endif // STANDARD_SYSTEM

#include "provision/provision_verify.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
const std::string INVALID_PROVISION_1 = R"(
    {
        "version-name": "1.0.0",
        "issuer": "App Gallery"
    })";
const std::string INVALID_PROVISION_2 = R"(
    {
        "version-code": 2,
        "version-name": "1.1.0",
        "uuid": "uuid_1_1",
        "type": "release",
        "app-distribution-type": "os_integration",
        "validity": {
            "not-before": 1262275201,
            "not-after": 2524579301
        },
        "bundle-info": {
            "developer-id": "developer_id_1_1",
            "development-certificate": "ABCD",
            "distribution-certificate": "1234",
            "bundle-name": "com.test1_1",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        }
    }})";   // brackets mismatching
const std::string INVALID_PROVISION_3 = R"(
    {
        "version-code": 2,
        "version-name": "2.0.0",
        "uuid": "uuid_2",
        "type": "release",
        "app-distribution-type": "os_integration",
        "validity": {
            "not-before": 1262275200,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_2",
            "development-certificate": "ABCDEFG",
            "distribution-certificate": "123456789",
            "bundle-name": "com.test2",
            "app-feature": "hos_normal_app"
        }
    })";  // apl not exist
/* validity: 2010 - 2030 */
const std::string VALID_PROVISION = R"(
    {
        "version-code": 2,
        "version-name": "2.0.0",
        "uuid": "uuid_2",
        "type": "release",
        "app-distribution-type": "os_integration",
        "validity": {
            "not-before": 1262275200,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_2",
            "development-certificate": "ABCDEFG",
            "distribution-certificate": "123456789",
            "bundle-name": "com.test2",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        }
    })";
const std::string VALID_PROVISION_PERMISSION = R"(
    {
        "version-code": 3,
        "version-name": "3.0.0",
        "uuid": "uuid_3",
        "type": "release",
        "app-distribution-type": "app_gallery",
        "validity": {
            "not-before": 1262275300,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_3",
            "development-certificate": "ABCDEFGH",
            "distribution-certificate": "1234567890",
            "bundle-name": "com.test3",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        },
        "permissions" : {
            "restricted-permissions" : ["ohos.permission.GET_PHONE_STATE",
                "ohos.permission.GET_APP_ACCOUNTS"],
            "restricted-capabilities" : ["ohos.permission.test"]
        },
        "issuer": "App Gallery"
    })";
const std::string UNAUTHORIZED_DEVICE_EMPTY_DEVICE_LIST_PROVISION = R"(
    {
        "version-code": 6,
        "version-name": "6.0.0",
        "uuid": "uuid_6",
        "type": "debug",
        "validity": {
            "not-before": 1262275600,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_6",
            "development-certificate": "ABCDEFGHIJ",
            "distribution-certificate": "1234567890123",
            "bundle-name": "com.test6",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        },
        "debug-info": {
            "device-id-type": "udid"
        },
        "issuer": "App Gallery"
    })";
const std::string UNSUPPORTED_DEVICE_TYPE_PROVISION = R"(
    {
        "version-code": 7,
        "version-name": "7.0.0",
        "uuid": "uuid_7",
        "type": "debug",
        "validity": {
            "not-before": 1262275600,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_7",
            "development-certificate": "ABCDEFGHIJM",
            "distribution-certificate": "12345678901231",
            "bundle-name": "com.test7",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        },
        "debug-info": {
            "device-id-type": "unknownType",
            "device-ids" : ["1234ABCD"]
        },
        "issuer": "App Gallery"
    })";
const std::string DEVICE_UNAUTH_PROVISION = R"(
    {
        "version-code": 8,
        "version-name": "8.0.0",
        "uuid": "uuid_8",
        "type": "debug",
        "validity": {
            "not-before": 1262275700,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "developer_id_8",
            "development-certificate": "ABCDEFGHIJK",
            "distribution-certificate": "12345678901234",
            "bundle-name": "com.test8",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        },
        "debug-info": {
            "device-id-type": "udid",
            "device-ids" : ["1234ABCD"]
        },
        "issuer": "App Gallery"
    })";
const int MAXIMUM_NUM_DEVICES = 100;

class ProvisionVerifyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ProvisionVerifyTest::SetUpTestCase(void)
{
}

void ProvisionVerifyTest::TearDownTestCase(void)
{
}

void ProvisionVerifyTest::SetUp()
{
}

void ProvisionVerifyTest::TearDown()
{
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify001 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input a invalid HarmonyAppProvision
     *     and run ParseAndVerify function.
     * @tc.expected: step1. return code will be PROVISION_INVALID.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(INVALID_PROVISION_1, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_INVALID);
    ret = ParseAndVerify(INVALID_PROVISION_2, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_INVALID);
    ret = ParseAndVerify(INVALID_PROVISION_3, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_INVALID);
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify002 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify002, TestSize.Level1)
{
    /*
     * @tc.steps: step2. input a valid HarmonyAppProvision (without permission info)
     *     and run ParseAndVerify function.
     * @tc.expected: step2. return code will be PROVISION_OK.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(VALID_PROVISION, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_OK);
    ASSERT_EQ(info.versionCode, 2); // version code is 2
    ASSERT_EQ(info.versionName, "2.0.0");
    ASSERT_EQ(info.uuid, "uuid_2");
    ASSERT_EQ(info.type, ProvisionType::RELEASE);
    ASSERT_TRUE(info.permissions.restrictedPermissions.empty());
    ASSERT_TRUE(info.permissions.restrictedCapabilities.empty());
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify003 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify003, TestSize.Level1)
{
    /*
     * @tc.steps: step3. input a valid HarmonyAppProvision (with permission info)
     *     and run ParseAndVerify function.
     * @tc.expected: step3. return code will be PROVISION_OK.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(VALID_PROVISION_PERMISSION, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_OK);
    ASSERT_EQ(info.versionCode, 3); // version code is 3
    ASSERT_EQ(info.versionName, "3.0.0");
    ASSERT_EQ(info.uuid, "uuid_3");
    ASSERT_EQ(info.type, ProvisionType::RELEASE);
    ASSERT_EQ(info.distributionType, AppDistType::APP_GALLERY);
    ASSERT_TRUE(info.permissions.restrictedPermissions.size() == 2);    // 2 restricted permissions
    ASSERT_EQ(info.permissions.restrictedPermissions[0], "ohos.permission.GET_PHONE_STATE");
    ASSERT_TRUE(info.permissions.restrictedCapabilities.size() == 1);   // 1 restricted capabilities
    ASSERT_EQ(info.permissions.restrictedCapabilities[0], "ohos.permission.test");
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify004 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify004, TestSize.Level1)
{
    /*
     * @tc.steps: step4. input a HarmonyAppProvision with empty device-id list
     *     and run ParseAndVerify function.
     * @tc.expected: step4. return code will be PROVISION_DEVICE_UNAUTHORIZED.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(UNAUTHORIZED_DEVICE_EMPTY_DEVICE_LIST_PROVISION, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_DEVICE_UNAUTHORIZED);
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify005 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify005, TestSize.Level1)
{
    /*
     * @tc.steps: step5. input a HarmonyAppProvision with a list of device IDs (Exceed maximum number)
     *     and run ParseAndVerify function.
     * @tc.expected: step5. return code will be PROVISION_NUM_DEVICE_EXCEEDED.
     */
    std::string prefixNumDeviceExceededProvision = R"(
    {
        "version-code": 1,
        "version-name": "1.0.0",
        "uuid": "test_uuid",
        "type": "debug",
        "validity": {
            "not-before": 1262275200,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "test_developer_id",
            "development-certificate": "ABCDEFG",
            "distribution-certificate": "123456789",
            "bundle-name": "com.hello",
            "app-feature": "hos_normal_app"
        },
        "debug-info": {
            "device-id-type": "udid",
            "device-ids" : [)";
    std::string postfixNumDeviceExceededProvision = R"(]
        },
        "issuer": "App Gallery"
    })";
    for (int i = 0; i < MAXIMUM_NUM_DEVICES; i++) {
        prefixNumDeviceExceededProvision += R"("1234ABCD",)";
    }
    prefixNumDeviceExceededProvision += R"("1234ABCD")";
    prefixNumDeviceExceededProvision += postfixNumDeviceExceededProvision;
    ProvisionInfo info;
    int ret = ParseAndVerify(prefixNumDeviceExceededProvision, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_NUM_DEVICE_EXCEEDED);
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify006 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify006, TestSize.Level1)
{
    /*
     * @tc.steps: step6. input a HarmonyAppProvision with unsupported device type
     *     and run ParseAndVerify function.
     * @tc.expected: step6. return code will be PROVISION_UNSUPPORTED_DEVICE_TYPE.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(UNSUPPORTED_DEVICE_TYPE_PROVISION, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_UNSUPPORTED_DEVICE_TYPE);
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify007 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify007, TestSize.Level1)
{
    /*
     * @tc.steps: step7. input a HarmonyAppProvision with an unauthorized device id
     *     and run ParseAndVerify function.
     * @tc.expected: step7. return code will be PROVISION_DEVICE_UNAUTHORIZED.
     */
    ProvisionInfo info;
    int ret = ParseAndVerify(DEVICE_UNAUTH_PROVISION, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_DEVICE_UNAUTHORIZED);
}

/**
 * @tc.name: Test HarmonyAppProvision ProvisionVerify008 function
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(ProvisionVerifyTest, ProvisionVerify008, TestSize.Level1)
{
    /*
     * @tc.steps: step8. input a valid HarmonyAppProvision with device info
     *     and run ParseAndVerify function.
     * @tc.expected: step8. return code will be PROVISION_OK.
     */
    std::string prefixValidDeviceInfoProvision = R"(
    {
        "version-code": 9,
        "version-name": "9.0.0",
        "uuid": "test_uuid",
        "type": "debug",
        "validity": {
            "not-before": 1262275200,
            "not-after": 2524579300
        },
        "bundle-info": {
            "developer-id": "test_developer_9",
            "development-certificate": "ABCDEFG",
            "distribution-certificate": "123456789",
            "bundle-name": "com.hello",
            "apl": "normal",
            "app-feature": "hos_normal_app"
        },
        "debug-info": {
            "device-id-type": "udid",
            "device-ids" : [")";
    std::string postfixValidDeviceInfoProvision = R"("]
        },
        "issuer": "App Gallery"
    })";
    string deviceId = "";
#ifndef STANDARD_SYSTEM
    OHOS::AccountSA::OhosAccountKits::GetInstance().GetUdid(deviceId);
#else
    char udid[DEV_UUID_LEN] = {0};
    int udidRet = GetDevUdid(udid, sizeof(udid));
    ASSERT_EQ(udidRet, EC_SUCCESS);
    deviceId = std::string(udid, sizeof(udid) - 1);
#endif
    prefixValidDeviceInfoProvision += deviceId;
    prefixValidDeviceInfoProvision += postfixValidDeviceInfoProvision;
    ProvisionInfo info;
    int ret = ParseAndVerify(prefixValidDeviceInfoProvision, info);
    ASSERT_EQ(ret, AppProvisionVerifyResult::PROVISION_OK);
}
} // namespace
