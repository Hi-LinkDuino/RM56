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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_FADE_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_FADE_PAINTER_H

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/color.h"
#include "core/components/scroll/scroll_fade_painter.h"
#include "core/pipeline/base/render_context.h"

namespace flutter {
class Canvas;
class Paint;
class PaintData;
} // namespace flutter

namespace OHOS::Ace {

class FlutterScrollFadePainter : public ScrollFadePainter {
public:
    void PaintSide(RenderContext& context, const Size& size, const Offset& offset) override;

private:
    void Paint(flutter::Canvas* canvas, const Size& size, const Offset& offset);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_SCROLL_FADE_PAINTER_H
