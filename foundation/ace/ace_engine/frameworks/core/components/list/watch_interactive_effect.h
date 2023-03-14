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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_WATCH_INTERACTIVE_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_WATCH_INTERACTIVE_EFFECT_H

#include "core/components/list/interactive_effect.h"

namespace OHOS::Ace {

class WatchInteractiveEffect : public InteractiveEffect {
    DECLARE_ACE_TYPE(WatchInteractiveEffect, InteractiveEffect);

public:
    explicit WatchInteractiveEffect(const WeakPtr<PipelineContext>& context) : InteractiveEffect(context) {}

    ~WatchInteractiveEffect() override = default;

    void HandleOnClick() override
    {
        scaleBegin_ = scale_;
        if (theme_) {
            scaleEnd_ = scale_ * theme_->GetClickScale();         // scale_ * 0.95;
            alphaBegin_ = theme_->GetClickAlphaBegin();           // 0.0
            alphaEnd_ = theme_->GetClickAlphaEnd();               // 0.1
            focusDuration_ = theme_->GetClickAnimationDuration(); // 200.0
        } else {
            LOGE("HandleOnClick : theme is not invalid");
        }
    }

    void BuildClickAnimation() override;

private:
    void BuildClickScaleAnimation(const RefPtr<KeyframeAnimation<double>>& scaleAnimation);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_WATCH_INTERACTIVE_EFFECT_H
