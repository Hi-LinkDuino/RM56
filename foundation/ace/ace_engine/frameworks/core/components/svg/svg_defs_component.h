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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_DEFS_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_DEFS_COMPONENT_H

#include "core/components/declaration/svg/svg_base_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgDefsComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgDefsComponent, ComponentGroup);

public:
    SvgDefsComponent();
    explicit SvgDefsComponent(const std::list<RefPtr<Component>>& children);
    ~SvgDefsComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetDeclaration(const RefPtr<SvgBaseDeclaration>& declaration);

    const RefPtr<SvgBaseDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

private:
    RefPtr<SvgBaseDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_DEFS_COMPONENT_H
