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
#include "bluetooth_pbap_server.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class PbapObserverCommon : public PbapObserver {
public:
    PbapObserverCommon() = default;
    virtual ~PbapObserverCommon() = default;
    
    void OnServiceConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}
    void OnServicePermission(const BluetoothRemoteDevice &device) {}
    void OnServicePasswordRequired(const BluetoothRemoteDevice &device,
        const std::vector<uint8_t> &description, uint8_t charset, bool fullAccess = true) {}

private:
};

static PbapObserverCommon observer_;
static PbapServer *profile_;

class PbapServerTest : public testing::Test {
public:
    PbapServerTest()
    {}
    ~PbapServerTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};


void PbapServerTest::SetUpTestCase(void) {}
void PbapServerTest::TearDownTestCase(void) {}

void PbapServerTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void PbapServerTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}

/*
 * @tc.number: PbapServerUnit001
 * @tc.name: Disconnect
 * @tc.desc: Release the connection from pbapserver device.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_Disconnect start";
 
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_Disconnect end";
}

/*
 * @tc.number: PbapServerUnit002
 * @tc.name: RegisterObserver
 * @tc.desc: Register PbapServer observer instance.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_RegisterObserver start";
    profile_ = PbapServer::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: PbapServerUnit003
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister PbapServer observer instance.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_DeregisterObserver start";
    profile_ = PbapServer::GetProfile();
    profile_->DeregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: PbapServerUnit004
 * @tc.name: GetConnectedDevices
 * @tc.desc: Get remote PbapServer device list which are in the connected state.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetConnectedDevices start";
    profile_ = PbapServer::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: PbapServerUnit005
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get remote PbapServer device list which are in the specified states.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetDevicesByStates start";
    profile_ = PbapServer::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: PbapServerUnit006
 * @tc.name: GetDeviceState
 * @tc.desc: get the state with the specified remote device
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetDeviceState start";
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetDeviceState(device);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: PbapServerUnit007
 * @tc.name: SetConnectionStrategy
 * @tc.desc: Set the connection policy of the specified device.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_SetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_SetConnectionStrategy start";
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    int strategy = 0;
    bool isOk = profile_->SetConnectionStrategy(device, strategy);
    EXPECT_EQ(isOk, false);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_SetConnectionStrategy end";
}

/*
 * @tc.number: PbapServerUnit008
 * @tc.name: GetConnectionStrategy
 * @tc.desc: Get the connection policy of the specified device.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_GetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetConnectionStrategy start";
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetConnectionStrategy(device);
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GetConnectionStrategy end";
}

/*
 * @tc.number: PbapServerUnit009
 * @tc.name: GrantPermission
 * @tc.desc: Set whether to authorize the connection.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_GrantPermission, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GrantPermission start";
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    bool allow = true;
    bool save = false;
    profile_->GrantPermission(device, allow, save);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_GrantPermission end";
}

/*
 * @tc.number: PbapServerUnit010
 * @tc.name: SetDevicePassword
 * @tc.desc: Set device's password. please call after OnServicePasswordRequired event.
*/
HWTEST_F(PbapServerTest, PbapServer_UnitTest_SetDevicePassword, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_SetDevicePassword start";
    profile_ = PbapServer::GetProfile();
    BluetoothRemoteDevice device;
    string password = "123";
    string userId = "456";
    int state = profile_->SetDevicePassword(device, password, userId);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapServer_UnitTest_SetDevicePassword end";
}
}  // namespace Bluetooth
}  // namespace OHOS