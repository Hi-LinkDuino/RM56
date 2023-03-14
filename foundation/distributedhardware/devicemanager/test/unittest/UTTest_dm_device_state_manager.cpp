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

#include "dm_log.h"
#include "dm_constants.h"
#include "dm_adapter_manager.h"
#include "ipc_notify_device_state_req.h"
#include "ipc_notify_auth_result_req.h"
#include "ipc_notify_verify_auth_result_req.h"
#include "dm_device_state_manager.h"
#include "ipc_notify_device_found_req.h"
#include "ipc_notify_discover_result_req.h"
#include "hichain_connector.h"
#include "UTTest_dm_device_state_manager.h"

namespace OHOS {
namespace DistributedHardware {
void DmDeviceStateManagerTest::SetUp()
{
}

void DmDeviceStateManagerTest::TearDown()
{
}

void DmDeviceStateManagerTest::SetUpTestCase()
{
}

void DmDeviceStateManagerTest::TearDownTestCase()
{
}
namespace {
std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener_ = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<DmDeviceStateManager> dmDeviceStateManager =
    std::make_shared<DmDeviceStateManager>(softbusConnector, listener_, hiChainConnector_);

/**
 * @tc.name: DmDeviceStateManager_001
 * @tc.desc: set DmDeviceStateManager to tne new pointer,and it's not nullptr
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, DmDeviceStateManager_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmDeviceStateManager> p = std::make_shared<DmDeviceStateManager>(softbusConnector, listener_,
        hiChainConnector_);
    ASSERT_NE(p, nullptr);
}

/**
 * @tc.name: DmDeviceStateManager_002
 * @tc.desc: set DmDeviceStateManager to tne new pointer,it's not nullptr and delete it
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, DmDeviceStateManager_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmDeviceStateManager> p = std::make_shared<DmDeviceStateManager>(softbusConnector, listener_,
        hiChainConnector_);
    p.reset();
    EXPECT_EQ(p, nullptr);
}

/**
 * @tc.name: DestructDmDeviceStateManager_001
 * @tc.desc: set DmDeviceStateManager to tne new pointer,and it's not nullptr
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, DestructDmDeviceStateManager_001, testing::ext::TestSize.Level0)
{
    dmDeviceStateManager->~DmDeviceStateManager();
}

/**
 * @tc.name: OnDeviceOnline_001
 * @tc.desc: set info.deviceId to some para, and return it
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, OnDeviceOnline_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    DmDeviceInfo info;
    strcpy_s(info.deviceId, DM_MAX_DEVICE_ID_LEN, "123");
    dmDeviceStateManager->OnDeviceOnline(pkgName, info);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceStateReq>(listener_->ipcServerListener_.req_);
    DmDeviceInfo ret = pReq->GetDeviceInfo();
    int result = strcmp(info.deviceId, ret.deviceId);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: OnDeviceChanged_001
 * @tc.desc: set info.deviceId to some para, and return it
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, OnDeviceChanged_001, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    DmDeviceInfo info;
    strcpy_s(info.deviceId, DM_MAX_DEVICE_ID_LEN, "123");
    dmDeviceStateManager->OnDeviceChanged(pkgName, info);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceStateReq>(listener_->ipcServerListener_.req_);
    DmDeviceInfo ret = pReq->GetDeviceInfo();
    int result = strcmp(info.deviceId, ret.deviceId);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: OnDeviceReady_001
 * @tc.desc: set info.deviceId to some para, and return it
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, OnProfileReady_001, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    std::string deviceId;
    DmDeviceInfo info;
    strcpy_s(info.deviceId, DM_MAX_DEVICE_ID_LEN, "123");
    dmDeviceStateManager->OnProfileReady(pkgName, deviceId);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceStateReq>(listener_->ipcServerListener_.req_);
    DmDeviceInfo ret = pReq->GetDeviceInfo();
    int result = strcmp(info.deviceId, ret.deviceId);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: OnDeviceReady_001
 * @tc.desc: set info.deviceId to 123,and call OnDeviceReady ，change info.deviceId to 4
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, OnDeviceReady_001, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    DmDeviceInfo info;
    strcpy_s(info.deviceId, DM_MAX_DEVICE_ID_LEN, "123");
    dmDeviceStateManager->OnDeviceReady(pkgName, info);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceStateReq>(listener_->ipcServerListener_.req_);
    DmDeviceInfo ret = pReq->GetDeviceInfo();
    int result = strcmp(info.deviceId, ret.deviceId);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: OnDeviceChanged_001
 * @tc.desc: set info.deviceId to 123,and call OnDeviceChanged ，change info.deviceId to 4
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, OnDeviceChanged_002, testing::ext::TestSize.Level0)
{
    std::string pkgName;
    DmDeviceInfo info;
    strcpy_s(info.deviceId, DM_MAX_DEVICE_ID_LEN, "123");
    dmDeviceStateManager->OnDeviceChanged(pkgName, info);
    std::shared_ptr<IpcNotifyDeviceStateReq> pReq =
        std::static_pointer_cast<IpcNotifyDeviceStateReq>(listener_->ipcServerListener_.req_);
    DmDeviceInfo ret = pReq->GetDeviceInfo();
    int result = strcmp(info.deviceId, ret.deviceId);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: RegisterSoftbusStateCallback_001
 * @tc.desc: call  RegisterSoftbusStateCallback and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, RegisterSoftbusStateCallback_001, testing::ext::TestSize.Level0)
{
    int ret = dmDeviceStateManager->RegisterSoftbusStateCallback();
    EXPECT_EQ(ret, DM_OK);
}
/**
 * @tc.name: RegisterProfileListener_001
 * @tc.desc: call RegisterProfileListener and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, RegisterProfileListener_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    DmDeviceInfo info = {
        .deviceId = "123",
        .deviceName = "asda",
        .deviceTypeId = 1,
    };
    dmDeviceStateManager->RegisterOffLineTimer(info);
    int ret = dmDeviceStateManager->RegisterProfileListener(pkgName, info);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: PostDeviceOnline_001
 * @tc.desc: call PostDeviceOnline
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, PostDeviceOnline_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    DmDeviceInfo info = {
        .deviceId = "123",
        .deviceName = "asda",
        .deviceTypeId = 1,
    };
    dmDeviceStateManager->PostDeviceOnline(pkgName, info);
}

/**
 * @tc.name: PostDeviceOffline_001
 * @tc.desc: call PostDeviceOffline
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, PostDeviceOffline_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    DmDeviceInfo info = {
        .deviceId = "123",
        .deviceName = "asda",
        .deviceTypeId = 1,
    };
    dmDeviceStateManager->PostDeviceOffline(pkgName, info);
}

/**
 * @tc.name: RegisterDevStateCallback_001
 * @tc.desc: call RegisterDevStateCallback
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, RegisterDevStateCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    std::string extra = "ahaha";
    dmDeviceStateManager->RegisterDevStateCallback(pkgName, extra);
}

/**
 * @tc.name: UnRegisterDevStateCallback_001
 * @tc.desc: call UnRegisterDevStateCallback
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, UnRegisterDevStateCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "123";
    std::string extra;
    dmDeviceStateManager->UnRegisterDevStateCallback(pkgName, extra);
}

/**
 * @tc.name: RegisterOffLineTimer_001
 * @tc.desc: call RegisterOffLineTimer
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmDeviceStateManagerTest, RegisterOffLineTimer_001, testing::ext::TestSize.Level0)
{
    DmDeviceInfo info = {
        .deviceId = "123",
        .deviceName = "asda",
        .deviceTypeId = 1,
    };
    dmDeviceStateManager->RegisterOffLineTimer(info);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
