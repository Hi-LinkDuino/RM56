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

#include "common/spannable_string.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
namespace OHOS {
#if ENABLE_VECTOR_FONT
SpannableString::~SpannableString()
{
    if (text_ != nullptr) {
        UIFree(text_);
        text_ = nullptr;
    }
}
SpannableString::SpannableString(const char* text)
{
    if (text == nullptr) {
        return;
    }
    uint32_t textLen = static_cast<uint32_t>(strlen(text));
    if (textLen > MAX_TEXT_LENGTH) {
        return;
    }
    text_ = static_cast<char*>(UIMalloc(++textLen));
    if (text_ == nullptr) {
        return;
    }
    if (memcpy_s(text_, textLen, text, textLen) != EOK) {
        UIFree(text_);
        text_ = nullptr;
        return;
    }
}
void SpannableString::SetTextStyle(TextStyle textStyle, uint16_t startIndex, uint16_t endIndex)
{
    StyleSpan* span = new StyleSpan(textStyle, startIndex, endIndex);
    spanList_.PushBack(span);
}
#endif
}
