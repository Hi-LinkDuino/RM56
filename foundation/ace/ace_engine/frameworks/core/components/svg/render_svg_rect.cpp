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

#include "frameworks/core/components/svg/render_svg_rect.h"

#include "frameworks/core/components/svg/svg_rect_component.h"

namespace OHOS::Ace {

void RenderSvgRect::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgRectComponent> rectComponent = AceType::DynamicCast<SvgRectComponent>(component);
    if (!rectComponent) {
        LOGW("rect component is null");
        return;
    }
    x_ = rectComponent->GetX();
    y_ = rectComponent->GetY();
    rx_ = rectComponent->GetRx();
    ry_ = rectComponent->GetRy();
    width_ = rectComponent->GetWidth();
    height_ = rectComponent->GetHeight();
    RenderSvgBase::SetPresentationAttrs(component, rectComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgRect::PrepareAnimations(const RefPtr<Component>& component)
{
    const RefPtr<SvgRectComponent> rectComponent = AceType::DynamicCast<SvgRectComponent>(component);
    if (!rectComponent) {
        LOGW("rect component is null");
        return;
    }
    const auto& componentChildren = rectComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgRect::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weakRect = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](Dimension value) {
        auto rect = weakRect.Upgrade();
        if (!rect) {
            LOGE("rect is null");
            return;
        }
        bool ret = rect->SetProperty(attributeName, value);
        if (ret) {
            rect->MarkNeedRender(true);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgRect::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_WIDTH) {
        width_ = value;
    } else if (attrName == ATTR_NAME_HEIGHT) {
        height_ = value;
    } else if (attrName == ATTR_NAME_X) {
        x_ = value;
    } else if (attrName == ATTR_NAME_Y) {
        y_ = value;
    } else if (attrName == ATTR_NAME_RX) {
        rx_ = value;
    } else if (attrName == ATTR_NAME_RY) {
        ry_ = value;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

bool RenderSvgRect::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_WIDTH) {
        dimension = width_;
    } else if (attrName == ATTR_NAME_HEIGHT) {
        dimension = height_;
    } else if (attrName == ATTR_NAME_X) {
        dimension = x_;
    } else if (attrName == ATTR_NAME_Y) {
        dimension = y_;
    } else if (attrName == ATTR_NAME_RX) {
        dimension = rx_;
    } else if (attrName == ATTR_NAME_RY) {
        dimension = ry_;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

void RenderSvgRect::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

} // namespace OHOS::Ace
