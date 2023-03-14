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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "bluetooth_a2dp_snk.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

constexpr int TIME = 4;

class A2dpSinkObserverCommon : public A2dpSinkObserver {
public:
    A2dpSinkObserverCommon() = default;
    virtual ~A2dpSinkObserverCommon() = default;
    
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state){}

private:
};


static A2dpSinkObserverCommon observer_;
static A2dpSink *profile_;
static BluetoothHost *host_;


class A2dpSinkTest : public testing::Test {
public:
    A2dpSinkTest()
    {}
    ~A2dpSinkTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

};


void A2dpSinkTest::SetUpTestCase(void)
{
    
}
void A2dpSinkTest::TearDownTestCase(void)
{
    
}
void A2dpSinkTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void A2dpSinkTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
    sleep(TIME);
}

/*
 * @tc.number: A2dpSink001
 * @tc.name: GetProfile
 * @tc.desc: Get a2dp sink instance
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetProfile start";
 
    profile_ = A2dpSink::GetProfile();
    
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetProfile end";
}

/*
 * @tc.number: A2dpSink002
 * @tc.name: GetPlayingState
 * @tc.desc: Get device playing state by address when peer device is on connected
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_GetPlayingState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetPlayingState start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetPlayingState(device);
    EXPECT_EQ(state, RET_NO_ERROR);
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetPlayingState end";
}

/*
 * @tc.number: A2dpSink003
 * @tc.name: Connect
 * @tc.desc: Connect to the peer bluetooth device
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_Connect start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Connect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_Connect end";
}

/*
 * @tc.number: A2dpSink004
 * @tc.name: Disconnect
 * @tc.desc: Disconnect with the peer bluetooth service
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_Disconnect start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_Disconnect end";
}

/*
 * @tc.number: A2dpSink005
 * @tc.name: RegisterObserver
 * @tc.desc: Register callback function of framework
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_RegisterObserver start";
 
    profile_ = A2dpSink::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: A2dpSink006
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister callback function of framework
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_DeregisterObserver start";
 
    profile_ = A2dpSink::GetProfile();
    profile_->DeregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: HandsFreeUnit007
 * @tc.name: GetDeviceState
 * @tc.desc: Get device connection state by address
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetDeviceState start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetDeviceState(device), 0);
    
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: HandsFreeUnit008
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get devices by connection states
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetDevicesByStates start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: HandsFreeUnit009
 * @tc.name: GetConnectStrategy
 * @tc.desc: Get connect strategy by address
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_GetConnectStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetConnectStrategy start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetConnectStrategy(device), 0);
    
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_GetConnectStrategy end";
}

/*
 * @tc.number: HandsFreeUnit010
 * @tc.name: SetConnectStrategy
 * @tc.desc: Set connect strategy by address
*/
HWTEST_F(A2dpSinkTest, A2dpSink_UnitTest_SetConnectStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_SetConnectStrategy start";
 
    profile_ = A2dpSink::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->SetConnectStrategy(device,2);
    EXPECT_EQ(isOK, false);
    
    GTEST_LOG_(INFO) << "A2dpSink_UnitTest_SetConnectStrategy end";
}
}  // namespace Bluetooth
}  // namespace OHOS