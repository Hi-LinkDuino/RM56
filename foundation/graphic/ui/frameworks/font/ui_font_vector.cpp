/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "font/ui_font_vector.h"
#include <freetype/ftoutln.h>
#include <freetype/internal/ftobjs.h>
#include <freetype/ftbitmap.h>
#include "common/typed_text.h"
#include "draw/draw_utils.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "securec.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
UIFontVector::UIFontVector()
{
#ifdef _WIN32
    ttfDir_ = _pgmptr;
    size_t len = ttfDir_.size();
    size_t pos = ttfDir_.find_last_of('\\');
    if (pos != std::string::npos) {
        ttfDir_.replace((pos + 1), (len - pos), VECTOR_FONT_DIR);
    }
#else
    ttfDir_ = VECTOR_FONT_DIR;
#endif // _WIN32
    ftLibrary_ = nullptr;
    freeTypeInited_ = ((FT_Init_FreeType(&ftLibrary_) == 0) ? true : false);
    bitmapCache_ = nullptr;
}

UIFontVector::~UIFontVector()
{
    if (freeTypeInited_) {
        FT_Done_FreeType(ftLibrary_);
        freeTypeInited_ = false;
        UnregisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
    }
    delete bitmapCache_;
}

bool UIFontVector::IsColorEmojiFont(FT_Face &face)
{
    static const uint32_t tag = FT_MAKE_TAG('C', 'B', 'D', 'T');
    FT_ULong length = 0;
    FT_Load_Sfnt_Table(face, tag, 0, nullptr, &length);
    if (length) {
        return true;
    }
    return false;
}

int8_t SetupColorFont(FT_Face face)
{
    if (face->num_fixed_sizes == 0) {
        return INVALID_RET_VALUE;
    }
    FT_Int bestMatch = 0;
    const int32_t kDefaultPixelSize = 128;
    int32_t diff = MATH_ABS(kDefaultPixelSize - face->available_sizes[0].width);
    for (int32_t i = 1; i < face->num_fixed_sizes; ++i) {
        int32_t ndiff = MATH_ABS(kDefaultPixelSize - face->available_sizes[i].width);
        if (ndiff < diff) {
            bestMatch = i;
            diff = ndiff;
        }
    }
    return FT_Select_Size(face, bestMatch); // FT_Match_Size
}

uint8_t UIFontVector::RegisterFontInfo(const char* ttfName, uint8_t shaping)
{
    if ((ttfName == nullptr) || !freeTypeInited_) {
        return FONT_INVALID_TTF_ID;
    }

    if (bitmapCache_ == nullptr) {
        uintptr_t ramAddr = GetRamAddr();
        uint32_t ramLen = GetRamLen();
        bitmapCache_ = new(std::nothrow) UIFontCache(reinterpret_cast<uint8_t*>(ramAddr), ramLen);
        if (bitmapCache_ == nullptr) {
            return INVALID_RET_VALUE;
        }
    }

    int32_t j = 0;
    while (j < FONT_ID_MAX) {
        if ((fontInfo_[j].ttfName != nullptr) && !strncmp(fontInfo_[j].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
            return j;
        } else if (fontInfo_[j].ttfName == nullptr) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(ttfName);
            int32_t error = FT_New_Face(ftLibrary_, ttfPath.c_str(), 0, &ftFaces_[j]);
            if (error != 0) {
                return FONT_INVALID_TTF_ID;
            }
            fontInfo_[j].ttfName = ttfName;
            fontInfo_[j].shaping = shaping;
            fontInfo_[j].ttfId = j;
            currentFontInfoNum_ = j + 1;
            if (IsColorEmojiFont(ftFaces_[j])) {
                SetupColorFont(ftFaces_[j]);
            }
#if ENABLE_MULTI_FONT
            UIMultiFontManager::GetInstance()->UpdateScript(fontInfo_[j]);
#endif
            return j;
        }
        j++;
    }
    return FONT_INVALID_TTF_ID;
}

