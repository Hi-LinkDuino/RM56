/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UTTest_device_manager_service.h"

#include "dm_constants.h"
#include "dm_device_info.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DeviceManagerService);

void DeviceManagerServiceTest::SetUp()
{
}

void DeviceManagerServiceTest::TearDown()
{
}

void DeviceManagerServiceTest::SetUpTestCase()
{
}

void DeviceManagerServiceTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: StartDeviceDiscovery_001
 * @tc.desc: Set StartDeviceDiscovery's flag bit intFlag_ to False and return DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, StartDeviceDiscovery_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    DmSubscribeInfo subscribeInfo;
    std::string extra = "test";
    int ret = DeviceManagerService::GetInstance().StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: StartDeviceDiscovery_002
 * @tc.desc: Empty pkgName of StartDeviceDiscovery and return DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, StartDeviceDiscovery_002, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    DmSubscribeInfo subscribeInfo;
    std::string extra = "test";
    DeviceManagerService::GetInstance().Init();
    int ret = DeviceManagerService::GetInstance().StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: StartDeviceDiscovery_003
 * @tc.desc: Call StartDeviceDiscovery twice with pkgName not null and flag bit not false and return
 * DM_DISCOVERY_REPEATED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, StartDeviceDiscovery_003, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    DmSubscribeInfo subscribeInfo;
    std::string extra = "test";
    int ret = DeviceManagerService::GetInstance().StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    pkgName = "1com.ohos.test1";
    ret = DeviceManagerService::GetInstance().StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    EXPECT_EQ(ret, DM_DISCOVERY_FAILED);
}

/**
 * @tc.name: StopDeviceDiscovery_001
 * @tc.desc: Set StopDeviceDiscovery's flag bit intFlag_ to false and return DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, StopDeviceDiscovery_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    uint16_t subscribeId = 1;
    int ret = DeviceManagerService::GetInstance().StopDeviceDiscovery(pkgName, subscribeId);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name:StopDeviceDiscovery_002
 * @tc.desc: StopDeviceDiscovery is initialized, pkgName is null, and its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, StopDeviceDiscovery_002, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    uint16_t subscribeId = 1;
    DeviceManagerService::GetInstance().Init();
    int ret = DeviceManagerService::GetInstance().StopDeviceDiscovery(pkgName, subscribeId);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: GetLocalDeviceInfo_001
 * @tc.desc: Set the flag bit of GetLocalDeviceInfo to intFlag_ to false; The return value is DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetLocalDeviceInfo_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    DmDeviceInfo info;
    int ret = DeviceManagerService::GetInstance().GetLocalDeviceInfo(info);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name:GetLocalDeviceInfo_002
 * @tc.desc: Initialize the GetLocalDeviceInfo function with the return value DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetLocalDeviceInfo_002, testing::ext::TestSize.Level0)
{
    DmDeviceInfo info;
    DeviceManagerService::GetInstance().Init();
    int ret = DeviceManagerService::GetInstance().GetLocalDeviceInfo(info);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: Init_001
 * @tc.desc: The OnDeviceFound function does the worong case and return DM_INT_MULTIPLE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, Init_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    int ret = DeviceManagerService::GetInstance().Init();
    EXPECT_EQ(ret, DM_INT_MULTIPLE);
}

/**
 * @tc.name: Init_002
 * @tc.desc: The OnDeviceFound function does the correct case and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, Init_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::shared_ptr<SoftbusConnector> softbusConnector_ = nullptr;
    int ret = DeviceManagerService::GetInstance().Init();
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetTrustedDeviceList_001
 * @tc.desc: Set the intFlag of GetTrustedDeviceList to false. The return value is DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */

