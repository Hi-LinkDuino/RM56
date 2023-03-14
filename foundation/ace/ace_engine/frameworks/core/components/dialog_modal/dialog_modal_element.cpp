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

#include "core/components/dialog_modal/dialog_modal_element.h"

#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/animation/curves.h"
#include "core/components/box/box_element.h"
#include "core/components/clip/clip_element.h"
#include "core/components/dialog_modal/render_dialog_modal.h"
#include "core/components/page/page_element.h"
#include "core/components/root/render_root.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t APP_TRANSITION_DURATION = 250;
constexpr double SCALE_BEGIN = 0.9;
constexpr double SCALE_END = 1.0;
constexpr double OPACITY_BEGIN = 0.0;
constexpr double OPACITY_END = 1.0;

Size GetPageLayoutSize(const RefPtr<PageElement>& page)
{
    if (!page) {
        LOGE("Get page layout size failed. page is null.");
        return Size();
    }
    auto render = page->GetRenderNode();
    if (!render) {
        LOGE("Get page layout size failed. render is null.");
        return Size();
    }
    return render->GetLayoutSize();
}

} // namespace

RefPtr<OverlayElement> DialogModalElement::GetOverlayElement() const
{
    auto tween = AceType::DynamicCast<TweenElement>(GetFirstChild());
    if (!tween) {
        LOGE("Get overlay element failed. Tween element is null!");
        return RefPtr<OverlayElement>();
    }
    auto box = AceType::DynamicCast<BoxElement>(tween->GetContentElement());
    if (!box) {
        LOGE("Get overlay element failed. Box element is null!");
        return RefPtr<OverlayElement>();
    }
    auto clip = AceType::DynamicCast<ClipElement>(box->GetFirstChild());
    if (!clip) {
        LOGE("Get overlay element failed. Clip element is null!");
        return RefPtr<OverlayElement>();
    }
    auto bgBox = AceType::DynamicCast<BoxElement>(clip->GetFirstChild());
    if (!bgBox) {
        LOGE("Get overlay element failed. bgBox element is null!");
        return RefPtr<OverlayElement>();
    }
    auto stack = bgBox->GetFirstChild();
    if (!stack) {
        return RefPtr<OverlayElement>();
    }
    auto child = stack->GetChildren();
    if (child.size() > 1) {
        auto it = child.begin();
        it++;
        return AceType::DynamicCast<OverlayElement>(*it);
    }
    return RefPtr<OverlayElement>();
}

RefPtr<StageElement> DialogModalElement::GetStageElement() const
{
    auto tween = AceType::DynamicCast<TweenElement>(GetFirstChild());
    if (!tween) {
        LOGE("Get overlay element failed. Tween element is null!");
        return RefPtr<StageElement>();
    }
    auto box = AceType::DynamicCast<BoxElement>(tween->GetContentElement());
    if (!box) {
        LOGE("Get overlay element failed. Box element is null!");
        return RefPtr<StageElement>();
    }
    auto clip = AceType::DynamicCast<ClipElement>(box->GetFirstChild());
    if (!clip) {
        LOGE("Get overlay element failed. Clip element is null!");
        return RefPtr<StageElement>();
    }
    auto bgBox = AceType::DynamicCast<BoxElement>(clip->GetFirstChild());
    if (!bgBox) {
        LOGE("Get overlay element failed. bgBox element is null!");
        return RefPtr<StageElement>();
    }
    auto stack = bgBox->GetFirstChild();
    if (!stack) {
        return RefPtr<StageElement>();
    }
    return AceType::DynamicCast<StageElement>(stack->GetFirstChild());
}

void DialogModalElement::UpdateSystemBarHeight(double statusBar, double navigationBar)
{
    auto renderNode = AceType::DynamicCast<RenderDialogModal>(GetRenderNode());
    if (!renderNode) {
        return;
    }
    renderNode->UpdateSystemBarHeight(statusBar, navigationBar);
}

void DialogModalElement::RegisterTransitionListener()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Register Transition Listener failed. context is null.");
        return;
    }
    context->AddPageTransitionListener([weak = AceType::WeakClaim(this)](const TransitionEvent& event,
                                           const WeakPtr<PageElement>& in, const WeakPtr<PageElement>& out) {
        auto dialog = weak.Upgrade();
        if (!dialog) {
            LOGE("Handle transition event failed. dialog is null.");
            return;
        }
        auto dialogRender = AceType::DynamicCast<RenderDialogModal>(dialog->GetRenderNode());
        if (!dialogRender) {
            LOGE("Handle transition event failed. dialog render is null.");
            return;
        }
        if (event == TransitionEvent::POP_START) {
            auto page = AceType::DynamicCast<PageElement>(out.Upgrade());
            if (!page) {
                LOGE("Handle pop transition event failed. page out is null.");
                return;
            }
            auto pageHeight = GetPageLayoutSize(page).Height();
            dialogRender->AnimateTo(pageHeight, true);
        } else if (event == TransitionEvent::PUSH_START) {
            auto page = AceType::DynamicCast<PageElement>(in.Upgrade());
            if (!page) {
                // push first page.
                dialogRender->SetLayoutNotify([weak]() {
                    auto dialog = weak.Upgrade();
                    if (dialog) {
                        dialog->AnimateToEnterApp();
                    }
                });
                return;
            }
            auto render = page->GetRenderNode();
            if (!render) {
                LOGE("Handle push transition event failed. page render is null.");
                return;
            }
            auto fullSize = render->GetLayoutSize();
            auto pageHeight = fullSize.Height();
            dialogRender->AnimateTo(pageHeight, false);
        }
    });
}

