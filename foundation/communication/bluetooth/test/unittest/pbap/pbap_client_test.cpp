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
#include "bluetooth_pbap_client.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int TIME = 2;

class PbapClientObserverCommon : public PbapClientObserver {
public:
    PbapClientObserverCommon() = default;
    virtual ~PbapClientObserverCommon() = default;
    
    void OnServiceConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}
    void OnServicePasswordRequired(const BluetoothRemoteDevice &device,
        const std::vector<uint8_t> &description, uint8_t charset, bool fullAccess = true) {}
    void OnActionCompleted(
        const BluetoothRemoteDevice &device, int respCode, int actionType, const PbapPhoneBookData &result) {}

private:
};

static PbapClientObserverCommon observer_;
static PbapClient *profile_;


class PbapClientTest : public testing::Test {
public:
    PbapClientTest()
    {}
    ~PbapClientTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};


void PbapClientTest::SetUpTestCase(void) {}
void PbapClientTest::TearDownTestCase(void) {}

void PbapClientTest::SetUp()
{
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void PbapClientTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
    sleep(TIME);
}

/*
 * @tc.number: PbapClientUnit001
 * @tc.name: Connect
 * @tc.desc: Initiate the establishment of a service level connection to remote pbapclient device.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_Connect start";
 
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Connect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_Connect end";
}

/*
 * @tc.number: PbapClientUnit002
 * @tc.name: Disconnect
 * @tc.desc: Release the connection from pbapclient device.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_Disconnect start";
 
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool isOK = profile_->Disconnect(device);
    EXPECT_EQ(isOK, true);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_Disconnect end";
}

/*
 * @tc.number: PbapClientUnit003
 * @tc.name: RegisterObserver
 * @tc.desc: Register PbapClient observer instance.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_RegisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_RegisterObserver start";
 
    profile_ = PbapClient::GetProfile();
    profile_->RegisterObserver(&observer_);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_RegisterObserver end";
}

/*
 * @tc.number: PbapClientUnit004
 * @tc.name: DeregisterObserver
 * @tc.desc: Deregister PbapClient observer instance.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_DeregisterObserver, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_DeregisterObserver start";
 
    profile_ = PbapClient::GetProfile();
    profile_->DeregisterObserver(&observer_);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_DeregisterObserver end";
}

/*
 * @tc.number: PbapClientUnit005
 * @tc.name: GetConnectedDevices
 * @tc.desc: Get remote PbapClient device list which are in the connected state.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_GetConnectedDevices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetConnectedDevices start";
    profile_ = PbapClient::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile_->GetConnectedDevices();
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetConnectedDevices end";
}

/*
 * @tc.number: PbapClientUnit006
 * @tc.name: GetDevicesByStates
 * @tc.desc: Get remote PbapClient device list which are in the specified states.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_GetDevicesByStates, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetDevicesByStates start";
    profile_ = PbapClient::GetProfile();
    vector<int> states = {static_cast<int>(BTConnectState::CONNECTED)};
    vector<BluetoothRemoteDevice> devices = profile_->GetDevicesByStates(states);
    int vectorSize = devices.size();
    EXPECT_EQ(vectorSize, 0);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: PbapClientUnit007
 * @tc.name: GetDeviceState
 * @tc.desc: Get remote PbapClient device state which are in the specified states.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_GetDeviceState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetDeviceState start";
 
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetDeviceState(device);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetDevicesByStates end";
}

/*
 * @tc.number: PbapClientUnit008
 * @tc.name: SetConnectionStrategy
 * @tc.desc: set the strategy with the specified remote device
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_SetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetConnectionStrategy start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    int strategy = 0;
    bool isOk = profile_->SetConnectionStrategy(device, strategy);
    EXPECT_EQ(isOk, false);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetConnectionStrategy end";
}

/*
 * @tc.number: PbapClientUnit009
 * @tc.name: GetConnectionStrategy
 * @tc.desc: get the strategy with the specified remote device
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_GetConnectionStrategy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetConnectionStrategy start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->GetConnectionStrategy(device);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_GetConnectionStrategy end";
}

/*
 * @tc.number: PbapClientUnit010
 * @tc.name: SetDevicePassword
 * @tc.desc: Set device's password. please call after OnServicePasswordRequired event.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_SetDevicePassword, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetDevicePassword start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    string password = "123";
    string userId = "123";
    int state = profile_->SetDevicePassword(device, password, userId);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetDevicePassword end";
}

/*
 * @tc.number: PbapClientUnit011
 * @tc.name: PullPhoneBook
 * @tc.desc: Pull phone book from remote device after connected.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_PullPhoneBook, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullPhoneBook start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    PbapPullPhoneBookParam param;
    int state = profile_->PullPhoneBook(device, param);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullPhoneBook end";
}

/*
 * @tc.number: PbapClientUnit012
 * @tc.name: SetPhoneBook
 * @tc.desc: Set phone book from remote device after connected.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_SetPhoneBook, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetPhoneBook start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    std::u16string name = {'a', 'b', 'c', 0};
    int flag = 0;
    int state = profile_->SetPhoneBook(device, name, flag);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_SetPhoneBook end";
}

/*
 * @tc.number: PbapClientUnit013
 * @tc.name: PullvCardListing
 * @tc.desc: Pull vCard listing from remote device after connected.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_PullvCardListing, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullvCardListing start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    PbapPullvCardListingParam param;
    int state = profile_->PullvCardListing(device, param);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullvCardListing end";
}

/*
 * @tc.number: PbapClientUnit014
 * @tc.name: PullvCardEntry
 * @tc.desc: Pull vCard entry from remote device after connected.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_PullvCardEntry, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullvCardEntry start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    PbapPullvCardEntryParam param;
    int state = profile_->PullvCardEntry(device, param);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_PullvCardEntry end";
}

/*
 * @tc.number: PbapClientUnit015
 * @tc.name: IsDownloading
 * @tc.desc: Check if local device is downloading phonebook from remote device.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_IsDownloading, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_IsDownloading start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    bool isOk = profile_->IsDownloading(device);
    EXPECT_EQ(isOk, false);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_IsDownloading end";
}

/*
 * @tc.number: PbapClientUnit015
 * @tc.name: AbortDownloading
 * @tc.desc: Abort downloading phonebook from remote device.
*/
HWTEST_F(PbapClientTest, PbapClient_UnitTest_AbortDownloading, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_AbortDownloading start";
    profile_ = PbapClient::GetProfile();
    BluetoothRemoteDevice device;
    int state = profile_->AbortDownloading(device);
    EXPECT_EQ(state, -1);
    GTEST_LOG_(INFO) << "PbapClient_UnitTest_AbortDownloading end";
}
}  // namespace Bluetooth
}  // namespace OHOS