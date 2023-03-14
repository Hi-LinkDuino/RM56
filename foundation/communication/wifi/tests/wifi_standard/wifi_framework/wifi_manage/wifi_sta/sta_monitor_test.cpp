/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "sta_monitor.h"
#include <gtest/gtest.h>
#include "mock_sta_state_machine.h"
#include "mock_wifi_sta_hal_interface.h"
#include "wifi_idl_define.h"
#include <string>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;


namespace OHOS {
namespace Wifi {
class StaMonitorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pStaMonitor = std::make_unique<StaMonitor>();
        pStaMonitor->pStaStateMachine = new MockStaStateMachine();
        InitStaMonitorSuccess();
    }
    virtual void TearDown()
    {
        pStaMonitor.reset();
    }

public:
    std::unique_ptr<StaMonitor> pStaMonitor;

    void InitStaMonitorSuccess();
    void InitStaMonitorFail();
    void UnInitStaMonitorSuccess();
    void UnInitStaMonitorFail();
    void SetStateMachineSuccess();
    void OnConnectChangedCallBackSuccess1();
    void OnConnectChangedCallBackSuccess2();
    void OnConnectChangedCallBackFail1();
    void OnConnectChangedCallBackFail2();
    void OnConnectChangedCallBackFail3();
    void OnWpaStateChangedCallBackSuccess();
    void OnWpaStateChangedCallBackFail1();
    void OnWpaStateChangedCallBackFail2();
    void OnWpaSsidWrongKeyCallBackSuccess();
    void OnWpaSsidWrongKeyCallBackFail1();
    void OnWpaSsidWrongKeyCallBackFail2();
    void OnWpaSsidWrongKeyCallBackFail3();
    void OnWpsPbcOverlapCallBackSuccess();
    void OnWpsPbcOverlapCallBackFail1();
    void OnWpsPbcOverlapCallBackFail2();
    void OnWpsTimeOutCallBackSuccess();
    void OnWpsTimeOutCallBackFail1();
    void OnWpsTimeOutCallBackFail2();
};

void StaMonitorTest::InitStaMonitorSuccess()
{
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaMonitor->InitStaMonitor() == WIFI_OPT_SUCCESS);
}

void StaMonitorTest::InitStaMonitorFail()
{
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaMonitor->InitStaMonitor() == WIFI_OPT_FAILED);
}

void StaMonitorTest::UnInitStaMonitorSuccess()
{
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaMonitor->UnInitStaMonitor() == WIFI_OPT_SUCCESS);
}

void StaMonitorTest::UnInitStaMonitorFail()
{
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaMonitor->UnInitStaMonitor() == WIFI_OPT_FAILED);
}

void StaMonitorTest::SetStateMachineSuccess()
{
    pStaMonitor->SetStateMachine(pStaMonitor->pStaStateMachine);
}

void StaMonitorTest::OnConnectChangedCallBackSuccess1()
{
    int status = WPA_CB_CONNECTED;
    int networkId = 1;
    std::string bssid = "01:23:45:67:89:AB";
    pStaMonitor->OnConnectChangedCallBack(status, networkId, bssid);
}

void StaMonitorTest::OnConnectChangedCallBackSuccess2()
{
    int status = WPA_CB_DISCONNECTED;
    int networkId = 1;
    std::string bssid = "01:23:45:67:89:AB";
    pStaMonitor->OnConnectChangedCallBack(status, networkId, bssid);
}

void StaMonitorTest::OnConnectChangedCallBackFail1()
{
    int status = WPA_CB_CONNECTED;
    int networkId = 0;
    std::string bssid = "00:00:00:00:00:00";
    pStaMonitor->pStaStateMachine = nullptr;
    pStaMonitor->OnConnectChangedCallBack(status, networkId, bssid);
}

void StaMonitorTest::OnConnectChangedCallBackFail2()
{
    int status = WPA_CB_CONNECTED;
    int networkId = 0;
    std::string bssid = "00:00:00:00:00:00";
    pStaMonitor->OnConnectChangedCallBack(status, networkId, bssid);
}

void StaMonitorTest::OnConnectChangedCallBackFail3()
{
    int status = -1;
    int networkId = 0;
    std::string bssid = "00:00:00:00:00:00";
    pStaMonitor->OnConnectChangedCallBack(status, networkId, bssid);
}

void StaMonitorTest::OnWpaStateChangedCallBackSuccess()
{
    int status = 1;
    pStaMonitor->OnWpaStateChangedCallBack(status);
}

void StaMonitorTest::OnWpaStateChangedCallBackFail1()
{
    int status = 1;
    pStaMonitor->pStaStateMachine = nullptr;
    pStaMonitor->OnWpaStateChangedCallBack(status);
}

