/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_CONTROLLER_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {
using TimerFuncImpl = std::function<void()>;
using TimerInputImpl = std::function<void(int32_t)>;

class TextTimerController : public virtual AceType {
    DECLARE_ACE_TYPE(TextTimerController, AceType);

public:
    void Start()
    {
        if (start_) {
            start_();
        }
    }

    void OnStart(const TimerFuncImpl& start)
    {
        start_ = start;
    }

    void Pause()
    {
        if (pause_) {
            pause_();
        }
    }

    void OnPause(const TimerFuncImpl& pause)
    {
        pause_ = pause;
    }

    void Reset()
    {
        if (reset_) {
            reset_();
        }
    }

    void OnReset(const TimerFuncImpl& reset)
    {
        reset_ = reset;
    }

private:
    TimerFuncImpl start_;
    TimerFuncImpl pause_;
    TimerFuncImpl reset_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_CONTROLLER_H
