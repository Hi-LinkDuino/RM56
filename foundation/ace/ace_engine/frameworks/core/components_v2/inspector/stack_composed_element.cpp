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

#include "core/components_v2/inspector/stack_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const StackComposedElement&)>> CREATE_JSON_MAP {
    { "alignContent", [](const StackComposedElement& inspector) { return inspector.GetAlignContent(); } }
};

} // namespace

void StackComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("alignContent: ").append(GetAlignContent()));
    DumpLog::GetInstance().AddDesc(std::string("stackFit: ").append(GetStackFit()));
    DumpLog::GetInstance().AddDesc(std::string("overflow: ").append(GetOverflow()));
}

std::unique_ptr<JsonValue> StackComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string StackComposedElement::GetAlignContent() const
{
    auto renderStack = GetRenderStack();
    return renderStack ? renderStack->GetAlignment().GetAlignmentStr(GetDirection()) : "";
}

std::string StackComposedElement::GetStackFit() const
{
    auto renderStack = GetRenderStack();
    auto stackFit = renderStack ? renderStack->GetStackFit() : StackFit::KEEP;
    return ConvertStackFitToString(stackFit);
}

std::string StackComposedElement::GetOverflow() const
{
    auto renderStack = GetRenderStack();
    auto overflow = renderStack ? renderStack->GetOverflow() : Overflow::OBSERVABLE;
    return ConvertOverflowToString(overflow);
}

RefPtr<RenderStack> StackComposedElement::GetRenderStack() const
{
    auto node = GetInspectorNode(StackElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderStack>(node);
    }
    return nullptr;
}

void StackComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto stackElement = GetContentElement<StackElement>(StackElement::TypeId());
    if (!stackElement) {
        LOGE("get GetStackElement failed");
        return;
    }
    stackElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    stackElement->MarkDirty();
}

void StackComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto stackElement = GetContentElement<StackElement>(StackElement::TypeId());
    if (!stackElement) {
        LOGE("get GetStackElement failed");
        return;
    }
    auto child = stackElement->GetChildBySlot(slot);
    stackElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    stackElement->MarkDirty();
}

void StackComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto stackElement = GetContentElement<StackElement>(StackElement::TypeId());
    if (!stackElement) {
        LOGE("get GetStackElement failed");
        return;
    }
    auto child = stackElement->GetChildBySlot(slot);
    stackElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    stackElement->MarkDirty();
}
} // namespace OHOS::Ace::V2