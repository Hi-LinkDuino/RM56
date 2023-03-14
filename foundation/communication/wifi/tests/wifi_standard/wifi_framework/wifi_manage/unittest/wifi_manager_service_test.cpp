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
#include "wifi_manager_service_test.h"
#include "wifi_config_center.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiManagerServiceTest, StaOpenResTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnStaOpenRes != nullptr);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_FAILED);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_OPENING);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_SUCCEED);
    WifiSettings::GetInstance().SetAirplaneModeState(1);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_SUCCEED);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_DISABLED);
    sleep(2);
}

HWTEST_F(WifiManagerServiceTest, StaCloseResTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnStaOpenRes != nullptr);
    ASSERT_TRUE(cbk.OnStaCloseRes != nullptr);
    cbk.OnStaOpenRes(OperateResState::OPEN_WIFI_SUCCEED);
    cbk.OnStaCloseRes(OperateResState::CLOSE_WIFI_FAILED);
    cbk.OnStaCloseRes(OperateResState::CLOSE_WIFI_CLOSING);
    cbk.OnStaCloseRes(OperateResState::CLOSE_WIFI_SUCCEED);
    WifiSettings::GetInstance().SetAirplaneModeState(1);
    WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::RUNNING);
    cbk.OnStaCloseRes(OperateResState::CLOSE_WIFI_SUCCEED);
    sleep(2);
}

HWTEST_F(WifiManagerServiceTest, StaConnChangedResTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnStaConnChanged != nullptr);

    WifiLinkedInfo info;
    cbk.OnStaConnChanged(OperateResState::CONNECT_CONNECTING, info);
    cbk.OnStaConnChanged(OperateResState::CONNECT_AP_CONNECTED, info);
    cbk.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTING, info);
    cbk.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, info);
    sleep(2);
}

HWTEST_F(WifiManagerServiceTest, StaWpsChangedResTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnWpsChanged != nullptr);
    cbk.OnWpsChanged(WpsStartState::START_PIN_SUCCEED, 1);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, DealStreamChangedTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnStaStreamChanged  != nullptr);

    cbk.OnStaStreamChanged(StreamDirection::STREAM_DIRECTION_UP);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, DealRssiChangedTest, TestSize.Level1)
{
    StaServiceCallback cbk = WifiManager::GetInstance().GetStaCallback();
    ASSERT_TRUE(cbk.OnStaRssiLevelChanged   != nullptr);

    cbk.OnStaRssiLevelChanged(1);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ScanOpenResTest, TestSize.Level1)
{
    IScanSerivceCallbacks cbk = WifiManager::GetInstance().GetScanCallback();
    ASSERT_TRUE(cbk.OnScanStartEvent != nullptr);

    cbk.OnScanStartEvent();
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ScanCloseResTest, TestSize.Level1)
{
    IScanSerivceCallbacks cbk = WifiManager::GetInstance().GetScanCallback();
    ASSERT_TRUE(cbk.OnScanStopEvent != nullptr);

    cbk.OnScanStopEvent();
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ScanFinishedResTest, TestSize.Level1)
{
    IScanSerivceCallbacks cbk = WifiManager::GetInstance().GetScanCallback();
    ASSERT_TRUE(cbk.OnScanFinishEvent != nullptr);

    cbk.OnScanFinishEvent(1);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ScanInfoNotifyResTest, TestSize.Level1)
{
    IScanSerivceCallbacks cbk = WifiManager::GetInstance().GetScanCallback();
    ASSERT_TRUE(cbk.OnScanInfoEvent != nullptr);

    std::vector<InterScanInfo> results;
    cbk.OnScanInfoEvent(results);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ApStateResTest, TestSize.Level1)
{
    IApServiceCallbacks cbk = WifiManager::GetInstance().GetApCallback();
    ASSERT_TRUE(cbk.OnApStateChangedEvent != nullptr);

    cbk.OnApStateChangedEvent(ApState::AP_STATE_STARTING);
    cbk.OnApStateChangedEvent(ApState::AP_STATE_STARTED);
    cbk.OnApStateChangedEvent(ApState::AP_STATE_CLOSING);
    cbk.OnApStateChangedEvent(ApState::AP_STATE_CLOSED);
    cbk.OnApStateChangedEvent(ApState::AP_STATE_IDLE);
    cbk.OnApStateChangedEvent(ApState::AP_STATE_NONE);
    sleep(2);
}

HWTEST_F(WifiManagerServiceTest, ApJoinResTest, TestSize.Level1)
{
    IApServiceCallbacks cbk = WifiManager::GetInstance().GetApCallback();
    ASSERT_TRUE(cbk.OnHotspotStaJoinEvent != nullptr);

    StationInfo info;
    cbk.OnHotspotStaJoinEvent(info);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, ApLeaveResTest, TestSize.Level1)
{
    IApServiceCallbacks cbk = WifiManager::GetInstance().GetApCallback();
    ASSERT_TRUE(cbk.OnHotspotStaLeaveEvent != nullptr);

    StationInfo info;
    cbk.OnHotspotStaLeaveEvent(info);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pStateChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pStateChangedEvent != nullptr);

    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_STARTING);
    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_STARTED);
    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_CLOSING);
    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_CLOSED);
    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_IDLE);
    cbk.OnP2pStateChangedEvent(P2pState::P2P_STATE_NONE);
    sleep(2);
}

HWTEST_F(WifiManagerServiceTest, P2pPeersChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pPeersChangedEvent != nullptr);

    std::vector<WifiP2pDevice> peers;
    cbk.OnP2pPeersChangedEvent(peers);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pServiceChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pServicesChangedEvent != nullptr);

    std::vector<WifiP2pServiceInfo> services;
    cbk.OnP2pServicesChangedEvent(services);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pConnectionChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pConnectionChangedEvent != nullptr);

    WifiP2pLinkedInfo info;
    cbk.OnP2pConnectionChangedEvent(info);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pDeviceChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pThisDeviceChangedEvent != nullptr);

    WifiP2pDevice info;
    cbk.OnP2pThisDeviceChangedEvent(info);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pDiscoveryChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pDiscoveryChangedEvent != nullptr);

    cbk.OnP2pDiscoveryChangedEvent(true);
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pGroupsChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pGroupsChangedEvent != nullptr);

    cbk.OnP2pGroupsChangedEvent();
    sleep(1);
}

HWTEST_F(WifiManagerServiceTest, P2pActionChangeTest, TestSize.Level1)
{
    IP2pServiceCallbacks cbk = WifiManager::GetInstance().GetP2pCallback();
    ASSERT_TRUE(cbk.OnP2pActionResultEvent != nullptr);

    cbk.OnP2pActionResultEvent(P2pActionCallback::DiscoverDevices, WIFI_OPT_SUCCESS);
    sleep(1);
}
}  // namespace Wifi
}  // namespace OHOS