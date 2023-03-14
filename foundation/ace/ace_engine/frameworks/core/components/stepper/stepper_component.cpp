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

#include "core/components/stepper/stepper_component.h"

#include "core/components/stepper/render_stepper.h"
#include "core/components/stepper/stepper_element.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DEFAULT_NODE_INDEX = -1;

} // namespace

StepperComponent::StepperComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    stepperController_ = AceType::MakeRefPtr<StepperController>();
}

RefPtr<Element> StepperComponent::CreateElement()
{
    return AceType::MakeRefPtr<StepperElement>();
}

RefPtr<RenderNode> StepperComponent::CreateRenderNode()
{
    return RenderStepper::Create();
}

void StepperComponent::SetStepperLabels(const std::vector<StepperLabels>& stepperLabels)
{
    if (stepperLabels.empty()) {
        LOGW("StepperLabels are empty.");
        return;
    }
    stepperLabels_ = stepperLabels;
}

const std::vector<StepperLabels>& StepperComponent::GetStepperLabels() const
{
    return stepperLabels_;
}

void StepperComponent::InsertChild(int32_t position, const RefPtr<Component>& child)
{
    if (position < 0) {
        LOGE("InsertChild: the position is negative");
        return;
    }
    auto item = StepperItemComponent::GetStepperItem(child);
    if (!item) {
        LOGE("InsertChild: no stepper item in child");
        return;
    }
    auto pos = static_cast<uint32_t>(position);
    const auto& children = GetChildren();
    if (!child || pos > children.size()) {
        return;
    }
    if (pos == children.size()) {
        AppendChild(child);
        return;
    }
    item->SetIndex(pos);
    LOGD("InsertChild: position: %{public}d, index: %{public}d", pos, item->GetIndex());
    ComponentGroup::InsertChild(position, child);
    auto labelIter = stepperLabels_.begin();
    std::advance(labelIter, position);
    stepperLabels_.insert(labelIter, item->GetLabel());
    auto textIter = labelsTextStyles_.begin();
    std::advance(textIter, position);
    labelsTextStyles_.insert(textIter, item->GetTextStyle());
}

void StepperComponent::AppendChild(const RefPtr<Component>& child)
{
    auto item = StepperItemComponent::GetStepperItem(child);
    if (!item) {
        LOGE("AppendChild: no stepper item in child");
        return;
    }
    item->SetIndex(GetChildren().size());
    LOGD("AppendChild: index: %{public}d", item->GetIndex());
    ComponentGroup::AppendChild(child);
    AppendLabel(item->GetLabel());
    AppendTextStyle(item->GetTextStyle());
}

void StepperComponent::RemoveChild(const RefPtr<Component>& child)
{
    auto item = StepperItemComponent::GetStepperItem(child);
    if (!item) {
        LOGE("RemoveChild: no stepper item in child");
        return;
    }
    // removing children will reset label and styles
    stepperLabels_.clear();
    labelsTextStyles_.clear();
    item->SetIndex(DEFAULT_NODE_INDEX);
    ComponentGroup::RemoveChild(child);
}

} // namespace OHOS::Ace