uint8_t UIFontVector::RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return FONT_INVALID_TTF_ID;
    }
    uint8_t count = 0;
    for (uint8_t i = 0; i < num; i++) {
        uint8_t result = RegisterFontInfo(fontsTable[i].ttfName, fontsTable[i].shaping);
        if (result == FONT_INVALID_TTF_ID) {
            continue;
        }
        count++;
    }
    return count;
}

uint8_t UIFontVector::UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num)
{
    if (fontsTable == nullptr) {
        return 0;
    }
    uint8_t count = 0;
    for (uint8_t i = 0; i < num; i++) {
        uint8_t result = UnregisterFontInfo(fontsTable[i].ttfName);
        if (result == FONT_INVALID_TTF_ID) {
            return FONT_INVALID_TTF_ID;
        }
        count++;
    }
    return count;
}
uint8_t UIFontVector::UnregisterFontInfo(const char* ttfName)
{
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && !strncmp(fontInfo_[i].ttfName, ttfName, TTF_NAME_LEN_MAX)) {
                fontInfo_[i].ttfName = nullptr;
                FT_Done_Face(ftFaces_[i]);
                ftFaces_[i] = nullptr;
                return static_cast<uint8_t>(i);
            }
            i++;
        }
    }
    return FONT_INVALID_TTF_ID;
}

const UITextLanguageFontParam* UIFontVector::GetFontInfo(uint8_t fontId) const
{
    if (fontId < FONT_ID_MAX) {
        return static_cast<const UITextLanguageFontParam*>(&fontInfo_[fontId]);
    }
    return nullptr;
}

int32_t UIFontVector::OpenVectorFont(uint8_t ttfId)
{
    int32_t i = 0;
    int32_t fp = 0;
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            std::string ttfPath = ttfDir_;
            ttfPath.append(fontInfo_[i].ttfName);
#ifdef _WIN32
            fp = open(ttfPath.c_str(), O_RDONLY | O_BINARY);
#else
            fp = open(ttfPath.c_str(), O_RDONLY);
#endif
            return fp;
        }
        i++;
    }
    return -1;
}

bool UIFontVector::IsVectorFont() const
{
    return true;
}

uint8_t UIFontVector::GetFontWeight(uint8_t fontId)
{
    return FONT_BPP_8;
}

int8_t UIFontVector::SetFontPath(const char* path, FontType type)
{
    if (path == nullptr) {
        return INVALID_RET_VALUE;
    }
    ttfDir_ = path;
    return RET_VALUE_OK;
}

int8_t UIFontVector::GetFaceInfo(uint8_t fontId, uint8_t size, FaceInfo& faceInfo)
{
    if ((fontId >= FONT_ID_MAX) || (size == 0)) {
        return INVALID_RET_VALUE;
    }
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if ((fontInfo == nullptr) || (fontInfo->ttfName == nullptr)) {
        return INVALID_RET_VALUE;
    }

    if (!freeTypeInited_) {
        return INVALID_RET_VALUE;
    }

    faceInfo.key = GetKey(fontId, size);
    faceInfo.face = ftFaces_[fontId];

    // Set the size
    int error = FT_Set_Char_Size(faceInfo.face, size * FONT_PIXEL_IN_POINT, 0, 0, 0);
    if (error != 0) {
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}

uint16_t UIFontVector::GetHeight(uint8_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (bitmapCache_ == nullptr)) {
        return 0;
    }
    return static_cast<uint16_t>(faceInfo.face->size->metrics.height / FONT_PIXEL_IN_POINT);
}

