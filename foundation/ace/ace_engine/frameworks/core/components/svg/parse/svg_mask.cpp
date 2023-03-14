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

#include "frameworks/core/components/svg/parse/svg_mask.h"

#include "frameworks/core/components/svg/render_svg_mask.h"

namespace OHOS::Ace {

SvgMask::SvgMask() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgMaskComponent>();
}

RefPtr<SvgNode> SvgMask::Create()
{
    return AceType::MakeRefPtr<SvgMask>();
}

void SvgMask::SetAttr(const std::string& name, const std::string& value)
{
    if (!component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value))) {
        component_->GetDeclaration()->SetAttr({ std::make_pair(name, value) });
    }
}

void SvgMask::AppendChild(const RefPtr<SvgNode>& child)
{
    children_.emplace_back(child);
    component_->AppendChild(child->GetComponent());
}

void SvgMask::Update(RefPtr<RenderNode>& node)
{
    if (node) {
        node->Update(component_);
    }
}

RefPtr<RenderNode> SvgMask::CreateRender(
    const LayoutParam& layoutParam, const RefPtr<SvgBaseDeclaration>& parent, bool useBox)
{
    auto svgContext = svgContext_.Upgrade();
    if (svgContext == nullptr) {
        LOGW("svgContext is null.");
        return nullptr;
    }

    auto renderNode = AceType::DynamicCast<RenderSvgMask>(component_->CreateRenderNode());
    if (renderNode == nullptr) {
        return nullptr;
    }
    component_->Inherit(parent);
    renderNode->SetSvgRoot(svgContext->GetSvgRoot());
    renderNode->Attach(context_);

    for (auto child : children_) {
        auto childRender = child->CreateRender(layoutParam, component_->GetDeclaration(), useBox);
        if (childRender) {
            renderNode->AddChild(childRender, renderNode->GetChildren().size());
            child->Update(childRender);
        }
    }

    return renderNode;
}

} // namespace OHOS::Ace
