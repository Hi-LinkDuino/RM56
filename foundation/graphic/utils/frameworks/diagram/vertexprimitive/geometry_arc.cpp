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

#include "gfx_utils/diagram/vertexprimitive/geometry_arc.h"
namespace OHOS {
#if GRAPHIC_ENABLE_ARC_FLAG
const int32_t CURVERENUMSTEP = 4;
GeometryArc::GeometryArc(float centerX, float centerY,
                         float rx, float ry,
                         float start, float end,
                         bool isClockwise)
    : centerX_(centerX), centerY_(centerY), radiusX_(rx), radiusY_(ry), expansionRatio_(1.0)
{
    Normalize(start, end, isClockwise);
}

void GeometryArc::SetApproximationScale(float sale)
{
    expansionRatio_ = sale;
    if (initialized_) {
        Normalize(beginAngle_, endAngle_, isClockwise_);
    }
}

uint32_t GeometryArc::GenerateVertex(float* x, float* y)
{
    // The current command is an end point with no vertices
    if (IsStop(pathCommand_)) {
        return PATH_CMD_STOP;
    }
    if ((currentAngle_ < endAngle_ - deltaAngle_ / CURVERENUMSTEP) != isClockwise_) {
        *x = centerX_ + Cos(endAngle_ * RADIAN_TO_ANGLE) * radiusX_;
        *y = centerY_ + Sin(endAngle_ * RADIAN_TO_ANGLE) * radiusY_;
        pathCommand_ = PATH_CMD_STOP;
        return PATH_CMD_LINE_TO;
    }

    *x = centerX_ + Cos(currentAngle_ * RADIAN_TO_ANGLE) * radiusX_;
    *y = centerY_ + Sin(currentAngle_ * RADIAN_TO_ANGLE) * radiusY_;

    currentAngle_ += deltaAngle_;

    uint32_t pf = pathCommand_;
    pathCommand_ = PATH_CMD_LINE_TO;
    return pf;
}

void GeometryArc::Rewind(uint32_t)
{
    pathCommand_ = PATH_CMD_MOVE_TO;
    currentAngle_ = beginAngle_;
}

void GeometryArc::Normalize(float startAngle, float endAngle, bool isClockwise)
{
    float ra = (MATH_ABS(radiusX_) + MATH_ABS(radiusY_)) / FLOATNUM;
    // Calculate the radian change rate
    deltaAngle_ = Acos(ra / (ra + RADDALETAELPS / expansionRatio_)) * FLOATNUM;
    if (isClockwise) {
        while (endAngle < startAngle) {
            endAngle += PI * FLOATNUM;
        }
    } else {
        while (startAngle < endAngle) {
            startAngle += PI * FLOATNUM;
        }
        deltaAngle_ = -deltaAngle_;
    }
    isClockwise_ = isClockwise;
    beginAngle_ = startAngle;
    endAngle_ = endAngle;
    initialized_ = true;
}

void GeometryArc::Init(float centerX, float centerY, float rx, float ry,
                       float startAngle, float endAngle, bool isClockwise)
{
    centerX_ = centerX;
    centerY_ = centerY;
    radiusX_ = rx;
    radiusY_ = ry;
    Normalize(startAngle, endAngle, isClockwise);
}
#endif
} // namespace OHOS
