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

#include "core/components/common/painter/flutter_checkable_painter.h"

#include "third_party/skia/include/core/SkPaint.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t ENABLED_ALPHA = 255;
constexpr uint8_t DISABLED_ALPHA = 102;

} // namespace

void FlutterCheckablePainter::SetStrokeWidth(double strokeWidth, flutter::Paint& paint) const
{
    SkPaint* skPaint = paint.paint();
    if (!skPaint) {
        LOGE("paint is null");
        return;
    }

    skPaint->setAntiAlias(true);
    skPaint->setStyle(SkPaint::Style::kStroke_Style);
    skPaint->setStrokeWidth(strokeWidth);
}

RenderLayer FlutterCheckablePainter::ConfigureOpacityLayer(bool disabled)
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::OpacityLayer>(ENABLED_ALPHA, 0, 0);
    }
    uint8_t opacity = disabled ? DISABLED_ALPHA : ENABLED_ALPHA;
    if (layer_) {
        layer_->SetOpacity(opacity, 0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

} // namespace OHOS::Ace
