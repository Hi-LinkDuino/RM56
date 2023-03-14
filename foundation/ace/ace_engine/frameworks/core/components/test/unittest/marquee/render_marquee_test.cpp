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

#include <cmath>

#include "gtest/gtest.h"

#include "base/log/log.h"
#include "core/components/marquee/marquee_component.h"
#include "core/components/test/unittest/mock/mock_render_depend.h"
#include "core/pipeline/base/constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using MarqueeEventCallback = std::function<void(const std::string&)>;

constexpr int32_t SURFACE_WIDTH = 1080;
constexpr int32_t SURFACE_HEIGHT = 2244;
constexpr uint64_t MILLI_TO_NANO = 1000000;
constexpr uint64_t VSYNC_FIRST_FRAME = 1;
constexpr int64_t VSYNC_INTERVAL_MILLI = 16;
constexpr int64_t VSYNC_INTERVAL_NANO = VSYNC_INTERVAL_MILLI * MILLI_TO_NANO;
constexpr double DEFAULT_MARQUEE_SCROLL_DELAY = 85.0;
constexpr uint32_t LOOP_COUNT_FOREVER = 100;          // the loop count we treat as loop forever.
constexpr double SCROLL_AMOUNT_EQUAL_EPSILON = 3.001; // the accuracy when check position.

const std::string CUSTOM_TEXT = "marqueeText";
constexpr double CUSTOM_SCROLL_AMOUNT = 1000.0;
constexpr double CUSTOM_SCROLL_AMOUNT_INVALID = -10.0;
constexpr int32_t CUSTOM_LOOP = 10;
constexpr int32_t CUSTOM_LOOP_INVALID = -10;
uint64_t g_runningNano = 0;

class TestMarqueeEventHandler : public MockEventHandler {
public:
    explicit TestMarqueeEventHandler(MarqueeEventCallback callback) : callback_(callback) {}
    ~TestMarqueeEventHandler() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker) override
    {
        if (callback_) {
            callback_(eventMarker.GetData().eventId);
        }
    };

private:
    MarqueeEventCallback callback_;
};

class MockRenderMarquee : public RenderMarquee {
    DECLARE_ACE_TYPE(MockRenderMarquee, RenderMarquee);

public:
    MockRenderMarquee() = default;
    ~MockRenderMarquee() override = default;

    static RefPtr<MockRenderMarquee> g_mockRenderMarquee;

protected:
    TextDirection GetTextDirection(const std::string& text) const override
    {
        return TextDirection::LTR;
    }
};

void PrepareRender(const RefPtr<PipelineContext>& context, const RefPtr<MarqueeComponent>& component)
{
    g_runningNano = 0;
    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", component);
    context->SetTimeProvider([] { return g_runningNano; });
    context->SetupRootElement();
    context->PushPage(pageComponent);
    context->OnVsyncEvent(g_runningNano, 0);
    context->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);
}

void MockVsync(const RefPtr<PipelineContext>& context, uint64_t nanoDuration)
{
    uint64_t runningNano = VSYNC_FIRST_FRAME;
    while (runningNano < nanoDuration) {
        runningNano += VSYNC_INTERVAL_NANO;
        g_runningNano = runningNano;
        context->OnVsyncEvent(runningNano, 0);
    }
    // vsync one more time to trigger bounce, finish callback
    runningNano += VSYNC_INTERVAL_NANO;
    g_runningNano = runningNano;
    context->OnVsyncEvent(runningNano, 0);
}

RefPtr<MarqueeComponent> CreateDefaultMarquee()
{
    RefPtr<MarqueeComponent> component = AceType::MakeRefPtr<MarqueeComponent>();
    component->SetValue(CUSTOM_TEXT);
    component->SetBounceEventId(EventMarker(MARQUEE_BOUNCE));
    component->SetStartEventId(EventMarker(MARQUEE_START));
    component->SetFinishEventId(EventMarker(MARQUEE_FINISH));
    return component;
}

