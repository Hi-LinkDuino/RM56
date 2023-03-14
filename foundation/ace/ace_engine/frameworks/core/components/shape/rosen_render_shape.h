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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_ROSEN_RENDER_SHAPE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_ROSEN_RENDER_SHAPE_H

#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

#include "core/components/shape/render_shape.h"

namespace OHOS::Ace {

class RosenRenderShape : public RenderShape {
    DECLARE_ACE_TYPE(RosenRenderShape, RenderShape);

public:
    void Paint(RenderContext& context, const Offset& offset) override;
    Size CalcSize() override;
    void PaintOnCanvas(SkCanvas* skCanvas, const Offset& offset);

private:
    Size CreateRect();
    Size CreateCircle();
    Size CreateEllipse();
    Size CreatePolygon(bool needClose);
    Size CreatePath();
    void DrawStroke(SkCanvas* skCanvas, const SkPath& path);
    float GetFloatRadiusValue(const Dimension& src, const Dimension& dest, bool isVertical);

    SkPath path_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_ROSEN_RENDER_SHAPE_H
