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

#ifndef POWER_STATE_MACHINE_H
#define POWER_STATE_MACHINE_H

#include "power_device.h"
#include "state_machine.h"

namespace OHOS {
namespace bluetooth {
const std::string ACTIVE_STATE = "Active";
const std::string SNIFF_STATE = "Sniff";
const std::string ACTIV_ACTIVING_STATE = "A_Activing";
const std::string ACTIV_SNIFFING_STATE = "A_Sniffing";
const std::string SNIFF_SNIFFING_STATE = "S_Sniffing";
const std::string SNIFF_ACTIVING_STATE = "S_Activing";

/**
 * @brief Represents power state machine.
 *
 * @since 6
 */
class PowerStateMachine : public utility::StateMachine {
public:
    enum PowerStateMessage {
        MSG_PM_SET_SNIFF,
        MSG_PM_SET_ACTIVE,
        MSG_PM_MODE_CHANGE_ACTIVE,
        MSG_PM_MODE_CHANGE_SNIFF,
        MSG_PM_SET_SUBRATING_COMPLETE
    };
    /**
     * @brief A constructor used to create an <b>PowerStateMachine</b> instance.
     *
     * @since 6
     */
    PowerStateMachine(){};

    /**
     * @brief A destructor used to delete the <b>PowerStateMachine</b> instance.
     *
     * @since 6
     */
    ~PowerStateMachine() = default;

    /**
     * @brief A constructor used to create an <b>PowerStateMachine</b> instance.
     *
     * @param pd power device.
     * @since 6
     */
    void Init(PowerDevice &pd);
};

class PowerState : public utility::StateMachine::State {
public:
    /**
     * @brief A constructor used to create an <b>PowerState</b> instance.
     *
     * @param pm power manager.
     * @since 6
     */
    PowerState(const std::string &name, PowerStateMachine &psm, PowerDevice &pd) : State(name, psm), pd_(pd){};

    /**
     * @brief A constructor used to create an <b>PowerState</b> instance.
     *
     * @param pm power manager.
     * @since 6
     */
    PowerState(const std::string &name, PowerStateMachine &psm, PowerDevice &pd, utility::StateMachine::State &fstate)
        : State(name, psm, fstate), pd_(pd){};

    /**
     * @brief A destructor used to create an <b>PowerState</b> instance.
     *
     * @since 6
     */
    ~PowerState() = default;

protected:
    PowerDevice &pd_;
};

class PowerActiveState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerActiveState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @since 6
     */
    PowerActiveState(PowerStateMachine &psm, PowerDevice &pd) : PowerState(ACTIVE_STATE, psm, pd){};

    /**
     * @brief A destructor used to create an <b>PowerActiveState</b> instance.
     *
     * @since 6
     */
    ~PowerActiveState() = default;

    /**
     * @brief Operation should be executed when Entry the state.
     *
     * @since 6
     */
    virtual void Entry();

    /**
     * @brief Operation should be executed when Exit the state.
     *
     * @since 6
     */
    virtual void Exit();

    /**
     * @brief State dispatch message.
     *
     * @param msg Message.
     * @since 6
     */
    virtual bool Dispatch(const utility::Message &msg);
};

class PowerActiveActivingState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerActiveActivingState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @param fstate Father State.
     * @since 6
     */
    PowerActiveActivingState(PowerStateMachine &psm, PowerDevice &pd, utility::StateMachine::State &fstate)
        : PowerState(ACTIV_ACTIVING_STATE, psm, pd, fstate){};

    /**
     * @brief A destructor used to create an <b>PowerActiveActivingState</b> instance.
     *
     * @since 6
     */
    ~PowerActiveActivingState() = default;
    virtual void Entry();
    virtual void Exit();
    virtual bool Dispatch(const utility::Message &msg);
};

class PowerActiveSniffingState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerActiveSniffingState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @param fstate Father State.
     * @since 6
     */
    PowerActiveSniffingState(PowerStateMachine &psm, PowerDevice &pd, utility::StateMachine::State &fstate)
        : PowerState(ACTIV_SNIFFING_STATE, psm, pd, fstate){};

    /**
     * @brief A destructor used to create an <b>PowerActiveSniffingState</b> instance.
     *
     * @since 6
     */
    ~PowerActiveSniffingState() = default;
    virtual void Entry();
    virtual void Exit();
    virtual bool Dispatch(const utility::Message &msg);
};

class PowerSniffState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerSniffState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @since 6
     */
    PowerSniffState(PowerStateMachine &psm, PowerDevice &pd) : PowerState(SNIFF_STATE, psm, pd){};

    /**
     * @brief A destructor used to create an <b>PowerSniffState</b> instance.
     *
     * @since 6
     */
    ~PowerSniffState() = default;
    virtual void Entry();
    virtual void Exit();
    virtual bool Dispatch(const utility::Message &msg);
};

class PowerSniffActivingState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerSniffActivingState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @param fstate Father State.
     * @since 6
     */
    PowerSniffActivingState(PowerStateMachine &psm, PowerDevice &pd, utility::StateMachine::State &fstate)
        : PowerState(SNIFF_ACTIVING_STATE, psm, pd, fstate){};

    /**
     * @brief A destructor used to create an <b>PowerSniffActivingState</b> instance.
     *
     * @since 6
     */
    ~PowerSniffActivingState() = default;
    virtual void Entry();
    virtual void Exit();
    virtual bool Dispatch(const utility::Message &msg);
};

class PowerSniffSniffingState : public PowerState {
public:
    /**
     * @brief A constructor used to create an <b>PowerSniffSniffingState</b> instance.
     *
     * @param psm StateMachine.
     * @param pd Power Device.
     * @param fstate Father State.
     * @since 6
     */
    PowerSniffSniffingState(PowerStateMachine &psm, PowerDevice &pd, utility::StateMachine::State &fstate)
        : PowerState(SNIFF_SNIFFING_STATE, psm, pd, fstate){};

    /**
     * @brief A destructor used to create an <b>PowerSniffSniffingState</b> instance.
     *
     * @since 6
     */
    ~PowerSniffSniffingState() = default;
    virtual void Entry();
    virtual void Exit();
    virtual bool Dispatch(const utility::Message &msg);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // ADAPTER_STATE_MACHINE_H