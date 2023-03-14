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

#include "frameworks/core/pipeline/base/multi_composed_element.h"

#include "core/pipeline/base/multi_composed_component.h"
#include "frameworks/core/common/frontend.h"
#include "frameworks/core/pipeline/base/render_element.h"

namespace OHOS::Ace {

bool MultiComposedElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto multiComposed = AceType::DynamicCast<MultiComposedComponent>(newComponent);
    return multiComposed ? id_ == multiComposed->GetId() : false;
}

void MultiComposedElement::PerformBuild()
{
    auto multiComposed = AceType::DynamicCast<MultiComposedComponent>(component_);
    if (!multiComposed) {
        LOGW("MultiComposedElement: component MUST be instance of MultiComposedComponent");
        return;
    }

    UpdateChildren(multiComposed->GetChildren());
}

void MultiComposedElement::UpdateChildren(const std::list<RefPtr<Component>>& newComponents)
{
    if (component_->GetUpdateType() == UpdateType::REBUILD) {
        UpdateChildrenForRebuild(newComponents);
        return;
    }
    int32_t slot = 0;
    countRenderNode_ = 0;
    bool useSlot = GetRenderSlot() < 0 ? false : true;
    if (children_.empty()) {
        for (const auto& component : newComponents) {
            auto newChild = UpdateChildWithSlot(
                nullptr, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
            countRenderNode_ += newChild->CountRenderNode();
        }
        return;
    }

    // For declarative frontend, the component tree is very stable,
    // so size of children MUST be matched between elements and components
    if (children_.size() != newComponents.size()) {
        LOGW("Size of old children and new components are mismatched");
        return;
    }

    auto itChild = children_.begin();
    for (const auto& component : newComponents) {
        auto newChild = UpdateChildWithSlot(
            *(itChild++), component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
        countRenderNode_ += newChild->CountRenderNode();
    }
}

void MultiComposedElement::UpdateChildrenForRebuild(const std::list<RefPtr<Component>>& newComponents)
{
    auto itChildStart = children_.begin();
    auto itChildEnd = children_.end();
    auto itComponentStart = newComponents.begin();
    auto itComponentEnd = newComponents.end();
    int32_t slot = 0;

    countRenderNode_ = 0;
    bool useSlot = GetRenderSlot() < 0 ? false : true;

    // 1. Try to update children at start with new components by order
    while (itChildStart != itChildEnd && itComponentStart != itComponentEnd) {
        const auto& child = *itChildStart;
        const auto& component = *itComponentStart;
        if (!child->CanUpdate(component)) {
            break;
        }
        auto newChild = UpdateChildWithSlot(
            child, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
        countRenderNode_ += newChild->CountRenderNode();
        ++itChildStart;
        ++itComponentStart;
    }

    // 2. Try to find children at end with new components by order
    while (itChildStart != itChildEnd && itComponentStart != itComponentEnd) {
        const auto& child = *(--itChildEnd);
        const auto& component = *(--itComponentEnd);
        if (!child->CanUpdate(component)) {
            ++itChildEnd;
            ++itComponentEnd;
            break;
        }
    }

    // 3. Collect children at middle
    std::unordered_map<ComposeId, RefPtr<Element>> elements;
    while (itChildStart != itChildEnd) {
        const auto& child = *(itChildStart++);
        auto composedElement = AceType::DynamicCast<ComposedElement>(child);
        if (composedElement) {
            elements.emplace(composedElement->GetId(), child);
        } else {
            UpdateChildWithSlot(child, nullptr, DEFAULT_ELEMENT_SLOT, DEFAULT_RENDER_SLOT);
        }
    }

    // 4. Try to update children at middle with new components by order
    while (itComponentStart != itComponentEnd) {
        const auto& component = *(itComponentStart++);
        auto composedComponent = AceType::DynamicCast<BaseComposedComponent>(component);
        if (!composedComponent) {
            auto newChild = UpdateChildWithSlot(
                nullptr, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
            countRenderNode_ += newChild->CountRenderNode();
            continue;
        }
        auto it = elements.find(composedComponent->GetId());
        if (it == elements.end()) {
            auto newChild = UpdateChildWithSlot(
                nullptr, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
            countRenderNode_ += newChild->CountRenderNode();
            continue;
        }

        const auto& child = it->second;
        if (child->CanUpdate(component)) {
            auto newChild = UpdateChildWithSlot(
                child, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
            countRenderNode_ += newChild->CountRenderNode();
        } else {
            auto newChild = UpdateChildWithSlot(
                nullptr, component, slot++, useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
            countRenderNode_ += newChild->CountRenderNode();
        }

        elements.erase(it);
    }

    // 5. Remove these useless children
    for (const auto& node : elements) {
        UpdateChildWithSlot(node.second, nullptr, DEFAULT_ELEMENT_SLOT, DEFAULT_RENDER_SLOT);
    }

    // 6. Try to update children at end with new components by order
    while (itChildEnd != children_.end() && itComponentEnd != newComponents.end()) {
        auto newChild = UpdateChildWithSlot(*(itChildEnd++), *(itComponentEnd++), slot++,
            useSlot ? countRenderNode_ + GetRenderSlot() : DEFAULT_RENDER_SLOT);
        countRenderNode_ += newChild->CountRenderNode();
    }
}

} // namespace OHOS::Ace
