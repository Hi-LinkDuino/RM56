/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "bluetooth_a2dp_src.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

constexpr int TIME = 2;

class A2dpSourceObserverCommon : public A2dpSourceObserver {
public:
    A2dpSourceObserverCommon() = default;
    virtual ~A2dpSourceObserverCommon() = default;
    
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state){}
    virtual void OnConfigurationChanged(const BluetoothRemoteDevice &device, const A2dpCodecInfo &info, int error){}
    virtual void OnPlayingStatusChanged(const BluetoothRemoteDevice &device, int playingState, int error){}

private:
};


static A2dpSourceObserverCommon observer_;
static A2dpSource *profile_;
static BluetoothHost *host_;

class A2dpSourceTest : public testing::Test {
public:
    A2dpSourceTest()
    {}
    ~A2dpSourceTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void A2dpSourceTest::SetUpTestCase(void)
{
    
}
void A2dpSourceTest::TearDownTestCase(void)
{
    
}
void A2dpSourceTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void A2dpSourceTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
    sleep(TIME);
}

/*
 * @tc.number: A2dpSource001
 * @tc.name: GetProfile
 * @tc.desc: Get a2dp sink instance
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetProfile start";
 
    profile_ = A2dpSource::GetProfile();
    
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetProfile end";
}

/*
 * @tc.number: A2dpSource002
 * @tc.name: GetPlayingState
 * @tc.desc: Get device playing state by address when peer device is on connected
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_GetPlayingState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetPlayingState start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetPlayingState(device);
    EXPECT_EQ(state, RET_NO_ERROR);
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetPlayingState end";
}

/*
 * @tc.number: A2dpSource003
 * @tc.name: RegisterObserver
 * @tc.desc: Register callback function of framework
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_RegisterObserver start";
 
    profile_ = A2dpSource::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: A2dpSource004
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister callback function of framework
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_DeregisterObserver start";
 
    profile_ = A2dpSource::GetProfile();
    profile_->DeregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: A2dpSource005
 * @tc.name: GetDeviceState
 * @tc.desc: Get device connection state by address
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetDeviceState start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetDeviceState(device), 0);
    
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: A2dpSource006
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get devices by connection states
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetDevicesByStates start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: A2dpSource007
 * @tc.name: Connect
 * @tc.desc: Connect to the peer bluetooth device
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_Connect start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Connect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_Connect end";
}

/*
 * @tc.number: A2dpSource008
 * @tc.name: Disconnect
 * @tc.desc: Disconnect with the peer bluetooth service
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_Disconnect start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_Disconnect end";
}

/*
 * @tc.number: A2dpSource009
 * @tc.name: SetActiveSinkDevice
 * @tc.desc: Set the active sink device for audio connection.
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_SetActiveSinkDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_SetActiveSinkDevice start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->SetActiveSinkDevice(device), 0);

    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_SetActiveSinkDevice end";
}

/*
 * @tc.number: A2dpSource010
 * @tc.name: GetActiveSinkDevice
 * @tc.desc: Get the active sink device for audio connection.
*/
HWTEST_F(A2dpSourceTest, A2dpSource_UnitTest_GetActiveSinkDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetActiveSinkDevice start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device = profile_->GetActiveSinkDevice();
    
    GTEST_LOG_(INFO) << "A2dpSource_UnitTest_GetActiveSinkDevice end";
}

