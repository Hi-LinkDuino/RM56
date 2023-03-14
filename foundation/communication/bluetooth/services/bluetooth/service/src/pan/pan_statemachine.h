/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef PAN_STATEMACHINE_H
#define PAN_STATEMACHINE_H
#include <list>
#include <memory>
#include <string>

#include "interface_adapter_manager.h"
#include "interface_adapter_classic.h"
#include "interface_adapter_ble.h"
#include "state_machine.h"
#include "pan_bnep.h"
#include "pan_defines.h"
#include "pan_network.h"
#include "timer.h"
#include "pan_message.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for pan state machine.
 */
class PanStateMachine : public utility::StateMachine {
public:
    /**
     * @brief Construct a new PanStateMachine object.
     *
     * @param address Device address.
     */
    explicit PanStateMachine(const std::string &address);

    /**
     * @brief Destroy the PanStateMachine object.
     */
    ~PanStateMachine() = default;

    /**
     * @brief Initialise the state machine.
     */
    void Init();

    /**
     * @brief Check if current statemachine is removing.
     *
     * @return Returns <b>true</b> if the statemachine is removing; returns <b>false</b> if not.
     */
    bool IsRemoving() const;

    /**
     * @brief Mark statemachine removing.
     *
     * @param isRemoving removing mark.
     */
    void SetRemoving(bool isRemoving);

    /**
     * @brief Get the State Int object.
     *
     * @return Returns the state number.
     */
    int GetDeviceStateInt() const;

    uint16_t GetDeviceLcid();

    int GetDeviceType();

    void ProcessL2capConnectionEvent(const PanMessage &event);

    void ConnectionTimeout() const;
    void StartConnectionTimer() const;
    void StopConnectionTimer() const;
    void DisonnectionTimeout() const;
    void StartDisconnectionTimer() const;
    void StopDisconnectionTimer() const;
    void AddDeferredMessage(const PanMessage &msg);
    void ProcessDeferredMessage();
    std::string GetDeviceAdress();
    void NotifyStateTransitions();
    static std::string GetEventName(int what);

    void ProcessCloseReqEvent(const PanMessage &msg);
    void ProcessOpenEvent(const PanMessage &msg);
    void ProcessCloseEvent(const PanMessage &msg);
    void ProcessReciveData(const PanMessage &msg);
    void ProcessSendData(const PanMessage &msg);
    void ProcessOpenComplete(const PanMessage &msg);
    void setTethering(const bool enable);

    inline static const std::string DISCONNECTED = "Disconnected";
    inline static const std::string CONNECTING = "Connecting";
    inline static const std::string DISCONNECTING = "Disconnecting";
    inline static const std::string CONNECTED = "Connected";

private:
    std::string address_;
    bool isRemoving_ {false};
    int preState_ {0};
    std::list<PanMessage> deferMsgs_ {};
    std::unique_ptr<utility::Timer> connTimer_ {nullptr};
    std::unique_ptr<utility::Timer> disconnTimer_ {nullptr};
    PanBnep panBnep_;
    inline static const int CONNECTION_TIMEOUT_MS {60000};
    inline static const int DISCONNECTION_TIMEOUT_MS {60000};
    BT_DISALLOW_COPY_AND_ASSIGN(PanStateMachine);
};

class PanState : public utility::StateMachine::State {
public:
    PanState(const std::string &name, utility::StateMachine &statemachine, int stateInt,
        utility::StateMachine::State &parent)
        : State(name, statemachine, parent), stateInt_(stateInt), stateMachine_((PanStateMachine &)statemachine)
    {}

    PanState(const std::string &name, utility::StateMachine &statemachine, int stateInt)
        : State(name, statemachine), stateInt_(stateInt), stateMachine_((PanStateMachine &)statemachine)
    {}

    virtual ~PanState()
    {}
    int GetStateInt() const
    {
        return stateInt_;
    }

protected:
    int stateInt_ {PAN_STATE_DISCONNECTED};
    PanStateMachine &stateMachine_;
};

class PanDisconnectedState : public PanState {
public:
    PanDisconnectedState(const std::string &name, utility::StateMachine &statemachine)
        : PanState(name, statemachine, PAN_STATE_DISCONNECTED)
    {}
    ~PanDisconnectedState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
    bool isReentry_ {false};
};

class PanConnectingState : public PanState {
public:
    PanConnectingState(const std::string &name, utility::StateMachine &statemachine)
        : PanState(name, statemachine, PAN_STATE_CONNECTING)
    {}
    ~PanConnectingState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
};

class PanDisconnectingState : public PanState {
public:
    PanDisconnectingState(const std::string &name, utility::StateMachine &statemachine)
        : PanState(name, statemachine, PAN_STATE_DISCONNECTING)
    {}
    ~PanDisconnectingState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
};

class PanConnectedState : public PanState {
public:
    PanConnectedState(const std::string &name, utility::StateMachine &statemachine)
        : PanState(name, statemachine, PAN_STATE_CONNECTED)
    {}
    ~PanConnectedState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // Pan_STATEMACHINE_H
