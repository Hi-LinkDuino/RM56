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

#ifndef OHOS_AP_MONITOR_H
#define OHOS_AP_MONITOR_H

#include <set>
#include <map>

#include "wifi_ap_msg.h"
#include "ap_define.h"
#include "wifi_idl_struct.h"

namespace OHOS {
namespace Wifi {
class ApMonitor {
    FRIEND_GTEST(ApMonitor);

public:
    ApMonitor();
    virtual ~ApMonitor();
    /**
     * @Description  IDL called this interface when STA connected or
                     disconnected, report to state machine.
     * @param cbInfo - structure stored STA infos, only MAC and action.
     * @return None
     */
    virtual void OnStaJoinOrLeave(const WifiApConnectionNofify &cbInfo);

    /* @Description  IDL called this interface when STA connected or
                     disconnected, report to state machine.
     * @param staInfo - station information
     * @param event - event STA connected or disconnected.
     * @return None
     */
    virtual void DealStaJoinOrLeave(const StationInfo &info, ApStatemachineEvent event);
    /**
     * @Description  Asynchronously notifies the hostapd of the enable and disable status.
     * @param state - hostapd status
     * @return None
     */
    virtual void OnHotspotStateEvent(int state) const;
    /**
     * @Description  start monitor.
     * @param None
     * @return None
     */
    virtual void StartMonitor();
    /**
     * @Description  close monitor.
     * @param None
     * @return None
     */
    virtual void StopMonitor();

    /**
     * @Description  Register monitor callback handler.
     * @param iface - use interface
     * @param handler - handler callback list
     * @return None
     */
    virtual void RegisterHandler(const std::string &iface, const std::function<HandlerMethod> &handler);

    /**
     * @Description  Unregister monitor callback handler.
     * @param iface - use interface
     * @return None
     */
    virtual void UnregisterHandler(const std::string &iface);

    /**
     * @Description  SendMessage to the state machine.
     * @param iface - use interface
     * @param msgName - state machine event
     * @param param1 - Parameter
     * @param param2 - Parameter
     * @param messageObj - Any message object
     * @return None
     */
    virtual void SendMessage(const std::string &iface, ApStatemachineEvent msgName, int param1, int param2,
        const std::any &messageObj) const;

private:
    DISALLOW_COPY_AND_ASSIGN(ApMonitor)

private:
    std::map<std::string, std::function<HandlerMethod>> m_mapHandler;
    std::string m_selectIfacName;
    std::set<std::string> m_setMonitorIface;
};
}  // namespace Wifi
}  // namespace OHOS

#endif