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

#include "UTTest_dm_device_info_manager.h"

#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
void DeviceManagerImplTest::SetUp()
{
}

void DeviceManagerImplTest::TearDown()
{
}

void DeviceManagerImplTest::SetUpTestCase()
{
}

void DeviceManagerImplTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener_ = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<DmDeviceInfoManager> dmDeviceStateManager = std::make_shared<DmDeviceInfoManager>(softbusConnector);

/**
 * @tc.name: DmDeviceInfoManager_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerImplTest, DmDeviceInfoManager_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmDeviceInfoManager> deviceInfoManager = std::make_shared<DmDeviceInfoManager>(softbusConnector);
    ASSERT_NE(deviceInfoManager, nullptr);
}

/**
 * @tc.name: GetTrustedDeviceList_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerImplTest, GetTrustedDeviceList_001, testing::ext::TestSize.Level0)
{
    int32_t ret = DM_OK;
    std::string pkgName = "";
    std::string extra = "";
    std::vector<DmDeviceInfo> deviceList;
    std::shared_ptr<DmDeviceInfoManager> deviceInfoManager = std::make_shared<DmDeviceInfoManager>(softbusConnector);
    ret = deviceInfoManager->GetTrustedDeviceList(pkgName, extra, deviceList);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetLocalDeviceInfo_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DeviceManagerImplTest, GetLocalDeviceInfo_001, testing::ext::TestSize.Level0)
{
    DmDeviceInfo deviceInfo;
    int32_t ret = DM_OK;
    std::shared_ptr<DmDeviceInfoManager> deviceInfoManager = std::make_shared<DmDeviceInfoManager>(softbusConnector);
    ret = deviceInfoManager->GetLocalDeviceInfo(deviceInfo);
    ASSERT_EQ(ret, DM_OK);
}
}
} // namespace DistributedHardware
} // namespace OHOS
