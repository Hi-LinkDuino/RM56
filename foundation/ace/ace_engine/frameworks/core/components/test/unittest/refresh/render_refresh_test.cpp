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

#include "gtest/gtest.h"

#include "base/i18n/localization.h"
#include "base/log/log.h"
#include "core/components/box/render_box.h"
#include "core/components/refresh/refresh_component.h"
#include "core/components/refresh/refresh_theme.h"
#include "core/components/refresh/render_refresh.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using RefreshEventCallback = std::function<void(const std::string&, const std::string&)>;
using PulldownEventCallback = std::function<void(const std::string&, const std::string&)>;

constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t MILLISEC_TO_MICROSEC = 1000;
constexpr int64_t VSYNC_INTERVAL_MILLISEC = 16;
constexpr int64_t VSYNC_INTERVAL_MICROSEC = VSYNC_INTERVAL_MILLISEC * MILLISEC_TO_MICROSEC;
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

static int64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec);
}

const TouchEvent MOCK_DOWN_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::MOVE };
const TouchEvent MOCK_UP_TOUCH_EVENT { 10, 540.0f, 500.0f, TouchType::UP };

enum class DragDirection {
    UP,
    DOWN,
};

class TestPulldownEventHandler : public AceEventHandler {
public:
    explicit TestPulldownEventHandler(PulldownEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestPulldownEventHandler() override = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    }

    void HandleAsyncEvent(const EventMarker& eventMarker) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent, bool& result) override {};
    void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override {};

private:
    PulldownEventCallback eventCallback_;
};

class MockRenderRefresh final : public RenderRefresh {
    DECLARE_ACE_TYPE(MockRenderRefresh, RenderRefresh);

public:
    MockRenderRefresh() = default;
    ~MockRenderRefresh() override = default;
};

class MockRenderBox : public RenderBox {};
} // namespace

class RenderRefreshTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void DragRefresh(const DragDirection& dragDirection);
    void WaitAndMockVsync(int64_t waitFor);

    RefPtr<PipelineContext> context_;
    RefPtr<RenderRefresh> renderRefresh_;
};

void RenderRefreshTest::SetUpTestCase()
{
    Localization::GetInstance()->SetLocale("zh", "CN", "", "", "");
    GTEST_LOG_(INFO) << "RefreshTest SetUpTestCase";
}

void RenderRefreshTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "RefreshTest TearDownTestCase";
}

void RenderRefreshTest::SetUp()
{
    context_ = MockRenderCommon::GetMockContext();
    renderRefresh_ = AceType::MakeRefPtr<RenderRefresh>();
    renderRefresh_->Attach(context_);
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(context_);
    renderRefresh_->AddChild(renderBox);
}

void RenderRefreshTest::TearDown()
{
    renderRefresh_ = nullptr;
    context_ = nullptr;
}

void RenderRefreshTest::WaitAndMockVsync(int64_t waitFor)
{
    int64_t runningTime = 0;
    while (runningTime <= waitFor) {
        runningTime += VSYNC_INTERVAL_MILLISEC;
        context_->OnVsyncEvent(GetTickCount(), 0);
        usleep(VSYNC_INTERVAL_MICROSEC);
    }
    context_->OnVsyncEvent(GetTickCount(), 0);
}

void RenderRefreshTest::DragRefresh(const DragDirection& dragDirection)
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    TouchEvent touchPoint = MOCK_MOVE_TOUCH_EVENT;
    for (int32_t i = 0; i < 3; i++) {
        context_->OnTouchEvent(touchPoint);
        context_->OnVsyncEvent(GetTickCount(), 0);
        if (dragDirection == DragDirection::DOWN) {
            touchPoint.y += 20.0f;
        } else if (dragDirection == DragDirection::UP) {
            touchPoint.y -= 20.0f;
        }
    }
    TouchEvent touchEndPoint = MOCK_UP_TOUCH_EVENT;
    touchEndPoint.x = touchPoint.x;
    touchEndPoint.y = touchPoint.y;
    context_->OnTouchEvent(touchEndPoint);
    usleep(VSYNC_INTERVAL_MICROSEC);
    context_->OnVsyncEvent(GetTickCount(), 0);
}

