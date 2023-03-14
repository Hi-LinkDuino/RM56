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

#ifndef GRAPHIC_LITE_LINE_BREAK_H
#define GRAPHIC_LITE_LINE_BREAK_H

#include "graphic_config.h"
#if ENABLE_ICU
#include <cstdint>
#include <string>

#include "common/text.h"
#include "font/ui_font_header.h"
#include "gfx_utils/file.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/mem_api.h"

namespace OHOS {
class UILineBreakProxy;
/**
 * @brief Using ICU as the core of lineBreakEngine.
 *
 */
class UILineBreakEngine : public HeapBase {
public:
    /**
     * @brief Get UILineBreakEngine instannce.
     *
     * @return UILineBreakEngine&
     */
    static UILineBreakEngine& GetInstance();

    /**
     * @brief Init the line break engine and load the line break rules.
     *
     */
    void Init()
    {
        LoadRule();
    }

    /**
     * @brief Get the next line break position.
     *
     * @param record UILineBreakProxy instance.
     * @return uint16_t Next line break position.
     */
    uint16_t GetNextBreakPos(UILineBreakProxy& record);

    /**
     * @brief Set the rule file path.
     *
     * @param fp File descriptor.
     * @param offset The offset of rule.
     * @param size File size.
     * @return int32_t Result.
     */
    int32_t SetRuleBinInfo(int32_t fp, int32_t offset, uint32_t size)
    {
        fp_ = fp;
        offset_ = offset;
        int32_t fRet = lseek(fp_, offset, SEEK_SET);
        if (fRet != offset) {
            return fRet;
        }
        size_ = size;
        return 0;
    }

    /**
     * @brief Set the rule file load addr object.
     *
     * @param addr The rule file load addr.
     */
    void SetRuleFileLoadAddr(char* addr)
    {
        addr_ = addr;
    }

    /**
     * @brief Get the rule file load addr.
     *
     * @return char* The rule file load addr.
     */
    char* GetRuleFileLoadAddr() const
    {
        return addr_;
    }

    /**
     * @brief Get the size of rule file.
     *
     * @return int32_t The size of rule file.
     */
    int32_t GetRuleFileSize() const
    {
        return size_;
    }

    // 0xFFFF: unlimit the length until the end null.
    uint32_t GetNextLineAndWidth(const char* text,
                                 uint8_t fontId,
                                 uint8_t fontSize,
                                 int16_t space,
                                 bool allBreak,
                                 int16_t& maxWidth,
                                 int16_t& maxHeight,
                                 uint16_t& letterIndex,
                                 SizeSpan* sizeSpans,
                                 uint16_t len = 0xFFFF);
    bool IsBreakPos(uint32_t unicode, uint8_t fontId, uint8_t fontSize, int32_t& state);

private:
    UILineBreakEngine()
        : initSuccess_(false), addr_(nullptr), size_(0), fp_(0), offset_(0), lineBreakTrie_(nullptr), stateTbl_(nullptr)
    {
    }
    ~UILineBreakEngine() {}

    void LoadRule();
    int16_t GetLetterWidth(uint32_t unicode, uint16_t& letterIndex, int16_t& maxHeight,
                           uint8_t fontId, uint8_t fontSize, SizeSpan* sizeSpans);
    static constexpr const int32_t LINE_BREAK_STATE_START = 1;
    static constexpr const int32_t LINE_BREAK_STATE_STOP = 0;
    bool initSuccess_;
    char* addr_;
    int32_t size_;
    int32_t fp_;
    int32_t offset_;
    void* lineBreakTrie_;
    const void* stateTbl_;
};

/**
 * @brief Line break proxy.
 *
 */
class UILineBreakProxy : public HeapBase {
public:
    UILineBreakProxy() = delete;

    /**
     * @brief Construct a new UILineBreakProxy object.
     *
     * @param str Input string.
     * @param len The length of string.
     */
    UILineBreakProxy(uint16_t* str, uint16_t len) : str_(str), len_(len), prePos_(0) {}

    ~UILineBreakProxy()
    {
        str_ = nullptr;
        len_ = 0;
        prePos_ = 0;
    }

    /**
     * @brief Get next line break position.
     *
     * @return uint16_t Next line break position.
     */
    uint16_t GetNextBreakPos()
    {
        uint16_t offsetFromPrePos = UILineBreakEngine::GetInstance().GetNextBreakPos(*this);
        prePos_ += offsetFromPrePos;
        return prePos_;
    }

    /**
     * @brief Get the length of string.
     *
     * @return uint16_t The length of string.
     */
    uint16_t GetStrLen() const
    {
        if (prePos_ < len_) {
            return len_ - prePos_;
        }
        return 0;
    }

    /**
     * @brief Get the string.
     *
     * @return uint16_t* The str setted.
     */
    const uint16_t* GetStr() const
    {
        if (prePos_ < len_) {
            return &(str_[prePos_]);
        }
        return nullptr;
    }

private:
    uint16_t* str_;
    uint16_t len_;
    uint16_t prePos_;
};
} // namespace OHOS
#endif // ENABLE_ICU
#endif // GRAPHIC_LITE_LINE_BREAK_H
