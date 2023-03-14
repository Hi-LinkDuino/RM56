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
#include "bluetooth_opp.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_host.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
using namespace std;

class OppObserverTest : public OppObserver {
public:
    OppObserverTest() = default;
    virtual ~OppObserverTest() = default;
    virtual void OnReceiveIncomingFileChanged(const BluetoothOppTransferInformation &transferInformation) {}
    virtual void OnTransferStateChanged(const BluetoothOppTransferInformation &transferInformation) {}

private:
};


static Bluetooth::Opp  *profile_;
static OppObserverTest g_oppObserverTest;

class OppTest : public testing::Test {
public:
    OppTest();
    ~OppTest();

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    bool CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
        std::vector<std::string> bluetoothRemoteDeviceAddr);
    bool CompareTransferInformation(const BluetoothOppTransferInformation &information,
        const BluetoothOppTransferInformation &transferInformation);

private:
};

OppTest::OppTest()
{
}

OppTest::~OppTest()
{
}

void OppTest::SetUpTestCase(void)
{
}
void OppTest::TearDownTestCase(void)
{
}
void OppTest::SetUp()
{
}
void OppTest::TearDown()
{
}

bool OppTest::CompareDevice(std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState,
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

bool OppTest::CompareTransferInformation(const BluetoothOppTransferInformation &information,
    const BluetoothOppTransferInformation &transferInformation)
{
    if (&information == &transferInformation) {
        return true;
    }
    if ((information.GetFileName() != transferInformation.GetFileName()) ||
        (information.GetFilePath() != transferInformation.GetFilePath()) ||
        (information.GetMimeType() != transferInformation.GetMimeType()) ||
        (information.GetDeviceName() != transferInformation.GetDeviceName()) ||
        (information.GetDeviceAddress() != transferInformation.GetDeviceAddress()) ||
        (information.GetTimeStamp() != transferInformation.GetTimeStamp()) ||
        (information.GetCurrentBytes() != transferInformation.GetCurrentBytes()) ||
        (information.GetTotalBytes() != transferInformation.GetTotalBytes()) ||
        (information.GetFailedReason() != transferInformation.GetFailedReason()) ||
        (information.GetDirection() != transferInformation.GetDirection()) ||
        (information.GetStatus() != transferInformation.GetStatus()) ||
        (information.GetId() != transferInformation.GetId())) {
        return false;
    }
    return true;
}

/*
 * @tc.number: OPP_ModuleTest_GetDevicesByStates_00100
 * @tc.name: GetDevicesByStates
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_GetDevicesByStates_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetDevicesByStates_00100 start";

    GTEST_LOG_(INFO) << "GetDevicesByStates function test";

    profile_ = Opp::GetProfile();
    std::vector<int> states;
    std::vector<std::string> bluetoothRemoteDeviceAddr;
    std::vector<BluetoothRemoteDevice> bluetoothRemoteDeviceByState = profile_->GetDevicesByStates(states);
    EXPECT_TRUE(CompareDevice(bluetoothRemoteDeviceByState, bluetoothRemoteDeviceAddr));

    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetDevicesByStates_00100 end";
}


/*
 * @tc.number: OPP_ModuleTest_GetDeviceState_00100
 * @tc.name: GetDeviceState
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_GetDeviceState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetDeviceState_00100 start";

    GTEST_LOG_(INFO) << "GetDeviceState function test";

    int sucess = static_cast<int>(BTConnectState::DISCONNECTED);
    BluetoothRemoteDevice device;
    profile_ = Opp::GetProfile();
    EXPECT_EQ(sucess, profile_->GetDeviceState(device));

    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetDeviceState_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_SendFile_00100
 * @tc.name: SendFile
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_SendFile_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_SendFile_00100 start";

    GTEST_LOG_(INFO) << "SendFile function test";

    std::string device;
    std::vector<std::string>filePaths;
    std::vector<std::string>mimeTypes;
    profile_ = Opp::GetProfile();
    EXPECT_FALSE(profile_->SendFile(device, filePaths, mimeTypes));

    GTEST_LOG_(INFO) << "OPP_ModuleTest_SendFile_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_SetIncomingFileConfirmation_00100
 * @tc.name: SetIncomingFileConfirmation
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_SetIncomingFileConfirmation_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_SetIncomingFileConfirmation_00100 start";

    GTEST_LOG_(INFO) << "SetIncomingFileConfirmation function test";

    bool accept = true;
    profile_ = Opp::GetProfile();
    EXPECT_FALSE(profile_->SetIncomingFileConfirmation(accept));

    GTEST_LOG_(INFO) << "OPP_ModuleTest_SetIncomingFileConfirmation_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_GetCurrentTransferInformation_00100
 * @tc.name: GetCurrentTransferInformation
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_GetCurrentTransferInformation_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetCurrentTransferInformation_00100 start";

    GTEST_LOG_(INFO) << "GetCurrentTransferInformation function test";

    profile_ = Opp::GetProfile();
    BluetoothOppTransferInformation Infermation;
    BluetoothOppTransferInformation transferInfermation = profile_->GetCurrentTransferInformation();
    EXPECT_TRUE(CompareTransferInformation(Infermation, transferInfermation));

    GTEST_LOG_(INFO) << "OPP_ModuleTest_GetCurrentTransferInformation_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_CancelTransfer_00100
 * @tc.name: CancelTransfer
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_CancelTransfer_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_CancelTransfer_00100 start";

    GTEST_LOG_(INFO) << "CancelTransfer function test";

    profile_ = Opp::GetProfile();
    EXPECT_FALSE(profile_->CancelTransfer());

    GTEST_LOG_(INFO) << "OPP_ModuleTest_CancelTransfer_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_RegisterObserver_00100
 * @tc.name: RegisterObserver
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_RegisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_RegisterObserver_00100 start";

    GTEST_LOG_(INFO) << "RegisterObserver function test";

    profile_ = Opp::GetProfile();
    profile_->RegisterObserver(&g_oppObserverTest);

    GTEST_LOG_(INFO) << "OPP_ModuleTest_RegisterObserver_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_DeregisterObserver_00100
 * @tc.name: DeregisterObserver
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_DeregisterObserver_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_DeregisterObserver_00100 start";

    GTEST_LOG_(INFO) << "DeregisterObserver function test";

    profile_ = Opp::GetProfile();
    profile_->DeregisterObserver(&g_oppObserverTest);

    GTEST_LOG_(INFO) << "OPP_ModuleTest_DeregisterObserver_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_OnReceiveIncomingFileChanged_00100
 * @tc.name: OnReceiveIncomingFileChanged
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_OnReceiveIncomingFileChanged_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_OnReceiveIncomingFileChanged_00100 start";

    GTEST_LOG_(INFO) << "OnReceiveIncomingFileChanged function test";

    BluetoothOppTransferInformation trasnferInformation;
    g_oppObserverTest.OnReceiveIncomingFileChanged(trasnferInformation);

    GTEST_LOG_(INFO) << "OPP_ModuleTest_OnReceiveIncomingFileChanged_00100 end";
}

/*
 * @tc.number: OPP_ModuleTest_OnTransferStateChanged_00100
 * @tc.name: OnTransferStateChanged
 * @tc.desc:
 */
HWTEST_F(OppTest, OPP_ModuleTest_OnTransferStateChanged_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OPP_ModuleTest_OnTransferStateChanged_00100 start";

    GTEST_LOG_(INFO) << "OnTransferStateChanged function test";

    BluetoothOppTransferInformation trasnferInformation;
    g_oppObserverTest.OnTransferStateChanged(trasnferInformation);

    GTEST_LOG_(INFO) << "OPP_ModuleTest_OnTransferStateChanged_00100 end";
}
}  // namespace Bluetooth
}  // namespace OHOS
