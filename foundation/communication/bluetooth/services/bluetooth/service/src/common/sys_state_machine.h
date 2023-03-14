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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief This file is a state machine of service manager.
 *
 * @since 6
 */

/**
 * @file sys_state_machine.h
 *
 * @brief system state machine interface.
 *
 * @since 6
 */

#ifndef SYS_STATE_MACHINE_H
#define SYS_STATE_MACHINE_H

#include "adapter_manager.h"
#include "util/state_machine.h"

namespace OHOS {
namespace bluetooth {
// system state machine each state name
const std::string SYS_STATE_STOPPED = "Stopped";
const std::string SYS_STATE_STOPPING = "Stopping";
const std::string SYS_STATE_STARTED = "Started";
const std::string SYS_STATE_RESETTING = "Resetting";
const std::string SYS_STATE_FRESETTING = "FactoryResetting";

/**
 * @brief Represents system state machine.
 *
 * @since 6
 */
class SysStateMachine : public utility::StateMachine {
public:
    // define system state machine message kinds
    enum SysStateMessage {
        MSG_SYS_STOP_REQ,
        MSG_SYS_RESET_REQ,
        MSG_SYS_FACTORY_RESET_REQ,
        MSG_SYS_START_CMP,
        MSG_SYS_STOP_CMP,
        MSG_SYS_RESET_CMP,
        MSG_SYS_FACTORY_RESET_CMP,
        MSG_SYS_CLEAR_ALL_STORAGE_CMP,
        MSG_SYS_ADAPTER_STATE_CHANGE_REQ,
    };

    /**
     * @brief A constructor used to create an <b>SysStateMachine</b> instance.
     *
     * @since 6
     */
    SysStateMachine() = default;

    /**
     * @brief A destructor used to delete the <b>SysStateMachine</b> instance.
     *
     * @since 6
     */
    ~SysStateMachine() = default;

    /**
     * @brief Init system state machine.
     *
     * @param am Adapter manager pointer.
     * @since 6
     */
    void Init(AdapterManager &am);
};

/**
 * @brief Represents basic system state.
 *
 * @since 6
 */
class SysState : public utility::StateMachine::State {
public:
    /**
     * @brief A constructor used to create an <b>SysState</b> instance.
     *
     * @param name The system state name.
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysState(const std::string &name, SysStateMachine &sysStateMachine, AdapterManager &am)
        : State(name, sysStateMachine), am_(am){};

    /**
     * @brief A destructor used to delete the <b>SysState</b> instance.
     *
     * @since 6
     */
    ~SysState() = default;

protected:
    AdapterManager &am_;
};

class SysStoppingBaseState : public SysState {
public:
    /**
     * @brief A constructor used to create an <b>SysStoppingBaseState</b> instance.
     *
     * @param name The system state name.
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysStoppingBaseState(const std::string &name, SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysState(name, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysStoppingBaseState</b> instance.
     *
     * @since 6
     */
    ~SysStoppingBaseState() = default;

protected:
    void StoppingBaseProcess(int stateBREDR, int stateBLE);
};

class SysStoppedState : public SysState {
public:
    /**
     * @brief A constructor used to create an <b>SysStoppedState</b> instance.
     *
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysStoppedState(SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysState(SYS_STATE_STOPPED, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysStoppedState</b> instance.
     *
     * @since 6
     */
    ~SysStoppedState() = default;

    /**
     * @brief Entry system stop state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Exit system stop state.
     *
     * @since 6
     */
    virtual void Exit(){};

    /**
     * @brief System stop state's dispatch.
     *
     * @param msg Message context which is used in dispath.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};

class SysStartedState : public SysState {
public:
    /**
     * @brief A constructor used to create an <b>SysStartedState</b> instance.
     *
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysStartedState(SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysState(SYS_STATE_STARTED, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysStartedState</b> instance.
     *
     * @since 6
     */
    ~SysStartedState() = default;

    /**
     * @brief Entry system start state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Exit system start state.
     *
     * @since 6
     */
    virtual void Exit(){};

    /**
     * @brief System start state's dispatch.
     *
     * @param msg Message context which is used in dispath.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};

class SysStoppingState : public SysStoppingBaseState {
public:
    /**
     * @brief A constructor used to create an <b>SysStoppingState</b> instance.
     *
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysStoppingState(SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysStoppingBaseState(SYS_STATE_STOPPING, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysStoppingState</b> instance.
     *
     * @since 6
     */
    ~SysStoppingState() = default;

    /**
     * @brief Entry system stopping state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Exit system stopping state.
     *
     * @since 6
     */
    virtual void Exit();

    /**
     * @brief System stopping state's dispatch.
     *
     * @param msg Message context which is used in dispath.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};

class SysResettingState : public SysStoppingBaseState {
public:
    /**
     * @brief A constructor used to create an <b>SysResettingState</b> instance.
     *
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysResettingState(SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysStoppingBaseState(SYS_STATE_RESETTING, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysResettingState</b> instance.
     *
     * @since 6
     */
    ~SysResettingState() = default;

    /**
     * @brief Entry system resetting state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Exit system resetting state.
     *
     * @since 6
     */
    virtual void Exit(){};

    /**
     * @brief System resetting state's dispatch.
     *
     * @param msg Message context which is used in dispath.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};

class SysFactoryResettingState : public SysStoppingBaseState {
public:
    /**
     * @brief A constructor used to create an <b>SysFactoryResettingState</b> instance.
     *
     * @param sysStateMachine The state machine which this state belong to.
     * @param am Adapter manager pointer.
     * @since 6
     */
    SysFactoryResettingState(SysStateMachine &sysStateMachine, AdapterManager &am)
        : SysStoppingBaseState(SYS_STATE_FRESETTING, sysStateMachine, am){};

    /**
     * @brief A destructor used to delete the <b>SysFactoryResettingState</b> instance.
     *
     * @since 6
     */
    ~SysFactoryResettingState() = default;

    /**
     * @brief Entry system factory resetting state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Exit system factory resetting state.
     *
     * @since 6
     */
    virtual void Exit();

    /**
     * @brief System factory resetting state's dispatch.
     *
     * @param msg Message context which is used in dispath.
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // SYS_STATE_MACHINE_H