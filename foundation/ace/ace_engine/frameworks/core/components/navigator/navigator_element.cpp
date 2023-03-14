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

#include "core/components/navigator/navigator_element.h"

#include "core/components/navigator/render_navigator.h"
#include "core/components/stage/stage_element.h"

namespace OHOS::Ace {

void NavigatorElement::PerformBuild()
{
    SoleChildElement::PerformBuild();

    auto parent = GetParent().Upgrade();
    while (parent) {
        auto stage = AceType::DynamicCast<SectionStageElement>(parent);
        if (stage) {
            SetTargetContainer(stage);
            return;
        }
        parent = parent->GetParent().Upgrade();
    }
}

void NavigatorElement::OnClick()
{
    auto renderNode = AceType::DynamicCast<RenderNavigator>(renderNode_);
    renderNode->NavigatePage();
}

void NavigatorElement::SetTargetContainer(const WeakPtr<StageElement>& targetContainer)
{
    auto renderNode = AceType::DynamicCast<RenderNavigator>(renderNode_);
    if (renderNode) {
        renderNode->SetTargetContainer(targetContainer);
    }
}

} // namespace OHOS::Ace
