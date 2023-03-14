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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

enum class ColorInterpolationType {
    LINEAR_RGB,
    SRGB,
    AUTO
};

enum class FeInType {
    SOURCE_GRAPHIC,
    SOURCE_ALPHA,
    BACKGROUND_IMAGE,
    BACKGROUND_ALPHA,
    FILL_PAINT,
    STROKE_PAINT,
    PRIMITIVE
};

struct SvgFeAttribute : SvgBaseAttribute {
    Dimension x = Dimension(0.0, DimensionUnit::PERCENT);
    Dimension y = Dimension(0.0, DimensionUnit::PERCENT);
    Dimension height = Dimension(1.0, DimensionUnit::PERCENT);
    Dimension width = Dimension(1.0, DimensionUnit::PERCENT);
    std::string result;
    FeInType in = FeInType::PRIMITIVE;
    ColorInterpolationType colorInterpolationType = ColorInterpolationType::LINEAR_RGB;
};

class SvgFeDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgFeDeclaration, SvgBaseDeclaration);

public:
    SvgFeDeclaration() = default;
    ~SvgFeDeclaration() override = default;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    void SetHeight(const Dimension& height)
    {
        auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.height = height;
    }

    Dimension& GetHeight() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetWidth(const Dimension& width)
    {
        auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.width = width;
    }

    Dimension& GetWidth() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

    void SetResult(const std::string& result)
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        attribute.result = result;
    }

    const std::string& GetResult() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.result;
    }

    void SetIn(const std::string& in)
    {
        static const LinearMapNode<FeInType> IN_TABLE[] = {
            { "BackgroundAlpha", FeInType::BACKGROUND_ALPHA },
            { "BackgroundImage", FeInType::BACKGROUND_IMAGE },
            { "FillPaint", FeInType::FILL_PAINT },
            { "SourceAlpha", FeInType::SOURCE_ALPHA },
            { "SourceGraphic", FeInType::SOURCE_GRAPHIC },
            { "StrokePaint", FeInType::STROKE_PAINT },
        };
        int64_t inIndex = BinarySearchFindIndex(IN_TABLE, ArraySize(IN_TABLE), in.c_str());
        if (inIndex != -1) {
            auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
            attribute.in = IN_TABLE[inIndex].value;
        }
    }

    const FeInType& GetIn() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.in;
    }

    void SetColorInterpolationType(const std::string& colorInterpolationType)
    {
        static const LinearMapNode<ColorInterpolationType> COLOR_INTERPOLATION_TYPE_TABLE[] = {
            { "auto", ColorInterpolationType::AUTO },
            { "linearRGB", ColorInterpolationType::LINEAR_RGB },
            { "sRGB", ColorInterpolationType::SRGB },
        };
        int64_t inIndex = BinarySearchFindIndex(
            COLOR_INTERPOLATION_TYPE_TABLE, ArraySize(COLOR_INTERPOLATION_TYPE_TABLE), colorInterpolationType.c_str());
        if (inIndex != -1) {
            auto& attribute = MaybeResetAttribute<SvgFeAttribute>(AttributeTag::SPECIALIZED_ATTR);
            attribute.colorInterpolationType = COLOR_INTERPOLATION_TYPE_TABLE[inIndex].value;
        }
    }

    const ColorInterpolationType& GetColorInterpolationType() const
    {
        auto& attribute = static_cast<SvgFeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.colorInterpolationType;
    }

protected:
    virtual bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_DECLARATION_H