uint8_t UIFontVector::GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const
{
#if ENABLE_MULTI_FONT
    const UITextLanguageFontParam* fontParam1 = GetFontInfo(fontId);
    if (fontParam1 == nullptr) {
        return 0;
    }
    if (fontParam1->shaping == 0) {
        if (!UIMultiFontManager::GetInstance()->IsNeedShaping(text, ttfId, script)) {
            return 0; // 0 means  no need to shape
        }
        uint8_t* searchLists = nullptr;
        int8_t length = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
        const UITextLanguageFontParam* fontParam2 = nullptr;
        for (uint8_t i = 0; i < length; i++) {
            fontParam2 = GetFontInfo(searchLists[i]);
            if (fontParam2 == nullptr) {
                continue;
            }
            if (fontParam2->ttfId == ttfId) {
                return fontParam2->shaping;
            }
        }
        return 0;
    }
    ttfId = fontParam1->ttfId;

#if ENABLE_SHAPING
    script = UIMultiFontManager::GetInstance()->GetScriptByTtfId(ttfId);
#endif
    return fontParam1->shaping;
#else
    const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
    if (fontInfo == nullptr) {
        return 0;
    }
    ttfId = fontInfo->ttfId;
    return fontInfo->shaping;
#endif
}
uint8_t UIFontVector::GetFontId(const char* ttfName, uint8_t fontSize) const
{
    if (ttfName != nullptr) {
        int32_t i = 0;
        while (i < FONT_ID_MAX) {
            if ((fontInfo_[i].ttfName != nullptr) && (strstr(fontInfo_[i].ttfName, ttfName) != nullptr)) {
                return static_cast<uint8_t>(i);
            }
            i++;
        }
    }

    return FONT_ID_MAX;
}

uint8_t UIFontVector::GetFontId(uint32_t unicode) const
{
    int32_t i = 0;
    uint8_t ttfId = ((unicode >> 24) & 0x1F); // 24: Whether 25 ~29 bit storage is ttfId 0x1F:5bit
    while (i < FONT_ID_MAX) {
        if (fontInfo_[i].ttfName == nullptr) {
            i++;
            continue;
        }
        if (fontInfo_[i].ttfId == ttfId) {
            return i;
        }
        i++;
    }
    return FONT_INVALID_TTF_ID;
}

int16_t UIFontVector::GetWidth(uint32_t unicode, uint8_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo = {};
    int8_t ret = INVALID_RET_VALUE;

    if (TypedText::IsColourWord(unicode, fontId, fontSize)) {
        ret = LoadGlyphIntoFace(fontId, unicode, faceInfo.face);
        if (ret != RET_VALUE_OK) {
            return INVALID_RET_VALUE;
        }

        if ((fontId >= FONT_ID_MAX) || (fontSize == 0)) {
            return INVALID_RET_VALUE;
        }
        const UITextLanguageFontParam* fontInfo = GetFontInfo(fontId);
        if ((fontInfo == nullptr) || (fontInfo->ttfName == nullptr)) {
            return INVALID_RET_VALUE;
        }

        if (!freeTypeInited_) {
            return INVALID_RET_VALUE;
        }

        faceInfo.key = GetKey(fontId, fontSize);
        faceInfo.face = ftFaces_[fontId];
        ret = SetupColorFont(ftFaces_[fontId]);
    } else {
        ret = GetFaceInfo(fontId, fontSize, faceInfo);
    }

    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(faceInfo.key, unicode);
    if (bitmap != nullptr) {
        return reinterpret_cast<Metric*>(bitmap)->advance;
    }

    int8_t error = LoadGlyphIntoFace(fontId, unicode, faceInfo.face);
    if (error != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    SetFace(faceInfo, unicode);
    return static_cast<uint16_t>(faceInfo.face->glyph->advance.x / FONT_PIXEL_IN_POINT);
}

int8_t UIFontVector::GetFontHeader(FontHeader& fontHeader, uint8_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }

    fontHeader.ascender = static_cast<int16_t>(faceInfo.face->size->metrics.ascender / FONT_PIXEL_IN_POINT);
    fontHeader.descender = static_cast<int16_t>(faceInfo.face->size->metrics.descender / FONT_PIXEL_IN_POINT);
    fontHeader.fontHeight = static_cast<uint16_t>(faceInfo.face->size->metrics.height / FONT_PIXEL_IN_POINT);
    return RET_VALUE_OK;
}

