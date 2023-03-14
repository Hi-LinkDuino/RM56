/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CHARGER_UI_UPDATER_UI_H
#define CHARGER_UI_UPDATER_UI_H

#include "view.h"
#include "text_label.h"
#include "animation_label.h"
#include "frame.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
    struct FocusInfo {
        bool focus;
        bool focusable;
    };

    struct Bold {
        bool top;
        bool bottom;
    };

void MenuItemInit(int32_t height, int32_t width, View::BRGA888Pixel bgColor, Frame* gHosFrame);
void TextLabelInit(TextLabel* t, const std::string& text, struct Bold bold, struct FocusInfo focus,
    View::BRGA888Pixel color);
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_UPDATER_UI_H
