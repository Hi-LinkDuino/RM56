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

#include "adapter_state_machine.h"

#include "log.h"

#include "adapter_manager.h"
#include "power_manager.h"
#include "profile_service_manager.h"

namespace OHOS {
namespace bluetooth {
const int ENABLE_DISABLE_TIMEOUT_TIME = 70000;

void AdapterStateMachine::Init(IAdapter &adapter)
{
    std::unique_ptr<utility::StateMachine::State> turningOn = std::make_unique<AdapterTurningOnState>(*this, adapter);
    Move(turningOn);
    std::unique_ptr<utility::StateMachine::State> turnOn = std::make_unique<AdapterTurnOnState>(*this, adapter);
    Move(turnOn);
    std::unique_ptr<utility::StateMachine::State> turningOff = std::make_unique<AdapterTurningOffState>(*this, adapter);
    Move(turningOff);
    std::unique_ptr<utility::StateMachine::State> turnOff = std::make_unique<AdapterTurnOffState>(*this, adapter);
    Move(turnOff);
    InitState(TURN_OFF_STATE);
}

utility::Dispatcher &AdapterStateMachine::GetDispatch() const
{
    return dispatch_;
}

void AdapterTurnOffState::Entry()
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    AdapterManager::GetInstance()->OnAdapterStateChange(transport, BTStateID::STATE_TURN_OFF);
}

bool AdapterTurnOffState::Dispatch(const utility::Message &msg)
{
    switch (msg.what_) {
        case AdapterStateMachine::MSG_USER_ENABLE_REQ:
            Transition(TURNING_ON_STATE);
            return true;
        default:
            return false;
    }
}

AdapterTurningOnState::AdapterTurningOnState(AdapterStateMachine &stateMachine, IAdapter &adapter)
    : AdapterState(TURNING_ON_STATE, stateMachine, adapter)
{
    adapterTimer_ = std::make_unique<utility::Timer>([&]() -> void {
        stateMachine.GetDispatch().PostTask(std::bind(
            &utility::StateMachine::ProcessMessage, &stateMachine, AdapterStateMachine::MSG_ADAPTER_ENABLE_TIME_OUT));
    });
    profileTimer_ = std::make_unique<utility::Timer>([&]() -> void {
        stateMachine.GetDispatch().PostTask(std::bind(
            &utility::StateMachine::ProcessMessage, &stateMachine, AdapterStateMachine::MSG_PROFILE_ENABLE_TIME_OUT));
    });
}

void AdapterTurningOnState::Entry()
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    AdapterManager::GetInstance()->OnAdapterStateChange(transport, BTStateID::STATE_TURNING_ON);
    LOG_DEBUG("AdapterStateMachine::Timer enable adapter start transport is %{public}d", transport);
    adapterTimer_->Start(ENABLE_DISABLE_TIMEOUT_TIME, false);
    adapter_.GetContext()->Enable();
}

bool AdapterTurningOnState::Dispatch(const utility::Message &msg)
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    switch (msg.what_) {
        case AdapterStateMachine::MSG_ADAPTER_ENABLE_CMP:
            LOG_DEBUG("AdapterStateMachine::Timer enable adapter stop transport is %{public}d", transport);
            adapterTimer_->Stop();
            if (msg.arg1_) {
                LOG_DEBUG("AdapterStateMachine::Timer enable profile start transport is %{public}d", transport);
                if (transport == BTTransport::ADAPTER_BREDR) {
                    IPowerManager::GetInstance().Enable();
                }
                profileTimer_->Start(ENABLE_DISABLE_TIMEOUT_TIME, false);
                ProfileServiceManager::GetInstance()->Enable(transport);
            } else {
                Transition(TURNING_OFF_STATE);
            }
            break;
        case AdapterStateMachine::MSG_PROFILE_ENABLE_CMP:
            LOG_DEBUG("AdapterStateMachine::Timer enable profile stop transport is %{public}d", transport);
            profileTimer_->Stop();
            if (msg.arg1_) {
                Transition(TURN_ON_STATE);
            } else {
                ProfileServiceManager::GetInstance()->OnAllEnabled(transport);
                Transition(TURNING_OFF_STATE);
            }
            break;
        case AdapterStateMachine::MSG_ADAPTER_ENABLE_TIME_OUT:
            LOG_DEBUG("AdapterStateMachine::Timer enable timer out transport is %{public}d", transport);
            AdapterManager::GetInstance()->Reset();
            adapterStateMachine_.GetDispatch().PostTask(
                std::bind(&AdapterTurningOnState::Transition, this, TURNING_OFF_STATE));
            break;
        case AdapterStateMachine::MSG_PROFILE_ENABLE_TIME_OUT:
            LOG_DEBUG("AdapterStateMachine::Timer enable timer out transport is %{public}d", transport);
            AdapterManager::GetInstance()->Reset();
            ProfileServiceManager::GetInstance()->OnAllEnabled(transport);
            adapterStateMachine_.GetDispatch().PostTask(
                std::bind(&AdapterTurningOnState::Transition, this, TURNING_OFF_STATE));
            break;
        default:
            return false;
    }
    return true;
}

