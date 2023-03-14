/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_CONSTANT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_CONSTANT_H

namespace OHOS::Ace {

// Invalid client id.
constexpr int32_t IME_CLIENT_ID_NONE = 0;
// Where client id start from.
constexpr int32_t IME_CLIENT_ID_START = 1;

enum class TextAffinity {
    // The affinity of [upstream], which means the caret is tightly beside prev glyphs.
    // For example, we have a text named "abcde", and assume extent=3, it will result in the offset after 'c'
    // rather than before 'd'.
    // In most cases it doesn't matter much, but it matters if 'c' and 'd' have different style.
    UPSTREAM,
    DOWNSTREAM,
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_CONSTANT_H
