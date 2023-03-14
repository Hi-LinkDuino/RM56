/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "utest_input_manager_impl.h"
#include <osal_mem.h>
#include "input_proxy.h"
#include "input_host_callback.h"

const int32_t INIT_DEFAULT_VALUE = (255);
const int32_t GET_EVENT_TRY_NUM = (20);
const int32_t PREPARE_TIME_MS = (5000);
const int32_t ONE_PERIOD_TIME_MS = (500);
const int32_t KEEP_ALIVE_TIME_S = (100);
const int32_t TOUCH_INDEX = (1);
const int32_t DEV_INDEX2 = (2);
const int32_t DEV_INDEX3 = (3);
const int32_t INVALID_INDEX = (15);
const int32_t NAME_MAX_LEN = (10);
const int32_t TEST_RESULT_LEN = (32);
const int32_t MAX_DEVICES = 32;
using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::Input;
void InputManagerImplTetst::SetUpTestCase(void)
{
    std::cout << "Input::InputManagerImplTetst SetUpTestCase start" << std::endl;
}

void InputManagerImplTetst::TearDownTestCase(void)
{
    std::cout << "Input::InputManagerImplTetst TearDownTestCase" << std::endl;
}

void InputManagerImplTetst::SetUp(void)
{
}

void InputManagerImplTetst::TearDown(void)
{
}

HWTEST_F(InputManagerImplTetst, UTestScanInputDevice, TestSize.Level0)
{
    DevDesc sta[MAX_DEVICES] = {};
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    auto arrLen = sizeof(sta)/sizeof(DevDesc);
    auto rc = InputHostObj->ScanInputDevice(sta, arrLen);
    for (auto i=0; i< arrLen ; i++) {
        std::cout<<"Index: "<<sta[i].devIndex << "Type: " << sta[i].devType << std::endl;
    }
    EXPECT_EQ(INPUT_SUCCESS, rc);
}

HWTEST_F(InputManagerImplTetst, UTestScanInputDeviceFailed, TestSize.Level0)
{
    DevDesc sta[MAX_DEVICES] = {};
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_serviceaaa");
    EXPECT_EQ(InputHostObj, nullptr);
}


HWTEST_F(InputManagerImplTetst, UTestOpenInputDevice, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    uint32_t index = TOUCH_INDEX;
    std::cout << "UTestOpenInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    EXPECT_EQ(INPUT_SUCCESS, InputHostObj->OpenInputDevice(index));
    std::cout << "UTestOpenInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
}

HWTEST_F(InputManagerImplTetst, UTestOpenInputDeviceFailed, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    uint32_t index = INVALID_INDEX;
    std::cout << "UTestOpenInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    EXPECT_NE(INPUT_SUCCESS, InputHostObj->OpenInputDevice(index));
    std::cout << "UTestOpenInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
}

HWTEST_F(InputManagerImplTetst, UTestCloseInputDevice, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    uint32_t index = TOUCH_INDEX;
    std::cout << "UTestCloseInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    EXPECT_EQ(INPUT_SUCCESS, InputHostObj->CloseInputDevice(index));
    std::cout << "UTestCloseInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
}

HWTEST_F(InputManagerImplTetst, UTestCloseInputDeviceFailed, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    uint32_t index = INVALID_INDEX;
    std::cout << "UTestCloseInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    EXPECT_NE(INPUT_SUCCESS, InputHostObj->CloseInputDevice(index));
    std::cout << "UTestCloseInputDevice" << __func__ << __LINE__ << __FILE__ << std::endl;
}


