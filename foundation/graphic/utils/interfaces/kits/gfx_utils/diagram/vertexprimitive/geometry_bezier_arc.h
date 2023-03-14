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

/**
 * @addtogroup GraphicGeometry
 * @{
 *
 * @brief Defines ArcToBezier BezierArc BezierArcSvg.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GEOMETRY_BEZIER_ARC_H
#define GRAPHIC_LITE_GEOMETRY_BEZIER_ARC_H

#include "gfx_utils/diagram/depiction/depict_transform.h"
#include "gfx_utils/heap_base.h"
namespace OHOS {
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
const int32_t BEZIER_ARC_VERTEX_NUM = 26; // Number of Bezier Arc Vertex Coordinates.
/**
 * @brief Arcs are converted to Bezier curves.
 *
 * @since 1.0
 * @version 1.0
 */
void ArcToBezier(float cx, float cy, float rx, float ry,
                 float startAngle, float sweepAngle,
                 float* curve);

/**
 * @file geometry_bezier_arc.h
 *
 * @brief Defines Bezier arc structure class.
 *
 * @since 1.0
 * @version 1.0
 */
class BezierArc : public HeapBase {
public:
    BezierArc()
        : vertexIndex_(BEZIER_ARC_VERTEX_NUM), numberVertices_(0), currentCommand_(PATH_CMD_LINE_TO) {}

    /**
     * @brief Bezier curves are initialized with arcs.
     * @Param centerX centerY Arc Center.
     * @Param rx ry Arc Transverse and Longitudinal Radius.
     * @Param startAngle sweepAngle Arc start and end angle.
     * @since 1.0
     * @version 1.0
     */
    BezierArc(float centerX, float centerY,
              float rx, float ry,
              float startAngle,
              float sweepAngle)
    {
        Init(centerX, centerY, rx, ry, startAngle, sweepAngle);
    }

    /**
     * @brief Initializing Bezier Curves with Arcs.
     * @since 1.0
     * @version 1.0
     */
    void Init(float centerX, float centerY,
              float rx, float ry,
              float startAngle,
              float sweepAngle);

    /**
     * @brief Back to the beginning.
     *
     * @since 1.0
     * @version 1.0
     */
    void Rewind(uint32_t)
    {
        vertexIndex_ = 0;
    }

    /**
     * @brief Called during the sampling phase.
     * @param x is the pointer to the value of the point coordinate X.
     * @param y is the pointer to the value of the point coordinate Y.
     * @return Path command
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(float* x, float* y)
    {
        if (vertexIndex_ >= numberVertices_) {
            return PATH_CMD_STOP;
        }
        *x = arrayVertices_[vertexIndex_];
        *y = arrayVertices_[vertexIndex_ + 1];
        vertexIndex_ = vertexIndex_ + TWO_STEP;
        if (vertexIndex_ == TWO_STEP) {
            return uint32_t(PATH_CMD_MOVE_TO);
        } else {
            return currentCommand_;
        }
    }

    /**
     * @brief Get the number of vertex sources.
     * @return Number of vertices.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetNumberVertices() const
    {
        return numberVertices_;
    }

    /**
     * @brief Get vertex source.
     * @return Vertex Source Data.
     * @since 1.0
     * @version 1.0
     */
    const float* GetVertices() const
    {
        return arrayVertices_;
    }

    /**
     * @brief Get vertex source.
     * @return Vertex Source Data.
     * @since 1.0
     * @version 1.0
     */
    float* GetVertices()
    {
        return arrayVertices_;
    }

private:
    uint32_t vertexIndex_;
    uint32_t numberVertices_;
    float arrayVertices_[BEZIER_ARC_VERTEX_NUM];
    uint32_t currentCommand_;
};

/**
 * @brief Compute SVG-style Bezier arcs.
 *
 * @since 1.0
 * @version 1.0
 */
class BezierArcSvg : public HeapBase {
public:
    BezierArcSvg() : bezierArcModel_(), isRadiusJoinPath_(false) {}

    /**
     * @brief Calculates elliptical arcs from (x1, y1) to (x2, y2).
     * The direction of the ellipse is defined by two radii (rx, ry).
     * @param x1,y1 Starting point coordinates.
     * @param x2,y2 End point coordinates.
     * @param rx,ry Radius of major and minor axes
     * @param angle angle,
     * @param largeArcFlag Large Arc Mark
     * @param sweepFlag Sweep angle sign
     * @since 1.0
     * @version 1.0
     */
    BezierArcSvg(float x1,
                 float y1,
                 float rx, float ry,
                 float angle,
                 bool largeArcFlag, // Large Arc Mark
                 bool sweepFlag,    // Sweep angle sign
                 float x2, float y2) : bezierArcModel_(), isRadiusJoinPath_(false)
    {
        Init(x1, y1, rx, ry, angle, largeArcFlag, sweepFlag, x2, y2);
    }

    bool RadiiOK() const
    {
        return isRadiusJoinPath_;
    }

    void Init(float x1,
              float y1,
              float rx,
              float ry,
              float angle,
              bool largeArcFlag,
              bool sweepFlag,
              float x2,
              float y2);

    /**
     * @brief Go back to the first step.
     *
     * @since 1.0
     * @version 1.0
     */
    void Rewind(uint32_t)
    {
        bezierArcModel_.Rewind(0);
    }

    /**
     * @brief Called during the sampling phase.
     * @param X is the pointer to the value of the point coordinate X
     * @param Y is the pointer to the value of the point coordinate Y
     * @return Path command
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(float* x, float* y)
    {
        return bezierArcModel_.GenerateVertex(x, y);
    }

    /**
     * @brief Returns the vertex data of a Bezier arc.
     * @return Return vertex source.
     * @since 1.0
     * @version 1.0
     */
    const float* GetVertices() const
    {
        return bezierArcModel_.GetVertices();
    }

    /**
     * @brief Returns the vertex data of a Bezier arc.
     * @return Returns the vertex data of a Bezier arc.
     * @since 1.0
     * @version 1.0
     */
    float* GetVertices()
    {
        return bezierArcModel_.GetVertices();
    }

    /**
     * @brief Returns twice the number of value vertices,that is, for a vertex, it returns 2.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetNumberVertices() const
    {
        return bezierArcModel_.GetNumberVertices();
    }

private:
    BezierArc bezierArcModel_;
    bool isRadiusJoinPath_;
};
#endif
} // namespace OHOS
#endif
