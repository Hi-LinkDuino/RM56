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

#include "core/components/scoring/scoring_element.h"

#include "base/log/ace_scoring_log.h"
#include "core/components/page/page_element.h"
#include "core/components/scoring/render_scoring.h"
#include "core/components/scoring/scoring_component.h"

namespace OHOS::Ace {

void ScoringElement::PerformBuild()
{
    RefPtr<ScoringComponent> component = AceType::DynamicCast<ScoringComponent>(component_);
    std::string pageName = component->GetPageName();
    if (pageName.empty()) {
        auto parent = GetElementParent().Upgrade();
        while (parent) {
            auto pageElement = AceType::DynamicCast<PageElement>(parent);
            if (pageElement) {
                pageName = pageElement->GetPageUrl();
                auto render = AceType::DynamicCast<RenderScoring>(GetRenderNode());
                if (render) {
                    render->SetPageName(pageName);
                }
                break;
            }
            parent = parent->GetElementParent().Upgrade();
        }
    }
    ACE_SCORING_COMPONENT(pageName, component->GetName(), "Build");
    SoleChildElement::PerformBuild();
}

} // namespace OHOS::Ace