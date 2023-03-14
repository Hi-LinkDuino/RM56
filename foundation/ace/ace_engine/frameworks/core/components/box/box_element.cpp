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

#include "core/components/page/page_element.h"
#include "core/components/box/box_element.h"

namespace OHOS::Ace {

void BoxElement::PerformBuild()
{
    SoleChildElement::PerformBuild();
    const RefPtr<Component> component = Element::GetComponent();
    const RefPtr<BoxComponent> boxComponent = AceType::DynamicCast<BoxComponent>(component);
    if (!boxComponent) {
        return;
    }
    std::string id = boxComponent->GetGeometryTransitionId();
    geometryTransitionId_ = id;
    if (geometryTransitionId_.empty()) {
        return;
    }
    const RefPtr<PageElement> pageElement = GetPageElement();
    WeakPtr<BoxElement> boxElementWeak = AceType::WeakClaim(this);
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    AnimationOption option = context->GetExplicitAnimationOption();
    if (pageElement) {
        pageElement->AddGeometryTransition(id, boxElementWeak, option);
    }
}

} // namespace OHOS::Ace