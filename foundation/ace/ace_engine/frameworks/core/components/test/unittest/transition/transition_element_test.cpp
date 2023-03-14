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
#include "base/utils/system_properties.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/curve_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/page/page_element.h"
#include "core/components/page_transition/page_transition_component.h"
#include "core/components/stage/stage_component.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/overlay_mock.h"
#include "core/components/test/unittest/mock/render_mock.h"
#include "core/components/test/unittest/mock/transform_mock.h"
#include "core/components/test/unittest/mock/transition_mock.h"
#include "core/components/test/unittest/mock/window_mock.h"
#include "core/components/tween/tween_element.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t FRAME_TIME_IN_MILLISECOND = 200;
constexpr int32_t TEST_SURFACE_WIDTH = 1080;
constexpr int32_t TEST_SURFACE_HEIGHT = 1920;
constexpr float TRANSITION_EPSILON = 0.01f;
const char SHARE_ID_EXCHANGE[] = "share_id_exchange";
int32_t g_pageId = 0; // for alloc page id.
double g_componentWidth = 0.0;
double g_componentHeight = 0.0;
using CreatePushComponent = std::function<RefPtr<Component>(void)>;
using ConfigPushTransition = std::function<void(const RefPtr<PageTransitionComponent>&)>;
CreatePushComponent g_createPushComponent;
ConfigPushTransition g_configPushTransition;
bool g_isSeparation = false;
RRect g_rrect = RRect();

class TransitionRecord {
public:
    void Init()
    {
        pageInId_ = 0;
        pageOutId_ = 0;
        event_ = TransitionEvent::POP_START;
        eventCount_ = 0;
    }
    int32_t pageInId_ = 0;
    int32_t pageOutId_ = 0;
    TransitionEvent event_ = TransitionEvent::POP_START;
    int32_t eventCount_ = 0;
};

TransitionRecord g_transitionRecord;

} // namespace

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return g_rrect;
}

class TransitionGroup {
public:
    explicit TransitionGroup(const char* name) : name_(name) {}
    // only for test use.
    WeakPtr<MockRenderTransform> transformRenderContent_;
    WeakPtr<MockRenderTransform> transformRenderBackground_;
    WeakPtr<MockRenderDisplay> displayRenderContent_;
    WeakPtr<MockRenderDisplay> displayRenderBackground_;
    WeakPtr<TransitionElement> transitionElementContent_;
    WeakPtr<TransitionElement> transitionElementBackground_;
    WeakPtr<PageTransitionElement> pageTransitionElement_;
    int32_t startCounter_ = 0;
    int32_t stopCounter_ = 0;
    const char* name_ = nullptr;
};

class TransitionElementTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "TransitionElementTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TransitionElementTest TearDownTestCase";
    }

    void SetUp() override
    {
        const auto& info = testing::UnitTest::GetInstance()->current_test_info();
        GTEST_LOG_(INFO) << info->test_case_name() << " SetUpTestCase " << info->name();
        deviceType_ = DeviceType::TV;
        g_isSeparation = false;
        SystemProperties::InitDeviceType(deviceType_);
        SwitchHookToTransition(transitionA_);
        g_createPushComponent = CreatePushPage;
        g_pageId = 0;
        g_componentWidth = TEST_SURFACE_WIDTH / 2.0;
        g_componentHeight = TEST_SURFACE_HEIGHT / 2.0;
        MockOverlayElement::SetMockHook([this](const RefPtr<MockOverlayElement>& overlay) { overlay_ = overlay; });
        g_configPushTransition = nullptr;
        g_transitionRecord.Init();
        // reset transitions every test
        transitionA_ = TransitionGroup("transitionA");
        transitionB_ = TransitionGroup("transitionB");
        transitionC_ = TransitionGroup("transitionC");
    }

    void TearDown() override {}

    void InitEnvAndPushFirstComponent()
    {
        SystemProperties::InitDeviceType(deviceType_);
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
        auto stageElement = context_->GetStageElement();
        ASSERT_TRUE(stageElement);
        auto renderStage = stageElement->GetRenderNode();
        ASSERT_TRUE(renderStage);
        RefPtr<StageComponent> rootStage = AceType::MakeRefPtr<StageComponent>(std::list<RefPtr<Component>>());
        rootStage->SetMainStackSize(MainStackSize::MAX);
        renderStage->Update(rootStage);
        context_->AddPageTransitionListener(PageTransitionRecorder);
        EXPECT_TRUE(context_->CanPushPage());
        context_->PushPage(pushPage);
        context_->OnSurfaceChanged(TEST_SURFACE_WIDTH, TEST_SURFACE_HEIGHT);

        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs_);
    }

    void InitCustomScaleTransition()
    {
        g_configPushTransition = [](const RefPtr<PageTransitionComponent>& transitionComponent) {
            TweenOption InOption;
            auto scaleIn = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 6.0f, Curves::LINEAR);
            InOption.SetTransformFloatAnimation(AnimationType::SCALE, scaleIn);
            InOption.SetDuration(1000);

            TweenOption OutOption;
            auto scaleOut = AceType::MakeRefPtr<CurveAnimation<float>>(60.0f, 10.0f, Curves::LINEAR);
            OutOption.SetTransformFloatAnimation(AnimationType::SCALE, scaleOut);
            transitionComponent->SetContentTransitionOption(InOption, OutOption);
        };
        InitEnvAndPushFirstComponent();
    }

    void InitCustomTranslateTransition()
    {
        g_configPushTransition = [](const RefPtr<PageTransitionComponent>& transitionComponent) {
            TweenOption InOption;
            auto translateIn = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
                DimensionOffset(Dimension(1.0, DimensionUnit::PX), Dimension()),
                DimensionOffset(Dimension(6.0, DimensionUnit::PX), Dimension()), Curves::LINEAR);
            InOption.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateIn);
            InOption.SetDuration(1000);

            TweenOption OutOption;
            auto translateOut = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
                DimensionOffset(Dimension(60.0, DimensionUnit::PX), Dimension()),
                DimensionOffset(Dimension(10.0, DimensionUnit::PX), Dimension()), Curves::LINEAR);
            OutOption.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateOut);
            transitionComponent->SetContentTransitionOption(InOption, OutOption);
        };
        InitEnvAndPushFirstComponent();
    }

    void InitCardClipTransition(TransitionEvent event)
    {
        g_configPushTransition = [event](const RefPtr<PageTransitionComponent>& transitionComponent) {
            TweenOption InOption;
            RRect cardRRect = RRect::MakeRRect(Rect(Offset(100.0, 100.0), Size(100.0, 100.0)), Radius(10.0));
            auto translateAnimationIn = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
                DimensionOffset(Offset(cardRRect.GetRect().Left(), cardRRect.GetRect().Top())),
                DimensionOffset(Offset()), Curves::FRICTION);
            auto heightAnimationIn = AceType::MakeRefPtr<CurveAnimation<float>>(
                cardRRect.GetRect().Height() * 600.0f / cardRRect.GetRect().Width(), 1000.0f, Curves::FRICTION);
            auto scaleAnimationIn = AceType::MakeRefPtr<CurveAnimation<float>>(
                cardRRect.GetRect().Width() / 600.0f, 1.0f, Curves::FRICTION);
            InOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimationIn);
            InOption.SetTranslateAnimations(AnimationType::TRANSLATE, translateAnimationIn);
            InOption.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationIn);

            TweenOption OutOption;
            auto translateAnimationOut = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
                DimensionOffset(Offset(cardRRect.GetRect().Left(), cardRRect.GetRect().Top())), Curves::FRICTION);
            auto heightAnimationOut = AceType::MakeRefPtr<CurveAnimation<float>>(
                1000.0f, cardRRect.GetRect().Height() * 600.0f / cardRRect.GetRect().Width(), Curves::FRICTION);
            auto scaleAnimationOut = AceType::MakeRefPtr<CurveAnimation<float>>(
                1.0f, cardRRect.GetRect().Width() / 600.0f, Curves::FRICTION);
            OutOption.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimationOut);
            OutOption.SetTranslateAnimations(AnimationType::TRANSLATE, translateAnimationOut);
            OutOption.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimationOut);
            InOption.SetDuration(350);

            if (event == TransitionEvent::POP_START) {
                transitionComponent->SetContentTransitionOption(OutOption, InOption);
            } else {
                transitionComponent->SetContentTransitionOption(InOption, OutOption);
            }
        };
    }

    void InitCardOpacityTransition(TransitionEvent event)
    {
        g_configPushTransition = [event](const RefPtr<PageTransitionComponent>& transitionComponent) {
            TweenOption InOption;
            auto opacityInAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
            auto opacityInKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
            auto opacityInKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.286f, 1.0f);
            auto opacityInKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f);
            opacityInKeyframe2->SetCurve(Curves::SHARP);
            opacityInAnimation->AddKeyframe(opacityInKeyframe1);
            opacityInAnimation->AddKeyframe(opacityInKeyframe2);
            opacityInAnimation->AddKeyframe(opacityInKeyframe3);
            InOption.SetOpacityAnimation(opacityInAnimation);
            InOption.SetDuration(350);
            TweenOption OutOption;
            auto opacityOutAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
            auto opacityOutKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f);
            auto opacityOutKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.571f, 1.0f);
            auto opacityOutKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f);
            opacityOutKeyframe3->SetCurve(Curves::SHARP);
            opacityOutAnimation->AddKeyframe(opacityOutKeyframe1);
            opacityOutAnimation->AddKeyframe(opacityOutKeyframe2);
            opacityOutAnimation->AddKeyframe(opacityOutKeyframe3);
            OutOption.SetOpacityAnimation(opacityOutAnimation);
            if (event == TransitionEvent::POP_START) {
                transitionComponent->SetContentTransitionOption(OutOption, InOption);
            } else {
                transitionComponent->SetContentTransitionOption(InOption, OutOption);
            }
        };
    }

    // checked for PushPageTest001
    void CheckCustomScaleTransitionFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check scale value is right and transition is not stopped.
         */
        float scale = 0.0f;
        for (int32_t idx = 1; idx < 5; idx++) {
            LOGD("trigger frame for verify transition. frame idx: %{public}d", idx);
            platformWindowRaw_->TriggerOneFrame();
            EXPECT_EQ(VisibleType::VISIBLE, transitionB_.displayRenderContent_.Upgrade()->GetVisibleType());
            transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
            EXPECT_NEAR(scale, 1.0f * (idx + 1), TRANSITION_EPSILON);
            transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
            EXPECT_NEAR(scale, 10.0f * (6 - idx), TRANSITION_EPSILON);
            EXPECT_EQ(0, transitionA_.stopCounter_);
            EXPECT_EQ(0, transitionB_.stopCounter_);
        }

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final scale value is right, and transition is stopped.
         */
        LOGD("trigger last frame for verify transition.");
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 6.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 10.0f, TRANSITION_EPSILON);
        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
        CheckTouchable(false, transitionA_);
        CheckTouchable(true, transitionB_);
    }

    // checked for PushPageTest002
    void CheckCustomTranslateTransitionFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check scale value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        for (int32_t idx = 1; idx < 5; idx++) {
            LOGD("trigger frame for verify transition. frame idx: %{public}d", idx);
            platformWindowRaw_->TriggerOneFrame();
            transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
            EXPECT_NEAR(offsetX.Value(), 1.0f * (idx + 1), TRANSITION_EPSILON);
            transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
            EXPECT_NEAR(offsetX.Value(), 10.0f * (6 - idx), TRANSITION_EPSILON);

            EXPECT_EQ(0, transitionA_.stopCounter_);
            EXPECT_EQ(0, transitionB_.stopCounter_);
        }

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 6.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 10.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    void CheckPushPageClipFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check clip value is right.
         */
        auto renderNode = transitionB_.transitionElementContent_.Upgrade()->GetContentElement()->GetRenderNode();
        auto clipRender = AceType::DynamicCast<RenderClip>(renderNode);
        EXPECT_TRUE(clipRender);
        platformWindowRaw_->TriggerOneFrame();
        float scale = 0.0f;
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.327342f, TRANSITION_EPSILON);
        Rect clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 677.124, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger the fifth frame to check transition is done
         * @tc.expected: step2. check final clip value is right
         */
        platformWindowRaw_->TriggerFrames(4);
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.957529f, TRANSITION_EPSILON);
        clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 979.614, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);

        /**
         * @tc.steps: step3. trigger last frame to check transition is done
         * @tc.expected: step3. check final clip value is right
         */
        platformWindowRaw_->TriggerFrames(2);
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.0f, TRANSITION_EPSILON);
        clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 1000.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);
    }

    void CheckPushPageCardOpacityFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check opacity value is right.
         */
        auto display = transitionB_.displayRenderContent_.Upgrade();
        EXPECT_TRUE(display);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(display->GetOpacity(), 0.501961, TRANSITION_EPSILON);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(display->GetOpacity(), 1.0, TRANSITION_EPSILON);
    }

    void CheckPopPageClipFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check clip value is right.
         */
        auto renderNode = transitionB_.transitionElementContent_.Upgrade()->GetContentElement()->GetRenderNode();
        auto clipRender = AceType::DynamicCast<RenderClip>(renderNode);
        EXPECT_TRUE(clipRender);

        /**
         * @tc.steps: step2. trigger the first frame to check transition is done
         * @tc.expected: step2. check final clip value is right
         */
        platformWindowRaw_->TriggerOneFrame();
        float scale = 0.0f;
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.789714f, TRANSITION_EPSILON);
        Rect clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 899.063, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger the fifth frame to check transition is done
         * @tc.expected: step2. check final clip value is right
         */
        platformWindowRaw_->TriggerFrames(4);
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.180093f, TRANSITION_EPSILON);
        clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 606.445, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);

        /**
         * @tc.steps: step3. trigger last frame to check transition is done
         * @tc.expected: step3. check final clip value is right
         */
        platformWindowRaw_->TriggerFrames(2);
        clipRect = clipRender->GetClipRect(Offset());
        EXPECT_NEAR(clipRect.Height(), 600.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Width(), 540.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Top(), 0.0, TRANSITION_EPSILON);
        EXPECT_NEAR(clipRect.Left(), 0.0, TRANSITION_EPSILON);
    }

    void CheckPopPageCardOpacityFrames()
    {
        /**
         * @tc.steps: step1. trigger frames to check transition goes right
         * @tc.expected: step1. check opacity value is right.
         */
        auto display = transitionB_.displayRenderContent_.Upgrade();
        EXPECT_TRUE(display);
        platformWindowRaw_->TriggerFrames(4);
        EXPECT_NEAR(display->GetOpacity(), 0.870, TRANSITION_EPSILON);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(display->GetOpacity(), 0.337254, TRANSITION_EPSILON);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_NEAR(display->GetOpacity(), 0.0, TRANSITION_EPSILON);
    }

    // checked for TV transition
    void CheckTvTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 4 frames to check transition goes right
         * @tc.expected: step1. check scale value is right and transition is not stopped.
         */
        float scale = 0.0f;
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(VisibleType::VISIBLE, transitionB_.displayRenderContent_.Upgrade()->GetVisibleType());
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.58f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.04f, TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.85f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.09f, TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.91f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.08f, TRANSITION_EPSILON);

        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 0.98f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.01f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final scale value is right, and transition is stopped.
         */
        LOGD("trigger last frame for verify transition.");
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetScaleSetting(scale);
        EXPECT_NEAR(scale, 1.0f, TRANSITION_EPSILON);
        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    // checked for phone push transition
    void CheckPhonePushTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 2 frames to check transition goes right
         * @tc.expected: step1. check offset value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(VisibleType::VISIBLE, transitionB_.displayRenderContent_.Upgrade()->GetVisibleType());
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 38.84f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), -200.46f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        LOGD("trigger last frame for verify transition.");
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), -216.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    void PushPhoneOrWatchTwoPages()
    {
        /**
         * @tc.steps: step1. init pipeline and build tween component
         */
        InitEnvAndPushFirstComponent();
        /**
         * @tc.steps: step2. push page
         * @tc.expected: step2. check push page success
         */
        platformWindowRaw_->TriggerOneFrame();
        SwitchHookToTransition(transitionB_);
        RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
        EXPECT_TRUE(page);
        EXPECT_TRUE(context_->CanPushPage());
        context_->PushPage(page);

        /**
         * @tc.steps: step3. trigger frames to let transition done
         */
        platformWindowRaw_->TriggerOneFrame();
        GetTransitionGroupFromTransition(transitionB_);
        GetTransitionGroupFromTransition(transitionA_);
        platformWindowRaw_->TriggerFrames(3);
    }

    void PushPhoneTwoPages()
    {
        /**
         * @tc.steps: step1. init device type and push two pages
         */
        deviceType_ = DeviceType::PHONE;
        PushPhoneOrWatchTwoPages();
    }

    void PushWatchTwoPages()
    {
        /**
         * @tc.steps: step1. init device type and push two pages
         */
        deviceType_ = DeviceType::WATCH;
        PushPhoneOrWatchTwoPages();
    }

    // checked for phone pop transition
    void CheckPhonePopTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 2 frames to check transition goes right
         * @tc.expected: step1. check offset value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 292.13f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), -99.15f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        platformWindowRaw_->TriggerOneFrame();
        // page B has been pop and destroyed
        EXPECT_FALSE(transitionB_.transformRenderContent_.Upgrade());
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    // checked for phone pop transition
    void CheckPhonePopSharedTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 2 frames to check transition goes right
         * @tc.expected: step1. check offset value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), -1012, TRANSITION_EPSILON); // -1011.9884336590767
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        platformWindowRaw_->TriggerOneFrame();
        // page B has been pop and destroyed
        EXPECT_FALSE(transitionB_.transformRenderContent_.Upgrade());
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    // checked for watch pop transition
    void CheckWatchPushTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 2 frames to check transition goes right
         * @tc.expected: step1. check offset value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(VisibleType::VISIBLE, transitionB_.displayRenderContent_.Upgrade()->GetVisibleType());
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 77.67f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        LOGD("trigger last frame for verify transition.");
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    // checked for watch push transition
    void CheckWatchPopTransitionFrame()
    {
        /**
         * @tc.steps: step1. trigger 2 frames to check transition goes right
         * @tc.expected: step1. check offset value is right and transition is not stopped.
         */
        Dimension offsetX;
        Dimension offsetY;
        platformWindowRaw_->TriggerOneFrame();
        transitionB_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 738.28f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        /**
         * @tc.steps: step2. trigger last frame to check transition is done
         * @tc.expected: step2. check final offset value is right, and transition is stopped.
         */
        LOGD("trigger last frame for verify transition.");
        platformWindowRaw_->TriggerOneFrame();
        // page B has been pop and destroyed
        EXPECT_FALSE(transitionB_.transformRenderContent_.Upgrade());
        transitionA_.transformRenderContent_.Upgrade()->GetTranslateSetting(offsetX, offsetY);
        EXPECT_NEAR(offsetX.Value(), 0.0f, TRANSITION_EPSILON);
        EXPECT_NEAR(offsetY.Value(), 0.0f, TRANSITION_EPSILON);

        EXPECT_EQ(1, transitionA_.stopCounter_);
        EXPECT_EQ(1, transitionB_.stopCounter_);
    }

    static RefPtr<Component> CreatePushPage()
    {
        LOGD("start create push page.");
        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(TEST_SURFACE_WIDTH / 2.0f);
        boxComponent->SetHeight(TEST_SURFACE_HEIGHT / 2.0f);
        auto transitionComponent = AceType::MakeRefPtr<MockTransitionComponent>();
        g_isSeparation = SystemProperties::GetDeviceType() == DeviceType::TV;
        transitionComponent->SetSeparation(g_isSeparation);
        transitionComponent->SetContent(boxComponent);
        if (g_configPushTransition) {
            g_configPushTransition(transitionComponent);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(++g_pageId, "", transitionComponent);
        LOGD("end create push page.");
        return AceType::DynamicCast<Component>(pageComponent);
    }

    static RefPtr<Component> CreatePushPageWithShared()
    {
        g_pageId++;
        // make content.
        auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
        boxComponent->SetWidth(g_componentWidth);
        boxComponent->SetHeight(g_componentHeight);
        RefPtr<SharedTransitionComponent> sharedComponent;
        sharedComponent = AceType::MakeRefPtr<SharedTransitionComponent>(
            "TestCase" + std::to_string(g_pageId), "shared component", SHARE_ID_EXCHANGE);

        sharedComponent->SetChild(boxComponent);
        // make page.
        auto transitionComponent = AceType::MakeRefPtr<MockTransitionComponent>();
        g_isSeparation = SystemProperties::GetDeviceType() == DeviceType::TV;
        transitionComponent->SetSeparation(g_isSeparation);
        transitionComponent->SetContent(sharedComponent);
        if (g_configPushTransition) {
            g_configPushTransition(transitionComponent);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(g_pageId, "", transitionComponent);
        return AceType::DynamicCast<Component>(pageComponent);
    }

    static void PageTransitionRecorder(
        const TransitionEvent& event, const WeakPtr<PageElement>& pageIn, const WeakPtr<PageElement>& pageOut)
    {
        g_transitionRecord.event_ = event;
        g_transitionRecord.eventCount_++;
        if (pageIn.Upgrade()) {
            g_transitionRecord.pageInId_ = pageIn.Upgrade()->GetPageId();
        }
        if (pageOut.Upgrade()) {
            g_transitionRecord.pageOutId_ = pageOut.Upgrade()->GetPageId();
        }
    }

    static void SwitchHookToTransition(TransitionGroup& transitionGroup)
    {
        MockTransitionElement::SetMockHook([&transitionGroup](const RefPtr<PageTransitionElement>& transitionElement) {
            LOGD("Switch Hook To transition group: %{public}s", transitionGroup.name_);
            transitionGroup.pageTransitionElement_ = transitionElement;
        });
    }

    static void GetTransitionGroupFromTransition(TransitionGroup& transitionGroup)
    {
        EXPECT_TRUE(transitionGroup.pageTransitionElement_.Upgrade());
        auto transitionMock = AceType::DynamicCast<MockTransitionElement>(transitionGroup.pageTransitionElement_);
        EXPECT_TRUE(transitionMock.Upgrade());
        transitionGroup.transitionElementContent_ = transitionMock.Upgrade()->GetContentTransitionElement();
        EXPECT_TRUE(transitionGroup.transitionElementContent_.Upgrade());

        transitionGroup.displayRenderContent_ = AceType::DynamicCast<MockRenderDisplay>(
            transitionGroup.transitionElementContent_.Upgrade()->GetFirstChild()->GetRenderNode());
        EXPECT_TRUE(transitionGroup.displayRenderContent_.Upgrade());
        auto displayChildren = transitionGroup.displayRenderContent_.Upgrade()->GetChildren();
        EXPECT_TRUE(displayChildren.size() == 1);
        transitionGroup.transformRenderContent_ = AceType::DynamicCast<MockRenderTransform>(displayChildren.front());
        EXPECT_TRUE(transitionGroup.transformRenderContent_.Upgrade());

        if (g_isSeparation) {
            transitionGroup.transitionElementBackground_ = transitionMock.Upgrade()->GetBackgroundTransitionElement();
            EXPECT_TRUE(transitionGroup.transitionElementBackground_.Upgrade());
            transitionGroup.displayRenderBackground_ = AceType::DynamicCast<MockRenderDisplay>(
                transitionGroup.transitionElementBackground_.Upgrade()->GetFirstChild()->GetRenderNode());
            EXPECT_TRUE(transitionGroup.displayRenderBackground_.Upgrade());
            displayChildren = transitionGroup.displayRenderBackground_.Upgrade()->GetChildren();
            EXPECT_TRUE(displayChildren.size() == 1);
            transitionGroup.transformRenderBackground_ =
                AceType::DynamicCast<MockRenderTransform>(displayChildren.front());
            EXPECT_TRUE(transitionGroup.transformRenderBackground_.Upgrade());
        }

        auto controller = transitionMock.Upgrade()->GetTransitionController();
        EXPECT_TRUE(controller);
        LOGI("add start / stop listener for transition's animator.");
        controller->AddStartListener([&transitionGroup]() {
            auto transition = transitionGroup.pageTransitionElement_.Upgrade();
            EXPECT_TRUE(transition);
            LOGD("transition start. transition name: %{public}s", transitionGroup.name_);
            transitionGroup.startCounter_++;
        });
        controller->AddStopListener([&transitionGroup]() {
            auto transition = transitionGroup.pageTransitionElement_.Upgrade();
            EXPECT_TRUE(transition);
            LOGD("transition stop. transition name: %{public}s", transitionGroup.name_);
            transitionGroup.stopCounter_++;
        });
    }

    static void CheckTouchable(bool touchable, const TransitionGroup& transition)
    {
        EXPECT_EQ(touchable, transition.transformRenderContent_.Upgrade()->GetTouchable());
        EXPECT_EQ(touchable, transition.transformRenderBackground_.Upgrade()->GetTouchable());
    }

protected:
    DeviceType deviceType_ { DeviceType::TV };
    RefPtr<PipelineContext> context_;
    uint32_t frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
    RefPtr<MockOverlayElement> overlay_;
    TransitionGroup transitionA_ { TransitionGroup("transitionA") };
    TransitionGroup transitionB_ { TransitionGroup("transitionB") };
    TransitionGroup transitionC_ { TransitionGroup("transitionC") };
};

/**
 * @tc.name: PushPageTest001
 * @tc.desc: test scale animation in tween.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PushPageTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest PushTest001";
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    InitCustomScaleTransition();
    EXPECT_EQ(2, g_transitionRecord.eventCount_);
    EXPECT_EQ(TransitionEvent::PUSH_END, g_transitionRecord.event_);
    EXPECT_EQ(1, g_transitionRecord.pageInId_);
    g_transitionRecord.Init();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    EXPECT_TRUE(page);
    EXPECT_TRUE(context_->CanPushPage());
    context_->PushPage(page);
    EXPECT_EQ(0, g_transitionRecord.eventCount_);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     * @tc.expected: step3. check all transition is visible
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, g_transitionRecord.eventCount_);
    EXPECT_EQ(TransitionEvent::PUSH_START, g_transitionRecord.event_);
    EXPECT_EQ(2, g_transitionRecord.pageInId_);
    EXPECT_EQ(1, g_transitionRecord.pageOutId_);
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);
    EXPECT_NE(transitionA_.pageTransitionElement_, transitionB_.pageTransitionElement_);
    EXPECT_TRUE(transitionB_.displayRenderContent_.Upgrade()->GetHidden());
    EXPECT_EQ(VisibleType::VISIBLE, transitionA_.displayRenderContent_.Upgrade()->GetVisibleType());

    /**
     * @tc.steps: step4. push page while transition is going
     * @tc.expected: step4. push page failed
     */
    EXPECT_FALSE(context_->CanPushPage());
    CheckTouchable(false, transitionA_);
    CheckTouchable(false, transitionB_);

    /**
     * @tc.steps: step5. do custom scale transition
     * @tc.expected: step5. check custom scale transition
     */
    CheckCustomScaleTransitionFrames();
}

