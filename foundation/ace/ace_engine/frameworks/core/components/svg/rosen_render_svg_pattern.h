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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATTERN_H

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"

#include "frameworks/core/components/svg/render_svg_pattern.h"
#include "frameworks/core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

class RosenRenderSvgPattern : public RenderSvgPattern {
    DECLARE_ACE_TYPE(RosenRenderSvgPattern, RenderSvgPattern);

public:
    void Paint(RenderContext& context, const Offset& offset) override;
    bool OnAsPaint(const Offset& offset, const Rect& paintRect, SkPaint& skPaint);

protected:
    bool FitAttribute(const Rect& paintRect, Rect& tileRect, SkMatrix& skMatrix4);
    void FitRenderContext(RosenRenderContext& context, const Rect& patternRect);

private:
    void ResetAttrOffset();
    const Matrix4 GetTransform(const Rect& patternRect) const;

    float scaleY_ = 1.0f;
    float scaleX_ = 1.0f;
    float scale_ = 1.0f;
    float tx_ = 0.0f;
    float ty_ = 0.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATTERN_H
