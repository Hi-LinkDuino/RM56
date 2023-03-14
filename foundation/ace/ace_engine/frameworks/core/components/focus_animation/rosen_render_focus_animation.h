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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_ROSEN_RENDER_FOCUS_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_ROSEN_RENDER_FOCUS_ANIMATION_H

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "core/components/focus_animation/render_focus_animation.h"

namespace OHOS::Ace {

class RosenRenderFocusAnimation : public RenderFocusAnimation {
    DECLARE_ACE_TYPE(RosenRenderFocusAnimation, RenderFocusAnimation);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

    bool IsRepaintBoundary() const override
    {
        return RenderNode::IsRepaintBoundary();
    }

private:
    void PaintGlow(SkCanvas* canvas, SkPaint& paint, int32_t padding) const;
    void PaintClipRect(SkCanvas* skCanvas, double offset) const;
    void PaintTVFocus(SkCanvas* skCanvas);
    void PaintPhoneFocus(SkCanvas* skCanvas);

    double width_ = 0.0;
    double height_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_ROSEN_RENDER_FOCUS_ANIMATION_H