/**
 * @tc.name: PushPageTest002
 * @tc.desc: test translate animation in tween.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PushPageTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest PushTest002";
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    InitCustomTranslateTransition();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     */
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);

    /**
     * @tc.steps: step4. do custom translate transition
     * @tc.expected: step4. check custom translate transition
     */
    CheckCustomTranslateTransitionFrames();
}

/**
 * @tc.name: TransitionUIStandardTest001
 * @tc.desc: test ui standard for transition
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, TransitionUIStandardTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionUIStandardTest001";
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    g_configPushTransition = nullptr;
    InitEnvAndPushFirstComponent();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    EXPECT_TRUE(page);
    EXPECT_TRUE(context_->CanPushPage());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     * @tc.expected: step3. check all transition is visible
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);
    EXPECT_NE(transitionA_.pageTransitionElement_, transitionB_.pageTransitionElement_);
    LOGD("transition in: content display render %p, transform render %p",
        &(*transitionB_.displayRenderContent_.Upgrade()), &(*transitionB_.transformRenderContent_.Upgrade()));
    LOGD("transition out: content display render %p, transform render %p",
        &(*transitionA_.displayRenderContent_.Upgrade()), &(*transitionA_.transformRenderContent_.Upgrade()));
    EXPECT_TRUE(transitionB_.displayRenderContent_.Upgrade()->GetHidden());
    EXPECT_EQ(VisibleType::VISIBLE, transitionA_.displayRenderContent_.Upgrade()->GetVisibleType());

    /**
     * @tc.steps: step4. push page while transition is going
     * @tc.expected: step4. push page failed
     */
    EXPECT_FALSE(context_->CanPushPage());

    /**
     * @tc.steps: step5. do tv transition
     * @tc.expected: step5. check tv transition
     */
    CheckTvTransitionFrame();
}

