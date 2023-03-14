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

#include "gfx_utils/diagram/vertexprimitive/geometry_curves.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/diagram/common/common_math.h"
namespace OHOS {
const float CURVE_COLLINEARITY_EPSILON = 1e-30;
const float CURVE_ANGLE_TO_LERANCE_EPSILON = 0.01f;
const int32_t CURVE_RECURSION_LIMIT = 32;
const float CURVES_NUM_STEP_LEN = 0.25f;
float QuadBezierCurveIncr::ApproximationScale() const
{
    return approximationScale_;
}

void QuadBezierCurveIncr::ApproximationScale(float scale)
{
    approximationScale_ = scale;
}

void QuadBezierCurveIncr::Init(float x1, float y1,
                               float x2, float y2,
                               float x3, float y3)
{
    startCoordinate_.x = x1;
    startCoordinate_.y = y1;
    endCoordinate_.x = x3;
    endCoordinate_.y = y3;

    float deltaX1 = x2 - x1;
    float deltaY1 = y2 - y1;
    float deltaX2 = x3 - x2;
    float deltaY2 = y3 - y2;

    float len = Sqrt(deltaX1 * deltaX1 + deltaY1 * deltaY1) +
                Sqrt(deltaX2 * deltaX2 + deltaY2 * deltaY2);

    numberSteps_ = MATH_UROUND(len * CURVES_NUM_STEP_LEN * approximationScale_);
    const int32_t numStepsMax = 4;
    if (numberSteps_ < numStepsMax) {
        numberSteps_ = numStepsMax;
    }
    if (numberSteps_ == 0) {
        return;
    }
    float subdivideStep = 1.0f / numberSteps_;
    float subdivideStep2 = subdivideStep * subdivideStep;

    float tmpX = (x1 - x2 * FLOATNUM + x3) * subdivideStep2;
    float tmpY = (y1 - y2 * FLOATNUM + y3) * subdivideStep2;

    savedFinalCoordinate_.x = x1;
    finalCoordinate_.x = x1;
    savedFinalCoordinate_.y = y1;
    finalCoordinate_.y = y1;

    savedDeltaFinalCoordinate_.x = tmpX + (x2 - x1) * (FLOATNUM * subdivideStep);
    deltaFinalCoordinate_.x = tmpX + (x2 - x1) * (FLOATNUM * subdivideStep);
    savedDeltaFinalCoordinate_.y = tmpY + (y2 - y1) * (FLOATNUM * subdivideStep);
    deltaFinalCoordinate_.y = tmpY + (y2 - y1) * (FLOATNUM * subdivideStep);

    copyDeltaFinalCoordinate_.x = tmpX * FLOATNUM;
    copyDeltaFinalCoordinate_.y = tmpY * FLOATNUM;

    currentStep_ = numberSteps_;
}

void QuadBezierCurveIncr::Rewind(uint32_t)
{
    if (numberSteps_ == 0) {
        currentStep_ = -1;
        return;
    }

    currentStep_ = numberSteps_;
    finalCoordinate_.x = savedFinalCoordinate_.x;
    finalCoordinate_.y = savedFinalCoordinate_.y;
    deltaFinalCoordinate_.x = savedDeltaFinalCoordinate_.x;
    deltaFinalCoordinate_.y = savedDeltaFinalCoordinate_.y;
}

uint32_t QuadBezierCurveIncr::GenerateVertex(float* x, float* y)
{
    if (currentStep_ < 0) {
        return PATH_CMD_STOP;
    }
    if (currentStep_ == numberSteps_) {
        *x = startCoordinate_.x;
        *y = startCoordinate_.y;
        --currentStep_;
        return PATH_CMD_MOVE_TO;
    }
    if (currentStep_ == 0) {
        *x = endCoordinate_.x;
        *y = endCoordinate_.y;
        --currentStep_;
        return PATH_CMD_LINE_TO;
    }

    finalCoordinate_.x += deltaFinalCoordinate_.x;
    finalCoordinate_.y += deltaFinalCoordinate_.y;
    deltaFinalCoordinate_.x += copyDeltaFinalCoordinate_.x;
    deltaFinalCoordinate_.y += copyDeltaFinalCoordinate_.y;
    *x = finalCoordinate_.x;
    *y = finalCoordinate_.y;
    --currentStep_;
    return PATH_CMD_LINE_TO;
}

void QuadrBezierCurveDividOp::Init(float x1, float y1,
                                   float x2, float y2,
                                   float x3, float y3)
{
    points_.Clear();
    distanceToleranceSquare_ = HALFNUM / approximationScale_;
    distanceToleranceSquare_ *= distanceToleranceSquare_;
    Bezier(x1, y1, x2, y2, x3, y3);
    count_ = 0;
}

void QuadrBezierCurveDividOp::Recursive(float x1, float y1,
                                        float x2, float y2,
                                        float x3, float y3,
                                        uint32_t level)
{
    if (level > CURVE_RECURSION_LIMIT) {
        return;
    }

    // Calculates All Midpoints Of a Segment
    float x12 = (x1 + x2) / FLOATNUM;
    float y12 = (y1 + y2) / FLOATNUM;
    float x23 = (x2 + x3) / FLOATNUM;
    float y23 = (y2 + y3) / FLOATNUM;
    float x123 = (x12 + x23) / FLOATNUM;
    float y123 = (y12 + y23) / FLOATNUM;
    float deltaX = x3 - x1;
    float deltaY = y3 - y1;
    float distance = MATH_ABS(((x2 - x3) * deltaY - (y2 - y3) * deltaX));
    float da;
    if (distance > CURVE_COLLINEARITY_EPSILON) {
        if (distance * distance <= distanceToleranceSquare_ * (deltaX * deltaX + deltaY * deltaY)) {
            // If The Curvature Does Not Exceed The Distance Tolerance Value
            if (angleTolerance_ < CURVE_ANGLE_TO_LERANCE_EPSILON) {
                points_.PushBack(PointF(x123, y123));
                return;
            }

            da = MATH_ABS(FastAtan2F(y3 - y2, x3 - x2) - FastAtan2F(y2 - y1, x2 - x1));
            if (da >= PI) {
                da = TWO_TIMES * PI - da;
            }

            if (da < angleTolerance_) {
                points_.PushBack(PointF(x123, y123));
                return;
            }
        }
    } else {
        da = deltaX * deltaX + deltaY * deltaY;
        if (da == 0) {
            distance = CalcSqDistance(x1, y1, x2, y2);
        } else {
            distance = ((x2 - x1) * deltaX + (y2 - y1) * deltaY) / da;
            if (distance > 0 && distance < 1) {
                // Simple Collinear Case，1---2---3
                // We Can Leave Only Two Endpoints
                return;
            }
            if (distance <= 0) {
                distance = CalcSqDistance(x2, y2, x1, y1);
            } else if (distance >= 1) {
                distance = CalcSqDistance(x2, y2, x3, y3);
            } else {
                distance = CalcSqDistance(x2, y2, x1 + distance * deltaX, y1 + distance * deltaY);
            }
        }
        if (distance < distanceToleranceSquare_) {
            points_.PushBack(PointF(x2, y2));
            return;
        }
    }

    // Continue Subdivision
    Recursive(x1, y1, x12, y12, x123, y123, level + 1);
    Recursive(x123, y123, x23, y23, x3, y3, level + 1);
}

void QuadrBezierCurveDividOp::Bezier(float x1, float y1,
                                     float x2, float y2,
                                     float x3, float y3)
{
    points_.PushBack(PointF(x1, y1));
    Recursive(x1, y1, x2, y2, x3, y3, 0);
    points_.PushBack(PointF(x3, y3));
}

void CubicBezierCurveIncrement::ApproximationScale(float scale)
{
    approximationScale_ = scale;
}

float CubicBezierCurveIncrement::ApproximationScale() const
{
    return approximationScale_;
}

void CubicBezierCurveIncrement::Init(float x1, float y1,
                                     float x2, float y2,
                                     float x3, float y3,
                                     float x4, float y4)
{
    startCoordinate_.x = x1;
    startCoordinate_.y = y1;
    endCoordinate_.x = x4;
    endCoordinate_.y = y4;

    float deltaX1 = x2 - x1;
    float deltaY1 = y2 - y1;
    float deltaX2 = x3 - x2;
    float deltaY2 = y3 - y2;
    float deltaX3 = x4 - x3;
    float deltaY3 = y4 - y3;

    float len = (Sqrt(deltaX1 * deltaX1 + deltaY1 * deltaY1) +
                 Sqrt(deltaX2 * deltaX2 + deltaY2 * deltaY2) +
                 Sqrt(deltaX3 * deltaX3 + deltaY3 * deltaY3)) *
                 CURVES_NUM_STEP_LEN * approximationScale_;

    numberSteps_ = MATH_UROUND(len);
    const int32_t cuvereNumStep = 4;
    if (numberSteps_ < cuvereNumStep) {
        numberSteps_ = cuvereNumStep;
    }
    if (numberSteps_ == 0) {
        return;
    }
    float subdivideStep = 1.0f / numberSteps_;
    float subdivideStep2 = subdivideStep * subdivideStep;
    float subdivideStep3 = subdivideStep * subdivideStep * subdivideStep;
    const float prelMin = 3.0f;
    const float prelMax = 6.0f;

    float pre1 = prelMin * subdivideStep;
    float pre2 = prelMin * subdivideStep2;
    float pre4 = prelMax * subdivideStep2;
    float pre5 = prelMax * subdivideStep3;

    float tmp1X = x1 - x2 * FLOATNUM + x3;
    float tmp1Y = y1 - y2 * FLOATNUM + y3;

    float tmp2X = (x2 - x3) * prelMin - x1 + x4;
    float tmp2Y = (y2 - y3) * prelMin - y1 + y4;

    savedFinalCoordinate_.x = x1;
    finalCoordinate_.x = x1;
    savedFinalCoordinate_.y = y1;
    finalCoordinate_.y = y1;

    savedDeltaFinalCoordinate_.x = (x2 - x1) * pre1 + tmp1X * pre2 + tmp2X * subdivideStep3;
    deltaFinalCoordinate_.x = (x2 - x1) * pre1 + tmp1X * pre2 + tmp2X * subdivideStep3;
    savedDeltaFinalCoordinate_.y = (y2 - y1) * pre1 + tmp1Y * pre2 + tmp2Y * subdivideStep3;
    deltaFinalCoordinate_.y = (y2 - y1) * pre1 + tmp1Y * pre2 + tmp2Y * subdivideStep3;

    savedCopyDeltaFinalCoordinate_.x = tmp1X * pre4 + tmp2X * pre5;
    copyDeltaFinalCoordinate_.x = tmp1X * pre4 + tmp2X * pre5;
    savedCopyDeltaFinalCoordinate_.y = tmp1Y * pre4 + tmp2Y * pre5;
    copyDeltaFinalCoordinate_.y = tmp1Y * pre4 + tmp2Y * pre5;

    repeatCopyDeltaFinalCoordinate_.x = tmp2X * pre5;
    repeatCopyDeltaFinalCoordinate_.y = tmp2Y * pre5;

    currentStep_ = numberSteps_;
}

void CubicBezierCurveIncrement::Rewind(uint32_t)
{
    if (numberSteps_ == 0) {
        currentStep_ = -1;
        return;
    }

    currentStep_ = numberSteps_;
    finalCoordinate_.x = savedFinalCoordinate_.x;
    finalCoordinate_.y = savedFinalCoordinate_.y;
    deltaFinalCoordinate_.x = savedDeltaFinalCoordinate_.x;
    deltaFinalCoordinate_.y = savedDeltaFinalCoordinate_.y;
    copyDeltaFinalCoordinate_.x = savedCopyDeltaFinalCoordinate_.x;
    copyDeltaFinalCoordinate_.y = savedCopyDeltaFinalCoordinate_.y;
}

uint32_t CubicBezierCurveIncrement::GenerateVertex(float* x, float* y)
{
    if (currentStep_ < 0) {
        return PATH_CMD_STOP;
    }
    if (currentStep_ == numberSteps_) {
        *x = startCoordinate_.x;
        *y = startCoordinate_.y;
        --currentStep_;
        return PATH_CMD_MOVE_TO;
    }

    if (currentStep_ == 0) {
        *x = endCoordinate_.x;
        *y = endCoordinate_.y;
        --currentStep_;
        return PATH_CMD_LINE_TO;
    }

    finalCoordinate_.x += deltaFinalCoordinate_.x;
    finalCoordinate_.y += deltaFinalCoordinate_.y;
    deltaFinalCoordinate_.x += copyDeltaFinalCoordinate_.x;
    deltaFinalCoordinate_.y += copyDeltaFinalCoordinate_.y;
    copyDeltaFinalCoordinate_.x += repeatCopyDeltaFinalCoordinate_.x;
    copyDeltaFinalCoordinate_.y += repeatCopyDeltaFinalCoordinate_.y;

    *x = finalCoordinate_.x;
    *y = finalCoordinate_.y;
    --currentStep_;
    return PATH_CMD_LINE_TO;
}

void CubicBezierCurveDividOperate::Init(float x1, float y1,
                                        float x2, float y2,
                                        float x3, float y3,
                                        float x4, float y4)
{
    points_.Clear();
    distanceToleranceSquare_ = HALFNUM / approximationScale_;
    distanceToleranceSquare_ *= distanceToleranceSquare_;
    Bezier(x1, y1, x2, y2, x3, y3, x4, y4);
    count_ = 0;
}

void CubicBezierCurveDividOperate::Recursive(float x1, float y1,
                                             float x2, float y2,
                                             float x3, float y3,
                                             float x4, float y4,
                                             uint32_t level)
{
    if (level > CURVE_RECURSION_LIMIT) {
        return;
    }
    if (level > CURVE_RECURSION_LIMIT) {
        return;
    }

    /** Calculates All Midpoints Of a Segment */
    float x12 = (x1 + x2) / FLOATNUM;
    float y12 = (y1 + y2) / FLOATNUM;
    float x23 = (x2 + x3) / FLOATNUM;
    float y23 = (y2 + y3) / FLOATNUM;
    float x34 = (x3 + x4) / FLOATNUM;
    float y34 = (y3 + y4) / FLOATNUM;
    float x123 = (x12 + x23) / FLOATNUM;
    float y123 = (y12 + y23) / FLOATNUM;
    float x234 = (x23 + x34) / FLOATNUM;
    float y234 = (y23 + y34) / FLOATNUM;
    float x1234 = (x123 + x234) / FLOATNUM;
    float y1234 = (y123 + y234) / FLOATNUM;
    /** Try to Approximate the Entire Cubic Curve With a Straight Line */
    float delta41X = x4 - x1;
    float delta41Y = y4 - y1;
    float delta2 = MATH_ABS(((x2 - x4) * delta41Y - (y2 - y4) * delta41X));
    float delta3 = MATH_ABS(((x3 - x4) * delta41Y - (y3 - y4) * delta41X));
    float dxTemp;
    float dyTemp;
    float delta41;

    bool isEnabled = true;
    switch ((int32_t(delta2 > CURVE_COLLINEARITY_EPSILON) << 1) + int32_t(delta3 > CURVE_COLLINEARITY_EPSILON)) {
        case COLLINEAR:
            /** All are Collinear or p1 == p4 */
            delta41 = delta41X * delta41X + delta41Y * delta41Y;
            if (delta41 == 0) {
                delta2 = CalcSqDistance(x1, y1, x2, y2);
                delta3 = CalcSqDistance(x4, y4, x3, y3);
            } else {
                delta41 = 1 / delta41;
                dxTemp = x2 - x1;
                dyTemp = y2 - y1;
                delta2 = delta41 * (dxTemp * delta41X + dyTemp * delta41Y);
                dxTemp = x3 - x1;
                dyTemp = y3 - y1;
                delta3 = delta41 * (dxTemp * delta41X + dyTemp * delta41Y);
                if (delta2 > 0 && delta2 < 1 && delta3 > 0 && delta3 < 1) {
                    /** Simple Collinear Case，1--2--3--4 */
                    /** We Can Leave Only Two Endpoints */
                    return;
                }

                if (delta2 <= 0) {
                    delta2 = CalcSqDistance(x2, y2, x1, y1);
                } else if (delta2 >= 1) {
                    delta2 = CalcSqDistance(x2, y2, x4, y4);
                } else {
                    delta2 = CalcSqDistance(x2, y2, x1 + delta2 * delta41X, y1 + delta2 * delta41Y);
                }
                if (delta3 <= 0) {
                    delta3 = CalcSqDistance(x3, y3, x1, y1);
                } else if (delta3 >= 1) {
                    delta3 = CalcSqDistance(x3, y3, x4, y4);
                } else {
                    delta3 = CalcSqDistance(x3, y3, x1 + delta3 * delta41X, y1 + delta3 * delta41Y);
                }
            }

            if (delta2 > delta3) {
                if (delta2 < distanceToleranceSquare_) {
                    points_.PushBack(PointF(x2, y2));
                    isEnabled = false;
                }
            } else {
                if (delta3 < distanceToleranceSquare_) {
                    points_.PushBack(PointF(x3, y3));
                    isEnabled = false;
                }
            }
            break;
        case COLLINEAR1:
            /** p1、p2、p4 is Collinear */
            if (delta3 * delta3 <= distanceToleranceSquare_ * (delta41X * delta41X + delta41Y * delta41Y)) {
                if (angleTolerance_ < CURVE_ANGLE_TO_LERANCE_EPSILON) {
                    points_.PushBack(PointF(x23, y23));
                    isEnabled = false;
                }
                if (isEnabled) {
                    dxTemp = MATH_ABS(FastAtan2F(y4 - y3, x4 - x3) - FastAtan2F(y3 - y2, x3 - x2));
                    if (dxTemp >= PI) {
                        dxTemp = FLOATNUM * PI - dxTemp;
                    }
                    if (dxTemp < angleTolerance_) {
                        points_.PushBack(PointF(x2, y2));
                        points_.PushBack(PointF(x3, y3));
                        isEnabled = false;
                    }
                    if (isEnabled && cuspLimit_ != 0.0f && dxTemp > cuspLimit_) {
                        points_.PushBack(PointF(x3, y3));
                        isEnabled = false;
                    }
                }
            }
            break;
        case COLLINEAR2:
            /** p1、p3、p4 is Collinear. */
            if (delta2 * delta2 <= distanceToleranceSquare_ * (delta41X * delta41X + delta41Y * delta41Y)) {
                if (angleTolerance_ < CURVE_ANGLE_TO_LERANCE_EPSILON) {
                    points_.PushBack(PointF(x23, y23));
                    isEnabled = false;
                }
                if (isEnabled) {
                    dxTemp = MATH_ABS(FastAtan2F(y3 - y2, x3 - x2) - FastAtan2F(y2 - y1, x2 - x1));
                    if (dxTemp >= PI) {
                        dxTemp = FLOATNUM * PI - dxTemp;
                    }
                    if (dxTemp < angleTolerance_) {
                        points_.PushBack(PointF(x2, y2));
                        points_.PushBack(PointF(x3, y3));
                        isEnabled = false;
                    }
                    if (isEnabled && cuspLimit_ != 0.0f && dxTemp > cuspLimit_) {
                        points_.PushBack(PointF(x2, y2));
                        isEnabled = false;
                    }
                }
            }
            break;
        case COLLINEAR3:
            if ((delta2 + delta3) * (delta2 + delta3) <=
                    distanceToleranceSquare_ * (delta41X * delta41X + delta41Y * delta41Y)) {
                if (angleTolerance_ < CURVE_ANGLE_TO_LERANCE_EPSILON) {
                    points_.PushBack(PointF(x23, y23));
                    isEnabled = false;
                }
                delta41 = FastAtan2F(y3 - y2, x3 - x2);
                dxTemp = MATH_ABS(delta41 - FastAtan2F(y2 - y1, x2 - x1));
                dyTemp = MATH_ABS(FastAtan2F(y4 - y3, x4 - x3) - delta41);
                if (dxTemp >= PI) {
                    dxTemp = FLOATNUM * PI - dxTemp;
                }
                if (dyTemp >= PI) {
                    dyTemp = FLOATNUM * PI - dyTemp;
                }
                if (isEnabled) {
                    if (dxTemp + dyTemp < angleTolerance_) {
                        points_.PushBack(PointF(x23, y23));
                        isEnabled = false;
                    }
                    if (isEnabled && cuspLimit_ != 0.0f && dxTemp > cuspLimit_) {
                        points_.PushBack(PointF(x2, y2));
                        isEnabled = false;
                    }
                    if (isEnabled && cuspLimit_ != 0.0f && dyTemp > cuspLimit_) {
                        points_.PushBack(PointF(x3, y3));
                        isEnabled = false;
                    }
                }
            }
            break;
    }
    if (!isEnabled) {
        return;
    }
    Recursive(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
    Recursive(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
}

void CubicBezierCurveDividOperate::Bezier(float x1, float y1,
                                          float x2, float y2,
                                          float x3, float y3,
                                          float x4, float y4)
{
    points_.PushBack(PointF(x1, y1));
    Recursive(x1, y1, x2, y2, x3, y3, x4, y4, 0);
    points_.PushBack(PointF(x4, y4));
}
} // namespace OHOS
