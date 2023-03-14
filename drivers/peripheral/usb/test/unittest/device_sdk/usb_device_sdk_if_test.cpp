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

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
extern "C" {
#include "usb_device_cdcacm_test.h"
}

using namespace std;
using namespace testing::ext;
namespace {
#ifdef AUTO_NO_SKIP
static struct AcmDevice *g_acmDevice = nullptr;
#endif
class UsbDeviceSdkIfTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UsbDeviceSdkIfTest::SetUpTestCase()
{
}

void UsbDeviceSdkIfTest::TearDownTestCase()
{
}

void UsbDeviceSdkIfTest::SetUp()
{
}

void UsbDeviceSdkIfTest::TearDown()
{
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice001, TestSize.Level1)
{
    g_acmDevice = SetUpAcmDevice();
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice002, TestSize.Level1)
{
    struct UsbFnDevice *fnDev = nullptr;
    struct UsbFnDescriptorData descData;
    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = nullptr;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    EXPECT_EQ(nullptr, fnDev);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice003, TestSize.Level1)
{
    struct UsbFnDevice *fnDev = nullptr;
    struct UsbFnDescriptorData descData;
    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_1", &descData);
    EXPECT_EQ(nullptr, fnDev);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice004, TestSize.Level1)
{
    struct UsbFnDevice *fnDev = nullptr;
    struct UsbFnDescriptorData descData;
    descData.type = USBFN_DESC_DATA_TYPE_PROP;
    descData.property = nullptr;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    EXPECT_EQ(nullptr, fnDev);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice005, TestSize.Level1)
{
    struct UsbFnDevice *fnDev = nullptr;
    struct UsbFnDescriptorData descData;
    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    EXPECT_EQ(nullptr, fnDev);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice006, TestSize.Level1)
{
    struct UsbFnDevice *fnDev = nullptr;
    struct UsbFnDescriptorData descData;
    const char *udcName = NULL;
    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice(udcName, &descData);
    EXPECT_EQ(nullptr, fnDev);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfDeviceStatus001, TestSize.Level1)
{
    int32_t ret;
    UsbFnDeviceState devState;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    ret = UsbFnGetDeviceState(g_acmDevice->fnDev, &devState);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_TRUE((devState >= USBFN_STATE_BIND && devState <= USBFN_STATE_RESUME));
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfDeviceStatus002, TestSize.Level1)
{
    int32_t ret;
    UsbFnDeviceState devState;
    ret = UsbFnGetDeviceState(nullptr, &devState);
    EXPECT_NE(HDF_SUCCESS, ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfDeviceStatus003, TestSize.Level1)
{
    int32_t ret;
    int32_t count = 0;
    UsbFnDeviceState devState;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    for (count = 0; count < TEST_TIMES; count++) {
        ret = UsbFnGetDeviceState(g_acmDevice->fnDev, &devState);
        EXPECT_EQ(HDF_SUCCESS, ret);
        EXPECT_TRUE((devState >= USBFN_STATE_BIND && devState <= USBFN_STATE_RESUME));
    }
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfDeviceStatus004, TestSize.Level1)
{
    int32_t ret;
    ret = UsbFnGetDeviceState(nullptr, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfDeviceStatus005, TestSize.Level1)
{
    int32_t ret;
    UsbFnDeviceState *devState = NULL;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    ret = UsbFnGetDeviceState(g_acmDevice->fnDev, devState);
    EXPECT_NE(HDF_SUCCESS, ret);
}


HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice001, TestSize.Level1)
{
    const struct UsbFnDevice *device = nullptr;
    const char *udcName = "100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    EXPECT_NE(nullptr, device);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice002, TestSize.Level1)
{
    const struct UsbFnDevice *device = nullptr;
    const char *udcName = "100e0000.hidwc3_1";
    device = UsbFnGetDevice(udcName);
    EXPECT_EQ(nullptr, device);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice003, TestSize.Level1)
{
    struct UsbFnDevice *device = nullptr;
    device = (struct UsbFnDevice *)UsbFnGetDevice(nullptr);
    EXPECT_EQ(nullptr, device);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice004, TestSize.Level1)
{
    const struct UsbFnDevice *device = nullptr;
    const char *udcName = " 100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    EXPECT_EQ(nullptr, device);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice005, TestSize.Level1)
{
    const struct UsbFnDevice *device = nullptr;
    const char *udcName = "100e0000.hidwc3_0 ";
    device = UsbFnGetDevice(udcName);
    EXPECT_EQ(nullptr, device);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetDevice006, TestSize.Level1)
{
    const struct UsbFnDevice *device = nullptr;
    const char *udcName = "100e0000.hidwc3_0\0100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    EXPECT_EQ(nullptr, device);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface001, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0);
    EXPECT_NE(nullptr, fnInterface);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface002, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0x1);
    EXPECT_NE(nullptr, fnInterface);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface003, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0xA);
    EXPECT_EQ(nullptr, fnInterface);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface004, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0x20);
    EXPECT_EQ(nullptr, fnInterface);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface005, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(nullptr, 0);
    EXPECT_EQ(nullptr, fnInterface);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetInterface006, TestSize.Level1)
{
    struct UsbFnInterface *fnInterface = nullptr;
    int32_t cnt;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    for (cnt = 0; cnt < 0x2; cnt++) {
        fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, cnt);
        EXPECT_NE(nullptr, fnInterface);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo001, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    EXPECT_TRUE((nullptr != g_acmDevice));
    EXPECT_TRUE(nullptr != g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    EXPECT_TRUE(0 == info.id);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo002, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 1, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    EXPECT_TRUE(1 == info.id);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo003, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0xF, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo004, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0, nullptr);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo005, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    ret = UsbFnGetInterfacePipeInfo(nullptr, 1, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetpipeInfo006, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    EXPECT_TRUE((nullptr != g_acmDevice));
    EXPECT_TRUE(nullptr != g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->ctrlIface.fn, 0, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    EXPECT_TRUE(0 == info.id);
}
#endif

int32_t PropCallBack(const struct UsbFnInterface *intf, const char *name, const char *value)
{
    if (intf == nullptr || name == nullptr || value == nullptr) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp001, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp002, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp003, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    info.name = nullptr;
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp004, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    info.name = "name_test4";
    info.value = nullptr;
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp005, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    info.name = "name_test5";
    info.value = "test_value";
    info.getProp  = nullptr;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp006, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    info.name = "name_test5";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(nullptr, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp007, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->dataIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRegistProp008, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->dataIface.fn, &info);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp001, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "name_test", buffer);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    EXPECT_TRUE(strcmp(buffer, "test_value") == 0);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp002, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "unknown", buffer);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp003, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "idProduct", buffer);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp004, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfaceProp(nullptr, "idProduct", buffer);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp005, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "idProduct", nullptr);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp006, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    ret = UsbFnGetInterfaceProp(nullptr, "idProduct", buffer);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp007, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->dataIface.fn, "name_test", buffer);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    EXPECT_TRUE(strcmp(buffer, "test_value") == 0);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetProp008, TestSize.Level1)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnGetInterfaceProp(g_acmDevice->dataIface.fn, "idProduct", buffer);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp001, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "name_test", "hello");
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp002, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "unknown", "hello");
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp003, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(nullptr, "name_test", "hello");
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp004, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, nullptr, "hello");
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp005, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "idVendor", "0x625");
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp006, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "bLength", "0x14");
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfSetProp007, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnSetInterfaceProp(g_acmDevice->dataIface.fn, "name_test", "hello");
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest001, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest002, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
           sizeof(struct UsbCdcLineCoding) + sizeof(struct UsbCdcLineCoding));
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest003, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest004, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0x800);
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest005, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocCtrlRequest(nullptr, g_acmDevice->notifyPipe.maxPacketSize);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest006, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0x801);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest007, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->dataIface.handle, 0);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest008, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocCtrlRequest(g_acmDevice->dataIface.handle, 0x801);
    EXPECT_TRUE(nullptr == req);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocCtrlRequest009, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    UsbFnInterfaceHandle handle = NULL;
    req = UsbFnAllocCtrlRequest(handle, 0);
    EXPECT_TRUE(nullptr == req);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest001, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest002, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id, 0);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest003, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id, 0x800);
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest004, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(nullptr, g_acmDevice->notifyPipe.id, 0x800);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest005, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, 0x20, 0x800);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest006, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, 0, 0x801);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest007, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(nullptr != req);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest008, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id, 0);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest009, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id, 0x801);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfAllocRequest010, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, 0xf, 0x801);
    EXPECT_TRUE(nullptr == req);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest001, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest002, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest003, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        g_acmDevice->notifyPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest004, TestSize.Level1)
{
    int32_t ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest005, TestSize.Level1)
{
    int32_t ret;
    int32_t count;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    for (count = 0; count < TEST_TIMES; count++) {
        req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.maxPacketSize);
        EXPECT_TRUE(req != nullptr);
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfFreeRequest006, TestSize.Level1)
{
    int32_t ret;
    ret = UsbFnFreeRequest(nullptr);
    EXPECT_TRUE(ret != HDF_SUCCESS);
}

