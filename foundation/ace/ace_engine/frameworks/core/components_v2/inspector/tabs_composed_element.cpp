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

#include "core/components_v2/inspector/tabs_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const TabsComposedElement&)>> CREATE_JSON_MAP {
    { "barPosition", [](const TabsComposedElement& inspector) { return inspector.GetBarPosition(); } },
    { "index", [](const TabsComposedElement& inspector) { return inspector.GetIndex(); } },
    { "vertical", [](const TabsComposedElement& inspector) { return inspector.GetVertical(); } },
    { "barMode", [](const TabsComposedElement& inspector) { return inspector.GetBarMode(); } },
    { "barWidth", [](const TabsComposedElement& inspector) { return inspector.GetBarWidth(); } },
    { "barHeight", [](const TabsComposedElement& inspector) { return inspector.GetBarHeight(); } },
};

} // namespace

void TabsComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("barPosition: ").append(GetBarPosition()));
    DumpLog::GetInstance().AddDesc(std::string("index: ").append(GetIndex()));
    DumpLog::GetInstance().AddDesc(std::string("vertical: ").append(GetVertical()));
    DumpLog::GetInstance().AddDesc(std::string("scrollable: ").append(ConvertBoolToString(GetScrollable())));
    DumpLog::GetInstance().AddDesc(std::string("barMode: ").append(GetBarMode()));
    DumpLog::GetInstance().AddDesc(std::string("barWidth: ").append(GetBarWidth()));
    DumpLog::GetInstance().AddDesc(std::string("barHeight: ").append(GetBarHeight()));
}

std::unique_ptr<JsonValue> TabsComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TabsComposedElement::GetBarPosition() const
{
    auto renderTabBar = GetRenderTabBar(GetRenderFlex());
    auto barPosition = renderTabBar ? renderTabBar->GetBarPosition() : BarPosition::START;
    return ConvertBarPositionToString(barPosition);
}

std::string TabsComposedElement::GetIndex() const
{
    auto renderTabContent = GetRenderTabContent(GetRenderFlex());
    auto index = renderTabContent ? renderTabContent->GetCurrentIndex() : 0;
    return std::to_string(index);
}

std::string TabsComposedElement::GetVertical() const
{
    auto renderFlex = GetRenderFlex();
    auto vertical = renderFlex ? renderFlex->GetDirection() == FlexDirection::ROW : false;
    return ConvertBoolToString(vertical);
}

bool TabsComposedElement::GetScrollable() const
{
    auto renderTabContent = GetRenderTabContent(GetRenderFlex());
    return renderTabContent ? renderTabContent->IsScrollable() : true;
}

std::string TabsComposedElement::GetBarMode() const
{
    auto renderTabBar = GetRenderTabBar(GetRenderFlex());
    auto barMode = renderTabBar ? renderTabBar->GetBarMode() : TabBarMode::FIXED;
    return ConvertTabBarModeToString(barMode);
}

std::string TabsComposedElement::GetBarWidth() const
{
    auto renderBox = GetRenderBox(GetRenderTabBar(GetRenderFlex()));
    auto barWidth = renderBox ? renderBox->GetWidth() : 0.0;
    return std::to_string(barWidth);
}

std::string TabsComposedElement::GetBarHeight() const
{
    auto renderBox = GetRenderBox(GetRenderTabBar(GetRenderFlex()));
    auto barHeight = renderBox ? renderBox->GetHeight() : 0.0;
    return std::to_string(barHeight);
}

RefPtr<RenderFlex> TabsComposedElement::GetRenderFlex() const
{
    auto node = GetInspectorNode(TabsElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderFlex>(node);
    }
    return nullptr;
}

RefPtr<RenderTabBar> TabsComposedElement::GetRenderTabBar(const RefPtr<RenderNode>& node) const
{
    if (!node) {
        return nullptr;
    }

    auto renderTabBar = AceType::DynamicCast<RenderTabBar>(node);
    if (renderTabBar) {
        return renderTabBar;
    }

    for (const auto& child : node->GetChildren()) {
        auto result = GetRenderTabBar(child);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

RefPtr<RenderTabContent> TabsComposedElement::GetRenderTabContent(const RefPtr<RenderNode>& node) const
{
    if (!node) {
        return nullptr;
    }

    auto renderTabContent = AceType::DynamicCast<RenderTabContent>(node);
    if (renderTabContent) {
        return renderTabContent;
    }

    for (const auto& child : node->GetChildren()) {
        auto result = GetRenderTabContent(child);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

RefPtr<RenderBox> TabsComposedElement::GetRenderBox(const RefPtr<RenderNode>& node) const
{
    auto current = node;
    while (current) {
        auto renderBox = AceType::DynamicCast<RenderBox>(current);
        if (renderBox) {
            return renderBox;
        }
        current = current->GetParent().Upgrade();
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2