int8_t UIFontVector::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize)
{
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    if (!freeTypeInited_ || (faceInfo.face == nullptr) || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
#if ENABLE_VECTOR_FONT
    uint8_t* bitmap = bitmapCache_->GetBitmap(faceInfo.key, unicode, glyphNode.textStyle);
#else
    uint8_t* bitmap = bitmapCache_->GetBitmap(faceInfo.key, unicode);
#endif
    if (bitmap != nullptr) {
        Metric* f = reinterpret_cast<Metric*>(bitmap);
        glyphNode.left = f->left;
        glyphNode.top = f->top;
        glyphNode.cols = f->cols;
        glyphNode.rows = f->rows;
        glyphNode.advance = f->advance;
        return RET_VALUE_OK;
    }
#if ENABLE_VECTOR_FONT
    int8_t error = LoadGlyphIntoFace(fontId, unicode, faceInfo.face, glyphNode.textStyle);
#else
    int8_t error = LoadGlyphIntoFace(fontId, unicode, faceInfo.face);
#endif
    if (error != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    glyphNode.left = faceInfo.face->glyph->bitmap_left;
    glyphNode.top = faceInfo.face->glyph->bitmap_top;
    glyphNode.cols = faceInfo.face->glyph->bitmap.width;
    glyphNode.rows = faceInfo.face->glyph->bitmap.rows;
    glyphNode.advance = static_cast<uint16_t>(faceInfo.face->glyph->advance.x / FONT_PIXEL_IN_POINT);
#if ENABLE_VECTOR_FONT
    SetFace(faceInfo, unicode, glyphNode.textStyle);
#else
    SetFace(faceInfo, unicode);
#endif
    return RET_VALUE_OK;
}

uint8_t* UIFontVector::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize)
{
    if (GetGlyphNode(unicode, glyphNode, fontId, fontSize) != RET_VALUE_OK) {
        return nullptr;
    }
    FaceInfo faceInfo;
    int8_t ret = GetFaceInfo(fontId, fontSize, faceInfo);
    if (ret != RET_VALUE_OK) {
        return nullptr;
    }
#if ENABLE_VECTOR_FONT
    uint8_t* bitmap = bitmapCache_->GetBitmap(faceInfo.key, unicode, glyphNode.textStyle);
#else
    uint8_t* bitmap = bitmapCache_->GetBitmap(faceInfo.key, unicode);
#endif
    if (bitmap != nullptr) {
        return bitmap + sizeof(Metric);
    }
#if ENABLE_VECTOR_FONT
    SetFace(faceInfo, unicode, glyphNode.textStyle);
#else
    SetFace(faceInfo, unicode);
#endif
    return static_cast<uint8_t*>(faceInfo.face->glyph->bitmap.buffer);
}

bool UIFontVector::IsEmojiFont(uint8_t fontId)
{
    return IsColorEmojiFont(ftFaces_[fontId]);
}

#if ENABLE_VECTOR_FONT
void UIFontVector::SetItaly(FT_GlyphSlot slot)
{
    if (slot->format != FT_GLYPH_FORMAT_OUTLINE) {
        GRAPHIC_LOGE("SetItaly error");
        return;
    }
    float lean = 0.2f; // Slope of word
    FT_Matrix matrix;
    matrix.xx = 0x10000L; // Staggered matrix along x-axis
    matrix.xy = lean * 0x10000L;
    matrix.yx = 0;
    matrix.yy = 0x10000L;  // Staggered matrix along y-axis
    FT_Outline  outline = slot->outline;
    FT_Outline_Transform(&outline, &matrix);
}
void UIFontVector::SetBold(uint8_t fontId)
{
    int32_t error;
    FT_GlyphSlot slot = ftFaces_[fontId]->glyph;
    //some reasonable strength, copied from freeType
    FT_Pos xBold =
        FT_MulFix(ftFaces_[fontId]->units_per_EM, ftFaces_[fontId]->size->metrics.y_scale) / 24;
    FT_Pos yBold = xBold;
    if (ftFaces_[fontId]->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
        FT_BBox oldBox;
        FT_Outline_Get_CBox(&slot->outline, &oldBox);
        error = FT_Outline_Embolden(&slot->outline, xBold);
        if (error != 0) {
            GRAPHIC_LOGE("SetBold error");
            return;
        }
    } else if (ftFaces_[fontId]->glyph->format == FT_GLYPH_FORMAT_BITMAP) {
        FT_Library ftLibrary = slot->library;
        error = FT_Bitmap_Embolden(ftLibrary, &slot->bitmap, xBold, yBold);
        if (error != 0) {
            GRAPHIC_LOGE("SetBold error");
            return;
        }
    }
}
#endif

int8_t UIFontVector::LoadGlyphIntoFace(uint8_t& fontId, uint32_t unicode, FT_Face face)
{
    bool isHaveBitmap = false;
    int32_t error;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId != GetFontId(unicode)) {
            return INVALID_RET_VALUE;
        }
        unicode = unicode & (0xFFFFFF); // Whether 0 ~24 bit storage is unicode
        error = FT_Load_Glyph(face, unicode, FT_LOAD_RENDER);
        isHaveBitmap = true;
    } else {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            uint32_t glyph_index = FT_Get_Char_Index(ftFaces_[i], unicode);
            if (glyph_index == 0) {
                continue;
            }
            error = FT_Load_Glyph(ftFaces_[i], glyph_index, FT_LOAD_COLOR);
            if (error == RET_VALUE_OK) {
                isHaveBitmap = true;
                fontId = i;
                if (ftFaces_[i]->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
                    error = FT_Render_Glyph(ftFaces_[i]->glyph, FT_RENDER_MODE_NORMAL);
                    break;
                }
            }
        }
    }
    if (!isHaveBitmap || (error != 0) || (ftFaces_[fontId]->glyph->glyph_index == 0)) {
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}

