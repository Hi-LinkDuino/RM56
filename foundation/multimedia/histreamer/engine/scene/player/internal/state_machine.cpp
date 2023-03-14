/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "StateMachine"

#include "state_machine.h"
#include "eos_state.h"
#include "idle_state.h"
#include "init_state.h"
#include "utils/steady_clock.h"
#include "osal/utils/util.h"
#include "pause_state.h"
#include "playing_state.h"
#include "preparing_state.h"
#include "ready_state.h"
#include "stopped_state.h"

namespace OHOS {
namespace Media {
StateMachine::StateMachine(PlayExecutor& executor)
    : Task("StateMachine"),
      intentSync_("fsmSync"),
      curState_(std::make_shared<IdleState>(StateId::IDLE, executor)),
      jobs_("StateMachineJobQue")
{
    AddState(curState_);
    AddState(std::make_shared<InitState>(StateId::INIT, executor));
    AddState(std::make_shared<PreparingState>(StateId::PREPARING, executor));
    AddState(std::make_shared<ReadyState>(StateId::READY, executor));
    AddState(std::make_shared<PlayingState>(StateId::PLAYING, executor));
    AddState(std::make_shared<PauseState>(StateId::PAUSE, executor));
    AddState(std::make_shared<StoppedState>(StateId::STOPPED, executor));
    AddState(std::make_shared<EosState>(StateId::EOS, executor));
}

void StateMachine::Stop()
{
    MEDIA_LOG_I("StateMachine stop called.");
    // while (!jobs_.Empty()) {
    //     OSAL::SleepFor(10); // 10
    // }
    jobs_.SetActive(false);
    Task::Stop();
}

void StateMachine::SetStateCallback(StateChangeCallback* callback)
{
    callback_ = callback;
}

const std::string& StateMachine::GetCurrentState() const
{
    return curState_->GetName();
}

StateId StateMachine::GetCurrentStateId() const
{
    return curState_->GetStateId();
}

ErrorCode StateMachine::SendEvent(Intent intent, const Plugin::Any& param) const
{
    return const_cast<StateMachine*>(this)->SendEvent(intent, param);
}

ErrorCode StateMachine::SendEvent(Intent intent, const Plugin::Any& param)
{
    constexpr int timeoutMs = 30000;
    ErrorCode errorCode = ErrorCode::ERROR_TIMED_OUT;
    if (!intentSync_.WaitFor(
        intent, [this, intent, param] { return SendEventAsync(intent, param) == ErrorCode::SUCCESS; },
        timeoutMs, errorCode)) {
        MEDIA_LOG_E("SendEvent timeout, intent: " PUBLIC_LOG_S " - " PUBLIC_LOG_D32,
                    State::GetIntentName(intent), static_cast<int>(intent));
    }
    return errorCode;
}

ErrorCode StateMachine::SendEventAsync(Intent intent, const Plugin::Any& param) const
{
    return const_cast<StateMachine*>(this)->SendEventAsync(intent, param);
}

ErrorCode StateMachine::SendEventAsync(Intent intent, const Plugin::Any& param)
{
    MEDIA_LOG_D("SendEventAsync, intent: " PUBLIC_LOG_S " - " PUBLIC_LOG_D32,
                State::GetIntentName(intent), static_cast<int>(intent));
    if (jobs_.Push([this, intent, param]() -> Action { return ProcessIntent(intent, param); })) {
        return ErrorCode::SUCCESS;
    }
    return ErrorCode::ERROR_UNKNOWN;
}

Action StateMachine::ProcessIntent(Intent intent, const Plugin::Any& param)
{
    MEDIA_LOG_D("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_S,
                curState_->GetName().c_str(), State::GetIntentName(intent));
    PROFILE_BEGIN("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_S,
                  curState_->GetName().c_str(), State::GetIntentName(intent));
    OSAL::ScopedLock lock(mutex_);
    lastIntent = intent;
    ErrorCode rtv = ErrorCode::SUCCESS;
    Action nextAction = Action::ACTION_BUTT;
    std::tie(rtv, nextAction) = curState_->Execute(intent, param);
    if (nextAction != Action::ACTION_BUTT) {
        if (rtv == ErrorCode::SUCCESS) {
            rtv = ProcAction(nextAction);
        } else {
            (void)ProcAction(nextAction);
        }
    }
    OnIntentExecuted(intent, nextAction, rtv);
    PROFILE_END("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_S,
                curState_->GetName().c_str(), State::GetIntentName(intent));
    return (rtv == ErrorCode::SUCCESS) ? nextAction : Action::ACTION_BUTT;
}

void StateMachine::DoTask()
{
#ifdef UNIT_TEST
    constexpr int timeoutMs = 500;
    auto job = jobs_.Pop(timeoutMs);
#else
    auto job = jobs_.Pop();
#endif
    if (!job) {
        return;
    }
    auto action = job();
    switch (action) {
        case Action::ACTION_PENDING:
            pendingJobs_.push(job);
            break;
        case Action::TRANS_TO_IDLE:
        case Action::TRANS_TO_INIT:
        case Action::TRANS_TO_PREPARING:
        case Action::TRANS_TO_READY:
        case Action::TRANS_TO_PLAYING:
        case Action::TRANS_TO_PAUSE:
        case Action::TRANS_TO_STOPPED:
        case Action::TRANS_TO_EOS: {
            if (!pendingJobs_.empty()) {
                job = pendingJobs_.front();
                pendingJobs_.pop();
                action = job();
                if (action == Action::ACTION_PENDING) {
                    pendingJobs_.push(job);
                }
            }
            break;
        }
        case Action::ACTION_BUTT:
            // fall through
        default:
            break;
    }
}

void StateMachine::AddState(const std::shared_ptr<State>& state)
{
    states_[state->GetStateId()] = state;
}

ErrorCode StateMachine::ProcAction(Action nextAction)
{
    std::shared_ptr<State> nextState = nullptr;
    switch (nextAction) {
        case Action::TRANS_TO_IDLE:
            nextState = states_[StateId::IDLE];
            break;
        case Action::TRANS_TO_INIT:
            nextState = states_[StateId::INIT];
            break;
        case Action::TRANS_TO_PREPARING:
            nextState = states_[StateId::PREPARING];
            break;
        case Action::TRANS_TO_READY:
            nextState = states_[StateId::READY];
            break;
        case Action::TRANS_TO_PLAYING:
            nextState = states_[StateId::PLAYING];
            break;
        case Action::TRANS_TO_PAUSE:
            nextState = states_[StateId::PAUSE];
            break;
        case Action::TRANS_TO_STOPPED:
            nextState = states_[StateId::STOPPED];
            break;
        case Action::TRANS_TO_EOS:
            nextState = states_[StateId::EOS];
            break;
        default:
            break;
    }
    ErrorCode ret = ErrorCode::SUCCESS;
    if (nextState) {
        ret = TransitionTo(nextState);
    }
    return ret;
}

ErrorCode StateMachine::TransitionTo(const std::shared_ptr<State>& state)
{
    if (state == nullptr) {
        MEDIA_LOG_E("TransitionTo, state is nullptr");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    ErrorCode rtv = ErrorCode::SUCCESS;
    if (state != curState_) {
        curState_->Exit();
        curState_ = state;
        Action nextAction;
        std::tie(rtv, nextAction) = curState_->Enter(lastIntent);
        if (rtv == ErrorCode::SUCCESS) {
            rtv = ProcAction(nextAction);
        }
        if (callback_) {
            callback_->OnStateChanged(curState_->GetStateId());
        }
    }
    return rtv;
}

void StateMachine::OnIntentExecuted(Intent intent, Action action, ErrorCode result)
{
    MEDIA_LOG_D("OnIntentExecuted, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_S ", action: " PUBLIC_LOG_S
                ", result: " PUBLIC_LOG_S, curState_->GetName().c_str(),
                State::GetIntentName(intent), State::GetActionName(action), GetErrorName(result));
    if (action == Action::ACTION_PENDING) {
        return;
    }
    if (intent == Intent::NOTIFY_READY && action == Action::TRANS_TO_PLAYING) {
        intentSync_.Notify(Intent::PLAY, result);
    } else {
        intentSync_.Notify(intent, result);
    }
}
} // namespace Media
} // namespace OHOS