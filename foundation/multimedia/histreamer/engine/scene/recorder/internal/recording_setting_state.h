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

#ifndef HISTREAMER_HIRECORDER_RECORDING_SETTING_STATE_H
#define HISTREAMER_HIRECORDER_RECORDING_SETTING_STATE_H

#include "foundation/osal/thread/mutex.h"
#include "pipeline/core/error_code.h"
#include "recorder_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
namespace Record {
class RecordingSettingState : public State {
public:
    explicit RecordingSettingState(StateId stateId, RecorderExecutor& executor)
        : State(stateId, "RecordingSettingState", executor)
    {
    }

    ~RecordingSettingState() override = default;

    std::tuple<ErrorCode, Action> SetVideoSource(const Plugin::Any& param) override
    {
        auto ret = executor_.DoSetVideoSource(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_RECORDING_SETTING : Action::ACTION_BUTT;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> SetAudioSource(const Plugin::Any& param) override
    {
        auto ret = executor_.DoSetAudioSource(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_RECORDING_SETTING : Action::ACTION_BUTT;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> Configure(const Plugin::Any& param) override
    {
        auto ret = executor_.DoConfigure(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_RECORDING_SETTING : Action::ACTION_BUTT;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> SetOutputFormat(const Plugin::Any& param) override
    {
        auto ret = executor_.DoSetOutputFormat(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_RECORDING_SETTING : Action::ACTION_BUTT;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> Prepare() override
    {
        auto rtv = executor_.DoPrepare();
        if (rtv == ErrorCode::SUCCESS) {
            return {rtv, Action::ACTION_BUTT};
        } else {
            return {rtv, Action::TRANS_TO_ERROR};
        }
    }

    std::tuple<ErrorCode, Action> Start() override
    {
        return {ErrorCode::SUCCESS, Action::ACTION_PENDING};
    }

    std::tuple<ErrorCode, Action> Stop(const Plugin::Any& param) override
    {
        auto ret = executor_.DoStop(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_INIT : Action::TRANS_TO_ERROR;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> OnReady() override
    {
        return {ErrorCode::SUCCESS, Action::TRANS_TO_READY};
    }
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_HIRECORDER_RECORDING_SETTING_STATE_H
