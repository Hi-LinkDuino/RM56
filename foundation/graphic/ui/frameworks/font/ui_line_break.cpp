/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if ENABLE_ICU
#include "common/typed_text.h"
#include "draw/draw_utils.h"
#include "font/ui_font.h"
#include "font/ui_line_break.h"
#include "rbbidata.h"
#include "ucmndata.h"

using namespace U_ICU_NAMESPACE;
namespace OHOS {
static void* MemAlloc(const void* context, size_t size)
{
    return UIMalloc(size);
}

static void MemFree(const void* context, void* mem)
{
    if (mem == nullptr) {
        return;
    }
    UIFree(mem);
}

static void* MemRealloc(const void* context, void* mem, size_t size)
{
    return UIRealloc(mem, size);
}

UILineBreakEngine& UILineBreakEngine::GetInstance()
{
    static UILineBreakEngine instance;
    return instance;
}

uint16_t UILineBreakEngine::GetNextBreakPos(UILineBreakProxy& record)
{
    const UChar* str = reinterpret_cast<const UChar*>(record.GetStr());
    if ((str == nullptr) || !initSuccess_ || (lineBreakTrie_ == nullptr)) {
        return 0;
    }
    int32_t state = LINE_BREAK_STATE_START;
    const RBBIStateTable* rbbStateTable = reinterpret_cast<const RBBIStateTable*>(stateTbl_);
    const RBBIStateTableRow* row =
        reinterpret_cast<const RBBIStateTableRow*>(rbbStateTable->fTableData + rbbStateTable->fRowLen * state);
    UTrie2* trie2 = reinterpret_cast<UTrie2*>(lineBreakTrie_);
    for (uint16_t index = 0; index < record.GetStrLen(); ++index) {
        uint16_t category = UTRIE2_GET16(trie2, static_cast<uint32_t>(str[index]));
        // 0x4000: remove the dictionary flag bit
        if ((category & 0x4000) != 0) {
            // 0x4000: remove the dictionary flag bit
            category &= ~0x4000;
        }
        state = row->fNextState[category];
        row = reinterpret_cast<const RBBIStateTableRow*>(rbbStateTable->fTableData + rbbStateTable->fRowLen * state);
        int16_t completedRule = row->fAccepting;
        if ((completedRule > 0) || (state == LINE_BREAK_STATE_STOP)) {
            return index;
        }
    }
    return record.GetStrLen();
}

void UILineBreakEngine::LoadRule()
{
    if ((fp_ < 0) || (addr_ == nullptr)) {
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    u_setMemoryFunctions(nullptr, MemAlloc, MemRealloc, MemFree, &status);
    if (status != U_ZERO_ERROR) {
        return;
    }
    int32_t ret = lseek(fp_, offset_, SEEK_SET);
    if (ret != offset_) {
        return;
    }
    char* buf = addr_;
    ret = read(fp_, buf, size_);
    if (ret != size_) {
        return;
    }
    const char* dataInBytes = reinterpret_cast<const char*>(buf);
    const DataHeader* dh = reinterpret_cast<const DataHeader*>(buf);
    const RBBIDataHeader* rbbidh = reinterpret_cast<const RBBIDataHeader*>(dataInBytes + dh->dataHeader.headerSize);
    stateTbl_ = reinterpret_cast<const RBBIStateTable*>(reinterpret_cast<const char*>(rbbidh) + rbbidh->fFTable);
    status = U_ZERO_ERROR;
    lineBreakTrie_ = reinterpret_cast<UTrie2*>(
        utrie2_openFromSerialized(UTRIE2_16_VALUE_BITS, reinterpret_cast<const uint8_t*>(rbbidh) + rbbidh->fTrie,
                                  rbbidh->fTrieLen, nullptr, &status));
    if (status != U_ZERO_ERROR) {
        return;
    }
    initSuccess_ = true;
}

uint32_t UILineBreakEngine::GetNextLineAndWidth(const char* text,
                                                uint8_t fontId,
                                                uint8_t fontSize,
                                                int16_t space,
                                                bool allBreak,
                                                int16_t& maxWidth,
                                                int16_t& maxHeight,
                                                uint16_t& letterIndex,
                                                SizeSpan* sizeSpans,
                                                uint16_t len)
{
    if (text == nullptr) {
        return 0;
    }
    bool isAllCanBreak = allBreak;
    uint32_t byteIdx = 0;
    uint32_t preIndex = 0;
    int16_t lastWidth = 0;
    int16_t lastIndex = 0;
    int16_t curWidth = 0;
    int32_t state = LINE_BREAK_STATE_START;
    int16_t width = 0;
    int16_t height = 0;
    while ((text[byteIdx] != '\0') && (byteIdx < len)) {
        uint32_t unicode = TypedText::GetUTF8Next(text, preIndex, byteIdx);
        if (unicode == 0) {
            preIndex = byteIdx;
            continue;
        }
        if (isAllCanBreak || IsBreakPos(unicode, fontId, fontSize, state)) {
            state = LINE_BREAK_STATE_START;
            // Accumulates the status value from the current character.
            IsBreakPos(unicode, fontId, fontSize, state);
            lastIndex = preIndex;
            lastWidth = curWidth;
        }
        width = GetLetterWidth(unicode, letterIndex, height, fontId, fontSize, sizeSpans);
        letterIndex++;
        if (height > maxHeight) {
            maxHeight = height;
        }
        int16_t nextWidth = (curWidth > 0 && width > 0) ? (curWidth + space + width) : (curWidth + width);
        if (nextWidth > maxWidth) {
            letterIndex--;
            if (lastIndex == 0) {
                break;
            }
            maxWidth = lastWidth;
            return lastIndex;
        }
        curWidth = nextWidth;
        preIndex = byteIdx;
        if (byteIdx > 0 && ((text[byteIdx - 1] == '\r') || (text[byteIdx - 1] == '\n'))) {
            break;
        }
    }
    maxWidth = curWidth;
    return preIndex;
}

int16_t UILineBreakEngine::GetLetterWidth(uint32_t unicode, uint16_t& letterIndex, int16_t& height,
                                          uint8_t fontId, uint8_t fontSize, SizeSpan* sizeSpans)
{
    if (sizeSpans != nullptr && sizeSpans[letterIndex].isSizeSpan) {
        int16_t width = UIFont::GetInstance()->GetWidth(unicode, sizeSpans[letterIndex].fontId,
                                                        sizeSpans[letterIndex].size, 0);

        if (sizeSpans[letterIndex].height == 0) {
            height = UIFont::GetInstance()->GetHeight(sizeSpans[letterIndex].fontId,
                                                      sizeSpans[letterIndex].size);
            sizeSpans[letterIndex].height = height;
        } else {
            height = sizeSpans[letterIndex].height;
        }
        return width;
    } else {
        height = UIFont::GetInstance()->GetHeight(fontId, fontSize);
        return UIFont::GetInstance()->GetWidth(unicode, fontId, fontSize, 0);
    }
}

bool UILineBreakEngine::IsBreakPos(uint32_t unicode, uint8_t fontId, uint8_t fontSize, int32_t& state)
{
    if ((unicode > TypedText::MAX_UINT16_HIGH_SCOPE) || (stateTbl_ == nullptr) || (lineBreakTrie_ == nullptr)) {
        return true;
    }
    const RBBIStateTable* rbbStateTable = reinterpret_cast<const RBBIStateTable*>(stateTbl_);
    const RBBIStateTableRow* row =
        reinterpret_cast<const RBBIStateTableRow*>(rbbStateTable->fTableData + rbbStateTable->fRowLen * state);
    uint16_t utf16 = 0;
    if (unicode <= TypedText::MAX_UINT16_LOW_SCOPE) {
        utf16 = (unicode & TypedText::MAX_UINT16_LOW_SCOPE);
    } else if (unicode <= TypedText::MAX_UINT16_HIGH_SCOPE) {
        utf16 = static_cast<uint16_t>(TypedText::UTF16_LOW_PARAM + (unicode & TypedText::UTF16_LOW_MASK)); // low
        uint16_t category = UTRIE2_GET16(reinterpret_cast<UTrie2*>(lineBreakTrie_), static_cast<uint32_t>(utf16));
        // 0x4000: remove the dictionary flag bit
        if ((category & 0x4000) != 0) {
            // 0x4000: remove the dictionary flag bit
            category &= ~0x4000;
        }
        state = row->fNextState[category];
        row = reinterpret_cast<const RBBIStateTableRow*>(rbbStateTable->fTableData + rbbStateTable->fRowLen * state);
        utf16 = static_cast<uint16_t>(TypedText::UTF16_HIGH_PARAM1 + (unicode >> TypedText::UTF16_HIGH_SHIFT) -
                                      TypedText::UTF16_HIGH_PARAM2); // high
    }
    uint16_t category = UTRIE2_GET16(reinterpret_cast<UTrie2*>(lineBreakTrie_), static_cast<uint32_t>(utf16));
    // 0x4000: remove the dictionary flag bit
    if ((category & 0x4000) != 0) {
        // 0x4000: remove the dictionary flag bit
        category &= ~0x4000;
    }
    state = row->fNextState[category];
    row = reinterpret_cast<const RBBIStateTableRow*>(rbbStateTable->fTableData + rbbStateTable->fRowLen * state);
    return (row->fAccepting > 0 || state == LINE_BREAK_STATE_STOP);
}
} // namespace OHOS
#endif // ENABLE_ICU
