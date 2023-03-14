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

#include "frameworks/core/components/svg/render_svg_ellipse.h"

#include "frameworks/core/components/svg/svg_ellipse_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_CX[] = "cx";
const char ATTR_CY[] = "cy";
const char ATTR_RX[] = "rx";
const char ATTR_RY[] = "ry";

} // namespace

void RenderSvgEllipse::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgEllipseComponent> ellipseComponent = AceType::DynamicCast<SvgEllipseComponent>(component);
    if (!ellipseComponent) {
        LOGW("ellipse component is null");
        return;
    }
    cx_ = ellipseComponent->GetCx();
    cy_ = ellipseComponent->GetCy();
    rx_ = ellipseComponent->GetRx();
    ry_ = ellipseComponent->GetRy();
    RenderSvgBase::SetPresentationAttrs(component, ellipseComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgEllipse::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgEllipseComponent> ellipseComponent = AceType::DynamicCast<SvgEllipseComponent>(component);
    if (!ellipseComponent) {
        LOGW("ellipse component is null");
        return;
    }
    const auto& componentChildren = ellipseComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgEllipse::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weak = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](
            Dimension value) {
        auto ellipse = weak.Upgrade();
        if (!ellipse) {
            LOGE("ellipse is null");
            return;
        }
        if (ellipse->SetProperty(attributeName, value)) {
            ellipse->MarkNeedRender(true);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgEllipse::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_CX) {
        cx_ = value;
    } else if (attrName == ATTR_CY) {
        cy_ = value;
    } else if (attrName == ATTR_RX) {
        rx_ = value;
    } else if (attrName == ATTR_RY) {
        ry_ = value;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

bool RenderSvgEllipse::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_CX) {
        dimension = cx_;
    } else if (attrName == ATTR_CY) {
        dimension = cy_;
    } else if (attrName == ATTR_RX) {
        dimension = rx_;
    } else if (attrName == ATTR_RY) {
        dimension = ry_;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

void RenderSvgEllipse::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

} // namespace OHOS::Ace
