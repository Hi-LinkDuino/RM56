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
 * @file geometry_curves.h
 *
 * @brief Bezier Curve
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GEOMETRY_CURVES_H
#define GRAPHIC_LITE_GEOMETRY_CURVES_H

#include "gfx_utils/vector.h"
namespace OHOS {
const uint8_t CURVE_POINTS_LENGTH = 8;

enum CurveApproximationMethod {
    CURVEINCREMENT,
    CURVEDIVIDOPERATE
};

class QuadBezierCurveIncr {
public:
    QuadBezierCurveIncr() : numberSteps_(0), currentStep_(0), approximationScale_(1.0f) {}

    QuadBezierCurveIncr(float x1, float y1,
                        float x2, float y2,
                        float x3, float y3)
        : numberSteps_(0), currentStep_(0), approximationScale_(1.0f)
    {
        Init(x1, y1, x2, y2, x3, y3);
    }

    void Init(float x1, float y1, float x2, float y2, float x3, float y3);

    void Reset()
    {
        numberSteps_ = 0;
        currentStep_ = -1;
    }

    void ApproximationMethod(CurveApproximationMethod) {}

    CurveApproximationMethod ApproximationMethod() const
    {
        return CURVEINCREMENT;
    }

    void ApproximationScale(float scale);
    float ApproximationScale() const;

    void AngleTolerance(float) {}

    float AngleTolerance() const
    {
        return 0.0f;
    }

    void CuspLimit(float) {}

    float CuspLimit() const
    {
        return 0.0f;
    }

    void Rewind(uint32_t pathId);
    uint32_t GenerateVertex(float* x, float* y);

private:
    int32_t numberSteps_;
    int32_t currentStep_;
    float approximationScale_;
    PointF startCoordinate_;
    PointF endCoordinate_;
    PointF finalCoordinate_;
    PointF deltaFinalCoordinate_;
    PointF copyDeltaFinalCoordinate_;
    PointF savedFinalCoordinate_;
    PointF savedDeltaFinalCoordinate_;
};

class QuadrBezierCurveDividOp {
public:
    QuadrBezierCurveDividOp()
        : approximationScale_(1.0f), angleTolerance_(0.0f), count_(0) {}

    QuadrBezierCurveDividOp(float x1, float y1,
                            float x2, float y2,
                            float x3, float y3)
        : approximationScale_(1.0f), angleTolerance_(0.0f), count_(0)
    {
        Init(x1, y1, x2, y2, x3, y3);
    }

    void Init(float x1, float y1, float x2, float y2, float x3, float y3);

    void Reset()
    {
        points_.Clear();
        count_ = 0;
    }

    void ApproximationMethod(CurveApproximationMethod) {}

    CurveApproximationMethod ApproximationMethod() const
    {
        return CURVEDIVIDOPERATE;
    }

    void ApproximationScale(float scale)
    {
        approximationScale_ = scale;
    }
    float ApproximationScale() const
    {
        return approximationScale_;
    }

    void AngleTolerance(float angle)
    {
        angleTolerance_ = angle;
    }
    float AngleTolerance() const
    {
        return angleTolerance_;
    }

    void CuspLimit(float) {}

    float CuspLimit() const
    {
        return 0.0f;
    }

    void Rewind(uint32_t)
    {
        count_ = 0;
    }

    uint32_t GenerateVertex(float* x, float* y)
    {
        if (count_ >= points_.Size()) {
            return PATH_CMD_STOP;
        }
        const PointF& point = points_[count_++];
        *x = point.x;
        *y = point.y;
        return (count_ == 1) ? PATH_CMD_MOVE_TO : PATH_CMD_LINE_TO;
    }

private:
    void Bezier(float x1, float y1,
                float x2, float y2,
                float x3, float y3);
    void Recursive(float x1, float y1,
                   float x2, float y2,
                   float x3, float y3,
                   uint32_t level);

    float approximationScale_;
    float distanceToleranceSquare_;
    float angleTolerance_;
    uint32_t count_;
    Graphic::Vector<PointF> points_;
};

struct CubicBezierCurvePoints {
    float cpArray[CURVE_POINTS_LENGTH];

    CubicBezierCurvePoints() {}

    CubicBezierCurvePoints(float x1, float y1,
                           float x2, float y2,
                           float x3, float y3,
                           float x4, float y4)
    {
        cpArray[0] = x1;
        cpArray[1] = y1;
        cpArray[2] = x2;
        cpArray[3] = y2;
        cpArray[4] = x3;
        cpArray[5] = y3;
        cpArray[6] = x4;
        cpArray[7] = y4;
    }

    void Init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4)
    {
        cpArray[0] = x1;
        cpArray[1] = y1;
        cpArray[2] = x2;
        cpArray[3] = y2;
        cpArray[4] = x3;
        cpArray[5] = y3;
        cpArray[6] = x4;
        cpArray[7] = y4;
    }

