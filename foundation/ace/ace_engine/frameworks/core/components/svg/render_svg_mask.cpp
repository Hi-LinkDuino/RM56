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

#include "frameworks/core/components/svg/render_svg_mask.h"

#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/components/svg/render_svg.h"
#include "frameworks/core/components/svg/svg_mask_component.h"

namespace OHOS::Ace {

void RenderSvgMask::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgMaskComponent> maskComponent = AceType::DynamicCast<SvgMaskComponent>(component);
    if (!maskComponent) {
        LOGW("svg mask component is null");
        return;
    }
    auto& declaration = maskComponent->GetDeclaration();

    x_ = declaration->GetX();
    y_ = declaration->GetY();
    height_ = declaration->GetHeight();
    width_ = declaration->GetWidth();
    maskUnits_ = declaration->GetMaskUnits();
    maskContentUnits_ = declaration->GetMaskContentUnits();
    RenderSvgBase::SetPresentationAttrs(maskComponent->GetDeclaration());
    PrepareAnimations(component);
    AddMaskToRoot(id_, AceType::Claim(this));

    MarkNeedLayout();
}

double RenderSvgMask::ParseUnitsAttr(const Dimension& attr, double value)
{
    if (IsDefalutMaskUnits()) {
        // only support decimal or percent
        if (attr.Unit() == DimensionUnit::PERCENT) {
            return value * attr.Value();
        }
        return attr.Value() * value;
    }
    // percent and px
    if (attr.Unit() == DimensionUnit::PERCENT) {
        return value * attr.Value();
    }
    return attr.Value();
}

void RenderSvgMask::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgMaskComponent> maskComponent = AceType::DynamicCast<SvgMaskComponent>(component);
    if (!maskComponent) {
        LOGW("svg mask component is null");
        return;
    }
    const auto& componentChildren = maskComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgMask::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    double originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](
            double value) {
        auto mask = weak.Upgrade();
        if (!mask) {
            LOGE("svg mask is null");
            return;
        }
        bool ret = mask->SetProperty(attributeName, value);
        if (ret) {
            mask->OnNotifyRender();
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgMask::SetProperty(const std::string& attrName, double value)
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

bool RenderSvgMask::GetProperty(const std::string& attrName, double& value) const
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

void RenderSvgMask::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
}

} // namespace OHOS::Ace
