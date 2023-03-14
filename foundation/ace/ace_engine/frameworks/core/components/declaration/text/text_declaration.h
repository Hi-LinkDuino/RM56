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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXT_TEXT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXT_TEXT_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct TextSpecializedAttribute : Attribute {
    std::string data;
};

struct TextSpecializedStyle : Style {
    TextStyle textStyle;
    Color focusColor;
    bool isMaxWidthLayout = false;
    bool autoMaxLines = false;
};

class TextDeclaration : public Declaration {
    DECLARE_ACE_TYPE(TextDeclaration, Declaration);

public:
    TextDeclaration() = default;
    ~TextDeclaration() override = default;

    void InitializeStyle() override;

    const std::string& GetData() const
    {
        auto& attribute = static_cast<TextSpecializedAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.data;
    }

    void SetData(const std::string& data)
    {
        auto& attribute = MaybeResetAttribute<TextSpecializedAttribute>(AttributeTag::SPECIALIZED_ATTR);
        CheckIsChanged(attribute.data, data);
        attribute.data = data;
    }

    const TextStyle& GetTextStyle() const
    {
        auto& style = static_cast<TextSpecializedStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textStyle;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        auto& style = MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
        CheckIsChanged(style.textStyle, textStyle);
        style.textStyle = textStyle;
    }

    const Color& GetFocusColor() const
    {
        auto& style = static_cast<TextSpecializedStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusColor;
    }

    void SetFocusColor(const Color& focusColor)
    {
        auto& style = MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
        CheckIsChanged(style.focusColor, focusColor);
        style.focusColor = focusColor;
    }

    bool IsMaxWidthLayout() const
    {
        auto& style = static_cast<TextSpecializedStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.isMaxWidthLayout;
    }

    void SetIsMaxWidthLayout(bool isMaxWidthLayout)
    {
        auto& style = MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
        CheckIsChanged(style.isMaxWidthLayout, isMaxWidthLayout);
        style.isMaxWidthLayout = isMaxWidthLayout;
    }

    bool GetAutoMaxLines() const
    {
        auto& style = static_cast<TextSpecializedStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.autoMaxLines;
    }

    void SetAutoMaxLines(bool autoMaxLines)
    {
        auto& style = MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
        CheckIsChanged(style.autoMaxLines, autoMaxLines);
        style.autoMaxLines = autoMaxLines;
    }

    bool IsChanged() const
    {
        return isChanged_;
    }

    void SetIsChanged(bool isChanged)
    {
        isChanged_ = isChanged;
    }

    bool HasSetTextFontSize() const
    {
        return hasSetTextFontSize_;
    }

    bool HasSetTextColor() const
    {
        return hasSetTextColor_;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

private:
    template<class T>
    void CheckIsChanged(const T& update, const T& val)
    {
        if (update != val) {
            isChanged_ = true;
        }
    }

    bool isChanged_ = false;
    bool hasSetTextColor_ = false;
    bool hasSetTextFontSize_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXT_TEXT_DECLARATION_H
