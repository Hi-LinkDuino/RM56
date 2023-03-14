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

#include "core/components/dialog_modal/render_dialog_modal.h"

#include "base/log/event_report.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/dialog_modal/dialog_modal_component.h"
#include "core/components/page/page_element.h"
#include "core/components/root/render_root.h"
#include "core/components/stage/stage_element.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension BG_MARGIN = 16.0_vp;
constexpr int32_t TRANSITION_DURATION = 350;
constexpr double MAX_HEIGHT_PERCENT = 0.8;

} // namespace

RefPtr<RenderNode> RenderDialogModal::Create()
{
    return AceType::MakeRefPtr<RenderDialogModal>();
}

void RenderDialogModal::Update(const RefPtr<Component>& component)
{
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    MarkNeedLayout();
}

void RenderDialogModal::PerformLayout()
{
    auto child = GetFirstChild();
    if (!child) {
        LOGE("Child is null!");
        return;
    }
    auto statusBarPx = NormalizeToPx(Dimension(statusBarHeight_, DimensionUnit::VP));
    auto navigationBarPx = NormalizeToPx(Dimension(navigationBarHeight_, DimensionUnit::VP));
    auto innerLayoutParam = GetLayoutParam();
    auto maxSize = GetLayoutParam().GetMaxSize();
    bool exceeds = false;
    double dialogHeight = maxSize.Height() * MAX_HEIGHT_PERCENT + NormalizeToPx(BG_MARGIN) * 2;
    if (dialogHeight + statusBarPx + navigationBarPx > maxSize.Height()) {
        // exceeds total height.
        exceeds = true;
        dialogHeight = maxSize.Height() - navigationBarPx - NormalizeToPx(BG_MARGIN);
    }
    maxSize.SetHeight(dialogHeight);
    // Layout as max as possible.
    innerLayoutParam.SetMaxSize(maxSize);
    innerLayoutParam.SetMinSize(Size(maxSize.Width(), innerLayoutParam.GetMinSize().Height()));
    viewPort_.SetHeight(maxSize.Height() - NormalizeToPx(BG_MARGIN) * 2);
    viewPort_.SetWidth(maxSize.Width() - NormalizeToPx(BG_MARGIN) * 2);
    child->Layout(innerLayoutParam);
    auto childY = GetLayoutParam().GetMaxSize().Height() - navigationBarPx - maxSize.Height();
    child->SetPosition(Offset(0.0, childY));
    Size selfSize;
    if (!exceeds) {
        selfSize = Size(GetLayoutParam().GetMaxSize().Width(),
            GetLayoutParam().GetMaxSize().Height() - statusBarPx - navigationBarPx);
    } else {
        selfSize = Size(GetLayoutParam().GetMaxSize().Width(),
                        dialogHeight - statusBarPx);
    }
    SetLayoutSize(selfSize);
    PerformClip();
    if (notify_) {
        decltype(notify_) notify = std::move(notify_);
        notify();
    }
}

void RenderDialogModal::UpdateSystemBarHeight(double statusBar, double navigationBar)
{
    statusBarHeight_ = statusBar;
    double delta = NormalizeToPx(Dimension(navigationBar - navigationBarHeight_, DimensionUnit::VP));
    navigationBarHeight_ = navigationBar;
    MovePage(delta);
    MarkNeedLayout();
}

