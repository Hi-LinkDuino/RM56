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

#include "base/log/log.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/mock_render_depend.h"
#include "core/components_v2/swiper/swiper_element.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using SwiperEventCallback = std::function<void(const std::string&, const std::string&)>;
using UpdateSwiperCallback = std::function<void(const RefPtr<SwiperComponent>&)>;
const UpdateSwiperCallback EMPTY_CALLBAK;

constexpr int32_t SURFACE_WIDTH = 1080;
constexpr int32_t SURFACE_HEIGHT = 2244;

constexpr int64_t SEC_TO_NANOSEC = 1000000000;
constexpr int64_t MILLISEC_TO_MICROSEC = 1000;
constexpr int64_t VSYNC_INTERVAL_MILLISEC = 16;
constexpr int64_t VSYNC_INTERVAL_MICROSEC = VSYNC_INTERVAL_MILLISEC * MILLISEC_TO_MICROSEC;

constexpr uint32_t SWIPER_CHILD_COUNT = 5;
constexpr int32_t FIRST_CHILD_INDEX = 0;
constexpr int32_t SECOND_CHILD_INDEX = FIRST_CHILD_INDEX + 1;
constexpr int32_t LAST_CHILD_INDEX = SWIPER_CHILD_COUNT - 1;
constexpr int32_t PENULTIMATE_CHILD_INDEX = LAST_CHILD_INDEX - 1;
constexpr int64_t SWIPETO_ANIMATION_TIME = 500;
constexpr int64_t SPRING_ANIMATION_TIME = 2000;

constexpr double SWIPER_WIDTH = 1080;
constexpr double SWIPER_HEIGHT = 1920;
constexpr double DRAG_OFFSET_MIN = 0.0;
constexpr double DRAG_OFFSET_MAX = 1.0;
constexpr double DRAG_OFFSET_MAX_NEGATIVE = -1.0;
constexpr double DRAG_OFFSET_SWITCH_DP = 14.0;

const TouchEvent MOCK_DOWN_TOUCH_EVENT { 10, 540, 200, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT { 10, 540, 200, TouchType::MOVE };
const TouchEvent MOCK_UP_TOUCH_EVENT { 10, 540, 200, TouchType::UP };
enum class DragDirection {
    LEFT,
    RIGHT,
};

int64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec);
}

class TestSwiperEventHander : public MockEventHandler {
public:
    explicit TestSwiperEventHander(SwiperEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestSwiperEventHander() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    };

private:
    SwiperEventCallback eventCallback_;
};

} // namespace

// mock
namespace V2 {

void SwiperElement::PerformBuild() {}

bool SwiperElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

RefPtr<RenderNode> SwiperElement::CreateRenderNode()
{
    return nullptr;
}

void SwiperElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild) {}

RefPtr<Element> SwiperElement::OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component)
{
    return nullptr;
}

RefPtr<Component> SwiperElement::OnMakeEmptyComponent()
{
    return nullptr;
}

void SwiperElement::OnDataSourceUpdated(size_t startIndex) {}

void SwiperElement::OnPostFlush() {}

} // namespace V2

size_t V2::LazyForEachComponent::TotalCount()
{
    return 0;
}

RefPtr<Component> V2::LazyForEachComponent::GetChildByIndex(size_t index)
{
    return nullptr;
}

size_t V2::ElementProxyHost::GetReloadedCheckNum()
{
    return 0;
}

class SwiperComponentTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SwiperComponentTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "SwiperComponentTest TearDownTestCase";
    }

    void SetUp()
    {
        auto platformWindow = PlatformWindow::Create(nullptr);
        auto window = std::make_unique<Window>(std::move(platformWindow));
        auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        frontend_ = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend_, 0);

        swiperChildren_.clear();
        hookRenderBox = [this](const RefPtr<RenderBox>& box) {
            box->Attach(context_);
            swiperChildren_.push_back(box);
        };
    }
    void TearDown() {}

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<Frontend> frontend_;
    std::vector<RefPtr<RenderNode>> swiperChildren_;
    void CreateAndRenderSwiper(UpdateSwiperCallback callback);
    void CreateAndRenderSwiper(uint32_t childCount, UpdateSwiperCallback updateSwiper);
    void DragSwiper(const DragDirection& dragDirection);
    void WaitAndMockVsync(int64_t waitFor);
};

