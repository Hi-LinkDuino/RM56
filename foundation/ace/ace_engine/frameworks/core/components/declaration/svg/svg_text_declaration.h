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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgTextAttribute : SvgBaseAttribute {
    Dimension x;
    Dimension y;
    Dimension dx;
    Dimension dy;
    double rotate = 0.0;
    std::string value;
};

class SvgTextDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgTextDeclaration, SvgBaseDeclaration);

public:
    SvgTextDeclaration() = default;
    ~SvgTextDeclaration() override = default;
    void InitializeStyle() override;

    void SetRotate(double rotate)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.rotate = rotate;
    }

    double GetRotate() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.rotate;
    }

    const std::string& GetTextData() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.value;
    }

    void SetTextData(const std::string& textData)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.value = textData;
    }

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    const Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    const Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    void SetDx(const Dimension& dx)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.dx = dx;
    }

    const Dimension& GetDx() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.dx;
    }

    void SetDy(const Dimension& dy)
    {
        auto& attribute = MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.dy = dy;
    }

    const Dimension& GetDy() const
    {
        auto& attribute = static_cast<SvgTextAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.dy;
    }

    void SetHasX(bool hasX)
    {
        hasX_ = hasX;
    }

    void SetHasY(bool hasY)
    {
        hasY_ = hasY;
    }

    bool HasX() const
    {
        return hasX_;
    }

    bool HasY() const
    {
        return hasY_;
    }

    bool HasRotate() const
    {
        return hasRotate_;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);

    bool hasX_ = false;
    bool hasY_ = false;
    bool hasRotate_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_DECLARATION_H
