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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <securec.h>
#include "osal_time.h"
#include "hdf_log.h"
#include "input_manager.h"
#include "hdi_input_test.h"

using namespace testing::ext;

IInputInterface *g_inputInterface;
InputEventCb g_callback;
InputHostCb g_hotplugCb;
bool g_HasDev = false;

static void ReportHotPlugEventPkgCallback(const HotPlugEvent *msg);
static void ReportEventPkgCallback(const EventPackage **pkgs, uint32_t count, uint32_t devIndex);
static void CloseOnlineDev(DevDesc *sta, int32_t len);
static void OpenOnlineDev(DevDesc *sta, int32_t len);

class HdiInputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdiInputTest::SetUpTestCase()
{
    int32_t ret;
    DevDesc sta[MAX_DEVICES];
    ret = memset_s(sta, MAX_DEVICES * sizeof(DevDesc), 0, MAX_DEVICES * sizeof(DevDesc));
    if (ret != 0) {
        HDF_LOGE("memset failed.\n");
        return;
    }
    ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get input hdi failed, ret %d \n", __func__, ret);
    }

    g_callback.EventPkgCallback = ReportEventPkgCallback;
    g_hotplugCb.HotPlugCallback = ReportHotPlugEventPkgCallback;
    ret = g_inputInterface->iInputManager->ScanInputDevice(sta, MAX_DEVICES);
    if (ret) {
        HDF_LOGE("%s: scan device failed, ret %d \n", __func__, ret);
    }
    for (int32_t i = 0; i < MAX_DEVICES; i++) {
        if (sta[i].devIndex == 0) {
            break;
        }
        printf("%s: scan deviceIndex:%d,devType:%d. \n", __func__, sta[i].devIndex, sta[i].devType);
        g_HasDev = true;
    }
}

void HdiInputTest::TearDownTestCase()
{
    ReleaseInputInterface(g_inputInterface);
}

void HdiInputTest::SetUp()
{
}

void HdiInputTest::TearDown()
{
}

static void ReportEventPkgCallback(const EventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    if (pkgs == nullptr) {
        return;
    }
    for (int32_t i = 0; i < count; i++) {
        printf("%s: pkgs[%d] = 0x%x, 0x%x, %d\n", __func__, i, pkgs[i]->type, pkgs[i]->code, pkgs[i]->value);
        EXPECT_GE(pkgs[i]->type, 0);
        EXPECT_GE(pkgs[i]->code, 0);
        EXPECT_GE(pkgs[i]->value, 0);
    }
}

static void ReportHotPlugEventPkgCallback(const HotPlugEvent *msg)
{
    int32_t ret;
    if (msg == nullptr) {
        return;
    }
    HDF_LOGI("%s: status =%d devId=%d type =%d \n", __func__, msg->status, msg->devIndex, msg->devType);
    EXPECT_GE(msg->status, 0);
    EXPECT_GE(msg->devIndex, 0);
    EXPECT_GE(msg->devType, 0);

    if (msg->status == 0) {
        ret = g_inputInterface->iInputManager->OpenInputDevice(msg->devIndex);
        if (ret) {
            HDF_LOGE("%s: open device[%u] failed, ret %d \n", __func__, msg->devIndex, ret);
        }

        ret  = g_inputInterface->iInputReporter->RegisterReportCallback(msg->devIndex, &g_callback);
        if (ret) {
            HDF_LOGE("%s: register callback failed for device[%d], ret %d \n", __func__, msg->devIndex, ret);
        }
    } else {
        ret = g_inputInterface->iInputReporter->UnregisterReportCallback(msg->devIndex);
        if (ret) {
            HDF_LOGE("%s: unregister callback failed, ret %d \n", __func__, ret);
        }

        ret = g_inputInterface->iInputManager->CloseInputDevice(msg->devIndex);
        if (ret) {
            HDF_LOGE("%s: close device failed, ret %d \n", __func__, ret);
        }
    }
}

