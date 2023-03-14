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

#include "dm_constants.h"
#include "dm_device_state_manager.h"
#include "profile_adapter.h"
#include "profile_connector_callback.h"
#include "profile_connector.h"
#include "device_profile_adapter.h"
#include "distributed_device_profile_client.h"
#include "UTTest_device_profile_adapter.h"

namespace OHOS {
namespace DistributedHardware {
void DeviceProfileAdapterTest::SetUp()
{
}
void DeviceProfileAdapterTest::TearDown()
{
}
void DeviceProfileAdapterTest::SetUpTestCase()
{
}
void DeviceProfileAdapterTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<HiChainConnector> hiChainConnector = std::make_shared<HiChainConnector>();
std::shared_ptr<DmDeviceStateManager> deviceStateMgr = std::make_shared<DmDeviceStateManager>(softbusConnector,
                                                                                              listener,
                                                                                              hiChainConnector);
/**
 * @tc.name: RegisterProfileListener_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceProfileAdapterTest, RegisterProfileListener_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string deviceId = "deviceId";
    std::shared_ptr<DeviceProfileAdapter> deviceProfileAdapter = std::make_shared<DeviceProfileAdapter>();

    int32_t ret = DM_OK;
    ret = deviceProfileAdapter->RegisterProfileListener(pkgName, deviceId, deviceStateMgr);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: UnRegisterProfileListener_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceProfileAdapterTest, UnRegisterProfileListener_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<DeviceProfileAdapter> deviceProfileAdapter = std::make_shared<DeviceProfileAdapter>();
    
    int32_t ret = DM_OK;
    ret = deviceProfileAdapter->UnRegisterProfileListener(pkgName);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnProfileChanged_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceProfileAdapterTest, OnProfileChanged_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string deviceId = "deviceId";
    std::shared_ptr<DeviceProfileAdapter> deviceProfileAdapter = std::make_shared<DeviceProfileAdapter>();
    
    int32_t ret = DM_OK;
    deviceProfileAdapter->OnProfileChanged(pkgName, deviceId);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnProfileComplete_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceProfileAdapterTest, OnProfileComplete_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string deviceId = "deviceId";
    std::shared_ptr<DeviceProfileAdapter> deviceProfileAdapter = std::make_shared<DeviceProfileAdapter>();
    
    int32_t ret = DM_OK;
    deviceProfileAdapter->OnProfileComplete(pkgName, deviceId);
    ASSERT_EQ(ret, DM_OK);
}
}
} // namespace DistributedHardware
} // namespace OHOS
