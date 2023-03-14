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

#ifndef UI_MULTI_FONT_MANAGER_H
#define UI_MULTI_FONT_MANAGER_H

#include "graphic_config.h"
#include "font/ui_font_header.h"
#if ENABLE_MULTI_FONT
#include "gfx_utils/heap_base.h"
namespace OHOS {
class UIMultiFontManager : public HeapBase {
public:
    UIMultiFontManager(const UIMultiFontManager &) = delete;
    UIMultiFontManager& operator=(const UIMultiFontManager &) = delete;
    UIMultiFontManager(UIMultiFontManager &&) noexcept = delete;
    UIMultiFontManager& operator=(UIMultiFontManager &&) noexcept = delete;

    /**
     * @brief Get the Instance object
     *
     * @return UIMultiFontManager*
     */
    static UIMultiFontManager* GetInstance();
    void ClearSearchFontList();
    int8_t SetSearchFontList(uint8_t fontListId, uint8_t *fontIds, uint8_t size);
    int8_t GetSearchFontList(uint8_t fontListId, uint8_t **fontIds);
    void UpdateScript(UITextLanguageFontParam& fonts);
    bool IsNeedShaping(const char* text, uint8_t& ttfId, uint32_t& script);
    uint8_t GetShapingFontId(const char* text, uint8_t fontId, uint8_t& ttfId, uint32_t& script);
#if ENABLE_SHAPING
    uint32_t GetScriptByTtfId(uint8_t ttfId);
#endif

private:
    /**
     * @brief Construct a new UIMultiFontManager object
     *
     */
    UIMultiFontManager();

    /**
     * @brief Destroy the UIMultiFontManager object
     *
     */
    ~UIMultiFontManager();
    int8_t AddNewFont(uint8_t fontListId, uint8_t *fontIds, int8_t size, uint8_t fontId);
    int8_t UpdateFont(uint8_t fontListId, uint8_t *fontIds, uint8_t size);
    int8_t IsShapingLetter(uint32_t unicode, uint8_t& ttfId);
    struct FontIdNode {
        uint8_t* fontIds = nullptr;
        int8_t size = 0;
    };
    static constexpr uint8_t MAX_LIST_NUM = 26;
    static constexpr uint8_t DEFAULT_SHAPING_ID = 1;
    static constexpr const char *ARABIC_LANG = "Arabic";
    static constexpr const char *THAI_LANG = "Thai";
    static constexpr const char *MYAN_LANG = "Myanmar";
    static constexpr const char *DVCARI_LANG = "Devanagari";
    static constexpr const char *HBREW_LANG = "Hebrew";
    static constexpr const char *BENGALI_LANG = "Bengali";
    uint8_t arbicTtfId_;
    uint8_t thaiTtfId_;
    uint8_t myanmarTtfId_;
    uint8_t devanagariTtfId_;
    uint8_t hebrewTtfId_;
    uint8_t bengaliTtfId_;
    uint8_t topIndex_;
    uint8_t* fontIdIndex_;
    FontIdNode fontNodes_[MAX_LIST_NUM];
};
}
#endif
#endif