HWTEST_F(DeviceManagerServiceTest, GetTrustedDeviceList_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    std::string extra = "jdddd";
    std::vector<DmDeviceInfo> deviceList;
    int ret = DeviceManagerService::GetInstance().GetTrustedDeviceList(pkgName, extra, deviceList);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: GetTrustedDeviceList_002
 * @tc.desc:Set the intFlag of GetTrustedDeviceList to true and pkgName = null; The return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetTrustedDeviceList_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName;
    std::string extra = "jdddd";
    std::vector<DmDeviceInfo> deviceList;
    int ret = DeviceManagerService::GetInstance().GetTrustedDeviceList(pkgName, extra, deviceList);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: AuthenticateDevice_001
 * @tc.desc: 将GAuthenticateDevice的intFlag设置为false，设置pkgName = "com.ohos.test";其返回值为DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, AuthenticateDevice_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    std::string extra = "jdddd";
    int32_t authType = 0;
    std::string deviceId = "2345";
    int ret = DeviceManagerService::GetInstance().AuthenticateDevice(pkgName, authType, deviceId, extra);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: AuthenticateDevice_002
 * @tc.desc: Set intFlag for GAuthenticateDevice to True and pkgName to null; The return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, AuthenticateDevice_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName;
    std::string extra = "jdddd";
    int32_t authType = 0;
    std::string deviceId = " 2345";
    int ret = DeviceManagerService::GetInstance().AuthenticateDevice(pkgName, authType, deviceId, extra);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: AuthenticateDevice_003
 * @tc.desc: Set intFlag for GAuthenticateDevice to True and deviceId to null; The return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, AuthenticateDevice_003, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "com.ohos.test";
    std::string extra = "jdddd";
    int32_t authType = 0;
    std::string deviceId;
    int ret = DeviceManagerService::GetInstance().AuthenticateDevice(pkgName, authType, deviceId, extra);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}
