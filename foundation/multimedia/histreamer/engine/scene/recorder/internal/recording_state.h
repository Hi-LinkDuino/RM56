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

#ifndef HISTREAMER_HIRECORDER_RECORDING_STATE_H
#define HISTREAMER_HIRECORDER_RECORDING_STATE_H

#include <memory>
#include "pipeline/core/error_code.h"
#include "recorder_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
namespace Record {
class RecordingState : public State {
public:
    explicit RecordingState(StateId stateId, RecorderExecutor& executor) : State(stateId, "RecordingState", executor)
    {
    }

    ~RecordingState() override = default;

    std::tuple<ErrorCode, Action> Enter(Intent intent) override
    {
        MEDIA_LOG_D("Enter state: " PUBLIC_LOG_S, name_.c_str());
        ErrorCode ret;
        if (intent == Intent::RESUME) {
            ret = executor_.DoResume();
        } else {
            ret = executor_.DoStart();
        }
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Start() override
    {
        return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Resume() override
    {
        return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Pause() override
    {
        return {ErrorCode::SUCCESS, Action::TRANS_TO_PAUSE};
    }

    std::tuple<ErrorCode, Action> Stop(const Plugin::Any& param) override
    {
        auto ret = executor_.DoStop(param);
        Action action = (ret == ErrorCode::SUCCESS) ? Action::ACTION_BUTT : Action::TRANS_TO_ERROR;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> OnComplete() override
    {
        auto ret = executor_.DoOnComplete();
        Action action = (ret == ErrorCode::SUCCESS) ? Action::TRANS_TO_INIT : Action::ACTION_BUTT;
        return {ret, action};
    }
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_HIRECORDER_RECORDING_STATE_H
