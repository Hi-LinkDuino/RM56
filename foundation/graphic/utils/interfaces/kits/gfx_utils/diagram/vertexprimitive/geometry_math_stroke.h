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
 * @file geometry_math_stroke.h
 * @brief Defines
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GEOERTY_MATH_STROKE_H
#define GRAPHIC_LITE_GEOERTY_MATH_STROKE_H

#include "gfx_utils/diagram/common/common_math.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_vertex_sequence.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/vector.h"
namespace OHOS {
/**
 * @brief The style of the line end cap.
 */
enum LineCap {
    /** Add straight edges to each end of the line */
    BUTT_CAP,
    /** Add a square cap to each end of the line */
    SQUARE_CAP,
    /** Add a circular cap to each end of the line */
    ROUND_CAP
};

/**
 * @brief The type of corner created when two lines intersect
 */
enum LineJoin {
    /** Create sharp corners */
    MITER_JOIN = 0,
    MITER_JOIN_REVERT = 1,
    /** Create Fillets */
    ROUND_JOIN = 2,
    /** Create bevel */
    BEVEL_JOIN = 3,
    MITER_JOIN_ROUND = 4
};
class GeometryMathStroke {
public:
    GeometryMathStroke()
        : strokeWidth_(ALPHA_HALF),
          strokeWidthUsingAbs_(ALPHA_HALF),
          strokeWidthPercentDivision_(ALPHA_HALF / BUF_SIZE),
          strokeWidthSignal_(1),
#if GRAPHIC_ENABLE_LINECAP_FLAG
          lineCap_(BUTT_CAP),
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
          miterLimitMeasure_(DEFAULTMITERLIMIT),
          lineJoin_(MITER_JOIN),
#endif
          approxScaleRadio_(1.0f) {}

#if GRAPHIC_ENABLE_LINECAP_FLAG
    /**
     * @brief SetLineCap Defines the end style of the line
     */
    void SetLineCap(LineCap lineCapE)
    {
        lineCap_ = lineCapE;
    }

    LineCap GetLineCap() const
    {
        return lineCap_;
    }
    /**
     * @brief Calculate end style.
     * Pay attention to 90 degree rotation at both ends of the corner.
     */
    void CalcCap(Graphic::Vector<PointF>& vertexConsumer, const VertexDist& vd0, const VertexDist& vd1, float len)
    {
        vertexConsumer.Clear();
        if (len == 0.0f) {
            len += VERTEX_DIST_EPSILON;
        }
        float dx1;
        float dy1;
        if (len != 0) {
            dx1 = (vd1.vertexYCoord - vd0.vertexYCoord) / len;
            dy1 = (vd1.vertexXCoord - vd0.vertexXCoord) / len;
        } else {
            return;
        }
        float dx2 = 0;
        float dy2 = 0;

        dx1 *= strokeWidth_;
        dy1 *= strokeWidth_;

        if (lineCap_ != ROUND_CAP) {
            if (lineCap_ == SQUARE_CAP) {
                dx2 = dy1 * strokeWidthSignal_;
                dy2 = dx1 * strokeWidthSignal_;
            }
            AddVertex(vertexConsumer, vd0.vertexXCoord - dx1 - dx2, vd0.vertexYCoord + dy1 - dy2);
            AddVertex(vertexConsumer, vd0.vertexXCoord + dx1 - dx2, vd0.vertexYCoord - dy1 - dy2);
        } else {
            float deltaAngle = Acos(strokeWidthUsingAbs_ /
                                        (strokeWidthUsingAbs_ +RADDALETAELPS / approxScaleRadio_)) * TWO_TIMES;
            float angleStart;
            int32_t nIndex;
            int32_t divNumber = int32_t(PI / deltaAngle);

            deltaAngle = PI / (divNumber + 1);
            AddVertex(vertexConsumer, vd0.vertexXCoord - dx1, vd0.vertexYCoord + dy1);
            if (strokeWidthSignal_ > 0) {
                angleStart = FastAtan2F(dy1, -dx1);
                angleStart += deltaAngle;
                for (nIndex = 0; nIndex < divNumber; nIndex++) {
                    AddVertex(vertexConsumer, vd0.vertexXCoord + Cos(angleStart * RADIAN_TO_ANGLE) * strokeWidth_,
                              vd0.vertexYCoord + Sin(angleStart * RADIAN_TO_ANGLE) * strokeWidth_);
                    angleStart += deltaAngle;
                }
            } else {
                angleStart = FastAtan2F(-dy1, dx1);
                angleStart -= deltaAngle;
                for (nIndex = 0; nIndex < divNumber; nIndex++) {
                    AddVertex(vertexConsumer, vd0.vertexXCoord + Cos(angleStart * RADIAN_TO_ANGLE) * strokeWidth_,
                              vd0.vertexYCoord + Sin(angleStart * RADIAN_TO_ANGLE) * strokeWidth_);
                    angleStart -= deltaAngle;
                }
            }
            AddVertex(vertexConsumer, vd0.vertexXCoord + dx1, vd0.vertexYCoord - dy1);
        }
    }
#endif

#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief SetLineJoin Defines the type of corner created when two lines intersect.
     * Pay attention to 90 degree rotation at both ends of the corner.
     */
    void SetLineJoin(LineJoin lineJoinE)
    {
        lineJoin_ = lineJoinE;
    }

