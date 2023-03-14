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

#include "frameworks/core/components/ifelse/if_else_element.h"

#include "frameworks/core/components/ifelse/if_else_component.h"

namespace OHOS::Ace {

bool IfElseElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto ifElseComponent = AceType::DynamicCast<IfElseComponent>(newComponent);
    return ifElseComponent ? branchId_ == ifElseComponent->BranchId() : false;
}

void IfElseElement::Update()
{
    MultiComposedElement::Update();

    auto ifElseComponent = AceType::DynamicCast<IfElseComponent>(component_);
    if (!ifElseComponent) {
        LOGW("IfElseElement: component MUST be instance of IfElseComponent");
        return;
    }

    branchId_ = ifElseComponent->BranchId();
}

} // namespace OHOS::Ace
