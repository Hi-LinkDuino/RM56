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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "bluetooth_map_mse.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class MapServerObserverCommon : public MapServerObserver {
public:
    MapServerObserverCommon() = default;
    virtual ~MapServerObserverCommon() = default;
    
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}
    void OnPermission(const BluetoothRemoteDevice &device) {}

private:
};

static MapServerObserverCommon observer_;
static MapServer *profile_;

class MapServerTest : public testing::Test {
public:
    MapServerTest()
    {}
    ~MapServerTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};


void MapServerTest::SetUpTestCase(void) {}
void MapServerTest::TearDownTestCase(void) {}

void MapServerTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void MapServerTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}
/*
 * @tc.number: MapServerUnit001
 * @tc.name: Disconnect
 * @tc.desc: Disconnect bluetooth connection service.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_Disconnect start";
 
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_Disconnect end";
}

/*
 * @tc.number: MapServerUnit002
 * @tc.name: RegisterObserver
 * @tc.desc: Register the callback of map server.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_RegisterObserver start";
    profile_ = MapServer::GetProfile();
    profile_->RegisterObserver(observer_);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: MapServerUnit003
 * @tc.name: DeregisterObserver
 * @tc.desc: Unregister the callback of map server.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_DeregisterObserver start";
    profile_ = MapServer::GetProfile();
    profile_->DeregisterObserver(observer_);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: MapServerUnit004
 * @tc.name: GetState
 * @tc.desc: Get the connection status of the server.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GetState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetState start";
    profile_ = MapServer::GetProfile();
    int ret = profile_->GetState();
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetState end";
}

/*
 * @tc.number: MapServerUnit005
 * @tc.name: IsConnected
 * @tc.desc: Get whether bluetooth device is connected.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_IsConnected, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_IsConnected start";
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    bool isOk = profile_->IsConnected(device);
    EXPECT_EQ(isOk, false);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_IsConnected end";
}

/*
 * @tc.number: MapServerUnit006
 * @tc.name: GetConnectedDevices
 * @tc.desc: Get the list of connected bluetooth devices.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectedDevices start";
    profile_ = MapServer::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: MapServerUnit007
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get the device list through the connection status.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetDevicesByStates start";
    profile_ = MapServer::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: MapServerUnit008
 * @tc.name: GetConnectionState
 * @tc.desc: Get the connection status of the specified device.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GetConnectionState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectionState start";
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetConnectionState(device);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectionState end";
}

/*
 * @tc.number: MapServerUnit009
 * @tc.name: SetConnectionStrategy
 * @tc.desc: Set the connection policy of the specified device.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_SetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_SetConnectionStrategy start";
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    int strategy = 0;
    bool isOk = profile_->SetConnectionStrategy(device, strategy);
    EXPECT_EQ(isOk, false);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_SetConnectionStrategy end";
}

/*
 * @tc.number: MapServerUnit010
 * @tc.name: GetConnectionStrategy
 * @tc.desc: Get the connection policy of the specified device.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectionStrategy start";
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetConnectionStrategy(device);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GetConnectionStrategy end";
}

/*
 * @tc.number: MapServerUnit011
 * @tc.name: GrantPermission
 * @tc.desc: Set whether to authorize the connection.
*/
HWTEST_F(MapServerTest, MapServer_UnitTest_GrantPermission, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GrantPermission start";
    profile_ = MapServer::GetProfile();
    BluetoothRemoteDevice device;
    bool allow = true;
    bool save = false;
    profile_->GrantPermission(device, allow, save);
    GTEST_LOG_(INFO) << "MapServer_UnitTest_GrantPermission end";
}
}  // namespace Bluetooth
}  // namespace OHOS