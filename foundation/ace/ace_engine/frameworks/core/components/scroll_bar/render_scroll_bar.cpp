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

#include "core/components/scroll_bar/render_scroll_bar.h"

#include "base/utils/string_utils.h"
#include "core/animation/curve_animation.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/display/render_display.h"
#include "core/components/scroll_bar/scroll_bar_element.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t STOP_DURATION = 2000; // 2000ms
constexpr double KEYTIME_START = 0.0;
constexpr double KEYTIME_MIDDLE = 0.7;
constexpr double KEYTIME_END = 1.0;

} // namespace

RenderScrollBar::~RenderScrollBar()
{
    if (proxy_) {
        proxy_->UnRegisterScrollableNode(AceType::WeakClaim(this));
    }
}

void RenderScrollBar::Update(const RefPtr<Component>& component)
{
    auto scrollBarComponent = AceType::DynamicCast<ScrollBarComponent>(component);
    if (!scrollBarComponent) {
        LOGE("Type of component is not ScrollBarComponent.");
        return;
    }
    isAxisChanged_ = (axis_ == scrollBarComponent->GetAxis());
    axis_ = scrollBarComponent->GetAxis();
    displayMode_ = scrollBarComponent->GetDisplayMode();
    proxy_ = scrollBarComponent->GetScrollBarProxy();
    if (proxy_) {
        proxy_->UnRegisterScrollBar(AceType::WeakClaim(this));
        proxy_->RegisterScrollBar(AceType::WeakClaim(this));
    }
    InitOpacity();
    InitRecognizer();
    InitAnimator();
    InitChildPosition();
    MarkNeedLayout();
}

void RenderScrollBar::InitRecognizer()
{
    if (!isAxisChanged_ && dragRecognizer_) {
        LOGW("Axis is not change and DragRecognizer is already exist.");
        return;
    }
    dragRecognizer_ = AceType::MakeRefPtr<DragRecognizer>(axis_);
    dragRecognizer_->SetOnDragStart([weak = WeakClaim(this)](const DragStartInfo& startInfo) {
        auto scrollBar = weak.Upgrade();
        if (scrollBar) {
            scrollBar->HandleDragStart(startInfo);
        }
    });
    dragRecognizer_->SetOnDragUpdate([weak = WeakClaim(this)](const DragUpdateInfo& updateInfo) {
        auto scrollBar = weak.Upgrade();
        if (scrollBar) {
            scrollBar->HandleDragUpdate(updateInfo);
        }
    });
    dragRecognizer_->SetOnDragEnd([weak = WeakClaim(this)](const DragEndInfo& endInfo) {
        auto scrollBar = weak.Upgrade();
        if (scrollBar) {
            scrollBar->HandleDragFinish();
        }
    });
    dragRecognizer_->SetOnDragCancel([weak = WeakClaim(this)]() {
        auto scrollBar = weak.Upgrade();
        if (scrollBar) {
            scrollBar->HandleDragFinish();
        }
    });
}

void RenderScrollBar::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (axis_ != Axis::NONE && displayMode_ != DisplayMode::OFF) {
        dragRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragRecognizer_);
    }
}

void RenderScrollBar::HandleDragStart(const DragStartInfo& info)
{
    auto child = GetChildren().front();
    if (!child) {
        return;
    }

    StopAnimator();
}

void RenderScrollBar::HandleDragUpdate(const DragUpdateInfo& info)
{
    auto child = GetChildren().front();
    if (!child) {
        return;
    }
    StopAnimator();
    if (axis_ == Axis::HORIZONTAL) {
        auto localX = info.GetLocalLocation().GetX();
        if (GreatOrEqual(localX, GetTouchRect().Left()) && LessOrEqual(localX, GetTouchRect().Right())) {
            double positionX = std::clamp(child->GetPosition().GetX() + info.GetDelta().GetX(), 0.0,
                (GetLayoutSize() - child->GetLayoutSize()).Width());
            child->SetPosition(Offset(positionX, child->GetPosition().GetY()));
            MarkNeedRender();
            if (proxy_) {
                proxy_->NotifyScrollableNode(-info.GetMainDelta(), AceType::WeakClaim(this));
            }
        }
    } else {
        auto localY = info.GetLocalLocation().GetY();
        if (GreatOrEqual(localY, GetTouchRect().Top()) && LessOrEqual(localY, GetTouchRect().Bottom())) {
            double positionY = std::clamp(child->GetPosition().GetY() + info.GetDelta().GetY(), 0.0,
                (GetLayoutSize() - child->GetLayoutSize()).Height());
            child->SetPosition(Offset(child->GetPosition().GetX(), positionY));
            MarkNeedRender();
            if (proxy_) {
                proxy_->NotifyScrollableNode(-info.GetMainDelta(), AceType::WeakClaim(this));
            }
        }
    }
}

