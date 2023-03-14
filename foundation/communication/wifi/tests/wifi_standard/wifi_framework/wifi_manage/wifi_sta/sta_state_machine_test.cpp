/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "sta_state_machine.h"
#include "sta_define.h"
#include "mock_wifi_settings.h"
#include "mock_wifi_supplicant_hal_interface.h"
#include "mock_wifi_sta_hal_interface.h"
#include "mock_wifi_manager.h"
#include "mock_dhcp_service.h"
#include "mock_wifi_chip_hal_interface.h"
#include "mock_sta_network_check.h"
#include "mock_if_config.h"
#include <gtest/gtest.h>
#include "internal_message.h"
#include "mock_if_config.h"

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
class StaStateMachineTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pStaStateMachine.reset(new StaStateMachine());
        pStaStateMachine->InitStaStateMachine();

        NetStateHandler handle;
        pStaStateMachine->pNetcheck = new MockStaNetworkCheck(handle);
        pStaStateMachine->RegisterStaServiceCallback(WifiManager::GetInstance().GetStaCallback());
    }
    virtual void TearDown()
    {
        pStaStateMachine.reset();
    }
    void SleepMs(const int sleepMs)
    {
        std::unique_lock<std::mutex> lck(mMtxBlock);
        mCvTest.wait_for(lck, std::chrono::milliseconds(sleepMs));
    }
    std::unique_ptr<StaStateMachine> pStaStateMachine;
    std::mutex mMtxBlock;
    std::condition_variable mCvTest;

    void InitWifiLinkedInfoSuccess()
    {
        pStaStateMachine->InitWifiLinkedInfo();
    }

    void InitLastWifiLinkedInfoSuccess()
    {
        pStaStateMachine->InitLastWifiLinkedInfo();
    }

    void BuildStateTreeSuccess()
    {
        pStaStateMachine->BuildStateTree();
    }

    void RegisterStaServiceCallbackSuccess()
    {
        StaServiceCallback callbacks;
        pStaStateMachine->RegisterStaServiceCallback(callbacks);
    }

    void DealConnectTimeOutCmd()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), DisableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(1));
        InternalMessage msg;
        pStaStateMachine->DealConnectTimeOutCmd(&msg);
    }

    void RootStateGoInStateSuccess()
    {
        pStaStateMachine->pRootState->GoInState();
    }

    void RootStateGoOutStateSuccess()
    {
        pStaStateMachine->pRootState->GoOutState();
    }

    void RootStateExeMsgSuccess()
    {
        InternalMessage msg;
        EXPECT_TRUE(pStaStateMachine->pRootState->ExecuteStateMsg(&msg));
    }

    void RootStateExeMsgFail()
    {
        EXPECT_FALSE(pStaStateMachine->pRootState->ExecuteStateMsg(nullptr));
    }

    void InitStateGoInStateSuccess()
    {
        pStaStateMachine->pInitState->GoInState();
    }

    void InitStateGoOutStateSuccess()
    {
        pStaStateMachine->pInitState->GoOutState();
    }

    void InitStateExeMsgSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaOpenRes(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_ENABLE_WIFI);
        EXPECT_TRUE(pStaStateMachine->pInitState->ExecuteStateMsg(&msg));
    }

    void InitStateExeMsgFail1()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_DISABLE_WIFI);
        EXPECT_FALSE(pStaStateMachine->pInitState->ExecuteStateMsg(&msg));
    }

    void InitStateExeMsgFail2()
    {
        EXPECT_FALSE(pStaStateMachine->pInitState->ExecuteStateMsg(nullptr));
    }

    void ConvertDeviceCfgSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        WifiDeviceConfig config;
        EXPECT_EQ(WIFI_OPT_SUCCESS, pStaStateMachine->ConvertDeviceCfg(config));
    }

    void ConvertDeviceCfgFail1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        WifiDeviceConfig config;
        EXPECT_EQ(WIFI_OPT_FAILED, pStaStateMachine->ConvertDeviceCfg(config));
    }

    void ConvertDeviceCfgFail2()
    {
    }

    void StartWifiProcessSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaOpenRes(_)).Times(testing::AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).WillRepeatedly(Return(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        std::vector<WifiDeviceConfig> results;
        WifiDeviceConfig wifiDeviceConfig;
        wifiDeviceConfig.networkId = 1;
        results.push_back(wifiDeviceConfig);
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(results), Return(0)));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(1), Return(WIFI_IDL_OPT_FAILED)));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartWifiProcess();
    }

    void StartWifiProcessFail1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaOpenRes(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).WillRepeatedly(Return(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        std::vector<WifiDeviceConfig> results;
        WifiDeviceConfig wifiDeviceConfig;
        wifiDeviceConfig.networkId = 1;
        results.push_back(wifiDeviceConfig);
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(results), Return(0)));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartWifiProcess();
    }

    void StartWifiProcessFail2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaOpenRes(_)).Times(testing::AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartWifiProcess();
    }

    void InitWpsSettingsSuccess()
    {
        pStaStateMachine->pWpaStartingState->InitWpsSettings();
    }

    void WpaStartingStateGoInStateSuccess()
    {
        pStaStateMachine->pWpaStartingState->GoInState();
    }

    void WpaStartingStateGoOutStateSuccess()
    {
        pStaStateMachine->pWpaStartingState->GoOutState();
    }

    void WpaStartingStateExeMsgSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_SUP_CONNECTION_EVENT);
        EXPECT_TRUE(pStaStateMachine->pWpaStartingState->ExecuteStateMsg(&msg));
    }

    void WpaStartingStateExeMsgFail1()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_SUP_DISCONNECTION_EVENT);
        EXPECT_FALSE(pStaStateMachine->pWpaStartingState->ExecuteStateMsg(&msg));
    }

    void WpaStartingStateExeMsgFail2()
    {
        EXPECT_FALSE(pStaStateMachine->pWpaStartingState->ExecuteStateMsg(nullptr));
    }

    void WpaStartedStateGoInStateSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(1));
        pStaStateMachine->operationalMode = STA_CONNECT_MODE;
        pStaStateMachine->pWpaStartedState->GoInState();
    }

    void WpaStartedStateGoInStateSuccess2()
    {
        pStaStateMachine->operationalMode = STA_DISABLED_MODE;
        pStaStateMachine->pWpaStartedState->GoInState();
    }

    void WpaStartedStateGoOutStateSuccess()
    {
        pStaStateMachine->pWpaStartedState->GoOutState();
    }

    void WpaStartedStateExeMsgSuccess()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealStaCloseRes(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(IfConfig::GetInstance(), FlushIpAddr(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_DISABLE_WIFI);
        EXPECT_TRUE(pStaStateMachine->pWpaStartedState->ExecuteStateMsg(&msg));
    }

    void WpaStartedStateExeMsgFail1()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_ENABLE_WIFI);
        EXPECT_FALSE(pStaStateMachine->pWpaStartedState->ExecuteStateMsg(&msg));
    }

    void WpaStartedStateExeMsgFail2()
    {
        EXPECT_FALSE(pStaStateMachine->pWpaStartedState->ExecuteStateMsg(nullptr));
    }

    void StopWifiProcessSuccess1()
    {
        pStaStateMachine->currentTpType = IPTYPE_IPV4;
        EXPECT_CALL(WifiManager::GetInstance(), DealStaCloseRes(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pStaStateMachine->StopWifiProcess();
    }

    void StopWifiProcessSuccess2()
    {
        pStaStateMachine->currentTpType = IPTYPE_IPV6;
        EXPECT_CALL(WifiManager::GetInstance(), DealStaCloseRes(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pStaStateMachine->StopWifiProcess();
    }

    void StopWifiProcessFail()
    {
        pStaStateMachine->currentTpType = IPTYPE_IPV6;
        EXPECT_CALL(WifiManager::GetInstance(), DealStaCloseRes(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWifi()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->StopWifiProcess();
    }

    void WpaStoppingStateGoInStateSuccess()
    {
        pStaStateMachine->pWpaStoppingState->GoInState();
    }

    void WpaStoppingStateGoOutStateSuccess()
    {
        pStaStateMachine->pWpaStoppingState->GoOutState();
    }

    void WpaStoppingStateExeMsgSuccess()
    {
        InternalMessage msg;
        pStaStateMachine->pWpaStoppingState->ExecuteStateMsg(&msg);
    }

    void WpaStoppingStateExeMsgFail()
    {
        pStaStateMachine->pWpaStoppingState->ExecuteStateMsg(nullptr);
    }

    void LinkStateGoInStateSuccess()
    {
        pStaStateMachine->pLinkedState->GoInState();
    }

    void LinkStateGoOutStateSuccess()
    {
        pStaStateMachine->pLinkedState->GoOutState();
    }

    void LinkStateExeMsgFail()
    {
        EXPECT_FALSE(pStaStateMachine->pLinkedState->ExecuteStateMsg(nullptr));
    }

    void InitStaSMHandleMapSuccess()
    {
        EXPECT_EQ(WIFI_OPT_SUCCESS, pStaStateMachine->InitStaSMHandleMap());
    }

    void DealConnectToUserSelectedNetworkSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), EnableNetwork(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetDeviceState(_, _, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Connect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        InternalMessage msg;
        msg.SetParam1(0);
        msg.SetParam2(0);
        pStaStateMachine->DealConnectToUserSelectedNetwork(&msg);
    }

    void DealConnectToUserSelectedNetworkFail()
    {
        pStaStateMachine->DealConnectToUserSelectedNetwork(nullptr);
    }

    void DealConnectTimeOutCmdSuccess()
    {
        InternalMessage msg;
        pStaStateMachine->DealConnectToUserSelectedNetwork(nullptr);
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), DisableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        pStaStateMachine->DealConnectTimeOutCmd(&msg);
    }

    void DealConnectTimeOutCmdFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        pStaStateMachine->DealConnectTimeOutCmd(nullptr);
    }

    void DealConnectionEventSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(IfConfig::GetInstance(), FlushIpAddr(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::INVALID;
        pStaStateMachine->currentTpType = IPTYPE_IPV4;
        pStaStateMachine->lastLinkedInfo.detailedState = DetailedState::CONNECTING;
        InternalMessage msg;
        pStaStateMachine->DealDisconnectEvent(&msg);
    }

    void DealConnectionEventSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetMacAddress(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        EXPECT_CALL(IfConfig::GetInstance(), FlushIpAddr(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::INVALID;
        pStaStateMachine->currentTpType = IPTYPE_IPV6;
        pStaStateMachine->lastLinkedInfo.detailedState = DetailedState::CONNECTED;
        InternalMessage msg;
        pStaStateMachine->DealDisconnectEvent(&msg);
    }

    void DealWpaWrongPskEventSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        InternalMessage msg;
        pStaStateMachine->DealWpaLinkFailEvent(&msg);
    }

    void DealWpaWrongPskEventFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        InternalMessage msg;
        pStaStateMachine->DealWpaLinkFailEvent(nullptr);
    }

    void DealReassociateCmdSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal());
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        InternalMessage msg;
        pStaStateMachine->DealReassociateCmd(&msg);
    }

    void DealReassociateCmdFail1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal());
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        pStaStateMachine->DealReassociateCmd(nullptr);
    }

    void DealReassociateCmdFail2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal());
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->DealReassociateCmd(nullptr);
    }

    void DealStartWpsCmdSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNetworkList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        pStaStateMachine->wpsState = SetupMethod::DISPLAY;
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::INVALID));
        pStaStateMachine->DealStartWpsCmd(&msg);
    }

    void StartWpsModeSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPbcMode(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::PBC));
        pStaStateMachine->StartWpsMode(&msg);
    }

    void StartWpsModeSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPinMode(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::DISPLAY));
        msg.AddStringMessageBody("hmwifi1");
        msg.AddStringMessageBody("hmwifi2");
        pStaStateMachine->StartWpsMode(&msg);
    }

    void StartWpsModeSuccess3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPinMode(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::KEYPAD));
        msg.AddStringMessageBody("hmwifi1");
        msg.AddStringMessageBody("hmwifi2");
        pStaStateMachine->StartWpsMode(&msg);
    }

    void StartWpsModeSuccess4()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::INVALID));
        msg.AddStringMessageBody("hmwifi1");
        msg.AddStringMessageBody("hmwifi2");
        pStaStateMachine->StartWpsMode(&msg);
    }

    void StartWpsModeFail1()
    {
        pStaStateMachine->StartWpsMode(nullptr);
    }

    void StartWpsModeFail2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPbcMode(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::PBC));
        pStaStateMachine->StartWpsMode(nullptr);
    }

    void StartWpsModeFail3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPinMode(_, _))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::DISPLAY));
        pStaStateMachine->StartWpsMode(nullptr);
    }

    void StartWpsModeFail4()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWpsPinMode(_, _))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        msg.SetParam1(static_cast<int>(SetupMethod::KEYPAD));
        pStaStateMachine->StartWpsMode(nullptr);
    }

    void DealWpaBlockListClearEventSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaBlocklistClear()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        pStaStateMachine->DealWpaBlockListClearEvent(&msg);
    }

    void DealWpaBlockListClearEventFail()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaBlocklistClear())
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        pStaStateMachine->DealWpaBlockListClearEvent(&msg);
    }

    void DealWpsConnectTimeOutEventSuccess()
    {
        pStaStateMachine->wpsState = SetupMethod::INVALID;
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return((WIFI_IDL_OPT_FAILED)));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        pStaStateMachine->DealWpsConnectTimeOutEvent(&msg);
    }

    void DealWpsConnectTimeOutEventFail()
    {
        pStaStateMachine->wpsState = SetupMethod::INVALID;
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return((WIFI_IDL_OPT_FAILED)));
        pStaStateMachine->DealWpsConnectTimeOutEvent(nullptr);
    }

    void DealCancelWpsCmdSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        pStaStateMachine->wpsState = SetupMethod::PBC;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealCancelWpsCmdSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        pStaStateMachine->wpsState = SetupMethod::DISPLAY;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealCancelWpsCmdSuccess3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::KEYPAD;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealCancelWpsCmdFail1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::PBC;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealCancelWpsCmdFail2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::DISPLAY;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealCancelWpsCmdFail3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _)).Times(AtLeast(0));
        pStaStateMachine->wpsState = SetupMethod::KEYPAD;
        InternalMessage msg;
        pStaStateMachine->DealCancelWpsCmd(&msg);
    }

    void DealStartRoamCmdSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        pStaStateMachine->DealStartRoamCmd(&msg);
    }

    void DealStartRoamCmdFail1()
    {
        pStaStateMachine->DealStartRoamCmd(nullptr);
    }

    void DealStartRoamCmdFail2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        InternalMessage msg;
        pStaStateMachine->DealStartRoamCmd(&msg);
    }

    void DealStartRoamCmdFail3()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Reassociate()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        pStaStateMachine->DealStartRoamCmd(&msg);
    }

    void StartConnectToNetworkSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Connect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartConnectToNetwork(0);
    }

    void StartConnectToNetworkFail1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartConnectToNetwork(0);
    }

    void StartConnectToNetworkFail2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Connect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartConnectToNetwork(0);
    }

    void StartConnectToNetworkFali3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), EnableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Connect(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        pStaStateMachine->StartConnectToNetwork(0);
    }

    void MacAddressGenerateSuccess()
    {
        std::string strMac = "hmwifi";
        pStaStateMachine->MacAddressGenerate(strMac);
    }

    void SetRandomMacSuccess1()
    {
        WifiDeviceConfig deviceConfig;
        deviceConfig.wifiPrivacySetting = WifiPrivacyConfig::RANDOMMAC;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaDeviceMacAddress(_))
            .WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMacAddress(_)).Times(AtLeast(0)).WillOnce(Return(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetConnectMacAddr(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pStaStateMachine->SetRandomMac(0);
    }

    void SetRandomMacFail1()
    {
        WifiDeviceConfig deviceConfig;
        deviceConfig.wifiPrivacySetting = WifiPrivacyConfig::RANDOMMAC;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(deviceConfig), Return(-1)));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMacAddress(_)).Times(AtLeast(0)).WillOnce(Return(0));
        pStaStateMachine->SetRandomMac(0);
    }

    void SetRandomMacFail2()
    {
        WifiDeviceConfig deviceConfig;
        deviceConfig.wifiPrivacySetting = WifiPrivacyConfig::RANDOMMAC;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaDeviceMacAddress(_))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMacAddress(_)).Times(AtLeast(0)).WillOnce(Return(0));
        pStaStateMachine->SetRandomMac(0);
    }

    void StartRoamToNetworkSuccess()
    {
        pStaStateMachine->StartRoamToNetwork("a2:b1:f5:c7:d1");
    }

    void OnNetworkConnectionEventSuccess()
    {
        pStaStateMachine->OnNetworkConnectionEvent(0, "a2:b1:f5:c7:d1");
    }

    void SeparatingStateGoInStateSuccess()
    {
        pStaStateMachine->pSeparatingState->GoInState();
    }

    void SeparatingStateGoOutStateSuccess()
    {
        pStaStateMachine->pSeparatingState->GoOutState();
    }

    void SeparatingStateExeMsgSuccess()
    {
        InternalMessage msg;
        pStaStateMachine->pSeparatingState->ExecuteStateMsg(&msg);
    }

    void SeparatingStateExeMsgFail()
    {
        pStaStateMachine->pSeparatingState->ExecuteStateMsg(nullptr);
    }

    void SeparatedStateGoInStateSuccess()
    {
        pStaStateMachine->pSeparatedState->GoInState();
    }

    void SeparatedStateGoOutStateSuccess()
    {
        pStaStateMachine->pSeparatedState->GoOutState();
    }

    void SeparatedStateExeMsgSuccess1()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_NETWORK_DISCONNECTION_EVENT);
        EXPECT_FALSE(pStaStateMachine->pSeparatedState->ExecuteStateMsg(&msg));
    }

    void SeparatedStateExeMsgSuccess2()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealStaOpenRes(_));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_ENABLE_WIFI);
        EXPECT_TRUE(pStaStateMachine->pSeparatedState->ExecuteStateMsg(&msg));
    }

    void SeparatedStateExeMsgFail()
    {
        EXPECT_FALSE(pStaStateMachine->pSeparatedState->ExecuteStateMsg(nullptr));
    }

    void ApLinkedStateGoInStateSuccess()
    {
        pStaStateMachine->pApLinkedState->GoInState();
    }

    void ApLinkedStateGoOutStateSuccess()
    {
        pStaStateMachine->pApLinkedState->GoOutState();
    }

    void ApLinkedStateExeMsgSuccess1()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(testing::AtLeast(1));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_DISCONNECT);
        EXPECT_TRUE(pStaStateMachine->pApLinkedState->ExecuteStateMsg(&msg));
    }

    void ApLinkedStateExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT);
        EXPECT_TRUE(pStaStateMachine->pApLinkedState->ExecuteStateMsg(&msg));
    }

    void ApLinkedStateExeMsgFail1()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_NETWORK_DISCONNECTION_EVENT);
        EXPECT_FALSE(pStaStateMachine->pApLinkedState->ExecuteStateMsg(&msg));
    }

    void ApLinkedStateExeMsgFai2()
    {
        EXPECT_FALSE(pStaStateMachine->pApLinkedState->ExecuteStateMsg(nullptr));
    }

    void DisConnectProcessSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pStaStateMachine->DisConnectProcess();
    }

    void DisConnectProcessFail()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->DisConnectProcess();
    }

    void WpsStateGoInStateSuccess()
    {
        pStaStateMachine->pWpsState->GoInState();
    }

    void WpsStateGoOutStateSuccess()
    {
        pStaStateMachine->pWpsState->GoOutState();
    }

    void WpsStateExeMsgSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0)).WillRepeatedly(Return(0));
        EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SetDeviceState(_, _, _)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT);
        EXPECT_TRUE(pStaStateMachine->pWpsState->ExecuteStateMsg(&msg));
    }

    void WpsStateExeMsgSuccess2()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_STARTWPS);
        msg.SetParam1(static_cast<int>(SetupMethod::PBC));
        EXPECT_TRUE(pStaStateMachine->pWpsState->ExecuteStateMsg(&msg));
    }

    void WpsStateExeMsgSuccess3()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_STARTWPS);
        msg.SetParam1(static_cast<int>(SetupMethod::DISPLAY));
        EXPECT_TRUE(pStaStateMachine->pWpsState->ExecuteStateMsg(&msg));
    }

    void WpsStateExeMsgSuccess4()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_WPS_OVERLAP_EVENT);
        msg.SetParam1(static_cast<int>(SetupMethod::DISPLAY));
        EXPECT_TRUE(pStaStateMachine->pWpsState->ExecuteStateMsg(&msg));
    }

    void WpsStateExeMsgSuccess5()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealWpsChanged(_, _));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopWps()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_WPS_OVERLAP_EVENT);
        msg.SetParam1(static_cast<int>(SetupMethod::DISPLAY));
        EXPECT_TRUE(pStaStateMachine->pWpsState->ExecuteStateMsg(&msg));
    }

    void WpsStateExeMsgFail1()
    {
        EXPECT_FALSE(pStaStateMachine->pWpsState->ExecuteStateMsg(nullptr));
    }

    void WpsStateExeMsgFail2()
    {
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_WPS_START_EVENT);
        EXPECT_FALSE(pStaStateMachine->pWpsState->ExecuteStateMsg(nullptr));
    }

    void SyncAllDeviceConfigsSuccess()
    {
        std::vector<WifiDeviceConfig> result;
        WifiDeviceConfig wifiDeviceConfig;
        result.push_back(wifiDeviceConfig);
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(result), Return(0)));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig());
        pStaStateMachine->SyncAllDeviceConfigs();
    }

    void SyncAllDeviceConfigsFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->SyncAllDeviceConfigs();
    }

    void GetIpStateStateGoInStateSuccess1()
    {
        WifiDeviceConfig config;
        config.wifiProxyconfig.configureMethod = ConfigureProxyMethod::MANUALCONFIGUE;
        config.wifiIpConfig.assignMethod = AssignIpMethod::STATIC;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(config), Return(0)));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->pGetIpState->GoInState();
    }

    void GetIpStateStateGoInStateSuccess2()
    {
        pStaStateMachine->isRoam = true;
        WifiDeviceConfig config;
        config.wifiProxyconfig.configureMethod = ConfigureProxyMethod::MANUALCONFIGUE;
        config.wifiIpConfig.assignMethod = AssignIpMethod::DHCP;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDhcpIpType()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(config), Return(0)));
        pStaStateMachine->pGetIpState->GoInState();
    }

    void GetIpStateStateGoInStateSuccess3()
    {
        pStaStateMachine->isRoam = false;
        WifiDeviceConfig config;
        config.wifiProxyconfig.configureMethod = ConfigureProxyMethod::MANUALCONFIGUE;
        config.wifiIpConfig.assignMethod = AssignIpMethod::DHCP;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
            .WillRepeatedly(DoAll(SetArgReferee<1>(config), Return(0)));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDhcpIpType()).WillRepeatedly(Return(IPTYPE_IPV4));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->pGetIpState->GoInState();
    }

    void GetIpStateStateGoOutStateSuccess()
    {
        pStaStateMachine->pGetIpState->GoOutState();
    }

    void GetIpStateStateExeMsgSuccess()
    {
        InternalMessage msg;
        pStaStateMachine->pGetIpState->ExecuteStateMsg(&msg);
    }

    void GetIpStateStateExeMsgFail()
    {
        pStaStateMachine->pGetIpState->ExecuteStateMsg(nullptr);
    }

    void ConfigStaticIpAddressSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        pStaStateMachine->currentTpType = IPTYPE_IPV4;
        pStaStateMachine->getIpSucNum = 1;
        pStaStateMachine->isRoam = false;
        StaticIpAddress staticIpAddress;
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(AtLeast(0));
        ;
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(AtLeast(0));
        EXPECT_TRUE(pStaStateMachine->ConfigStaticIpAddress(staticIpAddress));
    }

    void ConfigStaticIpAddressSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        pStaStateMachine->currentTpType = IPTYPE_IPV6;
        pStaStateMachine->getIpSucNum = 1;
        pStaStateMachine->isRoam = false;
        StaticIpAddress staticIpAddress;
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(AtLeast(0));
        EXPECT_TRUE(pStaStateMachine->ConfigStaticIpAddress(staticIpAddress));
    }

    void ConfigStaticIpAddressSuccess3()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        pStaStateMachine->currentTpType = IPTYPE_MIX;
        pStaStateMachine->getIpSucNum = 1;
        pStaStateMachine->isRoam = false;
        StaticIpAddress staticIpAddress;
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealStaConnChanged(_, _)).Times(AtLeast(0));
        EXPECT_TRUE(pStaStateMachine->ConfigStaticIpAddress(staticIpAddress));
    }

    void ConfigStaticIpAddressFail()
    {
        pStaStateMachine->currentTpType = IPTYPE_BUTT;
        StaticIpAddress staticIpAddress;
        pStaStateMachine->getIpSucNum = 1;
        pStaStateMachine->isRoam = false;
        EXPECT_CALL(WifiSettings::GetInstance(), SaveIpInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        EXPECT_FALSE(pStaStateMachine->ConfigStaticIpAddress(staticIpAddress));
    }

    void HandleNetCheckResultSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        pStaStateMachine->linkedInfo.connState = ConnState::CONNECTED;
        pStaStateMachine->HandleNetCheckResult(StaNetState::NETWORK_STATE_WORKING, "");
    }

    void HandleNetCheckResultSuccess3()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        pStaStateMachine->linkedInfo.connState = ConnState::CONNECTED;
        pStaStateMachine->HandleNetCheckResult(StaNetState::NETWORK_STATE_UNKNOWN, "");
    }

    void HandleNetCheckResultFail()
    {
        pStaStateMachine->linkedInfo.connState = ConnState::DISCONNECTED;
        pStaStateMachine->HandleNetCheckResult(StaNetState::NETWORK_STATE_UNKNOWN, "");
    }

    void LinkedStateGoInStateSuccess()
    {
        pStaStateMachine->pLinkedState->GoInState();
    }

    void LinkedStateGoOutStateSuccess()
    {
        pStaStateMachine->pLinkedState->GoOutState();
    }

    void LinkedStateExeMsgSuccess()
    {
        InternalMessage msg;
        pStaStateMachine->pLinkedState->ExecuteStateMsg(&msg);
    }

    void LinkedStateExeMsgFail()
    {
        pStaStateMachine->pLinkedState->ExecuteStateMsg(nullptr);
    }

    void ApRoamingStateGoInStateSuccess()
    {
        pStaStateMachine->pApRoamingState->GoInState();
    }

    void ApRoamingStateGoOutStateSuccess()
    {
        pStaStateMachine->pApRoamingState->GoOutState();
    }

    void ApRoamingStateExeMsgSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(testing::AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        InternalMessage msg;
        msg.SetMessageName(WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT);
        EXPECT_TRUE(pStaStateMachine->pApRoamingState->ExecuteStateMsg(&msg));
    }

    void ApRoamingStateExeMsgFail()
    {
        EXPECT_FALSE(pStaStateMachine->pApRoamingState->ExecuteStateMsg(nullptr));
    }

    void ConnectToNetworkProcessSuccess()
    {
        pStaStateMachine->wpsState = SetupMethod::DISPLAY;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).WillRepeatedly(Return(-1));
        WifiIdlGetDeviceConfig config;
        config.value = "hmwifi";
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(config), Return(WIFI_IDL_OPT_FAILED)));
        EXPECT_CALL(WifiSettings::GetInstance(), AddWpsDeviceConfig(_));
        EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig());
        EXPECT_CALL(WifiSettings::GetInstance(), SetWifiState(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        InternalMessage msg;
        pStaStateMachine->ConnectToNetworkProcess(&msg);
    }

    void ConnectToNetworkProcessFail()
    {
        pStaStateMachine->ConnectToNetworkProcess(nullptr);
    }

    void SetWifiLinkedInfoSuccess1()
    {
        pStaStateMachine->linkedInfo.networkId = INVALID_NETWORK_ID;
        pStaStateMachine->lastLinkedInfo.networkId = 0;
        pStaStateMachine->SetWifiLinkedInfo(INVALID_NETWORK_ID);
    }

    void SetWifiLinkedInfoSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).WillRepeatedly(Return(0));
        pStaStateMachine->linkedInfo.networkId = INVALID_NETWORK_ID;
        pStaStateMachine->lastLinkedInfo.networkId = INVALID_NETWORK_ID;
        pStaStateMachine->SetWifiLinkedInfo(0);
    }

    void DhcpResultNotifyOnSuccessTest()
    {
        pStaStateMachine->linkedInfo.detailedState = DetailedState::CONNECTED;
        pStaStateMachine->getIpSucNum = 1;
        pStaStateMachine->isRoam = false;
        IpInfo ipInfo;
        ipInfo.ipAddress = IpTools::ConvertIpv4Address("192.168.0.2");
        ipInfo.gateway = IpTools::ConvertIpv4Address("192.168.0.1");
        EXPECT_CALL(WifiSettings::GetInstance(), GetIpInfo(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(ipInfo), Return(0)));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetStrDnsBak()).Times(AtLeast(0));

        std::string ifname = "wlan0";
        DhcpResult result;
        result.strYourCli = "192.168.0.0";
        result.strRouter1 = "test";
        result.iptype = 0;
        pStaStateMachine->pDhcpResultNotify->OnSuccess(0, ifname, result);
    }

    void DhcpResultNotifyOnFailedTest1()
    {
        pStaStateMachine->linkedInfo.detailedState = DetailedState::CONNECTED;
        pStaStateMachine->currentTpType = IPTYPE_IPV6;
        pStaStateMachine->getIpSucNum = 0;
        pStaStateMachine->getIpFailNum = 1;
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        std::string ifname = "wlan0";
        std::string reason = "test";
        pStaStateMachine->pDhcpResultNotify->OnFailed(0, ifname, reason);
    }

    void DhcpResultNotifyOnFailedTest2()
    {
        pStaStateMachine->linkedInfo.detailedState = DetailedState::CONNECTED;
        pStaStateMachine->isRoam = false;
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Disconnect()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        std::string ifname = "wlan1";
        std::string reason = "test";
        pStaStateMachine->pDhcpResultNotify->OnFailed(0, ifname, reason);
    }

    void DhcpResultNotifyOnFailedTest3()
    {
        pStaStateMachine->linkedInfo.detailedState = DetailedState::CONNECTED;
        pStaStateMachine->isRoam = true;
        std::string ifname = "wlan1";
        std::string reason = "test";
        pStaStateMachine->pDhcpResultNotify->OnFailed(0, ifname, reason);
    }

    void DhcpResultNotifyOnSerExitNotify()
    {
        std::string ifname = "wlan0";
        pStaStateMachine->pDhcpResultNotify->OnSerExitNotify(ifname);
    }

    void SaveLinkstateSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_));
        pStaStateMachine->SaveLinkstate(ConnState::CONNECTED, DetailedState::CONNECTED);
    }

    void DisableNetworkSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), DisableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pStaStateMachine->DisableNetwork(0);
    }

    void DisableNetworkFail1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), DisableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->DisableNetwork(0);
    }

    void DisableNetworkFail2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), DisableNetwork(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pStaStateMachine->DisableNetwork(0);
    }
};

