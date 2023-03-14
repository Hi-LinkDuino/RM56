/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ap_monitor.h"
#include "operator_overload.h"
#include "mock_wifi_ap_hal_interface.h"

using namespace OHOS;
using ::testing::_;
using ::testing::An;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
const std::string Mac = "AA:BB:CC:DD:EE:FF";
StationInfo staInfo = {
    "test_deviceName",
    Mac.c_str(),
    "127.0.0.1",
};
const int AP_ENABLE = 109;
const int AP_DISABLE = 110;
const int AP_FAILED = 111;
class ApMonitor_Test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pApMonitor = new ApMonitor();
    }
    virtual void TearDown()
    {
        delete pApMonitor;
        pApMonitor = nullptr;
    }

public:
    void WraUnregisterHandler(const std::string &iface)
    {
        pApMonitor->UnregisterHandler(iface);
        return;
    }

public:
    ApMonitor *pApMonitor;
};

/* StationChangeEvent */

HWTEST_F(ApMonitor_Test, StationChangeEvent_JOIN, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    const int type = 105;
    pApMonitor->StartMonitor();
    WifiApConnectionNofify cInfo;
    cInfo.type = type;
    cInfo.mac = "AA:BB:CC:DD:EE:FF";
    pApMonitor->OnStaJoinOrLeave(cInfo);
}
HWTEST_F(ApMonitor_Test, StationChangeEvent_LEAVE, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    const int type = 106;
    pApMonitor->StartMonitor();
    WifiApConnectionNofify cInfo;
    cInfo.type = type;
    cInfo.mac = "AA:BB:CC:DD:EE:FF";
    pApMonitor->OnStaJoinOrLeave(cInfo);
}
HWTEST_F(ApMonitor_Test, StationChangeEvent_NULL, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StartMonitor();
    WifiApConnectionNofify cInfo;
    pApMonitor->OnStaJoinOrLeave(cInfo);
}
/* OnHotspotStateEvent */
HWTEST_F(ApMonitor_Test, OnHotspotStateEvent_ENABLE, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StartMonitor();
    pApMonitor->OnHotspotStateEvent(AP_ENABLE);
}

HWTEST_F(ApMonitor_Test, OnHotspotStateEvent_DISABLE, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StartMonitor();
    pApMonitor->OnHotspotStateEvent(AP_DISABLE);
}
HWTEST_F(ApMonitor_Test, OnHotspotStateEvent_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StartMonitor();
    pApMonitor->OnHotspotStateEvent(AP_FAILED);
}

/* StartMonitor */
HWTEST_F(ApMonitor_Test, StartMonitor_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StartMonitor();
}

/* StopMonitor */
HWTEST_F(ApMonitor_Test, StopMonitor_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pApMonitor->StopMonitor();
}
/* UnregisterHandler */
HWTEST_F(ApMonitor_Test, UnregisterHandler_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    WraUnregisterHandler("wlan1");
}
} // namespace Wifi
} // namespace OHOS