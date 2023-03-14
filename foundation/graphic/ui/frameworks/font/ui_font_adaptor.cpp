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

#include "font/ui_font_adaptor.h"
#include "common/typed_text.h"
#include "graphic_config.h"
#if ENABLE_ICU
#include "font/ui_line_break.h"
#endif
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif
namespace OHOS {
uint32_t UIFontAdaptor::GetNextLineAndWidth(const char* txt,
                                            uint8_t fontId,
                                            uint8_t fontSize,
                                            int16_t letterSpace,
                                            int16_t& maxWidth,
                                            int16_t& maxHeight,
                                            uint16_t& letterIndex,
                                            SizeSpan* sizeSpans,
                                            bool allBreak,
                                            uint16_t len)
{
#if ENABLE_ICU
    return UILineBreakEngine::GetInstance().GetNextLineAndWidth(txt, fontId, fontSize, letterSpace, allBreak, maxWidth,
                                                                maxHeight, letterIndex, sizeSpans, len);
#else
    uint32_t index = TypedText::GetNextLine(txt, fontId, fontSize, letterSpace, maxWidth);
    maxWidth = TypedText::GetTextWidth(txt, fontId, fontSize, index, letterSpace);
    return index;
#endif
}

bool UIFontAdaptor::IsSameTTFId(uint8_t fontId, uint32_t unicode)
{
#if ENABLE_SHAPING
    return UITextShaping::IsSameTTFId(fontId, unicode);
#else
    return true;
#endif
}
} // namespace OHOS
