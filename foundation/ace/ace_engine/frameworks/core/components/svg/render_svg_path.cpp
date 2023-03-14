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

#include "frameworks/core/components/svg/render_svg_path.h"

#include "frameworks/core/components/svg/svg_path_component.h"

namespace OHOS::Ace {
namespace {

const char ATTR_NAME_D[] = "d";

} // namespace

void RenderSvgPath::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgPathComponent> pathComponent = AceType::DynamicCast<SvgPathComponent>(component);
    if (!pathComponent) {
        LOGW("path component is null");
        return;
    }
    d_ = pathComponent->GetD();
    RenderSvgBase::SetPresentationAttrs(component, pathComponent->GetDeclaration());
    PrepareAnimations(component);
    MarkNeedLayout();
}

void RenderSvgPath::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

void RenderSvgPath::PrepareAnimations(const RefPtr<Component>& component)
{
    const RefPtr<SvgPathComponent> pathComponent = AceType::DynamicCast<SvgPathComponent>(component);
    if (!pathComponent) {
        LOGW("path component is null");
        return;
    }
    const auto& componentChildren = pathComponent->GetChildren();
    RenderSvgBase::PrepareAnimation(componentChildren);
}

bool RenderSvgPath::PrepareSelfAnimation(const RefPtr<SvgAnimate>& component)
{
    if (component->GetAttributeName() != ATTR_NAME_D) {
        return false;
    }
    auto svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    component->Copy(svgAnimate);
    paths_.clear();
    PrepareWeightAnimate(svgAnimate, paths_, d_, isBy_);
    std::function<void(double)> callback;
    callback = [weak = AceType::WeakClaim(this)](double value) {
        auto svgPath = weak.Upgrade();
        if (!svgPath) {
            LOGE("svgPath is null");
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
