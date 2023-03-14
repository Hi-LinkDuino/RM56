/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <cstdlib>

#include "gtest/gtest.h"

#include "core/components_v2/pattern_lock/pattern_lock_component.h"
#include "core/components_v2/pattern_lock/render_pattern_lock.h"
#include "core/pipeline/base/render_node.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#define private public
#define protect public
using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace {
namespace {
using PatternLockEventCallback = std::function<void(const std::string&,  const BaseEventInfo& info)>;
constexpr int16_t UT_COL_COUNT = 3;
constexpr Dimension UT_SIDE_LENGTH = 600.0_vp;
const double UT_DISTENCE_STEP = UT_SIDE_LENGTH.Value() / 6;
constexpr Dimension UT_CIRCLE_RADIUS = 28.0_vp;
constexpr Dimension UT_PATH_WIDTH = 34.0_vp;
constexpr Dimension UT_CIRCLE_ACTIVE_RADIUS = 52.0_vp;
const Offset UT_CELLS_CENTER_POINT[9] = { Offset(UT_DISTENCE_STEP, UT_DISTENCE_STEP),
    Offset(UT_DISTENCE_STEP * 3, UT_DISTENCE_STEP), Offset(UT_DISTENCE_STEP * 5, UT_DISTENCE_STEP),
    Offset(UT_DISTENCE_STEP, UT_DISTENCE_STEP * 3), Offset(UT_DISTENCE_STEP * 3, UT_DISTENCE_STEP * 3),
    Offset(UT_DISTENCE_STEP * 5, UT_DISTENCE_STEP * 3), Offset(UT_DISTENCE_STEP, UT_DISTENCE_STEP * 5),
    Offset(UT_DISTENCE_STEP * 3, UT_DISTENCE_STEP * 5), Offset(UT_DISTENCE_STEP * 5, UT_DISTENCE_STEP * 5) };
constexpr int UT_CELLS_CODE[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
constexpr int16_t NUM_INT_TWO = 2;
constexpr int16_t NUM_INT_THREE = 3;
Offset GetRandomPointInActiveArea(const Offset& cellCenter)
{
    return Offset(cellCenter.GetX(), cellCenter.GetY() + UT_CIRCLE_RADIUS.Value());
}

Offset GetPointInActiveAreaBoundary(const Offset& cellCenter)
{
    return Offset(cellCenter.GetX() + UT_CIRCLE_ACTIVE_RADIUS.Value(), cellCenter.GetY());
}

Offset GetPointOutOfActiveAreaBoundary(const Offset& cellCenter)
{
    return Offset(cellCenter.GetX() + UT_CIRCLE_ACTIVE_RADIUS.Value() + 1, cellCenter.GetY());
}
class TestPatternLockEventHander : public AceEventHandler {
public:
    explicit TestPatternLockEventHander(PatternLockEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestPatternLockEventHander() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, info);
        }
    };
    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent, bool& result) override {};
    void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override {};
private:
    PatternLockEventCallback eventCallback_;
};
} // namespace
class MockRenderPatternLock final : public V2::RenderPatternLock {
    DECLARE_ACE_TYPE(MockRenderPatternLock, RenderPatternLock);

public:
    MockRenderPatternLock() = default;
    ~MockRenderPatternLock() override = default;
    Dimension GetSideLength() const
    {
        return sideLength_;
    }
    Dimension GetCircleRadius() const
    {
        return circleRadius_;
    }
    Color GetActiveColor() const
    {
        return activeColor_;
    }
    Color GetPathColor() const
    {
        return pathColor_;
    }
    Color GetRegularColor() const
    {
        return regularColor_;
    }
    Color GetSelectedColor() const
    {
        return selectedColor_;
    }
    Dimension GetStrokeWidth() const
    {
        return strokeWidth_;
    }
    bool GetIsMoveEventValid() const
    {
        return isMoveEventValid_;
    }
    Offset GetCellCenter() const
    {
        return cellCenter_;
    }
    std::vector<PatternLockCell> GetChoosePoint() const
    {
        return choosePoint_;
    }
    bool MockAddChoosePoint(Offset offset, int16_t x, int16_t y)
    {
        return AddChoosePoint(offset, x, y);
    }
    bool MockCheckChoosePointIsLastIndex(int16_t x, int16_t y, int16_t index) const
    {
        return CheckChoosePointIsLastIndex(x, y, index);
    }
    bool MockCheckChoosePoint(int16_t x, int16_t y) const
    {
        return CheckChoosePoint(x, y);
    }
    Offset MockGetCircleCenterByXY(const Offset& offset, int16_t x, int16_t y)
    {
        return GetCircleCenterByXY(offset, x, y);
    }
    void MockHandleCellTouchDown(const Offset& offset)
    {
        HandleCellTouchDown(offset);
    }
    void MockHandleCellTouchMove(const Offset& offset)
    {
        HandleCellTouchMove(offset);
    }
    void MockHandleCellTouchUp()
    {
        HandleCellTouchUp();
    }
    void MockHandleCellTouchCancel()
    {
        HandleCellTouchCancel();
    }
    void MockHandleReset()
    {
        HandleReset();
    }
};
class RenderPatternLockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
protected:
    RefPtr<PipelineContext> context_;
    RefPtr<MockRenderPatternLock> renderPatternLock_;
};
void RenderPatternLockTest::SetUpTestCase() {}
void RenderPatternLockTest::TearDownTestCase() {}
void RenderPatternLockTest::SetUp()
{
    context_ = MockRenderCommon::GetMockContext();
    renderPatternLock_ = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock_->Attach(context_);
}
void RenderPatternLockTest::TearDown()
{
    context_ = nullptr;
}
/**
 * @tc.name: RenderPatternLockUpdate001
 * @tc.desc:RenderPatternLock  Update from Component params
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockUpdate001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetStrokeWidth(UT_PATH_WIDTH);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    patternLock->SetRegularColor(Color::GREEN);
    patternLock->SetActiveColor(Color::BLACK);
    patternLock->SetSelectedColor(Color::RED);
    patternLock->SetPathColor(Color::BLUE);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);
    EXPECT_EQ(renderPatternLock->GetActiveColor(), Color::BLACK);
    EXPECT_EQ(renderPatternLock->GetPathColor(), Color::BLUE);
    EXPECT_EQ(renderPatternLock->GetRegularColor(), Color::GREEN);
    EXPECT_EQ(renderPatternLock->GetSelectedColor(), Color::RED);
    EXPECT_EQ(renderPatternLock->GetCircleRadius(), UT_CIRCLE_RADIUS);
    EXPECT_EQ(renderPatternLock->GetStrokeWidth(), UT_PATH_WIDTH);
    EXPECT_EQ(renderPatternLock->GetSideLength(), UT_SIDE_LENGTH);
}
/**
 * @tc.name: RenderPatternLockPerformLayout001
 * @tc.desc:RenderPatternLock PerformLayout  will  set cellcenter
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockPerformLayout001, TestSize.Level1)
{
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->PerformLayout();

    EXPECT_TRUE(renderPatternLock->GetLayoutSize() ==
                Size(renderPatternLock->GetSideLength().Value(), renderPatternLock->GetSideLength().Value()));

    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    renderPatternLock->Update(patternLock);
    renderPatternLock->PerformLayout();
    // need checkout layout if do context
}
/**
 * @tc.name: RenderPatternLockGetCircleCenterByXY001
 * @tc.desc:RenderPatternLock  GetCircleCenterByXY
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockGetCircleCenterByXY001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    Offset zero_zero = renderPatternLock->MockGetCircleCenterByXY(Offset::Zero(), 0, 0);

    Offset offset = Offset::Zero();

    for (int y = 0; y < UT_COL_COUNT; y++) {
        for (int x = 0; x < UT_COL_COUNT; x++) {
            EXPECT_EQ(static_cast<int>(renderPatternLock->MockGetCircleCenterByXY(offset, x + 1, y + 1).GetX()),
                static_cast<int>(UT_CELLS_CENTER_POINT[x + y * UT_COL_COUNT].GetX()));
            EXPECT_EQ(static_cast<int>(renderPatternLock->MockGetCircleCenterByXY(offset, x + 1, y + 1).GetY()),
                static_cast<int>(UT_CELLS_CENTER_POINT[x + y * UT_COL_COUNT].GetY()));
        }
    }
}
/**
 * @tc.name: RenderPatternLockAddChoosePoint001
 * @tc.desc:if do not have point,add this point,else do not add
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockAddChoosePoint001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    Offset offset = Offset::Zero();

    for (int y = 0; y < UT_COL_COUNT; y++) {
        for (int x = 0; x < UT_COL_COUNT; x++) {
            EXPECT_FALSE(renderPatternLock->MockCheckChoosePoint(x + 1, y + 1));
            EXPECT_TRUE(
                renderPatternLock->MockAddChoosePoint(UT_CELLS_CENTER_POINT[x + y * UT_COL_COUNT], x + 1, y + 1));

            EXPECT_TRUE(renderPatternLock->MockCheckChoosePoint(x + 1, y + 1));

            // Add  the same cell again
            EXPECT_FALSE(renderPatternLock->MockAddChoosePoint(offset, x + 1, y + 1));
        }
    }

    EXPECT_FALSE(renderPatternLock->MockCheckChoosePoint(UT_COL_COUNT + NUM_INT_TWO, UT_COL_COUNT + NUM_INT_TWO));
}
/**
 * @tc.name: RenderPatternLockAddChoosePoint001
 * @tc.desc:check  point  index from vector,back to front
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockCheckChoosePointIsLastIndex001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    renderPatternLock->MockHandleCellTouchDown(UT_CELLS_CENTER_POINT[0]);
    renderPatternLock->MockHandleCellTouchMove(UT_CELLS_CENTER_POINT[1]);

    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), NUM_INT_TWO);

    EXPECT_TRUE(renderPatternLock->MockCheckChoosePointIsLastIndex(NUM_INT_TWO, 1, 1));
    EXPECT_TRUE(renderPatternLock->MockCheckChoosePointIsLastIndex(1, 1, NUM_INT_TWO));

    EXPECT_FALSE(renderPatternLock->MockCheckChoosePointIsLastIndex(1, 1, 1));
    EXPECT_FALSE(renderPatternLock->MockCheckChoosePointIsLastIndex(NUM_INT_TWO, 1, NUM_INT_TWO));
    EXPECT_FALSE(renderPatternLock->MockCheckChoosePointIsLastIndex(1, 1, NUM_INT_THREE));
}
/**
 * @tc.name: RenderPatternLockTouchDown001_InActiveArea
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchDown001_InActiveArea, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    int testCount = 5;

    Offset offset;

    // Touch all cell to test if the cell can been selected.
    for (int j = 0; j < testCount; j++) {
        for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
            offset = GetRandomPointInActiveArea(UT_CELLS_CENTER_POINT[i]);
            renderPatternLock->MockHandleCellTouchDown(offset);
            EXPECT_TRUE(renderPatternLock->GetCellCenter() == offset);
            EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
            EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 1);
            EXPECT_EQ(renderPatternLock->GetChoosePoint()[0].GetCode(), UT_CELLS_CODE[i]);
        }
    }
}

/**
 * @tc.name: RenderPatternLockTouchDown002_InActiveAreaBoundary
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchDown002_InActiveAreaBoundary, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    // Touch all cell to test if the cell can been selected.
    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        renderPatternLock->MockHandleCellTouchDown(GetPointInActiveAreaBoundary(UT_CELLS_CENTER_POINT[i]));
        EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
        EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 1);
        EXPECT_EQ(renderPatternLock->GetChoosePoint()[0].GetCode(), UT_CELLS_CODE[i]);
    }
}
/**
 * @tc.name: RenderPatternLockTouchDown003_OutOfActiveAreaBoundary
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchDown003_OutOfActiveAreaBoundary, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    // Touch all cell to test if the cell can been selected.
    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        renderPatternLock->MockHandleCellTouchDown(GetPointOutOfActiveAreaBoundary(UT_CELLS_CENTER_POINT[i]));
        EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
        EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    }
}
/**
 * @tc.name: RenderPatternLockTouchMove001
 * @tc.desc:RenderPatternLock TouchMove  will  set choose point only after TouchDown
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchMove001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);
    // start  ChoosePointVector is empty
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    // IsMoveEventValid is false ,do not add point
    renderPatternLock->MockHandleCellTouchMove(UT_CELLS_CENTER_POINT[0]);
    EXPECT_FALSE(renderPatternLock->GetIsMoveEventValid());
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    // this point is not conform,do not add point,but IsMoveEventValid will true

    renderPatternLock->MockHandleCellTouchDown(Offset::Zero());
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    renderPatternLock->MockHandleCellTouchMove(UT_CELLS_CENTER_POINT[0]);
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 1);
    EXPECT_EQ(renderPatternLock->GetChoosePoint()[0].GetCode(), UT_CELLS_CODE[0]);
}
/**
 * @tc.name: RenderPatternLockTouchMove002_IntoActiveArea
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchMove002_IntoActiveArea, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    renderPatternLock->MockHandleCellTouchDown(Offset::Zero());
    EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    Offset offset;

    // Touch all cell to test if the cell can been selected.
    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        offset = GetRandomPointInActiveArea(UT_CELLS_CENTER_POINT[i]);
        renderPatternLock->MockHandleCellTouchMove(offset);
        EXPECT_TRUE(renderPatternLock->GetCellCenter() == offset);
        EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), i + 1);
        EXPECT_EQ(renderPatternLock->GetChoosePoint()[i].GetCode(), UT_CELLS_CODE[i]);
    }
}
/**
 * @tc.name: RenderPatternLockTouchMove003_ToActiveAreaBoundary
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchMove003_ToActiveAreaBoundary, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    renderPatternLock->MockHandleCellTouchDown(Offset::Zero());
    EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    // Touch all cell to test if the cell can been selected.
    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        renderPatternLock->MockHandleCellTouchMove(GetPointInActiveAreaBoundary(UT_CELLS_CENTER_POINT[i]));
        EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), i + 1);
        EXPECT_EQ(renderPatternLock->GetChoosePoint()[i].GetCode(), UT_CELLS_CODE[i]);
    }
}
/**
 * @tc.name: RenderPatternLockTouchMove004_ToOutOfActiveAreaBoundary
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchMove004_ToOutOfActiveAreaBoundary, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    renderPatternLock->MockHandleCellTouchDown(Offset::Zero());
    EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    // Touch all cell to test if the cell can been selected.
    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        renderPatternLock->MockHandleCellTouchMove(GetPointOutOfActiveAreaBoundary(UT_CELLS_CENTER_POINT[i]));
        EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    }
}
/**
 * @tc.name: RenderPatternLockTouchMove004_ToOutOfActiveAreaBoundary
 * @tc.desc:RenderPatternLock TouchDown  will  set choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockTouchMove005_TouchSameCell, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);

    renderPatternLock->MockHandleCellTouchDown(Offset::Zero());

    for (size_t i = 0; i < std::size(UT_CELLS_CENTER_POINT); i++) {
        for (int j = 0; j < NUM_INT_TWO; j++) {
            renderPatternLock->MockHandleCellTouchMove(GetRandomPointInActiveArea(UT_CELLS_CENTER_POINT[i]));
            EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), i + 1);
            EXPECT_EQ(renderPatternLock->GetChoosePoint()[i].GetCode(), UT_CELLS_CODE[i]);
        }
    }
}
/**
 * @tc.name: RenderPatternLockHandleReset001
 * @tc.desc:MockHandleReset will  reset basic parameter
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockHandleReset001, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);
    // start  ChoosePointVector is empty
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    // This point is conform,ChoosePointVector  will add this point
    renderPatternLock->MockHandleCellTouchDown(UT_CELLS_CENTER_POINT[0]);
    EXPECT_TRUE(renderPatternLock->GetCellCenter() == UT_CELLS_CENTER_POINT[0]);
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 1);
    EXPECT_EQ(renderPatternLock->GetChoosePoint()[0].GetCode(), UT_CELLS_CODE[0]);
    EXPECT_TRUE(renderPatternLock->GetIsMoveEventValid());

    renderPatternLock->MockHandleReset();

    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
    EXPECT_FALSE(renderPatternLock->GetIsMoveEventValid());
    EXPECT_TRUE(renderPatternLock->GetCellCenter().IsZero());
}
/**
 * @tc.name: RenderPatternLockHandleReset002_ByController
 * @tc.desc:MockHandleReset will  reset basic parameter
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockHandleReset002_ByController, TestSize.Level1)
{
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    patternLock->SetCircleRadius(UT_CIRCLE_RADIUS);
    auto controller = patternLock->GetPatternLockController();
    EXPECT_NE(controller, nullptr);

    RefPtr<MockRenderPatternLock> renderPatternLock = AceType::MakeRefPtr<MockRenderPatternLock>();
    renderPatternLock->Update(patternLock);
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);

    // This point is conform,ChoosePointVector  will add this point
    renderPatternLock->MockHandleCellTouchDown(UT_CELLS_CENTER_POINT[0]);
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 1);

    controller->Reset();
    EXPECT_EQ(renderPatternLock->GetChoosePoint().size(), 0);
}
/**
 * @tc.name: RenderPatternLockEvent001
 * @tc.desc: event  return choose point
 * @tc.type: FUNC
 */
