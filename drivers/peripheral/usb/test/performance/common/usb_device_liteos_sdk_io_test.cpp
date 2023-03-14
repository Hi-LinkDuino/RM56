/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"
#include "hdf_usb_device_test.h"

using namespace testing::ext;

namespace ModuleTest {
class UsbDeviceLiteosSdkIoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UsbDeviceLiteosSdkIoTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void UsbDeviceLiteosSdkIoTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void UsbDeviceLiteosSdkIoTest::SetUp() {}

void UsbDeviceLiteosSdkIoTest::TearDown() {}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCreate001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestAsync001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_ASYNC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestAsync002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_ASYNC002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestAsync003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_ASYNC003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestAsync004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_ASYNC004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestAsync005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_ASYNC005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceRequestSync007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REQUEST_SYNC007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceCancelRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CANCEL_REQUEST006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbDeviceLiteosSdkIoTest, UsbDeviceDelete001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DELETE_DEVICE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