HWTEST_F(InputManagerImplTetst, UTestGetGetInputDevice, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    HDF_LOGI("%{public}s: [Input] UTestGetGetInputDevice enter", __func__);
    DeviceInfo *dev = new DeviceInfo();
    uint32_t devIndex = TOUCH_INDEX;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    auto ret = InputHostObj->GetInputDevice(devIndex, &dev);
    if (ret) {
        HDF_LOGE("%{public}s: get device1 failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("####$%{public}s: devindex = %{public}u, devType = %{public}u",
            __func__, dev->devIndex, dev->devType);
    HDF_LOGI("#####%{public}s: chipInfo = %{public}s, vendorName = %{public}s, chipName = %{public}s",
             __func__,  dev->chipInfo, dev->vendorName, dev->chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, UTestGetGetInputDeviceFailed, TestSize.Level0)
{
    std::cout << "Input::InputManagerImplTetst::" << __func__ << std::endl;
    HDF_LOGI("%{public}s: [Input] UTestGetGetInputDevice enter", __func__);
    DeviceInfo *dev = new DeviceInfo();
    uint32_t devIndex = INVALID_INDEX;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    auto ret = InputHostObj->GetInputDevice(devIndex, &dev);
    if (ret) {
        HDF_LOGE("%{public}s: get device1 failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("####$%{public}s: devindex = %{public}u, devType = %{public}u",
             __func__, dev->devIndex, dev->devType);
    HDF_LOGI("#####%{public}s: chipInfo = %{public}s, vendorName = %{public}s, chipName = %{public}s",
             __func__,  dev->chipInfo, dev->vendorName, dev->chipName);
    EXPECT_NE(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, UTestGetGetInputDeviceList, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] UTestGetGetInputDevice enter", __func__);
    uint32_t devNum = 0;
    DeviceInfo* dev = (DeviceInfo*)(new char[ sizeof(DeviceInfo) * TEST_RESULT_LEN]);
    uint32_t size = TEST_RESULT_LEN;
    int32_t rc = 0;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    rc = InputHostObj->GetInputDeviceList(&devNum, &dev, size);

    HDF_LOGI("#####%{public}s: num = %{public}u, size[%{public}d]'s info is:[%{public}p]",
            __func__, devNum, size, dev);

    for (uint32_t i = 0; i < devNum; i++) {
        if (dev != nullptr && (dev+i) != nullptr) {
            HDF_LOGI("####%{public}s: index = %{public}d, devType = %{public}d",
                    __func__, (dev+i)->devIndex, (dev+i)->devType);
            HDF_LOGI("####%{public}s: chipInfo = %{public}s, vendorName = %{public}s, chipName = %{public}s",
                    __func__, (dev+i)->chipInfo, (dev+i)->vendorName, (dev+i)->chipName);
        }
    }
    EXPECT_EQ(INPUT_SUCCESS, rc);
}

HWTEST_F(InputManagerImplTetst, UTestGetGetInputDeviceListFailed, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] UTestGetGetInputDevice enter", __func__);
    uint32_t devNum = 0;
    DeviceInfo* dev = nullptr;
    uint32_t size = TEST_RESULT_LEN;
    int32_t rc = 0;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    rc = InputHostObj->GetInputDeviceList(&devNum, &dev, size);
    EXPECT_NE(INPUT_SUCCESS, rc);
}


HWTEST_F(InputManagerImplTetst, GetVendorName001, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetVendorName001 enter", __func__);
    int32_t ret;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    char* vendorName = new char[MAX_NODE_PATH_LEN];
    for (auto i=1; i<4; i++) {
        ret = InputHostObj->GetVendorName(i, vendorName, MAX_NODE_PATH_LEN);
        if (ret) {
            HDF_LOGE("%{public}s: get device1's vendor name failed, ret %{public}d", __func__, ret);
        }
        HDF_LOGI("#####%{public}s: device1's vendor name is: %{public}s", __func__, vendorName);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}


HWTEST_F(InputManagerImplTetst, GetChipInfo, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetChipInfo enter", __func__);
    int32_t ret;
    uint32_t devIndex = TOUCH_INDEX;
    uint32_t length = MAX_NODE_PATH_LEN;
    char* chipInfo = new char[MAX_NODE_PATH_LEN];
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->GetChipInfo(devIndex, chipInfo, length);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's chipInfo failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: device1's chipInfo is %{public}s:", __func__, chipInfo);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, GetChipInfoFailed, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetChipInfo enter", __func__);
    int32_t ret;
    uint32_t devIndex = INVALID_INDEX;
    uint32_t length = MAX_NODE_PATH_LEN;
    char* chipInfo = new char[MAX_NODE_PATH_LEN];
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->GetChipInfo(devIndex, chipInfo, length);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's chipInfo failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: device1's chipInfo is %{public}s:", __func__, chipInfo);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, GetChipName, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetChipName enter", __func__);
    int32_t ret;
    uint32_t devIndex = TOUCH_INDEX;
    uint32_t length = MAX_NODE_PATH_LEN;
    char* chipName = new char[MAX_NODE_PATH_LEN];
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->GetChipName(devIndex, chipName, length);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's vendor name failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: device1's chipName is %{public}s:", __func__, chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, GetChipNameFailed, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetChipName enter", __func__);
    int32_t ret;
    uint32_t devIndex = INVALID_INDEX;
    uint32_t length = MAX_NODE_PATH_LEN;
    char* chipName = new char[MAX_NODE_PATH_LEN];
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->GetChipName(devIndex, chipName, length);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's vendor name failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: device1's chipName is %{public}s:", __func__, chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, RegisterReportCallback, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] RegisterReportCallback enter", __func__);
    int32_t ret;
    uint32_t devIndex = TOUCH_INDEX;
    static OHOS::sptr<InputReportEventCallback> callback = new InputReportEventCallback();
    static OHOS::sptr<InputHostReportEventCallback> callback1 = new InputHostReportEventCallback();
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->RegisterReportCallback(devIndex, callback);
    if (ret) {
        HDF_LOGE("%{public}s:RegisterHotPlugCallback failed, ret %{public}d", __func__, ret);
    }
    ret = InputHostObj->RegisterReportCallback(DEV_INDEX2, callback);
    if (ret) {
        HDF_LOGE("%{public}s:RegisterHotPlugCallback failed, ret %{public}d", __func__, ret);
    }
    ret = InputHostObj->RegisterReportCallback(DEV_INDEX3, callback);
    if (ret) {
        HDF_LOGE("%{public}s:RegisterHotPlugCallback failed, ret %{public}d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    ret = InputHostObj->RegisterHotPlugCallback(callback1);
    if (ret) {
        HDF_LOGE("%{public}s: RegisterReportCallback failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: %{public}d:", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
    EXPECT_EQ(INPUT_SUCCESS, InputHostObj->OpenInputDevice(devIndex));
    EXPECT_EQ(INPUT_SUCCESS, InputHostObj->OpenInputDevice(DEV_INDEX2));
    EXPECT_EQ(INPUT_SUCCESS, InputHostObj->OpenInputDevice(DEV_INDEX3));
    sleep(KEEP_ALIVE_TIME_S);
}

HWTEST_F(InputManagerImplTetst, UnregisterReportCallback, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] UnregisterReportCallback enter", __func__);
    int32_t ret;
    uint32_t devIndex = TOUCH_INDEX;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->UnregisterReportCallback(devIndex);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's vendor name failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: %{public}d:", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, UnregisterReportCallback1, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] UnregisterReportCallback enter", __func__);
    int32_t ret;
    uint32_t devIndex = INVALID_INDEX;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->UnregisterReportCallback(devIndex);
    if (ret) {
        HDF_LOGE("%{public}s: get device1's vendor name failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: %{public}d:", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, RegisterHotPlugCallback, TestSize.Level0)
{
    int32_t ret;
    static OHOS::sptr<InputHostReportEventCallback> callback1 = new InputHostReportEventCallback();
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->RegisterHotPlugCallback(callback1);
    if (ret) {
        HDF_LOGE("%{public}s:RegisterHotPlugCallback failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: %{public}d:", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, RegisterHotPlugCallback1, TestSize.Level0)
{
    int32_t ret;
    static OHOS::sptr<InputReportEventCallback> callback1 = new InputReportEventCallback();
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service1");
    EXPECT_EQ(InputHostObj, nullptr);
}

HWTEST_F(InputManagerImplTetst, UnregisterHotPlugCallback, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetDeviceType enter", __func__);
    int32_t ret;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service");
    ASSERT_TRUE(InputHostObj != nullptr);
    ret = InputHostObj->UnregisterHotPlugCallback();
    if (ret) {
        HDF_LOGE("%{public}s:UnregisterHotPlugCallback failed, ret %{public}d", __func__, ret);
    }
    HDF_LOGI("#####%{public}s: %{public}d:", __func__, __LINE__);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

HWTEST_F(InputManagerImplTetst, UnregisterHotPlugCallback1, TestSize.Level0)
{
    HDF_LOGI("%{public}s: [Input] GetDeviceType enter", __func__);
    int32_t ret;
    OHOS::sptr<IInputHost> InputHostObj = IInputHost::Get("input_service2");
    EXPECT_EQ(InputHostObj, nullptr);
}

