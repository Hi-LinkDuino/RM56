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
#include "core/animation/card_transition_controller.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/page_transition/page_transition_component.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/render_mock.h"
#include "core/components/test/unittest/mock/transform_mock.h"
#include "core/components/test/unittest/mock/transition_property_mock.h"
#include "core/components/test/unittest/mock/window_mock.h"
#include "core/components/transition/transition_component.h"
#include "core/components/transition/transition_element.h"
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

constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t FRAME_TIME_IN_MILLISECOND = 10;
constexpr int32_t TEST_TRANSITION_WIDTH = 300;
constexpr int32_t TEST_TRANSITION_HEIGHT = 300;
constexpr int32_t TEST_SURFACE_WIDTH = 1080;
constexpr int32_t TEST_SURFACE_HEIGHT = 1920;
constexpr float TEST_EPSILON = 100.0f;
using ConfigTransitionComponent = std::function<void(const RefPtr<TransitionComponent>&)>;
ConfigTransitionComponent g_configTransitionComponent;

void CreateWidthKeyFrame(TweenOption& transitionOption, float end)
{
    auto keyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
    auto keyframe2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
    auto widthAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    widthAnimation->AddKeyframe(keyframe1);
    widthAnimation->AddKeyframe(keyframe2);
    transitionOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);
}

void CreateHeightKeyFrame(TweenOption& transitionOption, float end)
{
    auto keyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
    auto keyframe2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
    auto heightAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    heightAnimation->AddKeyframe(keyframe1);
    heightAnimation->AddKeyframe(keyframe2);
    transitionOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimation);
}

void CreateColorKeyFrame(TweenOption& transitionOption, Color end)
{
    auto keyframe1 = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, Color::TRANSPARENT);
    auto keyframe2 = AceType::MakeRefPtr<Keyframe<Color>>(1.0f, end);
    auto colorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    colorAnimation->AddKeyframe(keyframe1);
    colorAnimation->AddKeyframe(keyframe2);
    transitionOption.SetColorAnimation(colorAnimation);
}

} // namespace

class TransitionPropertyElementTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "TransitionPropertyElementTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TransitionPropertyElementTest TearDownTestCase";
    }

    void SetUp() override
    {
        MockRenderDisplay::SetMockHook(
            [this](const RefPtr<MockRenderDisplay>& displayRender) { displayRender_ = displayRender; });
        MockTransitionPropertyComponent::SetMockHook(
            [this](const RefPtr<TransitionElement>& transitionElement) { transitionElement_ = transitionElement; });
        MockRenderTransform::SetMockHook(
            [this](const RefPtr<MockRenderTransform>& transformRender) { transformRender_ = transformRender; });
    }

    void TearDown() override {}

    void CreateAndBuildTransitionComponent()
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

        boxComponent_ = AceType::MakeRefPtr<BoxComponent>();
        boxComponent_->SetWidth(TEST_TRANSITION_WIDTH);
        boxComponent_->SetHeight(TEST_TRANSITION_HEIGHT);
        boxComponent_->SetColor(Color::WHITE);
        transitionComponent_ = AceType::MakeRefPtr<MockTransitionPropertyComponent>(
            "test_transition_id", "transition component", boxComponent_);
        pageComponent_ = AceType::MakeRefPtr<PageTransitionComponent>();

        pageComponent_->SetContent(transitionComponent_);
        if (g_configTransitionComponent) {
            g_configTransitionComponent(transitionComponent_);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", pageComponent_);

        context_->SetupRootElement();
        context_->PushPage(pageComponent);
        context_->OnSurfaceChanged(TEST_SURFACE_WIDTH, TEST_SURFACE_HEIGHT);

        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs_);
    }

protected:
    RefPtr<PipelineContext> context_;
    uint32_t frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
    RefPtr<MockRenderTransform> transformRender_;
    RefPtr<MockRenderDisplay> displayRender_;
    RefPtr<TransitionElement> transitionElement_;
    RefPtr<TransitionComponent> transitionComponent_;
    RefPtr<PageTransitionComponent> pageComponent_;
    RefPtr<BoxComponent> boxComponent_;
};

/**
 * @tc.name: TransitionPropertyTest001
 * @tc.desc: test width animation in transition.
 * @tc.type: FUNC
 * @tc.require: AR000FL0U3
 * @tc.author: jiachunhui
 */
HWTEST_F(TransitionPropertyElementTest, TransitionPropertyTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionPropertyTest001";
    /**
     * @tc.steps: step1. init transition option and build transition component
     */
    g_configTransitionComponent = [](const RefPtr<TransitionComponent>& transitionComponent) {
        TweenOption transitionOption;
        CreateWidthKeyFrame(transitionOption, 600.0f);
        transitionOption.SetDuration(30);
        transitionOption.SetCurve(Curves::LINEAR);
        transitionOption.SetFillMode(FillMode::FORWARDS);
        transitionComponent->SetTransitionOption(transitionOption);
    };
    CreateAndBuildTransitionComponent();

    /**
     * @tc.steps: step2. trigger frames to let animation go
     * @tc.expected: step2. check the width value;
     */
    platformWindowRaw_->TriggerOneFrame();
    auto tweenElement = AceType::DynamicCast<TweenElement>(transitionElement_->GetFirstChild());
    auto box = AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
    EXPECT_TRUE(box);
    EXPECT_NEAR(box->GetWidth(), 500.0f, TEST_EPSILON);
}

