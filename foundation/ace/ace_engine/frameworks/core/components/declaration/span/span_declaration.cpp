/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/declaration/span/span_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/common/properties/text_style_parser.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension DEFAULT_FONT_SIZE = 14.0_px;

} // namespace

using namespace Framework;

void SpanDeclaration::InitCommonEvent()
{
    AddCommonEvent(EventTag::COMMON_RAW_EVENT);
    AddCommonEvent(EventTag::COMMON_GESTURE_EVENT);
    AddCommonEvent(EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT);
}

void SpanDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SPAN_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_SPAN_STYLE);
}

bool SpanDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SpanDeclaration&)> spanStyleOperators[] = {
        { DOM_SHOW,
            [](const std::string& val, SpanDeclaration& declaration) { declaration.SetIsShow(StringToBool(val)); } },
        { DOM_VALUE, [](const std::string& val, SpanDeclaration& declaration) { declaration.SetSpanData(val); } },
    };
    auto spanStyleIter = BinarySearchFindIndex(spanStyleOperators, ArraySize(spanStyleOperators), attr.first.c_str());
    if (spanStyleIter != -1) {
        spanStyleOperators[spanStyleIter].value(attr.second, *this);
        return true;
    }
    return false;
}

bool SpanDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, SpanDeclaration&)> spanStyleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetAllowScale(StringToBool(val));
                }
                declaration.isSetAllowScale_ = true;
            } },
        { DOM_TEXT_COLOR,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    auto textColor = val.empty() ? Color::BLACK : declaration.ParseColor(val);
                    specializedStyle.spanStyle.SetTextColor(textColor);
                }
                declaration.isSetFontColor_ = true;
            } },
        { DOM_TEXT_FONT_FAMILY,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetFontFamilies(declaration.ParseFontFamilies(val));
                }
                declaration.isSetFontFamily_ = true;
            } },
        { DOM_TEXT_FONT_FEATURE_SETTINGS,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetFontFeatures(ParseFontFeatureSettings(val));
                }
                declaration.isSetFontFeatures_ = true;
            } },
        { DOM_TEXT_FONT_SIZE,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    auto fontSize = val.empty() ? DEFAULT_FONT_SIZE : declaration.ParseDimension(val);
                    specializedStyle.spanStyle.SetFontSize(fontSize);
                }
                declaration.isSetFontSize_ = true;
            } },
        { DOM_TEXT_FONT_STYLE,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetFontStyle(ConvertStrToFontStyle(val));
                }
                declaration.isSetFontStyle_ = true;
            } },
        { DOM_TEXT_FONT_VARIANT,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetFontFeatures(ParseFontVariants(val));
                }
                declaration.isSetFontFeatures_ = true;
            } },
        { DOM_TEXT_FONT_WEIGHT,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetFontWeight(ConvertStrToFontWeight(val));
                }
                declaration.isSetFontWeight_ = true;
            } },
        { DOM_TEXT_DECORATION,
            [](const std::string& val, SpanDeclaration& declaration) {
                auto& specializedStyle = declaration.MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
                if (specializedStyle.IsValid()) {
                    specializedStyle.spanStyle.SetTextDecoration(ConvertStrToTextDecoration(val));
                }
                declaration.isSetTextDecoration_ = true;
            } },
    };
    auto spanStyleIter = BinarySearchFindIndex(spanStyleOperators, ArraySize(spanStyleOperators), style.first.c_str());
    if (spanStyleIter != -1) {
        spanStyleOperators[spanStyleIter].value(style.second, *this);
    }

    // span has no box component, set true to prevent setting style to box (empty object).
    return true;
}

} // namespace OHOS::Ace
