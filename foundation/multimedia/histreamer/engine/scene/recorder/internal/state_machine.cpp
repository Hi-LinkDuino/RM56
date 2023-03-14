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
#include "osal/utils/util.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Record {
StateMachine::StateMachine(RecorderExecutor& executor)
    : Task("StateMachine"),
      intentSync_("fsmSync"),
      curState_(std::make_shared<InitState>(StateId::INIT, executor)),
      jobs_("StateMachineJobQue")
{
    AddState(curState_);
    AddState(std::make_shared<ErrorState>(StateId::ERROR, executor));
    AddState(std::make_shared<RecordingSettingState>(StateId::RECORDING_SETTING, executor));
    AddState(std::make_shared<ReadyState>(StateId::READY, executor));
    AddState(std::make_shared<RecordingState>(StateId::RECORDING, executor));
    AddState(std::make_shared<PauseState>(StateId::PAUSE, executor));
}

void StateMachine::Stop()
{
    MEDIA_LOG_D("Stop called.");
    while (!jobs_.Empty()) {
        OSAL::SleepFor(10); // 10
    }
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
    constexpr int timeoutMs = 30000; /// 30000 ms
    ErrorCode errorCode = ErrorCode::ERROR_TIMED_OUT;
    if (!intentSync_.WaitFor(intent,
                             [this, intent, param] { return SendEventAsync(intent, param) == ErrorCode::SUCCESS; },
                             timeoutMs,
                             errorCode)) {
        MEDIA_LOG_E("SendEvent timeout, intent: " PUBLIC_LOG_D32, static_cast<int>(intent));
    }
    return errorCode;
}

ErrorCode StateMachine::SendEventAsync(Intent intent, const Plugin::Any& param) const
{
    return const_cast<StateMachine*>(this)->SendEventAsync(intent, param);
}

ErrorCode StateMachine::SendEventAsync(Intent intent, const Plugin::Any& param)
{
    MEDIA_LOG_D("SendEventAsync, intent: " PUBLIC_LOG_D32, static_cast<int>(intent));
    if (jobs_.Push([this, intent, param]() -> Action { return ProcessIntent(intent, param); })) {
        return ErrorCode::SUCCESS;
    }
    return ErrorCode::ERROR_UNKNOWN;
}

Action StateMachine::ProcessIntent(Intent intent, const Plugin::Any& param)
{
    MEDIA_LOG_D("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_D32 ".",
                curState_->GetName().c_str(), intent);
    PROFILE_BEGIN("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_D32 ".",
                  curState_->GetName().c_str(), intent);
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
    PROFILE_END("ProcessIntent, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_D32 ".",
                curState_->GetName().c_str(), intent);
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
        case Action::TRANS_TO_INIT:
        case Action::TRANS_TO_RECORDING_SETTING:
        case Action::TRANS_TO_READY:
        case Action::TRANS_TO_RECORDING:
        case Action::TRANS_TO_PAUSE: {
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
        case Action::TRANS_TO_INIT:
            nextState = states_[StateId::INIT];
            break;
        case Action::TRANS_TO_RECORDING_SETTING:
            nextState = states_[StateId::RECORDING_SETTING];
            break;
        case Action::TRANS_TO_READY:
            nextState = states_[StateId::READY];
            break;
        case Action::TRANS_TO_RECORDING:
            nextState = states_[StateId::RECORDING];
            break;
        case Action::TRANS_TO_PAUSE:
            nextState = states_[StateId::PAUSE];
            break;
        case Action::TRANS_TO_ERROR:
            nextState = states_[StateId::ERROR];
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
        MEDIA_LOG_E("TransitionTo, nullptr for state");
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
    MEDIA_LOG_D("OnIntentExecuted, curState: " PUBLIC_LOG_S ", intent: " PUBLIC_LOG_D32 ", action: " PUBLIC_LOG
                "d, result: " PUBLIC_LOG_S, curState_->GetName().c_str(),
                static_cast<int>(intent), static_cast<int>(action), GetErrorName(result));
    if (action == Action::ACTION_PENDING) {
        return;
    }
    if (intent == Intent::NOTIFY_READY && action == Action::TRANS_TO_RECORDING) {
        intentSync_.Notify(Intent::START, result);
    } else {
        intentSync_.Notify(intent, result);
    }
}
} // namespace Record
} // namespace Media
} // namespace OHOS