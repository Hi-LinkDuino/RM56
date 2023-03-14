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

#include "core/components/coverage/coverage_component.h"

#include "core/components/coverage/render_coverage.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension DEFAULT_FONT_SIZE = Dimension(40);

} // namespace

CoverageComponent::CoverageComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    std::list<RefPtr<Component>> childrenAlign;
    alignComponent_ = AceType::MakeRefPtr<AlignComponent>(childrenAlign, Alignment::CENTER);
    textBoxComponent_ = AceType::MakeRefPtr<OHOS::Ace::BoxComponent>();
    textComponent_ = AceType::MakeRefPtr<OHOS::Ace::TextComponentV2>("");
}

RefPtr<RenderNode> CoverageComponent::CreateRenderNode()
{
    return RenderCoverage::Create();
}

void CoverageComponent::Initialization()
{
    auto textStyle = textComponent_->GetTextStyle();
    textStyle.SetFontSize(DEFAULT_FONT_SIZE);
    textComponent_->SetTextStyle(textStyle);
    textBoxComponent_->SetChild(textComponent_);
    alignComponent_->AppendChild(textBoxComponent_);
    InsertChild(1, alignComponent_);
    Component::MergeRSNode(alignComponent_, textComponent_);
}

} // namespace OHOS::Ace