void AdapterTurnOnState::Entry()
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    AdapterManager::GetInstance()->OnAdapterStateChange(transport, BTStateID::STATE_TURN_ON);
    adapter_.GetContext()->PostEnable();
}

bool AdapterTurnOnState::Dispatch(const utility::Message &msg)
{
    switch (msg.what_) {
        case AdapterStateMachine::MSG_USER_DISABLE_REQ:
            Transition(TURNING_OFF_STATE);
            return true;
        default:
            return false;
    }
}

AdapterTurningOffState::AdapterTurningOffState(AdapterStateMachine &stateMachine, IAdapter &adapter)
    : AdapterState(TURNING_OFF_STATE, stateMachine, adapter)
{
    adapterTimer_ = std::make_unique<utility::Timer>([&]() -> void {
        stateMachine.GetDispatch().PostTask(std::bind(
            &utility::StateMachine::ProcessMessage, &stateMachine, AdapterStateMachine::MSG_ADAPTER_DISABLE_TIME_OUT));
    });
    profileTimer_ = std::make_unique<utility::Timer>([&]() -> void {
        stateMachine.GetDispatch().PostTask(std::bind(
            &utility::StateMachine::ProcessMessage, &stateMachine, AdapterStateMachine::MSG_PROFILE_DISABLE_TIME_OUT));
    });
}

void AdapterTurningOffState::Entry()
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    AdapterManager::GetInstance()->OnAdapterStateChange(transport, BTStateID::STATE_TURNING_OFF);
    LOG_DEBUG("AdapterStateMachine::Timer disable profile start transport is %{public}d", transport);
    profileTimer_->Start(ENABLE_DISABLE_TIMEOUT_TIME, false);
    ProfileServiceManager::GetInstance()->Disable(transport);
}

bool AdapterTurningOffState::Dispatch(const utility::Message &msg)
{
    BTTransport transport =
        (adapter_.GetContext()->Name() == ADAPTER_NAME_CLASSIC) ? BTTransport::ADAPTER_BREDR : BTTransport::ADAPTER_BLE;
    switch (msg.what_) {
        case AdapterStateMachine::MSG_PROFILE_DISABLE_CMP:
            LOG_DEBUG("AdapterStateMachine::Timer disable profile stop transport is %{public}d", transport);
            profileTimer_->Stop();
            LOG_DEBUG("AdapterStateMachine::Timer disable adapter Start transport is %{public}d", transport);
            adapterTimer_->Start(ENABLE_DISABLE_TIMEOUT_TIME, false);
            if (transport == BTTransport::ADAPTER_BREDR) {
                IPowerManager::GetInstance().Disable();
            }
            adapter_.GetContext()->Disable();
            break;
        case AdapterStateMachine::MSG_ADAPTER_DISABLE_CMP:
            LOG_DEBUG("AdapterStateMachine::Timer disable adapter stop transport is %{public}d", transport);
            adapterTimer_->Stop();
            Transition(TURN_OFF_STATE);
            break;
        case AdapterStateMachine::MSG_PROFILE_DISABLE_TIME_OUT:
            LOG_DEBUG("AdapterStateMachine::Timer disable profile time out transport is %{public}d", transport);
            ProfileServiceManager::GetInstance()->OnAllDisabled(transport);
            adapterTimer_->Start(ENABLE_DISABLE_TIMEOUT_TIME, false);
            adapter_.GetContext()->Disable();
            AdapterManager::GetInstance()->Reset();
            break;
        case AdapterStateMachine::MSG_ADAPTER_DISABLE_TIME_OUT:
            LOG_DEBUG("AdapterStateMachine::Timer disable adapter time out transport is %{public}d", transport);
            AdapterManager::GetInstance()->Reset();
            adapterStateMachine_.GetDispatch().PostTask(
                std::bind(&AdapterTurningOffState::Transition, this, TURN_OFF_STATE));
            break;
        default:
            return false;
    }
    return true;
}
}  // namespace bluetooth
}  // namespace OHOS