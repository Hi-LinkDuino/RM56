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

#include "core/components/common/painter/rosen_checkable_painter.h"

#include "third_party/skia/include/core/SkPaint.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t ENABLED_ALPHA = 255;
constexpr uint8_t DISABLED_ALPHA = 102;

} // namespace

void RosenCheckablePainter::SetStrokeWidth(double strokeWidth, SkPaint& skPaint) const
{
    skPaint.setAntiAlias(true);
    skPaint.setStyle(SkPaint::Style::kStroke_Style);
    skPaint.setStrokeWidth(strokeWidth);
}

float RosenCheckablePainter::ConfigureOpacity(bool disabled)
{
    uint8_t opacity = disabled ? DISABLED_ALPHA : ENABLED_ALPHA;
    return opacity / 255.0;
}

} // namespace OHOS::Ace
