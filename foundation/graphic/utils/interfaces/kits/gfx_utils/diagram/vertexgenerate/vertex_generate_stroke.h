/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LTE_VERTEX_GENERATE_STROKE_H
#define GRAPHIC_LTE_VERTEX_GENERATE_STROKE_H

#include "gfx_utils/diagram/vertexprimitive/geometry_math_stroke.h"
namespace OHOS {
class VertexGenerateStroke {
    enum Status {
        /** init state */
        INITIAL,
        /** ready state */
        READY,
        /** linecap start draw state */
        LINECAP_START,
        /** linecap end draw state */
        LINECAP_END,
        /** linejoin start draw state */
        LINEJOIN_START,
        /** close first curve state */
        CLOSE_FIRST,
        /** linejoin end draw state */
        LINEJOIN_END,
        /** closing end state */
        OUT_VERTICES,
        /** Clockwise needle end polygon */
        CLOCKWISE_ENDPOLY,
        /** Clockwise needle end polygon */
        ANTI_CLOCKWISE_ENDPOLY,
        /** end drawing state */
        STOP
    };

public:
    VertexGenerateStroke();

#if GRAPHIC_ENABLE_LINECAP_FLAG
    void SetLineCap(LineCap lc)
    {
        stroker_.SetLineCap(lc);
    }
    LineCap GetLineCap() const
    {
        return stroker_.GetLineCap();
    }
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    void SetLineJoin(LineJoin lj)
    {
        stroker_.SetLineJoin(lj);
    }

    void SetMiterLimit(float ml)
    {
        stroker_.SetMiterLimit(ml);
    }
    LineJoin GetLineJoin() const
    {
        return stroker_.GetLineJoin();
    }
    float GetMiterLimit() const
    {
        return stroker_.GetMiterLimit();
    }
#endif
    void SetWidth(float width)
    {
        stroker_.SetWidth(width);
    }

    void SetApproximationScale(float approximationScale)
    {
        stroker_.SetApproximationScale(approximationScale);
    }

    float GetWidth() const
    {
        return stroker_.GetWidth();
    }

    float GetApproximationScale() const
    {
        return stroker_.GetApproximationScale();
    }

    void SetStrokeShorten(float strokeShorten)
    {
        strokeShorten_ = strokeShorten;
    }

    float GetStrokeShorten() const
    {
        return strokeShorten_;
    }

    void RemoveAll();

    void AddVertex(float x, float y, uint32_t cmd);

    void Rewind(uint32_t pathId);
    uint32_t GenerateVertex(float* x, float* y);
    VertexGenerateFlags GetGenerateFlags()
    {
        return GENERATE_STROKE;
    }
private:
    VertexGenerateStroke(const VertexGenerateStroke&);
    const VertexGenerateStroke& operator=(const VertexGenerateStroke&);
    GeometryMathStroke stroker_;
    GeometryVertexSequence srcVertices_;
    Graphic::Vector<PointF> outVertices_;
    float strokeShorten_;
    uint32_t closed_;
    Status status_;
    Status prevStatus_;
    uint32_t srcVertex_;
    uint32_t outVertex_;

    void VertexReady(const uint32_t& verticesNum, uint32_t& cmd);
    void VertexLineCapStart();
    void VertexLineCapEnd(const uint32_t& verticesNum);
    void VertexLineJoinStart();
    void VertexLineJoinEnd();
    void VertexCloseFirst(uint32_t& cmd);
    bool IsVertexOutVertices(float* x, float* y);
};
} // namespace OHOS

#endif
