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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_FLUTTER_RENDER_TIP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_FLUTTER_RENDER_TIP_H

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "core/components/tip/render_tip.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

class FlutterRenderTip final : public RenderTip {
DECLARE_ACE_TYPE(FlutterRenderTip, RenderTip);

public:
    FlutterRenderTip() = default;
    ~FlutterRenderTip() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

private:
    SkCanvas* GetSkCanvas(RenderContext& context);
    void PaintTip(RenderContext& context, const Offset& offset);
    void PaintTopTip(SkCanvas* skCanvas, SkPaint paint, const Offset& offset);
    void PaintLeftTip(SkCanvas* skCanvas, SkPaint paint, const Offset& offset);

    SkPath path_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_FLUTTER_RENDER_TIP_H