#if ENABLE_VECTOR_FONT
int8_t UIFontVector::LoadGlyphIntoFace(uint8_t& fontId, uint32_t unicode, FT_Face face, TextStyle textStyle)
{
    int32_t error;
    bool isHaveBitmap = false;
    if (IsGlyphFont(unicode) != 0) {
        if (fontId != GetFontId(unicode)) {
            return INVALID_RET_VALUE;
        }
        unicode = unicode & (0xFFFFFF); // Whether 0 ~24 bit storage is unicode
        error = FT_Load_Glyph(face, unicode, FT_LOAD_NO_BITMAP);
        isHaveBitmap = true;
    } else {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            if (ftFaces_[i] != nullptr) {
                uint32_t glyph_index = FT_Get_Char_Index(ftFaces_[i], unicode);
                if (glyph_index == 0) {
                    continue;
                }
                error = FT_Load_Glyph(ftFaces_[i], glyph_index, FT_LOAD_COLOR);
                if (error == RET_VALUE_OK) {
                    fontId = i;
                    isHaveBitmap = true;
                    break;
                }
            }
        }
    }
    if (!isHaveBitmap) {
        return INVALID_RET_VALUE;
    }
    if (textStyle == TEXT_STYLE_ITALIC) {
        SetItaly(ftFaces_[fontId]->glyph);
    } else if (textStyle == TEXT_STYLE_BOLD) {
        SetBold(fontId);
    } else if (textStyle == TEXT_STYLE_BOLD_ITALIC) {
        SetItaly(ftFaces_[fontId]->glyph);
        SetBold(fontId);
    }
    if (ftFaces_[fontId]->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        error = FT_Render_Glyph(ftFaces_[fontId]->glyph, FT_RENDER_MODE_NORMAL);
    }
    if ((error != 0) || (ftFaces_[fontId]->glyph->glyph_index == 0)) {
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}
#endif

uint8_t UIFontVector::IsGlyphFont(uint32_t unicode)
{
    uint8_t unicodeFontId = GetFontId(unicode);
    if (unicodeFontId == FONT_INVALID_TTF_ID) {
        return 0;
    } else {
        return fontInfo_[unicodeFontId].shaping;
    }
}