/**
 * @tc.name: TransitionPropertyTest002
 * @tc.desc: test width animation in transition.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TV
 * @tc.author: jiachunhui
 */
HWTEST_F(TransitionPropertyElementTest, TransitionPropertyTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionPropertyTest002";
    /**
     * @tc.steps: step1. init transition option and build transition component
     */
    g_configTransitionComponent = [](const RefPtr<TransitionComponent>& transitionComponent) {
        TweenOption transitionOption;
        CreateWidthKeyFrame(transitionOption, 500.0f);
        transitionOption.SetDuration(15);
        transitionOption.SetCurve(Curves::LINEAR);
        transitionOption.SetFillMode(FillMode::FORWARDS);
        transitionComponent->SetTransitionOption(transitionOption);
    };
    CreateAndBuildTransitionComponent();

    /**
     * @tc.steps: step2. trigger frames to let animation go
     * @tc.expected: step2. check the width end value
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    auto tweenElement = AceType::DynamicCast<TweenElement>(transitionElement_->GetFirstChild());
    auto box = AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
    EXPECT_TRUE(box);
    EXPECT_NEAR(box->GetWidth(), 500.0f, TEST_EPSILON);
}

/**
 * @tc.name: TransitionPropertyTest003
 * @tc.desc: test height animation in transition.
 * @tc.type: FUNC
 * @tc.require: AR000FL0U0
 * @tc.author: jiachunhui
 */
HWTEST_F(TransitionPropertyElementTest, TransitionPropertyTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionPropertyTest003";
    /**
     * @tc.steps: step1. init transition option and build transition component
     */
    g_configTransitionComponent = [](const RefPtr<TransitionComponent>& transitionComponent) {
        TweenOption transitionOption;
        CreateHeightKeyFrame(transitionOption, 600.0f);
        transitionOption.SetDuration(30);
        transitionOption.SetCurve(Curves::LINEAR);
        transitionOption.SetFillMode(FillMode::FORWARDS);
        transitionComponent->SetTransitionOption(transitionOption);
    };
    CreateAndBuildTransitionComponent();

    /**
     * @tc.steps: step2. trigger frames to let animation go
     * @tc.expected: step2. check the height value;
     */
    platformWindowRaw_->TriggerOneFrame();
    auto tweenElement = AceType::DynamicCast<TweenElement>(transitionElement_->GetFirstChild());
    auto box = AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
    EXPECT_TRUE(box);
    EXPECT_NEAR(box->GetHeight(), 400.0f, TEST_EPSILON);
}

/**
 * @tc.name: TransitionPropertyTest004
 * @tc.desc: test height animation in transition.
 * @tc.type: FUNC
 * @tc.require: AR000FL0U1
 * @tc.author: jiachunhui
 */
HWTEST_F(TransitionPropertyElementTest, TransitionPropertyTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionPropertyTest004";
    /**
     * @tc.steps: step1. init transition option and build transition component
     */
    g_configTransitionComponent = [](const RefPtr<TransitionComponent>& transitionComponent) {
        TweenOption transitionOption;
        CreateHeightKeyFrame(transitionOption, 500.0f);
        transitionOption.SetDuration(15);
        transitionOption.SetCurve(Curves::EASE);
        transitionOption.SetFillMode(FillMode::FORWARDS);
        transitionComponent->SetTransitionOption(transitionOption);
    };
    CreateAndBuildTransitionComponent();

    /**
     * @tc.steps: step2. trigger frames to let animation go
     * @tc.expected: step2. check the height end value
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    auto tweenElement = AceType::DynamicCast<TweenElement>(transitionElement_->GetFirstChild());
    auto box = AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
    EXPECT_TRUE(box);
    EXPECT_NEAR(box->GetHeight(), 500.0f, TEST_EPSILON);
}

/**
 * @tc.name: TransitionPropertyTest005
 * @tc.desc: test color animation in transition.
 * @tc.type: FUNC
 * @tc.require: AR000FL0U2
 * @tc.author: jiachunhui
 */
HWTEST_F(TransitionPropertyElementTest, TransitionPropertyTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionPropertyTest005";
    /**
     * @tc.steps: step1. init transition option and build transition component
     */
    g_configTransitionComponent = [](const RefPtr<TransitionComponent>& transitionComponent) {
        TweenOption transitionOption;
        CreateColorKeyFrame(transitionOption, Color::BLACK);
        transitionOption.SetDuration(15);
        transitionOption.SetCurve(Curves::LINEAR);
        transitionOption.SetFillMode(FillMode::FORWARDS);
        transitionComponent->SetTransitionOption(transitionOption);
    };
    CreateAndBuildTransitionComponent();

    /**
     * @tc.steps: step2. trigger frames to let animation go
     * @tc.expected: step2. check the color value
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    auto tweenElement = AceType::DynamicCast<TweenElement>(transitionElement_->GetFirstChild());
    auto box = AceType::DynamicCast<RenderBoxBase>(tweenElement->GetContentRender());
    EXPECT_TRUE(box);
    EXPECT_EQ(box->GetColor(), Color::BLACK);
}

} // namespace OHOS::Ace
