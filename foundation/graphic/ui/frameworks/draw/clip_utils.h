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

#ifndef GRAPHIC_LITE_CLIP_UTILS_H
#define GRAPHIC_LITE_CLIP_UTILS_H

#include "gfx_utils/diagram/vertexprimitive/geometry_path_storage.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "render/render_buffer.h"
#include "render/render_scanline.h"

namespace OHOS {
/*
 * Indicates a path to be cliped.
 * Note: The path will be automatically closed. Only non-self-intersecting path are supported.
 */
class ClipPath : public HeapBase {
public:
    ClipPath()
    {
        vertices_ = new UICanvasVertices();
    }
    ~ClipPath()
    {
        if (vertices_ != nullptr) {
            vertices_->FreeAll();
            delete vertices_;
            vertices_ = nullptr;
        }
    }

    ClipPath& MoveTo(const PointF& point);
    ClipPath& LineTo(const PointF& point);
    ClipPath& CurveTo(const PointF& control1, const PointF& control2, const PointF& end);
    ClipPath& Arc(const PointF& center, float radius, int16_t startAngle, int16_t endAngle);
    ClipPath& Circle(const PointF& center, float radius);

    UICanvasVertices& GetVertices() const
    {
        return *vertices_;
    }
private:
    UICanvasVertices* vertices_;
};

class ClipUtils : public HeapBase {
public:
    ClipUtils() {}
    ~ClipUtils() {}

    void PerformScan(const ClipPath& path, const ImageInfo* imageInfo);
private:
    UICanvasVertices& CloseVertices(const ClipPath& path);
    void DrawPixel(int16_t x, int16_t y, uint8_t opa, const ImageInfo* imageInfo);
    void DrawHorLine(int16_t x, int16_t y, int16_t width, uint8_t opa, const ImageInfo* imageInfo);
};
};
#endif
