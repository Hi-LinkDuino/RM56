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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_CHECKBOX_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_CHECKBOX_H

#include "core/components/checkable/render_checkbox.h"
#include "core/components/common/painter/rosen_checkable_painter.h"

namespace OHOS::Ace {

class RosenRenderCheckbox : public RenderCheckbox, public RosenCheckablePainter {
    DECLARE_ACE_TYPE(RosenRenderCheckbox, RenderCheckbox);

public:
    RosenRenderCheckbox() = default;
    ~RosenRenderCheckbox() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void DrawCheck(SkCanvas* canvas, const Offset& origin, const SkPaint& paint, const SkPaint& shadowPaint) const;
    void DrawActiveBorder(SkCanvas* canvas, const Offset& paintOffset, SkPaint& strokePaint) const;
    void DrawActiveSquare(SkCanvas* canvas, const Offset& paintOffset, SkPaint& strokePaint) const;
    void DrawUnselected(SkCanvas* canvas, const Offset& origin, uint32_t paintColor, SkPaint& paint) const;
    void SetUIStatus(SkCanvas* canvas, const Offset& paintOffset, SkPaint& strokePaint, SkPaint& shadowPaint);
    void DrawBorder(SkCanvas* canvas, const Offset& origin, const SkPaint& paint, const Size& paintSize) const;
    void DrawAnimationOffToOn(
        SkCanvas* canvas, const Offset& origin, const SkPaint& paint, const SkPaint& shadowPaint) const;
    void DrawSquare(
        SkCanvas* canvas, const Offset& origin, const SkPaint& paint, const Size& paintSize) const;
    void DrawAnimationOnToOff(
        SkCanvas* canvas, const Offset& origin, const SkPaint& paint, const SkPaint& shadowPaint) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_ROSEN_RENDER_CHECKBOX_H
