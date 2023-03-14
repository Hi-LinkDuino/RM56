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

#include "frameworks/core/components/svg/parse/svg_ellipse.h"

#include "frameworks/core/components/box/box_component.h"
#include "frameworks/core/components/svg/render_svg_ellipse.h"

namespace OHOS::Ace {

SvgEllipse::SvgEllipse() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgEllipseComponent>();
}

RefPtr<SvgNode> SvgEllipse::Create()
{
    return AceType::MakeRefPtr<SvgEllipse>();
}

void SvgEllipse::SetAttr(const std::string& name, const std::string& value)
{
    component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value));
}

void SvgEllipse::AppendChild(const RefPtr<SvgNode>& child)
{
    children_.emplace_back(child);
    component_->AppendChild(child->GetComponent());
}

RefPtr<RenderNode> SvgEllipse::CreateRender(
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
    auto renderNode = AceType::DynamicCast<RenderSvgEllipse>(component_->CreateRenderNode());
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

SkPath SvgEllipse::AsPath(const Size& viewPort) const
{
    double rx = 0.0;
    if (GreatOrEqual(component_->GetRx().Value(), 0.0)) {
        rx = ConvertDimensionToPx(component_->GetRx(), viewPort, SvgLengthType::HORIZONTAL);
    } else {
        if (GreatNotEqual(component_->GetRy().Value(), 0.0)) {
            rx = ConvertDimensionToPx(component_->GetRy(), viewPort, SvgLengthType::VERTICAL);
        }
    }
    double ry = 0.0;
    if (GreatOrEqual(component_->GetRy().Value(), 0.0)) {
        ry = ConvertDimensionToPx(component_->GetRy(), viewPort, SvgLengthType::VERTICAL);
    } else {
        if (GreatNotEqual(component_->GetRx().Value(), 0.0)) {
            ry = ConvertDimensionToPx(component_->GetRx(), viewPort, SvgLengthType::HORIZONTAL);
        }
    }
    SkPath path;
    SkRect rect = SkRect::MakeXYWH(ConvertDimensionToPx(component_->GetCx(), viewPort, SvgLengthType::HORIZONTAL) - rx,
        ConvertDimensionToPx(component_->GetCy(), viewPort, SvgLengthType::VERTICAL) - ry, rx + rx, ry + ry);
    path.addOval(rect);
    return path;
}

} // namespace OHOS::Ace
