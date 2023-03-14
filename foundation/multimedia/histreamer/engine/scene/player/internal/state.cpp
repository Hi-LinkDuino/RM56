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

#define HST_LOG_TAG "State"

#include "state.h"
#include "pipeline/core/event.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
const char* State::GetStateName(StateId state)
{
    static const std::map<StateId, const char*> stateDesc = {
        {StateId::IDLE, "IDLE"},
        {StateId::INIT, "INIT"},
        {StateId::PREPARING, "PREPARING"},
        {StateId::READY, "READY"},
        {StateId::PAUSE, "PAUSE"},
        {StateId::PLAYING, "PLAYING"},
        {StateId::STOPPED, "STOPPED"},
        {StateId::EOS, "EOS"},
        {StateId::BUTT, "BUTT"},
    };
    return stateDesc.at(state);
}
const char* State::GetIntentName(Intent intent)
{
    static const std::map<Intent, const char*> intentDesc = {
        {Intent::SET_SOURCE, "SET_SOURCE"},
        {Intent::PREPARE, "PREPARE"},
        {Intent::SEEK, "SEEK"},
        {Intent::PLAY, "PLAY"},
        {Intent::PAUSE, "PAUSE"},
        {Intent::RESUME, "RESUME"},
        {Intent::STOP, "STOP"},
        {Intent::RESET, "RESET"},
        {Intent::SET_ATTRIBUTE, "SET_ATTRIBUTE"},
        {Intent::NOTIFY_READY, "NOTIFY_READY"},
        {Intent::NOTIFY_COMPLETE, "NOTIFY_COMPLETE"},
        {Intent::NOTIFY_ERROR, "NOTIFY_ERROR"},
        {Intent::INTENT_BUTT, "INTENT_BUTT"}
    };
    return intentDesc.at(intent);
}
const char* State::GetActionName(Action action)
{
    static const std::map<Action, const char*> actionDesc = {
        {Action::TRANS_TO_IDLE, "TRANS_TO_IDLE"},
        {Action::TRANS_TO_INIT, "TRANS_TO_INIT"},
        {Action::TRANS_TO_PREPARING, "TRANS_TO_PREPARING"},
        {Action::TRANS_TO_READY, "TRANS_TO_READY"},
        {Action::TRANS_TO_PLAYING, "TRANS_TO_PLAYING"},
        {Action::TRANS_TO_PAUSE, "TRANS_TO_PAUSE"},
        {Action::TRANS_TO_STOPPED, "TRANS_TO_STOPPED"},
        {Action::TRANS_TO_EOS, "TRANS_TO_EOS"},
        {Action::ACTION_PENDING, "ACTION_PENDING"},
        {Action::ACTION_BUTT, "ACTION_BUTT"}
    };
    return actionDesc.at(action);
}

State::State(StateId stateId, std::string name, PlayExecutor& executor)
    : stateId_(stateId), name_(std::move(name)), executor_(executor)
{
}
std::tuple<ErrorCode, Action> State::Enter(Intent intent)
{
    (void)intent;
    MEDIA_LOG_D("Enter state: " PUBLIC_LOG_S, name_.c_str());
    return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
}
void State::Exit()
{
    MEDIA_LOG_D("Exit state: " PUBLIC_LOG_S, name_.c_str());
}
std::tuple<ErrorCode, Action> State::Execute(Intent intent, const Plugin::Any& param)
{
    return DispatchIntent(intent, param);
}
const std::string& State::GetName()
{
    return name_;
}
StateId State::GetStateId()
{
    return stateId_;
}
std::tuple<ErrorCode, Action> State::SetSource(const Plugin::Any& source)
{
    (void)source;
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::Prepare()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::Play()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::Stop()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}

std::tuple<ErrorCode, Action> State::Reset()
{
    return {ErrorCode::SUCCESS, Action::TRANS_TO_IDLE};
}

std::tuple<ErrorCode, Action> State::Pause()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::Resume()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::Seek(const Plugin::Any& param)
{
    (void)param;
    // current state doesn't support seek, so allowed is false.
    executor_.DoSeek(false, 0, Plugin::SeekMode::SEEK_PREVIOUS_SYNC, true);
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::SetAttribute()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::OnReady()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::OnError(const Plugin::Any& param)
{
    ErrorCode errorCode = ErrorCode::ERROR_UNKNOWN;
    if (param.SameTypeWith(typeid(ErrorCode))) {
        errorCode = Plugin::AnyCast<ErrorCode>(param);
    }
    executor_.DoOnError(errorCode);
    return {ErrorCode::SUCCESS, Action::TRANS_TO_INIT};
}
std::tuple<ErrorCode, Action> State::OnComplete()
{
    return {ErrorCode::ERROR_INVALID_OPERATION, Action::ACTION_BUTT};
}
std::tuple<ErrorCode, Action> State::DispatchIntent(Intent intent, const Plugin::Any& param)
{
    ErrorCode rtv = ErrorCode::SUCCESS;
    Action nextAction = Action::ACTION_BUTT;
    switch (intent) {
        case Intent::SET_SOURCE:
            std::tie(rtv, nextAction) = SetSource(param);
            break;
        case Intent::PREPARE:
            std::tie(rtv, nextAction) = Prepare();
            break;
        case Intent::SEEK:
            std::tie(rtv, nextAction) = Seek(param);
            break;
        case Intent::PLAY:
            std::tie(rtv, nextAction) = Play();
            break;
        case Intent::PAUSE:
            std::tie(rtv, nextAction) = Pause();
            break;
        case Intent::RESUME:
            std::tie(rtv, nextAction) = Resume();
            break;
        case Intent::STOP:
            std::tie(rtv, nextAction) = Stop();
            break;
        case Intent::RESET:
            std::tie(rtv, nextAction) = Reset();
            break;
        case Intent::SET_ATTRIBUTE:
            std::tie(rtv, nextAction) = SetAttribute();
            break;
        case Intent::NOTIFY_READY:
            std::tie(rtv, nextAction) = OnReady();
            break;
        case Intent::NOTIFY_COMPLETE:
            std::tie(rtv, nextAction) = OnComplete();
            break;
        case Intent::NOTIFY_ERROR:
            std::tie(rtv, nextAction) = OnError(param);
            break;
        default:
            break;
    }
    MEDIA_LOG_D("DispatchIntent " PUBLIC_LOG_S ", curState: " PUBLIC_LOG_S ", action: " PUBLIC_LOG_S,
                GetIntentName(intent), name_.c_str(), GetActionName(nextAction));
    return {rtv, nextAction};
}
} // namespace Media
} // namespace OHOS