/**
 * @tc.name: RenderRefreshTest001
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest001 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();

    ASSERT_TRUE(refresh->GetRefreshType() == RefreshType::AUTO);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest001 stop";
}

/**
 * @tc.name: RenderRefreshTest002
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest002 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    ASSERT_TRUE(refresh->IsRefreshing() == false);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest002 stop";
}

/**
 * @tc.name: RenderRefreshTest003
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest003 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    ASSERT_TRUE(refresh->GetIndicatorOffset() == Dimension(16.0, DimensionUnit::VP));
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest003 stop";
}

/**
 * @tc.name: RenderRefreshTest004
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RI
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest004 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    ASSERT_TRUE(refresh->IsShowLastTime() == false);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest004 stop";
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: RenderRefreshTest005
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RL
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest005 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    ASSERT_TRUE(refresh->GetBackgroundColor() == Color::WHITE);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest005 stop";
}

/**
 * @tc.name: RenderRefreshTest006
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000DQ1RL
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest006 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    ASSERT_TRUE(refresh->GetProgressColor() == Color::BLACK);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest006 stop";
}
#endif

/**
 * @tc.name: RenderRefreshTest007
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000E8T7M
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest007 start";

    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    auto theme = THEME_MANAGER->GetTheme<RefreshTheme>();
    if (theme) {
        refresh->SetLoadingDistance(theme->GetLoadingDistance());
        refresh->SetRefreshDistance(theme->GetRefreshDistance());
        refresh->SetProgressDistance(theme->GetProgressDistance());
        refresh->SetProgressDiameter(theme->GetProgressDiameter());
        refresh->SetMaxDistance(theme->GetMaxDistance());
        refresh->SetShowTimeDistance(theme->GetShowTimeDistance());
    }
    renderRefresh_->Update(refresh);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::INACTIVE);

    /**
     * @tc.steps: step2. handle drag update 10px
     * @tc.expected: step2. Refresh change status to drag.
     */
    renderRefresh_->HandleDragUpdate(10.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::DRAG);

    /**
    * @tc.steps: step3. handle drag update 200px
    * @tc.expected: step3. Refresh change status to drag.
    */
    renderRefresh_->HandleDragUpdate(200.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::OVER_DRAG);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::DONE);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest007 stop";
}

/**
 * @tc.name: RenderRefreshTest008
 * @tc.desc: Test the refresh
 * @tc.type: FUNC
 * @tc.require: AR000E8T7M
 * @tc.author: liujinwei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest008 start";

    std::string pulldownEventId = "pulldown_id";

    RefPtr<TestPulldownEventHandler> eventHandler = AceType::MakeRefPtr<TestPulldownEventHandler>(
        [this, pulldownEventId](const std::string& eventId, const std::string& param) {
            std::string expectPulldownResult =
                std::string("\"pulldown\",{\"state\":\"start\"").append("},null");
            EXPECT_EQ(param, expectPulldownResult);
            EXPECT_EQ(eventId, pulldownEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    RefPtr<RefreshComponent> refresh = AceType::MakeRefPtr<RefreshComponent>();
    refresh->SetPulldownEventId(EventMarker(pulldownEventId));

    auto theme = THEME_MANAGER->GetTheme<RefreshTheme>();
    if (theme) {
        refresh->SetLoadingDistance(theme->GetLoadingDistance());
        refresh->SetRefreshDistance(theme->GetRefreshDistance());
        refresh->SetProgressDistance(theme->GetProgressDistance());
        refresh->SetProgressDiameter(theme->GetProgressDiameter());
        refresh->SetMaxDistance(theme->GetMaxDistance());
        refresh->SetShowTimeDistance(theme->GetShowTimeDistance());
    }
    renderRefresh_->Update(refresh);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::INACTIVE);

    /**
     * @tc.steps: step2. handle drag update 10px
     * @tc.expected: step2. Refresh change status to drag.
     */
    renderRefresh_->HandleDragUpdate(10.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(renderRefresh_->GetStatus() == RefreshStatus::DRAG);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest008 stop";
}

/**
 * @tc.name: RenderRefreshTest009
 * @tc.desc: Test the refresh pulldown to OVER_DRAG STATE
 * @tc.type: FUNC
 * @tc.require: AR000F3CAK
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest009 start";
    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    auto refresh = AceType::MakeRefPtr<RefreshComponent>();
    auto theme = THEME_MANAGER->GetTheme<RefreshTheme>();
    if (theme) {
        refresh->SetProgressDistance(theme->GetProgressDistance());
        refresh->SetProgressDiameter(theme->GetProgressDiameter());
        refresh->SetRefreshDistance(theme->GetRefreshDistance());
        refresh->SetMaxDistance(theme->GetMaxDistance());
        refresh->SetShowTimeDistance(theme->GetShowTimeDistance());
        refresh->SetLoadingDistance(theme->GetLoadingDistance());
    }
    renderRefresh_->RemoveChild(renderRefresh_->GetChildren().front());
    renderRefresh_->Update(refresh);
    auto loadingBox = renderRefresh_->GetChildren().front();
    ASSERT_TRUE(loadingBox != nullptr && !loadingBox->GetChildren().empty());
    auto loadingBGBox = loadingBox->GetChildren().front();
    ASSERT_TRUE(loadingBGBox != nullptr);
    auto loading = AceType::DynamicCast<RenderLoadingProgress>(loadingBGBox->GetChildren().front());
    ASSERT_TRUE(loading != nullptr);
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);

    /**
     * @tc.steps: step2. Trigger pulldown event to refresh.
     * @tc.expected: step2. Refresh and Loading state correctly.
     */
    renderRefresh_->HandleDragUpdate(100.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);
    renderRefresh_->HandleDragEnd();
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_LOOP);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest009 stop";
}

