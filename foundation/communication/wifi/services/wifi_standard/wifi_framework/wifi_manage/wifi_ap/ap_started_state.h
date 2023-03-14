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
#ifndef OHOS_AP_STARTEND_STATE_H
#define OHOS_AP_STARTEND_STATE_H

#include <map>
#include "ap_define.h"
#include "state.h"
#include "wifi_ap_nat_manager.h"
#include "wifi_ap_msg.h"

namespace OHOS {
namespace Wifi {
class ApStateMachine;
class ApConfigUse;
class ApMonitor;
class ApStartedState : public State {
    FRIEND_GTEST(ApStartedState);
public:
    /**
     * @Description  construction method.
     * @param None
     * @return None
     */
    ApStartedState(ApStateMachine &, ApConfigUse &, ApMonitor &);
    /**
     * @Description  destructor method.
     * @param None
     * @return None
     */
    virtual ~ApStartedState();

public:
    /**
     * @Description  Implement pure base class methods:Called when entering
                     the state，When receiving the start AP，Switch to the
                     current state by ApIdleState and call GoInState to initialize
                     hot spots(Start the wifi driver，Set hostapd to start
                     hotspot mode，Set blocklist, etc.)
     * @param None
     * @return None
     */
    virtual void GoInState() override;
    /**
     * @Description  Implement pure base class methods:Called when exiting
                     the state，Called when the hotspot is closed or the
                     abnormal error returns to ApIdleState Turn off WIFI
                     driver, disable hostapd, etc.
     * @param None
     * @return None
     */
    virtual void GoOutState() override;
    /**
     * @Description  Implement pure base class methods:The CMD processed
                     when the AP is in the running state (such as updating
                     the blocklist to hostapd.)
     * @param msg - processed message
     * @return HANDLED：Processed successfully    NOT_EXECUTED: Processed failed
     */
    virtual bool ExecuteStateMsg(InternalMessage *msg) override;

private:
    /**
     * @Description  Called inside the state，The processing function of
                     the HAL layer when the AP is turned on.
     * @param None
     * @return true: Successfully opened    false: Failed to open
     */
    bool StartAp() const;

    /**
     * @Description  Called inside the state，The processing function of
                     the HAL layer when the AP is turned off.
     * @param None
     * @return true: Closed successfully    false: Close failed
     */
    bool StopAp() const;

    /**
     * @Description  Called inside the state，processing function
                     configured.
     * @param apConfig - Hotspot Configure
     * @return true: Set successfully    false: Set failed
     */
    bool SetConfig(HotspotConfig &apConfig);

    /**
     * @Description  Called inside the state，processing function
                     configured when the AP is turned on.
     * @param None
     * @return true: Set successfully    false: Set failed
     */
    bool SetConfig();

    /**
     * @Description  Status update notification APSERVICE.
     * @param state - New state
     * @return None
     */
    void OnApStateChange(const ApState &state) const;

    /**
     * @Description  Start ap monitor.
     * @param None
     * @return None
     */
    void StartMonitor() const;

    /**
     * @Description  Stop ap monitor.
     * @param None
     * @return None
     */
    void StopMonitor() const;

    /**
     * @Description  start NAT
     * @param None
     * @return true: success    false: failed
     */
    bool EnableInterfaceNat() const;

    /**
     * @Description  stop NAT.
     * @param None
     * @return true: success    false: failed
     */
    bool DisableInterfaceNat() const;

private:
    /**
     * @Description  Handle hostapd failure events received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdFail(InternalMessage &msg) const;

    /**
     * @Description  Process the STA connection message received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdStationJoin(InternalMessage &msg) const;

    /**
     * @Description  Process the STA disconnect message received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdStationLeave(InternalMessage &msg) const;

    /**
     * @Description  Process the hotspot configuration message of the APP
                     received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdSetHotspotConfig(InternalMessage &msg);

    /**
     * @Description  Process the hotspot configuration update result
                     received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdUpdateConfigResult(InternalMessage &msg) const;

    /**
     * @Description  Process the add blocklist message received by the
                     state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdAddBlockList(InternalMessage &msg) const;

    /**
     * @Description  Process the delete blocklist message received by the
                     state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdDelBlockList(InternalMessage &msg) const;

    /**
     * @Description  Process the close hotspot message received by the
                     state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdStopHotspot(InternalMessage &msg) const;

    /**
     * @Description  Process the disconnected STA message received by the
                     state machine.
     * @param msg - Message body sent by the state machine
     * @return None
     */
    void ProcessCmdDisconnectStation(InternalMessage &msg) const;

    /**
     * @Description  Initialization.
     * @param None
     * @return None
     */
    void Init();

private:
    HotspotConfig m_hotspotConfig; /*
                                    * Store the configuration when set to
                                    * hostapd, hostapd will asynchronously
                                    * notify the setting result
                                    */
    WifiApNatManager mApNatManager;
    typedef void (ApStartedState::*ProcessFun)(InternalMessage &msg) const;
    std::map<ApStatemachineEvent, ProcessFun> mProcessFunMap; /*
                                                               * Message processing function map
                                                               * of the state machine
                                                               */
    ApStateMachine &m_ApStateMachine;
    ApConfigUse &m_ApConfigUse;
    ApMonitor &m_ApMonitor;
};
}  // namespace Wifi
}  // namespace OHOS
#endif