HWTEST_F(RenderPatternLockTest, RenderPatternLockEvent001, TestSize.Level1)
{
    std::string lockEventId = "patternCompleteEvent";
    RefPtr<TestPatternLockEventHander> eventHandler = AceType::MakeRefPtr<TestPatternLockEventHander>(
        [this, lockEventId](const std::string& eventId, const BaseEventInfo& info) {
            GTEST_LOG_(INFO) << "RenderPatternLockEvent001 handler run";
            auto lockEvent = TypeInfoHelper::DynamicCast<V2::PatternCompleteEvent>(&info);
            EXPECT_EQ(eventId, lockEventId);
            EXPECT_TRUE(lockEvent->GetInput()[0] == NUM_INT_THREE);
        });

    context_->RegisterEventHandler(eventHandler);
    RefPtr<V2::PatternLockComponent> patternLock = AceType::MakeRefPtr<V2::PatternLockComponent>();
    patternLock->SetPatternCompleteEvent(EventMarker(lockEventId));
    patternLock->SetSideLength(UT_SIDE_LENGTH);
    renderPatternLock_->Update(patternLock);
    renderPatternLock_->PerformLayout();
    renderPatternLock_->MockHandleCellTouchDown(UT_CELLS_CENTER_POINT[NUM_INT_THREE]);
    renderPatternLock_->MockHandleCellTouchUp();
}
} // namespace OHOS::Ace