class SwiperIndicatorTest : public RenderSwiper {
public:
    explicit SwiperIndicatorTest(const RefPtr<PipelineContext>& context)
    {
        std::list<RefPtr<Component>> componentChildren;
        for (uint32_t i = 0; i < 10; i++) {
            componentChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
        }

        RefPtr<SwiperComponent> swiperComponent = AceType::MakeRefPtr<SwiperComponent>(componentChildren);
        auto themeManager = AceType::MakeRefPtr<ThemeManager>();
        auto theme = themeManager->GetTheme<SwiperIndicatorTheme>();
        if (theme) {
            auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
            indicator->InitStyle(theme);
            indicator->SetSize(Dimension(6.0));
            indicator->SetSelectedSize(Dimension(12.0));
            indicator->SetPressSize(Dimension(8.0));
            indicator->SetHoverSize(Dimension(12.0));
            indicator->SetHotZoneSize(Dimension(32.0));
            indicator->SetIndicatorPointPadding(Dimension(8.0));
            indicator->SetPressPointPadding(Dimension(10.0));
            indicator->SetStartEndPadding(Dimension(13.0));
            indicator->SetPressPadding(Dimension(12.0));
            swiperComponent->SetIndicator(indicator);
        }
        Attach(context);
        Update(swiperComponent);
        swiperWidth_ = SWIPER_WIDTH;
        swiperHeight_ = SWIPER_HEIGHT;
        scale_ = 1.0;
    }

    static RefPtr<SwiperIndicatorTest> CreateIndicatorTest(const RefPtr<PipelineContext>& context)
    {
        return AceType::MakeRefPtr<SwiperIndicatorTest>(context);
    }

    virtual void UpdateIndicator()
    {
        return;
    }

    int32_t GetTargetIndex() const
    {
        return targetIndex_;
    }

    double GetIndicatorHeadOffset() const
    {
        return indicatorHeadOffset_;
    }

    double GetIndicatorTailOffset() const
    {
        return indicatorTailOffset_;
    }

    double GetIndicatorPointOffset() const
    {
        return indicatorPointOffset_;
    }

    double GetIndicatorAnimationDirect() const
    {
        return animationDirect_;
    }

    void SetCurrentIndex(int32_t index)
    {
        currentIndex_ = index;
    }

    bool GetQuickTrunItem() const
    {
        return quickTrunItem_;
    }

    SwiperIndicatorData& GetSwiperIndicatorData()
    {
        return swiperIndicatorData_;
    }
};

void SwiperComponentTest::CreateAndRenderSwiper(uint32_t childCount, UpdateSwiperCallback updateSwiper)
{
    std::list<RefPtr<Component>> componentChildren;
    for (uint32_t i = 0; i < childCount; i++) {
        componentChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
    }
    RefPtr<SwiperComponent> swiperComponent = AceType::MakeRefPtr<SwiperComponent>(componentChildren);

    if (updateSwiper) {
        updateSwiper(swiperComponent);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", swiperComponent);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);
}

void SwiperComponentTest::CreateAndRenderSwiper(UpdateSwiperCallback updateSwiper)
{
    CreateAndRenderSwiper(SWIPER_CHILD_COUNT, updateSwiper);
}

void SwiperComponentTest::WaitAndMockVsync(int64_t waitFor)
{
    int64_t runningTime = 0;
    do {
        runningTime += VSYNC_INTERVAL_MILLISEC;
        context_->OnVsyncEvent(GetTickCount(), 0);
        usleep(VSYNC_INTERVAL_MICROSEC);
    } while (runningTime < waitFor);
    context_->OnVsyncEvent(GetTickCount(), 0);
}

void SwiperComponentTest::DragSwiper(const DragDirection& dragDirection)
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    TouchEvent touchPoint = MOCK_MOVE_TOUCH_EVENT;
    double dragOffset = SWIPER_WIDTH * 0.3; // 0.3 mean darg 30% of the swiper width
    for (int i = 0; i < 3; i++) {
        context_->OnTouchEvent(touchPoint);
        if (dragDirection == DragDirection::LEFT) {
            touchPoint.x -= dragOffset;
        } else if (dragDirection == DragDirection::RIGHT) {
            touchPoint.x += dragOffset;
        }
    }
    TouchEvent touchEndPoint = MOCK_UP_TOUCH_EVENT;
    touchEndPoint.x = touchPoint.x;
    context_->OnTouchEvent(touchEndPoint);
}

