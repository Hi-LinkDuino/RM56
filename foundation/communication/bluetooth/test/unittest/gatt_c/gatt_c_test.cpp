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
#include <securec.h>

#include "bluetooth_def.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt.h"

using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
class GattCTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void GattCTest::SetUpTestCase(void) {}

void GattCTest::TearDownTestCase(void) {}

void GattCTest::SetUp() {}

void GattCTest::TearDown() {}

/*
 * @tc.name: GattCTest_001
 * @tc.desc: Test BleStartScanEx when g_BleCentralManager or BleScanConfigs is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5OH5C
*/
HWTEST_F(GattCTest, GattCTest_001, TestSize.Level1)
{
    BleScanConfigs configs;
    BleScanNativeFilter filter[1];
    int filterSize = sizeof(filter) / sizeof(BleScanNativeFilter);
    int res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_FAIL);

    BtGattCallbacks btGattCallbacks;
    BleGattRegisterCallbacks(&btGattCallbacks);
    res = BleStartScanEx(nullptr, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_FAIL);
}

/*
 * @tc.name: GattCTest_002
 * @tc.desc: Test BleStartScanEx when BleScanNativeFilter is null or filterSize is zero
 * @tc.type: FUNC
 * @tc.require: issueI5OH5C
*/
HWTEST_F(GattCTest, GattCTest_002, TestSize.Level1)
{
    BleScanConfigs configs;
    configs.reportDelayMillis = 0;
    configs.scanMode = SCAN_MODE_LOW_POWER;
    configs.legacy = true;
    configs.phy = PHY_LE_ALL_SUPPORTED;

    BleScanNativeFilter filter[1];
    int filterSize = sizeof(filter) / sizeof(BleScanNativeFilter);
    int res = BleStartScanEx(&configs, nullptr, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);

    res = BleStartScanEx(&configs, filter, 0);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);
}

/*
 * @tc.name: GattCTest_003
 * @tc.desc: Test BleStartScanEx success status
 * @tc.type: FUNC
 * @tc.require: issueI5OH5C
*/
HWTEST_F(GattCTest, GattCTest_003, TestSize.Level1)
{
    BleScanConfigs configs;
    configs.reportDelayMillis = 0;
    configs.scanMode = SCAN_MODE_LOW_POWER;
    configs.legacy = true;
    configs.phy = PHY_LE_ALL_SUPPORTED;

    BleScanNativeFilter filter[1];
    filter[0].address = (char *)"test";
    filter[0].deviceName = (char *)"test";
    filter[0].serviceUuid = (unsigned char*)"FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    filter[0].serviceUuidMask = (unsigned char*)"FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    filter[0].serviceUuidLength = sizeof("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
    unsigned char test1[1];
    unsigned char test2[1];
    unsigned char test3[1];
    unsigned char test4[1];
    filter[0].serviceData = test1;
    filter[0].serviceDataMask = test2;
    filter[0].manufactureData = test3;
    filter[0].manufactureDataMask = test4;
    filter[0].serviceData[0] = 1;
    filter[0].serviceDataMask[0] = 1;
    filter[0].manufactureData[0] = 1;
    filter[0].manufactureDataMask[0] = 1;
    filter[0].serviceDataLength = 1;
    filter[0].manufactureDataLength = 1;
    filter[0].manufactureId = 1;
    int filterSize = sizeof(filter) / sizeof(BleScanNativeFilter);
    int res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);

    filter[0].address = nullptr;
    filter[0].deviceName = nullptr;
    filter[0].serviceUuidMask = nullptr;
    filter[0].serviceData = nullptr;
    filter[0].manufactureData = nullptr;
    res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);

    filter[0].serviceData = test1;
    filter[0].manufactureData = test3;
    filter[0].serviceDataMask = nullptr;
    filter[0].manufactureDataMask = nullptr;
    filter[0].serviceUuid = nullptr;
    res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);
}

/*
 * @tc.name: GattCTest_004
 * @tc.desc: Test BleStartScanEx when serviceUuid's length is random
 * @tc.type: FUNC
 * @tc.require: issueI5OH5C
*/
HWTEST_F(GattCTest, GattCTest_004, TestSize.Level1)
{
    BleScanConfigs configs;
    configs.reportDelayMillis = 0;
    configs.scanMode = SCAN_MODE_LOW_POWER;
    configs.legacy = true;
    configs.phy = PHY_LE_ALL_SUPPORTED;

    BleScanNativeFilter filter[1];
    filter[0].address = nullptr;
    filter[0].deviceName = nullptr;
    filter[0].serviceUuid = (unsigned char*)"";
    filter[0].serviceUuidMask = (unsigned char*)"";
    filter[0].serviceUuidLength = 0;
    filter[0].serviceData = nullptr;
    filter[0].manufactureData = nullptr;
    int filterSize = sizeof(filter) / sizeof(BleScanNativeFilter);
    int res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);

    filter[0].serviceUuid = (unsigned char*)"FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    filter[0].serviceUuidMask = (unsigned char*)"FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    filter[0].serviceUuidLength = sizeof("FFFFFFF0-FFFF-FFFF-FFFF-FFFFFFFFFFFF");
    res = BleStartScanEx(&configs, filter, filterSize);
    EXPECT_EQ(res, OHOS_BT_STATUS_SUCCESS);
}
}  // namespace Bluetooth
}  // namespace OHOS
