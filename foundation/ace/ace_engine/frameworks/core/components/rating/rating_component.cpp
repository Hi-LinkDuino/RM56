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

#include "core/components/rating/rating_component.h"

#include "core/components/rating/rating_element.h"

namespace OHOS::Ace {

void RatingComponent::SetResIdFromTheme(const RefPtr<RatingTheme>& theme)
{
    if (!theme) {
        return;
    }
    foregroundResourceId_ = theme->GetForegroundResourceId();
    secondaryResourceId_ = theme->GetSecondaryResourceId();
    backgroundResourceId_ = theme->GetBackgroundResourceId();
}

void RatingComponent::SetMiniResIdFromTheme(const RefPtr<RatingTheme>& theme)
{
    if (!theme) {
        return;
    }
    foregroundResourceId_ = theme->GetForegroundMiniResourceId();
    secondaryResourceId_ = theme->GetSecondaryMiniResourceId();
    backgroundResourceId_ = theme->GetBackgroundMiniResourceId();
}

RefPtr<RenderNode> RatingComponent::CreateRenderNode()
{
    return RenderRating::Create();
}

RefPtr<Element> RatingComponent::CreateElement()
{
    return AceType::MakeRefPtr<RatingElement>();
}

void RatingComponent::SetThemeStyle(const RefPtr<RatingTheme>& theme)
{
    if (!theme) {
        return;
    }
    SetStarNum(theme->GetStarNum());
    SetStepSize(theme->GetStepSize());
    SetDefaultHeight(theme->GetRatingHeight());
    SetDesignedStarAspectRatio(theme->GetDesignedStarAspectRatio());
    SetFocusBorderWidth(theme->GetFocusBorderWidth());
    SetFocusBorderRadius(theme->GetFocusBorderRadius());
    SetHoverColor(theme->GetHoverColor());
    SetStarColorActive(theme->GetStarColorActive());
    SetStarColorInactive(theme->GetStarColorInactive());
}

} // namespace OHOS::Ace
