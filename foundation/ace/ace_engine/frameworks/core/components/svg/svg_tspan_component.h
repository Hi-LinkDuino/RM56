/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TSPAN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TSPAN_COMPONENT_H

#include "frameworks/core/components/svg/svg_text_component.h"

namespace OHOS::Ace {

class SvgTspanComponent : public SvgTextComponent {
    DECLARE_ACE_TYPE(SvgTspanComponent, SvgTextComponent);

public:
    SvgTspanComponent() = default;
    explicit SvgTspanComponent(const std::list<RefPtr<Component>>& children) : SvgTextComponent(children) {}
    ~SvgTspanComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TSPAN_COMPONENT_H
