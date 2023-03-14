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

#include "frameworks/core/components/svg/render_svg_filter.h"

#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/components/svg/svg_filter_component.h"

namespace OHOS::Ace {

void RenderSvgFilter::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgFilterComponent> FilterComponent = AceType::DynamicCast<SvgFilterComponent>(component);
    if (!FilterComponent) {
        LOGW("svg filter component is null");
        return;
    }
    auto& declaration = FilterComponent->GetDeclaration();

    x_ = declaration->GetX();
    y_ = declaration->GetY();
    height_ = declaration->GetHeight();
    width_ = declaration->GetWidth();
    RenderSvgBase::SetPresentationAttrs(FilterComponent->GetDeclaration());
    PrepareAnimations(component);
    AddFilterToRoot(id_, AceType::Claim(this));

    MarkNeedLayout();
}

void RenderSvgFilter::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgFilterComponent> filterComponent = AceType::DynamicCast<SvgFilterComponent>(component);
    if (!filterComponent) {
        LOGW("svg filter component is null");
        return;
    }
    const auto& componentChildren = filterComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgFilter::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    double originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](
            double value) {
        auto filter = weak.Upgrade();
        if (!filter) {
            LOGE("svg filter is null");
            return;
        }
        bool ret = filter->SetProperty(attributeName, value);
        if (ret) {
            filter->OnNotifyRender();
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgFilter::SetProperty(const std::string& attrName, double value)
{
    if (attrName == ATTR_NAME_X) {
        x_.SetValue(value);
    } else if (attrName == ATTR_NAME_Y) {
        y_.SetValue(value);
    } else if (attrName == ATTR_NAME_WIDTH) {
        width_.SetValue(value);
    } else if (attrName == ATTR_NAME_HEIGHT) {
        height_.SetValue(value);
    } else {
        return false;
    }
    return true;
}

bool RenderSvgFilter::GetProperty(const std::string& attrName, double& value) const
{
    if (attrName == ATTR_NAME_X) {
        value = x_.Value();
    } else if (attrName == ATTR_NAME_Y) {
        value = y_.Value();
    } else if (attrName == ATTR_NAME_WIDTH) {
        value = width_.Value();
    } else if (attrName == ATTR_NAME_HEIGHT) {
        value = height_.Value();
    } else {
        return false;
    }
    return true;
}

void RenderSvgFilter::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
}

} // namespace OHOS::Ace
