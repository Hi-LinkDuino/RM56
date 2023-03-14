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
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
class AvrcpControllerObserverCommon : public AvrcpController::IObserver {
public:
    AvrcpControllerObserverCommon() = default;
    virtual ~AvrcpControllerObserverCommon() = default;
    
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state){}
    void OnActionCompleted(const BluetoothRemoteDevice &device, const AvrcpCtResponse &resp){}
private:
};

const uint8_t ATTRIBUTES_TEST = 0x04;
const uint32_t ITEMS_ATTRIBUTES_TEST = 1U;
static AvrcpControllerObserverCommon observer_;
static AvrcpController *profile_;

class AvrcpControllerTest : public testing::Test {
public:
    AvrcpControllerTest()
    {}
    ~AvrcpControllerTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    BluetoothHost *host_;
};

void AvrcpControllerTest::SetUpTestCase(void)
{}
void AvrcpControllerTest::TearDownTestCase(void)
{}
void AvrcpControllerTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    
}
void AvrcpControllerTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}

/*
 * @tc.number: AvrcpController001
 * @tc.name: GetProfile
 * @tc.desc: Gets the static instance of the <b>AvrcpController</b> class.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetProfile start";
    profile_ = AvrcpController::GetProfile();
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetProfile end";
}

/*
 * @tc.number: AvrcpController002
 * @tc.name: GetConnectedDevices
 * @tc.desc: Gets the connected devices.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetConnectedDevices start";
 
    profile_ = AvrcpController::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: AvrcpController003
 * @tc.name: GetDeviceState
 * @tc.desc: Gets the connection state of the specified bluetooth device.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetDeviceState start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetDeviceState(device);
    EXPECT_EQ(state, 0);
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetDeviceState end";
}

/*
 * @tc.number: AvrcpController004
 * @tc.name: RegisterObserver
 * @tc.desc: Registers the observer.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_RegisterObserver start";
 
    profile_ = AvrcpController::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: AvrcpController005
 * @tc.name: UnregisterObserver
 * @tc.desc: Unregisters the observer.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_UnregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_UnregisterObserver start";
 
    profile_ = AvrcpController::GetProfile();
    profile_->UnregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_UnregisterObserver end";
}

/*
 * @tc.number: AvrcpController006
 * @tc.name: GetDevicesByStates
 * @tc.desc: Gets the devices of the specified states.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetDevicesByStates start";
 
    profile_ = AvrcpController::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: AvrcpController007
 * @tc.name: Connect
 * @tc.desc: Connects to the AVRCP TG service.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_Connect start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Connect(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_Connect end";
}

/*
 * @tc.number: AvrcpController008
 * @tc.name: Disconnect
 * @tc.desc: Disconnects from the AVRCP TG service.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_Disconnect start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_Disconnect end";
}

/*
 * @tc.number: AvrcpController009
 * @tc.name: PressButton
 * @tc.desc: Presses the button.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_PressButton, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_PressButton start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    uint8_t button = AVRC_KEY_OPERATION_VOLUME_UP;
    int ret = profile_->PressButton(device, button);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_PressButton end";
}

/*
 * @tc.number: AvrcpController010
 * @tc.name: ReleaseButton
 * @tc.desc: Releases the button.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_ReleaseButton, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_ReleaseButton start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    uint8_t button = AVRC_KEY_OPERATION_VOLUME_UP;
    int ret = profile_->ReleaseButton(device, button);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_ReleaseButton end";
}

/*
 * @tc.number: AvrcpController011
 * @tc.name: GetUnitInfo
 * @tc.desc: Gets the unit information.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetUnitInfo, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetUnitInfo start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetUnitInfo(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetUnitInfo end";
}

/*
 * @tc.number: AvrcpController012
 * @tc.name: GetSubUnitInfo
 * @tc.desc: Gets the sub unit information.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetSubUnitInfo, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSubUnitInfo start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetSubUnitInfo(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSubUnitInfo end";
}

/*
 * @tc.number: AvrcpController013
 * @tc.name: SetAddressedPlayer
 * @tc.desc: Informs which media player wishes to control.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_SetAddressedPlayer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetAddressedPlayer start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->SetAddressedPlayer(device, 0);

    EXPECT_EQ(ret, RET_BAD_STATUS);
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetAddressedPlayer end";
}

/*
 * @tc.number: AvrcpController014
 * @tc.name: SetBrowsedPlayer
 * @tc.desc: Informs to which player browsing commands should be routed.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_SetBrowsedPlayer, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetBrowsedPlayer start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->SetBrowsedPlayer(device, 0);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetBrowsedPlayer end";
}

/*
 * @tc.number: AvrcpController015
 * @tc.name: GetSupportedCompanies
 * @tc.desc: Get the supported companies by remote device.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetSupportedCompanies, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSupportedCompanies start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetSupportedCompanies(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSupportedCompanies end";
}

/*
 * @tc.number: AvrcpController016
 * @tc.name: GetSupportedEvents
 * @tc.desc: Get the supported events by remote device.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetSupportedEvents, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSupportedEvents start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetSupportedEvents(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetSupportedEvents end";
}

/*
 * @tc.number: AvrcpController017
 * @tc.name: GetPlayerAppSettingAttributes
 * @tc.desc: Gets the attribute of the player application.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayerAppSettingAttributes, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingAttributes start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetPlayerAppSettingAttributes(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingAttributes end";
}

/*
 * @tc.number: AvrcpController018
 * @tc.name: GetPlayerAppSettingValues
 * @tc.desc: Gets the values of the specified attribute of the player application.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayerAppSettingValues, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingValues start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetPlayerAppSettingValues(device, ATTRIBUTES_TEST);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingValues end";
}

/*
 * @tc.number: AvrcpController019
 * @tc.name: GetPlayerAppSettingCurrentValue
 * @tc.desc: Gets the current set values on the target for the provided player application setting attributes list.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayerAppSettingCurrentValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingCurrentValue start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    vector<uint8_t> attributes = {ATTRIBUTES_TEST};
    int ret = profile_->GetPlayerAppSettingCurrentValue(device, attributes);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerAppSettingCurrentValue end";
}

/*
 * @tc.number: AvrcpController020
 * @tc.name: SetPlayerAppSettingCurrentValue
 * @tc.desc: Sets the player application setting list of player application setting values on the target device for the
 * corresponding defined list of AvrcPlayerAttribute.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_SetPlayerAppSettingCurrentValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetPlayerAppSettingCurrentValue start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    vector<uint8_t> attributes = {ATTRIBUTES_TEST};
    vector<uint8_t> values = {0};
    int ret = profile_->SetPlayerAppSettingCurrentValue(device, attributes, values);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetPlayerAppSettingCurrentValue end";
}

/*
 * @tc.number: AvrcpController021
 * @tc.name: GetPlayerApplicationSettingAttributeText
 * @tc.desc: provide supported player application setting attribute displayable text.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayerApplicationSettingAttributeText, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerApplicationSettingAttributeText start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    vector<uint8_t> attributes = {ATTRIBUTES_TEST};

    int ret = profile_->GetPlayerApplicationSettingAttributeText(device, attributes);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerApplicationSettingAttributeText end";
}

/*
 * @tc.number: AvrcpController022
 * @tc.name: GetPlayerApplicationSettingValueText
 * @tc.desc: request the target device to provide target supported player application setting value displayable text.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayerApplicationSettingValueText, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerApplicationSettingValueText start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;

    vector<uint8_t> values = {0};
    int ret = profile_->GetPlayerApplicationSettingValueText(device, ATTRIBUTES_TEST, values);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayerApplicationSettingValueText end";
}

/*
 * @tc.number: AvrcpController023
 * @tc.name: GetElementAttributes
 * @tc.desc: Requests the TG to provide the attributes of the element specified in the parameter.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetElementAttributes, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetElementAttributes start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    vector<uint32_t> attributes = {ATTRIBUTES_TEST};

    int ret = profile_->GetElementAttributes(device, attributes);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetElementAttributes end";
}

/*
 * @tc.number: AvrcpController024
 * @tc.name: GetPlayStatus
 * @tc.desc: Gets the play status.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetPlayStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayStatus start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetPlayStatus(device);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetPlayStatus end";
}

/*
 * @tc.number: AvrcpController025
 * @tc.name: PlayItem
 * @tc.desc: Starts playing an item indicated by the UID.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_PlayItem, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_PlayItem start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->PlayItem(device, 0, 0);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_PlayItem end";
}

/*
 * @tc.number: AvrcpController026
 * @tc.name: AddToNowPlaying
 * @tc.desc: Adds an item indicated by the UID to the Now Playing queue.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_AddToNowPlaying, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_AddToNowPlaying start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->AddToNowPlaying(device, 0, 0);
    EXPECT_EQ (ret, RET_BAD_STATUS);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_AddToNowPlaying end";
}

/*
 * @tc.number: AvrcpController027
 * @tc.name: RequestContinuingResponse
 * @tc.desc: Requests continuing response.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_RequestContinuingResponse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_RequestContinuingResponse start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->RequestContinuingResponse(device, 0);
    EXPECT_EQ (ret, RET_BAD_STATUS);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_RequestContinuingResponse end";
}

/*
 * @tc.number: AvrcpController028
 * @tc.name: AbortContinuingResponse
 * @tc.desc: Aborts continuing response.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_AbortContinuingResponse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_AbortContinuingResponse start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->AbortContinuingResponse(device, 0);
    EXPECT_EQ (ret, RET_BAD_STATUS);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_AbortContinuingResponse end";
}

/*
 * @tc.number: AvrcpController029
 * @tc.name: ChangePath
 * @tc.desc: Navigates one level up or down in the virtual file system.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_ChangePath, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_ChangePath start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->ChangePath(device, 0, 0, 0);
    EXPECT_EQ (ret, RET_BAD_STATUS);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_ChangePath end";
}

/*
 * @tc.number: AvrcpController030
 * @tc.name: GetFolderItems
 * @tc.desc: Retrieves a listing of the contents of a folder.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetFolderItems, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetFolderItems start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;

    vector<uint32_t> attributes = {ITEMS_ATTRIBUTES_TEST};
    int ret = profile_->GetFolderItems(device, 0, 1, attributes);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetFolderItems end";
}

/*
 * @tc.number: AvrcpController031
 * @tc.name: GetMeidaPlayerList
 * @tc.desc: Retrieves a listing of the contents of a folder.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetMeidaPlayerList, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetMeidaPlayerList start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetMeidaPlayerList(device, 0, 1);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetMeidaPlayerList end";
}

/*
 * @tc.number: AvrcpController032
 * @tc.name: GetItemAttributes
 * @tc.desc: Retrieves the metadata attributes for a particular media element item or folder item.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetItemAttributes, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetItemAttributes start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;

    vector<uint32_t> attributes = {ITEMS_ATTRIBUTES_TEST};
    int ret = profile_->GetItemAttributes(device, 0, 1, attributes);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetItemAttributes end";
}

/*
 * @tc.number: AvrcpController033
 * @tc.name: GetTotalNumberOfItems
 * @tc.desc: Gets the number of items in the now playing scope.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_GetTotalNumberOfItems, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetTotalNumberOfItems start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetTotalNumberOfItems(device);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_GetTotalNumberOfItems end";
}

/*
 * @tc.number: AvrcpController034
 * @tc.name: SetAbsoluteVolume
 * @tc.desc: Sets an absolute volume to be used by the rendering device.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_SetAbsoluteVolume, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetAbsoluteVolume start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->SetAbsoluteVolume(device, 0);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_SetAbsoluteVolume end";
}

/*
 * @tc.number: AvrcpController035
 * @tc.name: EnableNotification
 * @tc.desc: Enables for receiving notifications asynchronously based on specific events occurring.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_EnableNotification, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_EnableNotification start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;

    vector<uint8_t> events = {0};
    int ret = profile_->EnableNotification(device, events);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_EnableNotification end";
}

/*
 * @tc.number: AvrcpController036
 * @tc.name: DisableNotification
 * @tc.desc: Disables for receiving notifications asynchronously based on specific events occurring.
*/
HWTEST_F(AvrcpControllerTest, AvrcpController_UnitTest_DisableNotification, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_DisableNotification start";
 
    profile_ = AvrcpController::GetProfile();
    BluetoothRemoteDevice device;

    vector<uint8_t> events = {0};
    int ret = profile_->DisableNotification(device, events);
    EXPECT_EQ (ret, 0);

    GTEST_LOG_(INFO) << "AvrcpController_UnitTest_DisableNotification end";
}
}  // namespace Bluetooth
}  // namespace OHOS