/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/indexer/render_indexer_item.h"

#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/text/render_text.h"
#include "core/components_v2/indexer/indexer_component.h"
#include "core/components_v2/indexer/indexer_item_component.h"

namespace OHOS::Ace::V2 {
namespace {
constexpr int32_t HOVER_ANIMATION_DURATION = 250;
} // namespace

RefPtr<RenderNode> RenderIndexerItem::Create()
{
    return AceType::MakeRefPtr<RenderIndexerItem>();
}

void RenderIndexerItem::Update(const RefPtr<Component>& component)
{
    RefPtr<IndexerItemComponent> itemComponent = AceType::DynamicCast<IndexerItemComponent>(component);
    if (itemComponent) {
        sectionIndex_ = itemComponent->GetSectionIndex();
        textComponent_ = itemComponent->GetTextComponent();
        normalStyle_ = itemComponent->GetNormalTextStyle();
        activeStyle_ = itemComponent->GetActiveTextStyle();
        rotate_ = itemComponent->GetRotateFlag();
        strText_ = itemComponent->GetSectionStr();
        itemType_ = itemComponent->GetItemType();
        itemSize_ = itemComponent->GetItemSize();
        isPrimary_ = itemComponent->IsItemPrimary();
        selectedBgColor_ = itemComponent->GetSelectedBackgroundColor();
        LOGI("[indexer] Update sectionIndex:%d", sectionIndex_);
    }
}

void RenderIndexerItem::PerformLayout()
{
    if (GetChildren().empty()) {
        LOGE("RenderIndexerItem: no child found in RenderIndexerItem!");
        return;
    }

    RefPtr<RenderBox> box;
    if (!rotate_) {
        box = AceType::DynamicCast<RenderBox>(GetChildren().front());
    } else {
        RefPtr<RenderNode> transform = GetChildren().front();
        if (transform && !transform->GetChildren().empty()) {
            box = AceType::DynamicCast<RenderBox>(transform->GetChildren().front());
        }
    }
    if (!box) {
        LOGE("[indexer] Get Box failed.");
        return;
    }

    auto child = GetChildren().front();
    if (child) {
        child->Layout(GetLayoutParam());
        child->SetPosition(Offset::Zero());
        SetLayoutSize(child->GetLayoutSize());
    } else {
        SetLayoutSize(Size());
    }

    SetRotate(rotateAngle_);
}

bool RenderIndexerItem::MouseHoverTest(const Point& parentLocalPoint)
{
    auto parent = GetParent().Upgrade();
    if (!parent || !AceType::DynamicCast<RenderBox>(parent)) {
        return RenderNode::MouseHoverTest(parentLocalPoint);
    }
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    if (InTouchRectList(parentLocalPoint, parent->GetTouchRectList())) {
        if (mouseState_ == MouseState::NONE) {
            OnMouseHoverEnterTest();
            mouseState_ = MouseState::HOVER;
        }
        context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
        return true;
    }

    if (mouseState_ == MouseState::HOVER) {
        OnMouseHoverExitTest();
        mouseState_ = MouseState::NONE;
    }
    return false;
}

void RenderIndexerItem::OnMouseHoverEnterTest()
{
    if (!clicked_) {
        ResetController(controllerExit_);
        if (!controllerEnter_) {
            controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
        }
        colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        CreateColorAnimation(colorAnimationEnter_, Color::TRANSPARENT, Color::FromString("#0C000000"));
        colorAnimationEnter_->SetCurve(Curves::FRICTION);
        StartHoverAnimation(controllerEnter_, colorAnimationEnter_);
    }
}

void RenderIndexerItem::OnMouseHoverExitTest()
{
    if (!clicked_) {
        ResetController(controllerEnter_);
        if (!controllerExit_) {
            controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
        }
        colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        RefPtr<RenderBox> box = AceType::DynamicCast<RenderBox>(GetChildren().front());
        if (!box) {
            return;
        }
        auto currentColor = box->GetColor();
        CreateColorAnimation(colorAnimationExit_, currentColor, Color::TRANSPARENT);
        if (currentColor == Color::FromString("#0C000000")) {
            colorAnimationExit_->SetCurve(Curves::FRICTION);
        } else {
            colorAnimationExit_->SetCurve(Curves::FAST_OUT_SLOW_IN);
        }
        StartHoverAnimation(controllerExit_, colorAnimationExit_);
    }
}

void RenderIndexerItem::StartHoverAnimation(RefPtr<Animator> controller,
    RefPtr<KeyframeAnimation<Color>>& colorAnimation)
{
    if (!controller || !colorAnimation) {
        return;
    }
    controller->ClearInterpolators();
    controller->AddInterpolator(colorAnimation);
    controller->SetDuration(HOVER_ANIMATION_DURATION);
    controller->SetFillMode(FillMode::FORWARDS);
    controller->Play();
}

void RenderIndexerItem::CreateColorAnimation(
    RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue, const Color& endValue)
{
    if (!colorAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0, endValue);
    colorAnimation->AddKeyframe(keyframeBegin);
    colorAnimation->AddKeyframe(keyframeEnd);
    RefPtr<RenderBox> box = AceType::DynamicCast<RenderBox>(GetChildren().front());
    if (!box) {
        return;
    }
    auto weakBox = WeakClaim(RawPtr(box));
    colorAnimation->AddListener([weakBox](const Color& value) {
        auto box = weakBox.Upgrade();
        if (box) {
            box->SetColor(value, true);
            box->MarkNeedRender();
        }
    });
}

void RenderIndexerItem::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void RenderIndexerItem::UpdateItemStyle()
{
    if (isAnimating_) {
        return;
    }

    RefPtr<RenderBox> box = AceType::DynamicCast<RenderBox>(GetChildren().front());
    if (!box) {
        LOGE("[indexer] Box is null.");
        return;
    }
    RefPtr<RenderText> text = AceType::DynamicCast<RenderText>(box->GetChildren().front());
    if (!text || !textComponent_) {
        LOGE("[indexer] Text is null.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    if (clicked_) {
        box->SetColor(selectedBgColor_, true);
        textComponent_->SetTextStyle(activeStyle_);
    } else {
        box->SetColor(Color::TRANSPARENT, true);
        textComponent_->SetTextStyle(normalStyle_);
    }
    text->Update(textComponent_);
}
} // namespace OHOS::Ace::V2