static void OpenOnlineDev(DevDesc *sta, int32_t len)
{
    int32_t ret = g_inputInterface->iInputManager->ScanInputDevice(sta, len);
    if (ret) {
        HDF_LOGE("%s: scan device failed, ret %d \n", __func__, ret);
    }
    ASSERT_EQ(ret, INPUT_SUCCESS);

    for (int32_t i = 0; i < len; i++) {
        if (sta[i].devIndex == 0) {
            break;
        }
        ret = g_inputInterface->iInputManager->OpenInputDevice(sta[i].devIndex);
        if (ret) {
            HDF_LOGE("%s: open device[%d] failed, ret %d \n", __func__, sta[i].devIndex, ret);
        }
        ASSERT_EQ(ret, INPUT_SUCCESS);

        ret  = g_inputInterface->iInputReporter->RegisterReportCallback(sta[i].devIndex, &g_callback);
        if (ret) {
            HDF_LOGE("%s: register callback failed for device[%d], ret %d \n", __func__, sta[i].devIndex, ret);
        }
        ASSERT_EQ(ret, INPUT_SUCCESS);
    }
}

static void CloseOnlineDev(DevDesc *sta, int32_t len)
{
    int32_t ret = g_inputInterface->iInputManager->ScanInputDevice(sta, len);
    if (ret) {
        HDF_LOGE("%s: scan device failed, ret %d \n", __func__, ret);
    }
    ASSERT_EQ(ret, INPUT_SUCCESS);

    for (int32_t i = 0; i < len; i++) {
        if (sta[i].devIndex == 0) {
            break;
        }
        ret = g_inputInterface->iInputReporter->UnregisterReportCallback(sta[i].devIndex);
        if (ret) {
            HDF_LOGE("%s: register callback failed for device[%d], ret %d \n", __func__, sta[i].devIndex, ret);
        }
        ASSERT_EQ(ret, INPUT_SUCCESS);

        ret = g_inputInterface->iInputManager->CloseInputDevice(sta[i].devIndex);
        if (ret) {
            HDF_LOGE("%s: close device[%d] failed, ret %d \n", __func__, sta[i].devIndex, ret);
        }
        ASSERT_EQ(ret, INPUT_SUCCESS);
    }
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0001
  * @tc.name: open input device test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0001, Function | MediumTest | Level1)
{
    DevDesc sta[MAX_DEVICES];

    HDF_LOGI("%s: [Input] RegisterCallbackAndReportData001 enter \n", __func__);
    int32_t ret;

    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret  = g_inputInterface->iInputManager->ScanInputDevice(sta, sizeof(sta)/sizeof(DevDesc));
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0002
  * @tc.name: open input device test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0002, Function | MediumTest | Level3)
{
    HDF_LOGI("%s: [Input] HotPlugCallback Testcase enter\n", __func__);
    DevDesc sta[MAX_DEVICES];

    int32_t ret = memset_s(sta, sizeof(sta), 0, sizeof(sta));
    if (ret != 0) {
        HDF_LOGE("%s: memcpy failed, line %d\n", __func__, __LINE__);
    }

    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret = g_inputInterface->iInputReporter->RegisterHotPlugCallback(&g_hotplugCb);
    if (ret) {
        HDF_LOGE("%s: register hotplug callback failed for device manager, ret %d\n", __func__, ret);
    }
    ASSERT_EQ(ret, INPUT_SUCCESS);

    OpenOnlineDev(sta, MAX_DEVICES);

    printf("%s: wait 5s for testing, pls hotplug now\n", __func__);
    printf("%s: The event data is as following:\n", __func__);
    OsalMSleep(KEEP_ALIVE_TIME_MS);

    ret = memset_s(sta, sizeof(sta), 0, sizeof(sta));
    if (ret != 0) {
        HDF_LOGE("%s: memcpy failed, line %d\n", __func__, __LINE__);
    }

    CloseOnlineDev(sta, MAX_DEVICES);

    ret = g_inputInterface->iInputReporter->UnregisterHotPlugCallback();
    if (ret) {
        HDF_LOGE("%s: unregister hotplug callback failed for device manager, ret %d\n", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0010
  * @tc.name: open input device test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0010, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    ASSERT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    ASSERT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0020
  * @tc.name: open input device error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0020, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "0" is used for testing nonexistent device node */
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(0);
    EXPECT_NE(ret, INPUT_SUCCESS);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputManager->OpenInputDevice(INVALID_INDEX);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0040
  * @tc.name: close input device test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0040, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret = 0;
    g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0050
  * @tc.name: close input device error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0050, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret = 0;
    ret = g_inputInterface->iInputManager->CloseInputDevice(0);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->CloseInputDevice(INVALID_INDEX);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0070
  * @tc.name: get input device id info test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0070, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret = 0;
    DeviceInfo *dev = nullptr;
    INPUT_CHECK_NULL_POINTER(g_inputInterface, INPUT_NULL_PTR);
    INPUT_CHECK_NULL_POINTER(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: open device1 failed, ret %d\n", __func__, ret);
    }
    ASSERT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, &dev);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    EXPECT_EQ((uint32_t)TOUCH_INDEX, dev->devIndex);
    HDF_LOGI("devindex = %u, devType = %u\n", dev->devIndex, dev->devType);
    HDF_LOGI("chipInfo = %s, VendorName = %s,chipName = %s\n", dev->chipInfo, dev->vendorName, dev->chipName);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0080
  * @tc.name: get input device id error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0080, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret = 0;
    DeviceInfo *dev = nullptr;

    ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, nullptr);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDevice(0, &dev);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDevice(INVALID_INDEX, &dev);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDevice(MAX_INPUT_DEV_NUM, &dev);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0100
  * @tc.name: get input device list info test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0100, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t num = 0;
    DeviceInfo *dev[MAX_INPUT_DEV_NUM] = {0};

    ret = g_inputInterface->iInputManager->GetInputDeviceList(&num, dev, MAX_INPUT_DEV_NUM);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ASSERT_LE(num, (uint32_t)MAX_INPUT_DEV_NUM);
    for (uint32_t i = 0; i < num; i++) {
        HDF_LOGI("num = %u,device[%d]'s info is :\n", num, i);
        HDF_LOGI("index = %u, devType = %u\n", dev[i]->devIndex, dev[i]->devType);
        HDF_LOGI("chipInfo = %s, VendorName = %s,chipName = %s\n", dev[i]->chipInfo, dev[i]->vendorName,
            dev[i]->chipName);
        EXPECT_LE(0, dev[i]->devType);
    }
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0110
  * @tc.name: get input device list info error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0110, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t num = 0;
    DeviceInfo *dev[MAX_INPUT_DEV_NUM] = {0}; 

    ret = g_inputInterface->iInputManager->GetInputDeviceList(nullptr, dev, MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDeviceList(&num, nullptr, MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputManager->GetInputDeviceList(&num, dev, 0);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0130
  * @tc.name: get input device type test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0130, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t devType = INIT_DEFAULT_VALUE;

    ret = g_inputInterface->iInputController->GetDeviceType(TOUCH_INDEX, &devType);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    EXPECT_EQ(devType, INDEV_TYPE_TOUCH);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0140
  * @tc.name: get input device type error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0140, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t devType = INIT_DEFAULT_VALUE;

    ret = g_inputInterface->iInputController->GetDeviceType(TOUCH_INDEX, nullptr);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetDeviceType(0, &devType);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetDeviceType(MAX_INPUT_DEV_NUM, &devType);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0160
  * @tc.name: get input device chip info test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0160, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char chipInfo[CHIP_INFO_LEN] = {0};

    ret = g_inputInterface->iInputController->GetChipInfo(TOUCH_INDEX, chipInfo, CHIP_INFO_LEN);
    ASSERT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("device's chip info is %s\n", chipInfo);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0170
  * @tc.name: get input device chip info error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0170, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char chipInfo[CHIP_INFO_LEN] = {0};

    ret = g_inputInterface->iInputController->GetChipInfo(0, chipInfo, CHIP_INFO_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipInfo(MAX_INPUT_DEV_NUM, chipInfo, CHIP_INFO_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipInfo(TOUCH_INDEX, nullptr, CHIP_INFO_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipInfo(TOUCH_INDEX, chipInfo, CHIP_INFO_LEN - 1);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0190
  * @tc.name: get input device info test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0190, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char chipInfo[CHIP_INFO_LEN] = {0};
    DeviceInfo *dev =NULL;
    ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, &dev);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipInfo(TOUCH_INDEX, chipInfo, CHIP_INFO_LEN);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("device1's chip info is %s? chipInfo = %s\n", chipInfo, dev->chipInfo);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0200
  * @tc.name: set device power status test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0200, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t setStatus = INPUT_LOW_POWER;
    uint32_t getStatus = 0;

    ret = g_inputInterface->iInputController->SetPowerStatus(TOUCH_INDEX, setStatus);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetPowerStatus(TOUCH_INDEX, &getStatus);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ASSERT_EQ(setStatus, getStatus);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0210
  * @tc.name: set device poewr status error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0210, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t setStatus = INPUT_LOW_POWER;

    ret = g_inputInterface->iInputController->SetPowerStatus(0, setStatus);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->SetPowerStatus(MAX_INPUT_DEV_NUM, setStatus);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->SetPowerStatus(TOUCH_INDEX, INPUT_POWER_STATUS_UNKNOWN);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0230
  * @tc.name: get device poewr status test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0230, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t setStatus = INPUT_RESUME;
    uint32_t getStatus = 0;

    ret = g_inputInterface->iInputController->SetPowerStatus(TOUCH_INDEX, setStatus);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetPowerStatus(TOUCH_INDEX, &getStatus);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ASSERT_EQ(setStatus, getStatus);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0240
  * @tc.name: get device poewr status error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0240, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t getStatus = 0;

    ret = g_inputInterface->iInputController->GetPowerStatus(0, &getStatus);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetPowerStatus(TOUCH_INDEX, nullptr);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetPowerStatus(MAX_INPUT_DEV_NUM, &getStatus);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0260
  * @tc.name: get device vendor name test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0260, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char vendorName[VENDOR_NAME_LEN] = {0};

    ret = g_inputInterface->iInputController->GetVendorName(TOUCH_INDEX, vendorName, VENDOR_NAME_LEN);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("device1's vendor name is %s:\n", vendorName);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0270
  * @tc.name: get device vendor name error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0270, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char vendorName[VENDOR_NAME_LEN] = {0};

    ret = g_inputInterface->iInputController->GetVendorName(MAX_INPUT_DEV_NUM, vendorName, VENDOR_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetVendorName(0, vendorName, VENDOR_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetVendorName(TOUCH_INDEX, nullptr, VENDOR_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetVendorName(TOUCH_INDEX, vendorName, VENDOR_NAME_LEN - 1);
    EXPECT_NE(ret, INPUT_SUCCESS);
}


/**
  * @tc.number: SUB_DriverSystem_HdiInput_0290
  * @tc.name: get device chip name test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0290, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char chipName[CHIP_NAME_LEN] = {0};

    ret = g_inputInterface->iInputController->GetChipName(TOUCH_INDEX, chipName, CHIP_NAME_LEN);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("device1's vendor name is %s:\n", chipName);
}
/**
  * @tc.number: SUB_DriverSystem_HdiInput_0300
  * @tc.name: get device chip name error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0300, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    char chipName[CHIP_NAME_LEN] = {0};

    ret = g_inputInterface->iInputController->GetChipName(MAX_INPUT_DEV_NUM, chipName, CHIP_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipName(0, chipName, CHIP_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipName(TOUCH_INDEX, nullptr, CHIP_NAME_LEN);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->GetChipName(TOUCH_INDEX, chipName, CHIP_NAME_LEN - 1);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0320
  * @tc.name: set device gesture mode test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0320, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t gestureMode = 1;

    ret = g_inputInterface->iInputController->SetGestureMode(TOUCH_INDEX, gestureMode);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0330
  * @tc.name: set device gesture mode error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0330, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t gestureMode = 1;

    ret = g_inputInterface->iInputController->SetGestureMode(0, gestureMode);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->SetGestureMode(MAX_INPUT_DEV_NUM, gestureMode);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0350
  * @tc.name: Run Capacitance test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0350, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t testType = MMI_TEST;
    char result[MAX_INPUT_DEV_NUM] = {0};

    ret = g_inputInterface->iInputController->RunCapacitanceTest(TOUCH_INDEX, testType, result, MAX_INPUT_DEV_NUM);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0360
  * @tc.name: Run Capacitance error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0360, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    uint32_t testType = MMI_TEST;
    char result[MAX_INPUT_DEV_NUM] = {0};

    ret = g_inputInterface->iInputController->RunCapacitanceTest(0, testType, result, MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->RunCapacitanceTest(MAX_INPUT_DEV_NUM, testType, result,
        MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->RunCapacitanceTest(TOUCH_INDEX, TEST_TYPE_UNKNOWN, result,
        MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->RunCapacitanceTest(TOUCH_INDEX, testType, nullptr, MAX_INPUT_DEV_NUM);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0380
  * @tc.name: Run Extra Command test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0380, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    InputExtraCmd extraCmd = {0};
    extraCmd.cmdCode = "WakeUpMode";
    extraCmd.cmdValue = "Enable";

    ret = g_inputInterface->iInputController->RunExtraCommand(TOUCH_INDEX, &extraCmd);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0390
  * @tc.name: Run Extra Command error test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0390, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    InputExtraCmd extraCmd = {0};
    extraCmd.cmdCode = "WakeUpMode";
    extraCmd.cmdValue = "Enable";

    ret = g_inputInterface->iInputController->RunExtraCommand(MAX_INPUT_DEV_NUM, &extraCmd);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->RunExtraCommand(0, &extraCmd);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputController->RunExtraCommand(TOUCH_INDEX, nullptr);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0410
  * @tc.name: Register Report Callback test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0410, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    g_callback.EventPkgCallback = ReportEventPkgCallback;
    ret = g_inputInterface->iInputReporter->RegisterReportCallback(0, &g_callback);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputReporter->RegisterReportCallback(MAX_INPUT_DEV_NUM, &g_callback);
    EXPECT_NE(ret, INPUT_SUCCESS);
    ret = g_inputInterface->iInputReporter->RegisterReportCallback(TOUCH_INDEX, nullptr);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.number: SUB_DriverSystem_HdiInput_0420
  * @tc.name: Register Report Callback test
  * @tc.desc: [C- SOFTWARE -0010]
  * @tc.size: Medium
  * @tc.level: level 0
  */
HWTEST_F(HdiInputTest, SUB_DriverSystem_HdiInput_0420, Function | MediumTest | Level0)
{
    ASSERT_EQ(g_HasDev, true);
    int32_t ret;
    g_callback.EventPkgCallback = ReportEventPkgCallback;

    ret = g_inputInterface->iInputReporter->RegisterReportCallback(TOUCH_INDEX, &g_callback);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    printf("wait 5 for testing, pls  touch the panel now\n");
    printf("the event data is as following:\n");
    OsalMSleep(KEEP_ALIVE_TIME_MS);
    ret = g_inputInterface->iInputReporter->UnregisterReportCallback(TOUCH_INDEX);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}