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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_TEXT_STYLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_TEXT_STYLE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "base/geometry/dimension.h"
#include "base/utils/linear_map.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/shadow.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

// The normal weight is W400, the larger the number after W, the thicker the font will be.
// BOLD is equal to W700 and NORMAL is equal to W400, lighter is W100, BOLDER is W900.
enum class FontWeight {
    W100 = 0,
    W200,
    W300,
    W400,
    W500,
    W600,
    W700,
    W800,
    W900,
    BOLD,
    NORMAL,
    BOLDER,
    LIGHTER,
    MEDIUM,
    REGULAR,
};

enum class FontStyle {
    NORMAL,
    ITALIC,
};

enum class TextBaseline {
    ALPHABETIC,
    IDEOGRAPHIC,
    TOP,
    BOTTOM,
    MIDDLE,
    HANGING,
};

enum class TextCase {
    NORMAL = 0,
    LOWERCASE,
    UPPERCASE,
};

enum class WordBreak { NORMAL = 0, BREAK_ALL, BREAK_WORD };

struct TextSizeGroup {
    Dimension fontSize = 14.0_px;
    uint32_t maxLines = INT32_MAX;
    TextOverflow textOverflow = TextOverflow::CLIP;
};

class ACE_EXPORT TextStyle final {
public:
    TextStyle() = default;
    TextStyle(const std::vector<std::string>& fontFamilies, double fontSize, FontWeight fontWeight, FontStyle fontStyle,
        const Color& textColor);
    ~TextStyle() = default;

    bool operator==(const TextStyle& rhs) const;
    bool operator!=(const TextStyle& rhs) const;

    TextBaseline GetTextBaseline() const
    {
        return textBaseline_;
    }

    const Dimension& GetBaselineOffset() const
    {
        return baselineOffset_;
    }

    void SetBaselineOffset(const Dimension& baselineOffset)
    {
        baselineOffset_ = baselineOffset;
    }

    void SetTextBaseline(TextBaseline baseline)
    {
        textBaseline_ = baseline;
    }

    void SetTextDecoration(TextDecoration textDecoration)
    {
        textDecoration_ = textDecoration;
    }

    FontStyle GetFontStyle() const
    {
        return fontStyle_;
    }

    void SetFontStyle(FontStyle fontStyle)
    {
        fontStyle_ = fontStyle;
    }

    const Dimension& GetFontSize() const
    {
        return fontSize_;
    }

    WhiteSpace GetWhiteSpace() const
    {
        return whiteSpace_;
    }

    void SetWhiteSpace(WhiteSpace whiteSpace)
    {
        whiteSpace_ = whiteSpace;
    }

    void SetFontSize(const Dimension& fontSize)
    {
        fontSize_ = fontSize;
    }

    FontWeight GetFontWeight() const
    {
        return fontWeight_;
    }

    void SetFontWeight(FontWeight fontWeight)
    {
        fontWeight_ = fontWeight;
    }

    const Color& GetTextColor() const
    {
        return textColor_;
    }

    void SetTextColor(const Color& textColor)
    {
        textColor_ = textColor;
    }

    TextDecoration GetTextDecoration() const
    {
        return textDecoration_;
    }

    const Dimension& GetWordSpacing() const
    {
        return wordSpacing_;
    }

    void SetWordSpacing(const Dimension& wordSpacing)
    {
        wordSpacing_ = wordSpacing;
    }

    const Color& GetTextDecorationColor() const
    {
        return textDecorationColor_;
    }

    void SetTextDecorationColor(const Color& textDecorationColor)
    {
        textDecorationColor_ = textDecorationColor;
    }

    const std::vector<std::string>& GetFontFamilies() const
    {
        return fontFamilies_;
    }

    void SetFontFamilies(const std::vector<std::string>& fontFamilies)
    {
        fontFamilies_ = fontFamilies;
    }

    Dimension GetTextIndent() const
    {
        return textIndent_;
    }

    void SetTextIndent(const Dimension& textIndent)
    {
        textIndent_ = textIndent;
    }

