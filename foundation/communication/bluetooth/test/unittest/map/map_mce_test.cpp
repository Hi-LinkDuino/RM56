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
#include "bluetooth_map_mce.h"
#include "bluetooth_host.h"
#include "bluetooth_def.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class MapClientObserverCommon : public MapClientObserver{
public:
    MapClientObserverCommon() = default;
    virtual ~MapClientObserverCommon() = default;
    
    void OnMapActionCompleted(
        const BluetoothRemoteDevice &device, const MapAction &action, MapExecuteStatus status) {}

    void OnMapEventReported(const BluetoothRemoteDevice &device, const MapEventReport &report) {}

    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}

    void OnBmessageCompleted(
        const BluetoothRemoteDevice &deviceAddress, const MapBMessage &bmsg, MapExecuteStatus status) {}

    void OnMessagesListingCompleted(
        const BluetoothRemoteDevice &deviceAddress, const MessagesListing &listing, MapExecuteStatus status) {}

    void OnConversationListingCompleted(
        const BluetoothRemoteDevice &deviceAddress, const ConversationListing &listing, MapExecuteStatus status) {}

private:
};

static MapClientObserverCommon observer_;
static MapClient *profile_;

class MapClientTest : public testing::Test {
public:
    MapClientTest()
    {}
    ~MapClientTest()
    {}

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};


void MapClientTest::SetUpTestCase(void)
{}
void MapClientTest::TearDownTestCase(void)
{}
void MapClientTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void MapClientTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
}



