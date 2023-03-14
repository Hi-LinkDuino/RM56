/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "usbd_device_test.h"
#include <iostream>
#include <vector>
#include "hdf_log.h"
#include "usb_param.h"
#include "usbd_client.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;

const int SLEEP_TIME = 3;

const uint8_t BUS_NUM_1 = 1;
const uint8_t DEV_ADDR_2 = 2;

const uint8_t BUS_NUM_255 = 255;
const uint8_t DEV_ADDR_255 = 255;

void UsbdDeviceTest::SetUpTestCase(void)
{
    auto ret = UsbdClient::GetInstance().SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdFunctionTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }
    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void UsbdDeviceTest::TearDownTestCase(void) {}

void UsbdDeviceTest::SetUp(void) {}

void UsbdDeviceTest::TearDown(void) {}

/**
 * @tc.name: UsbdDevice001
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdOpenDevice001, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result =%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDevice002
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdOpenDevice002, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_255;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDevice003
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdOpenDevice003, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_255;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDevice004
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdOpenDevice004, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_255;
    uint8_t devAddr = DEV_ADDR_255;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdDevice011
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdCloseDevice001, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDevice012
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdCloseDevice002, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_255;
    ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    dev.busNum = BUS_NUM_1;
    UsbdClient::GetInstance().CloseDevice(dev);
}

/**
 * @tc.name: UsbdDevice013
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdCloseDevice003, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = DEV_ADDR_255;
    ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    dev.devAddr = DEV_ADDR_2;
    UsbdClient::GetInstance().CloseDevice(dev);
}

/**
 * @tc.name: UsbdDevice014
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, UsbdCloseDevice004, TestSize.Level1)
{
    uint8_t busNum = BUS_NUM_1;
    uint8_t devAddr = DEV_ADDR_2;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_255;
    dev.devAddr = DEV_ADDR_255;
    ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    UsbdClient::GetInstance().CloseDevice(dev);
}
