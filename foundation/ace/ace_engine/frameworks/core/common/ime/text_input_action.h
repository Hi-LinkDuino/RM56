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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_ACTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_ACTION_H

#include <cstdint>

namespace OHOS::Ace {

/**
 * Define what kind of action when "enter" pressed stands for.
 * Attention: Should keep sync with C++.
 */
enum class TextInputAction {
    BEGIN = 0,
    UNSPECIFIED = BEGIN,
    NONE = 1,
    GO = 2,
    SEARCH = 3,
    SEND = 4,
    NEXT = 5,
    DONE = 6,
    PREVIOUS = 7,
    END = PREVIOUS,
};

// For convenience of convert int value from Java
TextInputAction CastToTextInputAction(int32_t value);

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_ACTION_H
