/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_CHECKBOX_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_CHECKBOX_H

#include "core/components/checkable/render_checkbox.h"
#include "core/components/common/painter/flutter_checkable_painter.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

class FlutterRenderCheckbox : public RenderCheckbox, public FlutterCheckablePainter {
    DECLARE_ACE_TYPE(FlutterRenderCheckbox, RenderCheckbox);

public:
    FlutterRenderCheckbox() = default;
    ~FlutterRenderCheckbox() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    RenderLayer GetRenderLayer() override
    {
        return ConfigureOpacityLayer(disabled_);
    }

private:
    void DrawCheck(const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint,
        const flutter::Paint& shadowPaint) const;
    void DrawActiveBorder(const ScopedCanvas& canvas, const Offset& paintOffset, flutter::Paint& strokePaint) const;
    void DrawActiveSquare(const ScopedCanvas& canvas, const Offset& paintOffset, flutter::Paint& strokePaint) const;
    void SetUIStatus(ScopedCanvas& canvas, Offset& paintOffset,
        flutter::Paint& strokePaint, flutter::Paint& shadowPaint);
    void DrawUnselected(
        const ScopedCanvas& canvas, const Offset& origin, uint32_t paintColor, flutter::Paint& paint) const;
    void DrawBorder(
        const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const Size& paintSize) const;
    void DrawSquare(
        const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const Size& paintSize) const;
    void DrawAnimationOffToOn(ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint,
        const flutter::Paint& shadowPaint) const;
    void DrawAnimationOnToOff(ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint,
        const flutter::Paint& shadowPaint) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_FLUTTER_RENDER_CHECKBOX_H
