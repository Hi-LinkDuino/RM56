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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_TV_INTERACTIVE_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_TV_INTERACTIVE_EFFECT_H

#include "core/components/list/interactive_effect.h"

namespace OHOS::Ace {

class TVInteractiveEffect : public InteractiveEffect {
    DECLARE_ACE_TYPE(TVInteractiveEffect, InteractiveEffect);

public:
    explicit TVInteractiveEffect(const WeakPtr<PipelineContext>& context) : InteractiveEffect(context) {}

    ~TVInteractiveEffect() override = default;

    void HandleOnClick() override {}

    void HandleOnFocus() override
    {
        scaleBegin_ = scale_;
        if (theme_) {
            scaleEnd_ = theme_->GetFocusScale();                  // 1.05
            focusDuration_ = theme_->GetFocusAnimationDuration(); // 100.0
        } else {
            LOGE("TV HandleOnFocus : theme is not invalid");
        }
    }

    void HandleOnBlur() override
    {
        scaleBegin_ = scale_;
        if (theme_) {
            scaleEnd_ = theme_->GetFocusScaleLarge();             // 1.0
            alphaBegin_ = theme_->GetClickAlphaEnd();             // 0.9
            alphaEnd_ = theme_->GetClickAlphaBegin();             // 0.2
            focusDuration_ = theme_->GetFocusAnimationDuration(); // 100.0
        } else {
            LOGE("TV HandleOnBlur : theme is not invalid");
        }
    }

    void BuildStateAnimation() override;
    void BuildClickAnimation() override {}
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_TV_INTERACTIVE_EFFECT_H
