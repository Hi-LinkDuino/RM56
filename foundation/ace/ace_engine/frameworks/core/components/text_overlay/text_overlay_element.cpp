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

#include "core/components/text_overlay/text_overlay_element.h"

#include "core/components/focus_collaboration/focus_collaboration_element.h"
#include "core/components/text_overlay/render_text_overlay.h"
#include "core/components/text_overlay/text_overlay_component.h"

namespace OHOS::Ace {

void TextOverlayElement::PerformBuild()
{
    overlayComponent_ = AceType::DynamicCast<TextOverlayComponent>(component_);
    if (!overlayComponent_) {
        return;
    }
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, overlayComponent_->BuildChild(overlayComponent_->GetIsSingleHandle(),
        !overlayComponent_->GetIsSingleHandle(), false, !overlayComponent_->GetIsSingleHandle()));

    auto renderTextOverlay = AceType::DynamicCast<RenderTextOverlay>(renderNode_);
    if (!renderTextOverlay) {
        return;
    }
    auto callback = [weak = WeakClaim(this)](
                        bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->RebuildChild(isSingleHandle, hasToolBar, hasMenu, hasIcon, hasAnimation);
        }
    };
    renderTextOverlay->SetOnRebuild(callback);

    auto startAnimationCallback = [weak = WeakClaim(this)](const TweenOption& outerOption,
                                      const TweenOption& innerOption, bool isSingleHandle, bool isIn) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->OnStartAnimation(outerOption, innerOption, isSingleHandle, isIn);
        }
    };
    renderTextOverlay->SetStartAnimationCallback(startAnimationCallback);
}

void TextOverlayElement::OnFocus()
{
    auto renderOverlay = AceType::DynamicCast<RenderTextOverlay>(renderNode_);
    if (renderOverlay) {
        renderOverlay->OnFocusChange(RenderStatus::FOCUS);
    }
}

void TextOverlayElement::OnBlur()
{
    auto renderOverlay = AceType::DynamicCast<RenderTextOverlay>(renderNode_);
    if (renderOverlay) {
        renderOverlay->OnFocusChange(RenderStatus::BLUR);
    }
}

void TextOverlayElement::RebuildChild(
    bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation, bool buildToolBarOnly)
{
    if (!overlayComponent_) {
        return;
    }
    auto renderOverlay = AceType::DynamicCast<RenderTextOverlay>(renderNode_);
    if (!renderOverlay) {
        return;
    }

    if (!buildToolBarOnly) {
        const auto& child = children_.empty() ? nullptr : children_.front();
        UpdateChild(child, overlayComponent_->BuildChild(isSingleHandle, hasToolBar, hasMenu, hasIcon, hasAnimation));
        renderOverlay->MarkNeedLayout();
        return;
    }
    if (GetChildren().front()) {
        auto columnElement = GetChildren().front()->GetChildren().front();
        if (!columnElement) {
            return;
        }
        columnElement->UpdateChild(columnElement->GetFirstChild(),
            overlayComponent_->BuildToolBar(isSingleHandle, hasToolBar, hasMenu, hasIcon, hasAnimation));
        if (!columnElement->GetFirstChild()) {
            return;
        }
        auto renderBox = columnElement->GetFirstChild()->GetRenderNode();
        if (renderBox) {
            renderBox->MarkNeedLayout(true, false);
        }
    }
}

bool TextOverlayElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

