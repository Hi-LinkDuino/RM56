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

#ifndef HISTREAMER_HIPLAYER_PAUSE_STATE_H
#define HISTREAMER_HIPLAYER_PAUSE_STATE_H

#include <memory>
#include "foundation/log.h"
#include "pipeline/core/error_code.h"
#include "play_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
class PauseState : public State {
public:
    explicit PauseState(StateId stateId, PlayExecutor& executor) : State(stateId, "PauseState", executor)
    {
    }

    ~PauseState() override = default;

    std::tuple<ErrorCode, Action> Enter(Intent) override
    {
        MEDIA_LOG_D("Pause state entered.");
        auto ret = executor_.DoPause();
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Play() override
    {
        MEDIA_LOG_D("Play in pause state.");
        return {ErrorCode::SUCCESS, Action::TRANS_TO_PLAYING};
    }

    std::tuple<ErrorCode, Action> Seek(const Plugin::Any& param) override
    {
        MEDIA_LOG_D("Seek in pause state.");
        std::tuple<ErrorCode, Action> err {ErrorCode::ERROR_INVALID_PARAMETER_TYPE, Action::ACTION_BUTT};
        FALSE_RETURN_V(param.SameTypeWith(typeid(SeekInfo)), err);
        auto info = Plugin::AnyCast<SeekInfo>(param);
        auto ret = executor_.DoSeek(true, info.hstTime, info.mode, true);
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Pause() override
    {
        MEDIA_LOG_D("Pause in pause state.");
        return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Resume() override
    {
        MEDIA_LOG_D("Resume in pause state.");
        return {ErrorCode::SUCCESS, Action::TRANS_TO_PLAYING};
    }

    std::tuple<ErrorCode, Action> Stop() override
    {
        MEDIA_LOG_D("Stop called in pause state.");
        return {ErrorCode::SUCCESS, Action::TRANS_TO_STOPPED};
    }
};
} // namespace Media
} // namespace OHOS
#endif
