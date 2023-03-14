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

#include "core/components_v2/inspector/navigator_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/navigator/render_navigator.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const NavigatorComposedElement&)>> CREATE_JSON_MAP {
    { "active", [](const NavigatorComposedElement& inspector) { return inspector.GetActive(); } },
    { "target", [](const NavigatorComposedElement& inspector) { return inspector.GetTarget(); } },
    { "type", [](const NavigatorComposedElement& inspector) { return inspector.GetNavigatorType(); } },
    { "params", [](const NavigatorComposedElement& inspector) { return inspector.GetParams(); } }
};

}

void NavigatorComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("active: ").append(GetActive()));
    DumpLog::GetInstance().AddDesc(
        std::string("target: ").append(GetTarget()));
    DumpLog::GetInstance().AddDesc(
        std::string("type: ").append(GetNavigatorType()));
    DumpLog::GetInstance().AddDesc(
        std::string("params: ").append(GetParams()));
}

std::unique_ptr<JsonValue> NavigatorComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string NavigatorComposedElement::GetActive() const
{
    auto node = GetInspectorNode(NavigatorElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderNavigator = AceType::DynamicCast<RenderNavigator>(node);
    if (renderNavigator) {
        return renderNavigator->GetActive() ? "true" : "false";
    }
    return "-";
}

std::string NavigatorComposedElement::GetTarget() const
{
    auto node = GetInspectorNode(NavigatorElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderNavigator = AceType::DynamicCast<RenderNavigator>(node);
    if (renderNavigator) {
        return renderNavigator->GetTarget();
    }
    return "-";
}

std::string NavigatorComposedElement::GetNavigatorType() const
{
    auto node = GetInspectorNode(NavigatorElement::TypeId());
    if (!node) {
        return "NavigationType.Push";
    }
    auto renderNavigator = AceType::DynamicCast<RenderNavigator>(node);
    if (renderNavigator) {
        if (renderNavigator->GetType() == NavigatorType::PUSH) {
            return "NavigationType.Push";
        } else if (renderNavigator->GetType() == NavigatorType::REPLACE) {
            return "NavigationType.Replace";
        } else {
            return "NavigationType.Back";
        }
    }
    return "NavigationType.Push";
}

std::string NavigatorComposedElement::GetParams() const
{
    auto node = GetInspectorNode(NavigatorElement::TypeId());
    if (!node) {
        return "{}";
    }
    auto renderNavigator = AceType::DynamicCast<RenderNavigator>(node);
    if (renderNavigator) {
        return renderNavigator->GetParam();
    }
    return "{}";
}

void NavigatorComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto navigatorElement = GetContentElement<NavigatorElement>(NavigatorElement::TypeId());
    if (!navigatorElement) {
        LOGE("get GetNavigatorElement failed");
        return;
    }
    navigatorElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    navigatorElement->MarkDirty();
    LOGD("navigator AddChildWithSlot");
}

void NavigatorComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto navigatorElement = GetContentElement<NavigatorElement>(NavigatorElement::TypeId());
    if (!navigatorElement) {
        LOGE("get GetNavigatorElement failed");
        return;
    }
    auto child = GetElementChildBySlot(navigatorElement, slot);
    if (!child) {
        LOGE("navigatorElement get GetChildBySlot failed");
        return;
    }
    navigatorElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    navigatorElement->MarkDirty();
    LOGD("navigator UpdateChildWithSlot");
}

void NavigatorComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto navigatorElement = GetContentElement<NavigatorElement>(NavigatorElement::TypeId());
    if (!navigatorElement) {
        LOGE("get GetNavigatorElement failed");
        return;
    }
    auto child = GetElementChildBySlot(navigatorElement, slot);
    if (!child) {
        LOGE("navigatorElement get GetChildBySlot failed");
        return;
    }
    navigatorElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    navigatorElement->MarkDirty();
    LOGD("navigator DeleteChildWithSlot");
}

} // namespace OHOS::Ace::V2