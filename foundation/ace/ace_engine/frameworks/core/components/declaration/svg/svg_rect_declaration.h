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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_RECT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_RECT_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgRectAttribute : SvgBaseAttribute {
    Dimension x;
    Dimension y;
    Dimension rx = -1.0_px;
    Dimension ry = -1.0_px;
    Dimension width;
    Dimension height;
};

class SvgRectDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgRectDeclaration, SvgBaseDeclaration);

public:
    SvgRectDeclaration() = default;
    ~SvgRectDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    const Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    const Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    const Dimension& GetRx() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.rx;
    }

    const Dimension& GetRy() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.ry;
    }

    const Dimension& GetWidth() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

    const Dimension& GetHeight() const
    {
        auto& attribute = static_cast<SvgRectAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    void SetRx(const Dimension& rx)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (GreatOrEqual(rx.Value(), 0.0)) {
            attribute.rx = rx;
        }
    }

    void SetRy(const Dimension& ry)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (GreatOrEqual(ry.Value(), 0.0)) {
            attribute.ry = ry;
        }
    }

    void SetWidth(const Dimension& width)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (GreatOrEqual(width.Value(), 0.0)) {
            attribute.width = width;
        }
    }

    void SetHeight(const Dimension& height)
    {
        auto& attribute = MaybeResetAttribute<SvgRectAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (GreatOrEqual(height.Value(), 0.0)) {
            attribute.height = height;
        }
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_RECT_DECLARATION_H
