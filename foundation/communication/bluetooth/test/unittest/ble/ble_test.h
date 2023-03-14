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
#include "bluetooth_ble_advertiser.h"
#include "bluetooth_ble_central_manager.h"
#include "bluetooth_host.h"

namespace OHOS {
namespace Bluetooth {
using namespace testing::ext;

class BleAdvertiseCallbackTest : public Bluetooth::BleAdvertiseCallback {
public:
    BleAdvertiseCallbackTest(){};
    ~BleAdvertiseCallbackTest(){};

private:
    void OnStartResultEvent(int result);
};

class BleHostObserverTest : public Bluetooth::BluetoothHostObserver {
public:
    BleHostObserverTest(){};
    ~BleHostObserverTest(){};

private:
    void OnStateChanged(const int transport, const int status);
    void OnDiscoveryStateChanged(int status)
    {}
    void OnDiscoveryResult(const BluetoothRemoteDevice &device)
    {}
    void OnPairRequested(const BluetoothRemoteDevice &device)
    {}
    void OnPairConfirmed(const BluetoothRemoteDevice &device, int reqType, int number)
    {}
    void OnScanModeChanged(int mode)
    {}
    void OnDeviceNameChanged(const std::string &device)
    {}
    void OnDeviceAddrChanged(const std::string &address)
    {}
};

class BleCentralManagerCallbackTest : public Bluetooth::BleCentralManagerCallback {
public:
    BleCentralManagerCallbackTest(){};
    ~BleCentralManagerCallbackTest(){};

private:
    void OnScanCallback(const Bluetooth::BleScanResult &result)
    {}
    void OnBleBatchScanResultsEvent(const std::vector<Bluetooth::BleScanResult> &results)
    {}
    void OnStartOrStopScanEvent(int resultCode, bool isStartScan)
    {}
};

class BleTest : public testing::Test {
public:
    BleTest();
    ~BleTest();

    Bluetooth::BluetoothHost *host_ = nullptr;
    static BleTest *bleInstance_;
    Bluetooth::BleAdvertiserSettings bleAdvertiserSettings_ {};
    BleCentralManagerCallbackTest bleCentralManagerCallbackTest_ {};
    BleAdvertiseCallbackTest bleAdvertiseCallbackTest_ {};
    BleHostObserverTest bleHostObserverTest_ {};
    Bluetooth::BleScanSettings bleScanSettings_ {};

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void InitAdvertiseSettings();
    void InitScanSettings();
    bool HaveUuid(Bluetooth::BleAdvertiserData advData);
    bool ManufacturerData(Bluetooth::BleAdvertiserData advData);
    bool ServiceData(Bluetooth::BleAdvertiserData advData);
    bool EnableBle();
    bool DisableBle();

private:
    bool isHaveUuid = false;
};
}  // namespace Bluetooth
}  // namespace OHOS