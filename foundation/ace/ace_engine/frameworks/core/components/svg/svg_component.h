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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_COMPONENT_H

#include "core/components/declaration/svg/svg_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgComponent, ComponentGroup);

public:
    SvgComponent();
    explicit SvgComponent(const std::list<RefPtr<Component>>& children);
    ~SvgComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void SetX(const Dimension& x);
    const Dimension& GetX() const;
    void SetY(const Dimension& y);
    const Dimension& GetY() const;
    void SetWidth(const Dimension& width);
    const Dimension& GetWidth() const;
    void SetHeight(const Dimension& height);
    const Dimension& GetHeight() const;
    void SetViewBox(const Rect& viewBox);
    const Rect& GetViewBox() const;
    void SetAutoMirror(bool autoMirror);
    bool GetAutoMirror() const;
    void SetDeclaration(const RefPtr<SvgDeclaration>& declaration);

    void MarkIsRoot(bool isRoot)
    {
        isRoot_ = isRoot;
    }

    bool IsRoot() const
    {
        return isRoot_;
    }

    const RefPtr<SvgDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

    void Inherit(const RefPtr<SvgBaseDeclaration>& parent)
    {
        declaration_->Inherit(parent);
    }

private:
    RefPtr<SvgDeclaration> declaration_;
    bool isRoot_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_COMPONENT_H
