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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgAttribute : SvgBaseAttribute {
    Rect viewBox;
    Dimension x;
    Dimension y;
    Dimension width = -1.0_px;
    Dimension height = -1.0_px;
    bool autoMirror = false;
};

class SvgDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgDeclaration, SvgBaseDeclaration);

public:
    SvgDeclaration() = default;
    ~SvgDeclaration() override = default;
    void InitializeStyle() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    void SetX(const Dimension& x)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.x = x;
    }

    const Dimension& GetX() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.x;
    }

    void SetY(const Dimension& y)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.y = y;
    }

    const Dimension& GetY() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.y;
    }

    void SetWidth(const Dimension& width)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.width = width;
    }

    const Dimension& GetWidth() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

    void SetHeight(const Dimension& height)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.height = height;
    }

    const Dimension& GetHeight() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetViewBox(const Rect& viewBox)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.viewBox = viewBox;
    }

    const Rect& GetViewBox() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.viewBox;
    }

    void SetAutoMirror(bool autoMirror)
    {
        auto& attribute = MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.autoMirror = autoMirror;
    }

    bool GetAutoMirror() const
    {
        auto& attribute = static_cast<SvgAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.autoMirror;
    }

protected:
    void InitSpecialized() override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_DECLARATION_H
