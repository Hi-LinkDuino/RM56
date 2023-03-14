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
#include "bluetooth_avrcp_ct.h"
#include "bluetooth_avrcp_tg.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class AvrcpTargetObserverCommon : public AvrcpTarget::IObserver {
public:
    AvrcpTargetObserverCommon() = default;
    virtual ~AvrcpTargetObserverCommon() = default;
    
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state){}
private:
};

static AvrcpTargetObserverCommon observer_;
static AvrcpTarget *profile_;

class AvrcpTargetTest : public testing::Test {
public:
    AvrcpTargetTest()
    {}
    ~AvrcpTargetTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};


void AvrcpTargetTest::SetUpTestCase(void)
{}
void AvrcpTargetTest::TearDownTestCase(void)
{}
void AvrcpTargetTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void AvrcpTargetTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}

/*
 * @tc.number: AvrcpTarget001
 * @tc.name: GetProfile
 * @tc.desc: Gets the static instance of the <b>AvrcpTarget</b> class.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetProfile start";
 
    profile_ = AvrcpTarget::GetProfile();
    
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetProfile end";
}

/*
 * @tc.number: AvrcpTarget002
 * @tc.name: GetConnectedDevices
 * @tc.desc: Gets the connected devices.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectedDevices start";
 
    profile_ = AvrcpTarget::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: AvrcpTarget003
 * @tc.name: GetDeviceState
 * @tc.desc: Gets the connection state of the specified bluetooth device.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDeviceState start";
 
    profile_ = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetDeviceState(device);
    EXPECT_EQ(state, 0);
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: AvrcpTarget004
 * @tc.name: RegisterObserver
 * @tc.desc: Registers the observer.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_RegisterObserver start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: AvrcpTarget005
 * @tc.name: UnregisterObserver
 * @tc.desc: Unregisters the observer.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_UnregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_DeregisterObserver start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->UnregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: AvrcpTarget006
 * @tc.name: SetActiveDevice
 * @tc.desc: Sets the active device.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_SetActiveDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetActiveDevice start";
 
    profile_ = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device;
    profile_->SetActiveDevice(device);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetActiveDevice end";
}

/*
 * @tc.number: AvrcpTarget007
 * @tc.name: GetDevicesByStates
 * @tc.desc: Gets the devices of the specified states.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDevicesByStates start";
 
    profile_ = AvrcpTarget::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: AvrcpTarget008
 * @tc.name: Connect
 * @tc.desc: Connects to the AVRCP CT service.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Connect start";
 
    profile_ = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device;
    profile_->Connect(device);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Connect end";
}

/*
 * @tc.number: AvrcpTarget009
 * @tc.name: Disconnect
 * @tc.desc: Disconnects from the AVRCP CT service.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Disconnect start";
 
    profile_ = AvrcpTarget::GetProfile();
    BluetoothRemoteDevice device;
    profile_->Disconnect(device);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Disconnect end";
}

/*
 * @tc.number: AvrcpTarget010
 * @tc.name: NotifyPlaybackStatusChanged
 * @tc.desc: Notifies the playback status is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyPlaybackStatusChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlaybackStatusChanged start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->NotifyPlaybackStatusChanged(0,0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlaybackStatusChanged end";
}

/*
 * @tc.number: AvrcpTarget011
 * @tc.name: NotifyTrackChanged
 * @tc.desc: Notifies the track is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyTrackChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackChanged start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->NotifyTrackChanged(0,0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackChanged end";
}

/*
 * @tc.number: AvrcpTarget012
 * @tc.name: NotifyTrackReachedEnd
 * @tc.desc: Notifies the track reached end is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyTrackReachedEnd, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackReachedEnd start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->NotifyTrackReachedEnd(0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackReachedEnd end";
}

/*
 * @tc.number: AvrcpTarget013
 * @tc.name: NotifyTrackReachedStart
 * @tc.desc: Notifies the track reached start is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyTrackReachedStart, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackReachedStart start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->NotifyTrackReachedStart(0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyTrackReachedStart end";
}

/*
 * @tc.number: AvrcpTarget014
 * @tc.name: NotifyPlaybackPosChanged
 * @tc.desc: Notifies the player application setting is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyPlaybackPosChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlaybackPosChanged start";
 
    profile_ = AvrcpTarget::GetProfile();
    profile_->NotifyPlaybackPosChanged(0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlaybackPosChanged end";
}

/*
 * @tc.number: AvrcpTarget015
 * @tc.name: NotifyPlayerAppSettingChanged
 * @tc.desc: Notifies the player application setting is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyPlayerAppSettingChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlayerAppSettingChanged start";
 
    profile_ = AvrcpTarget::GetProfile();
    vector<uint8_t> attributes = {0};
    vector<uint8_t> values = {0};
    profile_->NotifyPlayerAppSettingChanged(attributes, values);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyPlayerAppSettingChanged end";
}

/*
 * @tc.number: AvrcpTarget016
 * @tc.name: NotifyNowPlayingContentChanged
 * @tc.desc: Notifies the addressed player is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyNowPlayingContentChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyNowPlayingContentChanged start";
 
    profile_ = AvrcpTarget::GetProfile();

    profile_->NotifyNowPlayingContentChanged();

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyNowPlayingContentChanged end";
}

/*
 * @tc.number: AvrcpTarget017
 * @tc.name: NotifyAvailablePlayersChanged
 * @tc.desc: Notifies that has the available player.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyAvailablePlayersChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyAvailablePlayersChanged start";
 
    profile_ = AvrcpTarget::GetProfile();

    profile_->NotifyAvailablePlayersChanged();

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyAvailablePlayersChanged end";
}

/*
 * @tc.number: AvrcpTarget018
 * @tc.name: NotifyAddressedPlayerChanged
 * @tc.desc: Notifies the addressed player is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyAddressedPlayerChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyAddressedPlayerChanged start";
 
    profile_ = AvrcpTarget::GetProfile();

    profile_->NotifyAddressedPlayerChanged(0 ,0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyAddressedPlayerChanged end";
}

/*
 * @tc.number: AvrcpTarget019
 * @tc.name: NotifyUidChanged
 * @tc.desc: Notifies the uids is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyUidChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyUidChanged start";
 
    profile_ = AvrcpTarget::GetProfile();

    profile_->NotifyUidChanged(0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyUidChanged end";
}

/*
 * @tc.number: AvrcpTarget020
 * @tc.name: NotifyVolumeChanged
 * @tc.desc: Notifies the absolute volume is changed.
*/
HWTEST_F(AvrcpTargetTest, Avrcp_UnitTest_NotifyVolumeChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyVolumeChanged start";
 
    profile_ = AvrcpTarget::GetProfile();

    profile_->NotifyVolumeChanged(0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_NotifyVolumeChanged end";
}
}  // namespace Bluetooth
}  // namespace OHOS