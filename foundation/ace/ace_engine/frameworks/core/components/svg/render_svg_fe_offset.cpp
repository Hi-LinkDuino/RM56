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

#include "frameworks/core/components/svg/render_svg_fe_offset.h"

#include "frameworks/core/components/svg/svg_fe_offset_component.h"

namespace OHOS::Ace {

void RenderSvgFeOffset::Update(const RefPtr<Component> &component)
{
    const RefPtr<SvgFeOffsetComponent> feComponent = AceType::DynamicCast<SvgFeOffsetComponent>(component);
    if (!feComponent) {
        LOGW("fe offset component is null");
        return;
    }

    auto& declaration = feComponent->GetDeclaration();
    if (declaration) {
        dx_ = declaration->GetDx();
        dy_ = declaration->GetDy();
        SetFeCommonAttrs(declaration);
    }
}

} // namespace OHOS::Ace
