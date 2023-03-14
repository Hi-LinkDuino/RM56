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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONFIGURATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONFIGURATION_H

#include <string>

#include "core/common/ime/text_input_action.h"
#include "core/common/ime/text_input_type.h"

namespace OHOS::Ace {

struct TextInputConfiguration {
    std::string ToJsonString() const;

    // Currently this is which only supported.
    TextInputType type = TextInputType::TEXT;
    // Obscure support for password and so on.
    bool obscureText = false;
    // What label to show on action button. For e.g. GO / Search!
    std::string actionLabel;
    TextInputAction action = TextInputAction::UNSPECIFIED;

    // Below maybe supported in the future. Just keep for placeholder.
    bool autoCorrect = false;
    std::string capitalization;
    std::string keyboardAppearance;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_CONFIGURATION_H
