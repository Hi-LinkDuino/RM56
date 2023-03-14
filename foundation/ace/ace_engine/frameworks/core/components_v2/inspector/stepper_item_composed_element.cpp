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


#include "core/components_v2/inspector/stepper_item_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/stepper/render_stepper_item.h"
#include "core/components_v2/inspector/utils.h"
namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const StepperItemComposedElement&)>> CREATE_JSON_MAP {
    { "prevLabel", [](const StepperItemComposedElement& inspector) { return inspector.GetPrevLabel(); } },
    { "nextLabel", [](const StepperItemComposedElement& inspector) { return inspector.GetNextLabel(); } },
    { "status", [](const StepperItemComposedElement& inspector) { return inspector.GetStatus(); } },
};
}

void StepperItemComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    for (const auto& value : CREATE_JSON_MAP) {
        DumpLog::GetInstance().AddDesc(std::string(value.first + ":").append(value.second(*this)));
    }
}

std::unique_ptr<JsonValue> StepperItemComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string StepperItemComposedElement::GetPrevLabel() const
{
    auto renderStepperItem = GetInspectorElement<RenderStepperItem>(StepperItemElement::TypeId());
    if (!renderStepperItem) {
        return "";
    }
    auto label = renderStepperItem->GetLabel();
    return label.leftLabel;
}

std::string StepperItemComposedElement::GetNextLabel() const
{
    auto renderStepperItem = GetInspectorElement<RenderStepperItem>(StepperItemElement::TypeId());
    if (!renderStepperItem) {
        return "";
    }
    auto label = renderStepperItem->GetLabel();
    return label.rightLabel;
}

std::string StepperItemComposedElement::GetStatus() const
{
    auto renderStepperItem = GetInspectorElement<RenderStepperItem>(StepperItemElement::TypeId());
    if (!renderStepperItem) {
        return "ItemState.Normal";
    }
    auto label = renderStepperItem->GetLabel();
    auto status = label.initialStatus;
    if (status == "normal") {
        return "ItemState.Normal";
    } else if (status == "disabled") {
        return "ItemState.Disabled";
    } else if (status == "waiting") {
        return "ItemState.Waiting";
    } else if (status == "skip") {
        return "ItemState.Skip";
    } else {
        return "ItemState.Normal";
    }
    
}

void StepperItemComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto stepperItemV2 = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!stepperItemV2) {
        LOGE("get GetStepperItemV2 failed");
        return;
    }
    stepperItemV2->UpdateChildWithSlot(nullptr, newComponent, -1, -1);
    stepperItemV2->MarkDirty();
}

void StepperItemComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto stepperItemV2 = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!stepperItemV2) {
        LOGE("get GetStepperItemV2 failed");
        return;
    }
    auto child = stepperItemV2->GetFirstChild();
    stepperItemV2->UpdateChildWithSlot(child, newComponent, -1, -1);
    stepperItemV2->MarkDirty();
}

void StepperItemComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto stepperItemV2 = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!stepperItemV2) {
        LOGE("get GetStepperItemV2 failed");
        return;
    }
    auto child = stepperItemV2->GetFirstChild();
    stepperItemV2->UpdateChildWithSlot(child, nullptr, -1, -1);
    stepperItemV2->MarkDirty();
}

} // namespace OHOS::Ace::V2