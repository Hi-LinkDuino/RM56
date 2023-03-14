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

#include "font/ui_font_builder.h"
#include "font/ui_font.h"

namespace OHOS {
UIFontBuilder::UIFontBuilder() : uiTextLangFontsTable_(nullptr), langTextDefaultParamTable_(nullptr),
                                 totalLangId_(0), totalFontId_(0), totalTextId_(0) {}

UIFontBuilder* UIFontBuilder::GetInstance()
{
    static UIFontBuilder uiFontBuilder;
    return &uiFontBuilder;
}

void UIFontBuilder::SetTextLangFontsTable(const UITextLanguageFontParam* uiTextLangFontsTable,
                                          uint8_t totalFontId)
{
    if ((uiTextLangFontsTable != nullptr) && (totalFontId > 0)) {
        uiTextLangFontsTable_ = const_cast<UITextLanguageFontParam*>(uiTextLangFontsTable);
        totalFontId_ = totalFontId;
    }
}

void UIFontBuilder::SetLangTextDefaultParamTable(const LangTextParam* langTextDefaultParamTable,
                                                 uint8_t totalLangId)
{
    if ((langTextDefaultParamTable != nullptr) && (totalLangId > 0)) {
        langTextDefaultParamTable_ = const_cast<LangTextParam*>(langTextDefaultParamTable);
        totalLangId_ = totalLangId;
    }
}

void UIFontBuilder::SetMaxTextId(uint16_t totalTextId)
{
    totalTextId_ = totalTextId;
}

UITextLanguageFontParam* UIFontBuilder::GetTextLangFontsTable(uint8_t langFontId)
{
    if ((langFontId >= totalFontId_) || (uiTextLangFontsTable_ == nullptr)) {
        return nullptr;
    }
    return &(uiTextLangFontsTable_[langFontId]);
}

uint8_t UIFontBuilder::GetTotalLangId() const
{
    return totalLangId_;
}

uint8_t UIFontBuilder::GetTotalFontId() const
{
    uint8_t fontIdMax = 0xFF;
    if (!UIFont::GetInstance()->IsVectorFont()) {
        fontIdMax = totalFontId_;
    }
    return fontIdMax;
}

uint8_t UIFontBuilder::GetBitmapFontIdMax() const
{
    return totalFontId_;
}

uint16_t UIFontBuilder::GetTotalTextId() const
{
    return totalTextId_;
}

LangTextParam* UIFontBuilder::GetLangTextDefaultParamTable()
{
    if (langTextDefaultParamTable_ == nullptr) {
        return nullptr;
    }
    return langTextDefaultParamTable_;
}
} // namespace OHOS