/**
 * @tc.name: RenderSwiper001
 * @tc.desc: Test when the swiper component is listening index changes, Can receive event callback when the index
 * changes.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper001 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, and add event listeners in swiper.
     */
    std::string changeEventId = "swiper_change_id";
    RefPtr<TestSwiperEventHander> eventHander = AceType::MakeRefPtr<TestSwiperEventHander>(
        [this, changeEventId](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("{\"index\":").append(std::to_string(FIRST_CHILD_INDEX + 1).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHander);
    CreateAndRenderSwiper([this, changeEventId](const RefPtr<SwiperComponent>& swiper) {
        swiper->SetChangeEventId(EventMarker(changeEventId));
    });

    /**
     * @tc.steps: step2. Swipe to changes component index.
     * @tc.expected: step2. Event listener the index changes.
     */
    DragSwiper(DragDirection::LEFT);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper001 stop";
}

/**
 * @tc.name: RenderSwiper002
 * @tc.desc: Test the invalid index value to create a swiper component. The component can be successfully created, but
 * the initial swiper display page is the default value.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper002 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing sub-components, the index field of the swiper
     * component is -1.
     * @tc.expected: step1. swiper component displays the first page.
     */
    CreateAndRenderSwiper([this](const RefPtr<SwiperComponent>& swiper) {
        int32_t invalidIndex = -1;
        swiper->SetIndex(invalidIndex);
    });
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper002 stop";
}

/**
 * @tc.name: RenderSwiper003
 * @tc.desc: The test did not set the index property of the swiper component, and the initial display page of swiper was
 * the first page.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper003 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, and the swiper component not contain
     * the index field.
     * @tc.expected: step1. swiper component displays the first page.
     */
    CreateAndRenderSwiper(EMPTY_CALLBAK);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper003 stop";
}

/**
 * @tc.name: RenderSwiper004
 * @tc.desc: Test set the index property of the swiper component. The initial swiper display page is the page specified
 * by index.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper004 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, and the swiper component contains
     * the index field.
     * @tc.expected: step1. swiper component displays the page specified by index.
     */
    uint32_t initIndex = 1;
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, initIndex, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiper->SetIndex(initIndex);
        swiperController = swiper->GetSwiperController();
    });
    swiperController->SwipeTo(initIndex);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    EXPECT_EQ(swiperChildren_[initIndex]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper004 stop";
}

/**
 * @tc.name: RenderSwiper005
 * @tc.desc: Tests that when the component is set to play automatically and the playback duration is not set, the
 * sub-component will be played backward according to the default duration.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper005, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper005 start";

    /**
     * @tc.steps: step1. Build a swiper component tree with sub-components. The swiper component contains the autoplay
     * field and does not include the time interval field.
     */
    CreateAndRenderSwiper([this](const RefPtr<SwiperComponent>& swiper) { swiper->SetAutoPlay(true); });

    /**
     * @tc.steps: step2. Before the default playback duration interval, check the current display component index.
     * @tc.expected: step2. The current display component index has not changed.
     */
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);

    /**
     * @tc.steps: step3. After the default playback duration interval, check the current display component index.
     * @tc.expected: step3. The current display component index is displayed as the second page.
     */
    WaitAndMockVsync(DEFAULT_SWIPER_AUTOPLAY_INTERVAL);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX + 1]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper005 stop";
}

/**
 * @tc.name: RenderSwiper006
 * @tc.desc: Test that when the component is set to play automatically and the playback duration is set, the
 * sub-component will be played backwards according to the set duration.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper006, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper006 start";

    /**
     * @tc.steps: step1. Build a swiper component tree with sub-components. The swiper component contains the autoplay
     * field and does not include the time interval field.
     */
    double autoPlayInterval = 800;
    CreateAndRenderSwiper([this, autoPlayInterval](const RefPtr<SwiperComponent>& swiper) {
        swiper->SetAutoPlay(true);
        swiper->SetAutoPlayInterval(autoPlayInterval);
    });

    /**
     * @tc.steps: step2. Before the specified playback duration interval, check the current display component index.
     * @tc.expected: step2. The current display component index has not changed.
     */
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);

    /**
     * @tc.steps: step3. After the specified playback duration interval, check the current display component index.
     * @tc.expected: step3. The current display component index is displayed as the second page.
     */
    WaitAndMockVsync(autoPlayInterval);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX + 1]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper006 stop";
}

