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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_QRCODE_QRCODE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_QRCODE_QRCODE_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct QrcodeAttribute : Attribute {
    std::string value;
    QrcodeType qrcodeType { QrcodeType::RECT };
};

struct QrcodeStyle : Style {
    Color backgroundColor;
    Color qrcodeColor;
    Dimension qrcodeWidth;
    Dimension qrcodeHeight;
    bool isWidthDefined = false;
    bool isHeightDefined = false;
};

class QrcodeDeclaration : public Declaration {
    DECLARE_ACE_TYPE(QrcodeDeclaration, Declaration);

public:
    QrcodeDeclaration() = default;
    ~QrcodeDeclaration() override = default;

    void InitializeStyle() override;

    const std::string& GetValue() const
    {
        auto& attribute = static_cast<QrcodeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.value;
    }

    QrcodeType GetType() const
    {
        auto& attribute = static_cast<QrcodeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.qrcodeType;
    }

    const Color& GetBackgroundColor() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.backgroundColor;
    }

    const Color& GetQrcodeColor() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.qrcodeColor;
    }

    const Dimension& GetQrcodeWidth() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.qrcodeWidth;
    }

    const Dimension& GetQrcodeHeight() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.qrcodeHeight;
    }

    bool IsWidthDefined() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.isWidthDefined;
    }

    bool IsHeightDefined() const
    {
        auto& style = static_cast<QrcodeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.isHeightDefined;
    }

    void SetValue(const std::string& value)
    {
        auto& attribute = MaybeResetAttribute<QrcodeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.value = value;
    }

    void SetQrcodeType(QrcodeType qrcodeType)
    {
        auto& attribute = MaybeResetAttribute<QrcodeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.qrcodeType = qrcodeType;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.backgroundColor = backgroundColor;
    }

    void SetQrcodeColor(const Color& qrcodeColor)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.qrcodeColor = qrcodeColor;
    }

    void SetQrcodeWidth(const Dimension& qrcodeWidth)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.qrcodeWidth = qrcodeWidth;
        style.isWidthDefined = true;
    }

    void SetQrcodeHeight(const Dimension& qrcodeHeight)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.qrcodeHeight = qrcodeHeight;
        style.isHeightDefined = true;
    }

    void SetWidthDefined(bool isWidthDefined)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.isWidthDefined = isWidthDefined;
    }

    void SetHeightDefined(bool isHeightDefined)
    {
        auto& style = MaybeResetStyle<QrcodeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.isHeightDefined = isHeightDefined;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_QRCODE_QRCODE_DECLARATION_H
