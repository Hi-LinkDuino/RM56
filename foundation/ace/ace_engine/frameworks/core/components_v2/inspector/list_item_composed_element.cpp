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

#include "core/components_v2/inspector/list_item_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"
#include "core/components_v2/list/list_item_component.h"
#include "core/components_v2/list/render_list_item.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ListItemComposedElement&)>> CREATE_JSON_MAP {
    { "sticky", [](const ListItemComposedElement& inspector) { return inspector.GetSticky(); } },
    { "editable", [](const ListItemComposedElement& inspector) { return inspector.GetEditable(); } }
};
}

void ListItemComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("sticky: ").append(GetSticky()));
    DumpLog::GetInstance().AddDesc(
        std::string("editable: ").append(GetEditable()));
}

std::unique_ptr<JsonValue> ListItemComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string ListItemComposedElement::GetSticky() const
{
    auto node = GetInspectorNode(ListItemElement::TypeId(), true);
    if (!node) {
        return "Sticky.None";
    }
    auto renderListItem = AceType::DynamicCast<RenderListItem>(node);
    if (renderListItem) {
        auto stickyMode = renderListItem->GetSticky();
        if (stickyMode == StickyMode::NORMAL) {
            return "Sticky.Normal";
        } else if (stickyMode == StickyMode::OPACITY) {
            return "Sticky.Opacity";
        }
    }
    return "Sticky.None";
}

std::string ListItemComposedElement::GetEditable() const
{
    auto node = GetInspectorNode(ListItemElement::TypeId(), true);
    if (!node) {
        return "false";
    }
    auto renderListItem = AceType::DynamicCast<RenderListItem>(node);
    if (renderListItem) {
        auto editMode = renderListItem->GetEditMode();
        if (editMode == EditMode::NONE) {
            return "false";
        } else if (editMode == EditMode::MOVABLE) {
            return "EditMode.Movable";
        } else if (editMode == EditMode::DELETABLE) {
            return "EditMode.Deletable";
        } else if (editMode == (EditMode::DELETABLE | EditMode::MOVABLE)) {
            return "true";
        }
    }
    return "false";
}

int32_t ListItemComposedElement::GetZIndex() const 
{
    auto node = GetInspectorNode(ListItemElement::TypeId(), true);
    if (!node) {
        return 0;
    }
    auto renderListItem = AceType::DynamicCast<RenderListItem>(node);
    if (!renderListItem) {
        return 0;
    }
    return renderListItem->GetZIndex();
}

void ListItemComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto child = GetInspectorComposedElement();
    if (!child) {
        return;
    }                                                                                
    auto parentElement = child->GetElementParent().Upgrade();
    if (!parentElement) {
        return;
    }
    slot = DEFAULT_ELEMENT_SLOT;
    parentElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    parentElement->MarkDirty();
}

} // namespace OHOS::Ace::V2