/**
 * @tc.name: RenderSwiper007
 * @tc.desc: Test that when the component support cyclic sliding, it can right slide from the first.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper007 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, the loop field in the swiper is true.
     */
    CreateAndRenderSwiper([this](const RefPtr<SwiperComponent>& swiper) {
        swiper->SetIndex(FIRST_CHILD_INDEX);
        swiper->SetLoop(true);
    });

    /**
     * @tc.steps: step2. Swipe right from the last one to the first.
     * @tc.expected: step2. Swipe success.
     */
    DragSwiper(DragDirection::RIGHT);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[SWIPER_CHILD_COUNT - 1]->GetPosition().GetX(), 0);
    EXPECT_NE(swiperChildren_[0]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper007 stop";
}

/**
 * @tc.name: RenderSwiper008
 * @tc.desc: Test that when the component support cyclic sliding, it can left slide from the last.
 */
HWTEST_F(SwiperComponentTest, RenderSwiper008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper008 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, the loop field in the swiper is true.
     */
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiper->SetIndex(LAST_CHILD_INDEX);
        swiper->SetLoop(true);
        swiperController = swiper->GetSwiperController();
    });

    /**
     * @tc.steps: step2. Swipe left from the first one to the last.
     * @tc.expected: step2. Swipe success.
     */
    swiperController->SwipeTo(LAST_CHILD_INDEX);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    DragSwiper(DragDirection::LEFT);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    EXPECT_NE(swiperChildren_[LAST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper008 stop";
}

/**
 * @tc.name: RenderSwiper009
 * @tc.desc: Test that when the component does not support cyclic sliding, it cannot right slide from the first.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper009 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, the loop field in the swiper is false.
     */
    CreateAndRenderSwiper([this](const RefPtr<SwiperComponent>& swiper) {
        swiper->SetIndex(FIRST_CHILD_INDEX);
        swiper->SetLoop(false);
    });

    /**
     * @tc.steps: step2. Swipe right.
     * @tc.expected: step2. Swipe failed.
     */
    DragSwiper(DragDirection::RIGHT);
    WaitAndMockVsync(SPRING_ANIMATION_TIME);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper009 stop";
}

/**
 * @tc.name: RenderSwiper010
 * @tc.desc: Test that when the component does not support cyclic sliding, it cannot left slide from the last.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper010 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, the loop field in the swiper is false.
     */
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiper->SetIndex(LAST_CHILD_INDEX);
        swiper->SetLoop(false);
        swiperController = swiper->GetSwiperController();
    });

    /**
     * @tc.steps: step2. Swipe left.
     * @tc.expected: step2. Swipe failed.
     */
    swiperController->SwipeTo(LAST_CHILD_INDEX);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    DragSwiper(DragDirection::LEFT);
    WaitAndMockVsync(SPRING_ANIMATION_TIME);
    EXPECT_EQ(swiperChildren_[LAST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper010 stop";
}

/**
 * @tc.name: RenderSwiper011
 * @tc.desc: Test when the number of swiper sub-components is only 1, the component is set to cyclic sliding and will
 * not be cyclic sliding.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper011 start";

    /**
     * @tc.steps: step1. Build a swiper component tree containing subcomponents, the loop field in the swiper is true.
     */
    CreateAndRenderSwiper(1, EMPTY_CALLBAK);

    /**
     * @tc.steps: step2. Swipe right.
     * @tc.expected: step2. Swipe failed.
     */
    DragSwiper(DragDirection::RIGHT);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);

    /**
     * @tc.steps: step3. Swipe left.
     * @tc.expected: step3. Swipe failed.
     */
    DragSwiper(DragDirection::LEFT);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    WaitAndMockVsync(DEFAULT_SWIPER_ANIMATION_DURATION);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper011 stop";
}

/**
 * @tc.name: RenderSwiper012
 * @tc.desc: Test that when the component does not support cyclic sliding, it cannot left slide from the last.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper012, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper012 start";

    /**
     * @tc.steps: step1. Build a swiper component tree with subcomponents, and the swiper component contains the
     * duration field.
     */
    double animationDuration = 1000;
    CreateAndRenderSwiper(
        [this, animationDuration](const RefPtr<SwiperComponent>& swiper) { swiper->SetDuration(animationDuration); });

    /**
     * @tc.steps: step2. Swipe to left, check animation execution results.
     * @tc.expected: step2. Animation execution is incomplete and does not reach the destination.
     */
    DragSwiper(DragDirection::LEFT);
    EXPECT_NE(swiperChildren_[FIRST_CHILD_INDEX + 1]->GetPosition().GetX(), 0);

    /**
     * @tc.steps: step3. After duration which set by step1, check animation execution results.
     * @tc.expected: step3. Animation execution is complete and reach the destination.
     */
    WaitAndMockVsync(animationDuration);
    EXPECT_NE(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX + 1]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper012 stop";
}

