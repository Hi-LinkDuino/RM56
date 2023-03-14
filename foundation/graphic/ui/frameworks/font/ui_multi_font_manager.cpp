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

#include "font/ui_multi_font_manager.h"
#include "common/typed_text.h"
#include "font/ui_font.h"

#include "gfx_utils/mem_api.h"
#include "securec.h"
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif
#if ENABLE_MULTI_FONT
namespace OHOS {
UIMultiFontManager::UIMultiFontManager()
    : arbicTtfId_(0), thaiTtfId_(0), myanmarTtfId_(0),
      devanagariTtfId_(0), hebrewTtfId_(0),
      bengaliTtfId_(0), topIndex_(0)
{
    const UITextLanguageFontParam* fontParam = nullptr;
    uint8_t totalFontId = UIFontBuilder::GetInstance()->GetTotalFontId();
    for (uint8_t i = 0; i < totalFontId; i++) {
        fontParam = UIFont::GetInstance()->GetFontInfo(i);
        if (fontParam == nullptr) {
            continue;
        }
        if (!fontParam->shaping) {
            continue;
        }
        if (strstr(fontParam->ttfName, ARABIC_LANG) != nullptr) {
            arbicTtfId_ = fontParam->ttfId;
        } else if (strstr(fontParam->ttfName, THAI_LANG) != nullptr) {
            thaiTtfId_ = fontParam->ttfId;
        } else if (strstr(fontParam->ttfName, MYAN_LANG) != nullptr) {
            myanmarTtfId_ = fontParam->ttfId;
        } else if (strstr(fontParam->ttfName, DVCARI_LANG) != nullptr) {
            devanagariTtfId_ = fontParam->ttfId;
        } else if (strstr(fontParam->ttfName, HBREW_LANG) != nullptr) {
            hebrewTtfId_ = fontParam->ttfId;
        } else if (strstr(fontParam->ttfName, BENGALI_LANG) != nullptr) {
            bengaliTtfId_ = fontParam->ttfId;
        }
    }
    fontIdIndex_ = static_cast<uint8_t*>(UIMalloc(totalFontId));
    if (fontIdIndex_ == nullptr) {
        return;
    }
    for (uint8_t index = 0; index < totalFontId; index++) {
        fontIdIndex_[index] = MAX_LIST_NUM;
    }
    for (uint8_t index = 0; index < MAX_LIST_NUM; index++) {
        fontNodes_[index].fontIds = nullptr;
        fontNodes_[index].size = 0;
    }
}

UIMultiFontManager::~UIMultiFontManager()
{
    UIFree(fontIdIndex_);
    fontIdIndex_ = nullptr;
}


int8_t UIMultiFontManager::AddNewFont(uint8_t fontListId, uint8_t *fontIds, int8_t size, uint8_t fontId)
{
    fontNodes_[fontId].fontIds = static_cast<uint8_t *>(UIMalloc(size));
    if (fontNodes_[fontId].fontIds == nullptr) {
        return fontId;
    }
    fontIdIndex_[fontListId] = fontId;
    (void)memcpy_s(fontNodes_[fontId].fontIds, size, fontIds, size);
    fontNodes_[fontId].size = size;
    return fontId + 1;
}

int8_t UIMultiFontManager::UpdateFont(uint8_t fontListId, uint8_t *fontIds, uint8_t size)
{
    uint8_t index = fontIdIndex_[fontListId];
    if (index < topIndex_) {
        UIFree(fontNodes_[index].fontIds);
        fontNodes_[index].fontIds = nullptr;
    } else {
        index = topIndex_;
    }

    uint8_t nextIndex = AddNewFont(fontListId, fontIds, size, index);
    if (topIndex_ < nextIndex) {
        topIndex_ = nextIndex;
    }
    return (fontNodes_[index].fontIds == nullptr) ? INVALID_RET_VALUE : RET_VALUE_OK;
}

void UIMultiFontManager::UpdateScript(UITextLanguageFontParam& fonts)
{
    if (strstr(fonts.ttfName, ARABIC_LANG) != nullptr) {
        arbicTtfId_ = fonts.ttfId;
    } else if (strstr(fonts.ttfName, THAI_LANG) != nullptr) {
        thaiTtfId_ = fonts.ttfId;
    } else if (strstr(fonts.ttfName, MYAN_LANG) != nullptr) {
        myanmarTtfId_ = fonts.ttfId;
    } else if (strstr(fonts.ttfName, DVCARI_LANG) != nullptr) {
        devanagariTtfId_ = fonts.ttfId;
    } else if (strstr(fonts.ttfName, HBREW_LANG) != nullptr) {
        hebrewTtfId_ = fonts.ttfId;
    } else if (strstr(fonts.ttfName, BENGALI_LANG) != nullptr) {
        bengaliTtfId_ = fonts.ttfId;
    }
}

UIMultiFontManager* UIMultiFontManager::GetInstance()
{
    static UIMultiFontManager instance;
    return &instance;
}

void UIMultiFontManager::ClearSearchFontList()
{
    uint8_t totalFontId = UIFontBuilder::GetInstance()->GetTotalFontId();
    for (uint8_t index = 0; index < totalFontId; index++) {
        fontIdIndex_[index] = MAX_LIST_NUM;
    }

    for (auto &node : fontNodes_) {
        UIFree(node.fontIds);
        node.fontIds = nullptr;
        node.size = 0;
    }
    topIndex_ = 0;
}

int8_t UIMultiFontManager::SetSearchFontList(uint8_t fontListId, uint8_t *fontIds, uint8_t size)
{
    if ((fontListId >= UIFontBuilder::GetInstance()->GetTotalFontId()) || (fontIds == nullptr) || (size == 0) ||
        (fontIdIndex_ == nullptr) || (topIndex_ >= MAX_LIST_NUM)) {
        return INVALID_RET_VALUE;
    }
    // update
    return UpdateFont(fontListId, fontIds, size);
}

int8_t UIMultiFontManager::GetSearchFontList(uint8_t fontListId, uint8_t **fontIds)
{
    if ((fontListId >= UIFontBuilder::GetInstance()->GetTotalFontId()) || (fontIds == nullptr) ||
        (fontIdIndex_ == nullptr) || (fontIdIndex_[fontListId] >= MAX_LIST_NUM)) {
        return INVALID_RET_VALUE;
    }
    *fontIds = fontNodes_[fontIdIndex_[fontListId]].fontIds;
    return fontNodes_[fontIdIndex_[fontListId]].size;
}

bool UIMultiFontManager::IsNeedShaping(const char *text, uint8_t &ttfId, uint32_t &script)
{
    if (text == nullptr) {
        return false;
    }

    uint32_t i = 0;
    while (text[i] != '\0') {
        uint32_t unicode = TypedText::GetUTF8Next(text, i, i);
        if (IsShapingLetter(unicode, ttfId)) {
#if ENABLE_SHAPING
            script = GetScriptByTtfId(ttfId);
#endif
            return true;
        }
    }
    return false;
}

uint8_t UIMultiFontManager::GetShapingFontId(const char* text, uint8_t fontId, uint8_t& ttfId, uint32_t& script)
{
    // the shaping font is in search list, then shapingFontId_ store the real shaping fontid
    const UITextLanguageFontParam* fontParam1 = UIFont::GetInstance()->GetFontInfo(fontId);
    if (fontParam1 == nullptr) {
        return DEFAULT_SHAPING_ID;
    }
    if (!fontParam1->shaping) {
        if (!IsNeedShaping(text, ttfId, script)) {
            return 0; // 0 means  no need to shape
        }
        uint8_t* searchLists = nullptr;
        int8_t length = GetSearchFontList(fontId, &searchLists);
        const UITextLanguageFontParam* fontParam2 = nullptr;
        for (uint8_t i = 0; i < length; i++) {
            fontParam2 = UIFont::GetInstance()->GetFontInfo(searchLists[i]);
            if (fontParam2 == nullptr) {
                continue;
            }
            if (fontParam2->ttfId == ttfId) {
                return searchLists[i];
            }
        }
        return 0;
    }
    ttfId = fontParam1->ttfId;

#if ENABLE_SHAPING
    script = GetScriptByTtfId(ttfId);
#endif
    return DEFAULT_SHAPING_ID;
}
int8_t UIMultiFontManager::IsShapingLetter(uint32_t unicode, uint8_t &ttfId)
{
    // arbic
    if ((unicode <= 0x06FF) && (unicode >= 0x0600)) {
        ttfId = arbicTtfId_;
        return ttfId != 0;
    }
    // thai
    if ((unicode <= 0x0E7F) && (unicode >= 0x0E00)) {
        ttfId = thaiTtfId_;
        return ttfId != 0;
    }
    // Devanagari
    if ((unicode <= 0x097F) && (unicode >= 0x0900)) {
        ttfId = devanagariTtfId_;
        return ttfId != 0;
    }
    // Hebrew
    if ((unicode <= 0x05FF) && (unicode >= 0x0590)) {
        ttfId = hebrewTtfId_;
        return ttfId != 0;
    }
    // Myanmar
    if ((unicode <= 0x109F) && (unicode >= 0x1000)) {
        ttfId = myanmarTtfId_;
        return ttfId != 0;
    }
    // Bengali
    if ((unicode <= 0x9FF) && (unicode >= 0x980)) {
        ttfId = bengaliTtfId_;
        return ttfId != 0;
    }
    return false;
}
#if ENABLE_SHAPING
uint32_t UIMultiFontManager::GetScriptByTtfId(uint8_t ttfId)
{
    // arbic
    if (ttfId == arbicTtfId_) {
        return SHAPING_SCRIPT_ARABIC;
    }
    // thai
    if (ttfId == thaiTtfId_) {
        return SHAPING_SCRIPT_THAI;
    }
    // Devanagari
    if (ttfId == devanagariTtfId_) {
        return SHAPING_SCRIPT_DEVANAGARI;
    }
    // Hebrew
    if (ttfId == hebrewTtfId_) {
        return SHAPING_SCRIPT_HEBREW;
    }
    // Myanmar
    if (ttfId == myanmarTtfId_) {
        return SHAPING_SCRIPT_MYANMAR;
    }
    // Bengali
    if (ttfId == bengaliTtfId_) {
        return SHAPING_SCRIPT_BENGALI;
    }
    return SHAPING_SCRIPT_INVALID;
}
#endif
}
#endif
