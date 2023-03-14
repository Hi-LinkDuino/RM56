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

#include "frameworks/bridge/common/dom/dom_svg_text_path.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgTextPath::DOMSvgTextPath(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName) {}

RefPtr<Component> DOMSvgTextPath::GetSpecializedComponent()
{
    return textPathComponent_;
}

void DOMSvgTextPath::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    textPathComponent_->InsertChild(slot, child->GetSpecializedComponent());
}

void DOMSvgTextPath::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto declaration = AceType::DynamicCast<SvgTextPathDeclaration>(declaration_);
    if (!declaration) {
        LOGE("declaration is null");
        return;
    }
    auto svgNode = AceType::DynamicCast<DOMSvgBase>(parentNode);
    if (svgNode) {
        declaration->Inherit(svgNode->GetDeclaration());
    }
}

void DOMSvgTextPath::PrepareSpecializedComponent()
{
    if (!textPathComponent_) {
        textPathComponent_ = AceType::MakeRefPtr<SvgTextPathComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgTextPathDeclaration>(declaration_);
    if (!declaration) {
        return;
    }
    textPathComponent_->SetDeclaration(declaration);
}

} // namespace OHOS::Ace::Framework
