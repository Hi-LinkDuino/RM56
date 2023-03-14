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

#include "font/glyphs_file.h"
#include "font/font_ram_allocator.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
GlyphsFile::GlyphsFile()
    : binHeader_{{0}},
      start_(0),
      fontHeaderSectionStart_(0),
      fontIndexSectionStart_(0),
      glyphNodeSectionStart_(0),
      bitMapSectionStart_(0),
      fontHeaderCache_(nullptr),
      indexCache_(nullptr),
      fontName_(nullptr),
      fp_(-1),
      isFileSet_(false),
      fontNum_(0)
{
}
GlyphsFile::~GlyphsFile()
{
    if (fontName_) {
        UIFree(fontName_);
        fontName_ = nullptr;
    }
}

int8_t GlyphsFile::CacheInit()
{
    uint32_t size = 0;
    for (int32_t i = 0; i < fontNum_; i++) {
        size += fontHeaderCache_[i].indexLen;
    }

    indexCache_ = reinterpret_cast<uint8_t *>(FontRamAllocator::GetInstance().Allocate(size));
    if (indexCache_ == nullptr) {
        GRAPHIC_LOGE("GlyphsFile::CacheInit Allocate failed");
        return INVALID_RET_VALUE;
    }

    int32_t ret = read(fp_, indexCache_, size);
    if (ret != static_cast<int32_t>(size)) {
        GRAPHIC_LOGE("GlyphsFile::CacheInit read failed");
        return INVALID_RET_VALUE;
    }

    return RET_VALUE_OK;
}

int8_t GlyphsFile::GetNodeFromFile(uint32_t unicode, uint8_t fontId, GlyphNode& node)
{
    uint16_t idx = 0;
    uint8_t key;
    uint32_t offset;
    GlyphInfo glyphInfo;
    int8_t result = GetGlyphInfo(fontId, glyphInfo);
    if (result != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }
    for (int32_t i = RADIX_SHIFT_START; i >= 0; i -= RADIX_TREE_BITS) {
        offset = idx * sizeof(IndexNode);
        key = static_cast<uint8_t>((unicode >> static_cast<uint8_t>(i)) & RADIX_TREE_MASK);
        offset += key * sizeof(uint16_t);
        idx = *(reinterpret_cast<uint16_t*>(glyphInfo.indexCache + offset));
        if (idx == 0) {
            return INVALID_RET_VALUE;
        }
    }

    offset = glyphInfo.glyphNodeSectionStart + (idx - 1) * sizeof(GlyphNode);
    int32_t ret = lseek(fp_, offset, SEEK_SET);
    if (ret != static_cast<int32_t>(offset)) {
        GRAPHIC_LOGE("GlyphsFile::GetNodeFromFile lseek failed");
        return INVALID_RET_VALUE;
    }

    ret = read(fp_, &node, sizeof(GlyphNode));
    if (ret < 0) {
        GRAPHIC_LOGE("GlyphsFile::GetNodeFromFile read failed");
        return INVALID_RET_VALUE;
    }

    return RET_VALUE_OK;
}

void GlyphsFile::SetFontName(const char* fontName)
{
    if (fontName_ != nullptr) {
        return;
    }

    if (fontName == nullptr) {
        GRAPHIC_LOGE("GlyphsFile::SetFontName invalid parameters");
        return;
    }

    uint32_t nameCopyOffset = 0;
    uint32_t nameLen = strlen(fontName);
    if (nameLen > FONT_NAME_LEN_MAX) {
        nameCopyOffset = nameLen - FONT_NAME_LEN_MAX;
        nameLen = FONT_NAME_LEN_MAX;
    } else if (nameLen == 0) {
        return;
    }

    fontName_ = static_cast<char *>(UIMalloc(++nameLen));
    if (fontName_ == nullptr) {
        return;
    }
    if (memcpy_s(fontName_, nameLen, fontName, nameLen) != EOK) {
        UIFree(fontName_);
        fontName_ = nullptr;
    }
}

