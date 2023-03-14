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

#include "frameworks/core/components/svg/parse/svg_line.h"

#include "frameworks/core/components/svg/render_svg_line.h"

namespace OHOS::Ace {

SvgLine::SvgLine() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgLineComponent>();
}

RefPtr<SvgNode> SvgLine::Create()
{
    return AceType::MakeRefPtr<SvgLine>();
}

void SvgLine::SetAttr(const std::string& name, const std::string& value)
{
    component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value));
}

void SvgLine::AppendChild(const RefPtr<SvgNode>& child)
{
    component_->AppendChild(child->GetComponent());
}

RefPtr<RenderNode> SvgLine::CreateRender(
    const LayoutParam& layoutParam, const RefPtr<SvgBaseDeclaration>& parent, bool useBox)
{
    component_->Inherit(parent);
    auto href = component_->GetDeclaration()->GetFillState().GetHref();
    if (!href.empty()) {
        auto gradient = GetGradient(href);
        if (gradient) {
            component_->GetDeclaration()->SetGradient(gradient.value());
        }
    }
    auto renderNode = AceType::DynamicCast<RenderSvgLine>(component_->CreateRenderNode());
    if (!renderNode) {
        LOGE("create renderNode failed");
        return nullptr;
    }
    renderNode->Attach(context_);
    renderNode->Update(component_);
    if (!useBox) {
        return renderNode;
    }

    auto boxComponent = CreateBoxComponent(layoutParam, component_->GetDeclaration()->GetClipPathHref());
    auto renderBox = boxComponent->CreateRenderNode();
    renderBox->Attach(context_);
    renderBox->Update(boxComponent);
    renderBox->AddChild(renderNode);
    return renderBox;
}

SkPath SvgLine::AsPath(const Size& viewPort) const
{
    SkPath path;
    path.moveTo(ConvertDimensionToPx(component_->GetX1(), viewPort, SvgLengthType::HORIZONTAL),
        ConvertDimensionToPx(component_->GetY1(), viewPort, SvgLengthType::VERTICAL));
    path.lineTo(ConvertDimensionToPx(component_->GetX2(), viewPort, SvgLengthType::HORIZONTAL),
        ConvertDimensionToPx(component_->GetY2(), viewPort, SvgLengthType::VERTICAL));
    return path;
}

} // namespace OHOS::Ace