/**
 * @tc.name: RenderSwiper013
 * @tc.desc: Test component can be extended to a specified page other than the current page through the swipeTo
 * interface
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper013 start";
    /**
     * @tc.steps: step1. Build a swiper component tree with subcomponents.
     */
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiperController = swiper->GetSwiperController();
    });

    /**
     * @tc.steps: step2. Get swiperController and call the controller's swipeTo interface to jump to the last page.
     */
    swiperController->SwipeTo(LAST_CHILD_INDEX);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    /**
     * @tc.steps: step3. View the current page index.
     * @tc.expected: step3. The current page is on the last page.
     */
    EXPECT_EQ(swiperChildren_[LAST_CHILD_INDEX]->GetPosition().GetX(), 0);
    GTEST_LOG_(INFO) << "SwiperComponentTest RenderSwiper013 stop";
}

/**
 * @tc.name: RenderSwiper014
 * @tc.desc: Test component can be extended to jump to next page circular through the showNext interface.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component tree with subcomponents and set current index to penultimate index.
     */
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiperController = swiper->GetSwiperController();
        swiper->SetIndex(PENULTIMATE_CHILD_INDEX);
    });

    /**
     * @tc.steps: step2. Get swiperController and call the controller's showNext interface to jump to the last page.
     */
    swiperController->SwipeTo(PENULTIMATE_CHILD_INDEX);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    swiperController->ShowNext();
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);

    /**
     * @tc.steps: step3. View the current page index and call the controller's showNext interface to jump to the first
     * page.
     * @tc.expected: step3. The current page is on the last page.
     */
    EXPECT_EQ(swiperChildren_[LAST_CHILD_INDEX]->GetPosition().GetX(), 0);
    swiperController->ShowNext();
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);

    /**
     * @tc.steps: step4. View the current page index.
     * @tc.expected: step4. The current page is on the first page.
     */
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
}

/**
 * @tc.name: RenderSwiper015
 * @tc.desc: Test component can be extended to jump to previous page circular through the showPrevious interface.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, RenderSwiper015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component tree with subcomponents and set current index to second index.
     */
    RefPtr<SwiperController> swiperController;
    CreateAndRenderSwiper([this, &swiperController](const RefPtr<SwiperComponent>& swiper) mutable {
        swiperController = swiper->GetSwiperController();
        swiper->SetIndex(SECOND_CHILD_INDEX);
    });

    /**
     * @tc.steps: step2. Get swiperController and call the controller's showPrevious interface to jump to the first
     * page.
     */
    swiperController->SwipeTo(SECOND_CHILD_INDEX);
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);
    swiperController->ShowPrevious();
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);

    /**
     * @tc.steps: step3. View the current page index and call the controller's showPrevious interface to jump to the
     * last page.
     * @tc.expected: step3. The current page is on the first page.
     */
    EXPECT_EQ(swiperChildren_[FIRST_CHILD_INDEX]->GetPosition().GetX(), 0);
    swiperController->ShowPrevious();
    WaitAndMockVsync(SWIPETO_ANIMATION_TIME);

    /**
     * @tc.steps: step4. View the current page index.
     * @tc.expected: step4. The current page is on the last page.
     */
    EXPECT_EQ(swiperChildren_[LAST_CHILD_INDEX]->GetPosition().GetX(), 0);
}

/**
 * @tc.name: Indicator001
 * @tc.desc: Test animation of indicator move to the next page.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper render, init animation resource.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);

    /**
     * @tc.steps: step2. indicator start moving animation, set index from 0 to 1.
     */
    swiperIndicator->StartIndicatorAnimation(0, 1);

    /**
     * @tc.steps: step3. View the current indicator offset.
     * @tc.expected: step3. check the offsets of indicator, value is in range of [0.0, 1.0).
     */
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset >= DRAG_OFFSET_MIN && headOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(tailOffset >= DRAG_OFFSET_MIN && tailOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(pointOffset >= DRAG_OFFSET_MIN && pointOffset < DRAG_OFFSET_MAX);
    EXPECT_EQ(swiperIndicator->GetIndicatorAnimationDirect(), DRAG_OFFSET_MAX);
    swiperIndicator->StopIndicatorAnimation();
}

