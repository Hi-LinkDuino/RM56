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

#include <gtest/gtest.h>
#include <mutex>
#include <thread>

#include "bluetooth_a2dp_src.h"
#include "bluetooth_def.h"
#include "bluetooth_device_class.h"
#include "bluetooth_host.h"

using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
class BluetoothHostObserverCommon : public BluetoothHostObserver {
public:
    BluetoothHostObserverCommon() = default;
    virtual ~BluetoothHostObserverCommon() = default;
    static BluetoothHostObserverCommon &GetInstance();
    void OnStateChanged(const int transport, const int status) override;
    void OnDiscoveryStateChanged(int status) override;
    void OnDiscoveryResult(const BluetoothRemoteDevice &device) override{};
    void OnPairRequested(const BluetoothRemoteDevice &device) override{};
    void OnPairConfirmed(const BluetoothRemoteDevice &device, int reqType, int number) override{};
    void OnDeviceNameChanged(const std::string &deviceName) override{};
    void OnScanModeChanged(int mode) override{};
    void OnDeviceAddrChanged(const std::string &address) override{};
};

BluetoothHostObserverCommon &BluetoothHostObserverCommon::GetInstance()
{
    static BluetoothHostObserverCommon instance;
    return instance;
}

void BluetoothHostObserverCommon::OnStateChanged(const int transport, const int status)
{
    if (transport == BT_TRANSPORT_BLE) {
        switch (status) {
            case STATE_TURNING_ON:
                GTEST_LOG_(INFO) << "BLE:Turning on ...";
                break;
            case STATE_TURN_ON:
                GTEST_LOG_(INFO) << "BLE:Turn on";
                break;
            case STATE_TURNING_OFF:
                GTEST_LOG_(INFO) << "BLE:Turning off ...";
                break;
            case STATE_TURN_OFF:
                GTEST_LOG_(INFO) << "BLE:Turn off";
                break;
            default:
                break;
        }
        return;
    } else {
        switch (status) {
            case STATE_TURNING_ON:
                GTEST_LOG_(INFO) << "BREDR:Turning on ...";
                break;
            case STATE_TURN_ON:
                GTEST_LOG_(INFO) << "BREDR:Turn on";
                break;
            case STATE_TURNING_OFF:
                GTEST_LOG_(INFO) << "BREDR:Turning off ...";
                break;
            case STATE_TURN_OFF:
                GTEST_LOG_(INFO) << "BREDR:Turn off";
                break;
            default:
                break;
        }
        return;
    }
}

void BluetoothHostObserverCommon::OnDiscoveryStateChanged(int status)
{
    switch (status) {
        case 0x01:
            GTEST_LOG_(INFO) << "discovery_start";
            break;
        case 0x02:
            GTEST_LOG_(INFO) << "discoverying";
            break;
        case 0x03:
            GTEST_LOG_(INFO) << "discovery_done";
            break;
        default:
            break;
    }
}

class HostTest : public testing::Test {
public:
    HostTest()
    {}
    ~HostTest()
    {}

    BluetoothHost *host_;
    BluetoothHostObserverCommon &btObserver_ = BluetoothHostObserverCommon::GetInstance();
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HostTest::SetUpTestCase(void)
{}

void HostTest::TearDownTestCase(void)
{}

void HostTest::SetUp()
{}

void HostTest::TearDown()
{}

HWTEST_F(HostTest, Host_ModuleTest_EnableBt_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_EnableBt_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->EnableBt());

    std::this_thread::sleep_for(std::chrono::seconds(3));

    GTEST_LOG_(INFO) << "Host_ModuleTest_EnableBt_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetRemoteDevice_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetRemoteDevice_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetRemoteDevice_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    BluetoothRemoteDevice remoteDevice = host_->GetRemoteDevice(device_.GetDeviceAddr(), BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetDeviceAddr(), remoteDevice.GetDeviceAddr());
    std::this_thread::sleep_for(std::chrono::seconds(3));

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetRemoteDevice_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtState_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBtState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtState_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(host_->GetBtState(), BTStateID::STATE_TURN_ON);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtState_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_BluetoothFactoryReset_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_BluetoothFactoryReset_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_BluetoothFactoryReset_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();

