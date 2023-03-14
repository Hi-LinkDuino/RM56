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

#ifndef A2DP_SERVICE_STATE_MACHINE_H
#define A2DP_SERVICE_STATE_MACHINE_H

#include <cstdint>
#include <memory>
#include <string>

#include "a2dp_def.h"
#include "raw_address.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
const std::string A2DP_STATE_CONNECTING = "A2dpServiceConnecting";
const std::string A2DP_STATE_CONNECTED = "A2dpServiceConnected";
const std::string A2DP_STATE_DISCONNECTING = "A2dpServiceDisconnecting";
const std::string A2DP_STATE_DISCONNECTED = "A2dpServiceDisconnected";

class A2dpState : public utility::StateMachine::State {
public:
    /**
     * @brief Construct a service State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpState(const std::string &name, utility::StateMachine &stateMachine) : State(name, stateMachine)
    {}

    /**
     * @brief Destruct a Service State object.
     * @since 6.0
     */
    virtual ~A2dpState() = default;
};

class A2dpDisconnected : public A2dpState {
public:
    /**
     * @brief Construct a disconnected State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpDisconnected(const std::string &name, utility::StateMachine &stateMachine) : A2dpState(name, stateMachine){};
    ~A2dpDisconnected() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry(){};

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit(){};

    /**
     * @brief Dispatch the message of service.
     * @param[in] The message of service related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);
};

class A2dpDisconnecting : public A2dpState {
public:
    /**
     * @brief Construct a disconnecting State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpDisconnecting(const std::string &name, utility::StateMachine &stateMachine) : A2dpState(name, stateMachine)
    {}

    ~A2dpDisconnecting() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry(){};

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit(){};

    /**
     * @brief Dispatch the message of service.
     * @param[in] The message of service related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);
};

class A2dpConnected : public A2dpState {
public:
    /**
     * @brief Construct a connected State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpConnected(const std::string &name, utility::StateMachine &stateMachine) : A2dpState(name, stateMachine)
    {}

    ~A2dpConnected() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry(){};

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit(){};

    /**
     * @brief Dispatch the message of service.
     * @param[in] The message of service related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);

private:
    /**
     * @brief Dispatch the message of service.
     * @param[in] rawAddr The address of remote device
     * @param[in] value The status of playing
     * @since 6.0
     */
    void UpdateDeviceInformation(RawAddress rawAddr, bool value, uint8_t role);
};

class A2dpConnecting : public A2dpState {
public:
    /**
     * @brief Construct a connecting State object.
     * @param name State's name.
     * @param stateMachine State is owned by which StateMachine.
     * @since 6.0
     */
    A2dpConnecting(const std::string &name, utility::StateMachine &stateMachine) : A2dpState(name, stateMachine)
    {}

    ~A2dpConnecting() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     * @since 6.0
     */
    void Entry(){};

    /**
     * @brief Operation should be executed when Exit the state.
     * @since 6.0
     */
    void Exit(){};

    /**
     * @brief Dispatch the message of service.
     * @param[in] The message of service related
     * @since 6.0
     */
    bool Dispatch(const utility::Message &msg);
};

class A2dpStateManager : public utility::StateMachine {
public:
    /**
     * @brief Construct a state machine object.
     *
     * @since 6.0
     */
    A2dpStateManager()
    {
        std::unique_ptr<StateMachine::State> disconnected =
            std::make_unique<A2dpDisconnected>(A2DP_STATE_DISCONNECTED, *this);
        std::unique_ptr<StateMachine::State> disconnecting =
            std::make_unique<A2dpDisconnecting>(A2DP_STATE_DISCONNECTING, *this);
        std::unique_ptr<StateMachine::State> connected = std::make_unique<A2dpConnected>(A2DP_STATE_CONNECTED, *this);
        std::unique_ptr<StateMachine::State> connecting =
            std::make_unique<A2dpConnecting>(A2DP_STATE_CONNECTING, *this);
        Move(disconnected);
        Move(disconnecting);
        Move(connected);
        Move(connecting);
        InitState(A2DP_STATE_DISCONNECTED);
    }

    /**
     * @brief Destruct a State machine object.
     * @since 6.0
     */
    ~A2dpStateManager() = default;

    /**
     * @brief Set the role of service.
     * @param[in] The role of service
     * @since 6.0
     */
    void SetRole(uint8_t role);

private:
    uint8_t role_ = A2DP_ROLE_SOURCE;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_SERVICE_STATE_MACHINE_H
