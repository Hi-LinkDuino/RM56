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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_FLUTTER_RENDER_SHAPE_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_FLUTTER_RENDER_SHAPE_CONTAINER_H

#include "core/components/custom_paint/offscreen_canvas.h"
#include "core/components/shape/flutter_render_shape.h"
#include "core/pipeline/layers/picture_layer.h"
#include "frameworks/core/components/shape/render_shape_container.h"
#include "frameworks/core/pipeline/layers/transform_layer.h"

namespace OHOS::Ace {

class FlutterRenderShapeContainer : public RenderShapeContainer {
    DECLARE_ACE_TYPE(FlutterRenderShapeContainer, RenderShapeContainer);

public:
    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void BitmapMesh(RenderContext& context, const Offset& offset);
    void DrawBitmapMesh(SkBitmap& bitmap, int column, int row,
        const float* vertices, const int* colors, const SkPaint* paint);
    RefPtr<FlutterRenderShape> GetShapeChild(const RefPtr<RenderNode>& node);
    bool IsRepaintBoundary() const override
    {
        return true;
    }

private:
    RefPtr<Flutter::TransformLayer> transformLayer_;
    SkCanvas* skCanvas_;
    // Flutter::PictureLayer pictureLayer_;
    SkBitmap skOffBitmap_;
    std::unique_ptr<SkCanvas> skOffCanvas_;

};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_FLUTTER_RENDER_SHAPE_CONTAINER_H