    GTEST_LOG_(INFO) << "Host_ModuleTest_BluetoothFactoryReset_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetProfileList_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetProfileList_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetProfileList_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    uint32_t profileSize = 10;
    EXPECT_EQ(host_->GetProfileList().size(), profileSize);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetProfileList_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetMaxNumConnectedAudioDevices_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetMaxNumConnectedAudioDevices_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetMaxNumConnectedAudioDevices_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(host_->GetMaxNumConnectedAudioDevices(), 6);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetMaxNumConnectedAudioDevices_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtProfileConnState_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBtProfileConnState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtProfileConnState_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(host_->GetBtProfileConnState(PROFILE_ID_GATT_CLIENT), (int)BTConnectState::DISCONNECTED);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtProfileConnState_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtConnectionState_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBtConnectionState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtConnectionState_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(host_->GetBtConnectionState(), (int)BTConnectState::DISCONNECTED);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtConnectionState_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_Start_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_Start_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_Start_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    host_->Start();

    GTEST_LOG_(INFO) << "Host_ModuleTest_Start_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_Stop_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_Stop_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_Stop_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    host_->Stop();

    GTEST_LOG_(INFO) << "Host_ModuleTest_Stop_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetLocalDeviceClass_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetLocalDeviceClass_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalDeviceClass_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    BluetoothDeviceClass remotCod(0x2A0104);
    EXPECT_TRUE(host_->SetLocalDeviceClass(remotCod));
    BluetoothDeviceClass getLocalcod = host_->GetLocalDeviceClass();
    EXPECT_EQ(remotCod.GetMajorClass(), getLocalcod.GetMajorClass());

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalDeviceClass_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetLocalDeviceClass_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetLocalDeviceClass_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetLocalDeviceClass_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    BluetoothDeviceClass remotCod(0x10100);
    EXPECT_TRUE(host_->SetLocalDeviceClass(remotCod));
    BluetoothDeviceClass getLocalcod = host_->GetLocalDeviceClass();
    EXPECT_EQ(remotCod.GetMajorClass(), getLocalcod.GetMajorClass());

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetLocalDeviceClass_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetLocalAddress_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetLocalAddress_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalAddress_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    std::string addr = host_->GetLocalAddress();
    EXPECT_TRUE(!addr.empty() && addr.size() > 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalAddress_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetLocalName_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetLocalName_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalName_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    std::string localName = "localName";
    host_->SetLocalName(localName);
    EXPECT_EQ(host_->GetLocalName(), localName);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetLocalName_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetLocalName_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetLocalName_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetLocalName_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    std::string localName = "localName";
    host_->SetLocalName(localName);
    EXPECT_EQ(host_->GetLocalName(), localName);

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetLocalName_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetBtScanMode_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetBtScanMode_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetBtScanMode_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->SetBtScanMode(SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, 0));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetBtScanMode_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtScanMode_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBtScanMode_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtScanMode_00100 start";

