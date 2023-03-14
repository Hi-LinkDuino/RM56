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

#include "frameworks/core/components/svg/render_svg_text_path.h"

#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/components/svg/svg_text_path_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_START_OFFSET[] = "startOffset";

} // namespace

void RenderSvgTextPath::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgTextPathComponent> textPathComponent = AceType::DynamicCast<SvgTextPathComponent>(component);
    if (!textPathComponent) {
        LOGW("svg text path component is null");
        return;
    }

    startOffset_ = textPathComponent->GetStartOffset();
    path_ = textPathComponent->GetPath();
    textData_ = textPathComponent->GetTextData();
    RenderSvgBase::SetPresentationAttrs(textPathComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgTextPath::PrepareAnimations(const RefPtr<Component>& component)
{
    const RefPtr<SvgTextPathComponent> textPathComponent = AceType::DynamicCast<SvgTextPathComponent>(component);
    if (!textPathComponent) {
        LOGW("svg text path component is null");
        return;
    }
    const auto& componentChildren = textPathComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgTextPath::PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate)
{
    Dimension originalValue;
    if (!GetProperty(svgAnimate->GetAttributeName(), originalValue)) {
        return false;
    }
    std::function<void(Dimension)> callback;
    callback = [weakText = AceType::WeakClaim(this), attributeName = svgAnimate->GetAttributeName()](Dimension value) {
        auto textPath = weakText.Upgrade();
        if (!textPath) {
            LOGE("svg text path is null");
            return;
        }
        bool ret = textPath->SetProperty(attributeName, value);
        if (ret) {
            textPath->OnNotifyRender();
        }
    };
    CreatePropertyAnimation(svgAnimate, originalValue, std::move(callback));
    return true;
}

bool RenderSvgTextPath::SetProperty(const std::string& attrName, const Dimension& value)
{
    if (attrName == ATTR_NAME_START_OFFSET) {
        startOffset_ = value;
    } else {
        return false;
    }
    return true;
}

bool RenderSvgTextPath::GetProperty(const std::string& attrName, Dimension& dimension) const
{
    if (attrName == ATTR_NAME_START_OFFSET) {
        dimension = startOffset_;
    } else {
        return false;
    }
    return true;
}

void RenderSvgTextPath::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
}

} // namespace OHOS::Ace
