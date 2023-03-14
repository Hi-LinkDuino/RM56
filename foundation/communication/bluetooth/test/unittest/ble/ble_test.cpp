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

#include "ble_test.h"
#include "bluetooth_def.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace OHOS {
namespace Bluetooth {
BleTest *BleTest::bleInstance_ = nullptr;
namespace {
const static int defaultInt = 150;
const static int INTERVAL = 350;
const static int MIN_ADV_LENGTH = 31;
const static int MAX_ADV_LENGTH = 1650;
Bluetooth::UUID g_uuid = Bluetooth::UUID::FromString("00000000-0000-1000-8000-00805F9B34FB");
Bluetooth::UUID g_serviceDataUuid = Bluetooth::UUID::FromString("00000000-0000-1000-8000-00805F9B34FA");
Bluetooth::UUID g_serviceUuid = Bluetooth::UUID::FromString("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
Bluetooth::UUID g_serviceUuidMask = Bluetooth::UUID::FromString("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
Bluetooth::UUID g_serviceSolicitationUuid = Bluetooth::UUID::FromString("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
Bluetooth::UUID g_serviceSolicitationUuidMask = Bluetooth::UUID::FromString("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
std::string g_serviceData = "123";
int g_manufacturerId = 24;
std::string g_manufacturerData = "1567";

std::mutex g_mx;
std::condition_variable g_cv;
bool g_ready = false;
int g_wait_time = 3;
}  // namespace

void BleTest::InitAdvertiseSettings()
{
    BleTest::bleInstance_->bleAdvertiserSettings_.SetConnectable(true);
    BleTest::bleInstance_->bleAdvertiserSettings_.SetLegacyMode(true);
    BleTest::bleInstance_->bleAdvertiserSettings_.SetInterval(INTERVAL);
    BleTest::bleInstance_->bleAdvertiserSettings_.SetTxPower(BLE_ADV_TX_POWER_LEVEL::BLE_ADV_TX_POWER_MEDIUM);
    BleTest::bleInstance_->bleAdvertiserSettings_.SetPrimaryPhy(PHY_TYPE::PHY_LE_ALL_SUPPORTED);
    BleTest::bleInstance_->bleAdvertiserSettings_.SetSecondaryPhy(PHY_TYPE::PHY_LE_2M);
}
void BleTest::InitScanSettings()
{
    BleTest::bleInstance_->bleScanSettings_.SetReportDelay(defaultInt);
    BleTest::bleInstance_->bleScanSettings_.SetScanMode(SCAN_MODE::SCAN_MODE_LOW_POWER);
    BleTest::bleInstance_->bleScanSettings_.SetLegacy(true);
    BleTest::bleInstance_->bleScanSettings_.SetPhy(PHY_TYPE::PHY_LE_ALL_SUPPORTED);
}

bool BleTest::EnableBle()
{
    GTEST_LOG_(INFO) << "enable ble start";
    std::unique_lock lock(g_mx);
    bool isEnable = host_->EnableBle();
    EXPECT_TRUE(isEnable);
    while (!g_ready) {
        g_cv.wait(lock);
    }
    std::this_thread::sleep_for(std::chrono::seconds(g_wait_time));
    EXPECT_TRUE(host_->IsBleEnabled());
    GTEST_LOG_(INFO) << "enable ble end";
    return isEnable;
}

bool BleTest::DisableBle()
{
    GTEST_LOG_(INFO) << "disable ble start";
    std::unique_lock lock(g_mx);
    bool isEnable = host_->DisableBle();
    EXPECT_TRUE(isEnable);
    while (!g_ready) {
        g_cv.wait(lock);
    }
    std::this_thread::sleep_for(std::chrono::seconds(g_wait_time));
    EXPECT_FALSE(host_->IsBleEnabled());
    GTEST_LOG_(INFO) << "disable ble end";
    return isEnable;
}

bool BleTest::HaveUuid(Bluetooth::BleAdvertiserData advData)
{
    for (Bluetooth::UUID uuid : advData.GetServiceUuids()) {
        if (uuid.Equals(g_uuid)) {
            isHaveUuid = true;
        }
    }
    return isHaveUuid;
}

bool BleTest::ManufacturerData(Bluetooth::BleAdvertiserData advData)
{
    if (advData.GetManufacturerData().empty()) {
        return false;
    } else {
        std::map<uint16_t, std::string> data = advData.GetManufacturerData();
        std::map<uint16_t, std::string>::iterator iter = data.begin();
        for (; iter != data.end(); ++iter) {
            if (g_manufacturerId == iter->first) {
                EXPECT_STREQ(g_manufacturerData.c_str(), iter->second.c_str());
            }
        }
    }
    return true;
}

bool BleTest::ServiceData(Bluetooth::BleAdvertiserData advData)
{
    if (advData.GetServiceData().empty()) {
        return false;
    } else {
        for (auto &iter : advData.GetServiceData()) {
            if (g_serviceData == iter.second) {
                EXPECT_STREQ(g_serviceData.c_str(), iter.second.c_str());
            }
        }
    }
    return true;
}

BleTest::BleTest()
{
    bleInstance_ = this;
    host_ = &BluetoothHost::GetDefaultHost();
}
BleTest::~BleTest()
{}

void BleTest::SetUpTestCase(void)
{}

void BleTest::TearDownTestCase(void)
{}

void BleTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    host_->RegisterObserver(bleHostObserverTest_);
}

void BleTest::TearDown()
{
    GTEST_LOG_(INFO) << "SetUp";
    host_->DeregisterObserver(bleHostObserverTest_);
}

void BleAdvertiseCallbackTest::OnStartResultEvent(int result)
{}

void BleHostObserverTest::OnStateChanged(const int transport, const int status)
{
    GTEST_LOG_(INFO) << "OnStateChanged";
    std::unique_lock lock(g_mx);
    if (transport == BT_TRANSPORT_BLE) {
        if (status == STATE_TURN_ON || status == STATE_TURN_OFF) {
            g_ready = true;
            g_cv.notify_all();
        }
    }
}


/**
 * @tc.number: BLE_ModuleTest_StartAdvertising_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(BleTest, BLE_ModuleTest_StartAdvertising_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Ble_ModuleTest_StartAdvertising_00100 start";

    EXPECT_TRUE(EnableBle());
    BleTest::bleInstance_->InitAdvertiseSettings();
    BleAdvertiser bleAdvertise;
    BleAdvertiserData advData;
    BleAdvertiserData scanData;
    advData.AddServiceUuid(g_uuid);
    advData.AddManufacturerData(g_manufacturerId, g_manufacturerData);
    advData.AddServiceData(g_serviceDataUuid, g_serviceData);
    advData.SetAdvFlag(BLE_ADV_FLAG_GEN_DISC);
    bleAdvertise.StartAdvertising(
        BleTest::bleInstance_->bleAdvertiserSettings_, advData, scanData, bleAdvertiseCallbackTest_);

    GTEST_LOG_(INFO) << "advData function test";
    EXPECT_TRUE(BleTest::bleInstance_->HaveUuid(advData));
    EXPECT_TRUE(BleTest::bleInstance_->ManufacturerData(advData));
    EXPECT_TRUE(BleTest::bleInstance_->ServiceData(advData));
    EXPECT_EQ(BLE_ADV_FLAG_GEN_DISC, advData.GetAdvFlag());

    bleAdvertise.StopAdvertising(bleAdvertiseCallbackTest_);
    vector<uint8_t> advData1 = {1, 2, 3};
    vector<uint8_t> scanData1 = {3, 2, 1};
    bleAdvertise.StartAdvertising(
        BleTest::bleInstance_->bleAdvertiserSettings_, advData1, scanData1, bleAdvertiseCallbackTest_);
    bleAdvertise.StopAdvertising(bleAdvertiseCallbackTest_);
    int32_t length = host_->GetBleMaxAdvertisingDataLength();
    EXPECT_LE(MIN_ADV_LENGTH, length);
    EXPECT_GE(MAX_ADV_LENGTH, length);

    bleAdvertise.Close(bleAdvertiseCallbackTest_);
    EXPECT_TRUE(DisableBle());
    GTEST_LOG_(INFO) << "Ble_ModuleTest_StartAdvertising_00100 end";
}

HWTEST_F(BleTest, BLE_ModuleTest_StartAdvertising_00200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartAdvertising_00200 start";
    BleTest::bleInstance_->InitAdvertiseSettings();
    EXPECT_TRUE(BleTest::bleInstance_->bleAdvertiserSettings_.IsConnectable());
    EXPECT_TRUE(BleTest::bleInstance_->bleAdvertiserSettings_.IsLegacyMode());
    EXPECT_EQ(static_cast<uint16_t>(INTERVAL), BleTest::bleInstance_->bleAdvertiserSettings_.GetInterval());
    EXPECT_EQ(
        BLE_ADV_TX_POWER_LEVEL::BLE_ADV_TX_POWER_MEDIUM, BleTest::bleInstance_->bleAdvertiserSettings_.GetTxPower());
    EXPECT_EQ(PHY_TYPE::PHY_LE_ALL_SUPPORTED, BleTest::bleInstance_->bleAdvertiserSettings_.GetPrimaryPhy());
    EXPECT_EQ(PHY_TYPE::PHY_LE_2M, BleTest::bleInstance_->bleAdvertiserSettings_.GetSecondaryPhy());
    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartAdvertising_00200 end";
}

/**
 * @tc.number: BLE_ModuleTest_StartCentralManager_00100
 * @tc.name:
 * @tc.desc:
 */
HWTEST_F(BleTest, BLE_ModuleTest_StartCentralManager_00100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartCentralManager_00100 start";
    EXPECT_TRUE(EnableBle());

    std::vector<BleScanFilter> filters;
    BleScanFilter filter;
    std::string deviceId = "42:0B:AC:A0:A0:ED";
    uint16_t manufacturerId = 0x4147;
    std::vector<uint8_t> serviceData = {0x1f, 0x77};
    std::vector<uint8_t> serviceDataMask = {0xff, 0xff};
    std::vector<uint8_t> manufactureData = {0x01, 0x09, 0x20, 0x02};
    std::vector<uint8_t> manufactureDataMask = {0x01, 0x09, 0x20, 0x02};
    filter.SetDeviceId(deviceId);
    filter.SetServiceUuid(g_serviceUuid);
    filter.SetServiceUuidMask(g_serviceUuidMask);
    filter.SetServiceSolicitationUuid(g_serviceSolicitationUuid);
    filter.SetServiceSolicitationUuidMask(g_serviceSolicitationUuidMask);
    filter.SetManufacturerId(manufacturerId);
    filter.SetServiceData(serviceData);
    filter.SetServiceDataMask(serviceDataMask);
    filter.SetManufactureData(manufactureData);
    filter.SetManufactureDataMask(manufactureDataMask);
    filters.push_back(filter);
    BleTest::bleInstance_->InitScanSettings();
    BleCentralManager bleCentralManager(bleCentralManagerCallbackTest_);
    bleCentralManager.ConfigScanFilter(filters);
    bleCentralManager.StartScan(BleTest::bleInstance_->bleScanSettings_);
    EXPECT_TRUE(host_->IsBtDiscovering(1));
    bleCentralManager.StopScan();
    EXPECT_TRUE(DisableBle());

    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartCentralManager_00100 end";
}

/**
 * @tc.number: BLE_ModuleTest_StartCentralManager_00200
 * @tc.name:
 * @tc.desc:
 */

HWTEST_F(BleTest, BLE_ModuleTest_StartCentralManager_00200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartCentralManager_00200 start";
    BleTest::bleInstance_->InitScanSettings();
    EXPECT_EQ(defaultInt, BleTest::bleInstance_->bleScanSettings_.GetReportDelayMillisValue());
    EXPECT_EQ(SCAN_MODE::SCAN_MODE_LOW_POWER, BleTest::bleInstance_->bleScanSettings_.GetScanMode());
    EXPECT_TRUE(BleTest::bleInstance_->bleScanSettings_.GetLegacy());
    EXPECT_EQ(PHY_TYPE::PHY_LE_ALL_SUPPORTED, BleTest::bleInstance_->bleScanSettings_.GetPhy());
    GTEST_LOG_(INFO) << "BLE_ModuleTest_StartCentralManager_00200 end";
}
}  // namespace Bluetooth
}  // namespace OHOS