/*
 * @tc.number: A2dpSource011
 * @tc.name: GetConnectStrategy
 * @tc.desc: Get connect strategy by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_GetConnectStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetConnectStrategy start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetConnectStrategy(device), 0);
    
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetConnectStrategy end";
}

/*
 * @tc.number: A2dpSource012
 * @tc.name: SetConnectStrategy
 * @tc.desc: Set connect strategy by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_SetConnectStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetConnectStrategy start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->SetConnectStrategy(device,2);
    EXPECT_EQ(isOK, false);
    
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetConnectStrategy end";
}

/*
 * @tc.number: A2dpSource013
 * @tc.name: GetCodecStatus
 * @tc.desc: Get codec status by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_GetCodecStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetCodecStatus start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    A2dpCodecStatus ret = profile_->GetCodecStatus(device);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetCodecStatus end";
}

/*
 * @tc.number: A2dpSource014
 * @tc.name: SetCodecPreference
 * @tc.desc: Set codec preference by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_SetCodecPreference, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetCodecPreference start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    A2dpCodecInfo info;
    info.codecPriority = 0;
    info.codecType = 1;
    info.sampleRate = 2;
    info.bitsPerSample = 3;
    info.channelMode = 4;
    info.codecSpecific1 = 5;
    info.codecSpecific2 = 6;
    info.codecSpecific3 = 7;
    info.codecSpecific4 = 8;
    EXPECT_EQ(profile_->SetCodecPreference(device, info), 0);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetCodecPreference end";
}

/*
 * @tc.number: A2dpSource015
 * @tc.name: SwitchOptionalCodecs
 * @tc.desc: Switch optional codecs by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_SwitchOptionalCodecs, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SwitchOptionalCodecs start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    bool isEnable = true;
    profile_->SwitchOptionalCodecs(device, isEnable);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SwitchOptionalCodecs end";
}

/*
 * @tc.number: A2dpSource016
 * @tc.name: GetOptionalCodecsSupportState
 * @tc.desc: Get optional codecs support state by address
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_GetOptionalCodecsSupportState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetOptionalCodecsSupportState start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetOptionalCodecsSupportState(device), 0);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetOptionalCodecsSupportState end";
}

/*
 * @tc.number: A2dpSource017
 * @tc.name: StartPlaying
 * @tc.desc: Start playing
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_StartPlaying, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_StartPlaying start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->StartPlaying(device), 0);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_StartPlaying end";
}

/*
 * @tc.number: A2dpSource018
 * @tc.name: SuspendPlaying
 * @tc.desc: Suspend playing
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_SuspendPlaying, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SuspendPlaying start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->SuspendPlaying(device), 0);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SuspendPlaying end";
}

/*
 * @tc.number: A2dpSource019
 * @tc.name: StopPlaying
 * @tc.desc: Stop playing
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_StopPlaying, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_StopPlaying start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->StopPlaying(device), 0);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_StopPlaying end";
}

/*
 * @tc.number: A2dpSource020
 * @tc.name: SetAudioConfigure
 * @tc.desc: Set audio configure
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_SetAudioConfigure, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetAudioConfigure start";
 
    profile_ = A2dpSource::GetProfile();
    BluetoothRemoteDevice device;
    profile_->SetAudioConfigure(device, 1, 2, 3);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_SetAudioConfigure end";
}

/*
 * @tc.number: A2dpSource021
 * @tc.name: WriteFrame
 * @tc.desc: Write frame data to a2dp 
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_WriteFrame, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_WriteFrame start";

    uint8_t str[4] = {'1', '2', '3', '4'};
    profile_ = A2dpSource::GetProfile();
    profile_->WriteFrame(str, 4);

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_WriteFrame end";
}

/*
 * @tc.number: A2dpSource022
 * @tc.name: GetRenderPosition
 * @tc.desc: Get delayValue, sendDataSize and timeStamp from a2dp
*/
HWTEST_F(A2dpSourceTest, A2dpSourceTest_UnitTest_GetRenderPosition, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetRenderPosition start";

    uint16_t delayValue = 0;
    uint16_t sendDataSize = 0;
    uint32_t timeStamp = 0;
    profile_ = A2dpSource::GetProfile();
    profile_->GetRenderPosition(delayValue, sendDataSize, timeStamp);
    GTEST_LOG_(INFO) << "delayValue =" << delayValue << ",sendDataSize =" << sendDataSize << 
        ",timeStamp =" << timeStamp;

    GTEST_LOG_(INFO) << "A2dpSourceTest_UnitTest_GetRenderPosition end";
}
}  // namespace Bluetooth
}  // namespace OHOS