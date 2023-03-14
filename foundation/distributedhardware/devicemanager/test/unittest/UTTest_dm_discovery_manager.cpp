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

#include <iostream>
#include <string>
#include <unistd.h>

#include "dm_log.h"
#include "dm_constants.h"
#include "dm_anonymous.h"
#include "ipc_server_listener.h"
#include "device_manager_service_listener.h"
#include "discovery_service.h"
#include "UTTest_dm_discovery_manager.h"

namespace OHOS {
namespace DistributedHardware {
void DmDiscoveryManagerTest::SetUp()
{
}

void DmDiscoveryManagerTest::TearDown()
{
}

void DmDiscoveryManagerTest::SetUpTestCase()
{
}

void DmDiscoveryManagerTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector_ = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener_ = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<DmDiscoveryManager> discoveryMgr_ = std::make_shared<DmDiscoveryManager>(softbusConnector_, listener_);

/**
 * @tc.name: DmDiscoveryManager_001
 * @tc.desc: Test whether the DmDiscoveryManager function can generate a new pointer
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, DmDiscoveryManager_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmDiscoveryManager> Test = std::make_shared<DmDiscoveryManager>(softbusConnector_, listener_);
    ASSERT_NE(Test, nullptr);
}

/**
 * @tc.name: DmDiscoveryManager_002
 * @tc.desc: Test whether the DmDiscoveryManager function can delete a new pointer
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, DmDiscoveryManager_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmDiscoveryManager> Test = std::make_shared<DmDiscoveryManager>(softbusConnector_, listener_);
    Test.reset();
    EXPECT_EQ(Test, nullptr);
}

/**
 * @tc.name:StartDeviceDiscovery_001
 * @tc.desc: keeping pkgame unchanged, call StartDeviceDiscovery twice
 *           so that its discoveryQueue is not empty and return DM_DISCOVERY_REPEATED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, StartDeviceDiscovery_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    DmSubscribeInfo subscribeInfo;
    const std::string extra;
    discoveryMgr_->StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    int32_t ret = discoveryMgr_->StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    EXPECT_EQ(ret, DM_DISCOVERY_REPEATED);
}

/**
 * @tc.name:StartDeviceDiscovery_002
 * @tc.desc: pkgame changed, call StartDeviceDiscovery twice
 *           so that its discoveryQueue is not empty and return DM_DISCOVERY_REPEATED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, StartDeviceDiscovery_002, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    std::string extra;
    DmSubscribeInfo subscribeInfo;
    discoveryMgr_->StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    pkgName = "com.ohos.helloworld.new";
    int32_t ret = discoveryMgr_->StartDeviceDiscovery(pkgName, subscribeInfo, extra);
    ASSERT_EQ(ret, DM_DISCOVERY_FAILED);
}

/**
 * @tc.name: OnDeviceFound_001
 * @tc.desc: The OnDeviceFound function does the correct case and assigns pkgName
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDeviceFound_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    uint16_t aaa = 11;
    DmDiscoveryContext context { pkgName, "121110", aaa };
    discoveryMgr_->discoveryContextMap_[pkgName] = context;
    sleep(1);
    DmDeviceInfo info;
    info.deviceId[0] = '\0';
    info.deviceName[0] = '\0';
    discoveryMgr_->OnDeviceFound(pkgName, info);
    std::shared_ptr<IpcNotifyDeviceFoundReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceFoundReq>(listener_->ipcServerListener_.req_);
    int ret1 = discoveryMgr_->discoveryContextMap_.count(pkgName);
    EXPECT_EQ(ret1, 1);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: OnDeviceFound_002
 * @tc.desc: set pkgName  not null and discoveryContextMap_ null and return
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDeviceFound_002, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    DmDeviceInfo info;
    discoveryMgr_->OnDeviceFound(pkgName, info);
    int ret1 = discoveryMgr_->discoveryContextMap_.count(pkgName);
    EXPECT_EQ(ret1, 1);
    std::shared_ptr<IpcNotifyDeviceFoundReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceFoundReq>(listener_->ipcServerListener_.req_);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: OnDiscoveryFailed_001
 * @tc.desc: The OnDeviceFound function takes the wrong case and emptying pkgName
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDiscoveryFailed_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    int32_t subscribeId = 1;
    int32_t failedReason = 3;
    discoveryMgr_->OnDiscoveryFailed(pkgName, subscribeId, failedReason);
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq =
        std::static_pointer_cast<IpcNotifyDiscoverResultReq>(listener_->ipcServerListener_.req_);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: OnDiscoveryFailed_002
 * @tc.desc: The OnDeviceFound function takes the wrong case and emptying pkgName
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDiscoveryFailed_002, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    int32_t subscribeId = 1;
    int32_t failedReason = 3;
    discoveryMgr_->OnDiscoveryFailed(pkgName, subscribeId, failedReason);
    int ret1 = discoveryMgr_->discoveryContextMap_.count(pkgName);
    EXPECT_EQ(ret1, 0);
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq =
        std::static_pointer_cast<IpcNotifyDiscoverResultReq>(listener_->ipcServerListener_.req_);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: OnDiscoverySuccess_001
 * @tc.desc: The OnDeviceFound function takes the wrong case and return pkgName
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDiscoverySuccess_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    int32_t subscribeId = 1;
    discoveryMgr_->OnDiscoverySuccess(pkgName, subscribeId);
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq =
        std::static_pointer_cast<IpcNotifyDiscoverResultReq>(listener_->ipcServerListener_.req_);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: OnDiscoverySuccess_002
 * @tc.desc: set pkgName null and return  discoveryContextMap_ null and return pkgName(null)
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, OnDiscoverySuccess_002, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    int32_t subscribeId = 1;
    discoveryMgr_->OnDiscoverySuccess(pkgName, subscribeId);
    int ret1 = discoveryMgr_->discoveryContextMap_.count(pkgName);
    EXPECT_EQ(ret1, 1);
    std::shared_ptr<IpcNotifyDiscoverResultReq> pReq =
        std::static_pointer_cast<IpcNotifyDiscoverResultReq>(listener_->ipcServerListener_.req_);
    std ::string ret = pReq->GetPkgName();
    EXPECT_EQ(ret, pkgName);
}

/**
 * @tc.name: HandleDiscoveryTimeout_001
 * @tc.desc: set pkgName not null and return 1(true)
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDiscoveryManagerTest, HandleDiscoveryTimeout_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.helloworld";
    discoveryMgr_->HandleDiscoveryTimeout();
    int ret = discoveryMgr_->discoveryContextMap_.count(pkgName);
    EXPECT_EQ(ret, 1);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
