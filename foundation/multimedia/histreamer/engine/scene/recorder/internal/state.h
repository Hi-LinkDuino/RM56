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

#ifndef HISTREAMER_HIRECORDER_STATE_H
#define HISTREAMER_HIRECORDER_STATE_H

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include "foundation/log.h"
#include "pipeline/core/error_code.h"
#include "plugin/common/any.h"
#include "recorder_executor.h"

namespace OHOS {
namespace Media {
namespace Record {
enum class StateId {
    INIT,
    RECORDING_SETTING,
    READY,
    PAUSE,
    RECORDING,
    ERROR,
    BUTT,
};

enum class Intent {
    SET_OBS,
    SET_VIDEO_SOURCE,
    SET_AUDIO_SOURCE,
    SET_OUTPUT_FORMAT,
    CONFIGURE,
    PREPARE,
    START,
    PAUSE,
    RESUME,
    STOP,
    RESET,
    NOTIFY_READY,
    NOTIFY_COMPLETE,
    NOTIFY_ERROR,
    INTENT_BUTT
};

enum class Action {
    TRANS_TO_INIT,
    TRANS_TO_RECORDING_SETTING,
    TRANS_TO_READY,
    TRANS_TO_RECORDING,
    TRANS_TO_PAUSE,
    TRANS_TO_ERROR,
    ACTION_PENDING,
    ACTION_BUTT
};

class State {
public:
    State(StateId stateId, std::string name, RecorderExecutor& executor);
    virtual ~State() = default;
    virtual std::tuple<ErrorCode, Action> Enter(Intent intent);
    virtual void Exit();
    std::tuple<ErrorCode, Action> Execute(Intent intent, const Plugin::Any& param);
    const std::string &GetName();
    StateId GetStateId();

    virtual std::tuple<ErrorCode, Action> SetVideoSource(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> SetAudioSource(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> Configure(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> SetOutputFormat(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> SetObs();
    virtual std::tuple<ErrorCode, Action> GetSurface();
    virtual std::tuple<ErrorCode, Action> Prepare();
    virtual std::tuple<ErrorCode, Action> Start();
    virtual std::tuple<ErrorCode, Action> Stop(const Plugin::Any& param);
    virtual std::tuple<ErrorCode, Action> Pause();
    virtual std::tuple<ErrorCode, Action> Resume();
    virtual std::tuple<ErrorCode, Action> Reset();
    virtual std::tuple<ErrorCode, Action> OnReady();
    virtual std::tuple<ErrorCode, Action> OnError(const Plugin::Any& param) final;
    virtual std::tuple<ErrorCode, Action> OnComplete();

protected:
    std::tuple<ErrorCode, Action> DispatchIntent(Intent intent, const Plugin::Any& param);

    const StateId stateId_;
    const std::string name_;
    RecorderExecutor &executor_;
    const std::map<Intent, std::string> intentDesc_ = {
        {Intent::SET_OBS, "SET_OBS"},
        {Intent::SET_VIDEO_SOURCE, "SET_VIDEO_SOURCE"},
        {Intent::SET_AUDIO_SOURCE, "SET_AUDIO_SOURCE"},
        {Intent::SET_OUTPUT_FORMAT, "SET_OUTPUT_FORMAT"},
        {Intent::CONFIGURE, "CONFIGURE"},
        {Intent::PREPARE, "PREPARE"},
        {Intent::START, "START"},
        {Intent::PAUSE, "PAUSE"},
        {Intent::RESUME, "RESUME"},
        {Intent::STOP, "STOP"},
        {Intent::RESET, "RESET"},
        {Intent::NOTIFY_READY, "NOTIFY_READY"},
        {Intent::NOTIFY_COMPLETE, "NOTIFY_COMPLETE"},
        {Intent::NOTIFY_ERROR, "NOTIFY_ERROR"},
        {Intent::INTENT_BUTT, "INTENT_BUTT"}};
    const std::map<Action, std::string> actionDesc_ = {
        {Action::TRANS_TO_INIT, "TRANS_TO_INIT"},
        {Action::TRANS_TO_RECORDING_SETTING, "TRANS_TO_RECORDING_SETTING"},
        {Action::TRANS_TO_READY, "TRANS_TO_READY"},
        {Action::TRANS_TO_RECORDING, "TRANS_TO_RECORDING"},
        {Action::TRANS_TO_PAUSE, "TRANS_TO_PAUSE"},
        {Action::TRANS_TO_ERROR, "TRANS_TO_ERROR"},
        {Action::ACTION_PENDING, "ACTION_PENDING"},
        {Action::ACTION_BUTT, "ACTION_BUTT"}};
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif
