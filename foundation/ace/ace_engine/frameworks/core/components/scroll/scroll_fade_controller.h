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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_CONTROLLER_H

#include <functional>

#include "base/geometry/axis.h"
#include "base/thread/cancelable_callback.h"
#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"

namespace OHOS::Ace {

using ScrollOverFunc = std::function<void(double, double)>;

enum class OverScrollState {
    IDLE = 0,
    ABSORB,
    PULL,
    RECEDE,
};

class ScrollFadeController : public AceType {
    DECLARE_ACE_TYPE(ScrollFadeController, AceType);

public:
    explicit ScrollFadeController(const WeakPtr<PipelineContext>& context);
    ~ScrollFadeController();
    void ProcessAbsorb(double velocity);
    void ProcessPull(double overDistance, double mainAxisExtent, double crossAxisExtent);

    void SetCallback(const ScrollOverFunc& callback)
    {
        callback_ = callback;
    }

protected:
    void ChangeState();
    void SchedulePullHoldTask();

private:
    void Initialize();
    void ProcessRecede(int32_t duration);
    void DecelerateListener(double value);

    double opacity_ = 0.0;
    double opacityFloor_ = 0.0;
    double opacityCeil_ = 0.0;
    double scaleSize_ = 0.0;
    double scaleSizeFloor_ = 0.0;
    double scaleSizeCeil_ = 0.0;
    double pullDistance_ = 0.0;
    OverScrollState state_ = OverScrollState::IDLE;
    RefPtr<Animator> controller_;
    WeakPtr<PipelineContext> context_;
    RefPtr<Animation<double>> decele_;
    ScrollOverFunc callback_;
    CancelableCallback<void()> pullTask_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_FADE_CONTROLLER_H