/**
 * @tc.name: RenderRefreshTest010
 * @tc.desc: Test the refresh pulldown to DRAG STATE
 * @tc.type: FUNC
 * @tc.require: AR000F3CAL
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest010 start";
    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh component displays correctly.
     */
    auto refresh = AceType::MakeRefPtr<RefreshComponent>();
    auto theme = THEME_MANAGER->GetTheme<RefreshTheme>();
    if (theme) {
        refresh->SetShowTimeDistance(theme->GetShowTimeDistance());
        refresh->SetLoadingDistance(theme->GetLoadingDistance());
        refresh->SetRefreshDistance(theme->GetRefreshDistance());
        refresh->SetProgressDiameter(theme->GetProgressDiameter());
        refresh->SetMaxDistance(theme->GetMaxDistance());
        refresh->SetProgressDistance(theme->GetProgressDistance());
    }
    renderRefresh_->RemoveChild(renderRefresh_->GetChildren().front());
    renderRefresh_->Update(refresh);
    auto loadingBox = renderRefresh_->GetChildren().front();
    ASSERT_TRUE(loadingBox != nullptr);
    auto loadingBGBox = loadingBox->GetChildren().front();
    ASSERT_TRUE(loadingBGBox != nullptr);
    auto loading = AceType::DynamicCast<RenderLoadingProgress>(loadingBGBox->GetChildren().front());
    ASSERT_TRUE(loading != nullptr);
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);

    /**
     * @tc.steps: step2. Trigger pulldown event to refresh.
     * @tc.expected: step2. Refresh and Loading state correctly.
     */
    renderRefresh_->HandleDragUpdate(10.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);
    renderRefresh_->HandleDragEnd();
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest010 stop";
}

/**
 * @tc.name: RenderRefreshTest011
 * @tc.desc: Test the refresh change from REFRESH to NONE STATE
 * @tc.type: FUNC
 * @tc.require: AR000F3CAM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderRefreshTest, RenderRefreshTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest011 start";
    /**
     * @tc.steps: step1. Build a component tree containing sub-components
     * @tc.expected: step1. Refresh and Loading state correctly.
     */
    auto refresh = AceType::MakeRefPtr<RefreshComponent>();
    refresh->SetRefreshEventId(EventMarker("eventId"));
    auto theme = THEME_MANAGER->GetTheme<RefreshTheme>();
    if (theme) {
        refresh->SetProgressDistance(theme->GetProgressDistance());
        refresh->SetLoadingDistance(theme->GetLoadingDistance());
        refresh->SetRefreshDistance(theme->GetRefreshDistance());
        refresh->SetMaxDistance(theme->GetMaxDistance());
        refresh->SetShowTimeDistance(theme->GetShowTimeDistance());
        refresh->SetProgressDiameter(theme->GetProgressDiameter());
    }
    renderRefresh_->RemoveChild(renderRefresh_->GetChildren().front());
    renderRefresh_->Update(refresh);
    auto box = renderRefresh_->GetChildren().front();
    ASSERT_TRUE(box != nullptr);
    auto bgBox = box->GetChildren().front();
    ASSERT_TRUE(bgBox != nullptr);
    auto loading = AceType::DynamicCast<RenderLoadingProgress>(bgBox->GetChildren().front());
    ASSERT_TRUE(loading != nullptr);
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);

    /**
     * @tc.steps: step2. Trigger pulldown event to refresh.
     * @tc.expected: step2. Refresh and Loading state correctly.
     */
    renderRefresh_->HandleDragUpdate(100.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_DRAG);
    renderRefresh_->HandleDragEnd();
    renderRefresh_->SetRefreshStatus(true);
    renderRefresh_->HandleDragUpdate(-100.0);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_LOOP);
    refresh->SetRefreshing(false);
    renderRefresh_->Update(refresh);
    renderRefresh_->PerformLayout();
    ASSERT_TRUE(loading->GetLoadingMode() == MODE_EXIT);
    GTEST_LOG_(INFO) << "RenderRefreshTest RenderRefreshTest011 stop";
}

} // namespace OHOS::Ace