/**
 * @tc.name: TransitionUIStandardTest002
 * @tc.desc: test phone push transition
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, TransitionUIStandardTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionUIStandardTest002";
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    deviceType_ = DeviceType::PHONE;
    g_configPushTransition = nullptr;
    InitEnvAndPushFirstComponent();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    EXPECT_TRUE(page);
    EXPECT_TRUE(context_->CanPushPage());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     * @tc.expected: step3. check all transition is visible
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);
    EXPECT_NE(transitionA_.pageTransitionElement_, transitionB_.pageTransitionElement_);
    EXPECT_TRUE(transitionB_.displayRenderContent_.Upgrade()->GetHidden());
    EXPECT_EQ(VisibleType::VISIBLE, transitionA_.displayRenderContent_.Upgrade()->GetVisibleType());

    /**
     * @tc.steps: step4. push page while transition is going
     * @tc.expected: step4. push page failed
     */
    EXPECT_FALSE(context_->CanPushPage());

    /**
     * @tc.steps: step5. do phone transition
     * @tc.expected: step5. check phone transition
     */
    CheckPhonePushTransitionFrame();
}

/**
 * @tc.name: TransitionUIStandardTest003
 * @tc.desc: test watch push transition
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, TransitionUIStandardTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionUIStandardTest003";
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    deviceType_ = DeviceType::WATCH;
    g_configPushTransition = nullptr;
    InitEnvAndPushFirstComponent();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    EXPECT_TRUE(page);
    EXPECT_TRUE(context_->CanPushPage());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     * @tc.expected: step3. check all transition is visible
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);
    EXPECT_NE(transitionA_.pageTransitionElement_, transitionB_.pageTransitionElement_);
    EXPECT_TRUE(transitionB_.displayRenderContent_.Upgrade()->GetHidden());
    EXPECT_EQ(VisibleType::VISIBLE, transitionA_.displayRenderContent_.Upgrade()->GetVisibleType());

    /**
     * @tc.steps: step4. push page while transition is going
     * @tc.expected: step4. push page failed
     */
    EXPECT_FALSE(context_->CanPushPage());

    /**
     * @tc.steps: step5. do watch transition
     * @tc.expected: step5. check watch transition
     */
    CheckWatchPushTransitionFrame();
}