HWTEST_F(StaStateMachineTest, InitWifiLinkedInfoSuccess, TestSize.Level1)
{
    InitWifiLinkedInfoSuccess();
}

HWTEST_F(StaStateMachineTest, InitLastWifiLinkedInfoSuccess, TestSize.Level1)
{
    InitLastWifiLinkedInfoSuccess();
}

HWTEST_F(StaStateMachineTest, BuildStateTreeSuccess, TestSize.Level1)
{
    BuildStateTreeSuccess();
}

HWTEST_F(StaStateMachineTest, RegisterStaServiceCallbackSuccess, TestSize.Level1)
{
    RegisterStaServiceCallbackSuccess();
}

HWTEST_F(StaStateMachineTest, DealConnectTimeOutCmd, TestSize.Level1)
{
    DealConnectTimeOutCmd();
}

HWTEST_F(StaStateMachineTest, RootStateGoInStateSuccess, TestSize.Level1)
{
    RootStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, RootStateGoOutStateSuccess, TestSize.Level1)
{
    RootStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, RootStateExeMsgSuccess, TestSize.Level1)
{
    RootStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, RootStateExeMsgFail, TestSize.Level1)
{
    RootStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, InitStateGoInStateSuccess, TestSize.Level1)
{
    InitStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, InitStateGoOutStateSuccess, TestSize.Level1)
{
    InitStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, InitStateExeMsgSuccess, TestSize.Level1)
{
    InitStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, InitStateExeMsgFail1, TestSize.Level1)
{
    InitStateExeMsgFail1();
}

