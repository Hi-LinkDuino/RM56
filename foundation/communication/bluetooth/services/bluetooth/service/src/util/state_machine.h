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

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <array>
#include <map>
#include <string>
#include "base_def.h"
#include "message.h"

namespace utility {
class StateMachine {
public:
    class State {
    public:
        /**
         * @brief Construct a new State object.
         * @param name State's name.
         * @param statemachine State is owned by which StateMachine.
         * @param parent State's parent state.
         * @since 6
         */
        explicit State(const std::string &name, StateMachine &statemachine)
            : name_(name), statemachine_(statemachine){};

        /**
         * @brief Construct a new State object.
         * @param name State's name.
         * @param statemachine State is owned by which StateMachine.
         * @param parent State's parent state.
         * @since 6
         */
        explicit State(const std::string &name, StateMachine &statemachine, State &parent)
            : name_(name), statemachine_(statemachine), parent_(&parent){};

        /**
         * @brief Destroy the State object
         * @since 6
         */
        virtual ~State(){};

        /**
         * @brief Get State's name.
         * @return State's name.
         * @since 6
         */
        const std::string &Name() const
        {
            return name_;
        };

        /**
         * @brief Transition to another state.
         * @param name Another state's name.
         */
        void Transition(const std::string &name)
        {
            statemachine_.Transition(name);
        };

    private:
        State() = delete;

        /**
         * @brief Operation should be executed when Entry the state.
         * @since 6
         */
        virtual void Entry() = 0;

        /**
         * @brief Operation should be executed when Exit the state.
         * @since 6
         */
        virtual void Exit() = 0;

        virtual bool Dispatch(const Message &msg) = 0;

        std::string name_ {""};
        StateMachine &statemachine_;
        State* parent_ {nullptr};

        friend class StateMachine;
        BT_DISALLOW_COPY_AND_ASSIGN(State);
    };

    /**
     * @brief Construct a new State Machine object
     * @since 6
     */
    StateMachine() {};

    /**
     * @brief Destroy the State Machine object, type is virtual.
     * @since 6
     */
    virtual ~StateMachine() {};

    /**
     * @brief Move unique_ptr of state to StateMachine.
     * @param state State pointer.
     * @since 6
     */
    void Move(std::unique_ptr<State> &state);

    /**
     * @brief Transation to init state.
     * @param name Init state's name.
     * @since 6
     */
    void InitState(const std::string &name);

    /**
     * @brief StateMachine process message
     * @param msg message reference.
     * @since 6
     */
    bool ProcessMessage(const Message &msg) const;

    /**
     * @brief Get the State object
     * @return const State &
     * @since 6
     */
    const State *GetState() const;

private:
    const static int STACK_DEPTH = 5;
    void Transition(const std::string &name);
    inline int GetStateDepth(State &state);

    int top_ {0};
    State* current_ {nullptr};
    std::array<State*, STACK_DEPTH> stack_ {};
    std::map<std::string, std::unique_ptr<State>> states_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(StateMachine);
};
}  // namespace utility

#endif  // STATE_MACHINE_H