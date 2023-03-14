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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_THEME_H

#include "base/geometry/size.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * VideoTheme defines color and styles of VideoComponent. VideoTheme should be built
 * using VideoTheme::Builder.
 */
class VideoTheme : public virtual Theme {
    DECLARE_ACE_TYPE(VideoTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<VideoTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<VideoTheme> theme = AceType::Claim(new VideoTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->btnSize_ = Size(themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_WIDTH).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_HEIGHT).Value());
            theme->btnEdge_ = Edge(themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_PADDING_LEFT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_PADDING_TOP).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_PADDING_RIGHT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_BTN_PADDING_BOTTOM).Value());
            theme->textEdge_ = Edge(themeConstants->GetDimension(THEME_VIDEO_BAR_TEXT_PADDING_LEFT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_TEXT_PADDING_TOP).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_TEXT_PADDING_RIGHT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_TEXT_PADDING_BOTTOM).Value());
            theme->sliderEdge_ = Edge(themeConstants->GetDimension(THEME_VIDEO_BAR_SLIDER_PADDING_LEFT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_SLIDER_PADDING_TOP).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_SLIDER_PADDING_RIGHT).Value(),
                themeConstants->GetDimension(THEME_VIDEO_BAR_SLIDER_PADDING_BOTTOM).Value());
            theme->timeTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_VIDEO_TEXT_FONTSIZE));
            theme->timeTextStyle_.SetTextColor(themeConstants->GetColor(THEME_VIDEO_BAR_TIME_TEXT_COLOR));
            theme->errorTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_VIDEO_TEXT_FONTSIZE));
            theme->errorTextStyle_.SetTextColor(themeConstants->GetColor(THEME_VIDEO_ERROR_TEXT_COLOR));
            theme->bkgColor_ = themeConstants->GetColor(THEME_VIDEO_BAR_BACKGROUND);
            return theme;
        }
    };

    ~VideoTheme() override = default;

    const Size& GetBtnSize() const
    {
        return btnSize_;
    }

    const Edge& GetBtnEdge() const
    {
        return btnEdge_;
    }

    const Edge& GetTextEdge() const
    {
        return textEdge_;
    }

    const Edge& GetSliderEdge() const
    {
        return sliderEdge_;
    }

    const TextStyle& GetTimeTextStyle() const
    {
        return timeTextStyle_;
    }

    const TextStyle& GetErrorTextStyle() const
    {
        return errorTextStyle_;
    }

    const Color& GetBkgColor() const
    {
        return bkgColor_;
    }

protected:
    VideoTheme() = default;

private:
    Size btnSize_;
    Edge btnEdge_;
    Edge textEdge_;
    Edge sliderEdge_;
    TextStyle timeTextStyle_;
    TextStyle errorTextStyle_;
    Color bkgColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_THEME_H
