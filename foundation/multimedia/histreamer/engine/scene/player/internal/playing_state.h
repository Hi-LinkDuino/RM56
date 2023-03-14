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

#ifndef HISTREAMER_HIPLAYER_PLAYING_STATE_H
#define HISTREAMER_HIPLAYER_PLAYING_STATE_H

#include <memory>
#include "foundation/log.h"
#include "pipeline/core/error_code.h"
#include "play_executor.h"
#include "state.h"

namespace OHOS {
namespace Media {
class PlayingState : public State {
public:
    explicit PlayingState(StateId stateId, PlayExecutor& executor) : State(stateId, "PlayingState", executor)
    {
    }

    ~PlayingState() override = default;

    std::tuple<ErrorCode, Action> Enter(Intent intent) override
    {
        MEDIA_LOG_D("Enter state: " PUBLIC_LOG_S, name_.c_str());
        ErrorCode ret;
        if (intent == Intent::RESUME) {
            ret = executor_.DoResume();
        } else {
            ret = executor_.DoPlay();
        }
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Play() override
    {
        return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Seek(const Plugin::Any& param) override
    {
        MEDIA_LOG_D("Seek in playing state.");
        std::tuple<ErrorCode, Action> err {ErrorCode::ERROR_INVALID_PARAMETER_TYPE, Action::ACTION_BUTT};
        FALSE_RETURN_V(param.SameTypeWith(typeid(SeekInfo)), err);
        auto info = Plugin::AnyCast<SeekInfo>(param);
        auto ret = executor_.DoSeek(true, info.hstTime, info.mode, true);
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Pause() override
    {
        return {ErrorCode::SUCCESS, Action::TRANS_TO_PAUSE};
    }

    std::tuple<ErrorCode, Action> Stop() override
    {
        return {ErrorCode::SUCCESS, Action::TRANS_TO_STOPPED};
    }

    std::tuple<ErrorCode, Action> OnComplete() override
    {
        auto ret = executor_.DoOnComplete();
        if (executor_.IsSingleLoop()) {
            MEDIA_LOG_I("will seeking to 0 and continue playing");
            auto seekRes = executor_.DoSeek(true, 0, Plugin::SeekMode::SEEK_PREVIOUS_SYNC, false);
            return {seekRes, Action::ACTION_BUTT};
        } else {
            return {ret, Action::TRANS_TO_EOS};
        }
    }
};
} // namespace Media
} // namespace OHOS
#endif