/**
 * @tc.name: Indicator002
 * @tc.desc: Test the offset of move indicator.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, init animation resource.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);

    /**
     * @tc.steps: step2. move indicator -10.0dp, set index from 0 to 1.
     */
    swiperIndicator->MoveIndicator(1, -10.0, false);

    /**
     * @tc.steps: step3. View the current indicator offset.
     * @tc.expected: step3. check the offsets of indicator, value is in range of (0.0, 1.0).
     */
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset > DRAG_OFFSET_MIN && headOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(tailOffset > DRAG_OFFSET_MIN && tailOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(pointOffset > DRAG_OFFSET_MIN && pointOffset < DRAG_OFFSET_MAX);
    EXPECT_EQ(swiperIndicator->GetIndicatorAnimationDirect(), DRAG_OFFSET_MAX);
}

/**
 * @tc.name: Indicator003
 * @tc.desc: Test animation of indicator move to the last page.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper render, init animation resource.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);

    /**
     * @tc.steps: step2. indicator start moving animation, set index from 1 to 0.
     */
    swiperIndicator->StartIndicatorAnimation(1, 0);

    /**
     * @tc.steps: step3. View the current indicator offset.
     * @tc.expected: step3. check the offsets of indicator, value is in range of (-1.0, 0.0].
     */
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset <= DRAG_OFFSET_MIN && headOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(tailOffset <= DRAG_OFFSET_MIN && tailOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(pointOffset <= DRAG_OFFSET_MIN && pointOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_EQ(swiperIndicator->GetIndicatorAnimationDirect(), DRAG_OFFSET_MAX_NEGATIVE);
    swiperIndicator->StopIndicatorAnimation();
}

/**
 * @tc.name: Indicator004
 * @tc.desc: Test the offset of move indicator.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, init animation resource.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);

    /**
     * @tc.steps: step2. move indicator 10.0dp, set index from 2 to 1.
     */
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->MoveIndicator(1, 10.0, false);

    /**
     * @tc.steps: step3. View the current indicator offset.
     * @tc.expected: step3. check the offsets of indicator, value is in range of (-1.0, 0.0).
     */
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset < DRAG_OFFSET_MIN && headOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(tailOffset < DRAG_OFFSET_MIN && tailOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(pointOffset < DRAG_OFFSET_MIN && pointOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_EQ(swiperIndicator->GetIndicatorAnimationDirect(), DRAG_OFFSET_MAX_NEGATIVE);
}

/**
 * @tc.name: Indicator005
 * @tc.desc: Test the offset of forward dragging indicator.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    // drag must after zoom completed. this command animation it.
    swiperIndicator->StopZoomAnimation();

    /**
     * @tc.steps: step2. drag indicator less than 4.0dp.
     * @tc.expected: step2. check the offsets of indicator, value is 0.0.
     */
    swiperIndicator->DragIndicator(3.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), 0.0);
    swiperIndicator->DragIndicator(1.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), 0.0);

    /**
     * @tc.steps: step3. drag indicator more than 4.0dp.
     * @tc.expected: step3. check the offsets of indicator, value is in range of (0.0, 1.0).
     */
    swiperIndicator->DragIndicator(8.0);
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset > DRAG_OFFSET_MIN && headOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(tailOffset > DRAG_OFFSET_MIN && tailOffset < DRAG_OFFSET_MAX);
    EXPECT_TRUE(pointOffset > DRAG_OFFSET_MIN && pointOffset < DRAG_OFFSET_MAX);
}

/**
 * @tc.name: Indicator006
 * @tc.desc: Test the offset of reverse dragging indicator at edge position.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->StopZoomAnimation();

    /**
     * @tc.steps: step2. drag indicator less than 4.0dp.
     * @tc.expected: step2. check the offsets of indicator, value is 0.0.
     */
    swiperIndicator->DragIndicator(-3.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), DRAG_OFFSET_MIN);
    swiperIndicator->DragIndicator(-1.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step3. drag indicator more than 4.0dp.
     * @tc.expected: step3. check the offsets of indicator, value is 0.0.
     */
    swiperIndicator->DragIndicator(-8.0);
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_EQ(headOffset, DRAG_OFFSET_MIN);
    EXPECT_EQ(tailOffset, DRAG_OFFSET_MIN);
    EXPECT_EQ(pointOffset, DRAG_OFFSET_MIN);
}

