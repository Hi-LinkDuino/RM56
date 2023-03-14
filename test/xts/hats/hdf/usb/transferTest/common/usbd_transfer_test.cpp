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

#include "usbd_transfer_test.h"
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
const uint8_t BUS_NUM_222 = 222;
const uint32_t LENGTH_NUM_255 = 255;
const uint8_t INTERFACEID_1 = 1;
const uint8_t POINTID_1 = 1;
const uint8_t POINTID_129 = 129;

void UsbdTransferTest::SetUpTestCase(void)
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
    HDF_LOGI("UsbdTransferTest:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

void UsbdTransferTest::TearDownTestCase(void)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    auto ret = UsbdClient::GetInstance().CloseDevice(dev);
    HDF_LOGI("UsbdTransferTest:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

void UsbdTransferTest::SetUp(void) {}

void UsbdTransferTest::TearDown(void) {}

/**
 * @tc.name: UsbdControlTransfer001
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 8, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer001 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdControlTransfer002
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_255;
    dev.devAddr = DEV_ADDR_2;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 8, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer002 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdControlTransfer004
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer004, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 6, 0x100, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer004 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdControlTransfer010
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer010, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 0, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer010 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdControlTransfer013
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer013, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer013 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdControlTransfer016
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer016, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer016 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdControlTransfer019
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer019, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint32_t length = LENGTH_NUM_255;
    uint8_t buffer[LENGTH_NUM_255] = {};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0X0C, 0, 0, 1000};
    auto ret = UsbdClient::GetInstance().ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer019 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdBulkTransferRead001
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead001 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdBulkTransferRead002
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdBulkTransferWrite001
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ01";
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdBulkTransferWrite002
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdBulkTransferWrite008
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite008, Function | MediumTest | Level1)
{
    HDF_LOGI("Case Start : UsbdBulkTransferWrite008 : BulkTransferWrite");
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().BulkTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: InterruptTransferRead001
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead001 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdInterruptTransferRead002
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdInterruptTransferWrite001
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ01";
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdInterruptTransferWrite002
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdInterruptTransferWrite008
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite008, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().InterruptTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdIsoTransferRead001
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[LENGTH_NUM_255] = {0};
    uint32_t length = LENGTH_NUM_255;
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead001 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdIsoTransferRead002
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_129;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d interfaceId=%{public}d", __LINE__, interfaceId);
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = BUS_NUM_222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdIsoTransferWrite001
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite001, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ01";
    struct UsbPipe pipe = {interfaceId, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdIsoTransferWrite002
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite002, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: UsbdIsoTransferWrite008
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite008, Function | MediumTest | Level1)
{
    struct UsbDev dev = {BUS_NUM_1, DEV_ADDR_2};
    dev.busNum = BUS_NUM_1;
    dev.devAddr = DEV_ADDR_2;
    uint8_t interfaceId = INTERFACEID_1;
    uint8_t pointid = POINTID_1;
    auto ret = UsbdClient::GetInstance().ClaimInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceId, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::GetInstance().IsoTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}
