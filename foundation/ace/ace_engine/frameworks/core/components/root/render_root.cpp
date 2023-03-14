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

#include "core/components/root/render_root.h"

#include "root_component.h"
#include "base/log/log.h"
#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/components/theme/app_theme.h"

namespace OHOS::Ace {
namespace {

bool IsShadowModal(const WeakPtr<PipelineContext>& contextWeak)
{
    auto context = contextWeak.Upgrade();
    if (!context) {
        return false;
    }
    auto modal = context->GetWindowModal();
    return (modal == WindowModal::DIALOG_MODAL) || (modal == WindowModal::SEMI_MODAL) ||
           (modal == WindowModal::SEMI_MODAL_FULL_SCREEN);
}

} // namespace

RenderRoot::RenderRoot() : RenderNode(true) {}

void RenderRoot::Update(const RefPtr<Component>& component)
{
    if (!controller_ && IsShadowModal(context_)) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        controller_->SetFillMode(FillMode::FORWARDS);
    }
    auto root = AceType::DynamicCast<RootComponent>(component);
    if (root) {
        isContextMenu_ = root->IsContextMenu();
    }
    MarkNeedLayout();
}

void RenderRoot::PerformLayout()
{
    auto appTheme = GetTheme<AppTheme>();
    // Use theme background color to clear canvas.
    if (appTheme && !isBgColorInit_) {
        bgColor_ = appTheme->GetBackgroundColor();
        if (IsShadowModal(context_)) {
            auto colorAnimation =
                AceType::MakeRefPtr<CurveAnimation<Color>>(Color::TRANSPARENT, bgColor_, Curves::FAST_OUT_SLOW_IN);
            bgColor_ = Color::TRANSPARENT;
            colorAnimation->AddListener([weak = AceType::WeakClaim(this)](const Color& color) {
                auto root = weak.Upgrade();
                if (!root) {
                    LOGE("Root transition failed. root is null.");
                    return;
                }
                if (!root->forceColor_) {
                    root->bgColor_ = color;
                    root->MarkNeedRender();
                }
            });
            controller_->AddInterpolator(colorAnimation);
        }
        if (isContextMenu_) {
            bgColor_ = Color::TRANSPARENT;
        }
        isBgColorInit_ = true;
    }
    viewPort_ = GetPaintRect().GetSize();
    LayoutParam innerLayoutParam;
    innerLayoutParam.SetMaxSize(GetPaintRect().GetSize());

    for (auto& child : GetChildren()) {
        child->Layout(innerLayoutParam);
        child->SetPosition(Offset::Zero());
    }
}

void RenderRoot::DumpLayerTree() {}

void RenderRoot::AnimateToHide(int32_t duration)
{
    if (controller_) {
        controller_->SetDuration(duration);
        controller_->Backward();
    }
}

void RenderRoot::AnimateToShow(int32_t duration)
{
    if (controller_) {
        controller_->SetDuration(duration);
        controller_->Forward();
    }
}

void RenderRoot::SetDefaultBgColor()
{
    auto appTheme = GetTheme<AppTheme>();
    if (!appTheme) {
        return;
    }
    bgColor_ = isContextMenu_ ? Color::TRANSPARENT : appTheme->GetBackgroundColor();
    forceColor_ = false;
    MarkNeedRender();
}

void RenderRoot::SetBgColor(const Color& color)
{
    bgColor_ = isContextMenu_ ? Color::TRANSPARENT : color;
    forceColor_ = true;
    MarkNeedRender();
}

} // namespace OHOS::Ace
