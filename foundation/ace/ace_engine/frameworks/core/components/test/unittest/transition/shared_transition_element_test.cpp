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

#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/curve_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/drop_filter/drop_filter_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/page/page_element.h"
#include "core/components/positioned/positioned_element.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/overlay_mock.h"
#include "core/components/test/unittest/mock/render_mock.h"
#include "core/components/test/unittest/mock/transform_mock.h"
#include "core/components/test/unittest/mock/transition_mock.h"
#include "core/components/test/unittest/mock/window_mock.h"
#include "core/components/transition/transition_component.h"
#include "core/components/tween/tween_element.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

namespace {

constexpr float TRANSITION_EPSILON = 0.01f;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t FRAME_TIME_IN_MILLISECOND = 200;
constexpr int32_t TEST_SURFACE_WIDTH = 1080;
constexpr int32_t TEST_SURFACE_HEIGHT = 1920;
RefPtr<MockRenderDropFilter> g_dropFilter;
int32_t g_pageId = 0; // for alloc page id.
int32_t g_nodeId = 0; // for alloc node id.
double g_componentWidth = 0.0;
double g_componentHeight = 0.0;
using CreatePushComponent = std::function<RefPtr<Component>(void)>;
using ConfigPushTransition = std::function<void(const RefPtr<PageTransitionComponent>&)>;
CreatePushComponent g_createPushComponent;
ConfigPushTransition g_configPushTransition;
SharedTransitionEffectType g_effectType = SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE;
const char SHARE_ID_EXCHANGE[] = "share_id_exchange";
const char SHARE_ID_EXCHANGE_OTHER[] = "share_id_exchange_other";
const char SHARE_ID_STATIC[] = "share_id_static";

RefPtr<RenderBoxBase> GetSharedSuitFromPositioned(RefPtr<Element> element)
{
    auto positioned = AceType::DynamicCast<PositionedElement>(element);
    if (!positioned) {
        LOGE("Get Shared Suit failed. positioned is null.");
        return nullptr;
    }
    auto tweenElement = AceType::DynamicCast<TweenElement>(positioned->GetFirstChild());
    if (!tweenElement) {
        LOGE("Get Shared Suit failed. tween element is null.");
        return nullptr;
    }
    return AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
}

} // namespace

class SharedTransitionElementTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp() override
    {
        const auto& info = testing::UnitTest::GetInstance()->current_test_info();
        GTEST_LOG_(INFO) << info->test_case_name() << " SetUpTestCase " << info->name();

        MockOverlayElement::SetMockHook([this](const RefPtr<MockOverlayElement>& overlay) { overlay_ = overlay; });
        SystemProperties::InitDeviceType(DeviceType::TV);
        g_createPushComponent = CreatePushPage;
        g_pageId = 0;
        g_nodeId = 0;
        g_componentWidth = TEST_SURFACE_WIDTH / 2.0;
        g_componentHeight = TEST_SURFACE_HEIGHT / 2.0;
        g_effectType = SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE;
    }

    void TearDown() override {}

    void InitEnvAndPushFirstComponent()
    {
        std::unique_ptr<PlatformWindow> platformWindow = TweenTestUtils::CreatePlatformWindow();
        platformWindowRaw_ = reinterpret_cast<MockPlatformWindow*>(platformWindow.get());
        auto window = TweenTestUtils::CreateWindow(std::move(platformWindow));
        auto taskExecutor = AceType::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        RefPtr<Frontend> frontend = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
        context_->SetTimeProvider(
            [this] { return this->platformWindowRaw_->GetCurrentTimestampNano() + NANOSECOND_TO_MILLISECOND * 10; });
        EXPECT_TRUE(g_createPushComponent);
        auto pushPage = AceType::DynamicCast<PageComponent>(g_createPushComponent());
        EXPECT_TRUE(pushPage);
        context_->SetupRootElement();
        EXPECT_TRUE(context_->CanPushPage());
        context_->PushPage(pushPage);
        context_->OnSurfaceChanged(TEST_SURFACE_WIDTH, TEST_SURFACE_HEIGHT);

        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs_);
    }

    void CheckSharedSuitWithUIStandardBecomeSmaller(RefPtr<RenderBoxBase> sharedSuitRender)
    {
        /**
         * @tc.steps: step1. trigger frames to make shared transition work.
         * @tc.expected: step1. check height for shared suit render
         */
        EXPECT_NEAR(TEST_SURFACE_HEIGHT / 2.0, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(800.16, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(622.06, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(532.28, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(491.39, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        // Last frame when push page, render node put back to source page, and height recovers to original height.
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(TEST_SURFACE_HEIGHT / 2.0f, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);
    }

    void CheckSharedSuitWithUIStandardBecomeBigger(RefPtr<RenderBoxBase> sharedSuitRender)
    {
        /**
         * @tc.steps: step1. trigger frames to make shared transition work.
         * @tc.expected: step1. check width for shared suit render
         */
        EXPECT_NEAR(TEST_SURFACE_WIDTH / 4.0f, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(359.90, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(460.08, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(510.59, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(533.59, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);

        // Last frame when pop page, render node put back to source page, and height recovers to original height.
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(TEST_SURFACE_WIDTH / 4.0, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);
    }

    void CheckSharedSuitWithUIStandardDynamicallyShareId1(RefPtr<RenderBoxBase> sharedSuitRender) const
    {
        /**
         * @tc.steps: step1. trigger frames to make shared transition work.
         * @tc.expected: step1. check height for shared suit render
         */
        EXPECT_NEAR(50.0, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(66.65, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(85.20, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(94.55, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(98.81, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        // Last frame when push page, render node put back to source page, and height recovers to original height.
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(50.0, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);
    }

    void CheckSharedSuitWithUIStandardDynamicallyShareId2(RefPtr<RenderBoxBase> sharedSuitRender) const
    {
        /**
         * @tc.steps: step1. trigger frames to make shared transition work.
         * @tc.expected: step1. check height for shared suit render
         */
        EXPECT_NEAR(50.0, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(99.95, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(155.60, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(183.66, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(196.44, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);

        // Last frame when push page, render node put back to source page, and height recovers to original height.
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(50.0, sharedSuitRender->GetHeight(), TRANSITION_EPSILON);
    }

    void CheckSharedSuitWithUIStandardStatic(RefPtr<RenderBoxBase> sharedSuitRender)
    {
        // Second Page width equals quarter of TEST_SURFACE_WIDTH.
        EXPECT_NEAR(TEST_SURFACE_WIDTH / 4.0, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);
        auto width = sharedSuitRender->GetWidth();
        for (int32_t i = 0; i < 5; i++) {
            platformWindowRaw_->TriggerOneFrame();
            EXPECT_NEAR(width, sharedSuitRender->GetWidth(), TRANSITION_EPSILON);
        }
    }

    static RefPtr<Component> CreatePushPage()
    {
        // make content.
        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(g_componentWidth);
        boxComponent->SetHeight(g_componentHeight);
        RefPtr<SharedTransitionComponent> sharedComponent;
        if (g_effectType == SharedTransitionEffectType::SHARED_EFFECT_STATIC) {
            sharedComponent = AceType::MakeRefPtr<SharedTransitionComponent>(
                "TestCase" + std::to_string(g_nodeId++), "shared component", SHARE_ID_STATIC);
            auto effect = SharedTransitionEffect::GetSharedTransitionEffect(g_effectType, SHARE_ID_STATIC);
            sharedComponent->SetEffect(effect);
        } else {
            sharedComponent = AceType::MakeRefPtr<SharedTransitionComponent>(
                "TestCase" + std::to_string(g_nodeId++), "shared component", SHARE_ID_EXCHANGE);
        }
        sharedComponent->SetChild(boxComponent);
        // make page.
        auto transitionComponent = AceType::MakeRefPtr<MockTransitionComponent>();
        transitionComponent->SetContent(sharedComponent);
        if (g_configPushTransition) {
            g_configPushTransition(transitionComponent);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(++g_pageId, "", transitionComponent);
        return AceType::DynamicCast<Component>(pageComponent);
    }

    static RefPtr<Component> CreatePushPageComponent(std::string shareId1, std::string shareId2)
    {
        // make content.
        auto sharedComponent1 = CreateSharedComponent(200.0, 200.0, shareId1);
        auto sharedComponent2 = CreateSharedComponent(100.0, 100.0, shareId2);
        std::list<RefPtr<Component>> children;
        children.emplace_back(sharedComponent1);
        children.emplace_back(sharedComponent2);
        RefPtr<ColumnComponent> flexComponent =
            AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);

        // make page.
        auto transitionComponent = AceType::MakeRefPtr<MockTransitionComponent>();
        transitionComponent->SetContent(flexComponent);
        if (g_configPushTransition) {
            g_configPushTransition(transitionComponent);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(++g_pageId, "", transitionComponent);
        return AceType::DynamicCast<Component>(pageComponent);
    }

    static RefPtr<Component> CreateSharedComponent(double width, double height, std::string shareId)
    {
        // make content.
        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(width);
        boxComponent->SetHeight(height);
        RefPtr<SharedTransitionComponent> sharedComponent;
        if (g_effectType == SharedTransitionEffectType::SHARED_EFFECT_STATIC) {
            sharedComponent = AceType::MakeRefPtr<SharedTransitionComponent>(
                "TestCase" + std::to_string(g_nodeId++), "shared component", shareId);
            auto effect = SharedTransitionEffect::GetSharedTransitionEffect(g_effectType, shareId);
            sharedComponent->SetEffect(effect);
        } else {
            sharedComponent = AceType::MakeRefPtr<SharedTransitionComponent>(
                "TestCase" + std::to_string(g_nodeId++), "shared component", shareId);
        }
        sharedComponent->SetChild(boxComponent);
        return AceType::DynamicCast<Component>(sharedComponent);
    }

    static RefPtr<Component> CreatePushPageWithBlur()
    {
        // make content.
        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(g_componentWidth);
        boxComponent->SetHeight(g_componentHeight);
        auto dropFilterComponent = AceType::MakeRefPtr<DropFilterComponent>(boxComponent, 2.5, 1.5);
        // make page.
        auto transitionComponent = AceType::MakeRefPtr<MockTransitionComponent>();
        transitionComponent->SetContent(dropFilterComponent);
        if (g_configPushTransition) {
            g_configPushTransition(transitionComponent);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(++g_pageId, "", transitionComponent);
        return AceType::DynamicCast<Component>(pageComponent);
    }

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<MockOverlayElement> overlay_;
    uint32_t frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
};

/**
 * @tc.name: SharedTransitionTest001
 * @tc.desc: test shared transition with exchange effect when push page. use default page transition parameters.
 * @tc.type: FUNC
 * @tc.require: AR000DQ2TU
 * @tc.author: zhouzebin
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    g_componentWidth = g_componentWidth / 2.0;   // 1080 / 4 = 270
    g_componentHeight = g_componentHeight / 2.0; // 1920 / 4 = 480
    auto page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);
    /**
     * @tc.steps: step3. trigger frame to make shared transition work.
     * @tc.expected: step3. check passenger in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    auto sharedElement = children.back();
    auto sharedSuitRender = GetSharedSuitFromPositioned(sharedElement);
    CheckSharedSuitWithUIStandardBecomeSmaller(sharedSuitRender);
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: SharedTransitionTest002
 * @tc.desc: test shared transition with exchange effect when pop page. use default page transition parameters.
 * @tc.type: FUNC
 * @tc.require: AR000DQ200
 * @tc.author: zhouzebin
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    g_componentWidth = g_componentWidth / 2.0;   // 1080 / 4 = 270
    g_componentHeight = g_componentHeight / 2.0; // 1920 / 4 = 480
    auto page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);
    platformWindowRaw_->TriggerFrames(6);

    /**
     * @tc.steps: step3. pop page and trigger frame to make shared transition work.
     * @tc.expected: step3. check passenger in overlay
     */
    context_->PopPage();
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    auto sharedElement = children.back();
    auto sharedSuitRender = GetSharedSuitFromPositioned(sharedElement);
    CheckSharedSuitWithUIStandardBecomeBigger(sharedSuitRender);
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: SharedTransitionTest003
 * @tc.desc: test shared transition with static effect when push page. use default page transition parameters.
 * @tc.type: FUNC
 * @tc.require: AR000DQ201
 * @tc.author: zhouzebin
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    g_componentWidth = g_componentWidth / 2.0;   // 1080 / 4 = 270
    g_componentHeight = g_componentHeight / 2.0; // 1920 / 4 = 480
    g_effectType = SharedTransitionEffectType::SHARED_EFFECT_STATIC;
    auto page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);
    /**
     * @tc.steps: step3. trigger frame to make shared transition work.
     * @tc.expected: step3. check passenger in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    auto sharedElement = children.back();
    auto sharedSuitRender = GetSharedSuitFromPositioned(sharedElement);
    CheckSharedSuitWithUIStandardStatic(sharedSuitRender);
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: SharedTransitionTest004
 * @tc.desc: test shared transition with static effect when pop page. (no effect applied)
 * @tc.type: FUNC
 * @tc.require: AR000DQ201
 * @tc.author: zhouzebin
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    g_componentWidth = g_componentWidth / 2.0;   // 1080 / 4 = 270
    g_componentHeight = g_componentHeight / 2.0; // 1920 / 4 = 480
    g_effectType = SharedTransitionEffectType::SHARED_EFFECT_STATIC;
    auto page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);
    platformWindowRaw_->TriggerFrames(6);
    /**
     * @tc.steps: step3. trigger frame to make shared transition work.
     * @tc.expected: step3. no passenger in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    context_->PopPage();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(0UL, children.size()); // Null.
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: OneShotTransitionBlurTest001
 * @tc.desc: test transition with blur component.
 * @tc.type: FUNC
 * @tc.require: AR000DQ205
 * @tc.author: zhouzebin
 */
HWTEST_F(SharedTransitionElementTest, OneShotTransitionBlurTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     * @tc.expected: step1. check sigmaX/Y value in render node.
     */
    g_createPushComponent = CreatePushPageWithBlur;
    MockRenderDropFilter::SetMockHook(
        [](const RefPtr<MockRenderDropFilter>& dropFilter) { g_dropFilter = dropFilter; });
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(g_dropFilter);
    EXPECT_NEAR(g_dropFilter->GetSigmaX(), 2.5, TRANSITION_EPSILON);
    EXPECT_NEAR(g_dropFilter->GetSigmaY(), 1.5, TRANSITION_EPSILON);
}

/**
 * @tc.name: SharedTransitionTest005
 * @tc.desc: test dynamically modify shareId in shared transition with exchange effect when push page.
 *           use default page transition parameters.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAC
 * @tc.author: chenlien
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    g_componentWidth = 50.0;
    g_componentHeight = 50.0;
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    auto page =
        AceType::DynamicCast<PageComponent>(CreatePushPageComponent(SHARE_ID_EXCHANGE_OTHER, SHARE_ID_EXCHANGE));
    context_->PushPage(page);
    /**
     * @tc.steps: step3. trigger frame to make shared transition work.
     * @tc.expected: step3. check passenger in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    auto sharedElement = children.back();
    auto sharedSuitRender = GetSharedSuitFromPositioned(sharedElement);
    CheckSharedSuitWithUIStandardDynamicallyShareId1(sharedSuitRender);
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: SharedTransitionTest006
 * @tc.desc: test dynamically modify shareId in shared transition with exchange effect when push page.
 *           use default page transition parameters.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAC
 * @tc.author: chenlien
 */
HWTEST_F(SharedTransitionElementTest, SharedTransitionTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push first page
     */
    g_componentWidth = 50.0;
    g_componentHeight = 50.0;
    InitEnvAndPushFirstComponent();
    EXPECT_TRUE(!!overlay_);

    /**
     * @tc.steps: step2. push the second page.
     */
    auto page =
        AceType::DynamicCast<PageComponent>(CreatePushPageComponent(SHARE_ID_EXCHANGE, SHARE_ID_EXCHANGE_OTHER));
    context_->PushPage(page);
    /**
     * @tc.steps: step3. trigger frame to make shared transition work.
     * @tc.expected: step3. check passenger in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    auto sharedElement = children.back();
    auto sharedSuitRender = GetSharedSuitFromPositioned(sharedElement);
    CheckSharedSuitWithUIStandardDynamicallyShareId2(sharedSuitRender);
    /**
     * @tc.steps: step4. trigger last frame to make transition done.
     * @tc.expected: step4. no passenger left in overlay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

} // namespace OHOS::Ace