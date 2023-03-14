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
 * @file depict_curve.h
 *
 * @brief Defines Building curve transformation pipes
 * The curve drawing is usually approximated by a series of short line segments,
 * which is the only efficient method to draw the curve
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_DEPICT_CURVE_H
#define GRAPHIC_LITE_DEPICT_CURVE_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_curves.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_path_storage.h"

namespace OHOS {
/**
 * @class DepictCurve
 * @brief By PATH_CMD_CURVE3 and PATH_CMD_CURVE4
 * The command calculates the generated curve points and
 * saves the generated points to the curve using the conversion pipe
 * move_to/line_to vertex sequence
 * @since 1.0
 * @version 1.0
 */
class DepictCurve {
public:
    /**
     * @brief DepictCurve Class constructor
     * The construction parameter is the vertexsource attribute, which determines the vertex source of the curve.
     * @since 1.0
     * @version 1.0
     */
    explicit DepictCurve(UICanvasVertices& source)
        : source_(&source), lastX_(0), lastY_(0) {}

    void Attach(UICanvasVertices& source)
    {
        source_ = &source;
    }

    /**
     * @brief Method for approximate estimation of curve
     * There are two methods for defining and estimating quadratic or cubic (Bessel) curves.
     * One is curve_ Inc, which is estimated by increasing the curve
     * The second is curve_ Div is by equally dividing the curve into n div segments
     * @since 1.0
     * @version 1.0
     */
    void ApproximationMethod(CurveApproximationMethod curvApproxiMethod)
    {
        quadraticBezier_.ApproximationMethod(curvApproxiMethod);
        cubicBezier_.ApproximationMethod(curvApproxiMethod);
    }

    CurveApproximationMethod ApproximationMethod() const
    {
        return cubicBezier_.ApproximationMethod();
    }

    /**
     * @brief Finally determine the accuracy of the estimate
     * In practical application, we need to convert the world coordinates of points to screen coordinates,
     * so there will always be a certain scaling factor.
     * Curves are usually processed in the world coordinate system and converted to pixel values when estimating.
     * It usually looks like this: m_curved.approximation_scale(transform_.scale());
     * Here, transform_ is a matrix of affine mapping,
     * which contains all transformations,
     * including viewpoint and scaling.
     * @since 1.0
     * @version 1.0
     */
    void ApproximationScale(float aScale)
    {
        quadraticBezier_.SetApproximationScale(aScale);
        cubicBezier_.SetApproximationScale(aScale);
    }

    float ApproximationScale() const
    {
        return cubicBezier_.ApproximationScale();
    }

    /**
     * @brief Sets the angle estimate in radians. The less this value,
     * the more accurate the estimation at the turn of the curve.
     * However, if set to 0, it means that the angle condition is not considered at all.
     * @since 1.0
     * @version 1.0
     */
    void AngleTolerance(float angleRate)
    {
        quadraticBezier_.AngleTolerance(angleRate);
        cubicBezier_.AngleTolerance(angleRate);
    }

    float AngleTolerance() const
    {
        return cubicBezier_.AngleTolerance();
    }

    /**
     * @brief An angle set in radians. If 0,
     * Then only the real cusp will have a bevel cut. If greater than 0,
     * Then it will limit the curvature of the curve. The higher the value,
     * the less sharp corners of the curve will be cut.
     * Generally, this value should not be greater than 10-15 degrees.
     * @since 1.0
     * @version 1.0
     */
    void CuspLimit(float v)
    {
        quadraticBezier_.CuspLimit(v);
        cubicBezier_.CuspLimit(v);
    }

    float CuspLimit() const
    {
        return cubicBezier_.CuspLimit();
    }
    /**
     * Reset the status attribute of a path segment
     * @path_id Is a path ID, calculated from 0
     * @since 1.0
     * @version 1.0
     */
    void Rewind(uint32_t pathId);

    /**
     * According to PATH_CMD command returns the vertex coordinates generated in each stage
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(float* x, float* y);

private:
    DepictCurve(const DepictCurve&);
    const DepictCurve& operator=(const DepictCurve&);

    UICanvasVertices* source_;
    float lastX_;
    float lastY_;
    QuadraticBezierCurve quadraticBezier_;
    CubicBezierCurve cubicBezier_;
};
} // namespace OHOS
#endif
