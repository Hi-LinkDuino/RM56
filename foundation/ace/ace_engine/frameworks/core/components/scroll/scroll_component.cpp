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

#include "core/components/scroll/scroll_component.h"

#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components/scroll/scroll_element.h"

namespace OHOS::Ace {

ScrollComponent::ScrollComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}

RefPtr<Element> ScrollComponent::CreateElement()
{
    return AceType::MakeRefPtr<ScrollElement>();
}

RefPtr<RenderNode> ScrollComponent::CreateRenderNode()
{
    return RenderSingleChildScroll::Create();
}

void ScrollComponent::InitScrollBar(const RefPtr<ScrollBarTheme>& theme, const std::pair<bool, Color>& scrollBarColor,
    const std::pair<bool, Dimension>& scrollBarWidth, EdgeEffect edgeEffect)
{
    if (!positionController_) {
        positionController_ = AceType::MakeRefPtr<ScrollPositionController>();
    }
    if (edgeEffect == EdgeEffect::SPRING) {
        SetScrollEffect(AceType::MakeRefPtr<ScrollSpringEffect>());
    } else if (edgeEffect == EdgeEffect::FADE) {
        SetScrollEffect(AceType::MakeRefPtr<ScrollFadeEffect>(Color::GRAY));
    } else {
        SetScrollEffect(AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE));
    }

    if (!theme) {
        return;
    }

    scrollBar_ = AceType::MakeRefPtr<ScrollBar>(DisplayMode::AUTO, theme->GetShapeMode());
    if (scrollBarWidth.first) {
        const auto& width = scrollBarWidth.second;
        scrollBar_->SetInactiveWidth(width);
        scrollBar_->SetNormalWidth(width);
        scrollBar_->SetActiveWidth(width);
        scrollBar_->SetTouchWidth(width);
    } else {
        scrollBar_->SetInactiveWidth(theme->GetNormalWidth());
        scrollBar_->SetNormalWidth(theme->GetNormalWidth());
        scrollBar_->SetActiveWidth(theme->GetActiveWidth());
        scrollBar_->SetTouchWidth(theme->GetTouchWidth());
    }
    scrollBar_->SetReservedHeight(theme->GetReservedHeight());
    scrollBar_->SetMinHeight(theme->GetMinHeight());
    scrollBar_->SetMinDynamicHeight(theme->GetMinDynamicHeight());
    scrollBar_->SetBackgroundColor(theme->GetBackgroundColor());
    scrollBar_->SetForegroundColor(scrollBarColor.first ? scrollBarColor.second : theme->GetForegroundColor());
    scrollBar_->SetPadding(theme->GetPadding());
    scrollBar_->SetScrollable(true);
}

} // namespace OHOS::Ace