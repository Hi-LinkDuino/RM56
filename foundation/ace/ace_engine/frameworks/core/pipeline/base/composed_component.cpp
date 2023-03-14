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

#include "core/pipeline/base/composed_component.h"

#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

ComposedComponent::ComposedComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
    : BaseComposedComponent(id, name), SingleChild(child) {}

RefPtr<Element> ComposedComponent::CreateElement()
{
    auto element = AceType::MakeRefPtr<ComposedElement>(GetId());

    // elementFunction_ is true only for userdefined custom views
    // in other case use default path in performBuild
    if (elementFunction_) {
        elementFunction_(element);
    }

    return element;
}

void ComposedComponent::SetElementFunction(ElementFunction&& func)
{
    elementFunction_ = std::move(func);
}

void ComposedComponent::CallElementFunction(const RefPtr<Element>& element)
{
    elementFunction_(AceType::DynamicCast<ComposedElement>(element));
}

} // namespace OHOS::Ace