    const std::unordered_map<std::string, int32_t>& GetFontFeatures() const
    {
        return fontFeatures_;
    }

    void SetFontFeatures(const std::unordered_map<std::string, int32_t>& fontFeatures)
    {
        fontFeatures_ = fontFeatures;
    }

    const Dimension& GetLineHeight() const
    {
        return lineHeight_;
    }

    void SetLineHeight(const Dimension& lineHeight, bool hasHeightOverride = true)
    {
        lineHeight_ = lineHeight;
        hasHeightOverride_ = hasHeightOverride;
    }

    bool HasHeightOverride() const
    {
        return hasHeightOverride_;
    }

    const Shadow& GetShadow() const
    {
        return shadow_;
    }

    void SetShadow(const Shadow& shadow)
    {
        shadow_ = shadow;
    }

    const Dimension& GetLetterSpacing() const
    {
        return letterSpacing_;
    }

    void SetLetterSpacing(const Dimension& letterSpacing)
    {
        letterSpacing_ = letterSpacing;
    }

    bool GetAdaptTextSize() const
    {
        return adaptTextSize_;
    }

    void SetAdaptTextSize(
        const Dimension& maxFontSize, const Dimension& minFontSize, const Dimension& fontSizeStep = 1.0_px);

    bool GetAdaptHeight() const
    {
        return adaptHeight_;
    }

    void SetAdaptHeight(bool adaptHeight)
    {
        adaptHeight_ = adaptHeight;
    }

    void DisableAdaptTextSize()
    {
        adaptTextSize_ = false;
    }

    uint32_t GetMaxLines() const
    {
        return maxLines_;
    }

    void SetMaxLines(uint32_t maxLines)
    {
        maxLines_ = maxLines;
    }

    void SetPreferFontSizes(const std::vector<Dimension>& preferFontSizes)
    {
        preferFontSizes_ = preferFontSizes;
        adaptTextSize_ = true;
    }

    const std::vector<Dimension>& GetPreferFontSizes() const
    {
        return preferFontSizes_;
    }

    // Must use with SetAdaptMinFontSize and SetAdaptMaxFontSize.
    void SetAdaptFontSizeStep(const Dimension& adaptTextSizeStep)
    {
        adaptFontSizeStep_ = adaptTextSizeStep;
    }
    // Must use with SetAdaptMaxFontSize.
    void SetAdaptMinFontSize(const Dimension& adaptMinFontSize)
    {
        adaptMinFontSize_ = adaptMinFontSize;
        adaptTextSize_ = true;
    }
    // Must use with SetAdaptMinFontSize.
    void SetAdaptMaxFontSize(const Dimension& adaptMaxFontSize)
    {
        adaptMaxFontSize_ = adaptMaxFontSize;
        adaptTextSize_ = true;
    }

    const Dimension& GetAdaptFontSizeStep() const
    {
        return adaptFontSizeStep_;
    }

    const Dimension& GetAdaptMinFontSize() const
    {
        return adaptMinFontSize_;
    }

    const Dimension& GetAdaptMaxFontSize() const
    {
        return adaptMaxFontSize_;
    }

    const std::vector<TextSizeGroup>& GetPreferTextSizeGroups() const
    {
        return preferTextSizeGroups_;
    }
    void SetPreferTextSizeGroups(const std::vector<TextSizeGroup>& preferTextSizeGroups)
    {
        preferTextSizeGroups_ = preferTextSizeGroups;
        adaptTextSize_ = true;
    }

    bool IsAllowScale() const
    {
        return allowScale_;
    }

    void SetAllowScale(bool allowScale)
    {
        allowScale_ = allowScale;
    }

    TextOverflow GetTextOverflow() const
    {
        return textOverflow_;
    }
    void SetTextOverflow(TextOverflow textOverflow)
    {
        textOverflow_ = textOverflow;
    }
    TextAlign GetTextAlign() const
    {
        return textAlign_;
    }
    void SetTextAlign(TextAlign textAlign)
    {
        textAlign_ = textAlign;
    }
    void SetTextVerticalAlign(VerticalAlign verticalAlign)
    {
        verticalAlign_ = verticalAlign;
    }

