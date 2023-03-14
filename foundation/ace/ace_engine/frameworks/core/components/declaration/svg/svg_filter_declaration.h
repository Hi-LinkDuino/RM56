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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FILTER_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FILTER_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgFilterAttribute : SvgBaseAttribute {
    Dimension x = Dimension(-0.1, DimensionUnit::PERCENT); // x-axis default value
    Dimension y = Dimension(-0.1, DimensionUnit::PERCENT); // y-axis default value
    Dimension width = Dimension(1.2, DimensionUnit::PERCENT); // masking area width default value
    Dimension height = Dimension(1.2, DimensionUnit::PERCENT); // masking area height default value
};

class SvgFilterDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgFilterDeclaration, SvgBaseDeclaration);

public:
    SvgFilterDeclaration() = default;
    ~SvgFilterDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgFilterAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgFilterAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgFilterAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgFilterAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    void SetHeight(const Dimension& height)
    {
        auto& attribute = MaybeResetAttribute<SvgFilterAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.height = height;
    }

    Dimension& GetHeight() const
    {
        auto& attribute = static_cast<SvgFilterAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetWidth(const Dimension& width)
    {
        auto& attribute = MaybeResetAttribute<SvgFilterAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.width = width;
    }

    Dimension& GetWidth() const
    {
        auto& attribute = static_cast<SvgFilterAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

protected:
    void InitSpecialized() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FILTER_DECLARATION_H