    /**
     * @brief SetMiterLimit Sets the maximum miter length.
     */
    void SetMiterLimit(float miterLimit)
    {
        miterLimitMeasure_ = miterLimit;
    }

    /**
     * @brief Calculate intersections and corners.
     * Pay attention to 90 degree rotation at both ends of the corner.
     */
    void CalcJoin(Graphic::Vector<PointF>& vertexConsumer,
                  const VertexDist& vertexDistBegin,
                  const VertexDist& vertexDistMiddle,
                  const VertexDist& vertexDistLast,
                  float deltaLengthPrev,
                  float deltaLengthLast)
    {
        bool isEnable = true;
        if (deltaLengthPrev == 0.0f) {
            deltaLengthPrev += VERTEX_DIST_EPSILON;
        }
        if (deltaLengthLast == 0.0f) {
            deltaLengthPrev += VERTEX_DIST_EPSILON;
        }
        float dx1 = 0;
        float dy1 = 0;
        float dx2 = 0;
        float dy2 = 0;
        if (deltaLengthPrev != 0) {
            dx1 = strokeWidth_ * (vertexDistMiddle.vertexYCoord - vertexDistBegin.vertexYCoord) / deltaLengthPrev;
            dy1 = strokeWidth_ * (vertexDistMiddle.vertexXCoord - vertexDistBegin.vertexXCoord) / deltaLengthPrev;
        } else {
            isEnable = false;
        }
        if (isEnable && deltaLengthLast != 0) {
            dx2 = strokeWidth_ * (vertexDistLast.vertexYCoord - vertexDistMiddle.vertexYCoord) / deltaLengthLast;
            dy2 = strokeWidth_ * (vertexDistLast.vertexXCoord - vertexDistMiddle.vertexXCoord) / deltaLengthLast;
        } else {
            isEnable = false;
        }
        if (!isEnable) {
            return;
        }
        vertexConsumer.Clear();
        float crossProduct =
            CrossProduct(vertexDistBegin.vertexXCoord, vertexDistBegin.vertexYCoord, vertexDistMiddle.vertexXCoord,
                         vertexDistMiddle.vertexYCoord, vertexDistLast.vertexXCoord, vertexDistLast.vertexYCoord);
        if (crossProduct != 0 && (crossProduct > 0) == (strokeWidth_ > 0)) {
            float limit =
                ((deltaLengthPrev < deltaLengthLast) ? deltaLengthPrev : deltaLengthLast) / strokeWidthUsingAbs_;
            CalcMiter(vertexConsumer, vertexDistBegin, vertexDistMiddle, vertexDistLast, dx1, dy1, dx2, dy2,
                      MITER_JOIN_REVERT, limit, 0);
        } else {
            float dx = (dx1 + dx2) * HALFNUM;
            float dy = (dy1 + dy2) * HALFNUM;
            float dbevel = Sqrt(dx * dx + dy * dy);
            float lim = strokeWidthUsingAbs_ * miterLimitMeasure_;
            bool isIntersection =
                CalcIntersection(vertexDistBegin.vertexXCoord + dx1, vertexDistBegin.vertexYCoord - dy1,
                                 vertexDistMiddle.vertexXCoord + dx1, vertexDistMiddle.vertexYCoord - dy1,
                                 vertexDistMiddle.vertexXCoord + dx2, vertexDistMiddle.vertexYCoord - dy2,
                                 vertexDistLast.vertexXCoord + dx2, vertexDistLast.vertexYCoord - dy2, &dx, &dy);
            LineJoin lineJoin = lineJoin_;
            if (lineJoin == MITER_JOIN) {
                if (CalcDistance(vertexDistMiddle.vertexXCoord, vertexDistMiddle.vertexYCoord, dx, dy) > lim) {
                    lineJoin = BEVEL_JOIN;
                }
            }
            bool isRoundOrBevel = false;
            if (lineJoin == ROUND_JOIN || lineJoin == BEVEL_JOIN) {
                if (approxScaleRadio_ * (strokeWidthUsingAbs_ - dbevel) < strokeWidthPercentDivision_) {
                    if (isIntersection) {
                        AddVertex(vertexConsumer, dx, dy);
                    } else {
                        AddVertex(vertexConsumer, vertexDistMiddle.vertexXCoord + dx1,
                                  vertexDistMiddle.vertexYCoord - dy1);
                    }
                    isRoundOrBevel = true;
                }
            }
            if (isRoundOrBevel) {
                return;
            }

            switch (lineJoin) {
                case MITER_JOIN:
                case MITER_JOIN_REVERT:
                case MITER_JOIN_ROUND:
                    CalcMiter(vertexConsumer, vertexDistBegin, vertexDistMiddle,
                              vertexDistLast, dx1, dy1, dx2, dy2, lineJoin_, miterLimitMeasure_, dbevel);
                    break;
                case ROUND_JOIN:
                    CalcArc(vertexConsumer, vertexDistMiddle.vertexXCoord,
                            vertexDistMiddle.vertexYCoord, dx1, -dy1, dx2, -dy2);
                    break;

                default:
                    AddVertex(vertexConsumer, vertexDistMiddle.vertexXCoord + dx1,
                              vertexDistMiddle.vertexYCoord - dy1);
                    AddVertex(vertexConsumer, vertexDistMiddle.vertexXCoord + dx2,
                              vertexDistMiddle.vertexYCoord - dy2);
                    break;
            }
        }
    }

