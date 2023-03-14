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

#include "core/components_v2/inspector/wrap_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/wrap/render_wrap.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::unique_ptr<JsonValue>(const WrapComposedElement&)>> CREATE_JSON_MAP {
    { "constructor", [](const WrapComposedElement& inspector) { return inspector.GetConstructor(); } }
};

}

void WrapComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("direction: ").append(GetFlexDirection()));
    DumpLog::GetInstance().AddDesc(
        std::string("wrap: ").append(GetWrap()));
    DumpLog::GetInstance().AddDesc(
        std::string("justifyContent: ").append(GetJustifyContent()));
    DumpLog::GetInstance().AddDesc(
        std::string("alignItems: ").append(GetAlignItems()));
    DumpLog::GetInstance().AddDesc(
        std::string("alignContent: ").append(GetAlignContent()));
}

std::unique_ptr<JsonValue> WrapComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::unique_ptr<JsonValue> WrapComposedElement::GetConstructor() const
{
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("direction", GetFlexDirection().c_str());
    jsonValue->Put("wrap", GetWrap().c_str());
    jsonValue->Put("justifyContent", GetJustifyContent().c_str());
    jsonValue->Put("alignItems", GetAlignItems().c_str());
    jsonValue->Put("alignContent", GetAlignContent().c_str());
    return jsonValue;
}

std::string WrapComposedElement::GetFlexDirection() const
{
    auto node = GetInspectorNode(WrapElement::TypeId());
    if (!node) {
        return "FlexDirection.Row";
    }
    auto renderWrap = AceType::DynamicCast<RenderWrap>(node);
    if (renderWrap) {
        return ConvertWrapDirectionToStirng(renderWrap->GetDirection());
    }
    return "FlexDirection.Row";
}

std::string WrapComposedElement::GetWrap() const
{
    auto node = GetInspectorNode(WrapElement::TypeId());
    if (!node) {
        return "FlexWrap.NoWrap";
    }
    auto renderWrap = AceType::DynamicCast<RenderWrap>(node);
    if (renderWrap) {
        if (renderWrap->GetDirection() == WrapDirection::HORIZONTAL_REVERSE
            || renderWrap->GetDirection() == WrapDirection::VERTICAL_REVERSE) {
                return "FlexWrap.WrapReverse";
            }
        return "FlexWrap.Wrap";
    }
    return "FlexWrap.NoWrap";
}

std::string WrapComposedElement::GetJustifyContent() const
{
    auto node = GetInspectorNode(WrapElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    auto renderWrap = AceType::DynamicCast<RenderWrap>(node);
    if (renderWrap) {
        return ConvertWrapAlignmentToStirng(renderWrap->GetJustifyContent());
    }
    return "FlexAlign.Start";
}

std::string WrapComposedElement::GetAlignItems() const
{
    auto node = GetInspectorNode(WrapElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    auto renderWrap = AceType::DynamicCast<RenderWrap>(node);
    if (renderWrap) {
        return ConvertWrapAlignmentToStirng(renderWrap->GetAlignItems());
    }
    return "FlexAlign.Start";
}

std::string WrapComposedElement::GetAlignContent() const
{
    auto node = GetInspectorNode(WrapElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    auto renderWrap = AceType::DynamicCast<RenderWrap>(node);
    if (renderWrap) {
        return ConvertWrapAlignmentToStirng(renderWrap->GetAlignContent());
    }
    return "FlexAlign.Start";
}

} // namespace OHOS::Ace::V2