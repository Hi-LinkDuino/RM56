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

#include "gfx_utils/geometry2d.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const uint16_t VECTOR2_SIZE = 4;
    const int16_t BASE_VALUE = BASE_VALUE;
    const int16_t TWOFOLD_VALUE = 2 * BASE_VALUE;
    const int16_t THREEFOLD_VALUE = 3 * BASE_VALUE;
    const int16_t FOURFOLD_VALUE = 4 * BASE_VALUE;
    const int16_t FIVEFOLD_VALUE = 5 * BASE_VALUE;
}

class Geometry2dTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: LineOperator_001
 * @tc.desc: Verify index operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, LineOperator_001, TestSize.Level0)
{
    Vector2<int16_t> pt1 = { BASE_VALUE, TWOFOLD_VALUE };
    Vector2<int16_t> pt2 = { THREEFOLD_VALUE, FOURFOLD_VALUE };
    Line* line = new Line(pt1, pt2);

    EXPECT_EQ(line->operator[](0).x_, BASE_VALUE);
    EXPECT_EQ(line->operator[](0).y_, TWOFOLD_VALUE);
    EXPECT_EQ(line->operator[](1).x_, THREEFOLD_VALUE);
    EXPECT_EQ(line->operator[](1).y_, FOURFOLD_VALUE);
    delete line;
}

