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

#include "frameworks/core/components/svg/render_svg.h"

#include "frameworks/core/components/display/render_display.h"
#include "frameworks/core/components/svg/svg_component.h"
#include "frameworks/core/components/transform/render_transform.h"

namespace OHOS::Ace {

namespace {

const char TRANSFORM_ROTATE[] = "rotate";
const char TRANSFORM_SCALE[] = "scale";
const char TRANSFORM_SKEW[] = "skew";
const char TRANSFORM_TRANSLATE[] = "translate";

} // namespace

RenderSvg::~RenderSvg()
{
    svgAnimates_.clear();
}

void RenderSvg::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgComponent> svgComponent = AceType::DynamicCast<SvgComponent>(component);
    if (!svgComponent) {
        LOGW("svg component is null");
        return;
    }
    isSvgNode_ = true;
    x_ = svgComponent->GetX();
    y_ = svgComponent->GetY();
    width_ = svgComponent->GetWidth();
    height_ = svgComponent->GetHeight();
    viewBox_ = svgComponent->GetViewBox();
    isRoot_ = svgComponent->IsRoot();
    autoMirror_ = svgComponent->GetAutoMirror();
    RenderSvgBase::SetPresentationAttrs(svgComponent->GetDeclaration());
    AddSvgAnimations(svgComponent);
    MarkNeedLayout();
}

void RenderSvg::AddSvgAnimations(const RefPtr<SvgComponent>& svgComponent)
{
    if (!svgComponent) {
        LOGW("svg component is null");
        return;
    }
    svgAnimates_.clear();
    hasUpdated_ = true;
    const auto& componentChildren = svgComponent->GetChildren();
    for (const auto& childComponent : componentChildren) {
        auto svgAnimateComponent = AceType::DynamicCast<SvgAnimate>(childComponent);
        if (!svgAnimateComponent || svgAnimateComponent->GetSvgAnimateType() == SvgAnimateType::MOTION) {
            continue;
        }
        auto svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
        svgAnimateComponent->Copy(svgAnimate);
        svgAnimates_.emplace_back(svgAnimate);
    }
}

void RenderSvg::PrepareAnimations()
{
    if (!hasUpdated_) {
        return;
    }
    hasUpdated_ = false;
    for (const auto& svgAnimate : svgAnimates_) {
        RenderSvgBase::PreparePropertyAnimation(svgAnimate);
    }
}