    sleep(1);
    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, host_->GetBtScanMode());

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtScanMode_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetBondableMode_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetBondableMode_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetBondableMode_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->SetBondableMode(BT_TRANSPORT_BREDR, BONDABLE_MODE_OFF));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetBondableMode_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtScanMode_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBondableMode_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBondableMode_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_EQ(host_->GetBondableMode(BTTransport::ADAPTER_BREDR), BONDABLE_MODE_OFF);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBondableMode_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_StartBtDiscovery_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_StartBtDiscovery_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_StartBtDiscovery_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->StartBtDiscovery());

    GTEST_LOG_(INFO) << "Host_ModuleTest_StartBtDiscovery_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsBtDiscovering_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsBtDiscovering_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsBtDiscovering_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->IsBtDiscovering(BTTransport::ADAPTER_BREDR));

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsBtDiscovering_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_CancelBtDiscovery_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_CancelBtDiscovery_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_CancelBtDiscovery_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->CancelBtDiscovery());

    GTEST_LOG_(INFO) << "Host_ModuleTest_CancelBtDiscovery_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetBtDiscoveryEndMillis_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetBtDiscoveryEndMillis_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtDiscoveryEndMillis_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    host_->GetBtDiscoveryEndMillis();

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetBtDiscoveryEndMillis_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetPairedDevices_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetPairedDevices_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPairedDevices_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    std::vector<BluetoothRemoteDevice> remotedeviceList = host_->GetPairedDevices(BT_TRANSPORT_BREDR);
    EXPECT_EQ((int)remotedeviceList.size(), 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPairedDevices_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_RemovePair_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_RemovePair_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_RemovePair_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);

    EXPECT_FALSE(host_->RemovePair(device_));

    GTEST_LOG_(INFO) << "Host_ModuleTest_RemovePair_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_RemoveAllPairs_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_RemoveAllPairs_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_RemoveAllPairs_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->RemoveAllPairs());

    GTEST_LOG_(INFO) << "Host_ModuleTest_RemoveAllPairs_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsValidBluetoothAddr_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsValidBluetoothAddr_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothAddr_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->IsValidBluetoothAddr("00:00:00:00:00:00"));

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothAddr_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsValidBluetoothAddr_00200
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsValidBluetoothAddr_00200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothAddr_00200 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_FALSE(host_->IsValidBluetoothAddr("00"));

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothAddr_00200 end";
}