    float operator[](uint32_t i) const
    {
        return cpArray[i];
    }
    float& operator[](uint32_t i)
    {
        return cpArray[i];
    }
};

class CubicBezierCurveIncrement {
public:
    CubicBezierCurveIncrement()
        : numberSteps_(0), currentStep_(0), approximationScale_(1.0f) {}

    CubicBezierCurveIncrement(float x1, float y1, float x2, float y2,
                              float x3, float y3, float x4, float y4)
        : numberSteps_(0), currentStep_(0), approximationScale_(1.0f)
    {
        Init(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    CubicBezierCurveIncrement(const CubicBezierCurvePoints& curve4Points)
        : numberSteps_(0), currentStep_(0), approximationScale_(1.0f)
    {
        Init(curve4Points[0], curve4Points[1], curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5], curve4Points[6], curve4Points[7]);
    }

    void Init(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    void Init(const CubicBezierCurvePoints& curve4Points)
    {
        Init(curve4Points[0], curve4Points[1], curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5], curve4Points[6], curve4Points[7]);
    }

    void Reset()
    {
        numberSteps_ = 0;
        currentStep_ = -1;
    }

    void ApproximationMethod(CurveApproximationMethod) {}

    CurveApproximationMethod ApproximationMethod() const
    {
        return CURVEINCREMENT;
    }

    void ApproximationScale(float scale);
    float ApproximationScale() const;

    void AngleTolerance(float) {}

    float AngleTolerance() const
    {
        return 0.0f;
    }

    void CuspLimit(float) {}

    float CuspLimit() const
    {
        return 0.0f;
    }

    void Rewind(uint32_t pathId);
    uint32_t GenerateVertex(float* x, float* y);

private:
    int32_t numberSteps_;
    int32_t currentStep_;
    float approximationScale_;
    PointF startCoordinate_;
    PointF endCoordinate_;
    PointF finalCoordinate_;
    PointF deltaFinalCoordinate_;
    PointF copyDeltaFinalCoordinate_;
    PointF repeatCopyDeltaFinalCoordinate_;
    PointF savedFinalCoordinate_;
    PointF savedDeltaFinalCoordinate_;
    PointF savedCopyDeltaFinalCoordinate_;
};

inline CubicBezierCurvePoints CatromToBezier(float x1, float y1,
                                             float x2, float y2,
                                             float x3, float y3,
                                             float x4, float y4)
{
    return CubicBezierCurvePoints(
        x2,
        y2,
        (-x1 + SIX_TIMES * x2 + x3) / SIX_TIMES,
        (-y1 + SIX_TIMES * y2 + y3) / SIX_TIMES,
        (x2 + SIX_TIMES * x3 - x4) / SIX_TIMES,
        (y2 + SIX_TIMES * y3 - y4) / SIX_TIMES,
        x3,
        y3);
}

inline CubicBezierCurvePoints CatromToBezier(const CubicBezierCurvePoints& curve4Points)
{
    return CatromToBezier(curve4Points[0], curve4Points[1], curve4Points[2],
                          curve4Points[3], curve4Points[4], curve4Points[5],
                          curve4Points[6], curve4Points[7]);
}
/**
 * @brief bspline Convert Curve to Cubic Bezier Curve
 * @param bspline Curve 4 points, start, end and middle 2 control points
 * @since 1.0
 * @version 1.0
 */
inline CubicBezierCurvePoints UbsplineToBezier(float x1, float y1,
                                               float x2, float y2,
                                               float x3, float y3,
                                               float x4, float y4)
{
    return CubicBezierCurvePoints(
        (x1 + FOUR_TIMES * x2 + x3) / SIX_TIMES,
        (y1 + FOUR_TIMES * y2 + y3) / SIX_TIMES,
        (FOUR_TIMES * x2 + TWO_TIMES * x3) / SIX_TIMES,
        (FOUR_TIMES * y2 + TWO_TIMES * y3) / SIX_TIMES,
        (TWO_TIMES * x2 + FOUR_TIMES * x3) / SIX_TIMES,
        (TWO_TIMES * y2 + FOUR_TIMES * y3) / SIX_TIMES,
        (x2 + FOUR_TIMES * x3 + x4) / SIX_TIMES,
        (y2 + FOUR_TIMES * y3 + y4) / SIX_TIMES);
}
/**
 * @brief bspline Convert Curve to Bezier Curve
 * @param bspline Curve 4 points, start, end and middle 2 control points
 * @since 1.0
 * @version 1.0
 */
inline CubicBezierCurvePoints UbsplineToBezier(const CubicBezierCurvePoints& curve4Points)
{
    return UbsplineToBezier(curve4Points[0], curve4Points[1], curve4Points[2], curve4Points[3],
                            curve4Points[4], curve4Points[5], curve4Points[6], curve4Points[7]);
}
/**
 * @brief Hermite Convert Curve to 3-order Bezier Curve
 * @param Hermite Curve 2 points, start and end
 * @since 1.0
 * @version 1.0
 */
inline CubicBezierCurvePoints HermiteToBezier(float x1, float y1,
                                              float x2, float y2,
                                              float x3, float y3,
                                              float x4, float y4)
{
    return CubicBezierCurvePoints(
        x1, y1,
        (THREE_TIMES * x1 + x3) / THREE_TIMES,
        (THREE_TIMES * y1 + y3) / THREE_TIMES,
        (THREE_TIMES * x2 - x4) / THREE_TIMES,
        (THREE_TIMES * y2 - y4) / THREE_TIMES,
        x2, y2);
}
/**
 * @brief Hermite Convert Curve to 3-order Bezier Curve
 * @param Hermite Curve 2 points, start and end
 * @since 1.0
 * @version 1.0
 */
inline CubicBezierCurvePoints HermiteToBezier(const CubicBezierCurvePoints& curve4Points)
{
    return HermiteToBezier(curve4Points[0], curve4Points[1],
                           curve4Points[2], curve4Points[3],
                           curve4Points[4], curve4Points[5],
                           curve4Points[6], curve4Points[7]);
}

class CubicBezierCurveDividOperate {
public:
    CubicBezierCurveDividOperate()
        : approximationScale_(1.0f), angleTolerance_(0.0f), cuspLimit_(0.0f), count_(0) {}

