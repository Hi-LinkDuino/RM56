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

#include "core/components/stack/stack_component.h"

#ifndef WEARABLE_PRODUCT
#include "core/components/navigation_bar/navigation_container_component.h"
#endif

namespace OHOS::Ace {

void StackComponent::SetNavigationBar(const RefPtr<Component>& navigationBar)
{
    navigationBar_ = navigationBar;
    auto rootChild = GetChildren().front();
#ifndef WEARABLE_PRODUCT
    auto container = AceType::MakeRefPtr<NavigationContainerComponent>(navigationBar, rootChild);
#else
    auto container = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::FLEX_START, FlexAlign::STRETCH, std::list<RefPtr<Component>>());
    container->SetContainsNavigation(true);
    container->SetMainAxisSize(MainAxisSize::MIN);
    container->AppendChild(navigationBar_);
    container->AppendChild(rootChild);
#endif
    RemoveChild(rootChild);
    InsertChild(0, container);
}

} // namespace OHOS::Ace
