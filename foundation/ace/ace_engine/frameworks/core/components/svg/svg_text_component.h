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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TEXT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TEXT_COMPONENT_H

#include "core/components/declaration/svg/svg_text_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgTextComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgTextComponent, ComponentGroup);

public:
    SvgTextComponent();
    explicit SvgTextComponent(const std::list<RefPtr<Component>>& children);
    ~SvgTextComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    const std::string& GetTextData() const;
    void SetTextData(const std::string& textData);
    void SetX(const Dimension& x);
    const Dimension& GetX() const;
    void SetY(const Dimension& y);
    const Dimension& GetY() const;
    void SetDx(const Dimension& dx);
    const Dimension& GetDx() const;
    void SetDy(const Dimension& dy);
    const Dimension& GetDy() const;
    void SetHasX(bool hasX);
    bool GetHasX() const;
    void SetHasY(bool hasY);
    bool GetHasY() const;
    void SetRotate(double rotate);
    double GetRotate() const;
    void SetDeclaration(const RefPtr<SvgTextDeclaration>& declaration);

    const RefPtr<SvgTextDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

private:
    RefPtr<SvgTextDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_TEXT_COMPONENT_H
