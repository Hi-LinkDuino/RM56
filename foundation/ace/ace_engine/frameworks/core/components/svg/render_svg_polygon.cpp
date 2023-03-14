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

#include "frameworks/core/components/svg/render_svg_polygon.h"

#include "frameworks/core/components/svg/svg_polygon_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_POLYGON_POINTS[] = "points";

} // namespace

void RenderSvgPolygon::Update(const RefPtr<Component> &component)
{
    auto polygonComponent = AceType::DynamicCast<SvgPolygonComponent>(component);
    if (!polygonComponent) {
        LOGW("polygon component is null");
        return;
    }
    points_ = polygonComponent->GetPoints();
    isClose_ = polygonComponent->IsClose();
    RenderSvgBase::SetPresentationAttrs(component, polygonComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgPolygon::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

void RenderSvgPolygon::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgPolygonComponent> polygonComponent = AceType::DynamicCast<SvgPolygonComponent>(component);
    if (!polygonComponent) {
        LOGW("polygon component is null");
        return;
    }
    const auto &componentChildren = polygonComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgPolygon::PrepareSelfAnimation(const RefPtr<SvgAnimate>& component)
{
    if (component->GetAttributeName() != ATTR_NAME_POLYGON_POINTS) {
        return false;
    }
    auto svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    component->Copy(svgAnimate);
    pointsVector_.clear();

    PrepareWeightAnimate(svgAnimate, pointsVector_, points_, isBy_);

    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this)](double value) {
        auto svgPath = weak.Upgrade();
        if (!svgPath) {
            LOGE("svgPolygon is null");
            return;
        }
        svgPath->SetWeight(value);
        svgPath->MarkNeedRender(true);
    };
    double originalValue = 0.0;
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

} // namespace OHOS::Ace
