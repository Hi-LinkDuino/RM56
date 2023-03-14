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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_COMPOSITE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_COMPOSITE_H

#include "frameworks/core/components/declaration/svg/svg_fe_composite_declaration.h"
#include "frameworks/core/components/svg/render_svg_fe.h"

namespace OHOS::Ace {

class RenderSvgFeComposite : public RenderSvgFe {
    DECLARE_ACE_TYPE(RenderSvgFeComposite, RenderSvgFe);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    FeInType GetIn2Type();

protected:
    FeInType in2_ = FeInType::PRIMITIVE;
    FeOperatorType operatorType_ = FeOperatorType::FE_OVER;
    double k1_ = 0.0;
    double k2_ = 0.0;
    double k3_ = 0.0;
    double k4_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_FE_COMPOSITE_H
