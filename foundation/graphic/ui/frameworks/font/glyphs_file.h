/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef GLYPHS_FILE_H
#define GLYPHS_FILE_H

#include "font/ui_font_header.h"

namespace OHOS {
class GlyphsFile {
public:
    GlyphsFile();

    GlyphsFile(const GlyphsFile&) = delete;

    GlyphsFile& operator=(const GlyphsFile&) = delete;

    GlyphsFile(const GlyphsFile&&) = delete;

    GlyphsFile& operator=(const GlyphsFile&&) = delete;

    ~GlyphsFile();

    int8_t GetFontVersion(const char* fontName, char* version, uint8_t len);

    int16_t GetFontHeight(uint8_t fontId);

    const FontHeader* GetFontHeader(uint8_t fontId);

    int8_t GetBitmap(GlyphNode& node, uint8_t* bitmap);

    int8_t SetFile(const char* fontName, int32_t fp, uint32_t start);

    int8_t GetNodeFromFile(uint32_t unicode, uint8_t fontId, GlyphNode& node);

    bool IsSameFile(const char* fontName);

private:
    static constexpr uint8_t FONT_NAME_LEN_MAX = 64;
    static constexpr uint8_t RADIX_TREE_BITS = 4;
    static constexpr uint8_t RADIX_SHIFT_START = 32 - RADIX_TREE_BITS;
    static constexpr uint32_t RADIX_TREE_SLOT_NUM = 1 << RADIX_TREE_BITS;
    static constexpr uint32_t RADIX_TREE_MASK = RADIX_TREE_SLOT_NUM - 1;

    struct IndexNode {
        uint16_t stubs[RADIX_TREE_SLOT_NUM];
    };

    struct GlyphInfo {
        uint8_t fontId;
        uint32_t glyphNodeSectionStart;
        uint32_t bitMapSectionStart;
        uint32_t fontIndexSectionStart;
        uint8_t* indexCache;
        FontHeader* fontHeader;
    };

    int8_t CacheInit();
    int8_t GetGlyphInfo(uint8_t fontId, GlyphInfo& glyphInfo);
    void SetFontName(const char* fontName);

    BinHeader binHeader_;
    uint32_t start_;
    uint32_t fontHeaderSectionStart_;
    uint32_t fontIndexSectionStart_;
    uint32_t glyphNodeSectionStart_;
    uint32_t bitMapSectionStart_;

    FontHeader* fontHeaderCache_;
    uint8_t* indexCache_;
    char* fontName_;
    int32_t fp_;
    bool isFileSet_;
    uint8_t fontNum_;
};
} // namespace OHOS
#endif /* GLYPHS_FILE_H */
