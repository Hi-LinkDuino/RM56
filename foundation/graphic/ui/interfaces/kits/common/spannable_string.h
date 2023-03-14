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

#ifndef SPANNABLE_STRING_H
#define SPANNABLE_STRING_H
#include "gfx_utils/list.h"
#include "font/ui_font_header.h"
namespace OHOS {
#if ENABLE_VECTOR_FONT
class StyleSpan : public HeapBase {
public:
    StyleSpan() {};
    ~StyleSpan() {};
    TextStyle textStyle_;
    uint16_t start_;
    uint16_t end_;
    StyleSpan(TextStyle textStyle, uint16_t start, uint16_t end)
    {
        textStyle_ = textStyle;
        start_ = start;
        end_ = end;
    };
};
class SpannableString : public HeapBase {
public:
    SpannableString() {};
    ~SpannableString();
    SpannableString(const char* text);
    char* text_;
    List<StyleSpan*> spanList_;
    void SetTextStyle(TextStyle textStyle, uint16_t startIndex, uint16_t endIndex);
};
#endif
}
#endif