    CubicBezierCurveDividOperate(float x1, float y1,
                                 float x2, float y2,
                                 float x3, float y3,
                                 float x4, float y4)
        : approximationScale_(1.0f), angleTolerance_(0.0f), cuspLimit_(0.0f), count_(0)
    {
        Init(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    CubicBezierCurveDividOperate(const CubicBezierCurvePoints& curve4Points)
        : approximationScale_(1.0f), angleTolerance_(0.0f), count_(0)
    {
        Init(curve4Points[0], curve4Points[1],
             curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5],
             curve4Points[6], curve4Points[7]);
    }

    void Init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4);

    void Init(const CubicBezierCurvePoints& curve4Points)
    {
        Init(curve4Points[0], curve4Points[1],
             curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5],
             curve4Points[6], curve4Points[7]);
    }

    void Reset()
    {
        points_.Clear();
        count_ = 0;
    }

    void ApproximationMethod(CurveApproximationMethod) {}

    CurveApproximationMethod ApproximationMethod() const
    {
        return CURVEDIVIDOPERATE;
    }

    void ApproximationScale(float scale)
    {
        approximationScale_ = scale;
    }
    float ApproximationScale() const
    {
        return approximationScale_;
    }

    void AngleTolerance(float angleValue)
    {
        angleTolerance_ = angleValue;
    }
    float AngleTolerance() const
    {
        return angleTolerance_;
    }

    void CuspLimit(float angleValue)
    {
        cuspLimit_ = (angleValue == 0.0f) ? 0.0f : PI - angleValue;
    }

    float CuspLimit() const
    {
        return (cuspLimit_ == 0.0f) ? 0.0f : PI - cuspLimit_;
    }

    void Rewind(uint32_t)
    {
        count_ = 0;
    }

    uint32_t GenerateVertex(float* x, float* y)
    {
        if (count_ >= points_.Size()) {
            return PATH_CMD_STOP;
        }
        const PointF& p = points_[count_++];
        *x = p.x;
        *y = p.y;
        return (count_ == 1) ? PATH_CMD_MOVE_TO : PATH_CMD_LINE_TO;
    }

private:
    void Bezier(float x1, float y1,
                float x2, float y2,
                float x3, float y3,
                float x4, float y4);

    void Recursive(float x1, float y1,
                         float x2, float y2,
                         float x3, float y3,
                         float x4, float y4,
                         uint32_t level);

    float approximationScale_;
    float distanceToleranceSquare_;
    float angleTolerance_;
    float cuspLimit_;
    uint32_t count_;
    Graphic::Vector<PointF> points_;
};

/**
 * @brief quadratic Bezier curve.
 *
 * Draw a curve from the start, inflection and end points.
 *
 * @see QuadraticBezierCurve.
 * @since 1.0
 * @version 1.0
 */
class QuadraticBezierCurve {
public:
    QuadraticBezierCurve()
        : approximationMethod_(CURVEDIVIDOPERATE) {}

    QuadraticBezierCurve(float x1, float y1, float x2, float y2, float x3, float y3)
        : approximationMethod_(CURVEDIVIDOPERATE)
    {
        Init(x1, y1, x2, y2, x3, y3);
    }

