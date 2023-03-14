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

#include "core/components/tab_bar/tab_bar_size_animation.h"

#include <algorithm>

#include "core/animation/animator.h"
#include "core/components/tab_bar/render_tab_bar.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t SIZE_TRANSITION_DURATION = 300;
constexpr int32_t ACTIVE_TEXT_SIZE = 24;
constexpr int32_t INACTIVE_TEXT_SIZE = 18;
constexpr double ACTIVE_TEXT_OPACITY = 0.9;
constexpr double INACTIVE_TEXT_OPACITY = 18;

} // namespace

RefPtr<RenderText> GetTextItem(const RefPtr<RenderNode>& node)
{
    for (const auto& item : node->GetChildren()) {
        for (const auto& textItem : item->GetChildren()) {
            return AceType::DynamicCast<RenderText>(textItem);
        }
    }
    return nullptr;
}

void TabBarSizeAnimation::Initialize(const WeakPtr<PipelineContext>& context)
{
    ItemAnimationProp onFocusItem(Dimension(ACTIVE_TEXT_SIZE, DimensionUnit::VP), ACTIVE_TEXT_OPACITY);
    ItemAnimationProp onBlurItem(Dimension(INACTIVE_TEXT_SIZE, DimensionUnit::VP), INACTIVE_TEXT_OPACITY);

    onFocusTranslate_ =
        AceType::MakeRefPtr<CurveAnimation<ItemAnimationProp>>(onBlurItem, onFocusItem, Curves::FRICTION);
    onBlurTranslate_ =
        AceType::MakeRefPtr<CurveAnimation<ItemAnimationProp>>(onFocusItem, onBlurItem, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    controller_ = AceType::MakeRefPtr<Animator>(context);
    onFocusTranslate_->AddListener(Animation<ItemAnimationProp>::ValueCallback([weak](ItemAnimationProp value) {
        auto tabBar = weak.Upgrade();
        if (tabBar) {
            tabBar->ChangeItemProp(tabBar->onFocusItemText_, value);
        }
    }));
    onBlurTranslate_->AddListener(Animation<ItemAnimationProp>::ValueCallback([weak](ItemAnimationProp value) {
        auto tabBar = weak.Upgrade();
        if (tabBar) {
            tabBar->ChangeItemProp(tabBar->onBlurItemText_, value);
        }
    }));
    controller_->SetDuration(SIZE_TRANSITION_DURATION);
    controller_->AddInterpolator(onFocusTranslate_);
    controller_->AddInterpolator(onBlurTranslate_);
}

void TabBarSizeAnimation::Start(const WeakPtr<RenderNode>& weakTabbar, int32_t from, int32_t to)
{
    auto tabBar = weakTabbar.Upgrade();
    if (!tabBar) {
        return;
    }
    auto lastSelected = tabBar->GetChildren().begin();
    std::advance(lastSelected, from);

    onBlurItemText_ = GetTextItem(*lastSelected);

    auto currentSelected = tabBar->GetChildren().begin();
    std::advance(currentSelected, to);
    onFocusItemText_ = GetTextItem(*currentSelected);

    layoutCallback_ = [weakTabbar]() {
        auto tabBar = weakTabbar.Upgrade();
        if (tabBar) {
            tabBar->MarkNeedLayout();
        }
    };
    controller_->Play();
}

void TabBarSizeAnimation::ChangeItemProp(const RefPtr<RenderText>& renderText, const ItemAnimationProp& animationProp)
{
    if (renderText) {
        TextStyle textStyle;
        textStyle.SetTextColor(Color::FromRGBO(0, 0, 0, animationProp.opacity));
        textStyle.SetFontSize(animationProp.fontSize);
        textStyle.SetMaxLines(1);
        textStyle.SetTextOverflow(TextOverflow::CLIP);
        renderText->SetTextStyle(textStyle);
        renderText->MarkNeedMeasure();
        renderText->MarkNeedLayout();
    }
    layoutCallback_();
}

} // namespace OHOS::Ace
