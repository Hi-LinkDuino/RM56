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

#ifndef A2DP_STATE_MACHINE_H
#define A2DP_STATE_MACHINE_H

#include <cstdint>
#include <memory>
#include <string>

#include "a2dp_avdtp.h"
#include "a2dp_def.h"
#include "bt_def.h"
#include "btstack.h"
#include "message.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
const std::string A2DP_PROFILE_IDLE = "A2dpStateIdle";
const std::string A2DP_PROFILE_CONFIG = "A2dpStateConfigure";
const std::string A2DP_PROFILE_OPENING = "A2dpStateOpening";
const std::string A2DP_PROFILE_OPEN = "A2dpStateOpen";
const std::string A2DP_PROFILE_STREAMING = "A2dpStateStreaming";
const std::string A2DP_PROFILE_CLOSING = "A2dpStateClosing";

class A2dpStateIdle : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a Idle State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateIdle(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Idle State object.
     * @since 6.0
     */
    ~A2dpStateIdle() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry();

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit()
    {}

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the result of signaling connect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @param[in] errCode The error code of signaling connect
     * @since 6.0
     */
    void ProcessDiscoverReq(BtAddr addr, uint8_t role, uint8_t errCode);

    /**
     * @brief Process the failure of SDP.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSDPFailure(BtAddr addr, uint8_t role) const;

    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the requirement of disconnect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectReq(BtAddr addr, uint8_t role);

    /**
     * @brief Process the timeout.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessTimeout(BtAddr addr, uint8_t role);

    /**
     * @brief Process the Indication of configure.
     * @param[in] msgData The data of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSetConfigInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the requirement of configure.
     * @param[in] msgData The data of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSetConfigReq(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);

    /**
     * @brief Process the confirm of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectCfm(BtAddr addr, uint16_t handle, uint8_t role) const;
};

class A2dpStateConfigure : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a configure State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateConfigure(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Configure object.
     *
     * @since 6.0
     */
    ~A2dpStateConfigure() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry()
    {}

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit()
    {}

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the Indication of configure.
     * @param[in] msgData The data of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSetConfigInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the requirement of disconnect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectReq(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the Indication of configure.
     * @param[in] msgData The data of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessOpenInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the requirement of open.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessOpenReq(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);

    /**
     * @brief Process the indication of delay reporting.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     */
    void ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const;
};

class A2dpStateOpening : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a Opening State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateOpening(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}
    /**
     * @brief Destruct a Opening object.
     *
     * @since 6.0
     */
    ~A2dpStateOpening() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry()
    {}

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit()
    {}

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the requirement of disconnect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectReq(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the confirm of open.
     * @param[in] msgData The data of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessOpenCfm(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);

    /**
     * @brief Process the indication of delay reporting.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     */
    void ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const;
};

class A2dpStateOpen : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a Open State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateOpen(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Open object.
     *
     * @since 6.0
     */
    ~A2dpStateOpen() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry()
    {}

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit()
    {}

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the timeout.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessTimeout(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of start.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessStartInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the confirm of start.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessStartCfm(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of close.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessCloseInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the confirm of close.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessCloseCfm(BtAddr addr, uint8_t role);

    /**
     * @brief Process the confirm of reconfigure.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @param[in] handle The stream handle
     * @since 6.0
     */
    void ProcessReconfigCfm(BtAddr addr, uint8_t role, uint16_t handle);

    /**
     * @brief Process the requirement of disconnect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectReq(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the other cmd of open state.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSubOpenState(A2dpAvdtMsgData msgData, uint8_t role, int cmd);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);

    /**
     * @brief Process the indication of delay reporting.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     */
    void ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const;

    uint8_t label_ = 0;
};

class A2dpStateStreaming : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a Streaming State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateStreaming(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Streaming object.
     *
     * @since 6.0
     */
    ~A2dpStateStreaming() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry();

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit();

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the confirm of close.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessCloseCfm(BtAddr addr, uint8_t role);

    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the requirement of disconnect.
     * @param[in] addr The address of peer device
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectReq(BtAddr addr, uint8_t role);

    /**
     * @brief Process the confirm of suspend.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSuspendCfm(A2dpAvdtMsgData msgData, uint8_t role);

     /**
     * @brief Process the confirm of writting data.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessWriteCfm(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Process the confirm of suspend.
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSuspendReq(uint16_t handle, uint8_t role);

    /**
     * @brief Process the indication of suspend.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessSuspendInd(A2dpAvdtMsgData msgData, uint8_t role);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);

    /**
     * @brief Process the indication of delay reporting.
     * @param[in] msgData The information of message
     * @param[in] role The role of local profile
     */
    void ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const;

    uint8_t label_ = 0;
};

class A2dpStateClosing : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a Closing State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpStateClosing(const std::string &name, utility::StateMachine &stateMachine)
        : utility::StateMachine::State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Closing object.
     *
     * @since 6.0
     */
    ~A2dpStateClosing() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry()
    {}

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit()
    {}

    /**
     * @brief Dispatch the message of profile peer.
     * @param[in] The message of profile peer related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Process the indication of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the confirm of disconnection.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessDisconnectCfm(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Process the indication of close stream.
     * @param[in] addr The address of peer device
     * @param[in] handle The handle of stream
     * @param[in] role The role of local profile
     * @since 6.0
     */
    void ProcessCloseStreamInd(BtAddr addr, uint16_t handle, uint8_t role);

    /**
     * @brief Set current state
     *
     * @since 6.0
     */
    void SetStateName(std::string state);
};

class A2dpStateMachine : public utility::StateMachine {
public:
    /**
     * @brief Construct a State machine object.
     *
     * @since 6.0
     */
    A2dpStateMachine()
    {
        std::unique_ptr<StateMachine::State> idl = std::make_unique<A2dpStateIdle>(A2DP_PROFILE_IDLE, *this);
        std::unique_ptr<StateMachine::State> configure =
            std::make_unique<A2dpStateConfigure>(A2DP_PROFILE_CONFIG, *this);
        std::unique_ptr<StateMachine::State> opening = std::make_unique<A2dpStateOpening>(A2DP_PROFILE_OPENING, *this);
        std::unique_ptr<StateMachine::State> open = std::make_unique<A2dpStateOpen>(A2DP_PROFILE_OPEN, *this);
        std::unique_ptr<StateMachine::State> streaming =
            std::make_unique<A2dpStateStreaming>(A2DP_PROFILE_STREAMING, *this);
        std::unique_ptr<StateMachine::State> closing = std::make_unique<A2dpStateClosing>(A2DP_PROFILE_CLOSING, *this);
        Move(idl);
        Move(configure);
        Move(opening);
        Move(open);
        Move(streaming);
        Move(closing);
        InitState(A2DP_PROFILE_IDLE);
    }

    /**
     * @brief Destruct a State machine object.
     *
     * @since 6.0
     */
    ~A2dpStateMachine() = default;

    /**
     * @brief Get the name of state current
     *
     * @since 6.0
     */
    const std::string GetStateName(void) const;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_STATE_MACHINE_H