uint64_t CalculateLoopDuration(double scrollAmount)
{
    ACE_DCHECK(scrollAmount > 0);
    ACE_DCHECK(!NearZero(scrollAmount));
    double millisecond = static_cast<double>(SURFACE_WIDTH) * 2.0f * DEFAULT_MARQUEE_SCROLL_DELAY / scrollAmount;
    // Translate raw duration to vsync duration
    uint64_t vsyncCount = static_cast<uint64_t>(std::ceil(millisecond / VSYNC_INTERVAL_MILLI));
    return MILLI_TO_NANO * vsyncCount * VSYNC_INTERVAL_MILLI;
}

// Mock vsync and check marquee whether loop as expected
void CheckLoopCount(
    const RefPtr<PipelineContext>& context, const RefPtr<MarqueeComponent>& component, uint32_t expectedLoop)
{
    uint32_t bounceCount = 0;
    RefPtr<TestMarqueeEventHandler> eventHandler =
        AceType::MakeRefPtr<TestMarqueeEventHandler>([&bounceCount](const std::string& eventId) {
            if (eventId == MARQUEE_BOUNCE) {
                bounceCount++;
            }
        });
    context->RegisterEventHandler(eventHandler);
    PrepareRender(context, component);

    uint64_t loopOnceDuration = CalculateLoopDuration(CUSTOM_SCROLL_AMOUNT);
    MockVsync(context, loopOnceDuration * expectedLoop);
    GTEST_LOG_(INFO) << "CheckLoopCount expectedLoop=" << expectedLoop << ", bounceCount=" << bounceCount;
    EXPECT_TRUE(bounceCount == expectedLoop);
}

double GetTextChildPosition()
{
    if (!MockRenderMarquee::g_mockRenderMarquee) {
        EXPECT_TRUE(false);
        return 0.0;
    }
    const auto& children = MockRenderMarquee::g_mockRenderMarquee->GetChildren();
    if (children.empty()) {
        EXPECT_TRUE(false);
        return 0.0;
    }
    auto textChild = children.front();
    if (!textChild) {
        EXPECT_TRUE(false);
        return 0.0;
    }
    return textChild->GetPosition().GetX();
}

// Mock vsync and check marquee whether move expected distance in one vsync.
// Note: "scrollAmount" should not be zero.
void CheckScrollAmount(const RefPtr<PipelineContext>& context, double scrollAmount)
{
    auto startPosition = GetTextChildPosition();
    MockVsync(context, VSYNC_INTERVAL_NANO);
    uint32_t duration = static_cast<uint32_t>(
        MockRenderMarquee::g_mockRenderMarquee->GetLayoutSize().Width() * DEFAULT_MARQUEE_SCROLL_DELAY / scrollAmount);
    double expectPosition = startPosition * (1.0 - 2.0 * static_cast<double>(VSYNC_INTERVAL_MILLI) / duration);
    auto endPosition = GetTextChildPosition();
    GTEST_LOG_(INFO) << "CheckScrollAmount expectPosition=" << expectPosition << ", endPosition=" << endPosition;
    EXPECT_TRUE(NearEqual(expectPosition, endPosition, SCROLL_AMOUNT_EQUAL_EPSILON));
}

} // namespace

class RenderMarqueeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<Frontend> frontend_;
};

void RenderMarqueeTest::SetUpTestCase() {}
void RenderMarqueeTest::TearDownTestCase() {}

void RenderMarqueeTest::SetUp()
{
    frontend_ = Frontend::CreateDefault();
    context_ = RenderTestUtils::GetMockPipelineContext(frontend_);
}

void RenderMarqueeTest::TearDown()
{
    frontend_ = nullptr;
    context_ = nullptr;
}

RefPtr<MockRenderMarquee> MockRenderMarquee::g_mockRenderMarquee = nullptr;

RefPtr<RenderNode> RenderMarquee::Create()
{
    MockRenderMarquee::g_mockRenderMarquee = AceType::MakeRefPtr<MockRenderMarquee>();
    return MockRenderMarquee::g_mockRenderMarquee;
}

