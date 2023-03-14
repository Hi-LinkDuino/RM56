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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_GRADIENT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_GRADIENT_DECLARATION_H

#include "core/components/declaration/common/declaration.h"

namespace OHOS::Ace {

struct SvgGradientAttribute : Attribute {
    Gradient gradient = Gradient();
};

class SvgGradientDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SvgGradientDeclaration, Declaration);

public:
    SvgGradientDeclaration() = default;
    ~SvgGradientDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetTransform(const std::string& gradientTransform)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.SetGradientTransform(gradientTransform);
    }

    void SetSpreadMethod(SpreadMethod spreadMethod)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.SetSpreadMethod(spreadMethod);
    }

    void SetX1(const Dimension& x1)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetLinearGradient().x1 = x1;
    }

    void SetY1(const Dimension& y1)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetLinearGradient().y1 = y1;
    }

    void SetX2(const Dimension& x2)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetLinearGradient().x2 = x2;
    }

    void SetY2(const Dimension& y2)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetLinearGradient().y2 = y2;
    }

    void SetR(const Dimension& r)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetRadialGradient().radialHorizontalSize = r;
        attribute.gradient.GetRadialGradient().radialVerticalSize = r;
    }

    void SetCx(const Dimension& cx)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetRadialGradient().radialCenterX = cx;
    }

    void SetCy(const Dimension& cy)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetRadialGradient().radialCenterY = cy;
    }

    void SetFx(const Dimension& fx)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetRadialGradient().fRadialCenterX = fx;
    }

    void SetFy(const Dimension& fy)
    {
        auto& attribute = MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradient.GetRadialGradient().fRadialCenterY = fy;
    }

    const std::string& GetTransform() const
    {
        auto& attribute = static_cast<SvgGradientAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.gradient.GetGradientTransform();
    }

    SpreadMethod GetSpreadMethod() const
    {
        auto& attribute = static_cast<SvgGradientAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.gradient.GetSpreadMethod();
    }

    const Gradient& GetGradient() const
    {
        auto& attribute = static_cast<SvgGradientAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.gradient;
    }

    void SetGradientType(GradientType gradientType)
    {
        auto& attribute = static_cast<SvgGradientAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.gradient.SetType(gradientType);
    }

    void AddGradientColor(const GradientColor& gradientColor)
    {
        auto& attribute = static_cast<SvgGradientAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        attribute.gradient.AddColor(gradientColor);
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_GRADIENT_DECLARATION_H
