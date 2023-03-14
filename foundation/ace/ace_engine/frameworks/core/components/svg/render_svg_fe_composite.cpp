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

#include "frameworks/core/components/svg/render_svg_fe_composite.h"

#include "frameworks/core/components/svg/svg_fe_composite_component.h"

namespace OHOS::Ace {

void RenderSvgFeComposite::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgFeCompositeComponent> feComponent = AceType::DynamicCast<SvgFeCompositeComponent>(component);
    if (!feComponent) {
        LOGW("fe offset component is null");
        return;
    }

    auto& declaration = feComponent->GetDeclaration();
    if (declaration) {
        in2_ = declaration->GetIn2();
        operatorType_ = declaration->GetOperatorType();
        k1_ = declaration->GetK1();
        k2_ = declaration->GetK2();
        k3_ = declaration->GetK3();
        k4_ = declaration->GetK4();
        SetFeCommonAttrs(declaration);
    }
}

FeInType RenderSvgFeComposite::GetIn2Type()
{
    return in2_;
}

} // namespace OHOS::Ace