/**
 * @tc.name: Indicator007
 * @tc.desc: Test the offset of reverse dragging indicator at middle position.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator007, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StopZoomAnimation();

    /**
     * @tc.steps: step2. drag indicator less than 4.0dp.
     * @tc.expected: step2. check the offsets of indicator, value is 0.0.
     */
    swiperIndicator->DragIndicator(-3.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), DRAG_OFFSET_MIN);
    swiperIndicator->DragIndicator(-1.0);
    EXPECT_EQ(swiperIndicator->GetIndicatorHeadOffset(), DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step3. drag indicator more than 4.0dp.
     * @tc.expected: step3. check the offsets of indicator, value is in range of (-1.0, 0.0).
     */
    swiperIndicator->DragIndicator(-8.0);
    double headOffset = swiperIndicator->GetIndicatorHeadOffset();
    double pointOffset = swiperIndicator->GetIndicatorPointOffset();
    double tailOffset = swiperIndicator->GetIndicatorTailOffset();
    EXPECT_TRUE(headOffset < DRAG_OFFSET_MIN && headOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(tailOffset < DRAG_OFFSET_MIN && tailOffset > DRAG_OFFSET_MAX_NEGATIVE);
    EXPECT_TRUE(pointOffset < DRAG_OFFSET_MIN && pointOffset > DRAG_OFFSET_MAX_NEGATIVE);
}

/**
 * @tc.name: Indicator008
 * @tc.desc: Test the center position of point is correct.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator008, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render.
     * @tc.expected: step1. check point position of indicator is 0.0.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    Offset center1 = swiperIndicator->GetSwiperIndicatorData().indicatorItemData[2].center;
    EXPECT_EQ(center1.GetY(), DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step2. update indicator layout.
     * @tc.expected: step2. check the point position of indicator is larger than 0.0.
     */
    swiperIndicator->UpdateIndicatorLayout();
    Offset center2 = swiperIndicator->GetSwiperIndicatorData().indicatorItemData[2].center;
    EXPECT_NE(center1.GetY(), center2.GetY());
    EXPECT_TRUE(center2.GetY() > DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step3. update indicator layout.
     * @tc.expected: step3. check the point position of indicator is not changed.
     */
    swiperIndicator->UpdateIndicatorLayout();
    Offset center3 = swiperIndicator->GetSwiperIndicatorData().indicatorItemData[2].center;
    EXPECT_EQ(center2, center3);
}

/**
 * @tc.name: Indicator009
 * @tc.desc: Test the indicator offset can't be changed by drag when it is zoomming..
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator009, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, start zoom animation and set current index.
     * @tc.expected: step1. check indicator head offset is 0.0.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StopZoomAnimation();
    swiperIndicator->StartZoomInAnimation(false);
    double headOffset1 = swiperIndicator->GetIndicatorHeadOffset();
    EXPECT_EQ(headOffset1, DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step2. drag indicator more than 4.0dp.
     * @tc.expected: step2. check indicator head offset is 0.0.
     */
    swiperIndicator->DragIndicator(1.0);
    swiperIndicator->DragIndicator(7.0);
    double headOffset2 = swiperIndicator->GetIndicatorHeadOffset();
    EXPECT_EQ(headOffset2, DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step3. stop zoom animation and drag indicator more than 4.0dp.
     * @tc.expected: step3. check indicator head offset is not 0.0.
     */
    swiperIndicator->StopZoomAnimation();
    swiperIndicator->DragIndicator(1.0);
    swiperIndicator->DragIndicator(7.0);
    double headOffset3 = swiperIndicator->GetIndicatorHeadOffset();
    EXPECT_NE(headOffset3, DRAG_OFFSET_MIN);

    /**
     * @tc.steps: step4. drag end.
     * @tc.expected: step4. check indicator head offset is reset to 0.0.
     */
    swiperIndicator->DragIndicatorEnd();
    double headOffset4 = swiperIndicator->GetIndicatorHeadOffset();
    EXPECT_EQ(headOffset4, DRAG_OFFSET_MIN);
}

/**
 * @tc.name: Indicator010
 * @tc.desc: Test the focus changed when indicator drag offset 14.0dp.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator010, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, and set current index.
     * @tc.expected: step1. check indicator index is 2.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StopZoomAnimation();
    double currentIndex1 = swiperIndicator->GetCurrentIndex();
    EXPECT_EQ(currentIndex1, 2);

    /**
     * @tc.steps: step2. drag indicator more than 14.0dp.
     * @tc.expected: step2. check indicator index is 3.
     */
    swiperIndicator->DragIndicator(1.0);
    swiperIndicator->DragIndicator(DRAG_OFFSET_SWITCH_DP);
    double currentIndex2 = swiperIndicator->GetCurrentIndex();
    EXPECT_EQ(currentIndex2, 3);
}

