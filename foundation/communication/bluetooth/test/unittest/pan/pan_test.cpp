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
#include "bluetooth_pan.h"
#include "bluetooth_remote_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

class PanObserverTest : public PanObserver {
public:
    PanObserverTest() = default;
    virtual ~PanObserverTest() = default;
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {}

private:
};


static Bluetooth::Pan  *profile_;
static PanObserverTest panObserverTest;

class PanTest : public testing::Test {
public:
    PanTest();
    ~PanTest();

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    bool CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
                       std::vector<std::string> bluetoothRemoteDeviceAddr);

private:
};

PanTest::PanTest()
{
}

PanTest::~PanTest()
{
}

void PanTest::SetUpTestCase(void)
{
}
void PanTest::TearDownTestCase(void)
{
}
void PanTest::SetUp()
{
}
void PanTest::TearDown()
{
}
bool PanTest::CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
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
 * @tc.number: PAN_ModuleTest_GetDevicesByStates_00100
 * @tc.name: GetDevicesByStates
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_GetDevicesByStates_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_GetDevicesByStates_00100 start";

    GTEST_LOG_(INFO) << "GetDevicesByStates function test";

    profile_ = Pan::GetProfile();
    std::vector<int> states;
    std::vector<std::string> bluetoothRemoteDeviceAddr;
    std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState = profile_->GetDevicesByStates(states);
    EXPECT_TRUE(CompareDevice(bluetoothRemoteDeviceByState, bluetoothRemoteDeviceAddr));

    GTEST_LOG_(INFO) << "PAN_ModuleTest_GetDevicesByStates_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_GetDeviceState_00100
 * @tc.name: GetDeviceState
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_GetDeviceState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_GetDeviceState_00100 start";

    GTEST_LOG_(INFO) << "GetDeviceState function test";

    int sucess = static_cast<int>(BTConnectState::DISCONNECTED);
    BluetoothRemoteDevice device;
    profile_ = Pan::GetProfile();
    EXPECT_EQ(sucess, profile_->GetDeviceState(device));

    GTEST_LOG_(INFO) << "PAN_ModuleTest_GetDeviceState_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_Disconnect_00100
 * @tc.name: Disconnect
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_Disconnect_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_Disconnect_00100 start";

    GTEST_LOG_(INFO) << "Disconnect function test";

    BluetoothRemoteDevice device;
    profile_ = Pan::GetProfile();
    EXPECT_FALSE(profile_->Disconnect(device));

    GTEST_LOG_(INFO) << "PAN_ModuleTest_Disconnect_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_RegisterObserver_00100
 * @tc.name: RegisterObserver
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_RegisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_RegisterObserver_00100 start";

    GTEST_LOG_(INFO) << "RegisterObserver function test";

    profile_ = Pan::GetProfile();
    profile_->RegisterObserver(&panObserverTest);

    GTEST_LOG_(INFO) << "PAN_ModuleTest_RegisterObserver_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_DeregisterObserver_00100
 * @tc.name: DeregisterObserver
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_DeregisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_DeregisterObserver_00100 start";

    GTEST_LOG_(INFO) << "DeregisterObserver function test";

    profile_ = Pan::GetProfile();
    profile_->DeregisterObserver(&panObserverTest);

    GTEST_LOG_(INFO) << "PAN_ModuleTest_DeregisterObserver_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_OnConnectionStateChanged_00100
 * @tc.name: OnConnectionStateChanged
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_OnConnectionStateChanged_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_OnConnectionStateChanged_00100 start";

    GTEST_LOG_(INFO) << "OnConnectionStateChanged function test";

    BluetoothRemoteDevice device;
    int state = static_cast<int>(BTConnectState::DISCONNECTED);;
    panObserverTest.OnConnectionStateChanged(device, state);

    GTEST_LOG_(INFO) << "PAN_ModuleTest_OnConnectionStateChanged_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_SetTethering_00100
 * @tc.name: SetTethering
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_SetTethering_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_SetTethering_00100 start";

    GTEST_LOG_(INFO) << "SetTethering function test";

    bool value = false;
    profile_ = Pan::GetProfile();
    EXPECT_FALSE(profile_->SetTethering(value));

    GTEST_LOG_(INFO) << "PAN_ModuleTest_SetTethering_00100 end";
}

/*
 * @tc.number: PAN_ModuleTest_IsTetheringOn_00100
 * @tc.name: IsTetheringOn
 * @tc.desc:
 */
HWTEST_F(PanTest, PAN_ModuleTest_IsTetheringOn_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "PAN_ModuleTest_IsTetheringOn_00100 start";

    GTEST_LOG_(INFO) << "IsTetheringOn function test";

    profile_ = Pan::GetProfile();
    EXPECT_FALSE(profile_->IsTetheringOn());

    GTEST_LOG_(INFO) << "PAN_ModuleTest_IsTetheringOn_00100 end";
}
}  // namespace Bluetooth
}  // namespace OHOS