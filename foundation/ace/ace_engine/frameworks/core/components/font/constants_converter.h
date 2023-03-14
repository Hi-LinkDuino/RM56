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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_CONSTANTS_CONVERTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_CONSTANTS_CONVERTER_H

#include "flutter/third_party/txt/src/txt/text_baseline.h"
#include "flutter/third_party/txt/src/txt/text_decoration.h"
#include "flutter/third_party/txt/src/txt/text_style.h"
#include "third_party/skia/include/core/SkColor.h"

#include "core/components/common/properties/text_style.h"
#include "core/pipeline/pipeline_context.h"

namespace txt {
enum class FontWeight;
enum class FontStyle;
enum class TextAlign;
enum class TextDirection;
} // namespace txt

namespace OHOS::Ace {

enum class FontWeight;
enum class FontStyle;
enum class TextBaseline;
enum class TextAlign;
enum class TextDirection;
enum class TextDecoration;
enum class WhiteSpace;
class Color;

namespace Constants {

txt::FontWeight ConvertTxtFontWeight(FontWeight fontWeight);

txt::FontStyle ConvertTxtFontStyle(FontStyle fontStyle);

txt::TextBaseline ConvertTxtTextBaseline(TextBaseline textBaseline);

txt::TextAlign ConvertTxtTextAlign(TextAlign textAlign);

txt::TextDirection ConvertTxtTextDirection(TextDirection textDirection);

SkColor ConvertSkColor(Color color);

txt::TextDecoration ConvertTxtTextDecoration(TextDecoration textDecoration);

void ConvertTxtStyle(const TextStyle& textStyle, const WeakPtr<PipelineContext>& context, txt::TextStyle& txtStyle);

Rect ConvertSkRect(SkRect skRect);

} // namespace Constants

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_CONSTANTS_CONVERTER_H
