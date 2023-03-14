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

#include "core/components/dialog_tween/dialog_tween_element.h"

#include "core/components/dialog_tween/dialog_tween_component.h"
#include "core/components/dialog_tween/render_dialog_tween.h"
#include "core/components/transition/transition_element.h"

namespace OHOS::Ace {

void DialogTweenElement::PerformBuild()
{
    RefPtr<DialogTweenComponent> dialogTween = AceType::DynamicCast<DialogTweenComponent>(component_);
    if (!dialogTween) {
        return;
    }
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, dialogTween->GetChild());
    auto animator = dialogTween->GetAnimator();
    if (!animator) {
        return;
    }
    auto transitionElement = AceType::DynamicCast<TransitionElement>(GetChildren().front());
    if (!transitionElement) {
        return;
    }
    auto renderDialogTween = AceType::DynamicCast<RenderDialogTween>(renderNode_);
    animator->AddStartListener([weakDialog = AceType::WeakClaim(AceType::RawPtr(renderDialogTween))] {
        auto renderDialogTween = weakDialog.Upgrade();
        if (renderDialogTween) {
            renderDialogTween->SetDisableTouchEvent(true);
        }
    });
    animator->AddStopListener([weakDialog = AceType::WeakClaim(AceType::RawPtr(renderDialogTween)),
                                  weakTransition = AceType::WeakClaim(AceType::RawPtr(transitionElement)),
                                  weakController = WeakClaim(RawPtr(animator))] {
        auto renderDialogTween = weakDialog.Upgrade();
        if (renderDialogTween) {
            renderDialogTween->SetDisableTouchEvent(false);
        }
        auto transitionElement = weakTransition.Upgrade();
        if (transitionElement) {
            auto animator = weakController.Upgrade();
            if (animator) {
                animator->ClearInterpolators();
            }
            transitionElement->SwitchTransitionOption(TransitionOptionType::TRANSITION_OUT, true);
        }
    });

    transitionElement->SetController(animator);
    transitionElement->SwitchTransitionOption(TransitionOptionType::TRANSITION_IN, true);
}

bool DialogTweenElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.code == KeyCode::KEY_BACK && keyEvent.action == KeyAction::UP) {
        const auto& dialog = AceType::DynamicCast<RenderDialogTween>(renderNode_);
        if (dialog) {
            return dialog->PopDialog();
        }
        return false;
    }
    return FocusGroup::OnKeyEvent(keyEvent);
}

bool DialogTweenElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

} // namespace OHOS::Ace