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

#include "core/components/test/json/text_style_creator.h"

#include "base/log/log.h"
#include "core/components/common/properties/color.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/shadow_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

constexpr FontWeight FONT_WEIGHTS[] = { FontWeight::W100, FontWeight::W200, FontWeight::W300, FontWeight::W400,
    FontWeight::W500, FontWeight::W600, FontWeight::W700, FontWeight::W800, FontWeight::W900, FontWeight::BOLD,
    FontWeight::NORMAL, FontWeight::BOLDER, FontWeight::LIGHTER };
constexpr FontStyle FONT_STYLES[] = { FontStyle::NORMAL, FontStyle::ITALIC };
constexpr TextBaseline BASE_LINES[] = { TextBaseline::ALPHABETIC, TextBaseline::IDEOGRAPHIC };
constexpr TextDecoration TEXT_DECORATIONS[] = { TextDecoration::NONE, TextDecoration::UNDERLINE,
    TextDecoration::OVERLINE, TextDecoration::LINE_THROUGH, TextDecoration::INHERIT };

} // namespace

TextStyle TextStyleCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson TextStyle");
    TextStyle textStyle;
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != TEXT_STYLE_NAME) {
        LOGW("Create TextStyle err: not a TextStyle json.");
        // Currently return a default text style has no side effect.
        return textStyle;
    }
    if (json.Contains(TEXT_FONT_SIZE) && json.GetValue(TEXT_FONT_SIZE)->IsNumber()) {
        textStyle.SetFontSize(Dimension(json.GetValue(TEXT_FONT_SIZE)->GetDouble(), DimensionUnit::PX));
    }
    if (json.Contains(TEXT_FONT_WEIGHT) && json.GetValue(TEXT_FONT_WEIGHT)->IsNumber()) {
        textStyle.SetFontWeight(
            ConvertIntToEnum(json.GetValue(TEXT_FONT_WEIGHT)->GetInt(), FONT_WEIGHTS, FontWeight::NORMAL));
    }
    if (json.Contains(TEXT_FONT_STYLE) && json.GetValue(TEXT_FONT_STYLE)->IsNumber()) {
        textStyle.SetFontStyle(
            ConvertIntToEnum(json.GetValue(TEXT_FONT_STYLE)->GetInt(), FONT_STYLES, FontStyle::NORMAL));
    }
    if (json.Contains(TEXT_BASE_LINE) && json.GetValue(TEXT_BASE_LINE)->IsNumber()) {
        textStyle.SetTextBaseline(
            ConvertIntToEnum(json.GetValue(TEXT_BASE_LINE)->GetInt(), BASE_LINES, TextBaseline::ALPHABETIC));
    }
    if (json.Contains(TEXT_COLOR) && json.GetValue(TEXT_COLOR)->IsObject()) {
        auto colorJson = json.GetValue(TEXT_COLOR);
        auto color = ColorCreator::CreateFromJson(*colorJson);
        textStyle.SetTextColor(color);
    }
    if (json.Contains(TEXT_DECORATION) && json.GetValue(TEXT_DECORATION)->IsNumber()) {
        textStyle.SetTextDecoration(
            ConvertIntToEnum(json.GetValue(TEXT_DECORATION)->GetInt(), TEXT_DECORATIONS, TextDecoration::NONE));
    }
    if (json.Contains(SHADOW) && json.GetValue(SHADOW)->IsObject()) {
        auto shadow = ShadowCreator::CreateFromJson(*(json.GetValue(SHADOW)));
        textStyle.SetShadow(shadow);
    }
    if (json.Contains(TEXT_WORD_SPACING) && json.GetValue(TEXT_WORD_SPACING)->IsNumber()) {
        textStyle.SetWordSpacing(Dimension(json.GetValue(TEXT_WORD_SPACING)->GetDouble()));
    }
    auto arrayJsonObject = json.GetValue(TEXT_FONT_FAMILIES);
    if (arrayJsonObject && arrayJsonObject->IsArray()) {
        std::vector<std::string> fontFamilies_;
        for (int32_t i = 0; i < arrayJsonObject->GetArraySize(); i++) {
            fontFamilies_.emplace_back(arrayJsonObject->GetArrayItem(i)->GetString());
        }
        textStyle.SetFontFamilies(fontFamilies_);
    }
    if (json.Contains(TEXT_LINE_HEIGHT) && json.GetValue(TEXT_LINE_HEIGHT)->IsNumber()) {
        textStyle.SetLineHeight(Dimension(json.GetValue(TEXT_LINE_HEIGHT)->GetDouble(), DimensionUnit::PX));
    }
    if (json.Contains(TEXT_LETTER_SPACING) && json.GetValue(TEXT_LETTER_SPACING)->IsNumber()) {
        textStyle.SetLetterSpacing(Dimension(json.GetValue(TEXT_LETTER_SPACING)->GetDouble()));
    }
    if (json.Contains(TEXT_ADAPT) && json.GetValue(TEXT_ADAPT)->IsBool()) {
        if (json.Contains(TEXT_ADAPT_MIN_FONTSIZE) && json.GetValue(TEXT_ADAPT_MIN_FONTSIZE)->IsNumber()) {
            textStyle.SetAdaptTextSize(textStyle.GetFontSize(),
                Dimension(json.GetValue(TEXT_ADAPT_MIN_FONTSIZE)->GetDouble(), DimensionUnit::FP));
        }
    }
    return textStyle;
}

} // namespace OHOS::Ace