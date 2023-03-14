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

#include "constants_converter.h"

#include "flutter/third_party/txt/src/txt/font_style.h"
#include "flutter/third_party/txt/src/txt/font_weight.h"
#include "flutter/third_party/txt/src/txt/paragraph_style.h"
#include "flutter/third_party/txt/src/txt/text_decoration.h"

#include "base/i18n/localization.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/text_style.h"

namespace OHOS::Ace::Constants {

txt::FontWeight ConvertTxtFontWeight(FontWeight fontWeight)
{
    txt::FontWeight convertValue;
    switch (fontWeight) {
        case FontWeight::W100:
        case FontWeight::LIGHTER:
            convertValue = txt::FontWeight::w100;
            break;
        case FontWeight::W200:
            convertValue = txt::FontWeight::w200;
            break;
        case FontWeight::W300:
            convertValue = txt::FontWeight::w300;
            break;
        case FontWeight::W400:
        case FontWeight::NORMAL:
        case FontWeight::REGULAR:
            convertValue = txt::FontWeight::w400;
            break;
        case FontWeight::W500:
        case FontWeight::MEDIUM:
            convertValue = txt::FontWeight::w500;
            break;
        case FontWeight::W600:
            convertValue = txt::FontWeight::w600;
            break;
        case FontWeight::W700:
        case FontWeight::BOLD:
            convertValue = txt::FontWeight::w700;
            break;
        case FontWeight::W800:
            convertValue = txt::FontWeight::w800;
            break;
        case FontWeight::W900:
        case FontWeight::BOLDER:
            convertValue = txt::FontWeight::w900;
            break;
        default:
            LOGW("FontWeight setting error! Now using default FontWeight.");
            convertValue = txt::FontWeight::w400;
            break;
    }
    return convertValue;
}

txt::FontStyle ConvertTxtFontStyle(FontStyle fontStyle)
{
    txt::FontStyle convertValue;
    switch (fontStyle) {
        case FontStyle::NORMAL:
            convertValue = txt::FontStyle::normal;
            break;
        case FontStyle::ITALIC:
            convertValue = txt::FontStyle::italic;
            break;
        default:
            LOGW("FontStyle setting error! Now using default FontStyle");
            convertValue = txt::FontStyle::normal;
            break;
    }
    return convertValue;
}

txt::TextBaseline ConvertTxtTextBaseline(TextBaseline textBaseline)
{
    txt::TextBaseline convertValue;
    switch (textBaseline) {
        case TextBaseline::ALPHABETIC:
            convertValue = txt::TextBaseline::kAlphabetic;
            break;
        case TextBaseline::IDEOGRAPHIC:
            convertValue = txt::TextBaseline::kIdeographic;
            break;
        default:
            LOGW("TextBaseline setting error! Now using default TextBaseline");
            convertValue = txt::TextBaseline::kAlphabetic;
            break;
    }
    return convertValue;
}

txt::TextAlign ConvertTxtTextAlign(TextAlign textAlign)
{
    txt::TextAlign convertValue;
    switch (textAlign) {
        case TextAlign::LEFT:
            convertValue = txt::TextAlign::left;
            break;
        case TextAlign::RIGHT:
            convertValue = txt::TextAlign::right;
            break;
        case TextAlign::CENTER:
            convertValue = txt::TextAlign::center;
            break;
        case TextAlign::JUSTIFY:
            convertValue = txt::TextAlign::justify;
            break;
        case TextAlign::START:
            convertValue = txt::TextAlign::start;
            break;
        case TextAlign::END:
            convertValue = txt::TextAlign::end;
            break;
        default:
            LOGW("TextAlign setting error! Now using default TextAlign");
            convertValue = txt::TextAlign::start;
            break;
    }
    return convertValue;
}

txt::TextDirection ConvertTxtTextDirection(TextDirection textDirection)
{
    txt::TextDirection convertValue;
    switch (textDirection) {
        case TextDirection::RTL:
            convertValue = txt::TextDirection::rtl;
            break;
        case TextDirection::LTR:
            convertValue = txt::TextDirection::ltr;
            break;
        default:
            LOGW("TextDirection setting error! Now using default TextDirection");
            convertValue = txt::TextDirection::ltr;
            break;
    }
    return convertValue;
}

SkColor ConvertSkColor(Color color)
{
    return color.GetValue();
}

txt::TextDecoration ConvertTxtTextDecoration(TextDecoration textDecoration)
{
    txt::TextDecoration convertValue = txt::TextDecoration::kNone;
    switch (textDecoration) {
        case TextDecoration::NONE:
            convertValue = txt::TextDecoration::kNone;
            break;
        case TextDecoration::UNDERLINE:
            convertValue = txt::TextDecoration::kUnderline;
            break;
        case TextDecoration::OVERLINE:
            convertValue = txt::TextDecoration::kOverline;
            break;
        case TextDecoration::LINE_THROUGH:
            convertValue = txt::TextDecoration::kLineThrough;
            break;
        default:
            LOGW("TextDecoration setting error! Now using default TextDecoration");
            break;
    }
    return convertValue;
}

void ConvertTxtStyle(const TextStyle& textStyle, const WeakPtr<PipelineContext>& context, txt::TextStyle& txtStyle)
{
    txtStyle.color = ConvertSkColor(textStyle.GetTextColor());
    txtStyle.font_weight = ConvertTxtFontWeight(textStyle.GetFontWeight());
    // Font size must be px when transferring to txt::TextStyle
    auto pipelineContext = context.Upgrade();
    if (pipelineContext) {
        txtStyle.font_size = pipelineContext->NormalizeToPx(textStyle.GetFontSize());
        if (textStyle.IsAllowScale() || textStyle.GetFontSize().Unit() == DimensionUnit::FP) {
            txtStyle.font_size =
                pipelineContext->NormalizeToPx(textStyle.GetFontSize() * pipelineContext->GetFontScale());
        }
    } else {
        txtStyle.font_size = textStyle.GetFontSize().Value();
    }
    txtStyle.font_style = ConvertTxtFontStyle(textStyle.GetFontStyle());

    if (textStyle.GetWordSpacing().Unit() == DimensionUnit::PERCENT) {
        txtStyle.word_spacing = textStyle.GetWordSpacing().Value() * txtStyle.font_size;
    } else {
        if (pipelineContext) {
            txtStyle.word_spacing = pipelineContext->NormalizeToPx(textStyle.GetWordSpacing());
        } else {
            txtStyle.word_spacing = textStyle.GetWordSpacing().Value();
        }
    }
    if (pipelineContext) {
        txtStyle.letter_spacing = pipelineContext->NormalizeToPx(textStyle.GetLetterSpacing());
    }
    txtStyle.text_baseline = ConvertTxtTextBaseline(textStyle.GetTextBaseline());
    txtStyle.decoration = ConvertTxtTextDecoration(textStyle.GetTextDecoration());
    txtStyle.decoration_color = ConvertSkColor(textStyle.GetTextDecorationColor());
    txtStyle.font_families = textStyle.GetFontFamilies();
    txtStyle.locale = Localization::GetInstance()->GetFontLocale();

    txt::TextShadow txtShadow;
    const auto& spanShadow = textStyle.GetShadow();
    txtShadow.color = spanShadow.GetColor().GetValue();
    txtShadow.offset.fX = static_cast<SkScalar>(spanShadow.GetOffset().GetX());
    txtShadow.offset.fY = static_cast<SkScalar>(spanShadow.GetOffset().GetY());
    txtShadow.blur_radius = spanShadow.GetBlurRadius();
    txtStyle.text_shadows.emplace_back(txtShadow);

    if (textStyle.GetLineHeight().Unit() == DimensionUnit::PERCENT) {
        txtStyle.has_height_override = true;
        txtStyle.height = textStyle.GetLineHeight().Value();
    } else {
        double fontSize = txtStyle.font_size;
        double lineHeight = textStyle.GetLineHeight().Value();
        if (pipelineContext) {
            lineHeight = pipelineContext->NormalizeToPx(textStyle.GetLineHeight());
        }
        txtStyle.has_height_override = textStyle.HasHeightOverride();
        if (!NearEqual(lineHeight, fontSize) && (lineHeight > 0.0) && (!NearZero(fontSize))) {
            txtStyle.height = lineHeight / fontSize;
        } else {
            LOGD("use default text style height value.");
            txtStyle.height = 1;
            static const int32_t BEGIN_VERSION = 6;
            auto isBeginVersion = pipelineContext && pipelineContext->GetMinPlatformVersion() >= BEGIN_VERSION;
            if (NearZero(lineHeight) || (!isBeginVersion && NearEqual(lineHeight, fontSize))) {
                txtStyle.has_height_override = false;
            }
        }
    }

    // set font variant
    auto fontFeatures = textStyle.GetFontFeatures();
    if (!fontFeatures.empty()) {
        txt::FontFeatures features;
        for (auto iter = fontFeatures.begin(); iter != fontFeatures.end(); ++iter) {
            features.SetFeature(iter->first, iter->second);
        }
        txtStyle.font_features = features;
    }
}

Rect ConvertSkRect(SkRect skRect)
{
    Rect result;
    result.SetLeft(skRect.fLeft);
    result.SetTop(skRect.fTop);
    result.SetWidth(skRect.width());
    result.SetHeight(skRect.height());
    return result;
}

} // namespace OHOS::Ace::Constants
