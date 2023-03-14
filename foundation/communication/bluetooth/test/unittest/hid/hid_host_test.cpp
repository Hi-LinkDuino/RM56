/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "bluetooth_hid_host.h"
#include "bluetooth_remote_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

class HidHostObserverTest : public HidHostObserver {
public:
    HidHostObserverTest() = default;
    virtual ~HidHostObserverTest() = default;
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}

private:
};


static Bluetooth::HidHost  *profile_;
static HidHostObserverTest hidHostObserverTest;

class HidHostTest : public testing::Test {
public:
    HidHostTest();
    ~HidHostTest();

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    bool CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
                       std::vector<std::string> bluetoothRemoteDeviceAddr);

private:
};

HidHostTest::HidHostTest()
{
}

HidHostTest::~HidHostTest()
{
}

void HidHostTest::SetUpTestCase(void)
{
}
void HidHostTest::TearDownTestCase(void)
{
}
void HidHostTest::SetUp()
{
}
void HidHostTest::TearDown()
{
}

bool HidHostTest::CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
                                std::vector<std::string> bluetoothRemoteDeviceAddr)
{
    if (bluetoothRemoteDeviceByState.size() != bluetoothRemoteDeviceAddr.size()) {
        return false;
    }
    for (int i = 0; i < bluetoothRemoteDeviceByState.size(); i++) {
        if (bluetoothRemoteDeviceByState[i].GetDeviceAddr().compare(bluetoothRemoteDeviceAddr[i]) != 0) {
            return false;
        }
    }
    return true;
}

/*
 * @tc.number: HID_ModuleTest_GetDevicesByStates_00100
 * @tc.name: GetDevicesByStates
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_GetDevicesByStates_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_GetDevicesByStates_00100 start";

    GTEST_LOG_(INFO) << "GetDevicesByStates function test";

    profile_ = HidHost::GetProfile();
    std::vector<int> states;
    std::vector<std::string> bluetoothRemoteDeviceAddr;
    std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState = profile_->GetDevicesByStates(states);
    EXPECT_TRUE(CompareDevice(bluetoothRemoteDeviceByState, bluetoothRemoteDeviceAddr));

    GTEST_LOG_(INFO) << "HID_ModuleTest_GetDevicesByStates_00100 end";
}


/*
 * @tc.number: HID_ModuleTest_GetDeviceState_00100
 * @tc.name: GetDeviceState
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_GetDeviceState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_GetDeviceState_00100 start";

    GTEST_LOG_(INFO) << "GetDeviceState function test";

    int sucess = static_cast<int>(BTConnectState::DISCONNECTED);
    BluetoothRemoteDevice device;
    profile_ = HidHost::GetProfile();
    EXPECT_EQ(sucess, profile_->GetDeviceState(device));

    GTEST_LOG_(INFO) << "HID_ModuleTest_GetDeviceState_00100 end";
}

/*
 * @tc.number: HID_ModuleTest_Connect_00100
 * @tc.name: Connect
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_Connect_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_Connect_00100 start";

    GTEST_LOG_(INFO) << "Connect function test";

    BluetoothRemoteDevice device;
    profile_ = HidHost::GetProfile();
    EXPECT_FALSE(profile_->Connect(device));

    GTEST_LOG_(INFO) << "HID_ModuleTest_Connect_00100 end";
}

/*
 * @tc.number: HID_ModuleTest_Disconnect_00100
 * @tc.name: Disconnect
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_Disconnect_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_Disconnect_00100 start";

    GTEST_LOG_(INFO) << "Disconnect function test";

    BluetoothRemoteDevice device;
    profile_ = HidHost::GetProfile();
    EXPECT_FALSE(profile_->Disconnect(device));

    GTEST_LOG_(INFO) << "HID_ModuleTest_Disconnect_00100 end";
}

/*
 * @tc.number: HID_ModuleTest_RegisterObserver_00100
 * @tc.name: RegisterObserver
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_RegisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_RegisterObserver_00100 start";

    GTEST_LOG_(INFO) << "RegisterObserver function test";

    profile_ = HidHost::GetProfile();
    profile_->RegisterObserver(&hidHostObserverTest);

    GTEST_LOG_(INFO) << "HID_ModuleTest_RegisterObserver_00100 end";
}

/*
 * @tc.number: HID_ModuleTest_DeregisterObserver_00100
 * @tc.name: DeregisterObserver
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_DeregisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_DeregisterObserver_00100 start";

    GTEST_LOG_(INFO) << "DeregisterObserver function test";

    profile_ = HidHost::GetProfile();
    profile_->DeregisterObserver(&hidHostObserverTest);

    GTEST_LOG_(INFO) << "HID_ModuleTest_DeregisterObserver_00100 end";
}

/*
 * @tc.number: HID_ModuleTest_OnConnectionStateChanged_00100
 * @tc.name: OnConnectionStateChanged
 * @tc.desc:
 */
HWTEST_F(HidHostTest, HID_ModuleTest_OnConnectionStateChanged_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HID_ModuleTest_OnConnectionStateChanged_00100 start";

    GTEST_LOG_(INFO) << "OnConnectionStateChanged function test";

    BluetoothRemoteDevice device;
    int ret = 0;
    hidHostObserverTest.OnConnectionStateChanged(device, ret);

    GTEST_LOG_(INFO) << "HID_ModuleTest_OnConnectionStateChanged_00100 end";
}
}  // namespace Bluetooth
}  // namespace OHOS
