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
class UsbFnDeviceSdkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UsbFnDeviceSdkTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void UsbFnDeviceSdkTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void UsbFnDeviceSdkTest::SetUp()
{
    printf("%s\n", __func__);
}

void UsbFnDeviceSdkTest::TearDown()
{
   printf("%s\n", __func__);
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCreate006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CREATE_DEVICE006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStatus001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DEVICE_STATUS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStatus002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DEVICE_STATUS002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStatus003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DEVICE_STATUS003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStatus004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DEVICE_STATUS004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStatus005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DEVICE_STATUS005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetDevice006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_DEVICE006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetInterface006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_INTERFACE006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetPipeInfo006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PIPEINFO006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceRegistProp007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_REGIST_PROP007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetProp008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_PROP008, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceSetProp007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_SET_PROP007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocCtrlRequest008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_CTRLREQUEST008, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest008, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST008, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceAllocRequest009, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_ALLOC_REQUEST009, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceFreeRequest006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_FREE_REQUEST006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceGetRequestStatus006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_GET_REQUEST_STATUS006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStopRecvEvent001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_STOP_EVENT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStopRecvEvent002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_STOP_EVENT002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStopRecvEvent003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_STOP_EVENT003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStopRecvEvent004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_STOP_EVENT004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent006, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT006, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceStartRecvEvent007, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_START_EVENT007, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceOpenInterface001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_OPEN_INTERFACE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceOpenInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_OPEN_INTERFACE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceOpenInterface003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_OPEN_INTERFACE003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceOpenInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_OPEN_INTERFACE004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceOpenInterface005, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_OPEN_INTERFACE005, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCloseInterface001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CLOSE_INTERFACE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCloseInterface002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CLOSE_INTERFACE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCloseInterface003, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CLOSE_INTERFACE003, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceCloseInterface004, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_CLOSE_INTERFACE004, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceDelete001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DELETE_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(UsbFnDeviceSdkTest, UsbDeviceDelete002, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_USB_DEVICE_TYPE, USB_DEVICE_DELETE_DEVICE002, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};
