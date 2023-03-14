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

#ifndef GLYPHS_MANAGER_FONT_H
#define GLYPHS_MANAGER_FONT_H

#include "font/ui_font_header.h"
#include "font/glyphs_cache.h"
#include "font/glyphs_file.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class GlyphsManager : public HeapBase {
public:
    GlyphsManager();

    GlyphsManager(const GlyphsManager&) = delete;

    GlyphsManager& operator=(const GlyphsManager&) = delete;

    ~GlyphsManager();

    int8_t GetFontVersion(const char* fontName, char* version, uint8_t len);

    int16_t GetFontHeight(uint8_t fontId);

    int16_t GetFontWidth(uint32_t unicode, uint8_t fontId);

    const FontHeader* GetFontHeader(uint8_t fontId);

    const GlyphNode* GetGlyphNode(uint32_t unicode, uint8_t fontId);

    int8_t GetBitmap(uint32_t unicode, uint8_t* bitmap, uint8_t fontId);

    int8_t SetFile(const char* fontName, int32_t fp, uint32_t start);

private:
    GlyphsCache glyphsCache_;
    Graphic::Vector<GlyphsFile *> glyphsFiles_;
};
} // namespace OHOS
#endif /* GLYPHS_MANAGER_FONT_H */
