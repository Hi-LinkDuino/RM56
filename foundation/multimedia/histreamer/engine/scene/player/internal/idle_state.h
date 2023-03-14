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

#ifndef HISTREAMER_HIPLAYER_IDLE_STATE_H
#define HISTREAMER_HIPLAYER_IDLE_STATE_H
#include "state.h"
namespace OHOS {
namespace Media {
// idle --setSource--> init  anyState --reset--> idle
class IdleState : public State {
public:
    explicit IdleState(StateId stateId, PlayExecutor& executor) : State(stateId, "IdleState", executor) {}

    ~IdleState() override = default;

    std::tuple<ErrorCode, Action> SetSource(const Plugin::Any& param) override
    {
        std::shared_ptr<MediaSource> source;
        if (!param.SameTypeWith(typeid(std::shared_ptr<MediaSource>)) ||
            !(source = Plugin::AnyCast<std::shared_ptr<MediaSource>>(param))) {
            return {ErrorCode::ERROR_INVALID_PARAMETER_TYPE, Action::ACTION_BUTT};
        }
        auto ret = executor_.DoSetSource(source);
        Action action = ret == ErrorCode::SUCCESS ? Action::TRANS_TO_INIT : Action::ACTION_BUTT;
        return {ret, action};
    }

    std::tuple<ErrorCode, Action> Enter(Intent) override
    {
        return {executor_.DoReset(), Action::ACTION_BUTT};
    }
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_HIPLAYER_IDLE_STATE_H