void TextOverlayElement::OnStartAnimation(
    const TweenOption& outerOption, const TweenOption& innerOption, bool isSingleHandle, bool isIn)
{
    InitTween(AceType::Claim(this));

    // Start animation of outer tween.
    if (outerTween_) {
        AddListener(isSingleHandle, isIn);
        outerTween_->SetOption(outerOption);
        outerTween_->ApplyOptions();
        outerTween_->ApplyKeyframes();
        auto controller = outerTween_->GetController();
        if (controller) {
            controller->Play();
        }
    }

    // Start animation of inner tween.
    if (innerTween_) {
        innerTween_->SetOption(innerOption);
        innerTween_->ApplyOptions();
        innerTween_->ApplyKeyframes();
        if (innerTween_->GetController()) {
            innerTween_->GetController()->Play();
        }
    }

    // Show animation of menu is auto play, but hide animation should manual start.
    if (!isIn || !overlayComponent_) {
        return;
    }
    auto menu = overlayComponent_->GetMenu();
    if (menu) {
        auto menuAnimationCallback = menu->GetRefreshAnimationCallback();
        auto menuController = menu->GetAnimationController();
        if (menuAnimationCallback && menuController) {
            auto hideOption = menu->GetHideOption();
            hideOption.SetFillMode(FillMode::FORWARDS);
            menuAnimationCallback(hideOption, false);
            menuController->Play();
        }
    }
}

void TextOverlayElement::InitTween(const RefPtr<Element>& element)
{
    if (!element || (outerTween_ && innerTween_)) {
        return;
    }

    if (AceType::InstanceOf<TweenElement>(element)) {
        if (!outerTween_) {
            outerTween_ = AceType::DynamicCast<TweenElement>(element);
        } else if (!innerTween_) {
            innerTween_ = AceType::DynamicCast<TweenElement>(element);
        } else {
            return;
        }
    }

    for (const auto& child : element->GetChildren()) {
        InitTween(child);
    }
}

void TextOverlayElement::AddListener(bool isSingleHandle, bool isIn)
{
    if (!outerTween_) {
        return;
    }
    auto controller = outerTween_->GetController();
    if (!controller) {
        return;
    }
    controller->AddStartListener([weak = WeakClaim(this)]() {
        auto overlay = weak.Upgrade();
        if (overlay) {
            auto context = overlay->context_.Upgrade();
            if (context) {
                overlay->hasFocusAnimation_ = context->IsKeyEvent();
                // Don't show focus animation when show tween animation.
                context->SetIsKeyEvent(false);
            }
        }
    });

    // When animation is stop, rebuild child without animation.
    controller->AddStopListener([weak = WeakClaim(this), isSingleHandle, isIn]() {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->outerTween_.Reset();
            overlay->innerTween_.Reset();
            if (isIn) {
                overlay->RebuildChild(isSingleHandle, true, false, true, false, false);
            } else {
                overlay->RebuildChild(isSingleHandle, false, true, true, false, true);
            }
            auto context = overlay->context_.Upgrade();
            if (context && overlay->hasFocusAnimation_) {
                // When tween animation is stopped, restart focus animation.
                context->SetIsKeyEvent(true);
            }
        } else {
            LOGE("overlay is null");
            return;
        }
        auto renderOverlay = AceType::DynamicCast<RenderTextOverlay>(overlay->renderNode_);
        if (renderOverlay) {
            renderOverlay->SetIsAnimationStopped(true);
        }
    });

    auto renderOverlay = DynamicCast<RenderTextOverlay>(renderNode_);
    if (renderOverlay) {
        double offsetX = renderOverlay->GetHorizonOffsetForAnimation();
        double from = isIn ? offsetX : 0.0;
        double to = isIn ? 0.0 : offsetX;

        RefPtr<CurveAnimation<float>> offset = AceType::MakeRefPtr<CurveAnimation<float>>(from, to, Curves::FRICTION);
        offset->AddListener(Animation<float>::ValueCallback([count = 0, renderOverlay](double value) mutable {
            // Delay one frame to avoid flashing of text overlay when paintting.
            if (count > 1 && !renderOverlay->IsAnimationStarted()) {
                renderOverlay->SetIsAnimationStarted(true);
                renderOverlay->MarkNeedRender();
            }
            ++count;
        }));
        controller->AddInterpolator(offset);
    }
}

} // namespace OHOS::Ace