/**
 * @tc.name: AuthenticateDevice_004
 * @tc.desc: Set intFlag for GAuthenticateDevice to true and pkgName to com.ohos.test
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, AuthenticateDevice_004, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "com.ohos.test";
    std::string extra = "jdddd";
    int32_t authType = 0;
    std::string deviceId = "123456";
    int ret = DeviceManagerService::GetInstance().AuthenticateDevice(pkgName, authType, deviceId, extra);
    EXPECT_EQ(ret, DM_AUTH_NOT_SUPPORT);
}

/**
 * @tc.name: UnAuthenticateDevice_001
 * @tc.desc: 将UnAuthenticateDevice的intFlag设置为false，设置pkgName = "com.ohos.test";其返回值为DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, UnAuthenticateDevice_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    std::string deviceId = "12345";
    int ret = DeviceManagerService::GetInstance().UnAuthenticateDevice(pkgName, deviceId);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: UnAuthenticateDevice_002
 * @tc.desc:  Set intFlag for UnAuthenticateDevice to True and pkgName to null; The return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, UnAuthenticateDevice_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName;
    std::string deviceId = "12345";
    int ret = DeviceManagerService::GetInstance().UnAuthenticateDevice(pkgName, deviceId);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: UnAuthenticateDevice_003
 * @tc.desc: Set intFlag for UnAuthenticateDevice to true and pkgName to com.ohos.test; set deviceId  null ，The return
 * value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, UnAuthenticateDevice_003, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "com.ohos.test";
    std::string deviceId;
    int ret = DeviceManagerService::GetInstance().UnAuthenticateDevice(pkgName, deviceId);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: VerifyAuthentication_001
 * @tc.desc: Set intFlag for VerifyAuthentication to false and  set authParam = "jdjjjj"，The return value is
 * DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, VerifyAuthentication_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string authParam = "jdjjjj";
    int ret = DeviceManagerService::GetInstance().VerifyAuthentication(authParam);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: GetUdidByNetworkId_001
 * @tc.desc: Make success for GetUdidByNetworkId，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUdidByNetworkId_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "com.ohos.test";
    std::string netWorkId = "";
    std::string udid = "";
    int ret = DeviceManagerService::GetInstance().GetUdidByNetworkId(pkgName, netWorkId, udid);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetUdidByNetworkId_002
 * @tc.desc: Make not init for GetUdidByNetworkId，The return value is
 * DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUdidByNetworkId_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    std::string netWorkId = "";
    std::string udid = "";
    int ret = DeviceManagerService::GetInstance().GetUdidByNetworkId(pkgName, netWorkId, udid);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: GetUdidByNetworkId_003
 * @tc.desc: Make pkgName empty for GetUdidByNetworkId，The return value is
 * DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUdidByNetworkId_003, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "";
    std::string netWorkId = "";
    std::string udid = "";
    int ret = DeviceManagerService::GetInstance().GetUdidByNetworkId(pkgName, netWorkId, udid);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: GetUuidByNetworkId_001
 * @tc.desc: Make success for GetUuidByNetworkId，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUuidByNetworkId_001, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "com.ohos.test";
    std::string netWorkId = "";
    std::string uuid = "";
    int ret = DeviceManagerService::GetInstance().GetUuidByNetworkId(pkgName, netWorkId, uuid);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetUuidByNetworkId_002
 * @tc.desc: Make not init for GetUuidByNetworkId，The return value is
 * DM_NOT_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUuidByNetworkId_002, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = false;
    std::string pkgName = "com.ohos.test";
    std::string netWorkId = "";
    std::string uuid = "";
    int ret = DeviceManagerService::GetInstance().GetUuidByNetworkId(pkgName, netWorkId, uuid);
    EXPECT_EQ(ret, DM_NOT_INIT);
}

/**
 * @tc.name: GetUuidByNetworkId_003
 * @tc.desc: Make pkgName empty for GetUuidByNetworkId，The return value is
 * DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetUuidByNetworkId_003, testing::ext::TestSize.Level0)
{
    DeviceManagerService::GetInstance().intFlag_ = true;
    std::string pkgName = "";
    std::string netWorkId = "";
    std::string uuid = "";
    int ret = DeviceManagerService::GetInstance().GetUuidByNetworkId(pkgName, netWorkId, uuid);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: GetFaParam_001
 * @tc.desc: Make success for GetFaParam，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetFaParam_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    DmAuthParam authParam;
    int ret = DeviceManagerService::GetInstance().GetFaParam(pkgName, authParam);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetFaParam_002
 * @tc.desc: Make pkgName empty for GetFaParam, The return value is
 * DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, GetFaParam_002, testing::ext::TestSize.Level0)
{
    std::string pkgName = "";
    DmAuthParam authParam;
    int ret = DeviceManagerService::GetInstance().GetFaParam(pkgName, authParam);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: SetUserOperation_001
 * @tc.desc: Make success for SetUserOperation，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, SetUserOperation_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    int32_t action = 0;
    int ret = DeviceManagerService::GetInstance().SetUserOperation(pkgName, action);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: SetUserOperation_002
 * @tc.desc: Make pkgName empty for SetUserOperation，The return value is
 * DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, SetUserOperation_002, testing::ext::TestSize.Level0)
{
    std::string pkgName = "";
    int32_t action = 0;
    int ret = DeviceManagerService::GetInstance().SetUserOperation(pkgName, action);
    EXPECT_EQ(ret, DM_INPUT_PARA_EMPTY);
}

/**
 * @tc.name: RegisterDevStateCallback_001
 * @tc.desc: Make success for RegisterDevStateCallback，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, RegisterDevStateCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string extra = "extra";
    int ret = DeviceManagerService::GetInstance().RegisterDevStateCallback(pkgName, extra);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: UnRegisterDevStateCallback_001
 * @tc.desc: Make success for UnRegisterDevStateCallback，The return value is
 * DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerServiceTest, UnRegisterDevStateCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string extra = "extra";
    int ret = DeviceManagerService::GetInstance().UnRegisterDevStateCallback(pkgName, extra);
    EXPECT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