HWTEST_F(StaStateMachineTest, InitStateExeMsgFail2, TestSize.Level1)
{
    InitStateExeMsgFail2();
}

HWTEST_F(StaStateMachineTest, ConvertDeviceCfgSuccess, TestSize.Level1)
{
    ConvertDeviceCfgSuccess();
}

HWTEST_F(StaStateMachineTest, ConvertDeviceCfgFail1, TestSize.Level1)
{
    ConvertDeviceCfgFail1();
}

HWTEST_F(StaStateMachineTest, ConvertDeviceCfgFail2, TestSize.Level1)
{
    ConvertDeviceCfgFail2();
}

HWTEST_F(StaStateMachineTest, StartWifiProcessSuccess, TestSize.Level1)
{
    StartWifiProcessSuccess();
}

HWTEST_F(StaStateMachineTest, StartWifiProcessFail1, TestSize.Level1)
{
    StartWifiProcessFail1();
}

HWTEST_F(StaStateMachineTest, StartWifiProcessFail2, TestSize.Level1)
{
    StartWifiProcessFail2();
}

HWTEST_F(StaStateMachineTest, InitWpsSettingsSuccess, TestSize.Level1)
{
    InitWpsSettingsSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartingStateGoInStateSuccess, TestSize.Level1)
{
    WpaStartingStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartingStateGoOutStateSuccess, TestSize.Level1)
{
    WpaStartingStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartingStateExeMsgSuccess, TestSize.Level1)
{
    WpaStartingStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartingStateExeMsgFail1, TestSize.Level1)
{
    WpaStartingStateExeMsgFail1();
}

