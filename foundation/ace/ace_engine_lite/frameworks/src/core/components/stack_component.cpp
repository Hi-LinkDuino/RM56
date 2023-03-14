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

#include "stack_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
StackComponent::StackComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager)
    : Component(options, children, styleManager)
{
    SetComponentName(K_STACK);
    nativeView_.SetStyle(STYLE_BACKGROUND_OPA, 0);
}

UIView *StackComponent::GetComponentRootView() const
{
    return const_cast<UIViewGroup *>(&nativeView_);
}

bool StackComponent::ProcessChildren()
{
    // add all children to this container
    AppendChildren(this);
    return true;
}

void StackComponent::PostUpdate(uint16_t attrKeyId)
{
    nativeView_.LayoutChildren();
}

void StackComponent::AttachView(const Component *child)
{
    if (child == nullptr) {
        return;
    }
    nativeView_.Add(child->GetComponentRootView());
}
} // namespace ACELite
} // namespace OHOS
