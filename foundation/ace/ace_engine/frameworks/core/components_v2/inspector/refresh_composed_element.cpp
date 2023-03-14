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

#include "core/components_v2/inspector/refresh_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/refresh/render_refresh.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const  RefreshComposedElement&)>> CREATE_JSON_MAP {
    { "refreshing", [](const  RefreshComposedElement& inspector) { return inspector.GetRefreshing(); } },
    { "offset", [](const  RefreshComposedElement& inspector) { return inspector.GetRefreshOffset(); } },
    { "friction", [](const  RefreshComposedElement& inspector) { return inspector.GetFriction(); } },
};
}

void RefreshComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("refreshing: ").append(GetRefreshing()));
    DumpLog::GetInstance().AddDesc(
        std::string("offset: ").append(GetRefreshOffset()));
    DumpLog::GetInstance().AddDesc(
        std::string("friction: ").append(GetFriction()));
}

std::unique_ptr<JsonValue>  RefreshComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string RefreshComposedElement::GetRefreshing() const
{
    auto node = GetInspectorNode(RefreshElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderRefresh = AceType::DynamicCast<RenderRefresh>(node);
    if (renderRefresh) {
        return ConvertBoolToString(renderRefresh->GetRefreshing());
    }
    return "-";
}

std::string RefreshComposedElement::GetRefreshOffset() const
{
    auto node = GetInspectorNode(RefreshElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderRefresh = AceType::DynamicCast<RenderRefresh>(node);
    if (renderRefresh) {
        return renderRefresh->GetInspectorOffset().ToString();
    }
    return "-";
}

std::string RefreshComposedElement::GetFriction() const
{
    auto node = GetInspectorNode(RefreshElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderRefresh = AceType::DynamicCast<RenderRefresh>(node);
    if (renderRefresh) {
        return std::to_string(renderRefresh->GetFriction());
    }
    return "-";
}

void RefreshComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto refreshElement = GetContentElement<RefreshElement>(RefreshElement::TypeId());
    if (!refreshElement) {
        LOGE("get GetRefreshElement failed");
        return;
    }
    refreshElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    refreshElement->MarkDirty();
    LOGD("refresh AddChildWithSlot");
}

void RefreshComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto refreshElement = GetContentElement<RefreshElement>(RefreshElement::TypeId());
    if (!refreshElement) {
        LOGE("get GetRefreshElement failed");
        return;
    }
    auto child = GetElementChildBySlot(refreshElement, slot);
    if (!child) {
        LOGE("refreshElement get GetChildBySlot failed");
        return;
    }
    refreshElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    refreshElement->MarkDirty();
    LOGD("refresh UpdateChildWithSlot");
}

void RefreshComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto refreshElement = GetContentElement<RefreshElement>(RefreshElement::TypeId());
    if (!refreshElement) {
        LOGE("get GetRefreshElement failed");
        return;
    }
    auto child = GetElementChildBySlot(refreshElement, slot);
    if (!child) {
        LOGE("refreshElement get GetChildBySlot failed");
        return;
    }
    refreshElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    refreshElement->MarkDirty();
    LOGD("refresh DeleteChildWithSlot");
}
} // namespace OHOS::Ace::V2