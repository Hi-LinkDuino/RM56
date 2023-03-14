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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_H

#include "core/components/declaration/svg/svg_fe_declaration.h"
#include "frameworks/core/components/svg/render_svg_base.h"

namespace OHOS::Ace {

class RenderSvgFe : public RenderSvgBase {
    DECLARE_ACE_TYPE(RenderSvgFe, RenderSvgBase);

public:
    void PerformLayout() override;
    void SetFeCommonAttrs(const RefPtr<SvgFeDeclaration>& feDeclaration);
    FeInType GetInType() const;
    ColorInterpolationType GetColorType() const;

protected:
    Dimension x_;
    Dimension y_;
    Dimension height_;
    Dimension width_;
    std::string result_;
    FeInType in_ = FeInType::PRIMITIVE;
    ColorInterpolationType colorInterpolationType_ = ColorInterpolationType::LINEAR_RGB;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_H