void StaMonitorTest::OnWpaSsidWrongKeyCallBackSuccess()
{
    int status = 1;
    pStaMonitor->OnWpaSsidWrongKeyCallBack(status);
}

void StaMonitorTest::OnWpaSsidWrongKeyCallBackFail1()
{
    int status = 1;
    pStaMonitor->pStaStateMachine = nullptr;
    pStaMonitor->OnWpaSsidWrongKeyCallBack(status);
}

void StaMonitorTest::OnWpaSsidWrongKeyCallBackFail2()
{
    int status = -1;
    pStaMonitor->OnWpaSsidWrongKeyCallBack(status);
}

void StaMonitorTest::OnWpsPbcOverlapCallBackSuccess()
{
    int status = 1;
    pStaMonitor->OnWpsPbcOverlapCallBack(status);
}

void StaMonitorTest::OnWpsPbcOverlapCallBackFail1()
{
    int status = 1;
    pStaMonitor->pStaStateMachine = nullptr;
    pStaMonitor->OnWpsPbcOverlapCallBack(status);
}

void StaMonitorTest::OnWpsTimeOutCallBackSuccess()
{
    int status = 1;
    pStaMonitor->OnWpsTimeOutCallBack(status);
}

void StaMonitorTest::OnWpsTimeOutCallBackFail1()
{
    int status = 1;
    pStaMonitor->pStaStateMachine = nullptr;
    pStaMonitor->OnWpsTimeOutCallBack(status);
}

HWTEST_F(StaMonitorTest, InitStaMonitorSuccess, TestSize.Level1)
{
    InitStaMonitorSuccess();
}

HWTEST_F(StaMonitorTest, InitStaMonitorFail, TestSize.Level1)
{
    InitStaMonitorFail();
}

HWTEST_F(StaMonitorTest, UnInitStaMonitorSuccess, TestSize.Level1)
{
    UnInitStaMonitorSuccess();
}

HWTEST_F(StaMonitorTest, UnInitStaMonitorFail, TestSize.Level1)
{
    UnInitStaMonitorFail();
}

HWTEST_F(StaMonitorTest, OnConnectChangedCallBackFail1, TestSize.Level1)
{
    OnConnectChangedCallBackFail1();
}

HWTEST_F(StaMonitorTest, OnConnectChangedCallBackFail2, TestSize.Level1)
{
    OnConnectChangedCallBackFail2();
}

HWTEST_F(StaMonitorTest, SetStateMachineSuccess, TestSize.Level1)
{
    SetStateMachineSuccess();
}


HWTEST_F(StaMonitorTest, OnConnectChangedCallBackSuccess1, TestSize.Level1)
{
    OnConnectChangedCallBackSuccess1();
}

HWTEST_F(StaMonitorTest, OnConnectChangedCallBackSuccess2, TestSize.Level1)
{
    OnConnectChangedCallBackSuccess2();
}

HWTEST_F(StaMonitorTest, OnWpaStateChangedCallBackSuccess, TestSize.Level1)
{
    OnWpaStateChangedCallBackSuccess();
}

HWTEST_F(StaMonitorTest, OnWpaStateChangedCallBackFail1, TestSize.Level1)
{
    OnWpaStateChangedCallBackFail1();
}

HWTEST_F(StaMonitorTest, OnWpaSsidWrongKeyCallBackSuccess, TestSize.Level1)
{
    OnWpaSsidWrongKeyCallBackSuccess();
}

HWTEST_F(StaMonitorTest, OnWpaSsidWrongKeyCallBackFail1, TestSize.Level1)
{
    OnWpaSsidWrongKeyCallBackFail1();
}

HWTEST_F(StaMonitorTest, OnWpaSsidWrongKeyCallBackFail2, TestSize.Level1)
{
    OnWpaSsidWrongKeyCallBackFail2();
}

HWTEST_F(StaMonitorTest, OnWpsPbcOverlapCallBackSuccess, TestSize.Level1)
{
    OnWpsPbcOverlapCallBackSuccess();
}

HWTEST_F(StaMonitorTest, OnWpsPbcOverlapCallBackFail1, TestSize.Level1)
{
    OnWpsPbcOverlapCallBackFail1();
}

HWTEST_F(StaMonitorTest, OnWpsTimeOutCallBackSuccess, TestSize.Level1)
{
    OnWpsTimeOutCallBackSuccess();
}

HWTEST_F(StaMonitorTest, OnWpsTimeOutCallBackFail1, TestSize.Level1)
{
    OnWpsTimeOutCallBackFail1();
}
} // WIFI
} // OHOS
