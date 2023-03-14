/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_CONTROLLER_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

using SwipeToImpl = std::function<void(const int32_t, bool)>;
using ShowPrevImpl = std::function<void()>;
using ShowNextImpl = std::function<void()>;
using FinishAnimationImpl = std::function<void()>;

class SwiperController : public virtual AceType {
    DECLARE_ACE_TYPE(SwiperController, AceType);

public:
    void SwipeTo(int32_t index, bool reverse = false)
    {
        if (swipeToImpl_) {
            swipeToImpl_(index, reverse);
        }
    }

    void SetSwipeToImpl(const SwipeToImpl& swipeToImpl)
    {
        swipeToImpl_ = swipeToImpl;
    }

    void ShowPrevious()
    {
        if (showPrevImpl_) {
            showPrevImpl_();
        }
    }

    void SetShowPrevImpl(const ShowPrevImpl& showPrevImpl)
    {
        showPrevImpl_ = showPrevImpl;
    }

    void ShowNext()
    {
        if (showNextImpl_) {
            showNextImpl_();
        }
    }

    void SetShowNextImpl(const ShowNextImpl& showNextImpl)
    {
        showNextImpl_ = showNextImpl;
    }

    void FinishAnimation() const
    {
        if (finishImpl_) {
            finishImpl_();
        }
    }

    void SetFinishImpl(const FinishAnimationImpl& finishImpl)
    {
        finishImpl_ = finishImpl;
    }

    void SetFinishCallback(const EventMarker& marker)
    {
        finishCallback_ = marker;
    }

    const EventMarker& GetFinishCallback() const
    {
        return finishCallback_;
    }

private:
    SwipeToImpl swipeToImpl_;
    ShowPrevImpl showPrevImpl_;
    ShowNextImpl showNextImpl_;
    FinishAnimationImpl finishImpl_;
    EventMarker finishCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_CONTROLLER_H
