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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_LINE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_LINE_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgLineAttribute : SvgBaseAttribute {
    Dimension x1;
    Dimension y1;
    Dimension x2;
    Dimension y2;
};

class SvgLineDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgLineDeclaration, SvgBaseDeclaration);

public:
    SvgLineDeclaration() = default;
    ~SvgLineDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetX1(const Dimension& x1)
    {
        auto& attribute = MaybeResetAttribute<SvgLineAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x1 = x1;
    }

    void SetX2(const Dimension& x2)
    {
        auto& attribute = MaybeResetAttribute<SvgLineAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x2 = x2;
    }

    void SetY1(const Dimension& y1)
    {
        auto& attribute = MaybeResetAttribute<SvgLineAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y1 = y1;
    }

    void SetY2(const Dimension& y2)
    {
        auto& attribute = MaybeResetAttribute<SvgLineAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y2 = y2;
    }

    const Dimension& GetX1() const
    {
        auto& attribute = static_cast<SvgLineAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x1;
    }

    const Dimension& GetX2() const
    {
        auto& attribute = static_cast<SvgLineAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x2;
    }

    const Dimension& GetY1() const
    {
        auto& attribute = static_cast<SvgLineAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y1;
    }

    const Dimension& GetY2() const
    {
        auto& attribute = static_cast<SvgLineAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y2;
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_LINE_DECLARATION_H
