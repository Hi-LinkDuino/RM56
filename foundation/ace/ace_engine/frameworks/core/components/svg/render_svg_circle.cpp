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

#include "frameworks/core/components/svg/render_svg_circle.h"

#include "frameworks/core/components/svg/svg_circle_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_CX[] = "cx";
const char ATTR_NAME_CY[] = "cy";
const char ATTR_NAME_R[] = "r";

} // namespace

void RenderSvgCircle::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgCircleComponent> circleComponent = AceType::DynamicCast<SvgCircleComponent>(component);
    if (!circleComponent) {
        LOGW("circle component is null");
        return;
    }
    cx_ = circleComponent->GetCx();
    cy_ = circleComponent->GetCy();
    r_ = circleComponent->GetR();
    RenderSvgBase::SetPresentationAttrs(circleComponent, circleComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgCircle::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgCircleComponent> circleComponent = AceType::DynamicCast<SvgCircleComponent>(component);
    if (!circleComponent) {
        LOGW("circle component is null");
        return;
    }
    const auto& componentChildren = circleComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgCircle::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weak = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](
            Dimension value) {
        auto circle = weak.Upgrade();
        if (!circle) {
            LOGE("circle is null");
            return;
        }
        if (circle->SetProperty(attributeName, value)) {
            circle->MarkNeedRender(true);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgCircle::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_CX) {
        cx_ = value;
    } else if (attrName == ATTR_NAME_CY) {
        cy_ = value;
    } else if (attrName == ATTR_NAME_R) {
        r_ = value;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

bool RenderSvgCircle::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_CX) {
        dimension = cx_;
    } else if (attrName == ATTR_NAME_CY) {
        dimension = cy_;
    } else if (attrName == ATTR_NAME_R) {
        dimension = r_;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

void RenderSvgCircle::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

} // namespace OHOS::Ace
