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

#include "core/components/coverage/render_coverage.h"

#include "base/log/log.h"
#include "core/components/common/properties/alignment.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderCoverage::Create()
{
    return AceType::MakeRefPtr<RenderCoverage>();
}

RenderCoverage::RenderCoverage() {}

void RenderCoverage::Update(const RefPtr<Component>& component)
{
    auto coverageComponent = AceType::DynamicCast<CoverageComponent>(component);
    title_ = coverageComponent->GetTextVal();
    x_ = coverageComponent->GetX();
    y_ = coverageComponent->GetY();
    align_ = coverageComponent->GetAlignment();
}

void RenderCoverage::PerformLayout()
{
    const std::list<RefPtr<RenderNode>>& children = GetChildren();
    LayoutParam innerLayout = GetLayoutParam();
    auto firstChild = children.begin();
    if (firstChild == children.end()) {
        LOGD("RenderCoverage: firstChild is null.");
        return;
    }
    (*firstChild)->Layout(innerLayout);
    Size size = (*firstChild)->GetLayoutSize();
    SetLayoutSize(size);

    auto secondChild = ++firstChild;
    if (secondChild == children.end()) {
        LOGD("RenderCoverage: secondChild is null.");
        return;
    }
    LayoutParam secondChildLayoutParam;
    secondChildLayoutParam.SetMaxSize(size);
    (*secondChild)->Layout(secondChildLayoutParam);
}

} // namespace OHOS::Ace
