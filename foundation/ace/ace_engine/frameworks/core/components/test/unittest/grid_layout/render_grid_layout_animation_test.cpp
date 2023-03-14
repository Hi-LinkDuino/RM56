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

#include <vector>
#include <thread>
#include "gtest/gtest.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/test/unittest/grid_layout/grid_layout_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
constexpr double SPRING_INIT_DISTANCE = 100.0;
constexpr int32_t ELEMENT_COUNT_TEST = 9;
constexpr double GRID_OFFSET_TEST = 10.0;
constexpr int32_t MOVE_COUNT_TEST = 5;
constexpr double MOVE_STEP_SIZE_TEST = 20.0;
constexpr int64_t EVENT_WAIT_MILLSECOND_TEST = 50;
constexpr int64_t USEC_TIMES_TEST = 1000000000;
constexpr int64_t USEC_WAIT_SECOND_TEST = 1000;
}

int64_t GetTickCountText()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * USEC_TIMES_TEST + ts.tv_nsec);
}

class RenderGridLayoutAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<RenderGridLayout> renderNode_;
    int32_t index_ = 0;
    RefPtr<Animator> frictionController_;
    RefPtr<Animator> springController_;
    RefPtr<RawRecognizer> slideRecognizer_;
    RefPtr<GestureRecognizer> dragDropGesture_;
    Offset coordinateOffset_;

    bool CreateGrid(bool isVertical, bool isSpringRecognizer);
    void BackItemsData(std::vector<Offset>& data);
    bool GetSpringRecognizer();
    bool GetDragDropRecognizer();
    void SetDragDropEvent(const RefPtr<GridLayoutComponent>& component);
    void MockTouchEventDown(TouchEvent& info);
    void MockTouchEventMove(TouchEvent& info);
    void MockTouchEventUp(TouchEvent& info);
    void MockDragTouchEventDown(TouchEvent& info);
    void MockDragTouchEventMove(TouchEvent& info);
    void MockDragTouchEventUp(TouchEvent& info);
    void WaitAndMockVsync(int64_t waitFor);
    void InitImageAnimatorComponent();
};

void RenderGridLayoutAnimationTest::SetUpTestCase() {}
void RenderGridLayoutAnimationTest::TearDownTestCase() {}

void RenderGridLayoutAnimationTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderNode_ = AceType::MakeRefPtr<RenderGridLayout>();
    renderNode_->Attach(mockContext_);
    coordinateOffset_.Reset();
}

void RenderGridLayoutAnimationTest::TearDown()
{
    mockContext_ = nullptr;
    renderNode_ = nullptr;
    frictionController_ = nullptr;
    springController_ = nullptr;
    slideRecognizer_ = nullptr;
    dragDropGesture_ = nullptr;
}

bool RenderGridLayoutAnimationTest::GetSpringRecognizer()
{
    TouchRestrict touchRestrict;
    TouchTestResult result;
    renderNode_->OnTouchTestHit(coordinateOffset_, touchRestrict, result);
    for (auto it = result.begin(); it != result.end(); it++) {
        auto slideRecognizer = AceType::DynamicCast<RawRecognizer>(*it);
        if (slideRecognizer) {
            slideRecognizer_ = WeakPtr<RawRecognizer>(slideRecognizer).Upgrade();
            break;
        }
    }
    if (slideRecognizer_) {
        return true;
    } else {
        return false;
    }
}

bool RenderGridLayoutAnimationTest::GetDragDropRecognizer()
{
    TouchRestrict touchRestrict;
    TouchTestResult result;
    renderNode_->OnTouchTestHit(coordinateOffset_, touchRestrict, result);
    for (auto it = result.begin(); it != result.end(); it++) {
        auto dragRecognizer = AceType::DynamicCast<GestureRecognizer>(*it);
        if (dragRecognizer) {
            dragDropGesture_ = WeakPtr<GestureRecognizer>(dragRecognizer).Upgrade();
            break;
        }
    }
    if (dragDropGesture_) {
        return true;
    } else {
        return false;
    }
}

void RenderGridLayoutAnimationTest::SetDragDropEvent(const RefPtr<GridLayoutComponent>& component)
{
    component->SetOnGridDragEnterId([](const ItemDragInfo& info) {});
    component->SetOnGridDragMoveId([](const ItemDragInfo& info, int32_t itemIndex, int32_t insertIndex) {});
    component->SetOnGridDragLeaveId([](const ItemDragInfo& info, int32_t itemIndex) {});
    component->SetOnGridDragStartId([](const ItemDragInfo& info, int32_t itemIndex) {return nullptr;});
    component->SetOnGridDropId(
        [](const ItemDragInfo& info, int32_t itemIndex, int32_t insertIndex, bool isSuccess) {});
}