    /**
     * @brief Calculate miter length
     */
    void CalcMiter(Graphic::Vector<PointF>& vertexConsumer,
                   const VertexDist& vd0,
                   const VertexDist& vd1,
                   const VertexDist& vd2,
                   float dx1,
                   float dy1,
                   float dx2,
                   float dy2,
                   LineJoin linejoin,
                   float mlimit,
                   float dbevel)
    {
        float xi = vd1.vertexXCoord;
        float yi = vd1.vertexYCoord;
        float di = 1;
        float lim = strokeWidthUsingAbs_ * mlimit;
        bool miterLimitExceeded = true;
        bool intersectionFailed = true;
        if (CalcIntersection(vd0.vertexXCoord + dx1, vd0.vertexYCoord - dy1, vd1.vertexXCoord + dx1,
                             vd1.vertexYCoord - dy1, vd1.vertexXCoord + dx2, vd1.vertexYCoord - dy2,
                             vd2.vertexXCoord + dx2, vd2.vertexYCoord - dy2, &xi, &yi)) {
            di = CalcDistance(vd1.vertexXCoord, vd1.vertexYCoord, xi, yi);
            if (di <= lim) {
                AddVertex(vertexConsumer, xi, yi);
                miterLimitExceeded = false;
            }
            intersectionFailed = false;
        } else {
            float x2 = vd1.vertexXCoord + dx1;
            float y2 = vd1.vertexYCoord - dy1;
            if ((CrossProduct(vd0.vertexXCoord, vd0.vertexYCoord,
                              vd1.vertexXCoord, vd1.vertexYCoord, x2, y2) < 0.0f) ==
                (CrossProduct(vd1.vertexXCoord, vd1.vertexYCoord,
                              vd2.vertexXCoord, vd2.vertexYCoord, x2, y2) < 0.0f)) {
                AddVertex(vertexConsumer, vd1.vertexXCoord + dx1, vd1.vertexYCoord - dy1);
                miterLimitExceeded = false;
            }
        }

        if (miterLimitExceeded) {
            switch (linejoin) {
                case MITER_JOIN_REVERT:
                    AddVertex(vertexConsumer, vd1.vertexXCoord + dx1, vd1.vertexYCoord - dy1);
                    AddVertex(vertexConsumer, vd1.vertexXCoord + dx2, vd1.vertexYCoord - dy2);
                    break;
                case MITER_JOIN_ROUND:
                    CalcArc(vertexConsumer, vd1.vertexXCoord, vd1.vertexYCoord, dx1, -dy1, dx2, -dy2);
                    break;
                default:
                    if (intersectionFailed) {
                        mlimit *= strokeWidthSignal_;
                        AddVertex(vertexConsumer, vd1.vertexXCoord + dx1 + dy1 * mlimit,
                                  vd1.vertexYCoord - dy1 + dx1 * mlimit);
                        AddVertex(vertexConsumer, vd1.vertexXCoord + dx2 - dy2 * mlimit,
                                  vd1.vertexYCoord - dy2 - dx2 * mlimit);
                    } else {
                        float x1 = vd1.vertexXCoord + dx1;
                        float y1 = vd1.vertexYCoord - dy1;
                        float x2 = vd1.vertexXCoord + dx2;
                        float y2 = vd1.vertexYCoord - dy2;
                        di = (lim - dbevel) / (di - dbevel);
                        AddVertex(vertexConsumer, x1 + (xi - x1) * di, y1 + (yi - y1) * di);
                        AddVertex(vertexConsumer, x2 + (xi - x2) * di, y2 + (yi - y2) * di);
                    }
                    break;
            }
        }
    }
    void CalcArc(Graphic::Vector<PointF>& vertexConsumer, float x, float y, float dx1, float dy1, float dx2, float dy2)
    {
        const float limitScale = 0.125f;
        float angleStart = FastAtan2F(dy1 * strokeWidthSignal_, dx1 * strokeWidthSignal_);
        float angleEnd = FastAtan2F(dy2 * strokeWidthSignal_, dx2 * strokeWidthSignal_);
        float deltaAngle = angleStart - angleEnd;
        int32_t nIndex;
        int32_t divNumber;

        deltaAngle = Acos(strokeWidthUsingAbs_ / (strokeWidthUsingAbs_ + limitScale / approxScaleRadio_))
                * TWO_TIMES;

        AddVertex(vertexConsumer, x + dx1, y + dy1);
        if (strokeWidthSignal_ > 0) {
            if (angleStart > angleEnd) {
                angleEnd += TWO_TIMES * PI;
            }
            divNumber = int32_t((angleEnd - angleStart) / deltaAngle);
            deltaAngle = (angleEnd - angleStart) / (divNumber + 1);
            angleStart += deltaAngle;
            for (nIndex = 0; nIndex < divNumber; nIndex++) {
                AddVertex(vertexConsumer, x + Cos(angleStart * RADIAN_TO_ANGLE) * strokeWidth_,
                          y + Sin(angleStart * RADIAN_TO_ANGLE) * strokeWidth_);
                angleStart += deltaAngle;
            }
        } else {
            if (angleStart < angleEnd) {
                angleEnd -= TWO_TIMES * PI;
            }
            divNumber = int32_t((angleStart - angleEnd) / deltaAngle);
            deltaAngle = (angleStart - angleEnd) / (divNumber + 1);
            angleStart -= deltaAngle;
            for (nIndex = 0; nIndex < divNumber; nIndex++) {
                AddVertex(vertexConsumer, x + Cos(angleStart * RADIAN_TO_ANGLE) * strokeWidth_,
                          y + Sin(angleStart * RADIAN_TO_ANGLE) * strokeWidth_);
                angleStart -= deltaAngle;
            }
        }
        AddVertex(vertexConsumer, x + dx2, y + dy2);
    }
    LineJoin GetLineJoin() const
    {
        return lineJoin_;
    }
    /**
     * @brief GetMiterLimit Returns the maximum miter length
     */
    float GetMiterLimit() const
    {
        return miterLimitMeasure_;
    }
#endif

