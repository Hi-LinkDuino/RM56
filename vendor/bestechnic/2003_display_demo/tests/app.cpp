/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#if defined(UI_TEST) || defined(ABILITY_TEST)
#include "ohos_init.h"
#include "ui_main.h"
#if defined(UI_TEST)
#include "ui_test.h"
#elif defined(ABILITY_TEST)
#include "ability_test.h"
#endif

#if defined(HISTREAMER_TEST)
#include "audio_player.h"
#endif

/* ui app entry */
void RunApp()
{
#ifdef UI_TEST
    // AnimatorDemoStart();
    // UiDemoStart();
    // ImageDemoStart();
    ViewDemoStart();
#elif defined(ABILITY_TEST)
    // StartLauncherApp();
    StartJSApp();
#endif
}

void AppEntry(void)
{
    UiMain();
#if defined(HISTREAMER_TEST)
    AudioPlayerStart();
#endif
}

APP_FEATURE_INIT(AppEntry);
#endif
