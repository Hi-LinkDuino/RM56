/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_HIPLAYER_EOS_STATE_H
#define HISTREAMER_HIPLAYER_EOS_STATE_H

#include "state.h"
namespace OHOS {
namespace Media {
// when eos received
class EosState : public State {
public:
    explicit EosState(StateId stateId, PlayExecutor& executor) : State(stateId, "EosState", executor) {}

    ~EosState() override = default;

    std::tuple<ErrorCode, Action> Play() override
    {
        MEDIA_LOG_I("play in eos state.");
        auto ret = executor_.DoSeek(true, 0, Plugin::SeekMode::SEEK_PREVIOUS_SYNC, false);
        if (ret == ErrorCode::SUCCESS) {
            return {ErrorCode::SUCCESS, Action::TRANS_TO_PLAYING};
        } else {
            MEDIA_LOG_W("seek error in eos state when asked to play");
            return {ret, Action::ACTION_BUTT};
        }
    }

    std::tuple<ErrorCode, Action> Seek(const Plugin::Any& param) override
    {
        MEDIA_LOG_I("seek in eos is useless");
        return {ErrorCode::SUCCESS, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Enter(Intent intent) override
    {
        MEDIA_LOG_I("eos state entered");
        auto ret = executor_.DoPause();
        return {ret, Action::ACTION_BUTT};
    }

    std::tuple<ErrorCode, Action> Stop() override
    {
        MEDIA_LOG_I("stop eos state");
        return {ErrorCode::SUCCESS, Action::TRANS_TO_STOPPED};
    }
};
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_HIPLAYER_EOS_STATE_H