HWTEST_F(StaStateMachineTest, WpaStartingStateExeMsgFail2, TestSize.Level1)
{
    WpaStartingStateExeMsgFail2();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateGoInStateSuccess1, TestSize.Level1)
{
    WpaStartedStateGoInStateSuccess1();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateGoInStateSuccess2, TestSize.Level1)
{
    WpaStartedStateGoInStateSuccess2();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateGoOutStateSuccess, TestSize.Level1)
{
    WpaStartedStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateExeMsgSuccess, TestSize.Level1)
{
    WpaStartedStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateExeMsgFail1, TestSize.Level1)
{
    WpaStartedStateExeMsgFail1();
}

HWTEST_F(StaStateMachineTest, WpaStartedStateExeMsgFail2, TestSize.Level1)
{
    WpaStartedStateExeMsgFail2();
}

HWTEST_F(StaStateMachineTest, StopWifiProcessSuccess1, TestSize.Level1)
{
    StopWifiProcessSuccess1();
}

HWTEST_F(StaStateMachineTest, StopWifiProcessSuccess2, TestSize.Level1)
{
    StopWifiProcessSuccess2();
}

HWTEST_F(StaStateMachineTest, StopWifiProcessFail, TestSize.Level1)
{
    StopWifiProcessFail();
}

