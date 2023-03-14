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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {
namespace {
constexpr uint32_t SHOW_TIME = 250; // unit is ms.
constexpr uint32_t HIDE_TIME = 250; // unit is ms.
} // namespace

/**
 * PopupTheme defines color and styles of PopupComponent. PopupTheme should be built
 * using PopupTheme::Builder.
 */
class PopupTheme : public virtual Theme {
    DECLARE_ACE_TYPE(PopupTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<PopupTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<PopupTheme> theme = AceType::Claim(new PopupTheme());
            if (!themeConstants) {
                return theme;
            }
            // init theme from global data
            theme->padding_ = Edge(themeConstants->GetDimension(THEME_POPUP_PADDING_HORIZONTAL),
                themeConstants->GetDimension(THEME_POPUP_PADDING_VERTICAL),
                themeConstants->GetDimension(THEME_POPUP_PADDING_HORIZONTAL),
                themeConstants->GetDimension(THEME_POPUP_PADDING_VERTICAL));
            theme->maskColor_ = themeConstants->GetColor(THEME_POPUP_MASK_COLOR);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_POPUP_BACKGROUND_COLOR);
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_POPUP_TEXT_COLOR));
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_POPUP_TEXT_FONTSIZE));
            theme->radius_ = Radius(themeConstants->GetDimension(THEME_POPUP_RADIUS),
                themeConstants->GetDimension(THEME_POPUP_RADIUS));
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            theme->showTime_ = SHOW_TIME;
            theme->hideTime_ = HIDE_TIME;
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<PopupTheme>& theme) const
        {
            if (!themeStyle || !theme) {
                return;
            }
            theme->backgroundColor_ = themeStyle->GetAttr<Color>(POPUP_BACKGROUND_COLOR, Color());
        }
    };

    ~PopupTheme() override = default;

    const Edge& GetPadding() const
    {
        return padding_;
    }

    const Color& GetMaskColor() const
    {
        return maskColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    const Radius& GetRadius() const
    {
        return radius_;
    }

    uint32_t GetShowTime() const
    {
        return showTime_;
    }

    uint32_t GetHideTime() const
    {
        return hideTime_;
    }

protected:
    PopupTheme() = default;

private:
    Edge padding_;
    Color maskColor_;
    Color backgroundColor_;
    TextStyle textStyle_;
    Radius radius_;
    uint32_t showTime_ = 0;
    uint32_t hideTime_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_THEME_H
