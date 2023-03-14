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
#ifndef OHOS_P2P_PROVISION_DISCOVERY_STATE_H
#define OHOS_P2P_PROVISION_DISCOVERY_STATE_H

#include "state.h"
#include "p2p_define.h"
#include "wifi_p2p_group_manager.h"
#include "wifi_p2p_device_manager.h"

namespace OHOS {
namespace Wifi {
class P2pStateMachine;
class ProvisionDiscoveryState : public State {
    FRIEND_GTEST(ProvisionDiscoveryState);

public:
    /**
     * @Description Construct a new Provision Discovery State object
     * @param None
     * @return None
     */
    ProvisionDiscoveryState(P2pStateMachine &stateMachine, WifiP2pGroupManager &setGroupMgr,
        WifiP2pDeviceManager &setDeviceMgr);

    /**
     * @Description Destroy the Provision Discovery State object
     * @param None
     * @return None
     */
    ~ProvisionDiscoveryState() = default;

    /**
     * @Description - Called when entering state
     * @param None
     * @return None
     */
    virtual void GoInState() override;

    /**
     * @Description - Called when exiting state
     * @param None
     * @return None
     */
    virtual void GoOutState() override;

    /**
     * @Description - Message Processing Function
     * @param msg - Message object pointer
     * @return - bool true:success   false:fail
     */
    virtual bool ExecuteStateMsg(InternalMessage *msg) override;

private:
    /**
     * @Description Initialization
     * @param None
     * @return None
     */
    virtual void Init();

    /**
     * @Description Process the discover peer command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDiscoverPeer(InternalMessage &msg) const;

    /**
     * @Description Process the discover services command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDiscServices(InternalMessage &msg) const;

    /**
     * @Description Process the start listen command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdStartListen(InternalMessage &msg) const;

    /**
     * @Description Process the provision discover pbc response message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessProvDiscPbcRspEvt(InternalMessage &msg) const;

    /**
     * @Description Process the provision discover enter pin message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessProvDiscEnterPinEvt(InternalMessage &msg) const;

    /**
     * @Description Process the provision discover show pin message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessProvDiscShowPinEvt(InternalMessage &msg) const;

    /**
     * @Description Process the provision discover failure message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessProvDiscFailEvt(InternalMessage &msg) const;

    /**
     * @Description Process the cancel connect message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdCancelConnect(InternalMessage &msg) const;

private:
    using ProcessFun = bool (ProvisionDiscoveryState::*)(InternalMessage &msg) const;
    std::map<P2P_STATE_MACHINE_CMD, ProcessFun> mProcessFunMap;
    P2pStateMachine &p2pStateMachine;
    WifiP2pGroupManager &groupManager;
    WifiP2pDeviceManager &deviceManager;
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_P2P_PROVISION_DISCOVERY_STATE_H */
