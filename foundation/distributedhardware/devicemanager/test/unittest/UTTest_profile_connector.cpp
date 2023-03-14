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
#include "UTTest_profile_connector.h"

namespace OHOS {
namespace DistributedHardware {
void ProfileConnectorTest::SetUp()
{
}
void ProfileConnectorTest::TearDown()
{
}
void ProfileConnectorTest::SetUpTestCase()
{
}
void ProfileConnectorTest::TearDownTestCase()
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
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, RegisterProfileCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::string deviceId = "deviceId";
    std::string profileSoName = "libdevicemanagerext_profile.z.so";

    DeviceProfileAdapter * deviceProfileAdapter = new DeviceProfileAdapter();

    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    int32_t ret = DM_OK;
    ret = profileConnector_->RegisterProfileCallback(pkgName, deviceId, deviceProfileAdapter);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, UnRegisterProfileCallback_001, testing::ext::TestSize.Level0)
{
    std::string pkgName = "com.ohos.test";
    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    int32_t ret = DM_OK;
    ret = profileConnector_->UnRegisterProfileCallback(pkgName);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, SubscribeProfileEvents_001, testing::ext::TestSize.Level0)
{
    std::string deviceId = "deviceId";
    std::list<std::string> serviceIds;
    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    int32_t ret = DM_OK;
    ret = profileConnector_->SubscribeProfileEvents(serviceIds, deviceId);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, UnSubscribeProfileEvents_001, testing::ext::TestSize.Level0)
{
    std::string deviceId = "deviceId";
    std::list<std::string> serviceIds;
    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    int32_t ret = DM_OK;
    profileConnector_->SubscribeProfileEvents(serviceIds, deviceId);
    ret = profileConnector_->UnSubscribeProfileEvents();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: OnDecisionFilterResult_002
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, UnSubscribeProfileEvents_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    int32_t ret = DM_OK;
    ret = profileConnector_->UnSubscribeProfileEvents();
    ASSERT_EQ(ret, ERR_DM_UNSUBSCRIBE_DP_EVENTS);
}

/**
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(ProfileConnectorTest, OnSyncCompleted_001, testing::ext::TestSize.Level0)
{
    DeviceProfile::SyncResult syncResults;
    std::shared_ptr<ProfileConnector> profileConnector_;
    profileConnector_ = std::make_shared<ProfileConnector>();
    profileConnector_->OnSyncCompleted(syncResults);
}
}
} // namespace DistributedHardware
} // namespace OHOS
