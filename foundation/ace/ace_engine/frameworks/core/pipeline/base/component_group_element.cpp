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

#include "core/pipeline/base/component_group_element.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/common/frontend.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/base/multi_composed_component.h"

namespace OHOS::Ace {

RefPtr<Element> ComponentGroupElement::Create()
{
    return AceType::MakeRefPtr<ComponentGroupElement>();
}

void ComponentGroupElement::PerformBuild()
{
    RefPtr<ComponentGroup> componentGroup = AceType::DynamicCast<ComponentGroup>(component_);
    if (!componentGroup) {
        LOGW("Should be component group, but %s", AceType::TypeName(component_));
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGW("Pipeline context is invalid");
        return;
    }

    const auto& newComponents = componentGroup->GetChildren();
    if (context->GetIsDeclarative() && componentGroup->GetUpdateType() != UpdateType::REBUILD) {
        UpdateChildrenForDeclarative(newComponents);
    } else {
        UpdateChildren(newComponents);
    }
}

void ComponentGroupElement::UpdateChildren(const std::list<RefPtr<Component>>& newComponents)
{
    auto itChild = children_.begin();
    auto itChildEnd = children_.end();
    auto itComponent = newComponents.begin();
    auto itComponentEnd = newComponents.end();

    while (itChild != itChildEnd && itComponent != itComponentEnd) {
        const auto& child = *itChild;
        const auto& component = *itComponent;
        if (child->NeedUpdateWithComponent(component)) {
            if (!child->CanUpdate(component)) {
                break;
            }
            UpdateChild(child, component);
        }
        ++itChild;
        ++itComponent;
    }

    // children_ will be modified during UpdateChild.(some items will be removed from children_)
    while (itChild != itChildEnd) {
        UpdateChild(*(itChild++), nullptr);
    }

    while (itComponent != itComponentEnd) {
        UpdateChild(nullptr, *(itComponent++));
    }
}

void ComponentGroupElement::UpdateChildrenForDeclarative(const std::list<RefPtr<Component>>& newComponents)
{
    int32_t slot = 0;
    int32_t renderSlot = 0;
    if (children_.empty()) {
        for (const auto& component : newComponents) {
            auto newChild = UpdateChildWithSlot(nullptr, component, slot++, renderSlot);
            renderSlot += newChild->CountRenderNode();
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
        auto newChild = UpdateChildWithSlot(*(itChild++), component, slot++, renderSlot);
        renderSlot += newChild->CountRenderNode();
    }
}

} // namespace OHOS::Ace
