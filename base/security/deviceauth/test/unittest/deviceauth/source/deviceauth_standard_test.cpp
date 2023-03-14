/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "deviceauth_standard_test.h"
#include <gtest/gtest.h>
#include "common_defs.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "json_utils.h"
#include "securec.h"

using namespace std;
using namespace testing::ext;

#define TEST_REQ_ID 123
#define TEST_APP_ID "TestAppId"

class InitDeviceAuthServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void InitDeviceAuthServiceTest::SetUpTestCase() {}
void InitDeviceAuthServiceTest::TearDownTestCase() {}
void InitDeviceAuthServiceTest::SetUp() {}
void InitDeviceAuthServiceTest::TearDown() {}

HWTEST_F(InitDeviceAuthServiceTest, InitDeviceAuthServiceTest001, TestSize.Level0)
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

class DestroyDeviceAuthServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DestroyDeviceAuthServiceTest::SetUpTestCase() {}
void DestroyDeviceAuthServiceTest::TearDownTestCase() {}
void DestroyDeviceAuthServiceTest::SetUp() {}
void DestroyDeviceAuthServiceTest::TearDown() {}

HWTEST_F(DestroyDeviceAuthServiceTest, DestroyDeviceAuthServiceTest001, TestSize.Level0)
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
    DestroyDeviceAuthService();
}

class GetGmInstanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GetGmInstanceTest::SetUpTestCase() {}
void GetGmInstanceTest::TearDownTestCase() {}

void GetGmInstanceTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GetGmInstanceTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GetGmInstanceTest, GetGmInstanceTest001, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
}

class GetGaInstanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GetGaInstanceTest::SetUpTestCase() {}
void GetGaInstanceTest::TearDownTestCase() {}

void GetGaInstanceTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GetGaInstanceTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GetGaInstanceTest, GetGaInstanceTest001, TestSize.Level0)
{
    const GroupAuthManager *ga = GetGaInstance();
    EXPECT_NE(ga, nullptr);
}

class GmRegCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmRegCallbackTest::SetUpTestCase() {}
void GmRegCallbackTest::TearDownTestCase() {}

void GmRegCallbackTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmRegCallbackTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmRegCallbackTest, GmRegCallbackTest001, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    DeviceAuthCallback callback;
    int32_t ret = gm->regCallback(TEST_APP_ID, &callback);
    EXPECT_EQ(ret, HC_SUCCESS);
}

class GmUnRegCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmUnRegCallbackTest::SetUpTestCase() {}
void GmUnRegCallbackTest::TearDownTestCase() {}

void GmUnRegCallbackTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmUnRegCallbackTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmUnRegCallbackTest, GmUnRegCallbackTest001, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    DeviceAuthCallback callback;
    int32_t ret = gm->regCallback(TEST_APP_ID, &callback);
    EXPECT_EQ(ret, HC_SUCCESS);
    ret = gm->unRegCallback(TEST_APP_ID);
    EXPECT_EQ(ret, HC_SUCCESS);
}

class GmRegDataChangeListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmRegDataChangeListenerTest::SetUpTestCase() {}
void GmRegDataChangeListenerTest::TearDownTestCase() {}

void GmRegDataChangeListenerTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmRegDataChangeListenerTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmRegDataChangeListenerTest, GmRegDataChangeListenerTest001, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    DataChangeListener listener;
    int32_t ret = gm->regDataChangeListener(TEST_APP_ID, &listener);
    EXPECT_EQ(ret, HC_SUCCESS);
}

class GmUnRegDataChangeListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmUnRegDataChangeListenerTest::SetUpTestCase() {}
void GmUnRegDataChangeListenerTest::TearDownTestCase() {}

void GmUnRegDataChangeListenerTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmUnRegDataChangeListenerTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmUnRegDataChangeListenerTest, GmUnRegDataChangeListenerTest001, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    DataChangeListener listener;
    int32_t ret = gm->regDataChangeListener(TEST_APP_ID, &listener);
    EXPECT_EQ(ret, HC_SUCCESS);
    ret = gm->unRegDataChangeListener(TEST_APP_ID);
    EXPECT_EQ(ret, HC_SUCCESS);
}

class GmCreateGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmCreateGroupTest::SetUpTestCase() {}
void GmCreateGroupTest::TearDownTestCase() {}

void GmCreateGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmCreateGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmCreateGroupTest, GmCreateGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->createGroup(DEFAULT_OS_ACCOUNT, TEST_REQ_ID, TEST_APP_ID, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmDeleteGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmDeleteGroupTest::SetUpTestCase() {}
void GmDeleteGroupTest::TearDownTestCase() {}

void GmDeleteGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmDeleteGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmDeleteGroupTest, GmDeleteGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->deleteGroup(DEFAULT_OS_ACCOUNT, TEST_REQ_ID, TEST_APP_ID, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmAddMemberToGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmAddMemberToGroupTest::SetUpTestCase() {}
void GmAddMemberToGroupTest::TearDownTestCase() {}

void GmAddMemberToGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmAddMemberToGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmAddMemberToGroupTest, GmAddMemberToGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->addMemberToGroup(DEFAULT_OS_ACCOUNT, TEST_REQ_ID, TEST_APP_ID, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmDeleteMemberFromGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmDeleteMemberFromGroupTest::SetUpTestCase() {}
void GmDeleteMemberFromGroupTest::TearDownTestCase() {}

void GmDeleteMemberFromGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmDeleteMemberFromGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmDeleteMemberFromGroupTest, GmDeleteMemberFromGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->deleteMemberFromGroup(DEFAULT_OS_ACCOUNT, TEST_REQ_ID, TEST_APP_ID, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmProcessDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmProcessDataTest::SetUpTestCase() {}
void GmProcessDataTest::TearDownTestCase() {}

void GmProcessDataTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmProcessDataTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmProcessDataTest, GmProcessDataTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->processData(TEST_REQ_ID, NULL, 0);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmCheckAccessToGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmCheckAccessToGroupTest::SetUpTestCase() {}
void GmCheckAccessToGroupTest::TearDownTestCase() {}

void GmCheckAccessToGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmCheckAccessToGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmCheckAccessToGroupTest, GmCheckAccessToGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    int32_t ret = gm->checkAccessToGroup(TEST_REQ_ID, TEST_APP_ID, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetPkInfoListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetPkInfoListTest::SetUpTestCase() {}
void GmGetPkInfoListTest::TearDownTestCase() {}

void GmGetPkInfoListTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetPkInfoListTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetPkInfoListTest, GmGetPkInfoListTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    uint32_t returnNum = 0;
    int32_t ret = gm->getPkInfoList(TEST_REQ_ID, TEST_APP_ID, NULL, &returnData, &returnNum);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetGroupInfoByIdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetGroupInfoByIdTest::SetUpTestCase() {}
void GmGetGroupInfoByIdTest::TearDownTestCase() {}

void GmGetGroupInfoByIdTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetGroupInfoByIdTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetGroupInfoByIdTest, GmGetGroupInfoByIdTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    int32_t ret = gm->getGroupInfoById(TEST_REQ_ID, TEST_APP_ID, NULL, &returnData);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetGroupInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetGroupInfoTest::SetUpTestCase() {}
void GmGetGroupInfoTest::TearDownTestCase() {}

void GmGetGroupInfoTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetGroupInfoTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetGroupInfoTest, GmGetGroupInfoTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    uint32_t returnNum = 0;
    int32_t ret = gm->getGroupInfo(TEST_REQ_ID, TEST_APP_ID, NULL, &returnData, &returnNum);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetJoinedGroupsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetJoinedGroupsTest::SetUpTestCase() {}
void GmGetJoinedGroupsTest::TearDownTestCase() {}

void GmGetJoinedGroupsTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetJoinedGroupsTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetJoinedGroupsTest, GmGetJoinedGroupsTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    uint32_t returnNum = 0;
    int32_t ret = gm->getJoinedGroups(TEST_REQ_ID, TEST_APP_ID, IDENTICAL_ACCOUNT_GROUP, &returnData, &returnNum);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetRelatedGroupsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetRelatedGroupsTest::SetUpTestCase() {}
void GmGetRelatedGroupsTest::TearDownTestCase() {}

void GmGetRelatedGroupsTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetRelatedGroupsTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetRelatedGroupsTest, GmGetRelatedGroupsTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    uint32_t returnNum = 0;
    int32_t ret = gm->getRelatedGroups(TEST_REQ_ID, TEST_APP_ID, NULL, &returnData, &returnNum);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetDeviceInfoByIdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetDeviceInfoByIdTest::SetUpTestCase() {}
void GmGetDeviceInfoByIdTest::TearDownTestCase() {}

void GmGetDeviceInfoByIdTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetDeviceInfoByIdTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetDeviceInfoByIdTest, GmGetDeviceInfoByIdTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    int32_t ret = gm->getDeviceInfoById(TEST_REQ_ID, TEST_APP_ID, NULL, NULL, &returnData);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmGetTrustedDevicesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmGetTrustedDevicesTest::SetUpTestCase() {}
void GmGetTrustedDevicesTest::TearDownTestCase() {}

void GmGetTrustedDevicesTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmGetTrustedDevicesTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmGetTrustedDevicesTest, GmGetTrustedDevicesTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    char *returnData = NULL;
    uint32_t returnNum = 0;
    int32_t ret = gm->getTrustedDevices(TEST_REQ_ID, TEST_APP_ID, NULL, &returnData, &returnNum);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GmIsDeviceInGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GmIsDeviceInGroupTest::SetUpTestCase() {}
void GmIsDeviceInGroupTest::TearDownTestCase() {}

void GmIsDeviceInGroupTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GmIsDeviceInGroupTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GmIsDeviceInGroupTest, GmIsDeviceInGroupTest002, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
    bool ret = gm->isDeviceInGroup(TEST_REQ_ID, TEST_APP_ID, NULL, NULL);
    EXPECT_NE(ret, true);
}

class GaAuthDeviceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GaAuthDeviceTest::SetUpTestCase() {}
void GaAuthDeviceTest::TearDownTestCase() {}

void GaAuthDeviceTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GaAuthDeviceTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GaAuthDeviceTest, GaAuthDeviceTest002, TestSize.Level0)
{
    const GroupAuthManager *ga = GetGaInstance();
    EXPECT_NE(ga, nullptr);
    int32_t ret = ga->authDevice(DEFAULT_OS_ACCOUNT, TEST_REQ_ID, NULL, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}

class GaProcessDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void GaProcessDataTest::SetUpTestCase() {}
void GaProcessDataTest::TearDownTestCase() {}

void GaProcessDataTest::SetUp()
{
    int ret = InitDeviceAuthService();
    EXPECT_EQ(ret, HC_SUCCESS);
}

void GaProcessDataTest::TearDown()
{
    DestroyDeviceAuthService();
}

HWTEST_F(GaProcessDataTest, GaProcessDataTest002, TestSize.Level0)
{
    const GroupAuthManager *ga = GetGaInstance();
    EXPECT_NE(ga, nullptr);
    int32_t ret = ga->processData(TEST_REQ_ID, NULL, 0, NULL);
    EXPECT_NE(ret, HC_SUCCESS);
}