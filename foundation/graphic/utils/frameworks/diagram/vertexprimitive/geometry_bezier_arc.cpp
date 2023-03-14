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

#include "gfx_utils/diagram/vertexprimitive/geometry_bezier_arc.h"

namespace OHOS {
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
const uint16_t BEZIER_ARC_SETUP = 2;

const uint16_t BEZIER_ARC_VERTICES_SIZE_STEP = 6;

const uint16_t BEZIER_ARC_POINTS = 4;
/* Limit Value of Bezier Arc */
const float BEZIER_ARC_ANGLE_EPSILON = 0.01f;

const float BEZIER_ARC_DELTAX = 4.0f;

const float BEZIER_ARC_EQUAL_DIVISION = 3.0f;

const float BEZIER_ARC_RADIICHECK = 10.0f;

void ArcToBezier(float cx, float cy, float rx, float ry,
                 float startAngle, float sweepAngle,
                 float* curve)
{
    float y0 = Sin((sweepAngle / FLOATNUM) * RADIAN_TO_ANGLE);
    float x0 = Cos((sweepAngle / FLOATNUM) * RADIAN_TO_ANGLE);
    float tx = (1.0f - x0) * BEZIER_ARC_DELTAX / BEZIER_ARC_EQUAL_DIVISION;
    if (y0 == 0) {
        y0 = y0 + VERTEX_DIST_EPSILON;
    }
    float ty = y0 - tx * x0 / y0;
    float px[BEZIER_ARC_POINTS];
    float py[BEZIER_ARC_POINTS];
    px[0] = x0;
    py[0] = -y0;
    px[1] = x0 + tx;
    py[1] = -ty;
    px[2] = x0 + tx;
    py[2] = ty;
    px[3] = x0;
    py[3] = y0;

    float cosVal = Cos((startAngle + sweepAngle / FLOATNUM) * RADIAN_TO_ANGLE );
    float sinVal = Sin((startAngle + sweepAngle / FLOATNUM) * RADIAN_TO_ANGLE);

    for (uint16_t i = 0; i < BEZIER_ARC_POINTS; i++) {
        curve[i * BEZIER_ARC_SETUP] = cx + rx * (px[i] * cosVal - py[i] * sinVal);
        curve[i * BEZIER_ARC_SETUP + 1] = cy + ry * (px[i] * sinVal + py[i] * cosVal);
    }
}

void BezierArc::Init(float centerX, float centerY,
                     float rx, float ry,
                     float startAngle,
                     float sweepAngle)
{
    startAngle = Fmod(startAngle, FLOATNUM * PI);
    if (sweepAngle <= -FLOATNUM * PI) {
        sweepAngle = -FLOATNUM * PI;
    }
    if (sweepAngle >= FLOATNUM * PI) {
        sweepAngle = FLOATNUM * PI;
    }
    if (MATH_ABS(sweepAngle) < 1e-10) {
        numberVertices_ = BEZIER_ARC_POINTS;
        currentCommand_ = PATH_CMD_LINE_TO;
        arrayVertices_[0] = centerX + rx * Cos(startAngle * RADIAN_TO_ANGLE);
        arrayVertices_[1] = centerY + ry * Sin(startAngle * RADIAN_TO_ANGLE);
        arrayVertices_[2] = centerX + rx * Cos((startAngle + sweepAngle) * RADIAN_TO_ANGLE);
        arrayVertices_[3] = centerY + ry * Sin((startAngle + sweepAngle) * RADIAN_TO_ANGLE);
        return;
    }

    float prevSweep;
    float totalSweep = 0.0f;
    float localSweep = 0.0f;
    numberVertices_ = BEZIER_ARC_SETUP;
    currentCommand_ = PATH_CMD_CURVE4;
    bool done = false;
    do {
        if (sweepAngle < 0.0f) {
            prevSweep = totalSweep;
            totalSweep -= PI * HALFNUM;
            localSweep = -PI * HALFNUM;
            if (totalSweep <= sweepAngle + BEZIER_ARC_ANGLE_EPSILON) {
                localSweep = sweepAngle - prevSweep;
                done = true;
            }
        } else {
            prevSweep = totalSweep;
            totalSweep += PI * HALFNUM;
            localSweep = PI * HALFNUM;
            if (totalSweep >= sweepAngle - BEZIER_ARC_ANGLE_EPSILON) {
                localSweep = sweepAngle - prevSweep;
                done = true;
            }
        }

        ArcToBezier(centerX, centerY, rx, ry, startAngle, localSweep, arrayVertices_
                    + numberVertices_ - BEZIER_ARC_SETUP);

        startAngle += localSweep;
        numberVertices_ += BEZIER_ARC_VERTICES_SIZE_STEP;
    } while (numberVertices_ < BEZIER_ARC_VERTEX_NUM && !done);
}

void BezierArcSvg::Init(float x0, float y0,
                        float rx, float ry,
                        float angle,
                        bool largeArcFlag,
                        bool sweepFlag,
                        float x2, float y2)
{
    if (ry < 0.0f) {
        ry = -ry;
    }
    if (rx < 0.0f) {
        rx = -rx;
    }
    isRadiusJoinPath_ = true;
    float delatY2 = (y0 - y2) / FLOATNUM;
    float delatX2 = (x0 - x2) / FLOATNUM;
    float sinA = Sin(angle * RADIAN_TO_ANGLE);
    float cosA = Cos(angle * RADIAN_TO_ANGLE);
    float y1 = -sinA * delatX2 + cosA * delatY2;
    float x1 = cosA * delatX2 + sinA * delatY2;
    float prx = rx * rx;
    float pry = ry * ry;
    float px1 = x1 * x1;
    float py1 = y1 * y1;
    float radiiCheck = px1 / prx + py1 / pry;
    if (radiiCheck > 1.0f) {
        ry = Sqrt(radiiCheck) * ry;
        rx = Sqrt(radiiCheck) * rx;
        pry = ry * ry;
        prx = rx * rx;
        if (radiiCheck > BEZIER_ARC_RADIICHECK) {
            isRadiusJoinPath_ = false;
        }
    }
    float sign = (largeArcFlag == sweepFlag) ? -1.0f : 1.0f;
    float sq = (prx * pry - prx * py1 - pry * px1) / (prx * py1 + pry * px1);
    float coef = sign * Sqrt((sq < 0) ? 0 : sq);
    if (ry == 0) {
        ry += VERTEX_DIST_EPSILON;
    }
    if (rx == 0) {
        rx += VERTEX_DIST_EPSILON;
    }
    float cx1 = coef * ((rx * y1) / ry);
    float cy1 = coef * -((ry * x1) / rx);
    float sx2 = (x0 + x2) / FLOATNUM;
    float sy2 = (y0 + y2) / FLOATNUM;
    float cx = sx2 + (cosA * cx1 - sinA * cy1);
    float cy = sy2 + (sinA * cx1 + cosA * cy1);
    float ux = (x1 - cx1) / rx;
    float uy = (y1 - cy1) / ry;
    float vx = (-x1 - cx1) / rx;
    float vy = (-y1 - cy1) / ry;
    float p = ux;
    float n = Sqrt(ux * ux + uy * uy);
    sign = (uy < 0) ? -1.0f : 1.0f;
    if (n == 0) {
        n += VERTEX_DIST_EPSILON;
    }
    float v = p / n;
    if (v > 1.0f) {
        v = 1.0f;
    }
    if (v < -1.0f) {
        v = -1.0f;
    }
    float startAngle = sign * Acos(v);
    n = Sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
    p = ux * vx + uy * vy;
    sign = (ux * vy - uy * vx < 0) ? -1.0f : 1.0f;
    if (n == 0) {
        n += VERTEX_DIST_EPSILON;
    }
    v = p / n;
    if (v < -1.0f) {
        v = -1.0f;
    }
    if (v > 1.0f) {
        v = 1.0f;
    }
    float sweepAngle = sign * Acos(v);
    if (!sweepFlag && sweepAngle > 0.0f) {
        sweepAngle -= PI * FLOATNUM;
    } else if (sweepFlag && sweepAngle < 0.0f) {
        sweepAngle += PI * FLOATNUM;
    }
    bezierArcModel_.Init(0.0f, 0.0f, rx, ry, startAngle, sweepAngle);
    TransAffine mtx = TransAffine::TransAffineRotation(angle);
    mtx *= TransAffine::TransAffineTranslation(cx, cy);
    uint32_t limit = bezierArcModel_.GetNumberVertices() - BEZIER_ARC_SETUP;
    for (uint32_t i = BEZIER_ARC_SETUP; i < limit; i += BEZIER_ARC_SETUP) {
        mtx.Transform(bezierArcModel_.GetVertices() + i, bezierArcModel_.GetVertices() + i + 1);
    }
    bezierArcModel_.GetVertices()[0] = x0;
    bezierArcModel_.GetVertices()[1] = y0;
    if (bezierArcModel_.GetNumberVertices() > BEZIER_ARC_SETUP) {
        bezierArcModel_.GetVertices()[bezierArcModel_.GetNumberVertices() - BEZIER_ARC_SETUP] = x2;
        bezierArcModel_.GetVertices()[bezierArcModel_.GetNumberVertices() - 1] = y2;
    }
}
#endif
} // namespace OHOS
