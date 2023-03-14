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

#include "frameworks/core/components/svg/render_svg_fe.h"

namespace OHOS::Ace {

void RenderSvgFe::PerformLayout()
{
    LayoutParam layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
}

void RenderSvgFe::SetFeCommonAttrs(const RefPtr<SvgFeDeclaration>& feDeclaration)
{
    if (feDeclaration) {
        x_ = feDeclaration->GetX();
        y_ = feDeclaration->GetY();
        width_ = feDeclaration->GetWidth();
        height_ = feDeclaration->GetHeight();
        result_ = feDeclaration->GetResult();
        in_ = feDeclaration->GetIn();
        colorInterpolationType_ = feDeclaration->GetColorInterpolationType();

        SetPresentationAttrs(feDeclaration);
    }
}

FeInType RenderSvgFe::GetInType() const
{
    return in_;
}

ColorInterpolationType RenderSvgFe::GetColorType() const
{
    return colorInterpolationType_;
}

} // namespace OHOS::Ace
