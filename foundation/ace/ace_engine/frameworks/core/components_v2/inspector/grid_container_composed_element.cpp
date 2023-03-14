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

#include "core/components_v2/inspector/grid_container_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/box/render_box_base.h"
#include "core/components/common/layout/grid_container_info.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::unique_ptr<JsonValue>(const GridContainerComposedElement&)>> CREATE_JSON_MAP {
    { "constructor", [](const GridContainerComposedElement& inspector) { return inspector.GetConstructor(); } },
};

}

void GridContainerComposedElement::Dump()
{
    ColumnComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("columns: ").append(GetColumns()));
    DumpLog::GetInstance().AddDesc(
        std::string("sizeType: ").append(GetSizeType()));
    DumpLog::GetInstance().AddDesc(
        std::string("gutter: ").append(GetGutter()));
    DumpLog::GetInstance().AddDesc(
        std::string("margin: ").append(GetMarginLeft()));
}

std::unique_ptr<JsonValue> GridContainerComposedElement::ToJsonObject() const
{
    auto resultJson = ColumnComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::unique_ptr<JsonValue> GridContainerComposedElement::GetConstructor() const
{
    auto jsonStr = JsonUtil::Create(false);
    jsonStr->Put("columns", GetColumns().c_str());
    jsonStr->Put("sizeType", GetSizeType().c_str());
    jsonStr->Put("gutter", GetGutter().c_str());
    jsonStr->Put("margin", GetMarginLeft().c_str());
    return jsonStr;
}

std::string GridContainerComposedElement::GetColumns() const
{
    auto node = GetRenderBox();
    if (node) {
        return std::to_string(node->GetGridContainerInfo()->GetColumns());
    }
    return "";
}

std::string GridContainerComposedElement::GetSizeType() const
{
    auto node = GetRenderBox();
    if (node) {
        switch (node->GetGridContainerInfo()->GetSizeType()) {
            case GridSizeType::UNDEFINED:
                return std::string("SizeType.Auto");
            case GridSizeType::XS:
                return std::string("SizeType.XS");
            case GridSizeType::SM:
                return std::string("SizeType.SM");
            case GridSizeType::MD:
                return std::string("SizeType.MD");
            case GridSizeType::LG:
                return std::string("SizeType.LG");
            default:
                break;
        }
    }
    return std::string("SizeType.Auto");
}

std::string GridContainerComposedElement::GetGutter() const
{
    auto node = GetRenderBox();
    if (node) {
        return node->GetGridContainerInfo()->GetGutterWidth().ToString();
    }
    return "-";
}

std::string GridContainerComposedElement::GetMarginLeft() const
{
    auto node = GetRenderBox();
    if (node) {
        return node->GetMargin(DimensionHelper(&Edge::SetLeft, &Edge::Left)).ToString();
    }
    return "-";
}

void GridContainerComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto boxElement = GetContentElement<BoxElement>(BoxElement::TypeId());
    if (!boxElement) {
        LOGE("get GetBoxElement failed");
        return;
    }
    boxElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    boxElement->MarkDirty();
}

void GridContainerComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto boxElement = GetContentElement<BoxElement>(BoxElement::TypeId());
    if (!boxElement) {
        LOGE("get GetBoxElement failed");
        return;
    }
    auto child = boxElement->GetChildBySlot(slot);
    if (!child) {
        LOGE("boxElement get GetChildBySlot failed");
        return;
    }
    boxElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    boxElement->MarkDirty();
}

void GridContainerComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto boxElement = GetContentElement<BoxElement>(BoxElement::TypeId());
    if (!boxElement) {
        LOGE("get GetBoxElement failed");
        return;
    }
    auto child = boxElement->GetChildBySlot(slot);
    boxElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    boxElement->MarkDirty();
}

} // namespace OHOS::Ace::V2