void DialogModalElement::PerformBuild()
{
    SoleChildElement::PerformBuild();
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        controller_->SetDuration(APP_TRANSITION_DURATION);
        controller_->SetFillMode(FillMode::FORWARDS);
        auto scale = MakeRefPtr<CurveAnimation<float>>(SCALE_BEGIN, SCALE_END, Curves::FRICTION);
        scale->SetReverseCurve(Curves::FAST_OUT_SLOW_IN);
        auto opacity = MakeRefPtr<CurveAnimation<float>>(OPACITY_BEGIN, OPACITY_END, Curves::FAST_OUT_SLOW_IN);
        option_.SetTransformFloatAnimation(AnimationType::SCALE, scale);
        option_.SetOpacityAnimation(opacity);

        auto tween = AceType::DynamicCast<TweenElement>(GetFirstChild());
        if (!tween) {
            LOGE("Dialog Modal perform build failed. tween is null.");
            return;
        }
        tween->SetController(controller_);
        tween->SetOption(option_);
        tween->ApplyKeyframes();
        tween->SetOpacity(OPACITY_BEGIN);
    }
}

void DialogModalElement::CreateOriginAnimation()
{
    auto dialogRender = AceType::DynamicCast<RenderDialogModal>(GetRenderNode());
    if (!dialogRender) {
        LOGE("Create origin animation failed. dialog Render is null.");
        return;
    }
    const auto& clip = dialogRender->GetRenderClip();
    if (!clip) {
        LOGE("Create origin animation failed. clip is null.");
        return;
    }
    option_.ClearListeners();
    if (controller_) {
        controller_->ClearInterpolators();
    }
    const auto& clipRect = clip->GetClipRect(clip->GetPaintRect().GetOffset());
    double centerX = clipRect.GetOffset().GetX() + clipRect.Width() / 2.0;
    double centerY = clipRect.GetOffset().GetY() + clipRect.Height() / 2.0;
    option_.SetTransformOrigin(Dimension(centerX), Dimension(centerY));
    auto tween = AceType::DynamicCast<TweenElement>(GetFirstChild());
    if (!tween) {
        LOGE("Create origin animation failed. tween is null.");
        return;
    }
    tween->SetOption(option_);
    tween->ApplyKeyframes();
}

void DialogModalElement::AnimateToEnterApp()
{
    if (!controller_) {
        LOGE("Animate To Enter App failed. controller is null.");
        return;
    }
    controller_->RemoveStopListener(stopCallbackId_);
    CreateOriginAnimation();
    controller_->Forward();
    auto rootElement = GetElementParent().Upgrade();
    if (!rootElement) {
        LOGE("Animate To Enter App failed. root element is null.");
        return;
    }
    auto root = AceType::DynamicCast<RenderRoot>(rootElement->GetRenderNode());
    if (!root) {
        LOGE("Animate To Enter App failed. render root is null.");
        return;
    }
    root->AnimateToShow(APP_TRANSITION_DURATION);
}

void DialogModalElement::AnimateToExitApp()
{
    if (!controller_) {
        LOGE("Animate To Exit App failed. controller is null.");
        return;
    }
    controller_->RemoveStopListener(stopCallbackId_);
    CreateOriginAnimation();
    controller_->Backward();
    stopCallbackId_ = controller_->AddStopListener([contextWeak = context_]() {
        auto context = contextWeak.Upgrade();
        if (!context) {
            LOGE("Handle dialog modal exit transition failed. context is null.");
            return;
        }
        // force finish
        context->Finish(false);
    });
    auto rootElement = GetElementParent().Upgrade();
    if (!rootElement) {
        LOGE("Animate To Exit App failed. root element is null.");
        return;
    }
    auto root = AceType::DynamicCast<RenderRoot>(rootElement->GetRenderNode());
    if (!root) {
        LOGE("Animate To Exit App failed. render root is null.");
        return;
    }
    root->AnimateToHide(APP_TRANSITION_DURATION);
}

} // namespace OHOS::Ace
