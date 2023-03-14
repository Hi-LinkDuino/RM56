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

#include "frameworks/core/components/svg/svg_polygon_component.h"

#include "frameworks/core/components/svg/render_svg_polygon.h"
#include "frameworks/core/components/svg/svg_polygon_element.h"

namespace OHOS::Ace {

SvgPolygonComponent::SvgPolygonComponent()
{
    InitDeclaration();
}

SvgPolygonComponent::SvgPolygonComponent(bool isClose) : isClose_(isClose)
{
    InitDeclaration();
}

SvgPolygonComponent::SvgPolygonComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgPolygonComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgPolygonDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgPolygonComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgPolygonElement>();
}

RefPtr<RenderNode> SvgPolygonComponent::CreateRenderNode()
{
    return RenderSvgPolygon::Create();
}

const std::string& SvgPolygonComponent::GetPoints() const
{
    return declaration_->GetPoints();
}

void SvgPolygonComponent::SetPoints(const std::string& points)
{
    declaration_->SetPoints(points);
}

void SvgPolygonComponent::SetDeclaration(const RefPtr<SvgPolygonDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
