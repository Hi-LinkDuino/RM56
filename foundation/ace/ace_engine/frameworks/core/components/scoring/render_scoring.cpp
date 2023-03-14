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

#include "core/components/scoring/render_scoring.h"

#include "base/log/ace_scoring_log.h"
#include "core/components/scoring/scoring_component.h"

namespace OHOS::Ace {


RefPtr<RenderNode> RenderScoring::Create()
{
    return AceType::MakeRefPtr<RenderScoring>();
}

void RenderScoring::Update(const RefPtr<Component>& component)
{
    auto scoring = AceType::DynamicCast<ScoringComponent>(component);
    if (!scoring) {
        return;
    }

    name_ = scoring->GetName();
    pageName_ = scoring->GetPageName();
}

void RenderScoring::PerformLayout()
{
    ACE_SCORING_COMPONENT(pageName_, name_, "Layout");
    RenderProxy::PerformLayout();
}

void RenderScoring::Paint(RenderContext& context, const Offset& offset)
{
    ACE_SCORING_COMPONENT(pageName_, name_, "Render");
    RenderProxy::Paint(context, offset);
}

} // namespace OHOS::Ace