int8_t GlyphsFile::SetFile(const char* fontName, int32_t fp, uint32_t start)
{
    SetFontName(fontName);
    fp_ = fp;
    start_ = start;
    int32_t ret = lseek(fp_, start_, SEEK_SET);
    if (ret < 0) {
        GRAPHIC_LOGE("GlyphsFile::SetFile lseek failed");
        return INVALID_RET_VALUE;
    }
    ret = read(fp_, &binHeader_, sizeof(binHeader_));
    if (ret != sizeof(binHeader_)) {
        GRAPHIC_LOGE("GlyphsFile::SetFile read failed");
        return INVALID_RET_VALUE;
    }
    if (strncmp(binHeader_.fontMagic, FONT_MAGIC_NUMBER, FONT_MAGIC_NUM_LEN) != 0) {
        return INVALID_RET_VALUE;
    }
    if (binHeader_.fontNum > UIFontBuilder::GetInstance()->GetBitmapFontIdMax()) {
        GRAPHIC_LOGE("GlyphsFile::SetFile data error, fontNum need less than max fontId");
        return INVALID_RET_VALUE;
    }

    fontNum_ = binHeader_.fontNum;
    fontHeaderSectionStart_ = start_ + sizeof(binHeader_);
    int32_t size = sizeof(FontHeader) * fontNum_;
    fontIndexSectionStart_ = fontHeaderSectionStart_ + size;

    fontHeaderCache_ = reinterpret_cast<FontHeader*>(FontRamAllocator::GetInstance().Allocate(size));
    if (fontHeaderCache_ == nullptr) {
        GRAPHIC_LOGE("GlyphsFile::SetFile allocate font header cache failed");
        return INVALID_RET_VALUE;
    }

    ret = read(fp_, fontHeaderCache_, size);
    if (ret != size) {
        GRAPHIC_LOGE("GlyphsFile::SetFile read failed");
        return INVALID_RET_VALUE;
    }

    FontHeader* last = fontHeaderCache_ + fontNum_ - 1;
    size = last->indexOffset + last->indexLen;
    glyphNodeSectionStart_ = fontIndexSectionStart_ + size;

    size = 0;
    for (uint32_t i = 0; i < fontNum_; i++) {
        size += fontHeaderCache_[i].glyphNum * sizeof(GlyphNode);
    }
    bitMapSectionStart_ = glyphNodeSectionStart_ + size;
    ret = CacheInit();
    if (ret == RET_VALUE_OK) {
        isFileSet_ = true;
    }

    return ret;
}

bool GlyphsFile::IsSameFile(const char* fontName)
{
    if ((fontName_ == nullptr) || (fontName == nullptr)) {
        return false;
    }

    uint32_t Offset = 0;
    uint32_t nameLen = strlen(fontName);
    if (nameLen > FONT_NAME_LEN_MAX) {
        Offset = nameLen - FONT_NAME_LEN_MAX;
    }
    return (strcmp(fontName_, fontName + Offset) == 0);
}

