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
#include "ui_text_language.h"
#include "font/ui_font_builder.h"
#include "font/ui_font_header.h"

namespace OHOS {
const UITextLanguageFontParam g_uiTextLangFontsTable[LANGUAGE_FONT_ID_MAX] = {
    {14, 4, 0, 0, "RobotoCondensed-Regular"},
    {18, 4, 0, 0, "RobotoCondensed-Regular"},
    {20, 4, 0, 0, "RobotoCondensed-Regular"},
    {24, 4, 0, 0, "RobotoCondensed-Regular"},
    {26, 4, 0, 0, "RobotoCondensed-Regular"},
    {30, 4, 0, 0, "RobotoCondensed-Regular"},
    {38, 4, 0, 0, "RobotoCondensed-Regular"},
    {14, 4, 0, 1, "SourceHanSansSC-Regular"},
    {18, 4, 0, 1, "SourceHanSansSC-Regular"},
    {20, 4, 0, 1, "SourceHanSansSC-Regular"},
    {24, 4, 0, 1, "SourceHanSansSC-Regular"},
    {26, 4, 0, 1, "SourceHanSansSC-Regular"},
    {30, 4, 0, 1, "SourceHanSansSC-Regular"},
    {38, 4, 0, 1, "SourceHanSansSC-Regular"},
}; // g_uiTextLangFontsTable

const LangTextParam g_langTextDefaultParamTable[LANGUAGE_MAX] = {
    {0, 30, "SourceHanSansSC-Regular", "zh"},
    {0, 30, "RobotoCondensed-Regular", "en"},
    {0, 30, "RobotoCondensed-Regular", "en"},
}; // g_langTextDefaultParamTable

void BitmapFontInit()
{
    UIFontBuilder* uiFontBuilder = UIFontBuilder::GetInstance();
    if (uiFontBuilder == nullptr) {
        return;
    }
    uiFontBuilder->SetTextLangFontsTable(g_uiTextLangFontsTable, LANGUAGE_FONT_ID_MAX);
    uiFontBuilder->SetLangTextDefaultParamTable(g_langTextDefaultParamTable, LANGUAGE_MAX);
    uiFontBuilder->SetMaxTextId(LANGUAGE_TEXT_ID_MAX);
}
} // namespace OHOS
