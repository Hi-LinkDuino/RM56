/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "usbd_request_test.h"
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
const uint8_t BUS_NUM_222 = 222;
const uint8_t DEV_ADDR_222 = 222;
const uint32_t LENGTH_NUM_255 = 255;
const uint32_t TAG_LENGTH_NUM_1000 = 1000;
const int TAG_NUM_10 = 10;
const int TAG_NUM_11 = 11;
const uint8_t INTERFACEID_1 = 1;
const uint8_t POINTID_1 = 1;
const uint8_t POINTID_129 = 129;

void UsbdRequestTest::SetUpTestCase(void)
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
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    ret = UsbdClient::GetInstance().OpenDevice(dev);
    HDF_LOGI("UsbdRequestTest:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

void UsbdRequestTest::TearDownTestCase(void)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    auto ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdRequestTest:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

void UsbdRequestTest::SetUp(void) {}

void UsbdRequestTest::TearDown(void) {}

/**
 * @tc.name: UsbdConfig001
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdSetConfig001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().SetConfig(dev, configIndex);
    HDF_LOGI("UsbdRequestTest::UsbdSetConfigConfig001 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdConfig002
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdSetConfig002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().SetConfig(dev, configIndex);
    HDF_LOGI("UsbdRequestTest::UsbdSetConfig002 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdConfig001
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetConfig001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().GetConfig(dev, configIndex);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfig001 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdConfig002
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetConfig002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().GetConfig(dev, configIndex);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfig002 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdClaimInterface001
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceId);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdClaimInterface001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdClaimInterface001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdClaimInterface002
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceId);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdClaimInterface002, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {busNum, devAddr};
    dev.busNum = 20;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdClaimInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdSetInterface001
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceId, uint8_t altIndex);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdSetInterface001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t altIndex = 0;

    struct UsbDev dev = {busNum, devAddr};

    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdClaimInterface001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::GetInstance().SetInterface(dev, interfaceId, altIndex);
    HDF_LOGI("UsbdRequestTest::UsbdSetInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetInterface002
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceId, uint8_t altIndex);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdSetInterface002, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;

    uint8_t altIndex = 0;
    struct UsbDev dev = {busNum, devAddr};

    
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdSetInterface002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    ret = UsbdClient::GetInstance().SetInterface(dev, interfaceId, altIndex);
    HDF_LOGI("UsbdRequestTest::UsbdSetInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetDeviceDescriptor001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetDeviceDescriptor002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetDeviceDescriptor004, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = 0;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetDeviceDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetStringDescriptor001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 0;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetStringDescriptor002, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 1;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor003
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetStringDescriptor003, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 222;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor003 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetStringDescriptor004, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 255;
    uint8_t stringId = 0;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = 8;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetStringDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetConfigDescriptor001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configId = 0;
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetConfigDescriptor002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    uint8_t configId = 1;
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetConfigDescriptor004, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configId = 1;
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = LENGTH_NUM_255;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetInstance().GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdRequestTest::UsbdGetConfigDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdGetRawDescriptor001
 * @tc.desc: Test functions to GetRawDescriptor
 * @tc.desc: int32_t GetRawDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetRawDescriptor001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> rawData;
    auto ret = UsbdClient::GetInstance().GetRawDescriptor(dev, rawData);
    HDF_LOGI("UsbdRequestTest::UsbdGetRawDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             rawData.size(), sizeof(rawData));
    HDF_LOGI("UsbdRequestTest::UsbdGetRawDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdGetRawDescriptor002
 * @tc.desc: Test functions to GetRawDescriptor
 * @tc.desc: int32_t GetRawDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdGetRawDescriptor002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    struct UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> rawData;
    auto ret = UsbdClient::GetInstance().GetRawDescriptor(dev, rawData);
    HDF_LOGI("UsbdRequestTest::UsbdGetRawDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             rawData.size(), sizeof(rawData));
    HDF_LOGI("UsbdRequestTest::UsbdGetRawDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: GetFileDescriptor001
 * @tc.desc: Test functions to GetFileDescriptor
 * @tc.desc: int32_t GetFileDescriptor(const UsbDev &dev, int32_t &fd);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, GetFileDescriptor001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    struct UsbDev dev = {busNum, devAddr};
    int32_t fd = 0;
    auto ret = UsbdClient::GetInstance().GetFileDescriptor(dev, fd);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor001 %{public}d GetFileDescriptor=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor001 %{public}d fd=%{public}d", __LINE__, fd);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: GetFileDescriptor002
 * @tc.desc: Test functions to GetFileDescriptor
 * @tc.desc: int32_t GetFileDescriptor(const UsbDev &dev, int32_t &fd);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, GetFileDescriptor002, Function | MediumTest | Level1)
{
    uint8_t busNum = BUS_NUM_222;
    uint8_t devAddr = 2;
    struct UsbDev dev = {busNum, devAddr};
    int32_t fd = 0;
    auto ret = UsbdClient::GetInstance().GetFileDescriptor(dev, fd);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor002 %{public}d GetFileDescriptor=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor002 %{public}d fd=%{public}d", __LINE__, fd);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: GetFileDescriptor003
 * @tc.desc: Test functions to GetFileDescriptor
 * @tc.desc: int32_t GetFileDescriptor(const UsbDev &dev, int32_t &fd);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, GetFileDescriptor003, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = DEV_ADDR_222;
    struct UsbDev dev = {busNum, devAddr};
    int32_t fd = 0;
    auto ret = UsbdClient::GetInstance().GetFileDescriptor(dev, fd);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor003 %{public}d GetFileDescriptor=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor003 %{public}d fd=%{public}d", __LINE__, fd);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: GetFileDescriptor004
 * @tc.desc: Test functions to GetFileDescriptor
 * @tc.desc: int32_t GetFileDescriptor(const UsbDev &dev, int32_t &fd);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, GetFileDescriptor004, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    struct UsbDev dev = {busNum, devAddr};
    int32_t fd = LENGTH_NUM_255;
    auto ret = UsbdClient::GetInstance().GetFileDescriptor(dev, fd);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor004 %{public}d GetFileDescriptor=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdRequestTest::GetFileDescriptor004 %{public}d fd=%{public}d", __LINE__, fd);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestQueue001, Function | MediumTest | Level1)
{
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue001 %{public}d interfaceId=%{public}d pointid=%{public}d", __LINE__,
             interfaceId, pointid);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestQueue002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.devAddr = DEV_ADDR_2;
    dev.busNum = BUS_NUM_1;
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    dev = {222, 222};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue002 %{public}d interfaceId=%{public}d pointid=%{public}d", __LINE__,
             interfaceId, pointid);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdRequest007
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestQueue007, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t buffer[LENGTH_NUM_255] = "request 007";
    uint32_t length = LENGTH_NUM_255;
    uint8_t pointid = POINTID_1;
    uint8_t interfaceId = INTERFACEID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue write";
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestQueue007 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestWait001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = LENGTH_NUM_255;
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::GetInstance().RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait001 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    delete[] clientObj;
    clientObj = nullptr;
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestWait002, Function | MediumTest | Level1)
{
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint32_t length = LENGTH_NUM_255;
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbPipe pipe = {interfaceId, pointid};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::GetInstance().RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait002 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    delete[] clientObj;
    clientObj = nullptr;
}

/**
 * @tc.name: UsbdRequest004
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestWait004, Function | MediumTest | Level1)
{
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {};
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    uint32_t length = LENGTH_NUM_255;
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait004 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::GetInstance().RequestWait(dev, waitdata, bufferdata, -10000);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait004 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    delete[] clientObj;
    clientObj = nullptr;
}

/**
 * @tc.name: UsbdRequest005
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestWait005, Function | MediumTest | Level1)
{
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = LENGTH_NUM_255;
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    uint8_t buffer[LENGTH_NUM_255] = {};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait005 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    dev.devAddr = DEV_ADDR_255;
    dev.busNum = BUS_NUM_255;
    ret = UsbdClient::GetInstance().RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdRequestTest::UsbdRequestWait005 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    delete[] clientObj;
    clientObj = nullptr;
}

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestCancel001, Function | MediumTest | Level1)
{
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t buffer[LENGTH_NUM_255] = "request001";
    uint32_t length = LENGTH_NUM_255;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::GetInstance().RequestCancel(dev, pipe);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel001 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestCancel002, Function | MediumTest | Level1)
{
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t buffer[LENGTH_NUM_255] = "request002";
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ;
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    ret = UsbdClient::GetInstance().RequestCancel(dev, pipe);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel002 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    dev.busNum = BUS_NUM_1;
    ret = UsbdClient::GetInstance().RequestCancel(dev, pipe);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest004
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestCancel004, Function | MediumTest | Level1)
{
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue read";
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = "request004";
    uint8_t pointid = POINTID_129;
    uint8_t interfaceId = INTERFACEID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel004 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    pipe.interfaceId = 222;
    pipe.endpointId = 222;
    ret = UsbdClient::GetInstance().RequestCancel(dev, pipe);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel004 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdRequest005
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdRequestCancel005, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t buffer[LENGTH_NUM_255] = "request005";
    uint32_t length = LENGTH_NUM_255;
    uint8_t pointid = POINTID_1;
    uint8_t interfaceId = INTERFACEID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    uint8_t tag[TAG_LENGTH_NUM_1000] = "queue Write";
    std::vector<uint8_t> clientdata = {tag, tag + TAG_NUM_11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel005 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::GetInstance().RequestCancel(dev, pipe);
    HDF_LOGI("UsbdRequestTest::UsbdRequestCancel005 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdReleaseInterface001
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceId);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdReleaseInterface001, Function | MediumTest | Level1)
{
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().ReleaseInterface(dev, interfaceId);
    HDF_LOGI("UsbdRequestTest::UsbdReleaseInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdReleaseInterface002
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceId);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdRequestTest, UsbdReleaseInterface002, Function | MediumTest | Level1)
{
    uint8_t busNum = 25;
    uint8_t devAddr = 2;
    uint8_t interfaceId = INTERFACEID_1;
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::GetInstance().ReleaseInterface(dev, interfaceId);
    HDF_LOGI("UsbdRequestTest::UsbdReleaseInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}