void UIFontVector::SetFace(FaceInfo& faceInfo, uint32_t unicode) const
{
#if ENABLE_VECTOR_FONT
    SetFace(faceInfo, unicode, TEXT_STYLE_NORMAL);
#else
    Metric f;
    f.advance = static_cast<uint16_t>(faceInfo.face->glyph->advance.x / FONT_PIXEL_IN_POINT);
    f.left = faceInfo.face->glyph->bitmap_left;
    f.top = faceInfo.face->glyph->bitmap_top;
    f.cols = faceInfo.face->glyph->bitmap.width;
    f.rows = faceInfo.face->glyph->bitmap.rows;

    int16_t pixSize = 1;
    if (faceInfo.face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        pixSize = 0x04; // 4 Byte
    }
    uint32_t bitmapSize = faceInfo.face->glyph->bitmap.width * faceInfo.face->glyph->bitmap.rows * pixSize;
    uint8_t* bitmap = bitmapCache_->GetSpace(faceInfo.key, unicode, bitmapSize + sizeof(Metric));
    if (bitmap != nullptr) {
        if (memcpy_s(bitmap, sizeof(Metric), &f, sizeof(Metric)) != EOK) {
            return;
        }
        if (memcpy_s(bitmap + sizeof(Metric), bitmapSize, faceInfo.face->glyph->bitmap.buffer, bitmapSize) != EOK) {
            return;
        }
    }
#endif
}

#if ENABLE_VECTOR_FONT
void UIFontVector::SetFace(FaceInfo& faceInfo, uint32_t unicode, TextStyle textStyle) const
{
    Metric f;
    f.advance = static_cast<uint16_t>(faceInfo.face->glyph->advance.x / FONT_PIXEL_IN_POINT);
    f.left = faceInfo.face->glyph->bitmap_left;
    f.top = faceInfo.face->glyph->bitmap_top;
    f.cols = faceInfo.face->glyph->bitmap.width;
    f.rows = faceInfo.face->glyph->bitmap.rows;

    int16_t pixSize = 1;
    if (faceInfo.face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        pixSize = 0x04; // 4 Byte
    }
    uint32_t bitmapSize = faceInfo.face->glyph->bitmap.width * faceInfo.face->glyph->bitmap.rows * pixSize;
    uint8_t* bitmap = bitmapCache_->GetSpace(faceInfo.key, unicode, bitmapSize + sizeof(Metric), textStyle);
    if (bitmap != nullptr) {
        if (memcpy_s(bitmap, sizeof(Metric), &f, sizeof(Metric)) != EOK) {
            return;
        }
        if (memcpy_s(bitmap + sizeof(Metric), bitmapSize, faceInfo.face->glyph->bitmap.buffer, bitmapSize) != EOK) {
            return;
        }
    }
}
#endif

inline uint32_t UIFontVector::GetKey(uint8_t fontId, uint32_t size)
{
    return ((static_cast<uint32_t>(fontId)) << 24) + size; // fontId store at the (24+1)th bit
}

