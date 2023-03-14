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

#include "core/components/checkable/render_checkbox.h"

#include "base/log/event_report.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DEFUALT_CHECKBOX_ANIMATION_DURATION = 150;
constexpr double DEFAULT_MAX_CHECKBOX_SHAPE_SCALE = 1.0;
constexpr double DEFAULT_MID_CHECKBOX_SHAPE_SCALE = 0.5;
constexpr double DEFAULT_MIN_CHECKBOX_SHAPE_SCALE = 0.0;

const std::vector<double> CHECKBOX_SCALE = {
    DEFAULT_MAX_CHECKBOX_SHAPE_SCALE, DEFAULT_MID_CHECKBOX_SHAPE_SCALE, DEFAULT_MIN_CHECKBOX_SHAPE_SCALE
};
const std::vector<CheckableStatus> CHECKABLE_STATUS = {
    CheckableStatus::ALL, CheckableStatus::PART, CheckableStatus::NONE
};
} // namespace

void RenderCheckbox::Update(const RefPtr<Component>& component)
{
    RenderCheckable::Update(component);
    const auto& checkbox = AceType::DynamicCast<CheckboxComponent>(component);
    if (!checkbox) {
        LOGE("cast to checkbox component failed");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    component_ = checkbox;

    auto context = context_.Upgrade();
    if (context->GetIsDeclarative()) {
        UpdateGroupStatus();
        component_->SetGroupValue(CHECKABLE_STATUS[static_cast<int32_t>(status_)]);

        checkbox->SetGroupValueUpdateHandler([weak = AceType::WeakClaim(this)](CheckableStatus checked) {
            auto renderCheckbox = weak.Upgrade();
            if (renderCheckbox && renderCheckbox->UpdateGroupValue(checked)) {
                renderCheckbox->MarkNeedRender();
            }
        });
        checkbox->SetItemValueUpdateHandler([weak = AceType::WeakClaim(this)](bool checked) {
            auto renderCheckbox = weak.Upgrade();
            if (renderCheckbox && renderCheckbox->UpdateItemValue(checked)) {
                renderCheckbox->MarkNeedRender();
            }
        });

        onGroupChange_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            checkbox->GetOnGroupChange(), context_);
    }

    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        auto weak = AceType::WeakClaim(this);
        controller_->AddStopListener(Animator::StatusCallback([weak]() {
            auto checkBox = weak.Upgrade();
            if (checkBox) {
                checkBox->OnAnimationStop();
            }
        }));
    }

    auto theme = GetTheme<CheckboxTheme>();
    if (theme) {
        borderWidth_ = theme->GetBorderWidth();
        borderRadius_ = theme->GetBorderRadius();
        checkStroke_ = theme->GetCheckStroke();
    }
    if (checkbox->GetUpdateType() == UpdateType::ALL) {
        checked_ = checkbox->GetValue();
    }
    UpdateUIStatus();
    UpdateAccessibilityAttr();
}


bool RenderCheckbox::UpdateItemValue(const bool itemValue)
{

    if (!(component_->GetCheckboxName().empty())) {
        UpdateUIStatus();
        UpdateAnimation();
        controller_->Play();
    } else {
        return false;
    }

    return true;
}