HWTEST_F(StaStateMachineTest, WpaStoppingStateGoInStateSuccess, TestSize.Level1)
{
    WpaStoppingStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStoppingStateGoOutStateSuccess, TestSize.Level1)
{
    WpaStoppingStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStoppingStateExeMsgSuccess, TestSize.Level1)
{
    WpaStoppingStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, WpaStoppingStateExeMsgFail, TestSize.Level1)
{
    WpaStoppingStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, LinkStateGoInStateSuccess, TestSize.Level1)
{
    LinkStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, LinkStateGoOutStateSuccess, TestSize.Level1)
{
    LinkStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, LinkStateExeMsgFail, TestSize.Level1)
{
    LinkStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, InitStaSMHandleMapSuccess, TestSize.Level1)
{
    InitStaSMHandleMapSuccess();
}

HWTEST_F(StaStateMachineTest, DealConnectToUserSelectedNetworkSuccess, TestSize.Level1)
{
    DealConnectToUserSelectedNetworkSuccess();
}

HWTEST_F(StaStateMachineTest, DealConnectToUserSelectedNetworkFail, TestSize.Level1)
{
    DealConnectToUserSelectedNetworkFail();
}

HWTEST_F(StaStateMachineTest, DealConnectTimeOutCmdSuccess, TestSize.Level1)
{
    DealConnectTimeOutCmdSuccess();
}

