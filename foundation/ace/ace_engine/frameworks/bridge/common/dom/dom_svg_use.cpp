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

#include "frameworks/bridge/common/dom/dom_svg_use.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgUse::DOMSvgUse(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName) {}


RefPtr<Component> DOMSvgUse::GetSpecializedComponent()
{
    return useComponent_;
}

void DOMSvgUse::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    useComponent_->InsertChild(slot, child->GetSpecializedComponent());
}

void DOMSvgUse::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    DOMSvgBase::InheritAttrs(parentNode);
}

void DOMSvgUse::PrepareSpecializedComponent()
{
    if (!useComponent_) {
        useComponent_ = AceType::MakeRefPtr<SvgUseComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgDeclaration>(declaration_);
    if (declaration) {
        useComponent_->SetDeclaration(declaration);
    }
}

} // namespace OHOS::Ace::Framework
