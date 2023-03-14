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

#include "frameworks/bridge/common/dom/dom_svg_text.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgText::DOMSvgText(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName) {}

RefPtr<Component> DOMSvgText::GetSpecializedComponent()
{
    return textComponent_;
}

void DOMSvgText::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    textComponent_->InsertChild(slot, child->GetSpecializedComponent());
}

void DOMSvgText::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto declaration = AceType::DynamicCast<SvgTextDeclaration>(declaration_);
    if (!declaration) {
        LOGE("declaration is null");
        return;
    }
    // inherit presentation attributes
    auto svgNode = AceType::DynamicCast<DOMSvgBase>(parentNode);
    if (svgNode) {
        declaration->Inherit(svgNode->GetDeclaration());
    }
    // inherit rotate attribute
    if (!declaration->HasRotate()) {
        auto svgTextNode = AceType::DynamicCast<DOMSvgText>(parentNode);
        if (svgTextNode) {
            declaration->SetRotate(svgTextNode->GetRotate());
        }
    }
}

void DOMSvgText::PrepareSpecializedComponent()
{
    if (!textComponent_) {
        textComponent_ = AceType::MakeRefPtr<SvgTextComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgTextDeclaration>(declaration_);
    if (declaration) {
        textComponent_->SetDeclaration(declaration);
    }
}

double DOMSvgText::GetRotate() const
{
    auto declaration = AceType::DynamicCast<SvgTextDeclaration>(declaration_);
    if (!declaration) {
        return 0.0;
    }
    return declaration->GetRotate();
}

} // namespace OHOS::Ace::Framework
