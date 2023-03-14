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

#include "frameworks/bridge/common/dom/dom_stepper_item.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "core/components/stepper/stepper_theme.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr double DEFAULT_OPACITY = 1.0;

} // namespace

DOMStepperItem::DOMStepperItem(NodeId nodeId, const std::string& nodeName, int32_t index) : DOMNode(nodeId, nodeName)
{
    itemIndex_ = index;
    stepperItemComponent_ = AceType::MakeRefPtr<StepperItemComponent>(RefPtr<Component>());
}

bool DOMStepperItem::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMStepperItem&)> stepperItemStyleOperators[] = {
        { DOM_ALIGN_ITEMS, [](const std::string& val,
            DOMStepperItem& stepperItem) { stepperItem.flexCrossAlign_ = ConvertStrToFlexAlign(val); } },
        { DOM_TEXT_ALLOW_SCALE,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetAllowScale(StringToBool(value));
            } },
        { DOM_STEPPER_TEXT_COLOR,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetTextColor(stepperItem.ParseColor(value));
            } },
        { DOM_FLEX_DIRECTION, [](const std::string& val,
            DOMStepperItem& stepperItem) { stepperItem.flexDirection_ = ConvertStrToFlexDirection(val); } },
        { DOM_STEPPER_FONT_FAMILY,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetFontFamilies(ConvertStrToFontFamilies(value));
            } },
        { DOM_STEPPER_FONT_SIZE,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetFontSize(stepperItem.ParseDimension(value));
            } },
        { DOM_STEPPER_FONT_STYLE,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetFontStyle(ConvertStrToFontStyle(value));
            } },
        { DOM_STEPPER_FONT_WEIGHT,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetFontWeight(ConvertStrToFontWeight(value));
            } },
        { DOM_JUSTIFY_CONTENT, [](const std::string& val,
            DOMStepperItem& stepperItem) { stepperItem.flexMainAlign_ = ConvertStrToFlexAlign(val); } },
        { DOM_STEPPER_TEXT_DECORATION,
            [](const std::string& value, DOMStepperItem& stepperItem) {
              stepperItem.textStyle_.SetTextDecoration(ConvertStrToTextDecoration(value));
            } },
    };
    auto operatorIter = BinarySearchFindIndex(stepperItemStyleOperators,
        ArraySize(stepperItemStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        stepperItemStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return  false;
}

void DOMStepperItem::InitializeStyle()
{
    auto theme = GetTheme<StepperTheme>();
    if (!theme) {
        LOGE("Stepper theme is null");
        return;
    }
    textStyle_ = theme->GetTextStyle();
    textStyle_.SetAdaptTextSize(theme->GetTextStyle().GetFontSize(), theme->GetMinFontSize());
    textStyle_.SetMaxLines(theme->GetTextMaxLines());
    textStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
}

bool DOMStepperItem::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_STEPPER_ITEM_EVENT_APPEAR) {
        appearEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        stepperItemComponent_->SetAppearEventId(appearEventId_);
        return true;
    } else if (event == DOM_STEPPER_ITEM_EVENT_DISAPPEAR) {
        disappearEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        stepperItemComponent_->SetDisappearEventId(disappearEventId_);
        return true;
    } else {
        return false;
    }
}

void DOMStepperItem::AddStepperItem(const RefPtr<DOMNode>& node, int32_t slot)
{
    const auto& childComponent = node->GetRootComponent();
    if (!flexComponent_) {
        flexComponent_ = AceType::MakeRefPtr<FlexComponent>(
            flexDirection_, flexMainAlign_, flexCrossAlign_, std::list<RefPtr<Component>>());
    }
    flexComponent_->InsertChild(slot, childComponent);
    Component::MergeRSNode(childComponent, flexComponent_);
}

void DOMStepperItem::RemoveStepperItem(const RefPtr<DOMNode>& node)
{
    const auto& childComponent = node->GetRootComponent();
    if (flexComponent_) {
        flexComponent_->RemoveChild(childComponent);
    }
}

void DOMStepperItem::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (child) {
        AddStepperItem(child, slot);
    }
}

void DOMStepperItem::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (child) {
        RemoveStepperItem(child);
    }
}

void DOMStepperItem::PrepareSpecializedComponent()
{
    if (!stepperItemComponent_) {
        stepperItemComponent_ = AceType::MakeRefPtr<StepperItemComponent>(RefPtr<Component>());
    }
    stepperItemComponent_->SetLabel(GetLabel());
    stepperItemComponent_->SetTextStyle(textStyle_);
    ResetInitializedStyle();

    if (flexComponent_) {
        flexComponent_->SetDirection(flexDirection_);
        flexComponent_->SetMainAxisAlign(flexMainAlign_);
        flexComponent_->SetCrossAxisAlign(flexCrossAlign_);
    } else {
        flexComponent_ = AceType::MakeRefPtr<FlexComponent>(
            flexDirection_, flexMainAlign_, flexCrossAlign_, std::list<RefPtr<Component>>());
        flexComponent_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    }

    if (displayComponent_) {
        displayComponent_->SetVisible(VisibleType::VISIBLE);
        displayComponent_->SetOpacity(DEFAULT_OPACITY);
    } else {
        displayComponent_ = AceType::MakeRefPtr<DisplayComponent>();
    }
}

RefPtr<Component> DOMStepperItem::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    auto scroll = AceType::MakeRefPtr<ScrollComponent>(flexComponent_);
    scroll->SetTakeBoundary(false);
    RefPtr<Component> component = scroll;

    if (!components.empty()) {
        const auto& parent = components.back();
        if (parent) {
            parent->SetChild(component);
        }
        component = AceType::DynamicCast<Component>(components.front());
    }

    if (stepperItemComponent_) {
        if (displayComponent_) {
            displayComponent_->SetChild(component);
            stepperItemComponent_->SetChild(displayComponent_);
        } else {
            stepperItemComponent_->SetChild(component);
        }
    }
    return stepperItemComponent_;
}

void DOMStepperItem::ResetInitializedStyle()
{
    if (!stepperItemComponent_) {
        LOGE("stepper item is null, reset style failed.");
        return;
    }
    auto theme = GetTheme<FocusAnimationTheme>();
    if (theme) {
        stepperItemComponent_->SetFocusAnimationColor(theme->GetColor());
    }
}

} // namespace OHOS::Ace::Framework
