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

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "core/components/bubble/render_bubble.h"
#include "core/pipeline/base/flutter_render_context.h"

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_FLUTTER_RENDER_BUBBLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_FLUTTER_RENDER_BUBBLE_H

namespace OHOS::Ace {

class FlutterRenderBubble : public RenderBubble {
    DECLARE_ACE_TYPE(FlutterRenderBubble, RenderBubble);

public:
    FlutterRenderBubble() = default;
    ~FlutterRenderBubble() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

private:
    bool hasEventFired_ = false;
    SkPath path_;
    SkRRect rrect_;

    SkRRect MakeRRect();
    void UpdateBorderRadius() override;
    void PaintMask(RenderContext& context);
    void PaintBubble(RenderContext& context);
    void PaintTopBubble(SkCanvas* skCanvas, SkPaint paint);
    void PaintBottomBubble(SkCanvas* skCanvas, SkPaint paint);
    void PaintDefaultBubble(SkCanvas* skCanvas, SkPaint paint);
    void PaintShadow(SkCanvas* skCanvas);
    void PaintBorder(RenderContext& context);
    SkCanvas* GetSkCanvas(RenderContext& context);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_FLUTTER_RENDER_BUBBLE_H
