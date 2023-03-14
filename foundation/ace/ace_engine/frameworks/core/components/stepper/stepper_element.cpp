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

#include "core/components/stepper/stepper_element.h"

#include "core/components/stepper/render_stepper.h"
#include "core/components/stepper/stepper_component.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ELEMENT_CHANGE_END_LISTENER_KEY = 1002;
constexpr int32_t RIGHT_BUTTON_INDEX = 1;
constexpr int32_t DEFAULT_BUTTON_NUM = 2;

} // namespace

void StepperElement::PerformBuild()
{
    if (!leftFocusNode_ && !rightFocusNode_) {
        leftFocusNode_ = AceType::MakeRefPtr<FocusNode>();
        FocusGroup::AddChild(leftFocusNode_);
        rightFocusNode_ = AceType::MakeRefPtr<FocusNode>();
        FocusGroup::AddChild(rightFocusNode_);
        registerCallBack();
    }
    ComponentGroupElement::PerformBuild();
}

void StepperElement::registerCallBack()
{
    leftFocusNode_->SetOnClickCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonClick(true);
        }
    });

    rightFocusNode_->SetOnClickCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonClick(false);
        }
    });

    leftFocusNode_->SetOnFocusCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonFocus(true, true);
        }
    });

    rightFocusNode_->SetOnFocusCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonFocus(true, false);
        }
    });

    leftFocusNode_->SetOnBlurCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonFocus(false, true);
        }
    });

    rightFocusNode_->SetOnBlurCallback([weak = WeakClaim(this)](void) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleOnButtonFocus(false, false);
        }
    });
}

RefPtr<RenderNode> StepperElement::CreateRenderNode()
{
    return ComponentGroupElement::CreateRenderNode();
}

void StepperElement::OnFocus()
{
    auto stepper = DynamicCast<RenderStepper>(renderNode_);
    if (!stepper) {
        LOGE("get render node failed");
        itLastFocusNode_ = focusNodes_.end();
        return;
    }
    stepper->OnStatusChanged(RenderStatus::FOCUS);

    auto currentFocusNode = focusNodes_.begin();
    std::advance(currentFocusNode, RIGHT_BUTTON_INDEX);
    if ((*currentFocusNode)->RequestFocusImmediately()) {
        itLastFocusNode_ = currentFocusNode;
        rightFocusNode_ = *itLastFocusNode_;
    }
    stepper->RegisterChangeEndListener(
        ELEMENT_CHANGE_END_LISTENER_KEY, [this](int32_t index) { RequestChildFocus(index); });
}

void StepperElement::RequestChildFocus(int32_t index)
{
    auto currentFocusNode = focusNodes_.begin();
    std::advance(currentFocusNode, index + DEFAULT_BUTTON_NUM);
    if (currentFocusNode != focusNodes_.end()) {
        if ((*currentFocusNode)->RequestFocusImmediately()) {
            itLastFocusNode_ = currentFocusNode;
        }
    }
}

void StepperElement::OnBlur()
{
    FocusGroup::OnBlur();
    auto stepper = DynamicCast<RenderStepper>(renderNode_);
    if (stepper) {
        stepper->UnRegisterChangeEndListener(ELEMENT_CHANGE_END_LISTENER_KEY);
        stepper->OnStatusChanged(RenderStatus::BLUR);
    }
}

void StepperElement::HandleOnButtonClick(bool isLeft)
{
    auto stepper = DynamicCast<RenderStepper>(renderNode_);
    if (!stepper) {
        LOGE("get stepper render node failed");
        return;
    }
    stepper->HandleButtonClick(isLeft);
}

void StepperElement::HandleOnButtonFocus(bool focus, bool isLeft)
{
    auto stepper = DynamicCast<RenderStepper>(renderNode_);
    if (!stepper) {
        LOGE("get stepper render node failed");
        return;
    }
    stepper->UpdateButtonFocus(focus, isLeft);
}

bool StepperElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    if (*itLastFocusNode_ == leftFocusNode_) {
        if (reverse) {
            return RequestCurrentItemFocus();
        } else {
            return RequestRightButtonFocus();
        }
    } else if (*itLastFocusNode_ == rightFocusNode_) {
        if (vertical && reverse) {
            return RequestCurrentItemFocus();
        }
        if (!vertical && reverse) {
            return RequestLeftButtonFocus();
        }
    } else {
        if (vertical && !reverse) {
            return RequestLeftButtonFocus();
        }
    }
    return false;
}

bool StepperElement::RequestLeftButtonFocus()
{
    auto currentFocusNode = focusNodes_.begin();
    if ((*currentFocusNode)->RequestFocusImmediately()) {
        itLastFocusNode_ = currentFocusNode;
        leftFocusNode_ = *itLastFocusNode_;
        return true;
    }
    return false;
}

bool StepperElement::RequestRightButtonFocus()
{
    auto currentFocusNode = focusNodes_.begin();
    std::advance(currentFocusNode, 1);
    if ((*currentFocusNode)->RequestFocusImmediately()) {
        itLastFocusNode_ = currentFocusNode;
        rightFocusNode_ = *itLastFocusNode_;
        return true;
    }
    return false;
}

bool StepperElement::RequestCurrentItemFocus()
{
    auto stepper = DynamicCast<RenderStepper>(renderNode_);
    if (!stepper) {
        LOGE("get stepper render node failed");
        return false;
    }
    int32_t currentIndex = stepper->GetCurrentIndex();
    auto currentFocusNode = focusNodes_.begin();
    std::advance(currentFocusNode, currentIndex + DEFAULT_BUTTON_NUM);
    if (currentFocusNode != focusNodes_.end()) {
        if ((*currentFocusNode)->RequestFocusImmediately()) {
            itLastFocusNode_ = currentFocusNode;
            return true;
        }
    }
    return false;
}

bool StepperElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (!IsCurrentFocus()) {
        return false;
    }

    if (itLastFocusNode_ != focusNodes_.end() && (*itLastFocusNode_)->HandleKeyEvent(keyEvent)) {
        return true;
    }

    if (FocusNode::OnKeyEvent(keyEvent)) {
        return true;
    }

    if (keyEvent.action != KeyAction::UP) {
        return false;
    }

    switch (keyEvent.code) {
        case KeyCode::TV_CONTROL_UP:
            return RequestNextFocus(true, true, GetRect());
        case KeyCode::TV_CONTROL_DOWN:
            return RequestNextFocus(true, false, GetRect());
        case KeyCode::TV_CONTROL_LEFT:
            return RequestNextFocus(false, true, GetRect());
        case KeyCode::TV_CONTROL_RIGHT:
            return RequestNextFocus(false, false, GetRect());
        case KeyCode::KEY_TAB:
            return RequestNextFocus(false, false, GetRect()) || RequestNextFocus(true, false, GetRect());
        default:
            return false;
    }
}

} // namespace OHOS::Ace