int8_t GlyphsFile::GetGlyphInfo(uint8_t fontId, GlyphInfo& glyphInfo)
{
    uint16_t fontIdx = 0;
    if (fontId > UIFontBuilder::GetInstance()->GetBitmapFontIdMax()) {
        GRAPHIC_LOGE("GlyphsFile::GetGlyphInfo fontId need less than max fontId");
        return INVALID_RET_VALUE;
    }
    if (!isFileSet_) {
        GRAPHIC_LOGE("GlyphsFile::GetGlyphInfo file not set");
        return INVALID_RET_VALUE;
    }

    int32_t low = 0;
    int32_t high = binHeader_.fontNum - 1;
    bool found = false;

    while (low <= high) {
        int32_t mid = (low + high) / 2; // 2 means half
        if (fontHeaderCache_[mid].fontId == fontId) {
            fontIdx = mid;
            found = true;
            break;
        } else if (fontHeaderCache_[mid].fontId > fontId) {
            high = mid - 1;
        } else if (fontHeaderCache_[mid].fontId < fontId) {
            low = mid + 1;
        }
    }
    if (!found) {
        glyphInfo.fontHeader = nullptr;
        glyphInfo.fontId = UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
        return INVALID_RET_VALUE;
    }

    uint32_t size = 0;
    glyphInfo.fontId = fontId;
    glyphInfo.fontHeader = fontHeaderCache_ + fontIdx;
    glyphInfo.fontIndexSectionStart = fontIndexSectionStart_ + glyphInfo.fontHeader->indexOffset;
    for (uint32_t i = 0; i < fontIdx; i++) {
        size += fontHeaderCache_[i].glyphNum * sizeof(GlyphNode);
    }
    glyphInfo.glyphNodeSectionStart = glyphNodeSectionStart_ + size;
    glyphInfo.bitMapSectionStart = bitMapSectionStart_ + glyphInfo.fontHeader->glyphOffset;
    glyphInfo.indexCache = indexCache_ + glyphInfo.fontHeader->indexOffset;

    return RET_VALUE_OK;
}

int8_t GlyphsFile::GetFontVersion(const char* fontName, char* version, uint8_t len)
{
    if (!isFileSet_ || (version == nullptr) || (len > FONT_VERSION_LEN)) {
        GRAPHIC_LOGE("GlyphsFile::GetFontVersion invalid parameters");
        return INVALID_RET_VALUE;
    }

    if (!IsSameFile(fontName)) {
        return INVALID_RET_VALUE;
    }

    if (memset_s(version, len, 0, len) != EOK) {
        GRAPHIC_LOGE("GlyphsFile::GetFontVersion memset_s failed");
        return INVALID_RET_VALUE;
    }
    if (strcpy_s(version, len, binHeader_.fontVersion) != EOK) {
        GRAPHIC_LOGE("GlyphsFile::GetFontVersion strcpy_s failed");
        return INVALID_RET_VALUE;
    }
    return RET_VALUE_OK;
}

const FontHeader* GlyphsFile::GetFontHeader(uint8_t fontId)
{
    GlyphInfo glyphInfo;
    int8_t ret = GetGlyphInfo(fontId, glyphInfo);
    if (ret != RET_VALUE_OK) {
        return nullptr;
    }

    return glyphInfo.fontHeader;
}

int16_t GlyphsFile::GetFontHeight(uint8_t fontId)
{
    GlyphInfo glyphInfo;
    int8_t ret = GetGlyphInfo(fontId, glyphInfo);
    if (ret != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    return glyphInfo.fontHeader->fontHeight;
}

int8_t GlyphsFile::GetBitmap(GlyphNode& node, uint8_t* bitmap)
{
    if (bitmap == nullptr) {
        GRAPHIC_LOGE("GlyphsFile::GetBitmap invalid parameter");
        return INVALID_RET_VALUE;
    }

    GlyphInfo glyphInfo;
    int8_t result = GetGlyphInfo(node.fontId, glyphInfo);
    if (result != RET_VALUE_OK) {
        return INVALID_RET_VALUE;
    }

    uint32_t tmpBitMapSectionStart = glyphInfo.bitMapSectionStart;
    uint32_t offset = tmpBitMapSectionStart + node.dataOff;
    uint32_t size = node.kernOff - node.dataOff;
    int32_t ret = lseek(fp_, offset, SEEK_SET);
    if (ret != static_cast<int32_t>(offset)) {
        GRAPHIC_LOGE("GlyphsFile::GetBitmap lseek failed");
        return INVALID_RET_VALUE;
    }

    int32_t readSize = read(fp_, bitmap, size);
    if (readSize != static_cast<int32_t>(size)) {
        GRAPHIC_LOGE("GlyphsFile::GetBitmap read failed");
        return INVALID_RET_VALUE;
    }

    node.dataFlag = node.fontId;
    return RET_VALUE_OK;
}
} // namespace OHOS
