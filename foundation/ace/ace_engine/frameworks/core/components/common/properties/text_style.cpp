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

#include "core/components/common/properties/text_style.h"

#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

TextStyle::TextStyle(const std::vector<std::string>& fontFamilies, double fontSize, FontWeight fontWeight,
    FontStyle fontStyle, const Color& textColor)
    : fontFamilies_(fontFamilies), fontSize_(fontSize), fontWeight_(fontWeight), fontStyle_(fontStyle),
      textColor_(textColor)
{}

bool TextStyle::operator==(const TextStyle& rhs) const
{
    return fontFamilies_ == rhs.fontFamilies_ && fontFeatures_ == rhs.fontFeatures_ &&
           preferFontSizes_ == rhs.preferFontSizes_ && fontSize_ == rhs.fontSize_ &&
           adaptMinFontSize_ == rhs.adaptMinFontSize_ && adaptMaxFontSize_ == rhs.adaptMaxFontSize_ &&
           adaptFontSizeStep_ == rhs.adaptFontSizeStep_ && lineHeight_ == rhs.lineHeight_ &&
           fontWeight_ == rhs.fontWeight_ && fontStyle_ == rhs.fontStyle_ && textBaseline_ == rhs.textBaseline_ &&
           textOverflow_ == rhs.textOverflow_ && textAlign_ == rhs.textAlign_ && textColor_ == rhs.textColor_ &&
           textDecoration_ == rhs.textDecoration_ && shadow_ == rhs.shadow_ && wordSpacing_ == rhs.wordSpacing_ &&
           letterSpacing_ == rhs.letterSpacing_ && maxLines_ == rhs.maxLines_ && adaptTextSize_ == rhs.adaptTextSize_ &&
           allowScale_ == rhs.allowScale_ && wordBreak_ == rhs.wordBreak_ &&
           textDecorationColor_ == rhs.textDecorationColor_ && textCase_ == rhs.textCase_ &&
           baselineOffset_ == rhs.baselineOffset_ && adaptHeight_ == rhs.adaptHeight_ &&
           textIndent_ == rhs.textIndent_ && verticalAlign_ == rhs.verticalAlign_;
}

bool TextStyle::operator!=(const TextStyle& rhs) const
{
    return !(rhs == *this);
}

void TextStyle::SetAdaptTextSize(
    const Dimension& maxFontSize, const Dimension& minFontSize, const Dimension& fontSizeStep)
{
    adaptMaxFontSize_ = maxFontSize;
    adaptMinFontSize_ = minFontSize;
    adaptFontSizeStep_ = fontSizeStep;
    adaptTextSize_ = true;
}

} // namespace OHOS::Ace
