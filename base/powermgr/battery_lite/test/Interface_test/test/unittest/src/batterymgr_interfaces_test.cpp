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

#include "gtest/gtest.h"
#include "hilog/log.h"
#include "battery_info.h"
#include "battery_framework.h"

#undef LOG_TAG
#define LOG_TAG "BATTERYMGR_LITE"

using namespace std;
using namespace testing::ext;

namespace OHOS {
class BatterymgrInterfacesTest : public testing::Test {
protected:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: BatteryMgrInterfaceTest001
 * @tc.desc: Test the interface Battery::GetBatSoc.
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest001, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest001 called");
    int32_t soc = GetBatSoc();
    EXPECT_EQ ((soc >= 0 && soc <= 100), 1) << "GetBatterySoc ret = " << soc << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest002
 * @tc.desc: Test the interface Battery::GetChargingStatus().
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest002, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest002 called");
    BatteryChargeState status = GetChargingStatus();
    EXPECT_EQ ((status >= CHARGE_STATE_NONE && status <= CHARGE_STATE_BUTT), 1)
        << "GetChargingStatus ret = " << status << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest003
 * @tc.desc: Test the interface Battery::GetHealthStatus().
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest003, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest003 called");
    BatteryHealthState status = GetHealthStatus();
    EXPECT_EQ ((status >= HEALTH_STATE_UNKNOWN && status <= HEALTH_STATE_BUTT), 1) 
        << "GetHealthStatus ret = " << status << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest004
 * @tc.desc: Test the interface Battery::GetPluggedType
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest004, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest004 called");
    BatteryPluggedType status = GetPluggedType();
    EXPECT_EQ ((status >= PLUGGED_TYPE_NONE && status <= PLUGGED_TYPE_BUTT), 1) 
        << "GetPluggedType ret = " << status << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest005
 * @tc.desc: Test the interface Battery::GetBatVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest005, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest005 called");
    int32_t voltage = GetBatVoltage();
    EXPECT_EQ ((voltage >= 0 && voltage <= 100), 1) 
        << "GetBatVoltage ret = " << voltage << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest006
 * @tc.desc: Test the interface Battery::GetBatTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest006, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest007 called");
    int32_t temperature = GetBatTemperature();
    EXPECT_EQ ((temperature >= 0 && temperature <= 100), 1) 
        << "GetBatTemperature ret = " << temperature << endl;
};

/**
 * @tc.name: BatteryMgrInterfaceTest007
 * @tc.desc: Test the interface Battery::GetBatTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatterymgrInterfacesTest, BatteryMgrInterfaceTest007, TestSize.Level0)
{
    HILOG_INFO(HILOG_MODULE_APP, "BatteryMgrInterfaceTest006 called");
    char *technology = GetBatTechnology();
    EXPECT_EQ (strcmp(technology, "Ternary_Lithium"), 0) 
        << "GetBatTechnology ret = " << technology << endl;
};
};