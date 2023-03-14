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

#include "core/components_v2/inspector/panel_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/panel/render_sliding_panel.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const PanelComposedElement&)>> CREATE_JSON_MAP {
    { "type", [](const PanelComposedElement& inspector) { return inspector.GetPanelType(); } },
    { "mode", [](const PanelComposedElement& inspector) { return inspector.GetPanelMode(); } },
    { "dragBar", [](const PanelComposedElement& inspector) { return inspector.GetDragBar(); } },
    { "fullHeight", [](const PanelComposedElement& inspector) { return inspector.GetFullHeight(); } },
    { "halfHeight", [](const PanelComposedElement& inspector) { return inspector.GetHalfHeight(); } },
    { "miniHeight", [](const PanelComposedElement& inspector) { return inspector.GetMiniHeight(); } },
    { "show", [](const PanelComposedElement& inspector) { return inspector.GetShow(); } }
};

}


std::unique_ptr<JsonValue> PanelComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

void PanelComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("type: ").append(GetPanelType()));
    DumpLog::GetInstance().AddDesc(
        std::string("mode: ").append(GetPanelMode()));
    DumpLog::GetInstance().AddDesc(
        std::string("dragBar: ").append(GetDragBar()));
    DumpLog::GetInstance().AddDesc(
        std::string("fullHeight: ").append(GetFullHeight()));
    DumpLog::GetInstance().AddDesc(
        std::string("halfHeight: ").append(GetHalfHeight()));
    DumpLog::GetInstance().AddDesc(
        std::string("miniHeight: ").append(GetMiniHeight()));
    DumpLog::GetInstance().AddDesc(
        std::string("show: ").append(GetShow()));
}

std::string PanelComposedElement::GetPanelType() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "PanelType.Foldable";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        if (renderPanel->GetPanelType() == PanelType::MINI_BAR) {
            return "PanelType.Minibar";
        } else if (renderPanel->GetPanelType() == PanelType::FOLDABLE_BAR) {
            return "PanelType.Foldable";
        } else {
            return "PanelType.Temporary";
        }
    }
    return "PanelType.Foldable";
}

std::string PanelComposedElement::GetPanelMode() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        if (renderPanel->GetPanelMode() == PanelMode::MINI) {
            return "PanelMode.Mini";
        } else if (renderPanel->GetPanelMode() == PanelMode::HALF) {
            return "PanelMode.Half";
        } else {
            return "PanelMode.Full";
        }
    }
    return "-";
}

std::string PanelComposedElement::GetDragBar() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "false";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        if (renderPanel->HasDragBar()) {
            return "true";
        }
    }
    return "false";
}

std::string PanelComposedElement::GetFullHeight() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        return renderPanel->GetFullHeight().ToString();
    }
    return "-";
}

std::string PanelComposedElement::GetHalfHeight() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        return renderPanel->GetHalfHeight().ToString();
    }
    return "-";
}

std::string PanelComposedElement::GetMiniHeight() const
{
    auto node = GetInspectorNode(SlidingPanelElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderPanel = AceType::DynamicCast<RenderSlidingPanel>(node);
    if (renderPanel) {
        return renderPanel->GetMiniHeight().ToString();
    }
    return "-";
}

std::string PanelComposedElement::GetShow() const
{
    auto visibility = GetVisibility();
    if (visibility == "Visibility.Hidden" || visibility == "Visibility.None") {
        return "false";
    } else {
        return "true";
    }
}

void PanelComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto slidingPanelElement = GetContentElement<SlidingPanelElement>(SlidingPanelElement::TypeId());
    if (!slidingPanelElement) {
        LOGE("get GetSlidingPanelElement failed");
        return;
    }
    slidingPanelElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    slidingPanelElement->MarkDirty();
}

void PanelComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto slidingPanelElement = GetContentElement<SlidingPanelElement>(SlidingPanelElement::TypeId());
    if (!slidingPanelElement) {
        LOGE("get GetSlidingPanelElement failed");
        return;
    }
    auto element = GetInspectorComposedElementParent(slidingPanelElement);
    if (!element) {
        LOGE("slidingPanelElement get GetInspectorComposedElementParent failed");
        return;
    }
    auto child = GetElementChildBySlot(element, slot);
    if (!child) {
        LOGE("get GetElementChildBySlot failed");
        return;
    }
    element->UpdateChildWithSlot(child, newComponent, slot, slot);
    element->MarkDirty();
}

void PanelComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto slidingPanelElement = GetContentElement<SlidingPanelElement>(SlidingPanelElement::TypeId());
    if (!slidingPanelElement) {
        LOGE("get GetSlidingPanelElement failed");
        return;
    }
    auto element = GetInspectorComposedElementParent(slidingPanelElement);
    if (!element) {
        LOGE("slidingPanelElement get GetInspectorComposedElementParent failed");
        return;
    }
    auto child = GetElementChildBySlot(element, slot);
    if (!child) {
        LOGE("get GetElementChildBySlot failed");
        return;
    }
    element->UpdateChildWithSlot(child, nullptr, slot, slot);
    element->MarkDirty();
}

} // namespace OHOS::Ace::V2