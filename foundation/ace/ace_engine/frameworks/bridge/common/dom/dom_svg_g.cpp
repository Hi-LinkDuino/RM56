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

#include "frameworks/bridge/common/dom/dom_svg_g.h"
#include "frameworks/bridge/common/dom/dom_svg_animate.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSvgG::DOMSvgG(NodeId nodeId, const std::string& nodeName) : DOMSvgBase(nodeId, nodeName)
{
    transformComponent_ = AceType::MakeRefPtr<TransformComponent>();
    if (declaration_) {
        declaration_->SetHasDisplayStyle(true);
        auto& overflowStyle = declaration_->MaybeResetStyle<CommonOverflowStyle>(StyleTag::COMMON_OVERFLOW_STYLE);
        if (overflowStyle.IsValid()) {
            overflowStyle.overflow = Overflow::CLIP;
        }
    }
}

RefPtr<Component> DOMSvgG::GetSpecializedComponent()
{
    return gComponent_;
}

void DOMSvgG::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        LOGE("child is null");
        return;
    }
    if (AceType::InstanceOf<SvgAnimateComponent>(child->GetSpecializedComponent())) {
        gComponent_->InsertChild(slot, child->GetSpecializedComponent());
    } else {
        gComponent_->InsertChild(slot, child->GetRootComponent());
    }
}

void DOMSvgG::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    auto declaration = AceType::DynamicCast<SvgDeclaration>(declaration_);
    if (!declaration) {
        LOGE("declaration is null");
        return;
    }
    declaration->Inherit(parentNode->GetDeclaration());
    auto box = GetBoxComponent();
    if (box) {
        box->SetOverflow(Overflow::FORCE_CLIP);
    }
}

void DOMSvgG::PrepareSpecializedComponent()
{
    if (!gComponent_) {
        gComponent_ = AceType::MakeRefPtr<SvgGComponent>();
    }
    auto declaration = AceType::DynamicCast<SvgDeclaration>(declaration_);
    if (declaration) {
        gComponent_->SetDeclaration(declaration);
    }
}

} // namespace OHOS::Ace::Framework
