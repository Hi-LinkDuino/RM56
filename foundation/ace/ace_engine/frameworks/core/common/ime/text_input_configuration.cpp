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

#include "core/common/ime/text_input_configuration.h"

#include "base/json/json_util.h"

namespace OHOS::Ace {
namespace {

// Negotiated fields with Java
const char TYPE[] = "type";
const char OBSCURE_TEXT[] = "obscureText";
const char ACTION_LABEL[] = "actionLabel";
const char ACTION[] = "action";
const char AUTO_CORRECT[] = "autoCorrect";
const char CAPITALIZATION[] = "capitalization";
const char KEYBOARD_APPEARANCE[] = "keyboardAppearance";

} // namespace

std::string TextInputConfiguration::ToJsonString() const
{
    auto json = JsonUtil::Create(true);
    json->Put(TYPE, static_cast<int32_t>(type));
    json->Put(OBSCURE_TEXT, obscureText);
    json->Put(ACTION, static_cast<int32_t>(action));
    if (!actionLabel.empty()) {
        json->Put(ACTION_LABEL, actionLabel.c_str());
    }

    json->Put(AUTO_CORRECT, autoCorrect);
    if (!capitalization.empty()) {
        json->Put(CAPITALIZATION, capitalization.c_str());
    }
    if (!keyboardAppearance.empty()) {
        json->Put(KEYBOARD_APPEARANCE, keyboardAppearance.c_str());
    }

    return json->ToString();
}

} // namespace OHOS::Ace
