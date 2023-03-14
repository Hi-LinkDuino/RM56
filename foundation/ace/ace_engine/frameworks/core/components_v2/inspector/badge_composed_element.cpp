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

#include <unordered_map>
#include "core/components_v2/inspector/badge_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

const std::unordered_map<std::string, std::function<std::string(const BadgeComposedElement&)>> CREATE_JSON_MAP {
    { "count", [](const BadgeComposedElement& inspector) { return inspector.GetCount(); } },
    { "maxCount", [](const BadgeComposedElement& inspector) { return inspector.GetMaxCount(); } },
    { "position", [](const BadgeComposedElement& inspector) { return inspector.GetBadgePosition(); } },
    { "value", [](const BadgeComposedElement& inspector) { return inspector.GetLabel(); } },
    { "style", [](const BadgeComposedElement& inspector) { return inspector.GetStyle(); } }
};

void BadgeComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("badge_composed_element"));
    DumpLog::GetInstance().AddDesc(
        std::string("count: ").append(GetCount()));
    DumpLog::GetInstance().AddDesc(
        std::string("position: ").append(GetBadgePosition()));
    DumpLog::GetInstance().AddDesc(
        std::string("maxCount: ").append(GetMaxCount()));
    DumpLog::GetInstance().AddDesc(
        std::string("value: ").append(GetLabel()));
    DumpLog::GetInstance().AddDesc(
        std::string("style: ").append(GetStyle()));
}

std::unique_ptr<OHOS::Ace::JsonValue> BadgeComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string BadgeComposedElement::GetCount(void) const
{
    auto renderBadge = GetRenderBadge();
    int64_t count = renderBadge ? renderBadge->GetBadgeComponent()->GetMessageCount() : 0;
    return std::to_string(count);
}

std::string BadgeComposedElement::GetMaxCount(void) const
{
    auto renderBadge = GetRenderBadge();
    int64_t maxCount = renderBadge ? renderBadge->GetBadgeComponent()->GetMaxCount() : 99;
    return std::to_string(maxCount);
}

std::string BadgeComposedElement::GetBadgePosition(void) const
{
    auto renderBadge = GetRenderBadge();
    if (renderBadge) {
        switch (renderBadge->GetBadgeComponent()->GetBadgePosition()) {
            case BadgePosition::RIGHT_TOP:
                return std::string("BadgePosition.RightTop");
            case BadgePosition::RIGHT:
                return std::string("BadgePosition.Right");
            case BadgePosition::LEFT:
                return std::string("BadgePosition.Left");
            default:
                break;
        }
    }
    return std::string("-");
}

std::string BadgeComposedElement::GetLabel(void) const
{
    auto renderBadge = GetRenderBadge();
    std::string label =  renderBadge ? renderBadge->GetBadgeComponent()->GetBadgeLabel() : "";
    return label;
}

std::string BadgeComposedElement::GetStyle() const
{
    auto render = GetRenderBadge();
    auto jsonValue = JsonUtil::Create(false);
    if (render) {
        auto style = render->GetBadgeComponent();
        if (style) {
            jsonValue->Put("color", ConvertColorToString(style->GetBadgeTextColor()).c_str());
            jsonValue->Put("fontSize", style->GetBadgeFontSize().ToString().c_str());
            jsonValue->Put("badgeColor", ConvertColorToString(style->GetBadgeColor()).c_str());
            jsonValue->Put("badgeSize", style->GetBadgeCircleSize().ToString().c_str());
        }
    }
    return jsonValue->ToString();
}

OHOS::Ace::RefPtr<OHOS::Ace::RenderBadge> BadgeComposedElement::GetRenderBadge() const
{
    auto node = GetInspectorNode(BadgeElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderBadge>(node);
    }
    return nullptr;
}

void BadgeComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto badgeElement = GetContentElement<BadgeElement>(BadgeElement::TypeId());
    if (!badgeElement) {
        LOGE("get GetBadgeElement failed");
        return;
    }
    badgeElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    badgeElement->MarkDirty();
    LOGD("badge AddChildWithSlot");
}

void BadgeComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto badgeElement = GetContentElement<BadgeElement>(BadgeElement::TypeId());
    if (!badgeElement) {
        LOGE("get GetBadgeElement failed");
        return;
    }
    auto child = GetElementChildBySlot(badgeElement, slot);
    if (!child) {
        LOGE("badgeElement get GetChildBySlot failed");
        return;
    }
    badgeElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    badgeElement->MarkDirty();
    LOGD("badge UpdateChildWithSlot");
}

void BadgeComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto badgeElement = GetContentElement<BadgeElement>(BadgeElement::TypeId());
    if (!badgeElement) {
        LOGE("get GetBadgeElement failed");
        return;
    }
    auto child = GetElementChildBySlot(badgeElement, slot);
    if (!child) {
        LOGE("badgeElement get GetChildBySlot failed");
        return;
    }
    badgeElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    badgeElement->MarkDirty();
    LOGD("badge DeleteChildWithSlot");
}

} // namespace OHOS::Ace::V2
