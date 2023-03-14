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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_QRCODE_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_QRCODE_THEME_H

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace {

class QrcodeTheme : public virtual Theme {
    DECLARE_ACE_TYPE(QrcodeTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<QrcodeTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<QrcodeTheme> theme = AceType::Claim(new QrcodeTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->qrcodeColor_ = themeConstants->GetColor(THEME_QRCODE_COLOR);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_QRCODE_BACKGROUND_COLOR);
            theme->qrcodeType_ = QrcodeType(themeConstants->GetInt(THEME_QRCODE_TYPE));
            theme->qrcodeWidth_ = themeConstants->GetDimension(THEME_QRCODE_SIZE);
            theme->qrcodeHeight_ = themeConstants->GetDimension(THEME_QRCODE_SIZE);
            return theme;
        }
    };

    ~QrcodeTheme() override = default;

    const Color& GetQrcodeColor() const
    {
        return qrcodeColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    QrcodeType GetQrcodeType() const
    {
        return qrcodeType_;
    }

    const Dimension& GetQrcodeWidth() const
    {
        return qrcodeWidth_;
    }

    const Dimension& GetQrcodeHeight() const
    {
        return qrcodeHeight_;
    }

protected:
    QrcodeTheme() = default;

private:
    Color qrcodeColor_;
    Color backgroundColor_;
    QrcodeType qrcodeType_ { QrcodeType::RECT };
    Dimension qrcodeWidth_;
    Dimension qrcodeHeight_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_QRCODE_THEME_H