/*
 * @tc.number: MapClient001
 * @tc.name: GetProfile
 * @tc.desc: Get the Profile object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetProfile, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetProfile start";
 
    profile_ = MapClient::GetProfile();
    
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetProfile end";
}


/*
 * @tc.number: MapClient002
 * @tc.name: RegisterObserver
 * @tc.desc: Register Observer.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_RegisterObserver start";
 
    profile_ = MapClient::GetProfile();
    profile_->RegisterObserver(observer_);
    sleep(TIME);
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_RegisterObserver end";
}


/*
 * @tc.number: MapClient003
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister Observer.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_DeregisterObserver start";
 
    profile_ = MapClient::GetProfile();
    profile_->DeregisterObserver(observer_);
    sleep(TIME);
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_DeregisterObserver end";
}


/*
 * @tc.number: MapClient004
 * @tc.name: Connect
 * @tc.desc: Connect to map server.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Connect start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool ret = profile_->Connect(device);
    sleep(TIME);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Connect end";
}


/*
 * @tc.number: MapClient005
 * @tc.name: Disconnect
 * @tc.desc: disconnect from map server.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Disconnect start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool ret = profile_->Disconnect(device);
    sleep(TIME);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_Disconnect end";
}


/*
 * @tc.number: MapClient006
 * @tc.name: IsConnected
 * @tc.desc: display connect status.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_IsConnected, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_IsConnected start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool ret = profile_->IsConnected(device);
    sleep(TIME);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_IsConnected end";
}


/*
 * @tc.number: MapClient007
 * @tc.name: GetConnectedDevices
 * @tc.desc: Get the Connected Devices object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectedDevices start";
 
    profile_ = MapClient::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();
    sleep(TIME);


    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectedDevices end";
}


/*
 * @tc.number: MapClient008
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get the device list through the connection status.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDevicesByStates start";
 
    profile_ = MapClient::GetProfile();

    vector<int> statusList = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(statusList);
    sleep(TIME);


    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetDevicesByStates end";
}


/*
 * @tc.number: MapClient009
 * @tc.name: GetConnectionState
 * @tc.desc: Get the Connection State object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetConnectionState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectionState start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetConnectionState(device);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectionState end";
}


/*
 * @tc.number: MapClient010
 * @tc.name: SetConnectionStrategy
 * @tc.desc: Set the connection policy of the specified device.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_SetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetConnectionStrategy start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool ret = profile_->SetConnectionStrategy(device, 0);
    sleep(TIME);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetConnectionStrategy end";
}


/*
 * @tc.number: MapClient011
 * @tc.name: GetConnectionStrategy
 * @tc.desc: Get the Connection Strategy object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectionStrategy start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetConnectionStrategy(device);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConnectionStrategy end";
}


/*
 * @tc.number: MapClient012
 * @tc.name: GetUnreadMessages
 * @tc.desc: Get the Unread Messages object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetUnreadMessages, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetUnreadMessages start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    MapMessageType type = MapMessageType::SMS_GSM;
    bool ret = profile_->GetUnreadMessages(device, type, 3);
    sleep(TIME);
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetUnreadMessages end";
}


/*
 * @tc.number: MapClient013
 * @tc.name: GetSupportedFeatures
 * @tc.desc: Get the Supported Features object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetSupportedFeatures, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetSupportedFeatures start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    int ret = profile_->GetSupportedFeatures(device);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetSupportedFeatures end";
}


/*
 * @tc.number: MapClient014
 * @tc.name: SendMessage
 * @tc.desc: Send Message.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_SendMessage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SendMessage start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    MapSendMessageParameters message;
    int ret = profile_->SendMessage(device, message);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SendMessage end";
}


/*
 * @tc.number: MapClient015
 * @tc.name: SetNotificationFilter
 * @tc.desc: Set the Notification Filter object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_SetNotificationFilter, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetNotificationFilter start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    int mask = MAP_NOTIFICATION_FILTER_MASK_NEW_MESSAGE;
    int ret = profile_->SetNotificationFilter(device, mask);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetNotificationFilter end";
}


/*
 * @tc.number: MapClient016
 * @tc.name: GetMessagesListing
 * @tc.desc: Get the Messages Listing object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetMessagesListing, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMessagesListing start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;
    GetMessagesListingParameters para;
    int ret = profile_->GetMessagesListing(device, para);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMessagesListing end";
}


/*
 * @tc.number: MapClient017
 * @tc.name: GetMessage
 * @tc.desc: Get the Message object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetMessage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMessage start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    GetMessageParameters para;
    MapMessageType type = MapMessageType::SMS_GSM;
    std::u16string msgHandle = {'a', 'b', 'c'};
    int ret = profile_->GetMessage(device, type, msgHandle, para);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMessage end";
}


/*
 * @tc.number: MapClient018
 * @tc.name: UpdateInbox
 * @tc.desc: Update Inbox.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_UpdateInbox, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_UpdateInbox start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    MapMessageType type = MapMessageType::SMS_GSM;
    int ret = profile_->UpdateInbox(device, type);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_UpdateInbox end";
}


/*
 * @tc.number: MapClient019
 * @tc.name: GetConversationListing
 * @tc.desc: Get the Conversation Listing object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetConversationListing, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConversationListing start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    GetConversationListingParameters para;
    int ret = profile_->GetConversationListing(device, para);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetConversationListing end";
}


/*
 * @tc.number: MapClient020
 * @tc.name: SetMessageStatus
 * @tc.desc: Set the Message Status object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_SetMessageStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetMessageStatus start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    MapMessageType type = MapMessageType::SMS_GSM;
    MapSetMessageStatus msgStatus;
    int ret = profile_->SetMessageStatus(device, type, msgStatus);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetMessageStatus end";
}


/*
 * @tc.number: MapClient021
 * @tc.name: SetOwnerStatus
 * @tc.desc: Set the Owner Status object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_SetOwnerStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetOwnerStatus start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    SetOwnerStatusParameters para;
    int ret = profile_->SetOwnerStatus(device, para);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_SetOwnerStatus end";
}


/*
 * @tc.number: MapClient022
 * @tc.name: GetOwnerStatus
 * @tc.desc: Get the Owner Status object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetOwnerStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetOwnerStatus start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    std::string conversationId = "GetOwnerStatus test";
    int ret = profile_->GetOwnerStatus(device, conversationId);
    sleep(TIME);
    EXPECT_EQ(ret, 0);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetOwnerStatus end";
}


/*
 * @tc.number: MapClient023
 * @tc.name: GetMasInstanceInfo
 * @tc.desc: Get the Mas Instance Info object.
*/
HWTEST_F(MapClientTest, Avrcp_UnitTest_GetMasInstanceInfo, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMasInstanceInfo start";
 
    profile_ = MapClient::GetProfile();
    BluetoothRemoteDevice device;

    profile_->GetMasInstanceInfo(device);
    sleep(TIME);

    GTEST_LOG_(INFO) << "Avrcp_UnitTest_GetMasInstanceInfo end";
}


}  // namespace Bluetooth
}  // namespace OHOS