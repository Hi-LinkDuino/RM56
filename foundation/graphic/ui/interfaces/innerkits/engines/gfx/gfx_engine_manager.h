/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_GFX_ENGINE_MANAGER_H
#define GRAPHIC_LITE_GFX_ENGINE_MANAGER_H

#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/style.h"
#include "gfx_utils/transform.h"

namespace OHOS {
class BaseGfxEngine;
enum BlendMode {
    BLEND_MODE,          /* no blending */
    BLEND_SRC,           /* S */
    BLEND_DST,           /* D */
    BLEND_SRC_OVER,      /* S + (1 - Sa) * D */
    BLEND_DST_OVER,      /* (1 - Da) * S + D */
    BLEND_SRC_IN,        /* Da * S */
    BLEND_DST_IN,        /* Sa * D */
    BLEND_SRC_OUT,       /* S * (1 - Da) */
    BLEND_DST_OUT,       /* D * (1 - Sa) */
    BLEND_SCREEN,        /* S + D - S * D */
    BLEND_MULTIPLY,      /* S * (1 - Da) + D * (1 - Sa) + S * D */
    BLEND_ADDITIVE,      /* S + D */
    BLEND_SUBTRACT,      /* D * (1 - S) */
};

#ifndef TRANSFORMOPTION
#define TRANSFORMOPTION
struct TransformOption {
    TransformAlgorithm algorithm;
};
#endif

struct BlendOption {
    TransformMap transMap;
    BlendMode mode;
    TransformOption option;
    OpacityType opacity;
};

class Image;
struct ArcInfo {
    Point center;
    Point imgPos;
    uint16_t radius;
    int16_t startAngle;
    int16_t endAngle;
    const Image* imgSrc;
};

struct TransformDataInfo {
    ImageHeader header;
    const uint8_t* data;
    uint8_t pxSize;
    BlurLevel blurLevel;
    TransformAlgorithm algorithm;
    /** User-defined data */
    void* userData;
    /** Blit Flag*/
    bool blitFlag;
};

enum BufferInfoUsage {
    BUFFER_FB_SURFACE,
    BUFFER_MAP_SURFACE,
    BUFFER_SNAPSHOT_SURFACE
};

class BaseGfxEngine : public HeapBase {
public:
    virtual void DrawArc(BufferInfo& dst, ArcInfo& arcInfo, const Rect& mask,
                         const Style& style, OpacityType opacity, uint8_t cap);

    virtual void DrawLine(BufferInfo& dst, const Point& start, const Point& end,
                          const Rect& mask, int16_t width, ColorType color, OpacityType opacity);

    virtual void DrawLetter(BufferInfo& gfxDstBuffer,
                            const uint8_t* fontMap,
                            const Rect& fontRect,
                            const Rect& subRect,
                            const uint8_t fontWeight,
                            const ColorType& color,
                            const OpacityType opa);

    virtual void DrawCubicBezier(BufferInfo& dst, const Point& start, const Point& control1,
                                 const Point& control2, const Point& end, const Rect& mask,
                                 int16_t width, ColorType color, OpacityType opacity);

    virtual void DrawRect(BufferInfo& dst,
                          const Rect& rect,
                          const Rect& dirtyRect,
                          const Style& style,
                          OpacityType opacity);

    virtual void DrawTransform(BufferInfo& dst,
                               const Rect& mask,
                               const Point& position,
                               ColorType color,
                               OpacityType opacity,
                               const TransformMap& transMap,
                               const TransformDataInfo& dataInfo);

    // x/y: center of a circle
    virtual void ClipCircle(const ImageInfo* info, float x, float y, float radius);

    virtual void Blit(BufferInfo& dst,
                      const Point& dstPos,
                      const BufferInfo& src,
                      const Rect& subRect,
                      const BlendOption& blendOption);

    virtual void Fill(BufferInfo& dst,
                      const Rect& fillArea,
                      const ColorType color,
                      const OpacityType opacity);

    virtual uint8_t* AllocBuffer(uint32_t size, uint32_t usage);

    virtual void FreeBuffer(uint8_t* buffer);

    virtual BufferInfo* GetFBBufferInfo()
    {
        return nullptr;
    }

    virtual void Flush() {}

    virtual uint16_t GetScreenWidth()
    {
        return width_;
    }

    virtual uint16_t GetScreenHeight()
    {
        return height_;
    }

    virtual void SetScreenShape(ScreenShape screenShape)
    {
        screenShape_ = screenShape;
    }

    virtual ScreenShape GetScreenShape()
    {
        return screenShape_;
    }

    static BaseGfxEngine* GetInstance()
    {
        return baseEngine_;
    }

    static void InitGfxEngine(BaseGfxEngine* gfxEngine = nullptr)
    {
        if (gfxEngine == nullptr) {
            static BaseGfxEngine localGfxEngine;
            baseEngine_ = &localGfxEngine;
            return;
        }
        baseEngine_ = gfxEngine;
    }
protected:
    static BaseGfxEngine* baseEngine_;
    uint16_t width_ = HORIZONTAL_RESOLUTION;
    uint16_t height_ = VERTICAL_RESOLUTION;
    ScreenShape screenShape_ = RECTANGLE;
};
}

#endif // GRAPHIC_LITE_GFX_ENGINE_MANAGER_H
