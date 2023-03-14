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

#ifndef HISTREAMER_HIRECORDER_READY_STATE_H
#define HISTREAMER_HIRECORDER_READY_STATE_H

#include <memory>
#include "pipeline/core/error_code.h"
#include "recorder_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
namespace Record {
class ReadyState : public State {
public:
    explicit ReadyState(StateId stateId, RecorderExecutor& executor) : State(stateId, "ReadyState", executor)
    {
    }

    ~ReadyState() override = default;

    std::tuple<ErrorCode, Action> Start() override
    {
        MEDIA_LOG_D("Start in ready state.");
        return {ErrorCode::SUCCESS, Action::TRANS_TO_RECORDING};
    }

    std::tuple<ErrorCode, Action> Stop(const Plugin::Any& param) override
    {
        return {ErrorCode::ERROR_INVALID_OPERATION, Action::TRANS_TO_ERROR};
    }
};
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_HIRECORDER_READY_STATE_H
