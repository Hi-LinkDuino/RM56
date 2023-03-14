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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_GAUSSIANBLUR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_GAUSSIANBLUR_H

#include "core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"
#include "frameworks/core/components/svg/render_svg_fe.h"

namespace OHOS::Ace {

class RenderSvgFeGaussianBlur : public RenderSvgFe {
    DECLARE_ACE_TYPE(RenderSvgFeGaussianBlur, RenderSvgFe);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

protected:
    double deviationX_ = 0.0;
    double deviationY_ = 0.0;
    FeEdgeMode edgeMode_ = FeEdgeMode::EDGE_DUPLICATE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_GAUSSIANBLUR_H
