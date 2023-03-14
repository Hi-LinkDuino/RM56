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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ListItemTheme defines styles of list or grid item. ListItemTheme should be built
 * using ListItemTheme::Builder.
 */
class ListItemTheme : public virtual Theme {
DECLARE_ACE_TYPE(ListItemTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ListItemTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ListItemTheme> theme = AceType::Claim(new ListItemTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->clickColor_ = themeConstants->GetColor(THEME_ITEM_CLICK_COLOR);
            theme->clickAlphaBegin_ = themeConstants->GetDouble(THEME_ITEM_CLICK_ALPHA_BEGIN);
            theme->clickAlphaEnd_ = themeConstants->GetDouble(THEME_ITEM_CLICK_ALPHA_END);
            theme->clickScale_ = themeConstants->GetDouble(THEME_ITEM_CLICK_SCALE);
            theme->focusScale_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_SCALE);
            theme->focusScaleLarge_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_SCALE_LARGE);
            theme->focusScaleMiddle_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_SCALE_MIDDLE);
            theme->focusScaleLittle_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_SCALE_LITTLE);
            theme->focusOpacityLarge_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_OPACITY_LARGE);
            theme->focusOpacityMiddle_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_OPACITY_MIDDLE);
            theme->focusOpacityLittle_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_OPACITY_LITTLE);
            theme->focusAnimationDuration_ = themeConstants->GetDouble(THEME_ITEM_FOCUS_ANIMATION_DURATION);
            theme->clickAnimationDuration_ = themeConstants->GetDouble(THEME_ITEM_CLICK_ANIMATION_DURATION);
            theme->itemSize_ = themeConstants->GetDimension(THEME_ITEM_SIZE);
            theme->paddingInPercent_ = themeConstants->GetDouble(THEME_ITEM_PADDING_IN_PERCENT);
            theme->groupImageSize_ = themeConstants->GetDimension(THEME_ITEM_GROUP_IMAGE_SIZE);
            return theme;
        }
    };

    ~ListItemTheme() override = default;

    const Color& GetClickColor() const
    {
        return clickColor_;
    }
    double GetClickAlphaBegin() const
    {
        return clickAlphaBegin_;
    }
    double GetClickAlphaEnd() const
    {
        return clickAlphaEnd_;
    }
    double GetClickScale() const
    {
        return clickScale_;
    }
    double GetFocusScale() const
    {
        return focusScale_;
    }
    double GetFocusScaleLarge() const
    {
        return focusScaleLarge_;
    }
    double GetFocusScaleMiddle() const
    {
        return focusScaleMiddle_;
    }
    double GetFocusScaleLittle() const
    {
        return focusScaleLittle_;
    }
    double GetFocusOpacityLarge() const
    {
        return focusOpacityLarge_;
    }
    double GetFocusOpacityMiddle() const
    {
        return focusOpacityMiddle_;
    }
    double GetFocusOpacityLittle() const
    {
        return focusOpacityLittle_;
    }
    double GetFocusAnimationDuration() const
    {
        return focusAnimationDuration_;
    }
    double GetClickAnimationDuration() const
    {
        return clickAnimationDuration_;
    }
    Dimension GetItemSize() const
    {
        return itemSize_;
    }
    double GetItemPaddingInPercent() const
    {
        return paddingInPercent_;
    }
    Dimension GetGroupImageSize() const
    {
        return groupImageSize_;
    }

protected:
    ListItemTheme() = default;

private:
    Color clickColor_;
    double clickAlphaBegin_ = 1.0;
    double clickAlphaEnd_ = 1.0;
    double clickScale_ = 1.0;
    double focusScale_ = 1.0;
    double focusScaleLarge_ = 1.0;
    double focusScaleMiddle_ = 1.0;
    double focusScaleLittle_ = 1.0;
    double focusOpacityLarge_ = 1.0;
    double focusOpacityMiddle_ = 1.0;
    double focusOpacityLittle_ = 1.0;
    double focusAnimationDuration_ = 100.0;
    double clickAnimationDuration_ = 200.0;
    double paddingInPercent_ = 0.0;
    Dimension itemSize_;
    Dimension groupImageSize_;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_THEME_H
