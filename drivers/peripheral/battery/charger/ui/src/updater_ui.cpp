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

#include "updater_ui.h"
#include <cstdio>
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#elif defined(HAVE_TERMIO_H)
#include <termio.h>
#endif

#include "securec.h"
#include "surface_dev.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
constexpr int32_t LABEL_HEIGHT = 15;
constexpr int32_t LABEL0_OFFSET = 0;
constexpr int32_t LABEL2_OFFSET = 1;

void TextLabelInit(TextLabel* t, const std::string& text, struct Bold bold,
    struct FocusInfo focus, View::BRGA888Pixel color)
{
    if (t != nullptr) {
        t->SetText(text.c_str());
        t->SetOutLineBold(bold.top, bold.bottom);
        t->OnFocus(focus.focus);
        t->SetBackgroundColor(&color);
        t->SetFocusAble(focus.focusable);
    }
}

void MenuItemInit(int32_t height, int32_t width, View::BRGA888Pixel bgColor, Frame* gHosFrame)
{
    TextLabel* gTextLabel0 = nullptr;
    TextLabel* gTextLabel2 = nullptr;

    if (gHosFrame == nullptr) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Frame is nullptr.");
        return;
    }
    gTextLabel0 = new TextLabel(0, height * LABEL0_OFFSET / LABEL_HEIGHT, width, height /
        LABEL_HEIGHT, gHosFrame);
    struct FocusInfo info {true, true};
    struct Bold bold {true, false};
    TextLabelInit(gTextLabel0, "Reboot to normal system", bold, info, bgColor);
    if (!gTextLabel0) {
        BATTERY_HILOGD(FEATURE_CHARGING, "gTextLabel0 is nullptr.");
        return;
    }

    gTextLabel2 = new TextLabel(0, height * LABEL2_OFFSET / LABEL_HEIGHT, width, height /
        LABEL_HEIGHT, gHosFrame);
    info = {false, true};
    bold = {false, false};
    TextLabelInit(gTextLabel2, "Userdata reset", bold, info, bgColor);
    if (!gTextLabel2) {
        BATTERY_HILOGD(FEATURE_CHARGING, "gTextLabel2 is nullptr.");
        return;
    }
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
