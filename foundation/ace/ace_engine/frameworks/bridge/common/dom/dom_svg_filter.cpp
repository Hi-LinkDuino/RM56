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

#include "frameworks/bridge/common/dom/dom_svg_filter.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgFilter::DOMSvgFilter(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName) {}

RefPtr<Component> DOMSvgFilter::GetSpecializedComponent()
{
    return filterComponent_;
}

void DOMSvgFilter::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (filterComponent_) {
        filterComponent_->InsertChild(slot, child->GetSpecializedComponent());
    }
}

void DOMSvgFilter::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto declaration = AceType::DynamicCast<SvgFilterDeclaration>(declaration_);
    if (!declaration) {
        LOGE("declaration is null");
        return;
    }
    // inherit presentation attributes
    auto svgNode = AceType::DynamicCast<DOMSvgBase>(parentNode);
    if (svgNode) {
        declaration->Inherit(svgNode->GetDeclaration());
    }
}

void DOMSvgFilter::PrepareSpecializedComponent()
{
    if (!filterComponent_) {
        filterComponent_ = AceType::MakeRefPtr<SvgFilterComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgFilterDeclaration>(declaration_);
    if (declaration) {
        filterComponent_->SetDeclaration(declaration);
    }
}

} // namespace OHOS::Ace::Framework