/**
 * @tc.name: TransitionUIStandardTest004
 * @tc.desc: test watch pop transition
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, TransitionUIStandardTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionUIStandardTest004";
    /**
     * @tc.steps: step1. push two pages.
     */
    g_configPushTransition = nullptr;
    PushWatchTwoPages();
    /**
     * @tc.steps: step2. pop page while transition is done
     * @tc.expected: step2. pop page success
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    platformWindowRaw_->TriggerOneFrame();
    CheckWatchPopTransitionFrame();
}

/**
 * @tc.name: TransitionUIStandardTest005
 * @tc.desc: test phone pop transition
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, TransitionUIStandardTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TransitionElementTest TransitionUIStandardTest005";
    /**
     * @tc.steps: step1. init tween option and push two pages
     */
    g_configPushTransition = nullptr;
    PushPhoneTwoPages();

    /**
     * @tc.steps: step2. pop page while transition is done
     * @tc.expected: step2. pop page success
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    platformWindowRaw_->TriggerOneFrame();
    CheckPhonePopTransitionFrame();
}

/**
 * @tc.name: OneShotTransitionTest001
 * @tc.desc: test tv push transition with shared element with custom page transition parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, OneShotTransitionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    g_createPushComponent = CreatePushPageWithShared;
    InitCustomScaleTransition();
    g_transitionRecord.Init();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    EXPECT_TRUE(page);
    EXPECT_TRUE(context_->CanPushPage());
    context_->PushPage(page);
    EXPECT_EQ(0, g_transitionRecord.eventCount_);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     * @tc.expected: step3. check shared element in overlay
     */
    LOGD("trigger frame for stage's transition build and prepare animation");
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.

    /**
     * @tc.steps: step4. do custom scale transition
     * @tc.expected: step4. check custom scale transition and shared element removed from overlay
     */
    CheckCustomScaleTransitionFrames();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: OneShotTransitionTest002
 * @tc.desc: test phone pop transition with shared element with default page transition parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, OneShotTransitionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and push two pages with shared transition, exchange effect
     */
    g_createPushComponent = CreatePushPageWithShared;
    g_configPushTransition = nullptr;
    PushPhoneTwoPages();

    /**
     * @tc.steps: step2. pop page while transition is done
     * @tc.expected: step2. page pops up, transition is the reverse of the entrance, without the normal exit animation
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    platformWindowRaw_->TriggerOneFrame();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(1UL, children.size()); // SharedElement.
    CheckPhonePopSharedTransitionFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: OneShotTransitionTest003
 * @tc.desc: test watch pop transition with shared element with default page transition parameters.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, OneShotTransitionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init tween option and build tween component
     */
    g_createPushComponent = CreatePushPageWithShared;
    g_configPushTransition = nullptr;
    PushWatchTwoPages();

    /**
     * @tc.steps: step4. pop page while transition is done
     * @tc.expected: step4. pop page success
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    auto& children = overlay_->GetChildren();
    EXPECT_EQ(0UL, children.size()); // SharedElement.
    platformWindowRaw_->TriggerOneFrame();
    CheckWatchPopTransitionFrame();
    EXPECT_EQ(0UL, children.size()); // Null.
}

/**
 * @tc.name: PushPageCardTransitionTest001
 * @tc.desc: test card container size and offset animation in card push transition.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PushPageCardTransitionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init card transition option
     */
    g_rrect = RRect::MakeRRect(Rect(Offset(100.0, 100.0), Size(100.0, 100.0)), Radius(10.0));
    deviceType_ = DeviceType::PHONE;
    frameTimeMs_ = 50;
    InitCardClipTransition(TransitionEvent::PUSH_START);
    InitEnvAndPushFirstComponent();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     */
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);

    /**
     * @tc.steps: step4. do card transition
     * @tc.expected: step4. check card container animation
     */
    CheckPushPageClipFrames();
    frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
}

