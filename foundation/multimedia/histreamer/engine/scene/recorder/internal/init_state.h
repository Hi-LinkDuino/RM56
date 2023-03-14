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

#ifndef HISTREAMER_HIRECORDER_INIT_STATE_H
#define HISTREAMER_HIRECORDER_INIT_STATE_H

#include "foundation/osal/thread/mutex.h"
#include "pipeline/core/error_code.h"
#include "recorder_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
namespace Record {
class InitState : public State {
public:
    explicit InitState(StateId stateId, RecorderExecutor& executor) : State(stateId, "InitState", executor)
    {
    }

    ~InitState() override = default;

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

    std::tuple<ErrorCode, Action> Stop(const Plugin::Any& param) override
    {
        return {ErrorCode::ERROR_INVALID_OPERATION, Action::TRANS_TO_ERROR};
    }

    std::tuple<ErrorCode, Action> OnComplete() override
    {
        auto ret = executor_.DoOnComplete();
        return {ret, Action::ACTION_BUTT};
    }
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_HIRECORDER_INIT_STATE_H
