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

#include "frameworks/bridge/common/dom/dom_swiper.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr double MAX_OPACITY = 255.0;
const std::string DISPLAY_COMPOSED_NAME = "SwiperDisplayChild";

std::string GetDisplayComposedId(const RefPtr<DOMNode>& child)
{
    return "display" + std::to_string(child->GetNodeId());
}

} // namespace

DOMSwiper::DOMSwiper(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    swiperChild_ = AceType::MakeRefPtr<SwiperComponent>(std::list<RefPtr<Component>>());
}

void DOMSwiper::InitializeStyle()
{
    if (declaration_) {
        declaration_->InitializeStyle();
    }
}

void DOMSwiper::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    auto display = AceType::MakeRefPtr<DisplayComponent>(child->GetRootComponent());
    display->SetOpacity(MAX_OPACITY);
    swiperChild_->InsertChild(
        slot, AceType::MakeRefPtr<ComposedComponent>(GetDisplayComposedId(child), DISPLAY_COMPOSED_NAME, display));
}

void DOMSwiper::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    ACE_DCHECK(child);
    swiperChild_->RemoveChildByComposedId(GetDisplayComposedId(child));
}

void DOMSwiper::PrepareSpecializedComponent()
{
    swiperChild_->SetShow(GetDisplay() == DisplayType::NO_SETTING || GetDisplay() == DisplayType::FLEX);
    swiperChild_->SetDeclaration(AceType::DynamicCast<SwiperDeclaration>(declaration_));
    swiperChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    for (const auto& item : swiperChild_->GetChildren()) {
        auto composedDisplay = AceType::DynamicCast<ComposedComponent>(item);
        if (composedDisplay) {
            composedDisplay->MarkNeedUpdate();
        }
    }
}

void DOMSwiper::AdjustSpecialParamInLiteMode()
{
    auto declaration = AceType::DynamicCast<SwiperDeclaration>(declaration_);
    if (declaration) {
        declaration->SetShowIndicator(false);
    }
}

} // namespace OHOS::Ace::Framework
