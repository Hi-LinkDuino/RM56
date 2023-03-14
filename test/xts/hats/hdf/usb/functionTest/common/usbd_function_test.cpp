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
#include "usbd_function_test.h"
#include <iostream>
#include "hdf_log.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "usbd_client.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;

const int SLEEP_TIME = 3;
const int TEST_PORT_ID = 1;
const int TEST_POWER_ROLE = 2;
const int TEST_DATAR_ROLE = 2;
const int USB_FUNCTION_ACM = 1;
const int USB_FUNCTION_ECM = 2;
const int USB_FUNCTION_HDC = 4;

void UsbdFunctionTest::SetUpTestCase(void)
{
    auto ret = UsbdClient::GetInstance().SetPortRole(TEST_PORT_ID, TEST_POWER_ROLE, TEST_DATAR_ROLE);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdFunctionTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }
}

void UsbdFunctionTest::TearDownTestCase(void) {}

void UsbdFunctionTest::SetUp(void) {}

void UsbdFunctionTest::TearDown(void) {}

/**
 * @tc.name: UsbdGetCurrentFunctions001
 * @tc.desc: Test functions to GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdGetCurrentFunctions001, Function | MediumTest | Level1)
{
    int32_t funcs = 0;
    auto ret = UsbdClient::GetInstance().GetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdGetCurrentFunctions001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdGetCurrentFunctions002
 * @tc.desc: Test functions to GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdGetCurrentFunctions002, Function | MediumTest | Level1)
{
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(1);
    HDF_LOGI("UsbdFunctionTest::UsbdFunction011 %{public}d SetCurrentFunctions=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    int32_t funcs = 1;
    ret = UsbdClient::GetInstance().GetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdFunction001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions001
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions001, Function | MediumTest | Level1)
{
    int32_t funcs = 1;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions001 %{public}d SetCurrentFunctions=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions002
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions002, Function | MediumTest | Level1)
{
    int32_t funcs = -1;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdFunction002 %{public}d, ret=%{public}d, funcs=%{public}d", __LINE__, ret, funcs);
    ASSERT_TRUE(ret != 0);
}
/**
 * @tc.name: UsbdSetCurrentFunctions003
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions003, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ECM;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions004
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions004, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ACM | USB_FUNCTION_ECM;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions005
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions005, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_HDC;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions006
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions006, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ACM | USB_FUNCTION_HDC;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetCurrentFunctions007
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetCurrentFunctions007, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ECM | USB_FUNCTION_HDC;
    auto ret = UsbdClient::GetInstance().SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::UsbdSetCurrentFunctions007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetPortRole001
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetPortRole001, Function | MediumTest | Level1)
{
    auto ret = UsbdClient::GetInstance().SetPortRole(1, 1, 1);
    HDF_LOGI("UsbdFunctionTest::UsbdSetPortRole001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: UsbdSetPortRole002
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, UsbdSetPortRole002, Function | MediumTest | Level1)
{
    auto ret = UsbdClient::GetInstance().SetPortRole(2, 1, 1);
    HDF_LOGI("UsbdFunctionTest::UsbdSetPortRole002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: SetPortRole009
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SetPortRole09, Function | MediumTest | Level1)
{
    auto ret = UsbdClient::GetInstance().SetPortRole(1, 2, 2);
    HDF_LOGI("UsbdFunctionTest::SetPortRole09 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: QueryPort001
 * @tc.desc: Test functions to QueryPort
 * @tc.desc: int32_t QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, QueryPort001, Function | MediumTest | Level1)
{
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    int32_t mode = 0;
    auto ret = UsbdClient::GetInstance().QueryPort(portId, powerRole, dataRole, mode);
    HDF_LOGI("UsbdFunctionTest::QueryPort001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}
