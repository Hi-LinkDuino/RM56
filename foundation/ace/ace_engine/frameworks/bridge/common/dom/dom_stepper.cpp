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

#include "frameworks/bridge/common/dom/dom_stepper.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/stepper/stepper_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_stepper_item.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t DEFAULT_NODE_INDEX = -1;
constexpr int32_t METHOD_TO_ARGS_SIZE = 1;

} // namespace

DOMStepper::DOMStepper(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    stepperComponent_ = AceType::MakeRefPtr<StepperComponent>(std::list<RefPtr<Component>>());
}

bool DOMStepper::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMStepper&)> attrsOperators[] = {
        { DOM_STEPPER_INDEX,
            [](const std::string& val, DOMStepper& stepper) {
                stepper.stepperComponent_->SetIndex(StringUtils::StringToInt(val));
            } },
    };
    auto iter = BinarySearchFindIndex(attrsOperators, ArraySize(attrsOperators), attr.first.c_str());
    if (iter != -1) {
        attrsOperators[iter].value(attr.second, *this);
        return true;
    } else {
        return false;
    }
}

bool DOMStepper::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return false;
}

bool DOMStepper::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(int32_t, DOMStepper&)> eventOperators[] = {
        {
            DOM_STEPPER_EVENT_BACK,
            [](int32_t pageId, DOMStepper& stepper) {
                stepper.backEventId_ = EventMarker(stepper.GetNodeIdForEvent(), DOM_STEPPER_EVENT_BACK, pageId);
                stepper.stepperComponent_->SetBackEventId(stepper.backEventId_);
            },
        },
        {
            DOM_STEPPER_EVENT_CHANGE,
            [](int32_t pageId, DOMStepper& stepper) {
                stepper.changeEventId_ = EventMarker(stepper.GetNodeIdForEvent(), DOM_STEPPER_EVENT_CHANGE, pageId);
                stepper.stepperComponent_->SetChangeEventId(stepper.changeEventId_);
            },
        },
        {
            DOM_STEPPER_EVENT_FINISH,
            [](int32_t pageId, DOMStepper& stepper) {
                stepper.finishEventId_ = EventMarker(stepper.GetNodeIdForEvent(), DOM_STEPPER_EVENT_FINISH, pageId);
                stepper.stepperComponent_->SetFinishEventId(stepper.finishEventId_);
            },
        },
        {
            DOM_STEPPER_EVENT_NEXT,
            [](int32_t pageId, DOMStepper& stepper) {
                stepper.nextEventId_ = EventMarker(stepper.GetNodeIdForEvent(), DOM_STEPPER_EVENT_NEXT, pageId);
                stepper.stepperComponent_->SetNextEventId(stepper.nextEventId_);
            },
        },
        {
            DOM_STEPPER_EVENT_SKIP,
            [](int32_t pageId, DOMStepper& stepper) {
                stepper.skipEventId_ = EventMarker(stepper.GetNodeIdForEvent(), DOM_STEPPER_EVENT_SKIP, pageId);
                stepper.stepperComponent_->SetSkipEventId(stepper.skipEventId_);
            },
        },
    };
    auto iter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (iter != -1) {
        eventOperators[iter].value(pageId, *this);
        return true;
    }
    return false;
}

void DOMStepper::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_STEPPER_SET_RIGHT_BUTTON_STATUS) {
        auto controller = stepperComponent_->GetStepperController();
        if (!controller) {
            LOGE("get controller failed");
            return;
        }
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_TO_ARGS_SIZE) {
            LOGE("parse args error");
            return;
        }
        std::unique_ptr<JsonValue> valueParams = argsValue->GetArrayItem(0);

        std::string status;
        std::unique_ptr<JsonValue> statusValue = valueParams->GetValue("status");
        if (statusValue && statusValue->IsString()) {
            status = statusValue->GetString();
        }

        std::string label;
        std::unique_ptr<JsonValue> labelValue = valueParams->GetValue("label");
        if (labelValue && labelValue->IsString()) {
            label = labelValue->GetString();
        }
        controller->SetRightButtonStatus(status, label);
    }
}

void DOMStepper::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    auto domStepperItem = AceType::DynamicCast<DOMStepperItem>(child);
    if (!domStepperItem) {
        LOGE("child is not stepper item, add to stepper failed.");
        return;
    }
    auto childIndex = domStepperItem->GetItemIndex();
    LOGD("DOMStepper AddChild %{public}s, childIndex: %{public}d, slot: %{public}d",
        child->GetTag().c_str(), childIndex, slot);
    if (childIndex != DEFAULT_NODE_INDEX) {
        stepperComponent_->InsertChild(childIndex, child->GetRootComponent());
    } else {
        stepperComponent_->AppendChild(child->GetRootComponent());
    }
}

void DOMStepper::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!stepperComponent_ || !child) {
        LOGE("DOMStepper OnChildNodeRemoved listComponent_ is null");
        return;
    }
    stepperComponent_->RemoveChild(child->GetRootComponent());
}

void DOMStepper::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMStepper::ResetInitializedStyle()
{
    auto theme = GetTheme<StepperTheme>();
    if (!theme) {
        LOGE("Stepper theme is null");
        return;
    }
    stepperComponent_->SetDefaultPaddingStart(theme->GetDefaultPaddingStart());
    stepperComponent_->SetDefaultPaddingEnd(theme->GetDefaultPaddingEnd());
    stepperComponent_->SetProgressColor(theme->GetProgressColor());
    stepperComponent_->SetProgressDiameter(theme->GetProgressDiameter());
    stepperComponent_->SetArrowWidth(theme->GetArrowWidth());
    stepperComponent_->SetArrowHeight(theme->GetArrowHeight());
    stepperComponent_->SetArrowColor(theme->GetArrowColor());
    stepperComponent_->SetDisabledColor(theme->GetDisabledColor());
    stepperComponent_->SetRadius(theme->GetRadius());
    stepperComponent_->SetButtonPressedColor(theme->GetButtonPressedColor());
    stepperComponent_->SetButtonPressedHeight(theme->GetButtonPressedHeight());
    stepperComponent_->SetControlHeight(theme->GetControlHeight());
    stepperComponent_->SetControlMargin(theme->GetControlMargin());
    stepperComponent_->SetControlPadding(theme->GetControlPadding());
    stepperComponent_->SetFocusColor(theme->GetFocusColor());
    stepperComponent_->SetFocusBorderWidth(theme->GetFocusBorderWidth());
    stepperComponent_->SetMouseHoverColor(theme->GetMouseHoverColor());
    stepperComponent_->SetDisabledAlpha(theme->GetDisabledAlpha());
}

void DOMStepper::PrepareSpecializedComponent()
{
    stepperComponent_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
}

} // namespace OHOS::Ace::Framework
