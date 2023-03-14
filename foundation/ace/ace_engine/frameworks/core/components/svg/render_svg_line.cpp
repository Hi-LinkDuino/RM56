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

#include "frameworks/core/components/svg/render_svg_line.h"

#include "frameworks/core/components/svg/svg_line_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_X1[] = "x1";
const char ATTR_NAME_X2[] = "x2";
const char ATTR_NAME_Y1[] = "y1";
const char ATTR_NAME_Y2[] = "y2";

} // namespace

void RenderSvgLine::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgLineComponent> lineComponent = AceType::DynamicCast<SvgLineComponent>(component);
    if (!lineComponent) {
        LOGW("line component is null");
        return;
    }
    x1_ = lineComponent->GetX1();
    x2_ = lineComponent->GetX2();
    y1_ = lineComponent->GetY1();
    y2_ = lineComponent->GetY2();
    RenderSvgBase::SetPresentationAttrs(component, lineComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgLine::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

void RenderSvgLine::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgLineComponent> lineComponent = AceType::DynamicCast<SvgLineComponent>(component);
    if (!lineComponent) {
        LOGW("line component is null");
        return;
    }
    const auto& componentChildren = lineComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgLine::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weakLine = AceType::WeakClaim(this), attrName = svgAnimate->GetAttributeName()](
        Dimension value) {
        auto line = weakLine.Upgrade();
        if (!line) {
            LOGE("line is null");
            return;
        }
        if (line->SetProperty(attrName, value)) {
            line->MarkNeedRender(true);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgLine::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_X1) {
        x1_ = value;
    } else if (attrName == ATTR_NAME_X2) {
        x2_ = value;
    } else if (attrName == ATTR_NAME_Y1) {
        y1_ = value;
    } else if (attrName == ATTR_NAME_Y2) {
        y2_ = value;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

bool RenderSvgLine::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_X1) {
        dimension = x1_;
    } else if (attrName == ATTR_NAME_X2) {
        dimension = x2_;
    } else if (attrName == ATTR_NAME_Y1) {
        dimension = y1_;
    } else if (attrName == ATTR_NAME_Y2) {
        dimension = y2_;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

} // namespace OHOS::Ace