/**
 * @tc.number: Host_ModuleTest_GetPhonebookPermission_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetPhonebookPermission_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPhonebookPermission_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_TRUE(device_.SetPhonebookPermission(1));
    EXPECT_EQ(device_.GetPhonebookPermission(), 1);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPhonebookPermission_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetPhonebookPermission_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetPhonebookPermission_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetPhonebookPermission_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_TRUE(device_.SetPhonebookPermission(0));
    EXPECT_EQ(device_.GetPhonebookPermission(), 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetPhonebookPermission_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetMessagePermission_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetMessagePermission_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPhonebookPermission_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_TRUE(device_.SetMessagePermission(1));
    EXPECT_EQ(device_.GetMessagePermission(), 1);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetMessagePermission_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetMessagePermission_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetMessagePermission_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetMessagePermission_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_TRUE(device_.SetMessagePermission(0));
    EXPECT_EQ(device_.GetMessagePermission(), 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetMessagePermission_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetMessagePermission_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetPowerMode_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPowerMode_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_GE(device_.GetPowerMode(), 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPowerMode_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceAddr_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceAddr_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceAddr_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetDeviceAddr(), "00:00:00:00:00:00");

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceAddr_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceName_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceName_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceName_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_TRUE(device_.GetDeviceName().empty());

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceName_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceAlias_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceAlias_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceAlias_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetDeviceAlias(), INVALID_NAME);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceAlias_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetDeviceAlias_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetDeviceAlias_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDeviceAlias_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.SetDeviceAlias("deviceName"));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDeviceAlias_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceType_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceType_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceType_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetDeviceType(), INVALID_VALUE);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceType_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetPowerMode_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceBatteryLevel_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceBatteryLevel_00100 start";

    int batteryLevel = 0;
    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetDeviceBatteryLevel(), batteryLevel);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceBatteryLevel_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetPairState_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetPairState_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPairState_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device_.GetPairState(), PAIR_NONE);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetPairState_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_StartPair_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_StartPair_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_StartPair_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.StartPair());

    GTEST_LOG_(INFO) << "Host_ModuleTest_StartPair_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_CancelPairing_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_CancelPairing_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_CancelPairing_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.CancelPairing());

    GTEST_LOG_(INFO) << "Host_ModuleTest_CancelPairing_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsBondedFromLocal_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsBondedFromLocal_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsBondedFromLocal_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.IsBondedFromLocal());

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsBondedFromLocal_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsAclConnected_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsAclConnected_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsAclConnected_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.IsAclConnected());

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsAclConnected_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsAclEncrypted_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsAclEncrypted_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsAclEncrypted_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.IsAclEncrypted());

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsAclEncrypted_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceClass_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceClass_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceClass_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    BluetoothDeviceClass getLocalcod = device_.GetDeviceClass();
    EXPECT_EQ(device_.GetDeviceClass().GetMajorClass(), getLocalcod.GetMajorClass());

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceClass_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetDeviceUuids_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetDeviceUuids_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceUuids_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    std::vector<ParcelUuid> uuids = device_.GetDeviceUuids();
    EXPECT_EQ((int)uuids.size(), 0);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetDeviceUuids_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetDevicePin_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetDevicePin_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePin_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.SetDevicePin("000000"));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePin_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetDevicePairingConfirmation_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetDevicePairingConfirmation_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePairingConfirmation_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.SetDevicePairingConfirmation(true));
    EXPECT_FALSE(device_.SetDevicePairingConfirmation(false));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePairingConfirmation_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_SetDevicePasskey_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_SetDevicePasskey_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePasskey_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.SetDevicePasskey(123456, true));
    EXPECT_FALSE(device_.SetDevicePasskey(123456, false));

    GTEST_LOG_(INFO) << "Host_ModuleTest_SetDevicePasskey_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_PairRequestReply_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_PairRequestReply_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_PairRequestReply_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.PairRequestReply(true));
    EXPECT_FALSE(device_.PairRequestReply(false));

    GTEST_LOG_(INFO) << "Host_ModuleTest_PairRequestReply_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_GetTransportType_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_GetTransportType_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_GetTransportType_00100 start";

    BluetoothRemoteDevice device1_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_EQ(device1_.GetTransportType(), BT_TRANSPORT_BREDR);

    BluetoothRemoteDevice device2_("00:00:00:00:00:01", BT_TRANSPORT_BLE);
    EXPECT_EQ(device2_.GetTransportType(), BT_TRANSPORT_BLE);

    GTEST_LOG_(INFO) << "Host_ModuleTest_GetTransportType_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_ReadRemoteRssiValue_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_ReadRemoteRssiValue_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_ReadRemoteRssiValue_00100 start";

    BluetoothRemoteDevice device_("00:00:00:00:00:00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device_.ReadRemoteRssiValue());

    GTEST_LOG_(INFO) << "Host_ModuleTest_ReadRemoteRssiValue_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_IsValidBluetoothRemoteDevice_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_IsValidBluetoothRemoteDevice_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothRemoteDevice_00100 start";

    BluetoothRemoteDevice device0_("00", BT_TRANSPORT_BREDR);
    EXPECT_FALSE(device0_.IsValidBluetoothRemoteDevice());

    BluetoothRemoteDevice device1_("00:00:00:00:00:00", 3);
    EXPECT_FALSE(device1_.IsValidBluetoothRemoteDevice());

    GTEST_LOG_(INFO) << "Host_ModuleTest_IsValidBluetoothRemoteDevice_00100 end";
}

/**
 * @tc.number: Host_ModuleTest_DisableBt_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(HostTest, Host_ModuleTest_DisableBt_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Host_ModuleTest_DisableBt_00100 start";

    host_ = &BluetoothHost::GetDefaultHost();
    EXPECT_TRUE(host_->DisableBt());
    std::this_thread::sleep_for(std::chrono::seconds(3));

    GTEST_LOG_(INFO) << "Host_ModuleTest_DisableBt_00100 end";
}
}  // namespace Bluetooth
}  // namespace OHOS