    /**
     * @brief width Set area width
     */
    void SetWidth(float width)
    {
        strokeWidth_ = width * ALPHA_HALF;
        if (strokeWidth_ < 0) {
            strokeWidthUsingAbs_ = -strokeWidth_;
            strokeWidthSignal_ = -1;
        } else {
            strokeWidthUsingAbs_ = strokeWidth_;
            strokeWidthSignal_ = 1;
        }
        strokeWidthPercentDivision_ = strokeWidth_ / BUF_SIZE;
    }

    /**
     * @brief Add approximation
     */
    void SetApproximationScale(float approximationScale)
    {
        approxScaleRadio_ = approximationScale;
    }

    /**
     * @brief width Return width
     */
    float GetWidth() const
    {
        return strokeWidth_ * TWO_TIMES;
    }

    /**
     * @brief Returns the set approximate value
     */
    float GetApproximationScale() const
    {
        return approxScaleRadio_;
    }

private:
    inline void AddVertex(Graphic::Vector<PointF>& vertexConsumer, float x, float y)
    {
        vertexConsumer.PushBack(PointF(x, y));
    }

    float strokeWidth_;
    float strokeWidthUsingAbs_;
    float strokeWidthPercentDivision_;
    int32_t strokeWidthSignal_;

    float approxScaleRadio_;
#if GRAPHIC_ENABLE_LINECAP_FLAG
    LineCap lineCap_;
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    LineJoin lineJoin_;
    float miterLimitMeasure_;
#endif
};
} // namespace OHOS
#endif