/**
 * @tc.name: RenderMarqueeLoop001
 * @tc.desc: Test marquee could loop continuously by default.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDL
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeLoop001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default loop value.
     * @tc.expected: step1. Default loop value is -1.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    EXPECT_EQ(component->GetLoop(), DEFAULT_MARQUEE_LOOP);

    /**
     * @tc.steps: step2. Mock Vsync and check whether marquee will loop forever.
     * @tc.expected: step2. Marquee will loop forever.
     */
    CheckLoopCount(context_, component, LOOP_COUNT_FOREVER);
}

/**
 * @tc.name: RenderMarqueeLoop002
 * @tc.desc: Test marquee could loop as user defined.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDL
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeLoop002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with custom loop value.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    component->SetLoop(CUSTOM_LOOP);

    /**
     * @tc.steps: step2. Mock Vsync and check whether marquee will loop defined times.
     * @tc.expected: step2. Marquee loop times is same as custom defined.
     */
    CheckLoopCount(context_, component, CUSTOM_LOOP);
}

/**
 * @tc.name: RenderMarqueeLoop003
 * @tc.desc: Test marquee loop value is invalid.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDL
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeLoop003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with invalid custom loop value.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    component->SetLoop(CUSTOM_LOOP_INVALID);

    /**
     * @tc.steps: step2. Mock Vsync and check whether marquee will loop forever.
     * @tc.expected: step2. Marquee will loop forever.
     */
    CheckLoopCount(context_, component, LOOP_COUNT_FOREVER);
}

/**
 * @tc.name: RenderMarqueeScrollAmount001
 * @tc.desc: Test marquee default scroll amount.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDK
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeScrollAmount001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default scroll amount.
     */
    auto component = CreateDefaultMarquee();
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether text child move expected distance.
     * @tc.expected: step2. Text child moved expected distance during this vsync.
     */
    CheckScrollAmount(context_, DEFAULT_MARQUEE_SCROLL_AMOUNT);
}

/**
 * @tc.name: RenderMarqueeScrollAmount002
 * @tc.desc: Test marquee custom scroll amount.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDK
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeScrollAmount002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default scroll amount.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether text child move expected distance.
     * @tc.expected: step2. Text child moved expected distance during this vsync.
     */
    CheckScrollAmount(context_, CUSTOM_SCROLL_AMOUNT);
}

/**
 * @tc.name: RenderMarqueeScrollAmount003
 * @tc.desc: Test marquee use default scroll amount when an invalid value is set.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDK
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeScrollAmount003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default scroll amount.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT_INVALID);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether text child move expected distance.
     * @tc.expected: step2. Text child moved expected distance during this vsync.
     */
    CheckScrollAmount(context_, DEFAULT_MARQUEE_SCROLL_AMOUNT);
}

/**
 * @tc.name: RenderMarqueeDirection001
 * @tc.desc: Test marquee default scroll direction.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDJ
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeDirection001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default direction.
     */
    auto component = CreateDefaultMarquee();
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether text child move left.
     * @tc.expected: step2. Text child moved to left.
     */
    auto startPosition = GetTextChildPosition();
    MockVsync(context_, VSYNC_INTERVAL_NANO);
    auto endPosition = GetTextChildPosition();
    EXPECT_TRUE(endPosition < startPosition);
}

/**
 * @tc.name: RenderMarqueeDirection002
 * @tc.desc: Test marquee scroll direction to right.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDJ
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeDirection002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component with default direction.
     */
    auto component = CreateDefaultMarquee();
    component->SetDirection(MarqueeDirection::RIGHT);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether text child move right.
     * @tc.expected: step2. Text child moved to right.
     */
    auto startPosition = GetTextChildPosition();
    MockVsync(context_, VSYNC_INTERVAL_NANO);
    auto endPosition = GetTextChildPosition();
    EXPECT_TRUE(endPosition > startPosition);
}

