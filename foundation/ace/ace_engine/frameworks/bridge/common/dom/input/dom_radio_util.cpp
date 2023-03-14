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

#include "frameworks/bridge/common/dom/input/dom_radio_util.h"

#include "core/components/common/layout/constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"
#include "frameworks/bridge/js_frontend/js_frontend.h"

namespace OHOS::Ace::Framework {
namespace {

std::shared_ptr<JsPageRadioGroups> GetRadioGroups(const DOMNode& node)
{
    auto context = node.GetPipelineContext().Upgrade();
    if (!context) {
        LOGE("pipeline context is null");
        return nullptr;
    }
    auto frontend = context->GetFrontend();
    if (!frontend) {
        LOGE("frontend is null");
        return nullptr;
    }
    auto page = frontend->GetPage(node.GetPageId());
    if (!page) {
        auto jsFrontend = AceType::DynamicCast<JsFrontend>(frontend);
        if (!jsFrontend) {
            LOGE("not js frontend");
            return nullptr;
        }
        page = jsFrontend->GetCurrentReadyPage().Upgrade();
    }
    auto jsPage = AceType::DynamicCast<JsAcePage>(page);
    if (!jsPage) {
        LOGE("js page is null");
        return nullptr;
    }
    return jsPage->GetRadioGroups();
}

} // namespace

void DOMRadioUtil::InitDefaultValue(const RefPtr<RadioComponent<std::string>>& component) {}

RefPtr<RadioComponent<std::string>> DOMRadioUtil::CreateComponentAndSetChildAttr(
    const std::map<std::string, std::string>& attrs, DOMInput& node)
{
    // get theme to create component
    RefPtr<RadioTheme> theme = node.GetTheme<RadioTheme>();
    RefPtr<RadioComponent<std::string>> component = AceType::MakeRefPtr<RadioComponent<std::string>>(theme);
    // set default value
    InitDefaultValue(component);
    // set the default hot zone
    auto pipelineContext = node.GetPipelineContext().Upgrade();
    if (pipelineContext->UseLiteStyle()) {
        component->SetHorizontalPadding(Dimension(0));
        component->SetHotZoneVerticalPadding(Dimension(0));
    }
    auto boxComponent = node.GetBoxComponent();
    if (LessOrEqual(node.GetWidth().Value(), 0.0)) {
        node.SetWidth(theme->GetWidth());
        boxComponent->SetWidth(theme->GetWidth().Value(), theme->GetWidth().Unit());
    }
    if (LessOrEqual(node.GetHeight().Value(), 0.0)) {
        node.SetHeight(theme->GetHeight());
        boxComponent->SetHeight(theme->GetHeight().Value(), theme->GetHeight().Unit());
    }
    auto diameter = boxComponent->GetHeightDimension() > boxComponent->GetWidthDimension()
                        ? boxComponent->GetWidthDimension()
                        : boxComponent->GetHeightDimension();
    if (!boxComponent->GetBackDecoration()) {
        RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
        backDecoration->SetBorderRadius(Radius(diameter / 2.0));
        boxComponent->SetBackDecoration(backDecoration);
    }

    SetChildAttr(node, component, attrs);

    return component;
}

void DOMRadioUtil::SetChildAttr(const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component,
    const std::map<std::string, std::string>& attrs)
{
    if (!component) {
        LOGE("fail to set child attr due to radio component is null");
        return;
    }
    bool checked = false;
    for (const auto& attr : attrs) {
        if (attr.first == DOM_INPUT_CHECKED) {
            checked = StringToBool(attr.second);
        } else if (attr.first == DOM_INPUT_NAME) {
            CheckRadioGroup(node, component, attr.second);
        } else if (attr.first == DOM_DISABLED) {
            component->SetDisabled(StringToBool(attr.second));
        } else if (attr.first == DOM_INPUT_VALUE) {
            component->SetValue(attr.second);
        }
    }
    if (checked) {
        component->SetGroupValue(component->GetValue());
        component->SetOriginChecked(checked);
    } else {
        component->SetGroupValue("");
    }
}

void DOMRadioUtil::SetChildStyle(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<RadioComponent<std::string>>& component, const std::map<std::string, std::string>& styles)
{
    if (!component) {
        LOGE("fail to set child attr due to radio component is null");
        return;
    }
    // Padding is set in radio specially so that it can respond click event.
    boxComponent->SetPadding(Edge());
    for (const auto& style : styles) {
        if (style.first == DOM_PADDING) {
            Edge padding;
            if (Edge::FromString(style.second, padding)) {
                component->SetHorizontalPadding(padding.Left());
                component->SetHotZoneVerticalPadding(padding.Top());
            }
        }
    }
}

void DOMRadioUtil::AddChildEvent(const RefPtr<RadioComponent<std::string>>& component, int32_t pageId,
    const std::string& nodeId, const std::vector<std::string>& events)
{
    if (!component) {
        LOGE("fail to add child event due to radio component is null");
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

void DOMRadioUtil::RemoveRadioComponent(const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component)
{
    if (!component) {
        return;
    }
    auto radioGroups = GetRadioGroups(node);
    if (radioGroups == nullptr) {
        return;
    }

    auto radioGroupIter = (*radioGroups).find(component->GetGroupName());
    if (radioGroupIter != (*radioGroups).end()) {
        radioGroupIter->second.RemoveRadio(component);
        if (radioGroupIter->second.IsEmpty()) {
            (*radioGroups).erase(radioGroupIter);
        }
    }
}

void DOMRadioUtil::CheckRadioGroup(
    const DOMInput& node, const RefPtr<RadioComponent<std::string>>& component, const std::string& name)
{
    auto radioGroups = GetRadioGroups(node);
    if (radioGroups == nullptr) {
        return;
    }
    const auto& groupName = component->GetGroupName();
    if (!groupName.empty()) {
        if (groupName == name) {
            return;
        }
        auto& oldRadioGroup = (*radioGroups)[groupName];
        oldRadioGroup.RemoveRadio(component);
        if (oldRadioGroup.IsEmpty()) {
            (*radioGroups).erase(groupName);
        }
    }
    component->SetGroupName(name);
    auto& radioGroup = (*radioGroups)[name];
    radioGroup.AddRadio(component);
}

} // namespace OHOS::Ace::Framework