uint16_t UIFontVector::GetOffsetPosY(const char* text,
                                     uint16_t lineLength,
                                     bool& isEmojiLarge,
                                     uint8_t fontId,
                                     uint8_t fontSize)
{
    if (!freeTypeInited_ || (bitmapCache_ == nullptr)) {
        return INVALID_RET_VALUE;
    }
    uint32_t i = 0;
    uint32_t unicode;
    uint16_t textNum = 0;
    uint16_t emojiNum = 0;
    uint16_t loopNum = 0;
    GlyphNode glyphNode;
    GlyphNode emojiMaxNode = {};
    uint8_t maxFontSize = fontSize;
    while (i < lineLength) {
        unicode = TypedText::GetUTF8Next(text, i, i);
        uint8_t ret = GetGlyphNode(unicode, glyphNode, fontId, fontSize);
        if (ret == RET_VALUE_OK) {
            uint8_t weight = GetFontWeight(glyphNode.fontId);
            if (weight >= 16) { // 16: bit rgb565 rgba8888
                emojiMaxNode = glyphNode.rows > emojiMaxNode.rows ? glyphNode : emojiMaxNode;
                emojiNum++;
            } else {
                textNum++;
            }
            loopNum++;
        }
    }
    // The number of emoji is the same as the number of cycles, indicating that this line is all emoji
    // The number of words is the same as the number of cycles, which means that this line is all words
    if ((emojiNum == loopNum) || (textNum == loopNum)) {
        isEmojiLarge = true;
        return 0;
    }
    isEmojiLarge = emojiMaxNode.rows > maxFontSize;
    uint16_t offset = 0;
    if (isEmojiLarge) {
        // If the emoji is higher than the text
        if (emojiMaxNode.top >= maxFontSize) {
            offset = emojiMaxNode.top - maxFontSize;
        }
    } else {
        // If text are higher than emoji
        if (maxFontSize >= emojiMaxNode.rows) {
            offset = maxFontSize - emojiMaxNode.rows;
        }
    }
    return offset;
}

uint16_t UIFontVector::GetLineMaxHeight(const char *text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                                        uint16_t& letterIndex, SizeSpan* sizeSpans)
{
    if (!freeTypeInited_) {
        return INVALID_RET_VALUE;
    }
    uint32_t i = 0;
    uint32_t unicode;
    uint16_t textNum = 0;
    uint16_t emojiNum = 0;
    uint16_t loopNum = 0;
    uint16_t maxHeight = GetHeight(fontId, fontSize);
    while (i < lineLength) {
        unicode = TypedText::GetUTF8Next(text, i, i);
        TypedText::IsColourWord(unicode, fontId, fontSize) ? emojiNum++ : textNum++;
        loopNum++;
        if (sizeSpans != nullptr && sizeSpans[letterIndex].isSizeSpan) {
            uint16_t spannableHeight = 0;
            if (sizeSpans[letterIndex].height == 0) {
                spannableHeight = GetHeight(sizeSpans[letterIndex].fontId, sizeSpans[letterIndex].size);
                sizeSpans[letterIndex].height = spannableHeight;
            } else {
                spannableHeight = sizeSpans[letterIndex].height;
            }
            maxHeight = spannableHeight > maxHeight ? spannableHeight : maxHeight;
        }
        letterIndex++;
        if (i > 0 && ((text[i - 1] == '\r') || (text[i - 1] == '\n'))) {
            break;
        }
    }
    return GetMaxSubLineHeight(textNum, loopNum, maxHeight, emojiNum);
}

uint16_t UIFontVector::GetMaxSubLineHeight(uint16_t textNum, uint16_t loopNum, uint16_t maxHeight, uint16_t emojiNum)
{
    // The number of words is the same as the number of cycles, which means that this line is all words
    if (textNum == loopNum) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            if (!IsColorEmojiFont(ftFaces_[i])) {
                uint16_t height = static_cast<uint16_t>(ftFaces_[i]->size->metrics.height / FONT_PIXEL_IN_POINT);
                if (height > maxHeight) {
                    maxHeight = height;
                }
                return maxHeight;
            }
        }
    }
    // The number of emoji is the same as the number of cycles, indicating that this line is all emoji
    if (emojiNum == loopNum) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            if (IsColorEmojiFont(ftFaces_[i])) {
                return static_cast<uint16_t>(ftFaces_[i]->size->metrics.height / FONT_PIXEL_IN_POINT);
            }
        }
    }
    // A line has both emoji and words
    uint16_t tmpHeight = 0;
    if ((textNum > 0) && (emojiNum > 0)) {
        for (uint8_t i = 0; i < currentFontInfoNum_; i++) {
            tmpHeight = static_cast<uint16_t>(ftFaces_[i]->size->metrics.height / FONT_PIXEL_IN_POINT);
            maxHeight = tmpHeight > maxHeight ? tmpHeight : maxHeight;
        }
    }
    return maxHeight;
}
} // namespace OHOS
