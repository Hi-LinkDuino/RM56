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
#ifndef OHOS_P2P_ENABLED_STATE_H
#define OHOS_P2P_ENABLED_STATE_H

#include "state.h"
#include "p2p_define.h"
#include "p2p_macro.h"
#include "wifi_p2p_group_manager.h"
#include "wifi_p2p_device_manager.h"

namespace OHOS {
namespace Wifi {
class P2pStateMachine;
class P2pEnabledState : public State {
    FRIEND_GTEST(P2pEnabledState);

public:
    /**
     * @Description Construct a new P2pEnabledState object
     * @param None
     * @return None
     */
    P2pEnabledState(P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr);

    /**
     * @Description Destroy the P2pEnabledState object
     * @param None
     * @return None
     */
    ~P2pEnabledState() = default;

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
     * @Description - Initializing P2P Information
     * @param None
     * @return - bool true:success   false:fail
     */
    virtual bool P2pSettingsInitialization();

    /**
     * @Description - Initializing P2P Configuration
     * @param None
     * @return - bool true:success   false:fail
     */
    virtual bool P2pConfigInitialization();

    /**
     * @Description Initialization
     * @param None
     * @return None
     */
    virtual void Init();

    /**
     * @Description Process the disable command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDisable(InternalMessage &msg) const;

    /**
     * @Description Process the start listen command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdStartListen(InternalMessage &msg) const;

    /**
     * @Description Process the stop listen command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdStopListen(InternalMessage &msg) const;

    /**
     * @Description Process the discover peer command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDiscPeer(InternalMessage &msg) const;

    /**
     * @Description Process the stop discover peer command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdStopDiscPeer(InternalMessage &msg) const;

    /**
     * @Description Process the device found message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessDeviceFoundEvt(InternalMessage &msg) const;

    /**
     * @Description Process the device lost message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessDeviceLostEvt(InternalMessage &msg) const;

    /**
     * @Description Process the find stopped message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessFindStoppedEvt(InternalMessage &msg) const;

    /**
     * @Description Process the delete group command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDeleteGroup(InternalMessage &msg) const;

    /**
     * @Description Process the add local service command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdAddLocalService(InternalMessage &msg) const;

    /**
     * @Description Process the delete local service command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDelLocalService(InternalMessage &msg) const;

    /**
     * @Description Process the discover services command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdDiscServices(InternalMessage &msg) const;

    /**
     * @Description Process the stop discover services command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdStopDiscServices(InternalMessage &msg) const;

    /**
     * @Description Process the request service command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdRequestService(InternalMessage &msg) const;

    /**
     * @Description Process the service discover request message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessServiceDiscReqEvt(InternalMessage &msg) const;

    /**
     * @Description Process the service discover response message received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessServiceDiscRspEvt(InternalMessage &msg) const;

    /**
     * @Description Process the exception timeout message received by the state machine.
     * @param msg - Message body sent by the state machine
     * @return true - handle
     * @return false - not handle
     */
    virtual bool ProcessExceptionTimeOut(InternalMessage &msg) const;

    /**
     * @Description Process the set device name command received by the state machine.
     *
     * @param msg - Message body sent by the state machine
     * @return true - handle
     * @return false - not handle
     */
    virtual bool ProcessCmdSetDeviceName(InternalMessage &msg) const;

    /**
     * @Description Process the set WFD Info command received by the state machine.
     *
     * @param msg - Message body sent by the state machine
     * @return true - handle
     * @return false - not handle
     */
    virtual bool ProcessCmdSetWfdInfo(InternalMessage &msg) const;

    /**
     * @Description Process the cancel connect command received by the state machine
     * @param msg - Message body sent by the state machine
     * @return - bool true:handle   false:not handle
     */
    virtual bool ProcessCmdCancelConnect(InternalMessage &msg) const;

private:
    using ProcessFun = bool (P2pEnabledState::*)(InternalMessage &msg) const;
    std::map<P2P_STATE_MACHINE_CMD, ProcessFun> mProcessFunMap;
    P2pStateMachine &p2pStateMachine;
    WifiP2pGroupManager &groupManager;
    WifiP2pDeviceManager &deviceManager;
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_P2P_ENABLED_STATE_H */
