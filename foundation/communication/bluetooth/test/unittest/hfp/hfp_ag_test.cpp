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
#include "bluetooth_hfp_ag.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class HandsFreeAudioGatewayObserverCommon : public HandsFreeAudioGatewayObserver {
public:
    HandsFreeAudioGatewayObserverCommon() = default;
    virtual ~HandsFreeAudioGatewayObserverCommon() = default;
    
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}
    void OnScoStateChanged(const BluetoothRemoteDevice &device, int state) {}
    void OnActiveDeviceChanged(const BluetoothRemoteDevice &device) {}
    void OnHfEnhancedDriverSafetyChanged(const BluetoothRemoteDevice &device, int indValue) {}

private:
};

static HandsFreeAudioGatewayObserverCommon observer_;
static HandsFreeAudioGateway *profile_;
static BluetoothHost *host_;

class HandsFreeAudioGatewayTest : public testing::Test {
public:
    HandsFreeAudioGatewayTest()
    {}
    ~HandsFreeAudioGatewayTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void HandsFreeAudioGatewayTest::SetUpTestCase(void)
{
    
}
void HandsFreeAudioGatewayTest::TearDownTestCase(void)
{
    
}
void HandsFreeAudioGatewayTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void HandsFreeAudioGatewayTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}


/*
 * @tc.number: HandsFreeAudioGateway001
 * @tc.name: GetProfile
 * @tc.desc: Get the instance of HandsFreeAudioGateway object
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetProfile start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetProfile end";
}


/*
 * @tc.number: HandsFreeAudioGateway002
 * @tc.name: GetConnectedDevices
 * @tc.desc: Get remote HF device list which are in the connected state.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetConnectedDevices start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();


    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: HandsFreeAudioGateway003
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get remote HF device list which are in the specified states.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDevicesByStates start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: HandsFreeAudioGateway004
 * @tc.name: GetDeviceState
 * @tc.desc: Get the connection state of the specified remote HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDeviceState start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetDeviceState(device);
    EXPECT_EQ(state, 0);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDeviceState end";
}


/*
 * @tc.number: HandsFreeAudioGateway005
 * @tc.name: Connect
 * @tc.desc: Initiate the establishment of a service level connection to remote HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Connect start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Connect(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Connect end";
}


/*
 * @tc.number: HandsFreeAudioGateway006
 * @tc.name: Disconnect
 * @tc.desc: Release the audio connection from remote HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Disconnect end";
}


/*
 * @tc.number: HandsFreeAudioGateway007
 * @tc.name: GetScoState
 * @tc.desc: Get the Audio connection state of the specified remote HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetScoState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetScoState(device);
    EXPECT_EQ(state, 0);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ end";
}


/*
 * @tc.number: HandsFreeAudioGateway008
 * @tc.name: ConnectSco
 * @tc.desc: Initiate the establishment of an audio connection to remote active HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_ConnectSco, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ConnectSco start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    bool isOK = profile_->ConnectSco();
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ConnectSco end";
}


/*
 * @tc.number: HandsFreeAudioGateway009
 * @tc.name: DisconnectSco
 * @tc.desc: Release the audio connection from remote active HF device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_DisconnectSco, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DisconnectSco start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    bool isOK = profile_->DisconnectSco();
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DisconnectSco end";
}


/*
 * @tc.number: HandsFreeAudioGateway010
 * @tc.name: PhoneStateChanged
 * @tc.desc: This function used to Update changed phone call informations.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_PhoneStateChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_PhoneStateChanged start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    profile_->PhoneStateChanged(0, 0, 0, "123", 0, "");    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_PhoneStateChanged end";
}


/*
 * @tc.number: HandsFreeAudioGateway011
 * @tc.name: ClccResponse
 * @tc.desc: Send response for querying standard list current calls by remote Hf device.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_ClccResponse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ClccResponse start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    profile_->ClccResponse(0, 0, 0, 0, false, "123", 0);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ClccResponse end";
}


/*
 * @tc.number: HandsFreeAudioGateway012
 * @tc.name: OpenVoiceRecognition
 * @tc.desc: Open the voice recognition.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_OpenVoiceRecognition, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_OpenVoiceRecognition start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->OpenVoiceRecognition(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_OpenVoiceRecognition end";
}


/*
 * @tc.number: HandsFreeAudioGateway013
 * @tc.name: CloseVoiceRecognition
 * @tc.desc: Close the voice recognition.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_CloseVoiceRecognition, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_CloseVoiceRecognition start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->CloseVoiceRecognition(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_CloseVoiceRecognition end";
}


/*
 * @tc.number: HandsFreeAudioGateway014
 * @tc.name: SetActiveDevice
 * @tc.desc: Set the active device for audio connection.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_SetActiveDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_SetActiveDevice start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->SetActiveDevice(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_SetActiveDevice end";
}

/*
 * @tc.number: HandsFreeAudioGateway015
 * @tc.name: GetActiveDevice
 * @tc.desc: Set the active device for audio connection.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_GetActiveDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetActiveDevice start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    BluetoothRemoteDevice device = profile_->GetActiveDevice();
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetActiveDevice end";
}

/*
 * @tc.number: HandsFreeAudioGateway016
 * @tc.name: RegisterObserver
 * @tc.desc: Register HandsFree AudioGateway observer instance.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RegisterObserver start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: HandsFreeAudioGateway017
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister HandsFree AudioGateway observer instance.
*/
HWTEST_F(HandsFreeAudioGatewayTest, HandsFreeAudioGateway_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DeregisterObserver start";
 
    profile_ = HandsFreeAudioGateway::GetProfile();
    profile_->DeregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DeregisterObserver end";
}


}  // namespace Bluetooth
}  // namespace OHOS