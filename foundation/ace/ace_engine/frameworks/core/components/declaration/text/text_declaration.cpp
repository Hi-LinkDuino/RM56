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

#include "core/components/declaration/text/text_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/common/properties/text_style_parser.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/text/text_theme.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension DEFAULT_LETTER_SPACING = 0.0_px; // unit is px
constexpr Dimension DEFAULT_LINE_HEIGHT = 0.0_px;    // unit is px
constexpr Dimension DEFAULT_FONT_SIZE = 14.0_px;
const char DEFAULT_FONT_FAMILY[] = "sans-serif";

} // namespace

using namespace Framework;

void TextDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_TEXT_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_TEXT_STYLE);
}

void TextDeclaration::InitializeStyle()
{
    auto& style = MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
    if (!style.IsValid()) {
        return;
    }

    RefPtr<TextTheme> theme = GetTheme<TextTheme>();
    if (theme) {
        style.textStyle = theme->GetTextStyle();
        style.focusColor = theme->GetTextStyle().GetTextColor();
        std::vector<std::string> defaultFontFamilis;
        defaultFontFamilis.emplace_back(DEFAULT_FONT_FAMILY);
        style.textStyle.SetFontFamilies(defaultFontFamilis);
        style.textStyle.SetLetterSpacing(DEFAULT_LETTER_SPACING);
        style.textStyle.SetLineHeight(DEFAULT_LINE_HEIGHT, false);
    }
}

bool TextDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_VALUE) {
        SetData(attr.second);
        return true;
    }
    return false;
}

bool TextDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, TextDeclaration&)> textStyleOperators[] = {
        { DOM_TEXT_ADAPT_HEIGHT,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetAdaptHeight(StringToBool(val));
                }
            } },
        { DOM_TEXT_ALLOW_SCALE,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetAllowScale(StringToBool(val));
                }
            } },
        { DOM_TEXT_COLOR,
            [](const std::string& val, TextDeclaration& declaration) {
                declaration.hasSetTextColor_ = true;
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    const auto& color = val.empty() ? Color::BLACK : declaration.ParseColor(val);
                    specializedStyle.textStyle.SetTextColor(color);
                    specializedStyle.focusColor = color;
                }
            } },
        { DOM_TEXT_FONT_FAMILY,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetFontFamilies(declaration.ParseFontFamilies(val));
                }
            } },
        { DOM_TEXT_FONT_FEATURE_SETTINGS,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetFontFeatures(ParseFontFeatureSettings(val));
                }
            } },
        { DOM_TEXT_FONT_SIZE,
            [](const std::string& val, TextDeclaration& declaration) {
                declaration.hasSetTextFontSize_ = true;
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    auto fontSize = val.empty() ? DEFAULT_FONT_SIZE : declaration.ParseDimension(val);
                    specializedStyle.textStyle.SetFontSize(fontSize);
                }
            } },
        { DOM_TEXT_FONT_SIZE_STEP,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetAdaptFontSizeStep(declaration.ParseDimension(val));
                }
            } },
        { DOM_TEXT_FONT_STYLE,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetFontStyle(ConvertStrToFontStyle(val));
                }
            } },
        { DOM_TEXT_FONT_VARIANT,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetFontFeatures(ParseFontVariants(val));
                }
            } },
        { DOM_TEXT_FONT_WEIGHT,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetFontWeight(ConvertStrToFontWeight(val));
                }
            } },
        { DOM_TEXT_LETTER_SPACING,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetLetterSpacing(declaration.ParseDimension(val));
                }
            } },
        { DOM_TEXT_LINE_HEIGHT,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    if (val == "normal") {
                        specializedStyle.textStyle.SetLineHeight(Dimension(0.0));
                    } else {
                        specializedStyle.textStyle.SetLineHeight(declaration.ParseLineHeight(val));
                    }
                }
            } },
        { DOM_TEXT_LINES,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetMaxLines(StringUtils::StringToInt(val));
                }
            } },
        { DOM_TEXT_MAX_FONT_SIZE,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetAdaptMaxFontSize(declaration.ParseDimension(val));
                }
            } },
        { DOM_TEXT_MAX_LINES,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetMaxLines(StringUtils::StringToUint(val, UINT32_MAX));
                    declaration.SetAutoMaxLines(val == DOM_AUTO);
                }
            } },
        { DOM_TEXT_MIN_FONT_SIZE,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetAdaptMinFontSize(declaration.ParseDimension(val));
                }
            } },
        { DOM_TEXT_PREFER_FONT_SIZES,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetPreferFontSizes(declaration.ParsePreferFontSizes(val));
                }
            } },
        { DOM_TEXT_ALIGN,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextAlign(ConvertStrToTextAlign(val));
                }
            } },
        { DOM_TEXT_DECORATION,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextDecoration(ConvertStrToTextDecoration(val));
                }
            } },
        { DOM_TEXT_DECORATION_COLOR,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextDecorationColor(declaration.ParseColor(val));
                }
            } },
        { DOM_TEXT_INDENT, [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextIndent(StringToDimension(val));
                }
            } },
        { DOM_TEXT_OVERFLOW,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextOverflow(ConvertStrToTextOverflow(val));
                }
            } },
        { DOM_TEXT_VERTICAL_ALIGN,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetTextVerticalAlign(ConvertStrToTextVerticalAlign(val));
                }
            } },
        { DOM_TEXT_WHITE_SPACE, [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetWhiteSpace(ConvertStrToWhiteSpace(val));
                }
            } },
        { DOM_TEXT_WORD_BREAK,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.textStyle.SetWordBreak(ConvertStrToWordBreak(val));
                }
            } },
        { DOM_TEXT_WORD_SPACING,
            [](const std::string& val, TextDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<TextSpecializedStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    if (val == "normal") {
                        specializedStyle.textStyle.SetWordSpacing(Dimension(0.0));
                    } else {
                        specializedStyle.textStyle.SetWordSpacing(StringToDimension(val));
                    }
                }
            } },
    };
    auto operatorIter = BinarySearchFindIndex(textStyleOperators, ArraySize(textStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        textStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
