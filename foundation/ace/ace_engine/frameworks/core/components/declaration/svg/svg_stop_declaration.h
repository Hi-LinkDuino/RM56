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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_STOP_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_STOP_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgStopAttribute : Attribute {
    GradientColor gradientColor;
};

class SvgStopDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgStopDeclaration, SvgBaseDeclaration);

public:
    SvgStopDeclaration() = default;
    ~SvgStopDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetOffset(const Dimension& offset)
    {
        auto& attribute = MaybeResetAttribute<SvgStopAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradientColor.SetDimension(offset);
    }

    void SetColor(const Color& color)
    {
        auto& attribute = MaybeResetAttribute<SvgStopAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradientColor.SetColor(color);
    }

    void SetOpacity(double opacity)
    {
        auto& attribute = MaybeResetAttribute<SvgStopAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.gradientColor.SetOpacity(opacity);
    }

    const GradientColor& GetGradientColor() const
    {
        auto& attribute = static_cast<SvgStopAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.gradientColor;
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_STOP_DECLARATION_H