/**
 * @tc.name: RenderMarqueeEvent001
 * @tc.desc: Test marquee bounce event.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDM
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component and register bounce handler.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    uint32_t bounceCount = 0;
    RefPtr<TestMarqueeEventHandler> eventHandler =
        AceType::MakeRefPtr<TestMarqueeEventHandler>([&bounceCount](const std::string& eventId) {
            if (eventId == MARQUEE_BOUNCE) {
                bounceCount++;
            }
        });
    context_->RegisterEventHandler(eventHandler);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether received bounce event.
     * @tc.expected: step2. Bounce event received.
     */
    uint64_t loopOnceDuration = CalculateLoopDuration(CUSTOM_SCROLL_AMOUNT);
    MockVsync(context_, loopOnceDuration);
    EXPECT_TRUE(bounceCount == 1);
}

/**
 * @tc.name: RenderMarqueeEvent002
 * @tc.desc: Test marquee finish event.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDM
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeEvent002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component and register finish handler.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    component->SetLoop(CUSTOM_LOOP);
    uint32_t finishCount = 0;
    RefPtr<TestMarqueeEventHandler> eventHandler =
        AceType::MakeRefPtr<TestMarqueeEventHandler>([&finishCount](const std::string& eventId) {
            if (eventId == MARQUEE_FINISH) {
                finishCount++;
            }
        });
    context_->RegisterEventHandler(eventHandler);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync and check whether received finish event.
     * @tc.expected: step2. Finish event received.
     */
    uint64_t loopOnceDuration = CalculateLoopDuration(CUSTOM_SCROLL_AMOUNT);
    MockVsync(context_, loopOnceDuration * CUSTOM_LOOP);
    EXPECT_TRUE(finishCount == 1);
}

/**
 * @tc.name: RenderMarqueeEvent003
 * @tc.desc: Test marquee start event.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDM
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueeEvent003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component and register start handler.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    component->SetLoop(CUSTOM_LOOP);
    uint32_t startCount = 0;
    RefPtr<TestMarqueeEventHandler> eventHandler =
        AceType::MakeRefPtr<TestMarqueeEventHandler>([&startCount](const std::string& eventId) {
            if (eventId == MARQUEE_START) {
                startCount++;
            }
        });
    context_->RegisterEventHandler(eventHandler);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Mock Vsync one time and check whether received start event.
     * @tc.expected: step2. Start event received.
     */
    MockVsync(context_, VSYNC_INTERVAL_NANO);
    EXPECT_TRUE(startCount == 1);
}

/**
 * @tc.name: RenderMarqueePlayControl001
 * @tc.desc: Test marquee animation could stop.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDN
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueePlayControl001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component and prepare render.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Stop marquee and mock vsync, check whether text child still move.
     * @tc.expected: step2. Text child is not moving.
     */
    auto controller = component->GetController();
    ASSERT_TRUE(controller);
    controller->Stop();
    auto startPosition = GetTextChildPosition();
    MockVsync(context_, VSYNC_INTERVAL_NANO);
    auto endPosition = GetTextChildPosition();
    EXPECT_TRUE(NearEqual(startPosition, endPosition));
}

/**
 * @tc.name: RenderMarqueePlayControl002
 * @tc.desc: Test marquee animation could start after a stop.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDN
 * @tc.author: liruokun
 */
HWTEST_F(RenderMarqueeTest, RenderMarqueePlayControl002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a marquee component and prepare render.
     */
    auto component = CreateDefaultMarquee();
    component->SetScrollAmount(CUSTOM_SCROLL_AMOUNT);
    PrepareRender(context_, component);

    /**
     * @tc.steps: step2. Stop marquee and call start, mock vsync, check whether text child still move.
     * @tc.expected: step2. Text child will continue moving.
     */
    auto controller = component->GetController();
    ASSERT_TRUE(controller);
    controller->Stop();
    controller->Start();
    CheckScrollAmount(context_, CUSTOM_SCROLL_AMOUNT);
}

} // namespace OHOS::Ace
