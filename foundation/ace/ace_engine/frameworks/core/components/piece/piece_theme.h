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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_PIECE_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_PIECE_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * PieceTheme defines default style of DOMPiece,using PieceTheme::Builder to build DOMPiece.
 */
class PieceTheme : public virtual Theme {
    DECLARE_ACE_TYPE(PieceTheme, Theme);

public:
    class Builder final {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<PieceTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<PieceTheme> theme = AceType::Claim(new PieceTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->height_ = themeConstants->GetDimension(THEME_PIECE_HEIGHT);
            theme->textStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            auto maxlines = themeConstants->GetInt(THEME_PIECE_TEXT_LINES);
            theme->textStyle_.SetMaxLines(maxlines < 0 ? UINT32_MAX : maxlines);
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_PIECE_TEXT_COLOR));
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_PIECE_FONT_SIZE));
            theme->textStyle_.SetFontWeight(FontWeight(themeConstants->GetInt(THEME_PIECE_FONT_WEIGHT)));
            theme->hoverColor_ = themeConstants->GetColor(THEME_PIECE_HOVER_COLOR);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_PIECE_BACKGROUND_COLOR);
            theme->paddingHorizontal_ = themeConstants->GetDimension(THEME_PIECE_PADDING_HORIZONTAL);
            theme->paddingVertical_ = themeConstants->GetDimension(THEME_PIECE_PADDING_VERTICAL);
            theme->iconResource_ = themeConstants->GetResourceId(THEME_PIECE_ICON_SOURCE);
            theme->iconSize_ = themeConstants->GetDimension(THEME_PIECE_ICON_SIZE);
            theme->interval_ = themeConstants->GetDimension(THEME_PIECE_INTERVAL);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<PieceTheme>& theme) const
        {
            if (!themeStyle || !theme) {
                return;
            }
            theme->textStyle_.SetTextColor(themeStyle->GetAttr<Color>(PIECE_TEXT_COLOR, Color()));
            theme->textStyle_.SetFontSize(themeStyle->GetAttr<Dimension>(PIECE_TEXT_FONT_SIZE, 0.0_fp));
            theme->backgroundColor_ = themeStyle->GetAttr<Color>(PIECE_BACKGROUND_COLOR, Color())
                                          .BlendOpacity(themeStyle->GetAttr<double>(PIECE_BACKGROUND_COLOR_ALPHA, 0.0));
            theme->hoverColor_ = themeStyle->GetAttr<Color>(PIECE_HOVER_COLOR, Color());
        }
    };

    ~PieceTheme() override = default;
    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }
    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }
    const Dimension& GetHeight() const
    {
        return height_;
    }
    const Dimension& GetPaddingVertical() const
    {
        return paddingVertical_;
    }
    const Dimension& GetPaddingHorizontal() const
    {
        return paddingHorizontal_;
    }
    const Dimension& GetInterval() const
    {
        return interval_;
    }
    InternalResource::ResourceId GetIconResource() const
    {
        return iconResource_;
    }
    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }
    const Dimension& GetIconSize() const
    {
        return iconSize_;
    }

protected:
    PieceTheme() = default;

private:
    Color hoverColor_;
    Color backgroundColor_;
    Dimension height_;
    Dimension interval_;
    Dimension iconSize_;
    Dimension paddingVertical_;
    Dimension paddingHorizontal_;
    TextStyle textStyle_;
    InternalResource::ResourceId iconResource_ = InternalResource::ResourceId::NO_ID;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_PIECE_THEME_H