/**
 * @tc.name: LineOperator_002
 * @tc.desc: Verify index operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, LineOperator_002, TestSize.Level1)
{
    Line* line = new Line(BASE_VALUE, TWOFOLD_VALUE, THREEFOLD_VALUE, FOURFOLD_VALUE);
    if (line == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(line->operator[](0).x_, BASE_VALUE);
    EXPECT_EQ(line->operator[](0).y_, TWOFOLD_VALUE);
    EXPECT_EQ(line->operator[](1).x_, THREEFOLD_VALUE);
    EXPECT_EQ(line->operator[](1).y_, FOURFOLD_VALUE);
    delete line;
}

/**
 * @tc.name: PolygonMakeAABB_001
 * @tc.desc: Verify MakeAABB function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, PolygonMakeAABB_001, TestSize.Level0)
{
    Vector2<int16_t> vertexes[VECTOR2_SIZE] =
        { {0, 0}, {FIVEFOLD_VALUE, BASE_VALUE},
        {THREEFOLD_VALUE, FIVEFOLD_VALUE}, {BASE_VALUE, THREEFOLD_VALUE} };
    Polygon* polygon = new Polygon(vertexes, VECTOR2_SIZE);
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect rect = polygon->MakeAABB();

    EXPECT_EQ(rect.GetLeft(), 0);
    EXPECT_EQ(rect.GetTop(), 0);
    EXPECT_EQ(rect.GetRight(), FIVEFOLD_VALUE);
    EXPECT_EQ(rect.GetBottom(), FIVEFOLD_VALUE);
    delete polygon;
}

/**
 * @tc.name: PolygonGetVertexNum_001
 * @tc.desc: Verify GetVertexNum function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, PolygonGetVertexNum_001, TestSize.Level0)
{
    Vector2<int16_t> vertexes[VECTOR2_SIZE] =
        { { 0, 0 }, { FIVEFOLD_VALUE, BASE_VALUE },
        { THREEFOLD_VALUE, FIVEFOLD_VALUE }, { BASE_VALUE, THREEFOLD_VALUE } };
    Polygon* polygon = new Polygon(vertexes, VECTOR2_SIZE);
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    EXPECT_EQ(polygon->GetVertexNum(), VECTOR2_SIZE);
    delete polygon;
}

/**
 * @tc.name: PolygonSetVertexNum_001
 * @tc.desc: Verify SetVertexNum function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, PolygonSetVertexNum_001, TestSize.Level0)
{
    Polygon* polygon = new Polygon();
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint16_t VERTEX_NUM = 8;
    polygon->SetVertexNum(VERTEX_NUM);

    EXPECT_EQ(polygon->GetVertexNum(), VERTEX_NUM);
    delete polygon;
}

/**
 * @tc.name: PolygonOperator_001
 * @tc.desc: Verify operator[] function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, PolygonOperator_001, TestSize.Level0)
{
    Vector2<int16_t> vertexes[VECTOR2_SIZE] =
        { { 0, 0 }, { FIVEFOLD_VALUE, BASE_VALUE },
        { THREEFOLD_VALUE, FIVEFOLD_VALUE }, { BASE_VALUE, THREEFOLD_VALUE } };
    Polygon* polygon = new Polygon(vertexes, VECTOR2_SIZE);
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(polygon->operator[](1).x_, FIVEFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](2).y_, FIVEFOLD_VALUE);

    delete polygon;
}

/**
 * @tc.name: PolygonOperator_002
 * @tc.desc: Verify operator[] function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, PolygonOperator_002, TestSize.Level0)
{
    Rect rect(BASE_VALUE, TWOFOLD_VALUE, THREEFOLD_VALUE, FOURFOLD_VALUE);
    Polygon* polygon = new Polygon(rect);
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t i = 0;
    EXPECT_EQ(polygon->operator[](i).x_, BASE_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, TWOFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i).x_, THREEFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, TWOFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i).x_, THREEFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, FOURFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i).x_, BASE_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, FOURFOLD_VALUE);
    EXPECT_EQ(polygon->GetVertexNum(), VECTOR2_SIZE);

    delete polygon;
}

/**
 * @tc.name: Geometry2dIntersect_001
 * @tc.desc: Verify Intersect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, Geometry2dIntersect_001, TestSize.Level0)
{
    Vector2<int16_t> pt11 = { 0, TWOFOLD_VALUE };
    Vector2<int16_t> pt12 = { FOURFOLD_VALUE, TWOFOLD_VALUE };
    Line* line1 = new Line(pt11, pt12);
    if (line1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt21 = { TWOFOLD_VALUE, 0 };
    Vector2<int16_t> pt22 = { TWOFOLD_VALUE, FOURFOLD_VALUE };
    Line* line2 = new Line(pt21, pt22);
    if (line2 == nullptr) {
        delete line1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt31 = { 0, 0 };
    Vector2<int16_t> pt32 = { FIVEFOLD_VALUE, 0 };
    Line* line3 = new Line(pt31, pt32);
    if (line3 == nullptr) {
        delete line1;
        delete line2;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> out;

    EXPECT_EQ(Intersect(*line1, *line2, out), false);
    EXPECT_EQ(out.x_, TWOFOLD_VALUE);
    EXPECT_EQ(out.y_, TWOFOLD_VALUE);

    delete line1;
    delete line2;
    delete line3;
}

/**
 * @tc.name: Geometry2dIsIntersect_001
 * @tc.desc: Verify IsIntersect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, Geometry2dIsIntersect_001, TestSize.Level0)
{
    Vector2<int16_t> pt11 = { 0, TWOFOLD_VALUE };
    Vector2<int16_t> pt12 = { FOURFOLD_VALUE, TWOFOLD_VALUE };
    Line* line1 = new Line(pt11, pt12);
    if (line1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt21 = { TWOFOLD_VALUE, 0 };
    Vector2<int16_t> pt22 = { TWOFOLD_VALUE, FOURFOLD_VALUE };
    Line* line2 = new Line(pt21, pt22);
    if (line2 == nullptr) {
        delete line1;
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt31 = { 0, 0 };
    Vector2<int16_t> pt32 = { FIVEFOLD_VALUE, 0 };
    Line* line3 = new Line(pt31, pt32);
    if (line3 == nullptr) {
        delete line1;
        delete line2;
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(IsIntersect(*line1, *line2), true);

    delete line1;
    delete line2;
    delete line3;
}

/**
 * @tc.name: Geometry2dClip_001
 * @tc.desc: Verify Clip function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(Geometry2dTest, Geometry2dClip_001, TestSize.Level0)
{
    Vector2<int16_t> vertexes[VECTOR2_SIZE] =
        { { 0, 0 }, { FIVEFOLD_VALUE, BASE_VALUE },
        { THREEFOLD_VALUE, FIVEFOLD_VALUE }, { BASE_VALUE, THREEFOLD_VALUE } };
    Polygon* polygon = new Polygon(vertexes, VECTOR2_SIZE);
    if (polygon == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt11 = { BASE_VALUE, THREEFOLD_VALUE };
    Vector2<int16_t> pt12 = { FIVEFOLD_VALUE, BASE_VALUE };
    Line* line = new Line(pt11, pt12);
    if (line == nullptr) {
        delete polygon;
        EXPECT_EQ(1, 0);
        return;
    }
    Clip(*polygon, *line);
    uint16_t i = 0;
    EXPECT_EQ(polygon->GetVertexNum(), 0);
    EXPECT_EQ(polygon->operator[](i).x_, FIVEFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, BASE_VALUE);
    EXPECT_EQ(polygon->operator[](i).x_, BASE_VALUE);
    EXPECT_EQ(polygon->operator[](i++).y_, THREEFOLD_VALUE);
    EXPECT_EQ(polygon->operator[](i).x_, 0);
    EXPECT_EQ(polygon->operator[](i++).y_, 0);

    delete polygon;
    delete line;
}
} // namespace OHOS
