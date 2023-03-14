/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#ifndef UI_TEXT_LANGUAGE_H
#define UI_TEXT_LANGUAGE_H
#include <cstdint>
namespace OHOS {
void BitmapFontInit();

enum UITextLanguageLangId : uint8_t {
    LANGUAGE_CH = 0,
    LANGUAGE_GB = 1,
    LANGUAGE_EN = 2,
    LANGUAGE_MAX = 3
}; // UITextLanguageLangId

enum UITextLanguageFontId : uint8_t {
    F_ROBOTOCONDENSED_REGULAR_14_4 = 0,
    F_ROBOTOCONDENSED_REGULAR_18_4 = 1,
    F_ROBOTOCONDENSED_REGULAR_20_4 = 2,
    F_ROBOTOCONDENSED_REGULAR_24_4 = 3,
    F_ROBOTOCONDENSED_REGULAR_26_4 = 4,
    F_ROBOTOCONDENSED_REGULAR_30_4 = 5,
    F_ROBOTOCONDENSED_REGULAR_38_4 = 6,
    F_SOURCEHANSANSSC_REGULAR_14_4 = 7,
    F_SOURCEHANSANSSC_REGULAR_18_4 = 8,
    F_SOURCEHANSANSSC_REGULAR_20_4 = 9,
    F_SOURCEHANSANSSC_REGULAR_24_4 = 10,
    F_SOURCEHANSANSSC_REGULAR_26_4 = 11,
    F_SOURCEHANSANSSC_REGULAR_30_4 = 12,
    F_SOURCEHANSANSSC_REGULAR_38_4 = 13,
    LANGUAGE_FONT_ID_MAX = 14
}; // UITextLanguageFontId

enum UITextLanguageTextId : uint16_t {
    LANGUAGE_TEXT_ID_MAX = 0
}; // UITextLanguageTextId
} // namespace OHOS
#endif // UI_TEXT_LANGUAGE_H
