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

#include "core/components/flex/flex_component_v2.h"

#include "core/components/flex/flex_item_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/pipeline/base/composed_component.h"
#include "frameworks/core/components/foreach/for_each_component.h"
#include "frameworks/core/components/ifelse/if_else_component.h"

namespace OHOS::Ace {
namespace {

RefPtr<Component> AddFlexItemComponent(const RefPtr<Component>& component)
{
    if (AceType::InstanceOf<FlexItemComponent>(component)) {
        return component;
    }
    if (!component) {
        return component;
    }

    auto composedComponent = AceType::DynamicCast<ComposedComponent>(component);
    if (composedComponent) {
        auto composedChild = composedComponent->GetChild();
        if (AceType::InstanceOf<FlexItemComponent>(composedChild)) {
            return component;
        }

        if (AceType::InstanceOf<ComposedComponent>(composedChild)) {
            auto newComposedChild = AddFlexItemComponent(composedChild);
            composedComponent->SetChild(newComposedChild);
            return composedComponent;
        }

        composedComponent->SetChild(nullptr);
        auto newFlexItem = AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0);
        newFlexItem->SetChild(composedChild);
        composedComponent->SetChild(newFlexItem);
        composedComponent->SetNeedReserveChild(true);
        return component;
    }

    auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(component);
    if (!multiComposedComponent) {
        auto newFlexItem = AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0);
        newFlexItem->SetChild(component);
        return newFlexItem;
    }

    auto children = multiComposedComponent->GetChildren();
    multiComposedComponent->RemoveChildren();
    for (const auto& childComponent : children) {
        multiComposedComponent->AddChild(AddFlexItemComponent(childComponent));
    }

    return component;
}

}

void FlexComponentV2::OnChildAppended(const RefPtr<Component>& child)
{
    RefPtr<Component> newChild = AddFlexItemComponent(child);
    if (newChild != child) {
        RemoveChildDirectly(child);
        AppendChildDirectly(newChild);
    }
}

} // namespace OHOS::Ace