/**
 * @tc.name: PushPageCardTransitionTest002
 * @tc.desc: test Opacity animation in card push transition.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PushPageCardTransitionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init card transition option
     */
    g_rrect = RRect::MakeRRect(Rect(Offset(100.0, 100.0), Size(100.0, 100.0)), Radius(10.0));
    deviceType_ = DeviceType::PHONE;
    frameTimeMs_ = 50;
    InitCardOpacityTransition(TransitionEvent::PUSH_START);
    InitEnvAndPushFirstComponent();

    /**
     * @tc.steps: step2. push page
     * @tc.expected: step2. check push page success
     */
    platformWindowRaw_->TriggerOneFrame();
    SwitchHookToTransition(transitionB_);
    RefPtr<PageComponent> page = AceType::DynamicCast<PageComponent>(g_createPushComponent());
    context_->PushPage(page);

    /**
     * @tc.steps: step3. trigger first frame to let transition go
     */
    platformWindowRaw_->TriggerOneFrame();
    GetTransitionGroupFromTransition(transitionB_);
    GetTransitionGroupFromTransition(transitionA_);

    /**
     * @tc.steps: step4. do card transition
     * @tc.expected: step4. check opacity animation
     */
    CheckPushPageCardOpacityFrames();
    frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
}

/**
 * @tc.name: PopPageCardTransitionTest001
 * @tc.desc: test clip size and offset animation in card pop transition.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PopPageCardTransitionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init card transition and push two pages
     */
    g_rrect = RRect::MakeRRect(Rect(Offset(100.0, 100.0), Size(100.0, 100.0)), Radius(10.0));
    frameTimeMs_ = 50;
    deviceType_ = DeviceType::PHONE;
    InitCardClipTransition(TransitionEvent::POP_START);
    PushPhoneTwoPages();
    platformWindowRaw_->TriggerFrames(5);

    /**
     * @tc.steps: step2. pop page while transition is done
     * @tc.expected: step2. pop page success
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step3. do card transition
     * @tc.expected: step3. check clip animation
     */
    CheckPopPageClipFrames();
    frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
}

/**
 * @tc.name: PopPageCardTransitionTest002
 * @tc.desc: test opacity animation in card pop transition.
 * @tc.type: FUNC
 */
HWTEST_F(TransitionElementTest, PopPageCardTransitionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init card transition and push two pages
     */
    g_rrect = RRect::MakeRRect(Rect(Offset(100.0, 100.0), Size(100.0, 100.0)), Radius(10.0));
    frameTimeMs_ = 50;
    deviceType_ = DeviceType::PHONE;
    InitCardOpacityTransition(TransitionEvent::POP_START);
    PushPhoneTwoPages();
    platformWindowRaw_->TriggerFrames(5);
    /**
     * @tc.steps: step2. pop page while transition is done
     * @tc.expected: step2. pop page success
     */
    EXPECT_TRUE(context_->CanPopPage());
    context_->PopPage();
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step3. do card transition
     * @tc.expected: step3. check opacity animation
     */
    CheckPopPageCardOpacityFrames();
    frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
}

} // namespace OHOS::Ace