    VerticalAlign GetTextVerticalAlign() const
    {
        return verticalAlign_;
    }

    WordBreak GetWordBreak() const
    {
        return wordBreak_;
    }

    void SetWordBreak(WordBreak wordBreak)
    {
        wordBreak_ = wordBreak;
    }

    TextCase GetTextCase() const
    {
        return textCase_;
    }

    void SetTextCase(TextCase textCase)
    {
        textCase_ = textCase;
    }

private:
    std::vector<std::string> fontFamilies_;
    std::unordered_map<std::string, int32_t> fontFeatures_;
    std::vector<Dimension> preferFontSizes_;
    std::vector<TextSizeGroup> preferTextSizeGroups_;
    // use 14px for normal font size.
    Dimension fontSize_ { 14, DimensionUnit::PX };
    Dimension adaptMinFontSize_;
    Dimension adaptMaxFontSize_;
    Dimension adaptFontSizeStep_;
    Dimension lineHeight_;
    bool hasHeightOverride_ = false;
    FontWeight fontWeight_ { FontWeight::NORMAL };
    FontStyle fontStyle_ { FontStyle::NORMAL };
    TextBaseline textBaseline_ { TextBaseline::ALPHABETIC };
    Dimension baselineOffset_;
    TextOverflow textOverflow_ { TextOverflow::CLIP };
    VerticalAlign verticalAlign_ { VerticalAlign::NONE };
    TextAlign textAlign_ { TextAlign::START };
    Color textColor_ { Color::BLACK };
    Color textDecorationColor_ { Color::TRANSPARENT };
    TextDecoration textDecoration_ { TextDecoration::NONE };
    Shadow shadow_;
    WhiteSpace whiteSpace_{ WhiteSpace::PRE };
    Dimension wordSpacing_;
    Dimension textIndent_ { 0.0f, DimensionUnit::PX };
    Dimension letterSpacing_;
    uint32_t maxLines_ = UINT32_MAX;
    bool adaptTextSize_ = false;
    bool adaptHeight_ = false; // whether adjust text size with height.
    bool allowScale_ = true;
    WordBreak wordBreak_ { WordBreak::BREAK_WORD };
    TextCase textCase_ { TextCase::NORMAL };
};

namespace StringUtils {

inline FontWeight StringToFontWeight(const std::string& weight)
{
    static const LinearMapNode<FontWeight> fontWeightTable[] = {
        { "100", FontWeight::W100 },
        { "200", FontWeight::W200 },
        { "300", FontWeight::W300 },
        { "400", FontWeight::W400 },
        { "500", FontWeight::W500 },
        { "600", FontWeight::W600 },
        { "700", FontWeight::W700 },
        { "800", FontWeight::W800 },
        { "900", FontWeight::W900 },
        { "bold", FontWeight::BOLD },
        { "bolder", FontWeight::BOLDER },
        { "lighter", FontWeight::LIGHTER },
        { "medium", FontWeight::MEDIUM },
        { "regular", FontWeight::REGULAR },
    };
    auto weightIter = BinarySearchFindIndex(fontWeightTable, ArraySize(fontWeightTable), weight.c_str());
    return weightIter != -1 ? fontWeightTable[weightIter].value : FontWeight::NORMAL;
}

inline WordBreak StringToWordBreak(const std::string& wordBreak)
{
    static const LinearMapNode<WordBreak> wordBreakTable[] = {
        { "break-all", WordBreak::BREAK_ALL },
        { "break-word", WordBreak::BREAK_WORD },
        { "normal", WordBreak::NORMAL },
    };
    auto wordBreakIter = BinarySearchFindIndex(wordBreakTable, ArraySize(wordBreakTable), wordBreak.c_str());
    return wordBreakIter != -1 ? wordBreakTable[wordBreakIter].value : WordBreak::BREAK_WORD;
}

} // namespace StringUtils
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_TEXT_STYLE_H
