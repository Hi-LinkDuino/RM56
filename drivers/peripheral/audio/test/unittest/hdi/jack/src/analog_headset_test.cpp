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

/**
 * @file analog_headset_test.cpp
 * 
 * @brief The test equipment is an analog headset (headset_jack_index);
 * First test the API of the input module called;
 * Then test the headset plug-in recognition function, including four
 * comprehensive tests: normal multiple plug-in, fast, slow, headset
 * and headphone switching.
 * 
 * @since 1.0
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <string>
#include <unistd.h>
#include "analog_headset_ev.h"
#include "hdf_log.h"
#include "input_manager.h"
#include "osal_time.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER
namespace {
    const int INIT_DEFAULT_VALUE = 255;
    const int NAME_MAX_LEN = 10;
    const int KEEP_ALIVE_TIME_MS = 15000;
}

using namespace testing::ext;
static IInputInterface *CommonGetInputInterface(void)
{
    int32_t ret;
    static IInputInterface *inputIf = NULL;

    if (inputIf != NULL) {
        return inputIf;
    }
    ret = GetInputInterface(&inputIf);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get input hdi failed, ret %d", __func__, ret);
    }

    return inputIf;
}

class HdfAnalogHeadsetTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfAnalogHeadsetTest::SetUpTestCase()
{
    CommonGetInputInterface();
}

void HdfAnalogHeadsetTest::TearDownTestCase()
{
    IInputInterface *inputHdi = CommonGetInputInterface();
    ReleaseInputInterface(inputHdi);
}

void HdfAnalogHeadsetTest::SetUp()
{
}

void HdfAnalogHeadsetTest::TearDown()
{
}

/**
 * API test, start from here.
 */

/**
  * @tc.name: OpenInputDev001
  * @tc.desc: Open input device test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, OpenInputDev001, TestSize.Level1)
{
    int32_t ret;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputManager is NULL.", __func__);
        return;
    }

    ret = inputHdi->iInputManager->OpenInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: [OpenInputDevice] failed, ret %d.", __func__, ret);
        return;
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice001
  * @tc.desc: Get input device info test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, GetInputDevice001, TestSize.Level1)
{
    int32_t ret;
    DeviceInfo *dev = NULL;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputManager is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputManager->GetInputDevice(HEADSET_JACK_INDEX, &dev);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get device1 failed, ret %d", __func__, ret);
    } else {
        if (dev != NULL) {
            HDF_LOGI("%s: devindex = %u, devType = %u", __func__, dev->devIndex, dev->devType);
            HDF_LOGI("%s: chipInfo = %s, vendorName = %s, chipName = %s",
                __func__, dev->chipInfo, dev->vendorName, dev->chipName);
        }
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetVendorName001
  * @tc.desc: Get device vendor name test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, GetVendorName001, TestSize.Level1)
{
    int32_t ret;
    char vendorName[NAME_MAX_LEN] = { 0 };
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputController == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputController is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputController->GetVendorName(HEADSET_JACK_INDEX, vendorName, NAME_MAX_LEN);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get device1's vendor name failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: device1's vendor name is %s:", __func__, vendorName);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetChipName001
  * @tc.desc: Get device chip name test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, GetChipName001, TestSize.Level1)
{
    int32_t ret;
    char chipName[NAME_MAX_LEN] = { 0 };
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputController == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputController is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputController->GetChipName(HEADSET_JACK_INDEX, chipName, NAME_MAX_LEN);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get device1's chip name failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: device1's chip name is %s", __func__, chipName);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetDeviceType001
  * @tc.desc: Get input device type test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, GetDeviceType001, TestSize.Level1)
{
    int32_t ret;
    uint32_t devType = INIT_DEFAULT_VALUE;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputController == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputController is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputController->GetDeviceType(HEADSET_JACK_INDEX, &devType);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get device1's type failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: device1's type is %u", __func__, devType);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetChipInfo001
  * @tc.desc: Get input device chip info test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, GetChipInfo001, TestSize.Level1)
{
    int32_t ret;
    char chipInfo[CHIP_INFO_LEN] = { 0 };
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputController == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputController is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputController->GetChipInfo(HEADSET_JACK_INDEX, chipInfo, CHIP_INFO_LEN);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get device1's chip info failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: device1's chip info is %s", __func__, chipInfo);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: CloseInputDevice001
  * @tc.desc: Close input device test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, CloseInputDevice001, TestSize.Level1)
{
    int32_t ret;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi or inputHdi->iInputManager is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputManager->CloseInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: close device1 failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
 * Callback API test begin here.
 */
static void ReportHandleInputEvent(const EventPackage *pkg)
{
    bool in;
    bool invalidBtn = false;
    static struct JackNotifyInfo headsetHookBtn;

    if (pkg == nullptr) {
        HDF_LOGE("%s: pkg is nullptr.", __func__);
        return;
    }

    if (pkg->type != EV_KEY) {
        HDF_LOGE("%s: [pkg->type != EV_KEY] is true.", __func__);
        return;
    }
    switch (pkg->code) {
        case KEY_JACK_HEADSET:
            invalidBtn = true;
            break;
        case KEY_JACK_HEADPHONE:
            invalidBtn = true;
            break;
        default:
            break;
    }
    if (invalidBtn) {
        in = (!!pkg->value) ? true : false;
        headsetHookBtn.jackStatus = in;
        headsetHookBtn.jackType = pkg->code;
        if (in) {
            printf("%s plugs in.\n", (pkg->code == KEY_JACK_HEADSET) ? "headset" : "headphone");
        } else {
            printf("jack plugs out.\n");
        }
    }

    return;
}

