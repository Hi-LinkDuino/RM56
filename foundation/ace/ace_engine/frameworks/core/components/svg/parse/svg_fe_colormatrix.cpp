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

#include "frameworks/core/components/svg/parse/svg_fe_colormatrix.h"

#include "frameworks/core/components/svg/render_svg_fe_colormatrix.h"

namespace OHOS::Ace {

RefPtr<SvgNode> SvgFeColorMatrix::Create()
{
    return AceType::MakeRefPtr<SvgFeColorMatrix>();
}

SvgFeColorMatrix::SvgFeColorMatrix() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgFeColorMatrixComponent>();
}

void SvgFeColorMatrix::SetAttr(const std::string& name, const std::string& value)
{
    if (!component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value))) {
        component_->GetDeclaration()->SetAttr({ std::make_pair(name, value) });
    }
}

void SvgFeColorMatrix::AppendChild(const RefPtr<SvgNode>& child)
{
    component_->AppendChild(child->GetComponent());
}

RefPtr<RenderNode> SvgFeColorMatrix::CreateRender(
    const LayoutParam& layoutParam, const RefPtr<SvgBaseDeclaration>& parent, bool useBox)
{
    component_->Inherit(parent);
    auto renderNode = AceType::DynamicCast<RenderSvgFeColorMatrix>(component_->CreateRenderNode());
    if (!renderNode) {
        LOGE("create renderNode failed");
        return nullptr;
    }
    renderNode->Attach(context_);
    renderNode->Update(component_);
    return renderNode;
}

} // namespace OHOS::Ace
