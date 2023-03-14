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

#ifndef OHOS_WIFI_COMMON_EVENT_HELPER_H
#define OHOS_WIFI_COMMON_EVENT_HELPER_H

#include <string>
#include "common_event_subscriber.h"

namespace OHOS {
namespace Wifi {
const std::string COMMON_EVENT_WIFI_POWER_STATE = "usual.event.wifi.POWER_STATE";
const std::string COMMON_EVENT_WIFI_SCAN_FINISHED = "usual.event.wifi.SCAN_FINISHED";
const std::string COMMON_EVENT_WIFI_SCAN_STATE = "usual.event.wifi.SCAN_STATE";
const std::string COMMON_EVENT_WIFI_RSSI_VALUE = "usual.event.wifi.RSSI_VALUE";
const std::string COMMON_EVENT_WIFI_CONN_STATE = "usual.event.wifi.CONN_STATE";
const std::string COMMON_EVENT_WIFI_HOTSPOT_STATE = "usual.event.wifi.HOTSPOT_STATE";
const std::string COMMON_EVENT_WIFI_AP_STA_JOIN = "usual.event.wifi.WIFI_HS_STA_JOIN";
const std::string COMMON_EVENT_WIFI_AP_STA_LEAVE = "usual.event.wifi.WIFI_HS_STA_LEAVE";
const std::string COMMON_EVENT_WIFI_MPLINK_STATE = "usual.event.wifi.mplink.STATE_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_CONN_STATE = "usual.event.wifi.p2p.CONN_STATE_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_STATE_CHANGED = "usual.event.wifi.p2p.STATE_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_PEERS_STATE_CHANGED = "usual.event.wifi.p2p.DEVICES_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_PEERS_DISCOVERY_STATE_CHANGED =
    "usual.event.wifi.p2p.PEER_DISCOVERY_STATE_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_CURRENT_DEVICE_STATE_CHANGED = "usual.event.wifi.p2p.CURRENT_DEVICE_CHANGE";
const std::string COMMON_EVENT_WIFI_P2P_GROUP_STATE_CHANGED = "usual.event.wifi.p2p.GROUP_STATE_CHANGED";
class WifiCommonEventHelper {
public:
    static bool PublishEvent(const std::string &eventAction, const int &code, const std::string &data,
        const std::vector<std::string> &permissions);

    static bool PublishEvent(const std::string &eventAction, const int &code, const std::string &data);
    static bool PublishPowerStateChangeEvent(const int &code, const std::string &data);
    static bool PublishScanFinishedEvent(const int &code, const std::string &data);
    static bool PublishScanStateChangedEvent(const int &code, const std::string &data);
    static bool PublishRssiValueChangedEvent(const int &code, const std::string &data);
    static bool PublishConnStateChangedEvent(const int &code, const std::string &data);
    static bool PublishHotspotStateChangedEvent(const int &code, const std::string &data);
    static bool PublishApStaJoinEvent(const int &code, const std::string &data);
    static bool PublishApStaLeaveEvent(const int &code, const std::string &data);
    static bool PublishMPlinkEvent(const int &code, const std::string &data);
    static bool PublishP2pStateChangedEvent(const int &code, const std::string &data);
    static bool PublishP2pConnStateEvent(const int &code, const std::string &data);
    static bool PublishP2pPeersStateChangedEvent(const int &code, const std::string &data);
    static bool PublishP2pDicoveryStateChangedEvent(const int &code, const std::string &data);
    static bool PublishP2pCurrentDeviceStateChangedEvent(const int &code, const std::string &data);
    static bool PublishP2pGroupStateChangedEvent(const int &code, const std::string &data);
};
}  // namespace Wifi
}  // namespace OHOS
#endif
