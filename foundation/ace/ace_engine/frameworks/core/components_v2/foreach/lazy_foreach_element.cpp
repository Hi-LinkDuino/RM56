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

#include "core/components_v2/foreach/lazy_foreach_element.h"

namespace OHOS::Ace::V2 {

bool LazyForEachElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto lazyForEach = AceType::DynamicCast<LazyForEachComponent>(newComponent);
    return lazyForEach ? lazyForEach->GetId() == GetId() : false;
}

void LazyForEachElement::Update()
{
    // Save lazy foreach component
    lazyForEachComponent_ = AceType::DynamicCast<LazyForEachComponent>(component_);
    ForEachElement::Update();
}

void LazyForEachElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (!newComponent && lazyForEachComponent_) {
        // Clear all child of lazy foreach component while cleaning componet
        lazyForEachComponent_->RemoveChildren();
    }
    ForEachElement::SetNewComponent(newComponent);
}

} // namespace OHOS::Ace::V2
