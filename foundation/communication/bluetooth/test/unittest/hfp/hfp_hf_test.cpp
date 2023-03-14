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
#include "bluetooth_hfp_hf.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

constexpr int TIME = 2;

class HandsFreeUnitObserverCommon : public HandsFreeUnitObserver {
public:
    HandsFreeUnitObserverCommon() = default;
    virtual ~HandsFreeUnitObserverCommon() = default;
    
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}
    virtual void OnScoStateChanged(const BluetoothRemoteDevice &device, int state) {}
    virtual void OnCallChanged(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call) {}
    virtual void OnSignalStrengthChanged(const BluetoothRemoteDevice &device, int signal) {}
    virtual void OnRegistrationStatusChanged(const BluetoothRemoteDevice &device, int status) {}
    virtual void OnRoamingStatusChanged(const BluetoothRemoteDevice &device, int status) {}
    virtual void OnOperatorSelectionChanged(const BluetoothRemoteDevice &device, const std::string &name) {}
    virtual void OnSubscriberNumberChanged(const BluetoothRemoteDevice &device, const std::string &number) {}
    virtual void OnVoiceRecognitionStatusChanged(const BluetoothRemoteDevice &device, int status) {}
    virtual void OnInBandRingToneChanged(const BluetoothRemoteDevice &device, int status) {}

private:
};
static HandsFreeUnitObserverCommon observer_;
static HandsFreeUnit *profile_;
static BluetoothHost *host_;
class HandsFreeUnitTest : public testing::Test {
public:
    HandsFreeUnitTest()
    {}
    ~HandsFreeUnitTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void HandsFreeUnitTest::SetUpTestCase(void)
{
    
}
void HandsFreeUnitTest::TearDownTestCase(void)
{
    
}
void HandsFreeUnitTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void HandsFreeUnitTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}


/*
 * @tc.number: HandsFreeUnit001
 * @tc.name: GetProfile
 * @tc.desc: Get the instance of HandsFreeAudioGateway object.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetProfile start";
 
    profile_ = HandsFreeUnit::GetProfile();
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetProfile end";
}

/*
 * @tc.number: HandsFreeUnit002
 * @tc.name: ConnectSco
 * @tc.desc:  Initiate the establishment of an audio connection to remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_ConnectSco, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ConnectSco start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->ConnectSco(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_ConnectSco end";
}

/*
 * @tc.number: HandsFreeUnit003
 * @tc.name: DisconnectSco
 * @tc.desc: Release the audio connection from remote HF device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_DisconnectSco, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DisconnectSco start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->DisconnectSco(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DisconnectSco end";
}

/*
 * @tc.number: HandsFreeUnit004
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get remote AG device list which are in the specified states.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDevicesByStates start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);

    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: HandsFreeUnit005
 * @tc.name: GetDeviceState
 * @tc.desc: Get the connection state of the specified remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDeviceState start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetDeviceState(device), 0);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: HandsFreeUnit006
 * @tc.name: GetScoState
 * @tc.desc: Get the Audio connection state of the specified remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_GetScoState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetScoState start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->GetScoState(device), 0);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetScoState end";
}

/*
 * @tc.number: HandsFreeUnit007
 * @tc.name: SendDTMFTone
 * @tc.desc: Send DTMF tone code to remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_SendDTMFTone, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_SendDTMFTone start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->SendDTMFTone(device, 0), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_SendDTMFTone end";
}


/*
 * @tc.number: HandsFreeUnit008
 * @tc.name: Connect
 * @tc.desc: Initiate the establishment of a service level connection to remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Connect start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->Connect(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Connect end";
}

/*
 * @tc.number: HandsFreeUnit009
 * @tc.name: Disconnect
 * @tc.desc: Release the audio connection from remote AG device.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Disconnect start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->Disconnect(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_Disconnect end";
}


/*
 * @tc.number: HandsFreeUnit010
 * @tc.name: OpenVoiceRecognition
 * @tc.desc: Open voice recognition.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_OpenVoiceRecognition, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_OpenVoiceRecognition start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->OpenVoiceRecognition(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_OpenVoiceRecognition end";
}

/*
 * @tc.number: HandsFreeUnit011
 * @tc.name: CloseVoiceRecognition
 * @tc.desc: Close voice recognition.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_CloseVoiceRecognition, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_CloseVoiceRecognition start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->CloseVoiceRecognition(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_CloseVoiceRecognition end";
}

/*
 * @tc.number: HandsFreeUnit012
 * @tc.name: GetExistingCalls
 * @tc.desc: Get a list of all existing calls.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_GetExistingCalls, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetExistingCalls start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    vector<HandsFreeUnitCall> calls = profile_->GetExistingCalls(device);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_GetExistingCalls end";
}

/*
 * @tc.number: HandsFreeUnit013
 * @tc.name: AcceptIncomingCall
 * @tc.desc: Accept an incoming call.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_AcceptIncomingCall, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_AcceptIncomingCall start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->AcceptIncomingCall(device, 0), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_AcceptIncomingCall end";
}

/*
 * @tc.number: HandsFreeUnit014
 * @tc.name: HoldActiveCall
 * @tc.desc: Hold an active call.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_HoldActiveCall, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_HoldActiveCall start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->HoldActiveCall(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_HoldActiveCall end";
}

/*
 * @tc.number: HandsFreeUnit015
 * @tc.name: RejectIncomingCall
 * @tc.desc: Reject an incoming call.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_RejectIncomingCall, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RejectIncomingCall start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    EXPECT_EQ(profile_->RejectIncomingCall(device), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RejectIncomingCall end";
}

/*
 * @tc.number: HandsFreeUnit016
 * @tc.name: FinishActiveCall
 * @tc.desc: Start dial with specified call number.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_FinishActiveCall, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_FinishActiveCall start";
 
    profile_ = HandsFreeUnit::GetProfile();
    BluetoothRemoteDevice device;
    HandsFreeUnitCall call;
    EXPECT_EQ(profile_->FinishActiveCall(device, call), true);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_FinishActiveCall end";
}

/*
 * @tc.number: HandsFreeUnit017
 * @tc.name: StartDial
 * @tc.desc: Start dial with specified call number.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_StartDial, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_StartDial start";
 
    BluetoothRemoteDevice device;
    std::string number = "000";
    profile_ = HandsFreeUnit::GetProfile();
    std::optional<HandsFreeUnitCall> call = profile_->StartDial(device, number);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_StartDial end";
}

/*
 * @tc.number: HandsFreeUnit018
 * @tc.name: RegisterObserver
 * @tc.desc: Register HandsFree Unit observer instance.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RegisterObserver start";
 
    profile_ = HandsFreeUnit::GetProfile();
    profile_->RegisterObserver(&observer_);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: HandsFreeUnit019
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister HandsFree Unit observer instance.
*/
HWTEST_F(HandsFreeUnitTest, HandsFreeUnit_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DeregisterObserver start";
 
    profile_ = HandsFreeUnit::GetProfile();
    profile_->DeregisterObserver(&observer_);
    
    GTEST_LOG_(INFO) << "HandsFreeAudioGateway_UnitTest_DeregisterObserver end";
}

}  // namespace Bluetooth
}  // namespace OHOS