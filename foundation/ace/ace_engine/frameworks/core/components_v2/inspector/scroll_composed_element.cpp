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

#include "core/components_v2/inspector/scroll_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ScrollComposedElement&)>> CREATE_JSON_MAP {
    { "scrollable", [](const ScrollComposedElement& inspector) { return inspector.GetAxisDirection(); } },
    { "scrollBar", [](const ScrollComposedElement& inspector) { return inspector.GetDisplayMode(); } },
    { "scrollBarColor", [](const ScrollComposedElement& inspector) { return inspector.GetForegroundColor(); } },
    { "scrollBarWidth", [](const ScrollComposedElement& inspector) { return inspector.GetBarWidth(); } }
};

} // namespace

void ScrollComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("scrollable: ").append(GetAxisDirection()));
    DumpLog::GetInstance().AddDesc(std::string("scrollBar: ").append(GetDisplayMode()));
    DumpLog::GetInstance().AddDesc(std::string("scrollBarColor: ").append(GetForegroundColor()));
    DumpLog::GetInstance().AddDesc(std::string("scrollBarWidth: ").append(GetBarWidth()));
}

std::unique_ptr<JsonValue> ScrollComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string ScrollComposedElement::GetAxisDirection() const
{
    auto renderSingleChildScroll = GetRenderSingleChildScroll();
    if (!renderSingleChildScroll) {
        return "";
    }
    Axis axisDirection = renderSingleChildScroll->GetAxis();
    switch (axisDirection) {
        case Axis::VERTICAL:
            return std::string("ScrollDirection.Vertical");
        case Axis::HORIZONTAL:
            return std::string("ScrollDirection.Horizontal");
        case Axis::FREE:
            return std::string("ScrollDirection.Free");
        case Axis::NONE:
            return std::string("ScrollDirection.None");
        default:
            return std::string("ScrollDirection.Vertical");
    }
}

std::string ScrollComposedElement::GetDisplayMode() const
{
    auto renderSingleChildScroll = GetRenderSingleChildScroll();
    if (!renderSingleChildScroll) {
        return "";
    }
    auto scrollBar = renderSingleChildScroll->GetScrollBar();
    if (!scrollBar) {
        return "";
    }
    DisplayMode displayMode = scrollBar->GetDisplayMode();
    switch (displayMode) {
        case DisplayMode::OFF:
            return std::string("BarState.Off");
        case DisplayMode::AUTO:
            return std::string("BarState.Auto");
        case DisplayMode::ON:
            return std::string("BarState.On");
        default:
            return std::string("");
    }
}

std::string ScrollComposedElement::GetForegroundColor() const
{
    auto renderSingleChildScroll = GetRenderSingleChildScroll();
    if (!renderSingleChildScroll) {
        return "";
    }
    auto scrollBar = renderSingleChildScroll->GetScrollBar();
    if (!scrollBar) {
        return "";
    }
    return scrollBar->GetForegroundColor().ColorToString();
}

std::string ScrollComposedElement::GetBarWidth() const
{
    auto renderSingleChildScroll = GetRenderSingleChildScroll();
    if (!renderSingleChildScroll) {
        return "";
    }
    auto scrollBar = renderSingleChildScroll->GetScrollBar();
    if (!scrollBar) {
        return "";
    }
    Dimension BarWidth = scrollBar->GetActiveWidth();
    return BarWidth.ToString();
}

RefPtr<RenderSingleChildScroll> ScrollComposedElement::GetRenderSingleChildScroll() const
{
    auto node = GetInspectorNode(ScrollElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderSingleChildScroll>(node);
    }
    return nullptr;
}

void ScrollComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto scrollElement = GetContentElement<ScrollElement>(ScrollElement::TypeId());
    if (!scrollElement) {
        LOGE("get GetScrollElement failed");
        return;
    }
    slot = DEFAULT_ELEMENT_SLOT;
    scrollElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    scrollElement->MarkDirty();
}

void ScrollComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto scrollElement = GetContentElement<ScrollElement>(ScrollElement::TypeId());
    if (!scrollElement) {
        LOGE("get GetScrollElement failed");
        return;
    }
    auto child = GetElementChildBySlot(scrollElement, slot);
    if (!child) {
        LOGE("scrollElement get GetChildBySlot failed");
        return;
    }
    scrollElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    scrollElement->MarkDirty();
}

void ScrollComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto scrollElement = GetContentElement<ScrollElement>(ScrollElement::TypeId());
    if (!scrollElement) {
        LOGE("get GetScrollElement failed");
        return;
    }
    auto child = GetElementChildBySlot(scrollElement, slot);
    if (!child) {
        LOGE("scrollElement get GetChildBySlot failed");
        return;
    }
    scrollElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    scrollElement->MarkDirty();
}
} // namespace OHOS::Ace::V2