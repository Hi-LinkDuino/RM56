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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_ELEMENT_H

#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/rating/render_rating.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class RatingElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(RatingElement, RenderElement, FocusNode);

public:
    void OnFocus() override
    {
        const auto& renderRating = AceType::DynamicCast<RenderRating>(renderNode_);
        if (!renderRating) {
            return;
        }
        renderRating->ChangeStatus(RenderStatus::FOCUS);
        renderRating->SetFocusEventFlag(true);
        renderRating->MarkNeedLayout();
    }

    void OnBlur() override
    {
        const auto& renderRating = AceType::DynamicCast<RenderRating>(renderNode_);
        if (!renderRating) {
            return;
        }
        renderRating->SetFocusEventFlag(false);
        renderRating->MarkNeedLayout();
        auto focusAnimation = renderRating->GetFocusAnimation();
        if (!focusAnimation) {
            return;
        }
        focusAnimation->CancelFocusAnimation();
    }

    bool OnKeyEvent(const KeyEvent& keyEvent) override
    {
        if (keyEvent.action != KeyAction::UP) {
            return false;
        }
        const auto& renderRating = AceType::DynamicCast<RenderRating>(renderNode_);
        if (!renderRating) {
            return false;
        }
        return renderRating->HandleFocusEvent(keyEvent);
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_ELEMENT_H
