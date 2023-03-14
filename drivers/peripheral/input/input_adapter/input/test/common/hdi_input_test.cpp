/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <string>
#include <unistd.h>
#include "hdf_log.h"
#include "input_manager.h"
#include "osal_time.h"

using namespace testing::ext;
static IInputInterface *g_inputInterface;
static InputEventCb g_callback;
static InputHostCb g_hotplugCb;
static const int32_t KEEP_ALIVE_TIME_MS = 100000;
static const int32_t TOUCH_INDEX = 1;
static const int32_t INVALID_INDEX = 15;
static const int32_t INVALID_INDEX1 = -1;
static const int32_t MAX_DEVICES = 32;
class HdiInputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdiInputTest::SetUpTestCase()
{
    int32_t ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        printf("%s: get input hdi failed, ret %d\n", __func__, ret);
    }
}

void HdiInputTest::TearDownTestCase()
{
    if (g_inputInterface != NULL) {
        OsalMemFree(g_inputInterface);
        g_inputInterface = NULL;
    }
}

void HdiInputTest::SetUp()
{
}

void HdiInputTest::TearDown()
{
}

#define INPUT_CHECK_NULL_POINTER(pointer, ret) do { \
    if ((pointer) == NULL) { \
        printf("%s: null pointer", __func__); \
        ASSERT_EQ ((ret), INPUT_SUCCESS); \
    } \
} while (0)

static void ReportEventPkgCallback(const EventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    if (pkgs == NULL) {
        printf("pkgs is null!!!!\n");
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        printf("device action Index:%d  devIndex: %d type:%d code:%d value %d\n",
                i, devIndex, pkgs[i]->type, pkgs[i]->code, pkgs[i]->value);
    }
}

static void ReportHotPlugEventPkgCallback(const HotPlugEvent *msg)
{
    if (msg == NULL) {
        printf("pkgs is null!!!!\n");
        return;
    }
    printf("device hotplug action devIndex:%d devType:%d status:%d\n",
        msg->devIndex, msg->devType, msg->status);
    if (msg->status == INPUT_DEVICE_STATIS_OPENED) {
        EXPECT_EQ(g_inputInterface->iInputManager->OpenInputDevice(msg->devIndex), INPUT_SUCCESS);
    } else if (msg->status == INPUT_DEVICE_STATUS_CLOSED) {
        EXPECT_EQ(g_inputInterface->iInputManager->CloseInputDevice(msg->devIndex), INPUT_SUCCESS);
    } else {
        // do nothing
    }
}