/**
 * @tc.name: Indicator011
 * @tc.desc: Test the twice click event can't start new animation when indicator is in moving animation.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator011, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, set current index and start indicator animation from 2 to 3.
     * @tc.expected: step1. check indicator target index is 3.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StopZoomAnimation();
    swiperIndicator->StartIndicatorAnimation(2, 3, false);
    double targeIndex1 = swiperIndicator->GetTargetIndex();
    EXPECT_EQ(targeIndex1, 3);

    /**
     * @tc.steps: step2. start indicator animation from 2 to 1.
     * @tc.expected: step2. check indicator target index is 3.
     */
    swiperIndicator->StartIndicatorAnimation(2, 1, false);
    double targeIndex2 = swiperIndicator->GetTargetIndex();
    EXPECT_EQ(targeIndex2, 3);
}

/**
 * @tc.name: Indicator012
 * @tc.desc: Test the indicator moving distance when indicator moving animation cross multiple dots.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator012, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, set current index and start indicator animation from 2 to 3.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StopZoomAnimation();
    swiperIndicator->StartIndicatorAnimation(2, 3, false);
    swiperIndicator->UpdateIndicatorOffset(2, 3, 0.5);
    double offsetHead1 = swiperIndicator->GetIndicatorHeadOffset();
    double offsetTail1 = swiperIndicator->GetIndicatorTailOffset();
    double offsetPoint1 = swiperIndicator->GetIndicatorPointOffset();
    swiperIndicator->StopIndicatorAnimation();

    /**
     * @tc.steps: step2. start indicator animation from 2 to 5.
     * @tc.expected: step2. point offfset is equal, but head and tail offset is multiple relation.
     */
    swiperIndicator->SetCurrentIndex(2);
    swiperIndicator->StartIndicatorAnimation(2, 5, false);
    swiperIndicator->UpdateIndicatorOffset(2, 5, 0.5);
    double offsetHead2 = swiperIndicator->GetIndicatorHeadOffset();
    double offsetTail2 = swiperIndicator->GetIndicatorTailOffset();
    double offsetPoint2 = swiperIndicator->GetIndicatorPointOffset();
    EXPECT_TRUE(NearZero(offsetPoint2 - offsetPoint1));
    EXPECT_TRUE(NearZero(offsetTail2 - 3 * offsetTail1));
    EXPECT_TRUE(NearZero(offsetHead2 - 3 * offsetHead1));
}

/**
 * @tc.name: Indicator013
 * @tc.desc: Test the swiper refuse layout during animation unless forced.
 * @tc.type: FUNC
 */
HWTEST_F(SwiperComponentTest, Indicator013, TestSize.Level0)
{
    /**
     * @tc.steps: step1. Build a swiper render, set current index.
     */
    RefPtr<SwiperIndicatorTest> swiperIndicator = SwiperIndicatorTest::CreateIndicatorTest(context_);
    swiperIndicator->SetCurrentIndex(2);

    /**
     * @tc.steps: step2. Layout swiper without animation.
     */
    swiperIndicator->PerformLayout();

    /**
     * @tc.steps: step3. Start animation to the next index.
     * @tc.expected: step3. Quick trun item tag is false.
     */
    swiperIndicator->ShowNext();
    EXPECT_FALSE(swiperIndicator->GetQuickTrunItem());

    /**
     * @tc.steps: step4. Layout swiper with animation.
     */
    swiperIndicator->PerformLayout();

    /**
     * @tc.steps: step5. Start a new animation to the new index.
     * @tc.expected: step5. Quick trun item tag is true.
     */
    swiperIndicator->ShowNext();
    EXPECT_TRUE(swiperIndicator->GetQuickTrunItem());

    /**
     * @tc.steps: step6. Layout swiper with a new animation.
     * @tc.expected: step6. Quick trun item tag is false.
     */
    swiperIndicator->PerformLayout();
    EXPECT_FALSE(swiperIndicator->GetQuickTrunItem());
}

} // namespace OHOS::Ace
