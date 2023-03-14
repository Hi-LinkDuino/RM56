/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_SELECTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_SELECTION_H

#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

#include "core/common/ime/constant.h"

namespace OHOS::Ace {

/**
 * Stands for what text is selected and how the directional is.
 * We use base/extent to indicate the start/end position because of uncertain direction.
 */
struct TextSelection {
    TextSelection() = default;
    TextSelection(int32_t base, int32_t extent) : baseOffset(base), extentOffset(extent) {}

    void Update(int32_t base, int32_t extent)
    {
        baseOffset = base;
        extentOffset = extent;
    }

    // Usually called when none is selected.
    void Update(int32_t both)
    {
        baseOffset = both;
        extentOffset = both;
    }

    bool operator==(const TextSelection& other) const
    {
        return baseOffset == other.baseOffset && extentOffset == other.extentOffset;
    }

    bool operator!=(const TextSelection& other) const
    {
        return !operator==(other);
    }

    /**
     * @brief Get first selected character in the text.
     */
    inline int32_t GetStart() const
    {
        return std::min(baseOffset, extentOffset);
    }

    /**
     * @brief Get last selected character in the text.
     */
    inline int32_t GetEnd() const
    {
        return std::max(baseOffset, extentOffset);
    }

    /**
     * @brief Whether selection is valid.
     */
    inline bool IsValid() const
    {
        return baseOffset > -1 && extentOffset > -1;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Selection (" << baseOffset << ", " << extentOffset << ")";
        std::string output = ss.str();
        return output;
    }

    // May larger than, smaller than or equal to extentOffset.
    int32_t baseOffset = -1;

    // When paints caret, this is where the caret position is.
    int32_t extentOffset = -1;

    TextAffinity affinity = TextAffinity::UPSTREAM;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_SELECTION_H