void RenderDialogModal::AnimateTo(double pageHeight, bool reverse)
{
    animateTargetHeight_ = pageHeight;
    animatingPush_ = !reverse;
    if (controller_->IsRunning()) {
        controller_->Stop();
        lastAnimateFrame_ = false;
    }
    controller_->ClearInterpolators();
    controller_->ClearAllListeners();
    auto clip = GetRenderClip();
    if (!clip) {
        LOGE("AnimateTo failed. render clip is null.");
        EventReport::SendRenderException(RenderExcepType::CLIP_ERR);
        return;
    }
    double from = clip->GetHeight();
    auto keyframeFrom = AceType::MakeRefPtr<Keyframe<double>>(0.0, reverse ? pageHeight : from);
    auto keyframeTo = AceType::MakeRefPtr<Keyframe<double>>(1.0, reverse ? from : pageHeight);
    auto heightAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    heightAnimation->AddKeyframe(keyframeFrom);
    heightAnimation->AddKeyframe(keyframeTo);
    heightAnimation->SetCurve(Curves::FRICTION);

    heightAnimation->AddListener([weak = AceType::WeakClaim(this)](const double& height) {
        auto dialogModal = weak.Upgrade();
        if (!dialogModal) {
            LOGE("Semi modal is null.");
            return;
        }
        if (LessNotEqual(height, 0.0)) {
            LOGE("Height less than zero, do not animate it.");
            return;
        }
        dialogModal->animatingPageHeight_ = height;
        dialogModal->MarkNeedLayout();
    });
    controller_->AddInterpolator(heightAnimation);
    controller_->SetDuration(TRANSITION_DURATION);
    controller_->SetFillMode(FillMode::FORWARDS);
    if (reverse) {
        controller_->Backward();
    } else {
        controller_->Forward();
    }
    controller_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto dialog = weak.Upgrade();
        if (!dialog) {
            return;
        }
        dialog->lastAnimateFrame_ = true;
    });
}

Size RenderDialogModal::GetTopPageLayoutSize() const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Get Top page layout Size failed. context is null");
        return Size();
    }
    auto stage = context->GetStageElement();
    if (!stage) {
        LOGE("Get Top page layout Size failed. stage is null");
        return Size();
    }
    auto topPage = stage->GetLastChild();
    if (!topPage) {
        LOGE("Get Top page layout Size failed. topPage is null");
        return Size();
    }
    auto render = topPage->GetRenderNode();
    if (!render) {
        LOGE("Get Top page layout Size failed. render is null");
        return Size();
    }
    return render->GetLayoutSize();
}

bool RenderDialogModal::CanRouterPage() const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Query can router page failed. context is null.");
        return false;
    }
    auto stage = context->GetStageElement();
    if (!stage) {
        LOGE("Query can router page failed. stage is null.");
        return false;
    }
    return stage->CanRouterPage();
}

void RenderDialogModal::PerformClip()
{
    auto clip = GetRenderClip();
    if (!clip) {
        LOGE("Perform build failed. render clip is null");
        EventReport::SendRenderException(RenderExcepType::CLIP_ERR);
        return;
    }
    double pageHeight = 0.0;
    if (IsAnimating()) {
        pageHeight = animatingPageHeight_;
        if (!NearEqual(GetTopPageLayoutSize().Height(), animateTargetHeight_) && animatingPush_) {
            // Page height has changed during push page animation, change animation target height.
            AnimateTo(GetTopPageLayoutSize().Height(), false);
        }
        lastAnimateFrame_ = false;
    } else {
        if (!CanRouterPage()) {
            LOGD("Stage in page transition, do nothing.");
            return;
        }
        pageHeight = GetTopPageLayoutSize().Height();
    }
    clip->SetOffsetY(clip->GetLayoutSize().Height() - pageHeight);
    clip->SetHeight(pageHeight);
    clip->MarkNeedRender();
}

RefPtr<RenderClip> RenderDialogModal::GetRenderClip() const
{
    auto transform = GetFirstChild();
    if (!transform) {
        LOGE("Get Clip Render failed. transform is null.");
        return nullptr;
    }
    auto display = transform->GetFirstChild();
    if (!display) {
        LOGE("Get Clip Render failed. transform is null.");
        return nullptr;
    }
    auto box = display->GetFirstChild();
    if (!box) {
        LOGE("Get Clip Render failed. box is null.");
        return nullptr;
    }
    return AceType::DynamicCast<RenderClip>(box->GetFirstChild());
}

void RenderDialogModal::MovePage(double delta)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Move failed, context is null");
        return;
    }
    auto clip = GetRenderClip();
    if (!clip) {
        LOGE("Move failed, clip is null");
        return;
    }
    double newHeight = clip->GetGlobalOffset().GetY() + clip->GetPaintRect().Height() - delta;
    context->MovePage(Offset(clip->GetPaintRect().Width(), newHeight), delta);
}

} // namespace OHOS::Ace
