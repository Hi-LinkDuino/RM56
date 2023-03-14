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

#include "frameworks/bridge/common/dom/input/dom_checkbox_util.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr bool INPUT_CHECKED_VALUE_DEFAULT = false;
constexpr Dimension BOX_HOVER_RADIUS = 8.0_vp;

} // namespace

void DOMCheckboxUtil::InitDefaultValue(const RefPtr<CheckboxComponent>& component)
{
    component->SetValue(INPUT_CHECKED_VALUE_DEFAULT);
}

RefPtr<CheckboxComponent> DOMCheckboxUtil::CreateComponentAndSetChildAttr(
    const std::map<std::string, std::string>& attrs, DOMInput& node)
{
    // get theme to create component
    RefPtr<CheckboxTheme> theme = node.GetTheme<CheckboxTheme>();
    RefPtr<CheckboxComponent> component = AceType::MakeRefPtr<CheckboxComponent>(theme);
    // set default value
    InitDefaultValue(component);
    // Set default width and height to box.
    auto boxComponent = node.GetBoxComponent();
    if (boxComponent) {
        if (!boxComponent->GetBackDecoration()) {
            RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
            backDecoration->SetBorderRadius(Radius(BOX_HOVER_RADIUS));
            boxComponent->SetBackDecoration(backDecoration);
        }
    }
    // set the default hot zone
    auto pipelineContext = node.GetPipelineContext().Upgrade();
    if (pipelineContext->UseLiteStyle()) {
        component->SetHorizontalPadding(Dimension(0));
        component->SetHotZoneVerticalPadding(Dimension(0));
    }
    if (LessOrEqual(node.GetWidth().Value(), 0.0) && boxComponent) {
        node.SetWidth(theme->GetWidth());
        boxComponent->SetWidth(theme->GetWidth().Value(), theme->GetWidth().Unit());
    }
    if (LessOrEqual(node.GetHeight().Value(), 0.0) && boxComponent) {
        node.SetHeight(theme->GetHeight());
        boxComponent->SetHeight(theme->GetHeight().Value(), theme->GetHeight().Unit());
    }

    SetChildAttr(component, attrs);

    return component;
}

void DOMCheckboxUtil::SetChildAttr(
    const RefPtr<CheckboxComponent>& component, const std::map<std::string, std::string>& attrs)
{
    if (!component) {
        LOGE("fail to set child attr due to checkbox component is null");
        return;
    }
    static const LinearMapNode<void (*)(const RefPtr<CheckboxComponent>&, const std::string&)> attrOperators[] = {
        { DOM_INPUT_CHECKED, [](const RefPtr<CheckboxComponent>& component,
                             const std::string& value) { component->SetValue(StringToBool(value)); } },
        { DOM_DISABLED, [](const RefPtr<CheckboxComponent>& component,
                        const std::string& value) { component->SetDisabled(StringToBool(value)); } },
    };
    for (const auto& [key, value] : attrs) {
        auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), key.c_str());
        if (operatorIter != -1) {
            attrOperators[operatorIter].value(component, value);
        }
    }
}

void DOMCheckboxUtil::AddChildEvent(const RefPtr<CheckboxComponent>& component, int32_t pageId,
    const std::string& nodeId, const std::vector<std::string>& events)
{
    if (!component) {
        LOGE("fail to add child event due to checkbox component is null");
        return;
    }
    for (const auto& event : events) {
        if (event == DOM_CHANGE) {
            component->SetChangeEvent(EventMarker(nodeId, event, pageId));
        } else if (event == DOM_CLICK) {
            EventMarker eventMarker(nodeId, event, pageId);
            eventMarker.SetCatchMode(false);
            component->SetClickEvent(eventMarker);
        } else if (event == DOM_CATCH_BUBBLE_CLICK) {
            EventMarker eventMarker(nodeId, event, pageId);
            eventMarker.SetCatchMode(true);
            component->SetClickEvent(eventMarker);
        }
    }
}

} // namespace OHOS::Ace::Framework