bool RenderCheckbox::UpdateGroupValue(const CheckableStatus groupValue)
{
    if (!(component_->GetGroupName().empty())) {
        UpdateGroupStatus();
        if (onGroupChange_) {
            std::vector<std::string> result;
            component_->GetSelectedCheckBoxName(result);
            onGroupChange_(std::make_shared<CheckboxGroupResult>(result, static_cast<int32_t>(status_)));
        }
        if (component_->GetGroupValue() != groupValue) {
            component_->SetGroupValue(groupValue);
            lastStatus_ = status_;
            UpdateUIStatus();
            UpdateAnimation();
            controller_->Play();
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void RenderCheckbox::UpdateAccessibilityAttr()
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetCheckedState(checked_);
    if (accessibilityNode->GetClicked()) {
        accessibilityNode->SetClicked(false);
        auto context = context_.Upgrade();
        if (context) {
            AccessibilityEvent checkboxEvent;
            checkboxEvent.nodeId = accessibilityNode->GetNodeId();
            checkboxEvent.eventType = "click";
            context->SendEventToAccessibility(checkboxEvent);
        }
    }
}

void RenderCheckbox::HandleClick()
{
    auto context = context_.Upgrade();
    if (context->GetIsDeclarative()) {
        if (!(component_->GetGroupName().empty())) {
            lastStatus_ = status_;
            auto value = (component_->GetGroupValue() ==
                CheckableStatus::ALL) ? CheckableStatus::NONE : CheckableStatus::ALL;
            component_->SetGroupValue(value);
            component_->SetMember(value == CheckableStatus::ALL);
            UpdateGroupStatus();

            if (onGroupChange_) {
                std::vector<std::string> result;
                component_->GetSelectedCheckBoxName(result);
                onGroupChange_(std::make_shared<CheckboxGroupResult>(result, static_cast<int32_t>(status_)));
            }
        } else if (!(component_->GetCheckboxName().empty())) {
            component_->SetValue(!component_->GetValue());
            if (component_->GetGroup()) {
                component_->GetGroup()->SetGroupStatus();
            }
        }
    }
    UpdateUIStatus();
    UpdateAnimation();
    MarkNeedRender();
    if (controller_) {
        controller_->Play();
    }
}

void RenderCheckbox::UpdateAnimation()
{
    if (!controller_) {
        LOGE("the controller is nullptr");
        return;
    }
    double from = 0.0;
    double to = 0.0;
    if (!(component_->GetGroupName().empty())) {
        from = CHECKBOX_SCALE[static_cast<int32_t>(lastStatus_)];
        to = CHECKBOX_SCALE[static_cast<int32_t>(status_)];
    } else if (!(component_->GetCheckboxName().empty())) {
        if (component_->GetValue()) {
            from = DEFAULT_MIN_CHECKBOX_SHAPE_SCALE;
            to = DEFAULT_MAX_CHECKBOX_SHAPE_SCALE;
        } else {
            from = DEFAULT_MAX_CHECKBOX_SHAPE_SCALE;
            to = DEFAULT_MIN_CHECKBOX_SHAPE_SCALE;
        }
    } else {
        if (checked_) {
            from = DEFAULT_MAX_CHECKBOX_SHAPE_SCALE;
            to = DEFAULT_MIN_CHECKBOX_SHAPE_SCALE;
        } else {
            from = DEFAULT_MAX_CHECKBOX_SHAPE_SCALE;
            to = DEFAULT_MIN_CHECKBOX_SHAPE_SCALE;
        }
    }

    if (translate_) {
        controller_->RemoveInterpolator(translate_);
    }
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto checkBox = weak.Upgrade();
        if (checkBox) {
            checkBox->UpdateCheckBoxShape(value);
        }
    }));
    controller_->SetDuration(DEFUALT_CHECKBOX_ANIMATION_DURATION);
    controller_->AddInterpolator(translate_);
}

void RenderCheckbox::UpdateCheckBoxShape(const double value)
{
    if (value < DEFAULT_MIN_CHECKBOX_SHAPE_SCALE || value > DEFAULT_MAX_CHECKBOX_SHAPE_SCALE) {
        return;
    }
    shapeScale_ = value;
    if (!(component_->GetGroupName().empty())) {
        if (lastStatus_ == SelectStatus::ALL && status_ == SelectStatus::PART) {
            uiStatus_ = UIStatus::ON_TO_PART;
        } else if (lastStatus_ == SelectStatus::ALL && status_ == SelectStatus::NONE) {
            uiStatus_ = UIStatus::ON_TO_OFF;
        } else if (lastStatus_ == SelectStatus::PART && status_ == SelectStatus::ALL) {
            uiStatus_ = UIStatus::PART_TO_ON;
        } else if (lastStatus_ == SelectStatus::PART && status_ == SelectStatus::NONE) {
            uiStatus_ = UIStatus::PART_TO_OFF;
        } else if (lastStatus_ == SelectStatus::NONE && status_ == SelectStatus::ALL) {
            uiStatus_ = UIStatus::OFF_TO_ON;
        } else if (lastStatus_ == SelectStatus::NONE && status_ == SelectStatus::PART) {
            uiStatus_ = UIStatus::OFF_TO_PART;
        }
    } else if (!(component_->GetCheckboxName().empty())) {
        uiStatus_ = (component_->GetValue()) ? UIStatus::OFF_TO_ON : UIStatus::ON_TO_OFF;
    } else {
        uiStatus_ = checked_ ? UIStatus::ON_TO_OFF : UIStatus::OFF_TO_ON;
    }

    MarkNeedRender();
}

void RenderCheckbox::UpdateGroupStatus()
{
    if (!component_) {
        return;
    }

    auto checkboxList = component_->GetCheckboxList();
    int count = 0;
    isGroup_ = !checkboxList.empty();
    if (isGroup_) {
        for (auto& item : checkboxList) {
            if (item->GetValue()) {
                count++;
            }
        }
        if (count == (int)checkboxList.size()) {
            status_ = SelectStatus::ALL;
        } else if (count > 0 && (int)checkboxList.size() > count) {
            status_ = SelectStatus::PART;
        } else {
            status_ = SelectStatus::NONE;
        }
    }
}

void RenderCheckbox::OnAnimationStop()
{
    // after the animation stopped,we need to update the check status
    RenderCheckable::HandleClick();
    UpdateAccessibilityAttr();
}

} // namespace OHOS::Ace