bool RenderGridLayoutAnimationTest::CreateGrid(bool isVertical, bool isSpringRecognizer)
{
    std::string GRID_ROW_ARGS = "1fr 1fr 1fr ";
    std::string GRID_COL_ARGS = "1fr 1fr 1fr ";
    if (!renderNode_) {
        return false;
    }
    GridDragEventResult gridDragEvent;
    auto component = GridLayoutTestUtils::CreateDragComponent(GRID_ROW_ARGS, GRID_COL_ARGS);
    auto gridComponent = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!gridComponent) {
        return false;
    }
    gridComponent->SetSupportAnimation(true);
    gridComponent->SetDragAnimation(true);
    gridComponent->SetEdgeEffection(EdgeEffect::SPRING);
    gridComponent->SetEditMode(true);
    if (isVertical) {
        gridComponent->SetDirection(FlexDirection::COLUMN);
    } else {
        gridComponent->SetDirection(FlexDirection::ROW);
    }

    if (!isSpringRecognizer) {
        SetDragDropEvent(gridComponent);
    }
    renderNode_->Update(gridComponent);

    for (int32_t i = 0; i < ELEMENT_COUNT_TEST; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateDragRenderItem();
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", gridComponent);

    mockContext_->SetupRootElement();
    mockContext_->PushPage(pageComponent);
    mockContext_->OnVsyncEvent(GetTickCountText(), 0);

    renderNode_->PerformLayout();
    if (isSpringRecognizer) {
        return GetSpringRecognizer();
    } else {
        return GetDragDropRecognizer();
    }
}

void RenderGridLayoutAnimationTest::BackItemsData(std::vector<Offset>& data)
{
    data.clear();
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    for (const auto& item: items) {
        data.push_back(item->GetPosition());
    }
}

void RenderGridLayoutAnimationTest::MockTouchEventDown(TouchEvent& info)
{
    if (slideRecognizer_) {
        TouchPoint point;
        info.x = renderNode_->GetGlobalOffset().GetX() + GRID_OFFSET_TEST - coordinateOffset_.GetX();
        info.y = renderNode_->GetGlobalOffset().GetY() + GRID_OFFSET_TEST - coordinateOffset_.GetY();
        point.x = info.x;
        point.y = info.y;
        info.type = TouchType::DOWN;
        info.time = std::chrono::high_resolution_clock::now();
        point.x = info.x;
        point.y = info.y;
        info.pointers.clear();
        info.pointers.push_back(point);
        slideRecognizer_->HandleEvent(info);
    }
}

void RenderGridLayoutAnimationTest::MockTouchEventMove(TouchEvent& info)
{
    if (slideRecognizer_) {
        for (int32_t i = 0; i < MOVE_COUNT_TEST; i++) {
            TouchPoint point;
            info.x += MOVE_STEP_SIZE_TEST;
            info.y += MOVE_STEP_SIZE_TEST;
            info.type = TouchType::MOVE;
            info.time = std::chrono::high_resolution_clock::now();
            point.x = info.x;
            point.y = info.y;
            info.pointers.clear();
            info.pointers.push_back(point);
            slideRecognizer_->HandleEvent(info);
            std::this_thread::sleep_for(std::chrono::milliseconds(EVENT_WAIT_MILLSECOND_TEST));
        }
    }
}

void RenderGridLayoutAnimationTest::MockTouchEventUp(TouchEvent& info)
{
    if (slideRecognizer_) {
        TouchPoint point;
        info.type = TouchType::UP;
        info.time = std::chrono::high_resolution_clock::now();
        point.x = info.x;
        point.y = info.y;
        info.pointers.clear();
        info.pointers.push_back(point);
        slideRecognizer_->HandleEvent(info);
    }
}

void RenderGridLayoutAnimationTest::MockDragTouchEventDown(TouchEvent& info)
{
    if (dragDropGesture_) {
        info.x = renderNode_->GetGlobalOffset().GetX() + GRID_OFFSET_TEST - coordinateOffset_.GetX();
        info.y = renderNode_->GetGlobalOffset().GetY() + GRID_OFFSET_TEST - coordinateOffset_.GetY();
        info.type = TouchType::DOWN;
        info.time = std::chrono::high_resolution_clock::now();
        dragDropGesture_->HandleEvent(info);
    }
}

