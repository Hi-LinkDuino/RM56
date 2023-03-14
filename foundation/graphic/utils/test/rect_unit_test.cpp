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

#include "gfx_utils/rect.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const int16_t TEST_VALUE = 100;
    const int16_t MIN_VALUE = 50;
    const int16_t MAX_VALUE = 200;
}
class RectTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown() {}
    static Rect* rect_;
};

Rect* RectTest::rect_ = nullptr;

void RectTest::SetUpTestCase(void)
{
    if (rect_ == nullptr) {
        rect_ = new Rect();
    }
}

void RectTest::TearDownTestCase(void)
{
    if (rect_ != nullptr) {
        delete rect_;
        rect_ = nullptr;
    }
}

void RectTest::SetUp()
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, 0, 0, 0);
}
/**
 * @tc.name: RectOperator_001
 * @tc.desc: Verify copy operator function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectOperator_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect1 = new Rect();
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect1->SetRect(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    rect_->operator=(*rect1);

    EXPECT_EQ(rect_->GetLeft(), 0);
    EXPECT_EQ(rect_->GetTop(), 0);
    EXPECT_EQ(rect_->GetRight(), HORIZONTAL_RESOLUTION);
    EXPECT_EQ(rect_->GetBottom(), VERTICAL_RESOLUTION);

    delete rect1;
}

/**
 * @tc.name: RectSetRect_001
 * @tc.desc: Verify SetRect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetRect_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    EXPECT_EQ(rect_->GetLeft(), 0);
    EXPECT_EQ(rect_->GetTop(), 0);
    EXPECT_EQ(rect_->GetRight(), HORIZONTAL_RESOLUTION);
    EXPECT_EQ(rect_->GetBottom(), VERTICAL_RESOLUTION);
}

/**
 * @tc.name: RectGetWidth_001
 * @tc.desc: Verify GetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetWidth_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(rect_->GetWidth(), 1);
}

/**
 * @tc.name: RectGetHeight_001
 * @tc.desc: Verify GetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetHeight_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(rect_->GetHeight(), 1);
}

/**
 * @tc.name: RectGetX_001
 * @tc.desc: Verify GetX function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetX_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(TEST_VALUE, 0, 0, 0);
    EXPECT_EQ(rect_->GetX(), TEST_VALUE);
}

/**
 * @tc.name: RectGetY_001
 * @tc.desc: Verify GetY function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetY_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, TEST_VALUE, 0, 0);
    EXPECT_EQ(rect_->GetY(), TEST_VALUE);
}

/**
 * @tc.name: RectGetLeft_001
 * @tc.desc: Verify GetLeft function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetLeft_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(TEST_VALUE, 0, 0, 0);
    EXPECT_EQ(rect_->GetLeft(), TEST_VALUE);
}

/**
 * @tc.name: RectGetTop_001
 * @tc.desc: Verify GetTop function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetTop_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, TEST_VALUE, 0, 0);
    EXPECT_EQ(rect_->GetTop(), TEST_VALUE);
}

/**
 * @tc.name: RectGetRight_001
 * @tc.desc: Verify GetRight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetRight_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, 0, TEST_VALUE, 0);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE);
}

/**
 * @tc.name: RectGetBottom_001
 * @tc.desc: Verify GetBottom function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetBottom_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRect(0, 0, 0, TEST_VALUE);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE);
}

/**
 * @tc.name: RectSetX_001
 * @tc.desc: Verify SetX function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetX_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetX(TEST_VALUE);
    EXPECT_EQ(rect_->GetLeft(), TEST_VALUE);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE);
}

/**
 * @tc.name: RectSetY_001
 * @tc.desc: Verify SetY function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetY_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetY(TEST_VALUE);
    EXPECT_EQ(rect_->GetTop(), TEST_VALUE);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE);
}

/**
 * @tc.name: RectSetPosition_001
 * @tc.desc: Verify SetPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetPosition_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetPosition(TEST_VALUE, TEST_VALUE);
    EXPECT_EQ(rect_->GetLeft(), TEST_VALUE);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE);
    EXPECT_EQ(rect_->GetTop(), TEST_VALUE);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE);
}

/**
 * @tc.name: RectSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetWidth_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetWidth(TEST_VALUE);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE - 1);
}

/**
 * @tc.name: RectSetHeight_001
 * @tc.desc: Verify SetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetHeight_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetHeight(TEST_VALUE);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE - 1);
}

/**
 * @tc.name: RectSetLeft_001
 * @tc.desc: Verify SetLeft function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetLeft_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetLeft(TEST_VALUE);
    EXPECT_EQ(rect_->GetLeft(), TEST_VALUE);
}

/**
 * @tc.name: RectSetTop_001
 * @tc.desc: Verify SetTop function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetTop_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetTop(TEST_VALUE);
    EXPECT_EQ(rect_->GetTop(), TEST_VALUE);
}

/**
 * @tc.name: RectSetRight_001
 * @tc.desc: Verify SetRight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetRight_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetRight(TEST_VALUE);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE);
}

/**
 * @tc.name: RectSetBottom_001
 * @tc.desc: Verify SetBottom function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectSetBottom_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->SetBottom(TEST_VALUE);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE);
}

/**
 * @tc.name: RectResize_001
 * @tc.desc: Verify Resize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectResize_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    rect_->Resize(TEST_VALUE, TEST_VALUE);

    EXPECT_EQ(rect_->GetLeft(), 0);
    EXPECT_EQ(rect_->GetTop(), 0);
    EXPECT_EQ(rect_->GetRight(), TEST_VALUE - 1);
    EXPECT_EQ(rect_->GetBottom(), TEST_VALUE - 1);
}

/**
 * @tc.name: RectGetSize_001
 * @tc.desc: Verify GetSize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectGetSize_001, TestSize.Level0)
{
    if (rect_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(rect_->GetSize(), 1);
}

/**
 * @tc.name: RectIntersect_001
 * @tc.desc: Verify Intersect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectIntersect_001, TestSize.Level0)
{
    Rect* rect1 = new Rect(TEST_VALUE, TEST_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect2 = new Rect(0, 0, MAX_VALUE, MAX_VALUE);
    if (rect2 == nullptr) {
        delete rect1;
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect3 = new Rect(0, 0, MIN_VALUE, 0);
    if (rect3 == nullptr) {
        delete rect1;
        delete rect2;
        EXPECT_EQ(1, 0);
        return;
    }

    EXPECT_EQ(rect1->Intersect(*rect1, *rect2), true);
    EXPECT_EQ(rect1->Intersect(*rect1, *rect3), false);

    delete rect1;
    delete rect2;
    delete rect3;
}

/**
 * @tc.name: RectJoin_001
 * @tc.desc: Verify Join function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectJoin_001, TestSize.Level0)
{
    Rect* rect1 = new Rect(TEST_VALUE, TEST_VALUE, TEST_VALUE, TEST_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect2 = new Rect(MIN_VALUE, MIN_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect2 == nullptr) {
        delete rect1;
        EXPECT_EQ(1, 0);
        return;
    }

    rect1->Join(*rect1, *rect2);
    EXPECT_EQ(rect1->GetLeft(), MIN_VALUE);
    EXPECT_EQ(rect1->GetTop(), MIN_VALUE);
    EXPECT_EQ(rect1->GetRight(), MAX_VALUE);
    EXPECT_EQ(rect1->GetBottom(), MAX_VALUE);

    delete rect1;
    delete rect2;
}

/**
 * @tc.name: RectIsContains_001
 * @tc.desc: Verify IsContains function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectIsContains_001, TestSize.Level0)
{
    Rect* rect1 = new Rect(MIN_VALUE, MIN_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Vector2<int16_t> pt1 = {TEST_VALUE, TEST_VALUE};
    Vector2<int16_t> pt2 = {0, 0};

    EXPECT_EQ(rect1->IsContains(pt1), true);
    EXPECT_EQ(rect1->IsContains(pt2), false);

    delete rect1;
}

/**
 * @tc.name: RectIsContains_002
 * @tc.desc: Verify IsContains function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectIsContains_002, TestSize.Level0)
{
    Rect* rect1 = new Rect(MIN_VALUE, MIN_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point pt1;
    pt1.x = TEST_VALUE;
    pt1.y = TEST_VALUE;
    Point pt2;
    pt2.x = 0;
    pt2.y = 0;

    EXPECT_EQ(rect1->IsContains(pt1), true);
    EXPECT_EQ(rect1->IsContains(pt2), false);

    delete rect1;
}

/**
 * @tc.name: RectIsIntersect_001
 * @tc.desc: Verify IsIntersect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectIsIntersect_001, TestSize.Level0)
{
    Rect* rect1 = new Rect(MIN_VALUE, MIN_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect2 = new Rect(MAX_VALUE, MAX_VALUE, MIN_VALUE, MIN_VALUE);
    if (rect2 == nullptr) {
        delete rect1;
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect3 = new Rect();
    if (rect3 == nullptr) {
        delete rect1;
        delete rect2;
        EXPECT_EQ(1, 0);
        return;
    }

    EXPECT_EQ(rect1->IsIntersect(*rect2), true);
    EXPECT_EQ(rect1->IsIntersect(*rect3), false);

    delete rect1;
    delete rect2;
    delete rect3;
}

/**
 * @tc.name: RectIsContains_003
 * @tc.desc: Verify IsContains function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(RectTest, RectIsContains_003, TestSize.Level0)
{
    Rect* rect1 = new Rect(MIN_VALUE, MIN_VALUE, MAX_VALUE, MAX_VALUE);
    if (rect1 == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Rect* rect2 = new Rect(TEST_VALUE, TEST_VALUE, TEST_VALUE, TEST_VALUE);
    if (rect2 == nullptr) {
        delete rect1;
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(rect2->IsContains(*rect1), false);

    delete rect1;
    delete rect2;
}
} // namespace OHOS