bool RenderSvg::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    if (OpacityAnimation(svgAnimate)) {
        return true;
    }
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weak = AceType::WeakClaim(this), attrName = svgAnimate->GetAttributeName()](Dimension value) {
        auto svg = weak.Upgrade();
        if (!svg) {
            LOGE("svg is null");
            return;
        }
        bool ret = svg->SetProperty(attrName, value);
        if (ret) {
            svg->MarkNeedLayout(true);
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvg::OpacityAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    if (svgAnimate->GetAttributeName() != ATTR_NAME_OPACITY) {
        return false;
    }
    SetOpacityCallback();
    if (opacityCallback_) {
        double originalValue = opacity_ * (1.0 / UINT8_MAX);
        CreatePropertyAnimation(svgAnimate, originalValue, std::move(opacityCallback_));
    }
    return true;
}

void RenderSvg::SetOpacityCallback()
{
    int32_t nodeId = GetNodeId();
    auto parent = GetParent().Upgrade();
    while (parent) {
        if (parent->GetNodeId() != nodeId) {
            break;
        }
        auto displayRender = AceType::DynamicCast<RenderDisplay>(parent);
        if (displayRender) {
            opacityCallback_ = [weak = AceType::WeakClaim(AceType::RawPtr(displayRender))](double value) {
                auto display = weak.Upgrade();
                if (!display) {
                    LOGE("display is null");
                    return;
                }
                display->UpdateOpacity(static_cast<uint8_t>(round(value * UINT8_MAX)));
            };
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
}

bool RenderSvg::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_WIDTH) {
        width_ = value;
    } else if (attrName == ATTR_NAME_HEIGHT) {
        height_ = value;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

bool RenderSvg::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_WIDTH) {
        dimension = width_;
    } else if (attrName == ATTR_NAME_HEIGHT) {
        dimension = height_;
    } else {
        LOGE("invalid attrName");
        return false;
    }
    return true;
}

void RenderSvg::PerformLayout()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    const auto& children = GetChildren();
    LayoutParam layoutParam = GetLayoutParam();
    Size layoutSize;
    if (LessNotEqual(width_.Value(), 0.0)) {
        if (layoutParam.GetMaxSize().IsWidthInfinite()) {
            SetLayoutSize(Size(0.0, 0.0));
            return;
        }
        layoutSize.SetWidth(layoutParam.GetMaxSize().Width());
    } else {
        if (isFixSize_) {
            layoutSize.SetWidth(ConvertDimensionToPx(width_, LengthType::HORIZONTAL, isRoot_));
        } else {
            layoutSize.SetWidth(std::clamp(ConvertDimensionToPx(width_, LengthType::HORIZONTAL, isRoot_),
                layoutParam.GetMinSize().Width(), layoutParam.GetMaxSize().Width()));
        }
    }
    if (LessNotEqual(height_.Value(), 0.0)) {
        if (layoutParam.GetMaxSize().IsHeightInfinite()) {
            SetLayoutSize(Size(0.0, 0.0));
            return;
        }
        layoutSize.SetHeight(layoutParam.GetMaxSize().Height());
    } else {
        if (isFixSize_) {
            layoutSize.SetHeight(ConvertDimensionToPx(height_, LengthType::VERTICAL, isRoot_));
        } else {
            layoutSize.SetHeight(std::clamp(ConvertDimensionToPx(height_, LengthType::VERTICAL, isRoot_),
                layoutParam.GetMinSize().Height(), layoutParam.GetMaxSize().Height()));
        }
    }
    SetLayoutSize(layoutSize);
    for (const auto& child : children) {
        child->Layout(LayoutParam(layoutSize, Size()));
    }
    UpdateTransform();
    PrepareAnimations();
}

void RenderSvg::UpdateTransform()
{
    int32_t nodeId = GetNodeId();
    RefPtr<RenderTransform> transform = nullptr;
    auto parent = GetParent().Upgrade();
    while (parent && parent->GetNodeId() == nodeId) {
        transform = AceType::DynamicCast<RenderTransform>(parent);
        if (transform) {
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
    if (!transform) {
        LOGD("transform is null, just return");
        return;
    }
    if (!isRoot_ && (GreatNotEqual(x_.Value(), 0.0) || GreatNotEqual(y_.Value(), 0.0))) {
        transform->Translate(Dimension(ConvertDimensionToPx(x_, LengthType::VERTICAL)),
            Dimension(ConvertDimensionToPx(y_, LengthType::VERTICAL)));
    }
    if (isRoot_ && !NearZero(rootRotate_)) {
        transform->RotateZ(rootRotate_);
    }
    auto& transformAttr = (animateTransformAttrs_.empty()) ? transformAttrs_ : animateTransformAttrs_;
    for (auto& [type, values] : transformAttr) {
        if (type == TRANSFORM_TRANSLATE && values.size() >= 2) {
            transform->Translate(Dimension(values[0]), Dimension(values[1]));
        } else if (type == TRANSFORM_SCALE && values.size() >= 2) {
            transform->Scale(values[0], values[1]);
        } else if (type == TRANSFORM_ROTATE && values.size() >= 1) {
            transform->RotateZ(values[0]);
        } else if (type == TRANSFORM_SKEW && values.size() >= 2) {
            transform->Skew(values[0], values[1]);
        }
    }

    if (isRoot_ && autoMirror_) {
        auto context = context_.Upgrade();
        if (context && context->IsRightToLeft()) {
            Offset center = GetGlobalOffset() + GetLayoutSize() * 0.5;
            transform->Mirror(center, GetGlobalOffset());
        }
    }
}

} // namespace OHOS::Ace