HWTEST_F(HdiInputTest, ScanInputDevice, TestSize.Level1)
{
    DevDesc sta[MAX_DEVICES] = {0};
    printf("%s: [Input] ScanInputDevice enter %d\n", __func__, __LINE__);
    int32_t ret;
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    printf("%s: [Input] ScanInputDevice enter %d\n", __func__, __LINE__);
    ret  = g_inputInterface->iInputManager->ScanInputDevice(sta, sizeof(sta)/sizeof(DevDesc));
    printf("%s: [Input] ScanInputDevice enter %d\n", __func__, __LINE__);
    if (!ret) {
        printf("ScanInputDevice result %s:%d, %d, %d, %d\n",
               __func__, sta[0].devType, sta[0].devIndex, sta[1].devType, sta[1].devIndex);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: OpenInputDev001
  * @tc.desc: open input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867R, AR000F8QNL
  */
HWTEST_F(HdiInputTest, OpenInputDev001, TestSize.Level1)
{
    printf("%s: [Input] OpenInputDev001 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    printf("%s: [Input] OpenInputDev001 enter %d\n", __func__, __LINE__);
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    printf("%s: [Input] OpenInputDev001 enter %d\n", __func__, __LINE__);
    if (ret) {
        printf("%s: open device1 failed, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] OpenInputDev001 enter %d\n", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: OpenInputDevice002
  * @tc.desc: open input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867R
  */
HWTEST_F(HdiInputTest, OpenInputDevice002, TestSize.Level1)
{
    printf("%s: [Input] OpenInputDev002 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "15" is used for testing nonexistent device node */
    printf("%s: [Input] OpenInputDev002 enter %d\n", __func__, __LINE__);
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(INVALID_INDEX);
    if (ret != HDF_SUCCESS) {
        printf("%s: device5 dose not exist, can't open it, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] OpenInputDev002 enter %d\n", __func__, __LINE__);
    EXPECT_NE(ret, INPUT_SUCCESS);
}


/**
  * @tc.name: OpenInputDevice003
  * @tc.desc: open input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867R
  */
HWTEST_F(HdiInputTest, OpenInputDevice003, TestSize.Level1)
{
    printf("%s: [Input] OpenInputDev003 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "0" is used for testing nonexistent device node */
    printf("%s: [Input] OpenInputDev002 enter %d\n", __func__, __LINE__);
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(INVALID_INDEX1);
    if (ret != HDF_SUCCESS) {
        printf("%s: device5 dose not exist, can't open it, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] OpenInputDev002 enter %d\n", __func__, __LINE__);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: CloseInputDevice001
  * @tc.desc: close input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867T, AR000F8QNL
  */
HWTEST_F(HdiInputTest, CloseInputDevice001, TestSize.Level1)
{
    printf("%s: [Input] CloseInputDev001 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    printf("%s: [Input] CloseInputDev001 enter %d\n", __func__, __LINE__);
    if (ret) {
        printf("%s: close device1 failed, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] CloseInputDev001 enter %d\n", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}
/**
  * @tc.name: CloseInputDevice002
  * @tc.desc: close input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867T
  */
HWTEST_F(HdiInputTest, CloseInputDevice002, TestSize.Level1)
{
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "15" is used for testing nonexistent device node */
    int32_t ret = g_inputInterface->iInputManager->CloseInputDevice(INVALID_INDEX);
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    if (ret == INPUT_FAILURE) {
        printf("%s: device5 doesn't exist, can't close it, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: CloseInputDevice003
  * @tc.desc: close input device test
  * @tc.type: FUNC
  * @tc.require: AR000F867T
  */
HWTEST_F(HdiInputTest, CloseInputDevice003, TestSize.Level1)
{
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "15" is used for testing nonexistent device node */
    int32_t ret = g_inputInterface->iInputManager->CloseInputDevice(INVALID_INDEX1);
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    if (ret == INPUT_FAILURE) {
        printf("%s: device5 doesn't exist, can't close it, ret %d\n", __func__, ret);
    }
    printf("%s: [Input] CloseInputDev002 enter %d\n", __func__, __LINE__);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice001
  * @tc.desc: get input device info test
  * @tc.type: FUNC
  * @tc.require: AR000F867S
  */
HWTEST_F(HdiInputTest, GetInputDevice001, TestSize.Level1)
{
    printf("%s: [Input] GetInputDevice001 enter %d\n", __func__, __LINE__);
    DeviceInfo *dev = new DeviceInfo();
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, &dev);
    printf("%s: [Input] GetInputDevice001 enter %d\n", __func__, __LINE__);
    if (ret) {
        printf("%s: get device1 failed, ret %d\n", __func__, ret);
    }
    printf("GetInputDevice001 %s: devindex = %d, devType = %d\n", __func__, dev->devIndex, dev->devType);
    printf("GetInputDevice001: chipInfo = %s, vendorName = %s, chipName = %s devName = %s\n",
           dev->chipInfo, dev->vendorName, dev->chipName, dev->attrSet.devName);
    printf("GetInputDevice001: busType = %u, vendor = %u, product = %u version = %u\n",
           dev->attrSet.id.busType, dev->attrSet.id.vendor, dev->attrSet.id.product, dev->attrSet.id.version);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice002
  * @tc.desc: get input device info test
  * @tc.type: FUNC
  * @tc.require: AR000F867S
  */
HWTEST_F(HdiInputTest, GetInputDevice002, TestSize.Level1)
{
    printf("%s: [Input] GetInputDevice002 enter %d\n", __func__, __LINE__);
    DeviceInfo *dev = new DeviceInfo();
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->GetInputDevice(INVALID_INDEX1, &dev);
    printf("%s: [Input] GetInputDevice001 enter %d\n", __func__, __LINE__);
    if (ret) {
        printf("%s: get device1 failed, ret %d\n", __func__, ret);
    }
    printf("GetInputDevice001 %s: devindex = %d, devType = %d\n", __func__, dev->devIndex, dev->devType);
    printf("GetInputDevice001: chipInfo = %s, vendorName = %s, chipName = %s devName = %s\n",
           dev->chipInfo, dev->vendorName, dev->chipName, dev->attrSet.devName);
    printf("GetInputDevice001: busType = %u, vendor = %u, product = %u version = %u\n",
           dev->attrSet.id.busType, dev->attrSet.id.vendor, dev->attrSet.id.product, dev->attrSet.id.version);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice003
  * @tc.desc: get input device info test
  * @tc.type: FUNC
  * @tc.require: AR000F867S
  */
HWTEST_F(HdiInputTest, GetInputDevice003, TestSize.Level1)
{
    printf("%s: [Input] GetInputDevice003 enter %d\n", __func__, __LINE__);
    DeviceInfo *dev = new DeviceInfo();
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->GetInputDevice(INVALID_INDEX, &dev);
    printf("%s: [Input] GetInputDevice001 enter %d\n", __func__, __LINE__);
    if (ret) {
        printf("%s: get device1 failed, ret %d\n", __func__, ret);
    }
    printf("GetInputDevice001 %s: devindex = %d, devType = %d\n", __func__, dev->devIndex, dev->devType);
    printf("GetInputDevice001: chipInfo = %s, vendorName = %s, chipName = %s devName = %s\n",
           dev->chipInfo, dev->vendorName, dev->chipName, dev->attrSet.devName);
    printf("GetInputDevice001: busType = %u, vendor = %u, product = %u version = %u\n",
           dev->attrSet.id.busType, dev->attrSet.id.vendor, dev->attrSet.id.product, dev->attrSet.id.version);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDeviceList001
  * @tc.desc: get input device list info test
  * @tc.type: FUNC
  * @tc.require: AR000F8680
  */
HWTEST_F(HdiInputTest, GetInputDeviceList001, TestSize.Level1)
{
    printf("%s: [Input] GetInputDeviceList001 enter", __func__);
    int32_t ret;
    uint32_t num = 0;
    DeviceInfo *dev = new DeviceInfo[MAX_INPUT_DEV_NUM] {};
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputManager->GetInputDeviceList(&num, &dev, MAX_INPUT_DEV_NUM);
    if (ret) {
        printf("%s: get device list failed, ret %d\n", __func__, ret);
    }
    /* num <= MAX_INPUT_DEV_NUM return true */
    ASSERT_LE(num, MAX_INPUT_DEV_NUM);
    for (uint32_t i = 0; i < num; i++) {
        printf("%s: num = %u, device[%d]'s info is:", __func__, num, i);
        printf("%s: index = %u, devType = %u", __func__, (dev + i)->devIndex, (dev + i)->devType);
        printf("%s: chipInfo = %s, vendorName = %s, chipName = %s devName = %s\n",
               __func__, (dev + i)->chipInfo, (dev + i)->vendorName, (dev + i)->chipName, (dev + i)->attrSet.devName);
        printf("%s: busType = %u, vendor = %u, product = %u version = %u\n",
               __func__, (dev + i)->attrSet.id.busType, (dev + i)->attrSet.id.vendor,
               (dev + i)->attrSet.id.product, (dev + i)->attrSet.id.version);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RegisterCallbackAndReportData001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  * @tc.require: AR000F8682, AR000F8QNL
  */
HWTEST_F(HdiInputTest, RegisterCallbackAndReportData001, TestSize.Level1)
{
    printf("%s: [Input] RegisterCallbackAndReportData001 enter", __func__);
    int32_t ret;
    g_callback.EventPkgCallback = ReportEventPkgCallback;
    g_hotplugCb.HotPlugCallback = ReportHotPlugEventPkgCallback;
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(TOUCH_INDEX, &g_callback);
    if (ret) {
        printf("%s: register callback failed for device 1, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(2, &g_callback);
    if (ret) {
        printf("%s: register callback failed for device 2, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(3, &g_callback);
    if (ret) {
        printf("%s: register callback failed for device 3, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret  = g_inputInterface->iInputReporter->RegisterHotPlugCallback(&g_hotplugCb);
    if (ret) {
        printf("%s: register callback failed for device manager, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->OpenInputDevice(2);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->OpenInputDevice(3);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    printf("%s: wait 10s for testing, pls touch the panel now\n", __func__);
    printf("%s: The event data is as following:\n", __func__);
    OsalMSleep(KEEP_ALIVE_TIME_MS);
}

/**
  * @tc.name: UnregisterReportCallback001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  * @tc.require: SR000F867Q
  */
HWTEST_F(HdiInputTest, UnregisterReportCallback001, TestSize.Level1)
{
    printf("%s: [Input] UnregisterReportCallback001 enter", __func__);
    int32_t ret;
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    ret  = g_inputInterface->iInputReporter->UnregisterHotPlugCallback();
    if (ret) {
        printf("%s: register callback failed for device manager, ret %d\n", __func__, ret);
    }
    ret  = g_inputInterface->iInputReporter->UnregisterReportCallback(TOUCH_INDEX);
    if (ret) {
        printf("%s: unregister callback failed for device1, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    if (ret) {
        printf("%s: close device1 failed, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    printf("%s: Close the device1 successfully after all test\n", __func__);
}

