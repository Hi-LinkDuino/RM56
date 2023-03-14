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

#include "events/drag_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point NEW_POS = { 100, 200 };
    const Point LAST_POS = { 150, 250 };
    const Point TOTAL_LEN = { 50, 150 };
    constexpr uint8_t DIRECTION_LEFT_TO_RIGHT = 0;
    constexpr uint8_t DIRECTION_RIGHT_TO_LEFT = 1;
    constexpr uint8_t DIRECTION_TOP_TO_BOTTOM = 2;
    constexpr uint8_t DIRECTION_BOTTOM_TO_TOP = 3;
}

class DragEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static DragEvent* dragEvent_;
};

DragEvent* DragEventTest::dragEvent_ = nullptr;

void DragEventTest::SetUpTestCase(void)
{
    if (dragEvent_ == nullptr) {
        dragEvent_ = new DragEvent(NEW_POS, LAST_POS, TOTAL_LEN);
    }
}

void DragEventTest::TearDownTestCase(void)
{
    if (dragEvent_ != nullptr) {
        delete dragEvent_;
        dragEvent_ = nullptr;
    }
}
/**
 * @tc.name: DragEventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventConstructor_001, TestSize.Level0)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(dragEvent_->GetCurrentPos().x, NEW_POS.x);
    EXPECT_EQ(dragEvent_->GetCurrentPos().y, NEW_POS.y);
}

/**
 * @tc.name: DragEventGetLastPoint_001
 * @tc.desc: Verify GetLastPoint function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventGetLastPoint_001, TestSize.Level0)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(dragEvent_->GetLastPoint().x, LAST_POS.x);
    EXPECT_EQ(dragEvent_->GetLastPoint().y, LAST_POS.y);
}

/**
 * @tc.name: DragEventGetStartPoint_001
 * @tc.desc: Verify GetStartPoint function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventGetStartPoint_001, TestSize.Level0)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(dragEvent_->GetStartPoint().x, NEW_POS.x - TOTAL_LEN.x);
    EXPECT_EQ(dragEvent_->GetStartPoint().y, NEW_POS.y - TOTAL_LEN.y);
}

/**
 * @tc.name: DragEventSetPreLastPoint_001
 * @tc.desc: Verify SetPreLastPoint function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventSetPreLastPoint_001, TestSize.Level1)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const Point preLastPos = { 20, 40 };
    dragEvent_->SetPreLastPoint(preLastPos);
    EXPECT_EQ(dragEvent_->GetPreLastPoint().x, preLastPos.x);
    EXPECT_EQ(dragEvent_->GetPreLastPoint().y, preLastPos.y);
}

/**
 * @tc.name: DragEventGetDragDirection_001
 * @tc.desc: Verify GetDragDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventGetDragDirection_001, TestSize.Level0)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point startPos = dragEvent_->GetStartPoint();
    Point currentPos = dragEvent_->GetCurrentPos();
    if (MATH_ABS(currentPos.x - startPos.x) >= MATH_ABS(currentPos.y - startPos.y)) {
        if (currentPos.x > startPos.x) {
            EXPECT_EQ(dragEvent_->GetDragDirection(), DIRECTION_LEFT_TO_RIGHT);
        } else {
            EXPECT_EQ(dragEvent_->GetDragDirection(), DIRECTION_RIGHT_TO_LEFT);
        }
    } else {
        if (currentPos.y > startPos.y) {
            EXPECT_EQ(dragEvent_->GetDragDirection(), DIRECTION_TOP_TO_BOTTOM);
        } else {
            EXPECT_EQ(dragEvent_->GetDragDirection(), DIRECTION_BOTTOM_TO_TOP);
        }
    }
}

/**
 * @tc.name: DragEventGetDeltaX_001
 * @tc.desc: Verify GetDeltaX function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventGetDeltaX_001, TestSize.Level1)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(dragEvent_->GetDeltaX(), NEW_POS.x - LAST_POS.x);
}

/**
 * @tc.name: DragEventGetDeltaY_001
 * @tc.desc: Verify GetDeltaY function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(DragEventTest, DragEventGetDeltaY_001, TestSize.Level1)
{
    if (dragEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(dragEvent_->GetDeltaY(), NEW_POS.y - LAST_POS.y);
}
} // namespace OHOS
