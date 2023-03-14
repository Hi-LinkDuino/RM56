/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "gfx_utils/geometry2d.h"
#include "securec.h"

namespace OHOS {
/* return point of intersection of two lines when it is sure a and b is intersected */
bool Intersect(const Line& a, const Line& b, Vector2<int16_t>& out)
{
    Vector2<int16_t> aA = a[0];
    Vector2<int16_t> aB = a[1];
    Vector2<int16_t> bA = b[0];
    Vector2<int16_t> bB = b[1];
    int32_t den = static_cast<int32_t>(aA.x_ - aB.x_) * (bA.y_ - bB.y_) -
        static_cast<int32_t>(aA.y_ - aB.y_) * (bA.x_ - bB.x_);
    if (den == 0) {
        return false;
    }
    int32_t xNum = (static_cast<int32_t>(aA.x_) * aB.y_ - static_cast<int32_t>(aA.y_) * aB.x_) * (bA.x_ - bB.x_) -
        (aA.x_ - aB.x_) * (static_cast<int32_t>(bA.x_) * bB.y_ - static_cast<int32_t>(bA.y_) * bB.x_);
    int32_t yNum = (static_cast<int32_t>(aA.x_) * aB.y_ - static_cast<int32_t>(aA.y_) * aB.x_) * (bA.y_ - bB.y_) -
        (aA.y_ - aB.y_) * (static_cast<int32_t>(bA.x_) * bB.y_ - static_cast<int32_t>(bA.y_) * bB.x_);
    out.x_ = xNum / den;
    out.y_ = yNum / den;
    return true;
}

/* check two line intersect if intersect return true, else false */
bool IsIntersect(const Line& a, const Line& b)
{
    Vector2<int16_t> aA = a[0];
    Vector2<int16_t> aB = a[1];
    Vector2<int16_t> bA = b[0];
    Vector2<int16_t> bB = b[1];

    int32_t iPos = static_cast<int32_t>(aB.x_ - aA.x_) * (bA.y_ - aA.y_) -
        static_cast<int32_t>(aB.y_ - aA.y_) * (bA.x_ - aA.x_);
    int32_t kPos = static_cast<int32_t>(aB.x_ - aA.x_) * (bB.y_ - aA.y_) -
        static_cast<int32_t>(aB.y_ - aA.y_) * (bB.x_ - aA.x_);

    if (static_cast<int64_t>(iPos) * kPos <= 0) {
        return true;
    }
    return false;
}

/* This functions clips all the edges w.r.t one clip edge of clipping area */
void Clip(Polygon& poly, const Line& line)
{
    uint8_t vertexNum = poly.GetVertexNum();
    Polygon newPoly;
    int16_t iX;
    int16_t iY;
    int16_t kX;
    int16_t kY;

    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;

    int8_t k;
    int32_t iPos;
    int32_t kPos;

    /* (iX, iY), (kX, kY) are the co-ordinate values of the points */
    for (int8_t i = 0; i < vertexNum; i++) {
        /* i and k form a line in polygon  */
        k = (i + 1) % vertexNum;
        iX = poly[i].x_;
        iY = poly[i].y_;
        kX = poly[k].x_;
        kY = poly[k].y_;

        x1 = line[0].x_;
        y1 = line[0].y_;
        x2 = line[1].x_;
        y2 = line[1].y_;

        /* Calculating position of first point w.r.t. clipper line */
        iPos = static_cast<int32_t>(x2 - x1) * (iY - y1) - static_cast<int32_t>(y2 - y1) * (iX - x1);

        /* Calculating position of second point w.r.t. clipper line */
        kPos = static_cast<int32_t>(x2 - x1) * (kY - y1) - static_cast<int32_t>(y2 - y1) * (kX - x1);

        uint8_t newVertexNum = newPoly.GetVertexNum();

        if (iPos < 0 && kPos < 0) {
            /* Case 1 : When both points are inside */
            newPoly[newVertexNum].x_ = kX;
            newPoly[newVertexNum].y_ = kY;

            newVertexNum++;
        } else if (iPos >= 0 && kPos < 0) {
            /* Case 2: When only first point is outside */
            Vector2<int16_t> intersectPoint;
            bool intersect = Intersect(Line(x1, y1, x2, y2), Line(iX, iY, kX, kY), intersectPoint);
            if (!intersect) {
                continue;
            }

            newPoly[newVertexNum].x_ = intersectPoint.x_;
            newPoly[newVertexNum].y_ = intersectPoint.y_;
            newVertexNum++;

            newPoly[newVertexNum].x_ = kX;
            newPoly[newVertexNum].y_ = kY;
            newVertexNum++;
        } else if (iPos < 0 && kPos >= 0) {
            /* Case 3: When only second point is outside Only point of intersection with edge is added */
            Vector2<int16_t> intersectPoint;
            bool intersect = Intersect(Line(x1, y1, x2, y2), Line(iX, iY, kX, kY), intersectPoint);
            if (!intersect) {
                continue;
            }

            newPoly[newVertexNum].x_ = intersectPoint.x_;
            newPoly[newVertexNum].y_ = intersectPoint.y_;
            newVertexNum++;
        }
        newPoly.SetVertexNum(newVertexNum);
    }

    /* Copying new points into original array and changing the no. of vertices */
    poly = newPoly;
}

/* Implements Sutherland–Hodgman algorithm for polygon clipping */
Polygon SuthHodgClip(const Rect& clipRect, const Polygon& polygon)
{
    Polygon newPolygon(polygon);

    Clip(newPolygon, Line(clipRect.GetLeft(), clipRect.GetTop(), clipRect.GetRight(), clipRect.GetTop()));
    Clip(newPolygon, Line(clipRect.GetRight(), clipRect.GetTop(), clipRect.GetRight(), clipRect.GetBottom()));
    Clip(newPolygon, Line(clipRect.GetRight(), clipRect.GetBottom(), clipRect.GetLeft(), clipRect.GetBottom()));
    Clip(newPolygon, Line(clipRect.GetLeft(), clipRect.GetBottom(), clipRect.GetLeft(), clipRect.GetTop()));
    return newPolygon;
}

/* the max cross point is two when a line cross a convex polygon */
void Clip(const Line& line, const Polygon& poly, Vector2<int16_t>* pOut, uint8_t *pNum)
{
    if (pOut == nullptr || pNum == nullptr) {
        return;
    }
    uint8_t vertexNum = poly.GetVertexNum();
    int16_t iX;
    int16_t iY;
    int16_t kX;
    int16_t kY;

    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;

    int8_t k;
    int32_t iPos;
    int32_t kPos;

    /* (iX, iY), (kX, kY) are the co-ordinate values of the points */
    for (int8_t i = 0; i < vertexNum; i++) {
        /* i and k form a line in polygon  */
        k = (i + 1) % vertexNum;
        iX = poly[i].x_;
        iY = poly[i].y_;
        kX = poly[k].x_;
        kY = poly[k].y_;

        x1 = line[0].x_;
        y1 = line[0].y_;
        x2 = line[1].x_;
        y2 = line[1].y_;

        /* Calculating position of first point w.r.t. clipper line */
        iPos = static_cast<int32_t>(x2 - x1) * (iY - y1) - static_cast<int32_t>(y2 - y1) * (iX - x1);

        /* Calculating position of second point w.r.t. clipper line */
        kPos = static_cast<int32_t>(x2 - x1) * (kY - y1) - static_cast<int32_t>(y2 - y1) * (kX - x1);

        if ((iPos >= 0 && kPos < 0) || (iPos < 0 && kPos >= 0)) {
            Vector2<int16_t> intersectPoint;
            Intersect(Line(x1, y1, x2, y2), Line(iX, iY, kX, kY), intersectPoint);
            (*pOut) = intersectPoint;
            (*pNum)++;
        }
    }
}

Polygon::Polygon(const Vector2<int16_t>* vertexes, const uint8_t vertexNum)
{
    ASSERT(vertexNum <= MAX_VERTEX_NUM);
    if (memcpy_s(vertexes_, MAX_VERTEX_NUM * sizeof(Vector2<int16_t>),
        vertexes, vertexNum * sizeof(Vector2<int16_t>)) != EOK) {
        ASSERT(0);
    }
    vertexNum_ = vertexNum;
}

Rect Polygon::MakeAABB() const
{
    int16_t minX = vertexes_[0].x_;
    int16_t maxX = vertexes_[0].x_;
    int16_t minY = vertexes_[0].y_;
    int16_t maxY = vertexes_[0].y_;
    for (uint8_t i = 1; i < vertexNum_; i++) {
        if (minX > vertexes_[i].x_) {
            minX = vertexes_[i].x_;
        }
        if (maxX < vertexes_[i].x_) {
            maxX = vertexes_[i].x_;
        }
        if (minY > vertexes_[i].y_) {
            minY = vertexes_[i].y_;
        }
        if (maxY < vertexes_[i].y_) {
            maxY = vertexes_[i].y_;
        }
    }
    return Rect(minX, minY, maxX, maxY);
}
} // namespace OHOS
