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

#include "core/components_v2/inspector/side_bar_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/side_bar/side_bar_container_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const SideBarComposedElement&)>> CREATE_JSON_MAP {
    { "type", [](const SideBarComposedElement& inspector) { return inspector.GetStyle(); } },
    { "showSideBar", [](const SideBarComposedElement& inspector) { return inspector.GetShowSideBar(); } },
    { "controlButton", [](const SideBarComposedElement& inspector) { return inspector.GetControlButton(); } },
    { "showControlButton", [](const SideBarComposedElement& inspector) { return inspector.GetShowControlButton(); } },
    { "sideBarWidth", [](const SideBarComposedElement& inspector) { return inspector.GetSideBarWidth(); } },
    { "minSideBarWidth", [](const SideBarComposedElement& inspector) { return inspector.GetMinSideBarWidth(); } },
    { "maxSideBarWidth", [](const SideBarComposedElement& inspector) { return inspector.GetMaxSideBarWidth(); } }
};

} // namespace

void SideBarComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("type: ").append(GetStyle()));
    DumpLog::GetInstance().AddDesc(std::string("showSideBar: ").append(GetShowSideBar()));
    DumpLog::GetInstance().AddDesc(std::string("controlButton: ").append(GetControlButton()));
    DumpLog::GetInstance().AddDesc(std::string("showControlButton: ").append(GetShowControlButton()));
    DumpLog::GetInstance().AddDesc(std::string("sideBarWidth: ").append(GetSideBarWidth()));
    DumpLog::GetInstance().AddDesc(std::string("minSideBarWidth: ").append(GetMinSideBarWidth()));
    DumpLog::GetInstance().AddDesc(std::string("maxSideBarWidth: ").append(GetMaxSideBarWidth()));
}

std::unique_ptr<JsonValue> SideBarComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string SideBarComposedElement::GetShowSideBar() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return ConvertBoolToString(render->GetShowSideBarContainer());
    }
    return "true";
}

std::string SideBarComposedElement::GetStyle() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return render->GetStyle();
    }
    return "SideBarContainerType.Embed";
}

std::string SideBarComposedElement::GetControlButton() const
{
    auto render = GetRenderSideBar();
    auto jsonValue = JsonUtil::Create(false);
    if (render) {
        auto jsonControl = JsonUtil::Create(false);
        jsonControl->Put("left", std::to_string(render->GetButtonLeft()).c_str());
        jsonControl->Put("top", std::to_string(render->GetButtonTop()).c_str());
        jsonControl->Put("width", std::to_string(render->GetButtonWidth()).c_str());
        jsonControl->Put("height", std::to_string(render->GetButtonHeight()).c_str());

        auto jsonIcon = JsonUtil::Create(false);
        jsonIcon->Put("shown", render->GetShowIcon().c_str());
        jsonIcon->Put("hidden", render->GetHiddenIcon().c_str());
        jsonIcon->Put("switching", render->GetSwitchIcon().c_str());

        jsonControl->Put("icon", jsonIcon);
        jsonValue->Put("controlButton", jsonControl);
        return jsonValue->ToString();
    }
    return jsonValue->ToString();
}

std::string SideBarComposedElement::GetShowControlButton() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return ConvertBoolToString(render->GetShowControlButton());
    }
    return "true";
}

std::string SideBarComposedElement::GetSideBarWidth() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return std::to_string(render->GetSideBarWidth().Value()).c_str();
    }
    return "200";
}

std::string SideBarComposedElement::GetMinSideBarWidth() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return std::to_string(render->GetSideBarMinWidth().Value()).c_str();
    }
    return "200";
}

std::string SideBarComposedElement::GetMaxSideBarWidth() const
{
    auto render = GetRenderSideBar();
    if (render) {
        return std::to_string(render->GetSideBarMaxWidth().Value()).c_str();
    }
    return "280";
}

RefPtr<RenderSideBarContainer> SideBarComposedElement::GetRenderSideBar() const
{
    auto node = GetInspectorNode(SideBarContainerElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderSideBarContainer>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