    void Init(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            curveInc_.Init(x1, y1, x2, y2, x3, y3);
        } else {
            curveDiv_.Init(x1, y1, x2, y2, x3, y3);
        }
    }

    void Reset()
    {
        curveInc_.Reset();
        curveDiv_.Reset();
    }

    void ApproximationMethod(CurveApproximationMethod curveApproximationMethod)
    {
        approximationMethod_ = curveApproximationMethod;
    }

    CurveApproximationMethod ApproximationMethod() const
    {
        return approximationMethod_;
    }

    void SetApproximationScale(float scale)
    {
        curveInc_.ApproximationScale(scale);
        curveDiv_.ApproximationScale(scale);
    }

    float ApproximationScale() const
    {
        return curveInc_.ApproximationScale();
    }

    void AngleTolerance(float angle)
    {
        curveDiv_.AngleTolerance(angle);
    }

    float AngleTolerance() const
    {
        return curveDiv_.AngleTolerance();
    }

    void CuspLimit(float angleValue)
    {
        curveDiv_.CuspLimit(angleValue);
    }

    float CuspLimit() const
    {
        return curveDiv_.CuspLimit();
    }

    void Rewind(uint32_t pathId)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            curveInc_.Rewind(pathId);
        } else {
            curveDiv_.Rewind(pathId);
        }
    }

    uint32_t GenerateVertex(float* x, float* y)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            return curveInc_.GenerateVertex(x, y);
        }
        return curveDiv_.GenerateVertex(x, y);
    }

private:
    QuadBezierCurveIncr curveInc_;
    QuadrBezierCurveDividOp curveDiv_;
    CurveApproximationMethod approximationMethod_;
};

/**
 * @brief Cubic Bezier curve
 *
 * Draw a curve according to the start point, control point and end point.
 *
 * @see CubicBezierCurve
 * @since 1.0
 * @version 1.0.
 */
class CubicBezierCurve {
public:
    CubicBezierCurve()
        : approximationMethod_(CURVEDIVIDOPERATE) {}

    CubicBezierCurve(float x1, float y1,
                     float x2, float y2,
                     float x3, float y3,
                     float x4, float y4)
        : approximationMethod_(CURVEDIVIDOPERATE)
    {
        Init(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    CubicBezierCurve(const CubicBezierCurvePoints& curve4Points)
        : approximationMethod_(CURVEDIVIDOPERATE)
    {
        Init(curve4Points[0], curve4Points[1], curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5], curve4Points[6], curve4Points[7]);
    }

    void Init(float x1, float y1,
              float x2, float y2,
              float x3, float y3,
              float x4, float y4)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            curveInc_.Init(x1, y1, x2, y2, x3, y3, x4, y4);
        } else {
            curveDiv_.Init(x1, y1, x2, y2, x3, y3, x4, y4);
        }
    }

    void Init(const CubicBezierCurvePoints& curve4Points)
    {
        Init(curve4Points[0], curve4Points[1], curve4Points[2], curve4Points[3],
             curve4Points[4], curve4Points[5], curve4Points[6], curve4Points[7]);
    }

    void Reset()
    {
        curveInc_.Reset();
        curveDiv_.Reset();
    }

    void ApproximationMethod(CurveApproximationMethod curveApproximationMethod)
    {
        approximationMethod_ = curveApproximationMethod;
    }

    CurveApproximationMethod ApproximationMethod() const
    {
        return approximationMethod_;
    }

    void SetApproximationScale(float scale)
    {
        curveInc_.ApproximationScale(scale);
        curveDiv_.ApproximationScale(scale);
    }
    float ApproximationScale() const
    {
        return curveInc_.ApproximationScale();
    }

    void AngleTolerance(float angleValue)
    {
        curveDiv_.AngleTolerance(angleValue);
    }

    float AngleTolerance() const
    {
        return curveDiv_.AngleTolerance();
    }

    void CuspLimit(float angleValue)
    {
        curveDiv_.CuspLimit(angleValue);
    }

    float CuspLimit() const
    {
        return curveDiv_.CuspLimit();
    }

    void Rewind(uint32_t pathId)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            curveInc_.Rewind(pathId);
        } else {
            curveDiv_.Rewind(pathId);
        }
    }

    uint32_t GenerateVertex(float* x, float* y)
    {
        if (approximationMethod_ == CURVEINCREMENT) {
            return curveInc_.GenerateVertex(x, y);
        }
        return curveDiv_.GenerateVertex(x, y);
    }

private:
    CubicBezierCurveIncrement curveInc_;
    CubicBezierCurveDividOperate curveDiv_;
    CurveApproximationMethod approximationMethod_;
};
} // namespace OHOS
#endif
