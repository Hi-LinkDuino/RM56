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

#include "frameworks/core/components/svg/parse/svg_rect.h"

#include "frameworks/core/components/svg/render_svg_rect.h"

namespace OHOS::Ace {

SvgRect::SvgRect() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgRectComponent>();
}

RefPtr<SvgNode> SvgRect::Create()
{
    return AceType::MakeRefPtr<SvgRect>();
}

void SvgRect::SetAttr(const std::string& name, const std::string& value)
{
    component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value));
}

void SvgRect::AppendChild(const RefPtr<SvgNode>& child)
{
    component_->AppendChild(child->GetComponent());
}

RefPtr<RenderNode> SvgRect::CreateRender(
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
    auto renderNode = AceType::DynamicCast<RenderSvgRect>(component_->CreateRenderNode());
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

SkPath SvgRect::AsPath(const Size& viewPort) const
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
    SkRRect roundRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(ConvertDimensionToPx(component_->GetX(), viewPort, SvgLengthType::HORIZONTAL),
            ConvertDimensionToPx(component_->GetY(), viewPort, SvgLengthType::VERTICAL),
            ConvertDimensionToPx(component_->GetWidth(), viewPort, SvgLengthType::HORIZONTAL),
            ConvertDimensionToPx(component_->GetHeight(), viewPort, SvgLengthType::VERTICAL)),
        rx, ry);
    SkPath path;
    path.addRRect(roundRect);
    return path;
}

} // namespace OHOS::Ace