void RenderGridLayoutAnimationTest::MockDragTouchEventMove(TouchEvent& info)
{
    if (dragDropGesture_) {
        for (int32_t i = 0; i < MOVE_COUNT_TEST; i++) {
            info.y -= MOVE_STEP_SIZE_TEST;
            info.type = TouchType::MOVE;
            info.time = std::chrono::high_resolution_clock::now();
            dragDropGesture_->HandleEvent(info);
            std::this_thread::sleep_for(std::chrono::milliseconds(EVENT_WAIT_MILLSECOND_TEST));
        }
    }
}

void RenderGridLayoutAnimationTest::MockDragTouchEventUp(TouchEvent& info)
{
    if (dragDropGesture_) {
        info.type = TouchType::UP;
        info.time = std::chrono::high_resolution_clock::now();
        dragDropGesture_->HandleEvent(info);
    }
}

void RenderGridLayoutAnimationTest::WaitAndMockVsync(int64_t waitFor)
{
    constexpr int64_t RUNNING_TIME_STEP_TEST = 16;
    constexpr int64_t USLEEP_TIME_TEST = 16000;
    int64_t runningTime = 0;
    do {
        runningTime += RUNNING_TIME_STEP_TEST;
        mockContext_->OnVsyncEvent(GetTickCountText(), 0);
        usleep(USLEEP_TIME_TEST);
    } while (runningTime < waitFor);
    mockContext_->OnVsyncEvent(GetTickCountText(), 0);
}

/**
 * @tc.name: RenderGridLayoutSpringTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RenderGridLayoutAnimationTest, RenderGridLayoutSpringTest001, TestSize.Level1)
{
    TouchEvent touchPoint;
    std::vector<Offset> data;
    bool result = CreateGrid(true, true);
    ASSERT_TRUE(result);
    BackItemsData(data);
    MockTouchEventDown(touchPoint);
    MockTouchEventMove(touchPoint);
    int32_t index = 0;
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        index++;
        ASSERT_TRUE(NearZero(dOffset.GetY() - SPRING_INIT_DISTANCE));
    }

    MockTouchEventUp(touchPoint);
    WaitAndMockVsync(USEC_WAIT_SECOND_TEST);
    index = 0;
    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        index++;
        ASSERT_TRUE(NearZero(dOffset.GetY() - SPRING_INIT_DISTANCE));
    }
}

/**
 * @tc.name: RenderGridLayoutSpringTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RenderGridLayoutAnimationTest, RenderGridLayoutSpringTest002, TestSize.Level1)
{
    TouchEvent touchPoint;
    std::vector<Offset> data;
    bool result = CreateGrid(false, true);
    ASSERT_TRUE(result);
    BackItemsData(data);
    MockTouchEventDown(touchPoint);
    MockTouchEventMove(touchPoint);
    int32_t index = 0;
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        index++;
        ASSERT_TRUE(NearZero(dOffset.GetX() - SPRING_INIT_DISTANCE));
    }

    MockTouchEventUp(touchPoint);
    WaitAndMockVsync(USEC_WAIT_SECOND_TEST);
    index = 0;
    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        index++;
        ASSERT_TRUE(NearZero(dOffset.GetX() - SPRING_INIT_DISTANCE));
    }
}

/**
 * @tc.name: RenderGridLayoutDragDropTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RenderGridLayoutAnimationTest, RenderGridLayoutDragDropTest001, TestSize.Level1)
{
    TouchEvent touchPoint;
    std::vector<Offset> data;
    bool result = CreateGrid(true, false);
    ASSERT_TRUE(result);
    BackItemsData(data);
    MockDragTouchEventDown(touchPoint);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    MockDragTouchEventMove(touchPoint);
    int32_t index = 0;
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();

    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        ASSERT_TRUE(NearZero(dOffset.GetY()));
        index++;
    }

    MockDragTouchEventUp(touchPoint);
    WaitAndMockVsync(USEC_WAIT_SECOND_TEST);
    index = 0;
    for (const auto& item: items) {
        Offset dOffset = item->GetPosition() - data[index];
        ASSERT_TRUE(NearZero(dOffset.GetX()));
        index++;
    }
}
} // namespace OHOS::Ace
