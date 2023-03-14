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

#include "core/components/dialog/dialog_element.h"

#include "core/components/dialog/dialog_component.h"
#include "core/components/dialog/render_dialog.h"
#include "core/components/transition/transition_element.h"

namespace OHOS::Ace {

void DialogElement::PerformBuild()
{
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component_);
    if (!dialog) {
        return;
    }
    dialogId_ = dialog->GetDialogId();
    dialog->BuildChild(GetThemeManager());
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, dialog->GetChild());
    const auto& animator = dialog->GetAnimator();
    if (!animator) {
        return;
    }
    auto transitionElement = AceType::DynamicCast<TransitionElement>(GetChildren().front());
    if (!transitionElement) {
        return;
    }
    animator->AddStopListener([weakTransition = AceType::WeakClaim(AceType::RawPtr(transitionElement)),
        weakController = WeakClaim(RawPtr(animator))] {
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
    animator->Play();
}

bool DialogElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

} // namespace OHOS::Ace