HWTEST_F(StaStateMachineTest, DealConnectTimeOutCmdFail, TestSize.Level1)
{
    DealConnectTimeOutCmdFail();
}

HWTEST_F(StaStateMachineTest, DealConnectionEventSuccess1, TestSize.Level1)
{
    DealConnectionEventSuccess1();
}

HWTEST_F(StaStateMachineTest, DealConnectionEventSuccess2, TestSize.Level1)
{
    DealConnectionEventSuccess2();
}

HWTEST_F(StaStateMachineTest, DealWpaWrongPskEventSuccess, TestSize.Level1)
{
    DealWpaWrongPskEventSuccess();
}

HWTEST_F(StaStateMachineTest, DealWpaWrongPskEventFail, TestSize.Level1)
{
    DealWpaWrongPskEventFail();
}

HWTEST_F(StaStateMachineTest, DealReassociateCmdSuccess, TestSize.Level1)
{
    DealReassociateCmdSuccess();
}

HWTEST_F(StaStateMachineTest, DealReassociateCmdFail1, TestSize.Level1)
{
    DealReassociateCmdFail1();
}

HWTEST_F(StaStateMachineTest, DealReassociateCmdFail2, TestSize.Level1)
{
    DealReassociateCmdFail2();
}

HWTEST_F(StaStateMachineTest, DealStartWpsCmdSuccess, TestSize.Level1)
{
    DealStartWpsCmdSuccess();
}

