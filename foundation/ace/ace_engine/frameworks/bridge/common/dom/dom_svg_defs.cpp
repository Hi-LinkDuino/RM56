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

#include "frameworks/bridge/common/dom/dom_svg_defs.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgDefs::DOMSvgDefs(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName) {}

RefPtr<Component> DOMSvgDefs::GetSpecializedComponent()
{
    return defsComponent_;
}

void DOMSvgDefs::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    defsComponent_->InsertChild(slot, child->GetSpecializedComponent());
}

void DOMSvgDefs::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    DOMSvgBase::InheritAttrs(parentNode);
}

void DOMSvgDefs::PrepareSpecializedComponent()
{
    if (!defsComponent_) {
        defsComponent_ = AceType::MakeRefPtr<SvgDefsComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgBaseDeclaration>(declaration_);
    if (declaration) {
        defsComponent_->SetDeclaration(declaration);
    }
}

} // namespace OHOS::Ace::Framework
