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


#include "core/components_v2/inspector/stepper_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/stepper/render_stepper.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const StepperComposedElement&)>> CREATE_JSON_MAP {
    { "index", [](const StepperComposedElement& inspector) { return inspector.GetIndex(); } }
};

}

void StepperComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("index: ").append(GetIndex()));
}

std::unique_ptr<JsonValue> StepperComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string StepperComposedElement::GetIndex() const
{
    auto node = GetStepperComponent();
    if (node) {
        return std::to_string(node->GetIndex());
    }
    return "0";
}

RefPtr<StepperComponent> StepperComposedElement::GetStepperComponent() const
{
    auto node = GetInspectorNode(StepperElement::TypeId());
    if (node) {
        auto renderStepper = AceType::DynamicCast<RenderStepper>(node);
        return renderStepper->GetStepperComponent();
    }
    return nullptr;
}

void StepperComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto stepperComponent = GetStepperComponent();
    stepperComponent->InsertChild(slot, newComponent);
    auto renderStepper = GetInspectorElement<RenderStepper>(StepperElement::TypeId());
    if (!renderStepper) {
        return;
    }
    renderStepper->Update(stepperComponent);
}

} // namespace OHOS::Ace::V2