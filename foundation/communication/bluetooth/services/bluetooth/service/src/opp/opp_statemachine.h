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

#ifndef OPP_STATEMACHINE_H
#define OPP_STATEMACHINE_H
#include <string>
#include "../obex/obex_client.h"
#include "opp_defines.h"
#include "opp_gap_client.h"
#include "opp_message.h"
#include "opp_sdp_client.h"
#include "state_machine.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for opp state machine.
 */
class OppStateMachine : public utility::StateMachine {
public:
    /**
     * @brief Construct a new OppStateMachine object.
     *
     * @param address Device address.
     */
    explicit OppStateMachine(const std::string &address);

    /**
     * @brief Destroy the OppStateMachine object.
     */
    ~OppStateMachine() = default;

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

    void ConnectionTimeout() const;
    void StartConnectionTimer() const;
    void StopConnectionTimer() const;
    void DisonnectionTimeout() const;
    void StartDisconnectionTimer() const;
    void StopDisconnectionTimer() const;

    std::string GetDeviceAdress();
    static std::string GetEventName(int what);
    void NotifyStateTransitions();

    void ProcessConnectReqEvent(const OppMessage &msg);
    void ProcessDisconnectReqEvent(const OppMessage &msg);
    void ProcessSdpCompleteEvent(const OppMessage &msg);
    void ProcessGapCompleteEvent(const OppMessage &msg);
    void ProcessConnectedEvent(const OppMessage &msg);
    void ProcessDisconnectedEvent(const OppMessage &msg);

    inline static const std::string DISCONNECTED = "Disconnected";
    inline static const std::string CONNECTING = "Connecting";
    inline static const std::string DISCONNECTING = "Disconnecting";
    inline static const std::string CONNECTED = "Connected";

private:
    std::string address_;
    ObexClientConfig obexConfig_ {};
    bool isRemoving_ {false};
    int preState_ {0};
    std::unique_ptr<OppSdpClient> sdpClient_ {nullptr};
    std::unique_ptr<OppGapClient> gapClient_ {nullptr};
    std::unique_ptr<utility::Timer> connTimer_ {nullptr};
    std::unique_ptr<utility::Timer> disconnTimer_ {nullptr};
    inline static const int connectionTimeoutMs {60000};
    inline static const int disconnectionTimeoutMs {60000};
    BT_DISALLOW_COPY_AND_ASSIGN(OppStateMachine);
};

class OppState : public utility::StateMachine::State {
public:
    OppState(const std::string &name, utility::StateMachine &statemachine, int stateInt,
        utility::StateMachine::State &parent)
        : State(name, statemachine, parent), stateInt_(stateInt), stateMachine_((OppStateMachine &)statemachine)
    {}

    OppState(const std::string &name, utility::StateMachine &statemachine, int stateInt)
        : State(name, statemachine), stateInt_(stateInt), stateMachine_((OppStateMachine &)statemachine)
    {}

    virtual ~OppState()
    {}
    int GetStateInt() const
    {
        return stateInt_;
    }

protected:
    int stateInt_ {OPP_STATE_DISCONNECTED};
    OppStateMachine &stateMachine_;
};

class OppDisconnectedState : public OppState {
public:
    OppDisconnectedState(const std::string &name, utility::StateMachine &statemachine)
        : OppState(name, statemachine, OPP_STATE_DISCONNECTED)
    {}
    ~OppDisconnectedState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;

private:
    bool isReentry_ {false};
};

class OppConnectingState : public OppState {
public:
    OppConnectingState(const std::string &name, utility::StateMachine &statemachine)
        : OppState(name, statemachine, OPP_STATE_CONNECTING)
    {}
    ~OppConnectingState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class OppDisconnectingState : public OppState {
public:
    OppDisconnectingState(const std::string &name, utility::StateMachine &statemachine)
        : OppState(name, statemachine, OPP_STATE_DISCONNECTING)
    {}
    ~OppDisconnectingState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};

class OppConnectedState : public OppState {
public:
    OppConnectedState(const std::string &name, utility::StateMachine &statemachine)
        : OppState(name, statemachine, OPP_STATE_CONNECTED)
    {}
    ~OppConnectedState() override = default;
    void Entry() override;
    void Exit() override;
    bool Dispatch(const utility::Message &msg) override;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_STATEMACHINE_H
