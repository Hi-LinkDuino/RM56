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

#include "core/components/popup/popup_element.h"

#include "base/utils/string_utils.h"
#include "core/components/box/render_box.h"
#include "core/components/bubble/bubble_component.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/popup/popup_theme.h"
#include "core/components/stack/stack_element.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {

void PopupElement::PerformBuild()
{
    RefPtr<PopupComponent> popupComponent = AceType::DynamicCast<PopupComponent>(component_);
    if (!popupComponent) {
        return;
    }
    popup_ = popupComponent;
    const auto& popupController = popupComponent->GetPopupController();
    if (!popupController) {
        return;
    }
    popupController->SetShowPopupImpl([weak = WeakClaim(this)]() {
        auto popupElement = weak.Upgrade();
        if (popupElement) {
            popupElement->ShowPopup();
        }
    });
    popupController->SetCancelPopupImpl([weak = WeakClaim(this)]() {
        auto popupElement = weak.Upgrade();
        if (popupElement) {
            popupElement->CancelPopup(popupElement->GetId());
        }
    });
}

bool PopupElement::ShowPopup()
{
    const auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    if (!popup_ || popup_->GetPopupParam()->GetTargetId().empty()) {
        return false;
    }

    if (!GetThemeManager()) {
        return false;
    }

    auto theme = GetThemeManager()->GetTheme<PopupTheme>();
    auto showAlphaAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, 1.0f, Curves::FAST_OUT_SLOW_IN);

    TweenOption showOption;
    showOption.SetDuration(theme->GetShowTime());
    showOption.SetOpacityAnimation(showAlphaAnimation);
    RefPtr<TweenComponent> tween = AceType::MakeRefPtr<TweenComponent>(popup_->GetId(), popup_->GetId());
    tween->SetShadow(ShadowConfig::DefaultShadowM);
    tween->SetIsFirstFrameShow(false);
    tween->SetAnimationOperation(AnimationOperation::PLAY);
    tween->SetTweenOption(showOption);

    RefPtr<BubbleComponent> bubble = AceType::MakeRefPtr<BubbleComponent>(popup_->GetChild());
    bubble->SetPopupParam(popup_->GetPopupParam());
    bubble->SetId(popup_->GetId());
    bubble->SetDisabledStatus(popup_->IsDisabledStatus());
    bubble->SetStateChangeEvent([weak = WeakClaim(this)](bool isVisible) {
                auto popup = weak.Upgrade();
                if (popup) {
                    popup->OnStateChange(isVisible);
                }
            });

    auto stackElement = context->GetLastStack();
    if (!stackElement) {
        return false;
    }
    weakStack_ = WeakClaim(RawPtr(stackElement));
    bubble->SetWeakStack(weakStack_);
    tween->SetChild(bubble);

    stackElement->PushComponent(tween, false);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto manager = context->GetAccessibilityManager();
    if (manager) {
        auto node = manager->GetAccessibilityNodeById(StringUtils::StringToInt(popup_->GetId()));
        if (!node) {
            return true;
        }
        node->SetZIndexToChild(stackElement->GetChildrenSize());
        manager->ClearNodeRectInfo(node, false);
    }
#endif
    return true;
}

bool PopupElement::CancelPopup(const ComposeId& id)
{
    auto stackElement = weakStack_.Upgrade();
    if (!stackElement) {
        return false;
    }
    stackElement->PopPopup(id);

    auto context = context_.Upgrade();
    if (context) {
        const auto& accessibilityManager = context->GetAccessibilityManager();
        if (accessibilityManager) {
            accessibilityManager->RemoveAccessibilityNodeById(StringUtils::StringToInt(popup_->GetId()));
        }
    }
    return true;
}

} // namespace OHOS::Ace