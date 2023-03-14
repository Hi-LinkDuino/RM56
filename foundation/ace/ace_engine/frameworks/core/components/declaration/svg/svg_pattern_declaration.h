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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_PATTERN_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_PATTERN_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgPatternAttribute : SvgBaseAttribute {
    Dimension x; // x-axis default value
    Dimension y; // y-axis default value
    Dimension width; // pattern area width default value
    Dimension height; // pattern area height default value
    std::string patternUnits = "objectBoundingBox";
    std::string patternContentUnits = "userSpaceOnUse";
    std::string patternTransform;
    Rect viewBox;
};

class SvgPatternDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgPatternDeclaration, SvgBaseDeclaration);

public:
    SvgPatternDeclaration() = default;
    ~SvgPatternDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    void SetHeight(const Dimension& height)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.height = height;
    }

    Dimension& GetHeight() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetWidth(const Dimension& width)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.width = width;
    }

    Dimension& GetWidth() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

    const std::string& GetPatternUnits() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.patternUnits;
    }

    void SetPatternUnits(const std::string& patternUnits)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.patternUnits = patternUnits;
    }

    const std::string& GetPatternContentUnits() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.patternContentUnits;
    }

    void SetPatternContentUnits(const std::string& patternContentUnits)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.patternContentUnits = patternContentUnits;
    }

    const std::string& GetPatternTransform() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.patternTransform;
    }

    void SetPatternTransform(const std::string& patternTransform)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.patternTransform = patternTransform;
    }

    const Rect& GetViewBox() const
    {
        auto& attribute = static_cast<SvgPatternAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.viewBox;
    }

    void SetViewBox(const Rect& viewBox)
    {
        auto& attribute = MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.viewBox = viewBox;
    }

    const std::string& GetId()
    {
        auto& commonAttr = MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
        return commonAttr.id;
    }

protected:
    void InitSpecialized() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_PATTERN_DECLARATION_H