HWTEST_F(StaStateMachineTest, StartWpsModeSuccess1, TestSize.Level1)
{
    StartWpsModeSuccess1();
}

HWTEST_F(StaStateMachineTest, StartWpsModeSuccess2, TestSize.Level1)
{
    StartWpsModeSuccess2();
}

HWTEST_F(StaStateMachineTest, StartWpsModeSuccess3, TestSize.Level1)
{
    StartWpsModeSuccess3();
}

HWTEST_F(StaStateMachineTest, StartWpsModeSuccess4, TestSize.Level1)
{
    StartWpsModeSuccess4();
}

HWTEST_F(StaStateMachineTest, StartWpsModeFail1, TestSize.Level1)
{
    StartWpsModeFail1();
}

HWTEST_F(StaStateMachineTest, StartWpsModeFail2, TestSize.Level1)
{
    StartWpsModeFail2();
}

HWTEST_F(StaStateMachineTest, StartWpsModeFail3, TestSize.Level1)
{
    StartWpsModeFail3();
}

HWTEST_F(StaStateMachineTest, StartWpsModeFail4, TestSize.Level1)
{
    StartWpsModeFail4();
}

HWTEST_F(StaStateMachineTest, DealWpaBlockListClearEventSuccess, TestSize.Level1)
{
    DealWpaBlockListClearEventSuccess();
}

HWTEST_F(StaStateMachineTest, DealWpaBlockListClearEventFail, TestSize.Level1)
{
    DealWpaBlockListClearEventFail();
}

HWTEST_F(StaStateMachineTest, DealWpsConnectTimeOutEventSuccess, TestSize.Level1)
{
    DealWpsConnectTimeOutEventSuccess();
}

HWTEST_F(StaStateMachineTest, DealWpsConnectTimeOutEventFail, TestSize.Level1)
{
    DealWpsConnectTimeOutEventFail();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdSuccess1, TestSize.Level1)
{
    DealCancelWpsCmdSuccess1();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdSuccess2, TestSize.Level1)
{
    DealCancelWpsCmdSuccess2();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdSuccess3, TestSize.Level1)
{
    DealCancelWpsCmdSuccess3();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdFail1, TestSize.Level1)
{
    DealCancelWpsCmdFail1();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdFail2, TestSize.Level1)
{
    DealCancelWpsCmdFail2();
}

HWTEST_F(StaStateMachineTest, DealCancelWpsCmdFail3, TestSize.Level1)
{
    DealCancelWpsCmdFail3();
}

HWTEST_F(StaStateMachineTest, DealStartRoamCmdSuccess, TestSize.Level1)
{
    DealStartRoamCmdSuccess();
}

HWTEST_F(StaStateMachineTest, DealStartRoamCmdFail1, TestSize.Level1)
{
    DealStartRoamCmdFail1();
}

HWTEST_F(StaStateMachineTest, DealStartRoamCmdFail2, TestSize.Level1)
{
    DealStartRoamCmdFail2();
}

HWTEST_F(StaStateMachineTest, DealStartRoamCmdFail3, TestSize.Level1)
{
    DealStartRoamCmdFail3();
}

HWTEST_F(StaStateMachineTest, StartConnectToNetworkSuccess, TestSize.Level1)
{
    StartConnectToNetworkSuccess();
}

HWTEST_F(StaStateMachineTest, StartConnectToNetworkFail1, TestSize.Level1)
{
    StartConnectToNetworkFail1();
}

HWTEST_F(StaStateMachineTest, StartConnectToNetworkFail2, TestSize.Level1)
{
    StartConnectToNetworkFail2();
}

HWTEST_F(StaStateMachineTest, StartConnectToNetworkFali3, TestSize.Level1)
{
    StartConnectToNetworkFali3();
}

HWTEST_F(StaStateMachineTest, MacAddressGenerateSuccess, TestSize.Level1)
{
    MacAddressGenerateSuccess();
}

HWTEST_F(StaStateMachineTest, SetRandomMacSuccess1, TestSize.Level1)
{
    SetRandomMacSuccess1();
}

HWTEST_F(StaStateMachineTest, SetRandomMacFail1, TestSize.Level1)
{
    SetRandomMacFail1();
}

HWTEST_F(StaStateMachineTest, SetRandomMacFail2, TestSize.Level1)
{
    SetRandomMacFail2();
}

HWTEST_F(StaStateMachineTest, StartRoamToNetworkSuccess, TestSize.Level1)
{
    StartRoamToNetworkSuccess();
}

HWTEST_F(StaStateMachineTest, OnNetworkConnectionEventSuccess, TestSize.Level1)
{
    OnNetworkConnectionEventSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatingStateGoInStateSuccess, TestSize.Level1)
{
    SeparatingStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatingStateGoOutStateSuccess, TestSize.Level1)
{
    SeparatingStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatingStateExeMsgSuccess, TestSize.Level1)
{
    SeparatingStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatingStateExeMsgFail, TestSize.Level1)
{
    SeparatingStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, SeparatedStateGoInStateSuccess, TestSize.Level1)
{
    SeparatedStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatedStateGoOutStateSuccess, TestSize.Level1)
{
    SeparatedStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, SeparatedStateExeMsgSuccess1, TestSize.Level1)
{
    SeparatedStateExeMsgSuccess1();
}

HWTEST_F(StaStateMachineTest, SeparatedStateExeMsgSuccess2, TestSize.Level1)
{
    SeparatedStateExeMsgSuccess2();
}

HWTEST_F(StaStateMachineTest, SeparatedStateExeMsgFail, TestSize.Level1)
{
    SeparatedStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateGoInStateSuccess, TestSize.Level1)
{
    ApLinkedStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateGoOutStateSuccess, TestSize.Level1)
{
    ApLinkedStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateExeMsgSuccess1, TestSize.Level1)
{
    ApLinkedStateExeMsgSuccess1();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateExeMsgSuccess2, TestSize.Level1)
{
    ApLinkedStateExeMsgSuccess2();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateExeMsgFail1, TestSize.Level1)
{
    ApLinkedStateExeMsgFail1();
}

HWTEST_F(StaStateMachineTest, ApLinkedStateExeMsgFai2, TestSize.Level1)
{
    ApLinkedStateExeMsgFai2();
}

