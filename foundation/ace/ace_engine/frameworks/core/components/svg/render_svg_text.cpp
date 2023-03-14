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

#include "frameworks/core/components/svg/render_svg_text.h"

#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/components/svg/svg_text_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_DX[] = "dx";
const char ATTR_NAME_DY[] = "dy";
const char ATTR_NAME_ROTATE[] = "rotate";

} // namespace

void RenderSvgText::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgTextComponent> textComponent = AceType::DynamicCast<SvgTextComponent>(component);
    if (!textComponent) {
        LOGW("svg text component is null");
        return;
    }
    x_ = textComponent->GetX();
    y_ = textComponent->GetY();
    dx_ = textComponent->GetDx();
    dy_ = textComponent->GetDy();
    hasX_ = textComponent->GetHasX();
    hasY_ = textComponent->GetHasY();
    textData_ = textComponent->GetTextData();
    rotate_ = textComponent->GetRotate();
    RenderSvgBase::SetPresentationAttrs(textComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgText::PrepareAnimations(const RefPtr<Component> &component)
{
    const RefPtr<SvgTextComponent> textComponent = AceType::DynamicCast<SvgTextComponent>(component);
    if (!textComponent) {
        LOGW("svg text component is null");
        return;
    }
    const auto& componentChildren = textComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgText::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weakText = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](
            Dimension value) {
        auto text = weakText.Upgrade();
        if (!text) {
            LOGE("svg text is null");
            return;
        }
        bool ret = text->SetProperty(attributeName, value);
        if (ret) {
            text->OnNotifyRender();
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgText::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_X) {
        x_ = value;
        hasX_ = true;
    } else if (attrName == ATTR_NAME_Y) {
        y_ = value;
        hasY_ = true;
    } else if (attrName == ATTR_NAME_DX) {
        dx_ = value;
    } else if (attrName == ATTR_NAME_DY) {
        dy_ = value;
    } else if (attrName == ATTR_NAME_ROTATE) {
        rotate_ = value.Value();
    } else {
        return false;
    }
    return true;
}

bool RenderSvgText::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_X) {
        dimension = x_;
    } else if (attrName == ATTR_NAME_Y) {
        dimension = y_;
    } else if (attrName == ATTR_NAME_DX) {
        dimension = dx_;
    } else if (attrName == ATTR_NAME_DY) {
        dimension = dy_;
    } else if (attrName == ATTR_NAME_ROTATE) {
        dimension = Dimension(rotate_);
    } else {
        return false;
    }
    return true;
}

void RenderSvgText::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
}

} // namespace OHOS::Ace
