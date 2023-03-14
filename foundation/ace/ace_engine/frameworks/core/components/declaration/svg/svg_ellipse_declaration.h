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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ELLIPSE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ELLIPSE_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgEllipseAttribute : SvgBaseAttribute {
    Dimension cx;
    Dimension cy;
    Dimension rx = -1.0_px;
    Dimension ry = -1.0_px;
};

class SvgEllipseDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgEllipseDeclaration, SvgBaseDeclaration);

public:
    SvgEllipseDeclaration() = default;
    ~SvgEllipseDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetCx(const Dimension& cx)
    {
        auto& attribute = MaybeResetAttribute<SvgEllipseAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.cx = cx;
    }

    const Dimension& GetCx() const
    {
        auto& attribute = static_cast<SvgEllipseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.cx;
    }

    void SetCy(const Dimension& cy)
    {
        auto& attribute = MaybeResetAttribute<SvgEllipseAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.cy = cy;
    }

    const Dimension& GetCy() const
    {
        auto& attribute = static_cast<SvgEllipseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.cy;
    }

    void SetRx(const Dimension& rx)
    {
        auto& attribute = MaybeResetAttribute<SvgEllipseAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.rx = rx;
    }

    const Dimension& GetRx() const
    {
        auto& attribute = static_cast<SvgEllipseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.rx;
    }

    void SetRy(const Dimension& ry)
    {
        auto& attribute = MaybeResetAttribute<SvgEllipseAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.ry = ry;
    }

    const Dimension& GetRy() const
    {
        auto& attribute = static_cast<SvgEllipseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.ry;
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ELLIPSE_DECLARATION_H