HWTEST_F(StaStateMachineTest, DisConnectProcessSuccess, TestSize.Level1)
{
    DisConnectProcessSuccess();
}

HWTEST_F(StaStateMachineTest, DisConnectProcessFail, TestSize.Level1)
{
    DisConnectProcessFail();
}

HWTEST_F(StaStateMachineTest, WpsStateGoInStateSuccess, TestSize.Level1)
{
    WpsStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpsStateGoOutStateSuccess, TestSize.Level1)
{
    WpsStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgSuccess1, TestSize.Level1)
{
    WpsStateExeMsgSuccess1();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgSuccess2, TestSize.Level1)
{
    WpsStateExeMsgSuccess2();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgSuccess3, TestSize.Level1)
{
    WpsStateExeMsgSuccess3();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgSuccess4, TestSize.Level1)
{
    WpsStateExeMsgSuccess4();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgSuccess5, TestSize.Level1)
{
    WpsStateExeMsgSuccess5();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgFail1, TestSize.Level1)
{
    WpsStateExeMsgFail1();
}

HWTEST_F(StaStateMachineTest, WpsStateExeMsgFail2, TestSize.Level1)
{
    WpsStateExeMsgFail2();
}

HWTEST_F(StaStateMachineTest, SyncAllDeviceConfigsSuccess, TestSize.Level1)
{
    SyncAllDeviceConfigsSuccess();
}

HWTEST_F(StaStateMachineTest, SyncAllDeviceConfigsFail, TestSize.Level1)
{
    SyncAllDeviceConfigsFail();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateGoInStateSuccess1, TestSize.Level1)
{
    GetIpStateStateGoInStateSuccess1();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateGoInStateSuccess2, TestSize.Level1)
{
    GetIpStateStateGoInStateSuccess2();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateGoInStateSuccess3, TestSize.Level1)
{
    GetIpStateStateGoInStateSuccess3();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateGoOutStateSuccess, TestSize.Level1)
{
    GetIpStateStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateExeMsgSuccess, TestSize.Level1)
{
    GetIpStateStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, GetIpStateStateExeMsgFail, TestSize.Level1)
{
    GetIpStateStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, ConfigStaticIpAddressSuccess1, TestSize.Level1)
{
    ConfigStaticIpAddressSuccess1();
}

HWTEST_F(StaStateMachineTest, ConfigStaticIpAddressSuccess2, TestSize.Level1)
{
    ConfigStaticIpAddressSuccess2();
}

HWTEST_F(StaStateMachineTest, ConfigStaticIpAddressSuccess3, TestSize.Level1)
{
    ConfigStaticIpAddressSuccess3();
}

HWTEST_F(StaStateMachineTest, ConfigStaticIpAddressFail, TestSize.Level1)
{
    ConfigStaticIpAddressFail();
}

HWTEST_F(StaStateMachineTest, HandleNetCheckResultSuccess1, TestSize.Level1)
{
    HandleNetCheckResultSuccess1();
}

HWTEST_F(StaStateMachineTest, HandleNetCheckResultSuccess3, TestSize.Level1)
{
    HandleNetCheckResultSuccess3();
}

HWTEST_F(StaStateMachineTest, HandleNetCheckResultFail, TestSize.Level1)
{
    HandleNetCheckResultFail();
}

HWTEST_F(StaStateMachineTest, LinkedStateGoInStateSuccess, TestSize.Level1)
{
    LinkedStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, LinkedStateGoOutStateSuccess, TestSize.Level1)
{
    LinkedStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, LinkedStateExeMsgSuccess, TestSize.Level1)
{
    LinkedStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, LinkedStateExeMsgFail, TestSize.Level1)
{
    LinkedStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, ApRoamingStateGoInStateSuccess, TestSize.Level1)
{
    ApRoamingStateGoInStateSuccess();
}

HWTEST_F(StaStateMachineTest, ApRoamingStateGoOutStateSuccess, TestSize.Level1)
{
    ApRoamingStateGoOutStateSuccess();
}

HWTEST_F(StaStateMachineTest, ApRoamingStateExeMsgSuccess, TestSize.Level1)
{
    ApRoamingStateExeMsgSuccess();
}

HWTEST_F(StaStateMachineTest, ApRoamingStateExeMsgFail, TestSize.Level1)
{
    ApRoamingStateExeMsgFail();
}

HWTEST_F(StaStateMachineTest, ConnectToNetworkProcessSuccess, TestSize.Level1)
{
    ConnectToNetworkProcessSuccess();
}

HWTEST_F(StaStateMachineTest, ConnectToNetworkProcessFail, TestSize.Level1)
{
    ConnectToNetworkProcessFail();
}

HWTEST_F(StaStateMachineTest, SetWifiLinkedInfoSuccess1, TestSize.Level1)
{
    SetWifiLinkedInfoSuccess1();
}

HWTEST_F(StaStateMachineTest, SetWifiLinkedInfoSuccess2, TestSize.Level1)
{
    SetWifiLinkedInfoSuccess2();
}

HWTEST_F(StaStateMachineTest, DhcpResultNotifyOnSuccessTest, TestSize.Level1)
{
    DhcpResultNotifyOnSuccessTest();
}

HWTEST_F(StaStateMachineTest, DhcpResultNotifyOnFailedTest1, TestSize.Level1)
{
    DhcpResultNotifyOnFailedTest1();
}

HWTEST_F(StaStateMachineTest, DhcpResultNotifyOnFailedTest2, TestSize.Level1)
{
    DhcpResultNotifyOnFailedTest2();
}

HWTEST_F(StaStateMachineTest, DhcpResultNotifyOnFailedTest3, TestSize.Level1)
{
    DhcpResultNotifyOnFailedTest3();
}

HWTEST_F(StaStateMachineTest, DhcpResultNotifyOnSerExitNotify, TestSize.Level1)
{
    DhcpResultNotifyOnSerExitNotify();
}

HWTEST_F(StaStateMachineTest, SaveLinkstateSuccess, TestSize.Level1)
{
    SaveLinkstateSuccess();
}

HWTEST_F(StaStateMachineTest, DisableNetworkSuccess, TestSize.Level1)
{
    DisableNetworkSuccess();
}

HWTEST_F(StaStateMachineTest, DisableNetworkFail1, TestSize.Level1)
{
    DisableNetworkFail1();
}

HWTEST_F(StaStateMachineTest, DisableNetworkFail2, TestSize.Level1)
{
    DisableNetworkFail2();
}
} // namespace Wifi
} // namespace OHOS