#ifdef AUTO_NO_SKIP
static void eventCallback(struct UsbFnEvent *event)
{
    (void)event;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent001, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff, nullptr, g_acmDevice);
    EXPECT_NE(HDF_SUCCESS, ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent002, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0x0, eventCallback, g_acmDevice);
    EXPECT_NE(HDF_SUCCESS, ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent003, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStartRecvInterfaceEvent(nullptr, 0xff, eventCallback, g_acmDevice);
    EXPECT_NE(HDF_SUCCESS, ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent004, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff, eventCallback, g_acmDevice);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent005, TestSize.Level1)
{
    int32_t ret;
    int32_t count = 0;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    for (count = 0; count < TEST_TIMES; count++) {
        ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
        EXPECT_TRUE(HDF_SUCCESS == ret);
        ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff, eventCallback, g_acmDevice);
        EXPECT_TRUE(HDF_SUCCESS == ret);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent006, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff, eventCallback, g_acmDevice);
    EXPECT_NE(HDF_SUCCESS, ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStartRecvEvent007, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->dataIface.fn, 0xff, eventCallback, g_acmDevice);
    EXPECT_NE(HDF_SUCCESS, ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStopRecvEvent001, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    ret = UsbFnStopRecvInterfaceEvent(nullptr);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStopRecvEvent002, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
    EXPECT_TRUE(HDF_SUCCESS == ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStopRecvEvent003, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfStopRecvEvent004, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->dataIface.fn);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCloseInterface001, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    ret = UsbFnCloseInterface(g_acmDevice->ctrlIface.handle);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    g_acmDevice->ctrlIface.handle = NULL;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCloseInterface002, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    ret = UsbFnCloseInterface(g_acmDevice->dataIface.handle);
    EXPECT_TRUE(HDF_SUCCESS == ret);
    g_acmDevice->dataIface.handle = NULL;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCloseInterface003, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    ret = UsbFnCloseInterface(g_acmDevice->ctrlIface.handle);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCloseInterface004, TestSize.Level1)
{
    int32_t ret;
    EXPECT_NE(nullptr, g_acmDevice);
    ret = UsbFnCloseInterface(g_acmDevice->dataIface.handle);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}


HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfOpenInterface001, TestSize.Level1)
{
    UsbFnInterfaceHandle handle;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    handle = UsbFnOpenInterface(g_acmDevice->ctrlIface.fn);
    EXPECT_TRUE(nullptr != handle);
    g_acmDevice->ctrlIface.handle = handle;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfOpenInterface002, TestSize.Level1)
{
    UsbFnInterfaceHandle handle;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    handle = UsbFnOpenInterface(g_acmDevice->dataIface.fn);
    EXPECT_TRUE(nullptr != handle);
    g_acmDevice->dataIface.handle = handle;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfOpenInterface003, TestSize.Level1)
{
    UsbFnInterfaceHandle handle;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.fn);
    handle = UsbFnOpenInterface(g_acmDevice->ctrlIface.fn);
    EXPECT_TRUE(handle == nullptr);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfOpenInterface004, TestSize.Level1)
{
    UsbFnInterfaceHandle handle;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.fn);
    handle = UsbFnOpenInterface(g_acmDevice->dataIface.fn);
    EXPECT_TRUE(handle == nullptr);
}
#endif

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfOpenInterface005, TestSize.Level1)
{
    struct UsbFnInterface *fn = nullptr;
    UsbFnInterfaceHandle handle;
    handle = UsbFnOpenInterface(fn);
    EXPECT_TRUE(handle == nullptr);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRemoveDevice001, TestSize.Level1)
{
    int32_t ret;
    ret = UsbFnRemoveDevice(nullptr);
    EXPECT_TRUE(HDF_SUCCESS != ret);
}

#ifdef AUTO_NO_SKIP
HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRemoveDevice002, TestSize.Level1)
{
    int32_t ret;

    EXPECT_NE(nullptr, g_acmDevice);
    ReleaseAcmDevice(g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    ret = UsbFnRemoveDevice(g_acmDevice->fnDev);
    EXPECT_EQ(HDF_SUCCESS, ret);
    OsalMemFree(g_acmDevice);
}
#endif
}
