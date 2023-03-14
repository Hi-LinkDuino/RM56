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

#include "frameworks/core/components/svg/parse/svg_polygon.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/svg/render_svg_polygon.h"

namespace OHOS::Ace {

SvgPolygon::SvgPolygon(bool isClose) : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgPolygonComponent>(isClose);
}

RefPtr<SvgNode> SvgPolygon::CreatePolygon()
{
    return AceType::MakeRefPtr<SvgPolygon>(true);
}

RefPtr<SvgNode> SvgPolygon::CreatePolyline()
{
    return AceType::MakeRefPtr<SvgPolygon>(false);
}

void SvgPolygon::SetAttr(const std::string& name, const std::string& value)
{
    component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value));
}

void SvgPolygon::AppendChild(const RefPtr<SvgNode>& child)
{
    component_->AppendChild(child->GetComponent());
}

RefPtr<RenderNode> SvgPolygon::CreateRender(
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
    auto renderNode = AceType::DynamicCast<RenderSvgPolygon>(component_->CreateRenderNode());
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

SkPath SvgPolygon::AsPath(const Size& viewPort) const
{
    if (component_->GetPoints().empty()) {
        return SkPath();
    }
    SkPath path;
    std::vector<SkPoint> skPoints;
    FlutterSvgPainter::StringToPoints(component_->GetPoints().c_str(), skPoints);
    if (skPoints.empty()) {
        return SkPath();
    }
    path.addPoly(&skPoints[0], skPoints.size(), component_->IsClose());
    if (component_->GetDeclaration()->GetClipState().IsEvenodd()) {
        path.setFillType(SkPath::FillType::kEvenOdd_FillType);
    }
    return path;
}

} // namespace OHOS::Ace