void RenderScrollBar::HandleDragFinish()
{
    if (displayMode_ == DisplayMode::AUTO && disappearAnimator_) {
        if (!disappearAnimator_->IsStopped()) {
            disappearAnimator_->Stop();
        }
        disappearAnimator_->Play();
    }
}

void RenderScrollBar::InitOpacity()
{
    switch (displayMode_) {
        case DisplayMode::OFF:
            opacity_ = 0;
            break;
        case DisplayMode::ON:
            opacity_ = UINT8_MAX;
            break;
        case DisplayMode::AUTO:
            opacity_ = 0;
            break;
        default:
            break;
    }
    UpdateDisplayOpacity(opacity_);
}

void RenderScrollBar::InitChildPosition()
{
    auto child = GetLastChild();
    if (!child) {
        return;
    }
    auto childPosition = child->GetPosition();
    if (axis_ == Axis::VERTICAL) {
        childPosition.SetX(0.0);
    } else if (axis_ == Axis::HORIZONTAL) {
        childPosition.SetY(0.0);
    } else {
        LOGD("Axis of ScrollBar is not support: %{public}d", axis_);
        return;
    }
    child->SetPosition(childPosition);
}

void RenderScrollBar::InitAnimator()
{
    if (disappearAnimator_ && !disappearAnimator_->IsStopped()) {
        disappearAnimator_->Stop();
    }
    if (displayMode_ != DisplayMode::AUTO) {
        LOGE("DisplayMode is not auto, don't need animator.");
        return;
    }
    if (disappearAnimator_) {
        disappearAnimator_->Play();
        return;
    }

    disappearAnimator_ = AceType::MakeRefPtr<Animator>(context_);
    auto hiddenStartKeyframe = AceType::MakeRefPtr<Keyframe<int32_t>>(KEYTIME_START, UINT8_MAX);
    auto hiddenMiddleKeyframe = AceType::MakeRefPtr<Keyframe<int32_t>>(KEYTIME_MIDDLE, UINT8_MAX);
    auto hiddenEndKeyframe = AceType::MakeRefPtr<Keyframe<int32_t>>(KEYTIME_END, 0);
    hiddenMiddleKeyframe->SetCurve(Curves::LINEAR);
    hiddenEndKeyframe->SetCurve(Curves::FRICTION);

    auto animation = AceType::MakeRefPtr<KeyframeAnimation<int32_t>>();
    animation->AddKeyframe(hiddenStartKeyframe);
    animation->AddKeyframe(hiddenMiddleKeyframe);
    animation->AddKeyframe(hiddenEndKeyframe);
    animation->AddListener([weakBar = AceType::WeakClaim(this)](int32_t value) {
        auto scrollBar = weakBar.Upgrade();
        if (scrollBar) {
            scrollBar->opacity_ = value;
            scrollBar->UpdateDisplayOpacity(value);
        }
    });
    disappearAnimator_->AddInterpolator(animation);
    disappearAnimator_->SetDuration(STOP_DURATION);
    disappearAnimator_->Play();
}

void RenderScrollBar::StopAnimator()
{
    if (disappearAnimator_ && !disappearAnimator_->IsStopped()) {
        disappearAnimator_->Stop();
    }
    if (displayMode_ != DisplayMode::OFF) {
        UpdateDisplayOpacity(UINT8_MAX);
    }
    MarkNeedRender();
}

void RenderScrollBar::StartAnimator()
{
    if (!disappearAnimator_) {
        LOGE("Animator is not exist.");
        return;
    }
    if (!disappearAnimator_->IsStopped()) {
        disappearAnimator_->Stop();
    }
    disappearAnimator_->Play();
}

void RenderScrollBar::UpdateDisplayOpacity(int32_t opacity)
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto display = AceType::DynamicCast<RenderDisplay>(parent);
        if (display) {
            display->UpdateOpacity(opacity);
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
}

void RenderScrollBar::PerformLayout()
{
    if (!GetChildren().empty()) {
        const auto& child = GetChildren().front();
        child->Layout(LayoutParam(GetLayoutParam().GetMaxSize(), Size()));
        childRect_ = Rect(child->GetPosition(), child->GetLayoutSize());
    }
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

void RenderScrollBar::OnPaintFinish()
{
    auto child = GetLastChild();
    if (child) {
        childRect_ = Rect(child->GetPosition(), child->GetLayoutSize());
    }
}

} // namespace OHOS::Ace