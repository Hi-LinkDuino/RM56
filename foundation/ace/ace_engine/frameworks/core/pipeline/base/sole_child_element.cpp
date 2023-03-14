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

#include "core/pipeline/base/sole_child_element.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

RefPtr<Element> SoleChildElement::Create()
{
    return AceType::MakeRefPtr<SoleChildElement>();
}

void SoleChildElement::PerformBuild()
{
    RefPtr<SoleChildComponent> component = AceType::DynamicCast<SoleChildComponent>(component_);
    if (!component) {
        LOGW("Should be sole child component, but %s", AceType::TypeName(component_));
        return;
    }
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, component->GetChild());
}

} // namespace OHOS::Ace