static void ReportEventPkgCallback(const EventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    int32_t i;

    if (pkgs == NULL) {
        HDF_LOGE("%s: pkgs is NULL.", __func__);
        return;
    }
    for (i = 0; i < count; i++) {
        if (pkgs[i]->type == EV_SYN) {
            break;
        }
        printf("%s: pkgs[%d] = 0x%x, 0x%x, %d.\t", __func__, i, pkgs[i]->type, pkgs[i]->code, pkgs[i]->value);
        ReportHandleInputEvent(pkgs[i]);
    }
}

static void ReportHotPlugEventPkgCallback(const HotPlugEvent *msg)
{
    if (msg == NULL) {
        HDF_LOGE("%s: msg is NULL.", __func__);
        return;
    }
    printf("%s: status =%d devId=%d type =%d", __func__, msg->status, msg->devIndex, msg->devType);
}

/**
  * @tc.name: RegisterReportCallback001
  * @tc.desc:  Registers a callback for reporting subscribed data of specified input devices to test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, RegisterReportCallback001, TestSize.Level1)
{
    int32_t ret;
    static InputEventCb eventCb;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputReporter == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi, iInputReporter or iInputManager is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputManager->OpenInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: open device[%d] failed, ret %d", __func__, HEADSET_JACK_INDEX, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);

    eventCb.EventPkgCallback = ReportEventPkgCallback;
    ret = inputHdi->iInputReporter->RegisterReportCallback(HEADSET_JACK_INDEX, &eventCb);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: [RegisterReportCallback] failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: wait 15s for testing, pls touch the panel now", __func__);
        HDF_LOGI("%s: The event data is as following:", __func__);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: UnregisterReportCallback001
  * @tc.desc: Unregisters the callback for reporting subscribed data of specified input devices to test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, UnregisterReportCallback001, TestSize.Level1)
{
    int32_t ret;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputReporter == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi, iInputReporter or iInputManager is NULL.", __func__);
        return;
    }

    ret = inputHdi->iInputReporter->UnregisterReportCallback(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: unregister callback failed for device1, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);

    ret = inputHdi->iInputManager->CloseInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: close device1 failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: Close the device1 successfully after all test", __func__);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RegisterHotPlugCallback001
  * @tc.desc: Registers a hot plug callback to the HDIs for input devices to test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, RegisterHotPlugCallback001, TestSize.Level1)
{
    int32_t ret;
    static InputHostCb hotPlugCb;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputReporter == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi, iInputReporter or iInputManager is NULL.", __func__);
        return;
    }
    ret = inputHdi->iInputManager->OpenInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: open device[%d] failed, ret %d", __func__, HEADSET_JACK_INDEX, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);

    hotPlugCb.HotPlugCallback = ReportHotPlugEventPkgCallback;
    ret = inputHdi->iInputReporter->RegisterHotPlugCallback(&hotPlugCb);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: [RegisterHotPlugCallback] failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: wait 15s for testing, pls touch the panel now", __func__);
        HDF_LOGI("%s: The event data is as following:", __func__);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: UnregisterHotPlugCallback001
  * @tc.desc: Unregisters the hot plug callback of input devices to test.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, UnregisterHotPlugCallback001, TestSize.Level1)
{
    int32_t ret;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputReporter == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi, iInputReporter or iInputManager is NULL.", __func__);
        return;
    }

    ret = inputHdi->iInputReporter->UnregisterHotPlugCallback();
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: unregister callback failed for device1, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);

    ret = inputHdi->iInputManager->CloseInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: close device1 failed, ret %d", __func__, ret);
    } else {
        HDF_LOGI("%s: Close the device1 successfully after all test", __func__);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
 * Comprehensive test, start from here.
 *
 */
static int32_t AnalogHeadsetTestInit()
{
    int32_t ret;
    static InputEventCb eventCb;
    IInputInterface *inputHdi = CommonGetInputInterface();

    if ((inputHdi == NULL) || (inputHdi->iInputReporter == NULL) || (inputHdi->iInputManager == NULL)) {
        HDF_LOGE("%s: inputHdi, iInputReporter or iInputManager is NULL.", __func__);
        return INPUT_FAILURE;
    }

    ret = inputHdi->iInputManager->OpenInputDevice(HEADSET_JACK_INDEX);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: open device[%d] failed, ret %d", __func__, HEADSET_JACK_INDEX, ret);
        ReleaseInputInterface(inputHdi);
        return INPUT_FAILURE;
    }

    eventCb.EventPkgCallback = ReportEventPkgCallback;
    ret = inputHdi->iInputReporter->RegisterReportCallback(HEADSET_JACK_INDEX, &eventCb);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: [RegisterReportCallback] failed, ret %d", __func__, ret);
        ReleaseInputInterface(inputHdi);
        return INPUT_FAILURE;
    }

    return INPUT_SUCCESS;
}

/**
  * @tc.name: AnalogHeadsetFunctionTest001
  * @tc.desc: Comprehensive test entrance of analog earphone.
  * @tc.type: FUNC
  * @tc.require: AR000GGUU3
  */
HWTEST_F(HdfAnalogHeadsetTest, AnalogHeadsetFunctionTest001, TestSize.Level1)
{
    int32_t ret;

    ret = AnalogHeadsetTestInit();

    printf("TEST1: Multiple plugging, wait 15s for testing, please hotplug now\n");
    printf("The event data is as following:\n");
    OsalMSleep(KEEP_ALIVE_TIME_MS);

    printf("\nTEST2: Switch HEADSET and HEADPHONE, wait 15s for testing, please hotplug now\n");
    printf("The event data is as following:\n");
    OsalMSleep(KEEP_ALIVE_TIME_MS);

    HDF_LOGI("%s: finished.", __func__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}
