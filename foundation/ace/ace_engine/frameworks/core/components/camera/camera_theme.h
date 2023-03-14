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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_THEME_H

#include "base/geometry/size.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/*
 * CameraTheme defines color and styles of Camera. CameraTheme should be built
 * using CameraTheme::Builder.
 */
class CameraTheme : public virtual Theme {
    DECLARE_ACE_TYPE(CameraTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<CameraTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<CameraTheme> theme = AceType::Claim(new CameraTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->bufferSize_ = Size(themeConstants->GetDimension(THEME_CAMERA_BUFFER_WIDTH).Value(),
                themeConstants->GetDimension(THEME_CAMERA_BUFFER_HEIGHT).Value());
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
            theme->errorTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_VIDEO_TEXT_FONTSIZE));
            theme->errorTextStyle_.SetTextColor(themeConstants->GetColor(THEME_VIDEO_ERROR_TEXT_COLOR));
#endif
            return theme;
        }
    };

    ~CameraTheme() override = default;

    const Size& GetBufferSize() const
    {
        return bufferSize_;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    const TextStyle& GetErrorTextStyle() const
    {
        return errorTextStyle_;
    }
#endif

protected:
    CameraTheme() = default;

private:
    Size bufferSize_;

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    TextStyle errorTextStyle_;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_THEME_H
