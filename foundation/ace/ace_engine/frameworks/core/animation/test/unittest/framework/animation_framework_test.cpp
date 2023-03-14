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
#include "core/animation/friction_motion.h"
#include "core/animation/scroll_motion.h"
#include "core/animation/spring_motion.h"
#include "core/animation/test/unittest/mock/animation_mock.h"
#include "core/animation/test/unittest/mock/animation_test_utils.h"
#include "core/components/test/json/json_frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

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
constexpr int32_t PICTURE_ANIMATION_COUNT = 3;
constexpr int32_t FRAME_TIME_IN_MILLISECOND = 10;
constexpr int32_t ANIMATION_REPEAT_LOOP = 10;
constexpr uint32_t ANIMATION_DURATION_MULTIPLE = 3;
constexpr uint32_t PICTURE_ANIMATION_DURATION_MULTIPLE = 2;
constexpr uint32_t KEYFRAME_ANIMATION_DURATION_MULTIPLE = 2;
constexpr uint64_t NANO_FRAME_TIME = static_cast<const uint64_t>(1e9 / 60);
constexpr float CUBIC_ERROR_BOUND = 0.01f;

} // namespace

class AnimationFrameworkTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AnimationFrameworkTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AnimationFrameworkTest TearDownTestCase";
    }

    void SetUp() override
    {
        std::unique_ptr<PlatformWindow> platformWindow = AnimationTestUtils::CreatePlatformWindow();
        platformWindowRaw_ = reinterpret_cast<MockPlatformWindow*>(platformWindow.get());
        auto window = AnimationTestUtils::CreateWindow(std::move(platformWindow));
        auto taskExecutor = AceType::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        RefPtr<Frontend> frontend = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
        context_->SetTimeProvider(
            [this] { return this->platformWindowRaw_->GetCurrentTimestampNano() + NANOSECOND_TO_MILLISECOND * 10; });
        flushEventMock_ = AceType::MakeRefPtr<AnimationMock>();

        context_->SetupRootElement();
        context_->OnSurfaceChanged(100, 200);
    }

    void TearDown() override {}

    void TriggerAndCheckBasicPropertyFirstFrame(AnimationOperation operation, float init) const
    {
        /**
         * @tc.steps: step2. trigger one frame to make prepare animation work
         */
        platformWindowRaw_->TriggerOneFrame();
        switch (operation) {
            case AnimationOperation::PLAY:
            case AnimationOperation::REVERSE: {
                EXPECT_EQ(init - 1, flushEventMock_->animationIntValue_);
                EXPECT_TRUE(flushEventMock_->animationStartStatus_);
                EXPECT_FALSE(flushEventMock_->animationStopStatus_);
                break;
            }
            case AnimationOperation::PAUSE: {
                EXPECT_TRUE(flushEventMock_->animationStartStatus_);
                EXPECT_TRUE(flushEventMock_->animationPauseStatus_);
                EXPECT_FALSE(flushEventMock_->animationStopStatus_);
                break;
            }
            case AnimationOperation::FINISH: {
                EXPECT_TRUE(flushEventMock_->animationStopStatus_);
                break;
            }
            case AnimationOperation::CANCEL: {
                EXPECT_FALSE(flushEventMock_->animationIdleStatus_);
                EXPECT_FALSE(flushEventMock_->animationStartStatus_);
                EXPECT_FALSE(flushEventMock_->animationPauseStatus_);
                EXPECT_FALSE(flushEventMock_->animationStopStatus_);
                break;
            }
            default:
                break;
        }
    }

    void InitBasicAnimationDirectionPropertyTest(int32_t duration, int32_t iteration,
        AnimationDirection animationDirection, AnimationOperation operation, FillMode fillMode)
    {
        /**
         * @tc.steps: step1. init animation and simulation controller
         */
        float init = 0.0f;
        float begin = 1.0f;
        float end = 9.0f;

        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * 10);
        flushEventMock_->InitController(context_);
        auto controller = flushEventMock_->GetAnimator();
        // verify curve animation
        flushEventMock_->animationInt_ =
            AceType::MakeRefPtr<CurveAnimation<int>>(static_cast<int>(begin), static_cast<int>(end), Curves::LINEAR);

        // verify keyframe animation
        auto keyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
        auto keyframe2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
        keyframe2->SetCurve(Curves::LINEAR);
        flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        flushEventMock_->keyframeAnimation_->AddKeyframe(keyframe1);
        flushEventMock_->keyframeAnimation_->AddKeyframe(keyframe2);

        flushEventMock_->animationInt_->SetInitValue(static_cast<int>(init));
        flushEventMock_->keyframeAnimation_->SetInitValue(init);

        // set duration / fill-mode / animationDirection / reverse / repeatTimes
        flushEventMock_->animationDuration_ = duration;
        controller->SetFillMode(fillMode);
        controller->SetAnimationDirection(animationDirection);
        flushEventMock_->operation_ = operation;
        flushEventMock_->iteration_ = iteration;

        flushEventMock_->animationIntValue_ = init - 1;
        context_->AddPostFlushListener(flushEventMock_);
        TriggerAndCheckBasicPropertyFirstFrame(operation, init);
    }

    void InitBasicPropertyTest(
        int32_t duration, int32_t iteration, AnimationOperation operation, FillMode fillMode, bool setInit)
    {
        /**
         * @tc.steps: step1. init animation and animator
         */
        float init = 0.0f;
        float begin = 1.0f;
        float end = 9.0f;
        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * 10);
        flushEventMock_->InitController(context_);
        auto controller = flushEventMock_->GetAnimator();
        // verify curve animation
        flushEventMock_->animationInt_ =
            AceType::MakeRefPtr<CurveAnimation<int>>(static_cast<int>(begin), static_cast<int>(end), Curves::LINEAR);

        // verify keyframe animation
        auto keyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
        auto keyframe2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
        keyframe2->SetCurve(Curves::LINEAR);
        flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        flushEventMock_->keyframeAnimation_->AddKeyframe(keyframe1);
        flushEventMock_->keyframeAnimation_->AddKeyframe(keyframe2);

        if (setInit) {
            flushEventMock_->animationInt_->SetInitValue(static_cast<int>(init));
            flushEventMock_->keyframeAnimation_->SetInitValue(init);
        }

        // set duration / fill-mode / reverse / repeatTimes / startDelay
        flushEventMock_->animationDuration_ = duration;
        controller->SetFillMode(fillMode);
        flushEventMock_->operation_ = operation;
        flushEventMock_->iteration_ = iteration;
        if (iteration == 2) {
            flushEventMock_->startDelay_ = 10;
        }

        flushEventMock_->animationIntValue_ = init - 1;
        context_->AddPostFlushListener(flushEventMock_);

        TriggerAndCheckBasicPropertyFirstFrame(operation, init);
    }

    void RunAndCheckBasicPropertyTestInterpolateValueReverse(float endValue)
    {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(5, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(static_cast<int32_t>(endValue), flushEventMock_->animationIntValue_);
        EXPECT_NEAR(endValue, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_TRUE(flushEventMock_->animationStopStatus_);
    }

    void RunAndCheckBasicPropertyTestInterpolateValueForward(float endValue)
    {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(5, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(static_cast<int32_t>(endValue), flushEventMock_->animationIntValue_);
        EXPECT_NEAR(endValue, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_TRUE(flushEventMock_->animationStopStatus_);
    }

    void RunAndCheckBasicPropertyTestInterpolateValue(float endValue)
    {
        if (flushEventMock_->startDelay_ == 10) {
            platformWindowRaw_->TriggerOneFrame();
        }
        // duration equals 0, stop immediately
        if (flushEventMock_->animationDuration_ == 0) {
            platformWindowRaw_->TriggerOneFrame();
            EXPECT_EQ(static_cast<int32_t>(endValue), flushEventMock_->animationIntValue_);
            EXPECT_NEAR(endValue, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
            EXPECT_TRUE(flushEventMock_->animationStopStatus_);
            return;
        }

        switch (flushEventMock_->operation_) {
            case AnimationOperation::REVERSE: {
                RunAndCheckBasicPropertyTestInterpolateValueReverse(endValue);
                break;
            }
            case AnimationOperation::PLAY: {
                RunAndCheckBasicPropertyTestInterpolateValueForward(endValue);
                break;
            }
            default:
                break;
        }
    }

    void RunAndCheckBasicAnimationDirectionPropertyTestReverse()
    {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(5, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(1, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    }

    void RunAndCheckBasicAnimationDirectionPropertyTestForward()
    {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(5, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);

        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(9, flushEventMock_->animationIntValue_);
        EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
        EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    }

    void RunAndCheckBasicAnimationDirectionPropertyTest(float endValue, int iteration)
    {
        if (iteration == 2) {
            RunAndCheckBasicAnimationDirectionPropertyTestForward();

            /**
             * @tc.steps1: repeat animation
             * @tc.expected: check repeat animation value is correct
             */
            RunAndCheckBasicPropertyTestInterpolateValueReverse(endValue);
        } else {
            // iteration is 3
            switch (flushEventMock_->operation_) {
                case AnimationOperation::REVERSE: {
                    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

                    /**
                     * @tc.steps1: repeat animation
                     * @tc.expected: check repeat animation value is correct
                     */
                    RunAndCheckBasicAnimationDirectionPropertyTestForward();

                    /**
                     * @tc.steps2: repeat animation
                     * @tc.expected: check repeat animation value is correct
                     */
                    RunAndCheckBasicPropertyTestInterpolateValueReverse(endValue);
                    break;
                }
                case AnimationOperation::PLAY: {
                    RunAndCheckBasicAnimationDirectionPropertyTestForward();

                    /**
                     * @tc.steps1: repeat animation
                     * @tc.expected: check repeat animation value is correct
                     */
                    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

                    /**
                     * @tc.steps2: repeat animation
                     * @tc.expected: check repeat animation value is correct
                     */
                    RunAndCheckBasicPropertyTestInterpolateValueForward(endValue);
                    break;
                }
                default:
                    break;
            }
        }
    }

    void InitAnimationAndAnimator(int32_t frameTimeMs, int32_t iteration, int32_t startDelay)
    {
        InitAnimationAndAnimator(frameTimeMs, iteration, startDelay, frameTimeMs * ANIMATION_DURATION_MULTIPLE);
    }

    void InitAnimationAndAnimator(int32_t frameTimeMs, int32_t iteration, int32_t startDelay, int32_t duration)
    {
        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
        flushEventMock_->InitController(context_);
        flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
        flushEventMock_->animationDuration_ = duration;
        flushEventMock_->startDelay_ = startDelay;
        flushEventMock_->iteration_ = iteration;
        context_->AddPostFlushListener(flushEventMock_);
    }

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<AnimationMock> flushEventMock_;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
};

/**
 * @tc.name: AnimationFrameworkTest001
 * @tc.desc: Verify the common property animation process for type int
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrameworkTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrameworkTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 30;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 6, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to let animation done
     * @tc.expected: step3. check linear curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(6, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationFrameworkTest002
 * @tc.desc: Verify the common property animation process for type float
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrameworkTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrameworkTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 30;
    // default curve is linear. test it with nullptr.
    flushEventMock_->animationFloat_ = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 6.0f, nullptr);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to let animation done
     * @tc.expected: step3. check linear curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(5.164f, flushEventMock_->animationFloatValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(6.0f, flushEventMock_->animationFloatValue_, CUBIC_ERROR_BOUND);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationFrameworkTest003
 * @tc.desc: when destruct animator, stop it first.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrameworkTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrameworkTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 30;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 6, Curves::LINEAR);
    int32_t previousValue = 0;
    flushEventMock_->animationInt_->AddListener([&previousValue](const int& value) {
        // check different value every callback.
        EXPECT_NE(previousValue, value);
        previousValue = value;
    });
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. free the old controller and write data to the freed memory
     * @tc.expected: step3. no fault happened, and no call back has been called.
     */
    // get the reference for controller and set the value after the controller is destructed.
    Animator& controller = *flushEventMock_->GetAnimator();

    // replace controller with a new one. and the old controller's destructor will be called
    flushEventMock_->InitController(context_);
    // clear all callbacks in destructor
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    // set the freed old controller's memory to test do not call back to old memory.
    controller.SetDuration(0);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationFrameworkTest004
 * @tc.desc: use color curve animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrameworkTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrameworkTest004 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = 30;
    flushEventMock_->animationColor_ =
        AceType::MakeRefPtr<CurveAnimation<Color>>(Color::BLACK, Color::WHITE, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger frames to make it done.
     * @tc.expected: step3. verify the first and last frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(Color(0xff9A9A9A), flushEventMock_->animationColorValue_);
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(Color::WHITE, flushEventMock_->animationColorValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationCurveTest001
 * @tc.desc: Verify the linear Curve
 * @tc.type: FUNC
 * @tc.require: AR000DATTR
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the testValue of time to test LinearCurve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. verify the result of LinearCurve.
     * @tc.expected: step2. the result is right.
     */
    EXPECT_NEAR(testValueFirst, Curves::LINEAR->MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(testValueSecond, Curves::LINEAR->MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(testValueThird, Curves::LINEAR->MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest002
 * @tc.desc: Verify the Sine Curve
 * @tc.type: FUNC
 * @tc.require: AR000DATTR
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the testValue of time to test SineCurve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test SineCurve.
     */
    float pi = 3.14f;
    float targetValueFirst = std::sin(pi * testValueFirst / 2);
    float targetValueSecond = std::sin(pi * testValueSecond / 2);
    float targetValueThird = std::sin(pi * testValueThird / 2);

    /**
     * @tc.steps: step3. verify the result of SineCurve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, Curves::SINE->MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(targetValueSecond, Curves::SINE->MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(targetValueThird, Curves::SINE->MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest003
 * @tc.desc: Verify the Cubic Curve
 * @tc.type: FUNC
 * @tc.require: AR000DATTR
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the testValue of time to test CubicCurve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue and actualValue of time to test CubicCurve.
     */
    float targetValueFirst = 0.30836f;
    float targetValueSecond = 0.68465f;
    float targetValueThird = 0.93772f;

    float actualValueFirst = Curves::EASE_OUT->MoveInternal(testValueFirst);
    float actualValueSecond = Curves::EASE_OUT->MoveInternal(testValueSecond);
    float actualValueThird = Curves::EASE_OUT->MoveInternal(testValueThird);

    /**
     * @tc.steps: step3. verify the result of CubicCurve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, actualValueFirst, CUBIC_ERROR_BOUND);
    EXPECT_NEAR(targetValueSecond, actualValueSecond, CUBIC_ERROR_BOUND);
    EXPECT_NEAR(targetValueThird, actualValueThird, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: AnimationCurveTest004
 * @tc.desc: Verify the decelerate Curve
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UQ
 * @tc.author: jinwuwen
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the testValue of time to test decelerate Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test decelerate Curve.
     */
    float targetValueFirst = 1.0 - std::pow(1.0 - testValueFirst, 2.0);
    float targetValueSecond = 1.0 - std::pow(1.0 - testValueSecond, 2.0);
    float targetValueThird = 1.0 - std::pow(1.0 - testValueThird, 2.0);

    /**
     * @tc.steps: step3. verify the result of decelerate Curve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, Curves::DECELE->MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(targetValueSecond, Curves::DECELE->MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(targetValueThird, Curves::DECELE->MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest005
 * @tc.desc: Verify the Steps Curve with start position
 * @tc.type: FUNC
 * @tc.require: AR000DQ20F
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest005, TestSize.Level1)
{
    int32_t steps = 4;
    float stepValue = 1.0f / steps;
    StepsCurve stepsCurveStart(steps, StepsCurvePosition::START);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test steps Curve.
     */
    float targetValueFirst = stepValue;
    float targetValueSecond = stepValue * 3;
    float targetValueThird = stepValue * 4;

    /**
     * @tc.steps: step3. verify the result of steps Curve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, stepsCurveStart.MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(targetValueSecond, stepsCurveStart.MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(targetValueThird, stepsCurveStart.MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationDomCurveTest001
 * @tc.desc: Verify the anticipate Curve
 * @tc.type: FUNC
 * @tc.require: AR000DSB24
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationDomCurveTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the testValue of time to test anticipate Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test anticipate Curve.
     */
    float targetValueFirst = -0.056f;
    float targetValueSecond = -0.125f;
    float targetValueThird = 0.256f;

    /**
     * @tc.steps: step3. verify the result of anticipate Curve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, Curves::ANTICIPATE->MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(targetValueSecond, Curves::ANTICIPATE->MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(targetValueThird, Curves::ANTICIPATE->MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest007
 * @tc.desc: Verify the Steps Curve with end position
 * @tc.type: FUNC
 * @tc.require: AR000DQ20G
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest007, TestSize.Level1)
{
    int32_t steps = 4;
    float stepValue = 1.0f / steps;
    StepsCurve stepsCurveEnd(steps, StepsCurvePosition::END);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test steps Curve.
     */
    float targetValueFirst = 0;
    float targetValueSecond = stepValue * 2;
    float targetValueThird = stepValue * 3;

    /**
     * @tc.steps: step3. verify the result of steps Curve.
     * @tc.expected: step3. the result is right.
     */
    EXPECT_NEAR(targetValueFirst, stepsCurveEnd.MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(targetValueSecond, stepsCurveEnd.MoveInternal(testValueSecond), FLT_EPSILON);
    EXPECT_NEAR(targetValueThird, stepsCurveEnd.MoveInternal(testValueThird), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest008
 * @tc.desc: Verify the Steps Curve with steps zero
 * @tc.type: FUNC
 * @tc.require: AR000DQ20I
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest008, TestSize.Level1)
{
    StepsCurve stepsCurveStart(0);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.8f;

    /**
     * @tc.steps: step2. verify the result of steps Curve.
     * @tc.expected: step2. the result is right.
     */
    EXPECT_NEAR(1.0f, stepsCurveStart.MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(1.0f, stepsCurveStart.MoveInternal(testValueSecond), FLT_EPSILON);
}

/**
 * @tc.name: AnimationCurveTest009
 * @tc.desc: Verify the Steps Curve with steps zero
 * @tc.type: FUNC
 * @tc.require: AR000DQ20I
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationCurveTest009, TestSize.Level1)
{
    RefPtr<StepsCurve> stepsCurveStart = AceType::MakeRefPtr<StepsCurve>(2);
    ComplementaryCurve complementaryCurve = ComplementaryCurve(stepsCurveStart);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.8f;

    /**
     * @tc.steps: step2. verify the result of steps Curve.
     * @tc.expected: step2. the result is right.
     */
    EXPECT_NEAR(0.5f, complementaryCurve.MoveInternal(testValueFirst), FLT_EPSILON);
    EXPECT_NEAR(0.0f, complementaryCurve.MoveInternal(testValueSecond), FLT_EPSILON);
}

/**
 * @tc.name: AnimationListenableTest001
 * @tc.desc: Verify the whether listen the value of animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGT
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationListenableTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 15;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 2, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     */
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step3. trigger one frame to let animation done
     * @tc.expected: step3. listen the LINEAR curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationListenableTest002
 * @tc.desc: Verify the whether listen the status of animator
 * @tc.type: FUNC
 * @tc.require: AR000DAIGT
 * @tc.author: jiachunhui
 */
HWTEST_F(AnimationFrameworkTest, AnimationListenableTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 15;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 2, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     */
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step3. trigger 3 frames to let animation done
     * @tc.expected: step3. listen the linear curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_TRUE(flushEventMock_->animationStartStatus_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
    EXPECT_EQ(0, flushEventMock_->repeatDoneTimes_);
    EXPECT_EQ(flushEventMock_->animationIntValue_, flushEventMock_->animationIntStopValue_);
}

/**
 * @tc.name: PictureAnimationTest001
 * @tc.desc: Verify the picture animation process for type int
 * @tc.type: FUNC
 * @tc.require: AR000DBTQE
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, PictureAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest PictureAnimationTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    uint32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * PICTURE_ANIMATION_DURATION_MULTIPLE;
    int32_t durationTotal = 0;
    flushEventMock_->pictureInt_ = AceType::MakeRefPtr<PictureAnimation<int>>();
    for (int32_t idx = 0; idx < PICTURE_ANIMATION_COUNT; idx++) {
        flushEventMock_->pictureInt_->AddPicture(1.0f / PICTURE_ANIMATION_COUNT, idx + 10);
        durationTotal += duration;
    }
    flushEventMock_->animationDuration_ = durationTotal;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger frames to see the begin and end for the first picture.
     * @tc.expected: step3. check 1st picture's last time
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(10, flushEventMock_->pictureIntValue_);
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * (frameTimeMs - 1), 1);
    EXPECT_EQ(10, flushEventMock_->pictureIntValue_);

    /**
     * @tc.steps: step4. trigger frames to see the begin and end for the 2nd picture
     * @tc.expected: step4. check 2nd picture's last time
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_EQ(11, flushEventMock_->pictureIntValue_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(11, flushEventMock_->pictureIntValue_);
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * (frameTimeMs - 1), 1);
    EXPECT_EQ(11, flushEventMock_->pictureIntValue_);

    /**
     * @tc.steps: step5. trigger frames to see the last frame
     * @tc.expected: step5. check the last picture's last time
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_EQ(12, flushEventMock_->pictureIntValue_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(12, flushEventMock_->pictureIntValue_);

    /**
     * @tc.steps: step6. in no repeat case, stay in last picture
     * @tc.expected: step6. check stay in last picture
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(12, flushEventMock_->pictureIntValue_);
}

/**
 * @tc.name: PictureAnimationTest002
 * @tc.desc: Verify the picture animation process for type string with repeat forever
 * @tc.type: FUNC
 * @tc.require: AR000DBTQE
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, PictureAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest PictureAnimationTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    uint32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * 1;
    int32_t durationTotal = 0;
    flushEventMock_->pictureString_ = AceType::MakeRefPtr<PictureAnimation<std::string>>();
    for (int32_t idx = 0; idx < PICTURE_ANIMATION_COUNT; idx++) {
        flushEventMock_->pictureString_->AddPicture(1.0f / (PICTURE_ANIMATION_COUNT + 1), std::to_string(idx));
        durationTotal += duration;
    }
    flushEventMock_->pictureString_->AutoScale();
    flushEventMock_->animationDuration_ = durationTotal;
    flushEventMock_->iteration_ = ANIMATION_REPEAT_INFINITE;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. check 1st loop. trigger frames to let animation done
     * @tc.expected: step3. check all the frames in first loop
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ("1", flushEventMock_->pictureStringValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ("2", flushEventMock_->pictureStringValue_);

    /**
     * @tc.steps: step4. run 10 loops and check. trigger 3 frames to let animation done
     * @tc.expected: step4. check all the frames after 10 loops as the first loop
     */
    for (int32_t loop = 0; loop < ANIMATION_REPEAT_LOOP; loop++) {
        platformWindowRaw_->TriggerOneFrame();
        platformWindowRaw_->TriggerOneFrame();
        platformWindowRaw_->TriggerOneFrame();
    }

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ("0", flushEventMock_->pictureStringValue_);
}

/**
 * @tc.name: PictureAnimationTest003
 * @tc.desc: Verify the picture animation process for type int
 * @tc.type: FUNC
 * @tc.require: AR000DBTQE
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, PictureAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest PictureAnimationTest003 start";
    /**
     * @tc.steps: step1. init picture animation
     */
    flushEventMock_->pictureString_ = AceType::MakeRefPtr<PictureAnimation<std::string>>();

    /**
     * @tc.steps: step2. verify adding picture with error duration
     * @tc.expected: step2. return false
     */
    EXPECT_FALSE(flushEventMock_->pictureString_->AddPicture(-1.0f, "invalid duration, negative"));
    EXPECT_FALSE(flushEventMock_->pictureString_->AddPicture(0.0f, "invalid duration, zero"));
    EXPECT_FALSE(flushEventMock_->pictureString_->AddPicture(2.0f, "invalid duration, not normalized"));

    /**
     * @tc.steps: step3. verify adding picture with valid edge
     * @tc.expected: step3. return true
     */
    EXPECT_TRUE(flushEventMock_->pictureString_->AddPicture(1.0f, "valid duration"));
}

/**
 * @tc.name: AnimationRepeatTest001
 * @tc.desc: test repeat animation for repeat 1 time
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    uint32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->iteration_ = 2;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. 1st round trigger 2 frames to let animation done
     * @tc.expected: step3. check linear curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step4. 2nd round. trigger 3 frames to let animation done
     * @tc.expected: step4. check linear curve's value in every frame
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step5. 3rd round. trigger 2 frames.
     * @tc.expected: step5. verify animation stopped
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    EXPECT_EQ(flushEventMock_->iteration_, flushEventMock_->repeatDoneTimes_ + 1);
}

/**
 * @tc.name: AnimationRepeatTest002
 * @tc.desc: test repeat animation for repeat forever
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    uint32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->iteration_ = ANIMATION_REPEAT_INFINITE;
    context_->AddPostFlushListener(flushEventMock_);
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step2. go for 10 loops and check.
     * @tc.expected: step2. check values in every frame.
     */
    for (int32_t loop = 0; loop < ANIMATION_REPEAT_LOOP; loop++) {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(2, flushEventMock_->animationIntValue_);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(3, flushEventMock_->animationIntValue_);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    }
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationRepeatTest003
 * @tc.desc: test repeat animation for skip multi frames
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    uint32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->iteration_ = 3;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. skip 2 loops and check
     * @tc.expected: step3. check the last loop
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * duration * 2, 3);
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step4. check loop ends
     * @tc.expected: step4. get the end value
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationRepeatTest004
 * @tc.desc: test repeat animation for skip all loops
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest004 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->iteration_ = 3;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. skip 2 loops and check
     * @tc.expected: step3. check animation stop
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * (static_cast<int>(duration)) * 4, 3);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    // skip all loops, only last loop trigger repeat callback
    EXPECT_EQ(0, flushEventMock_->repeatDoneTimes_);
}

/**
 * @tc.name: AnimationRepeatTest005
 * @tc.desc: test repeat animation for invalid repeat times
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest005 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->iteration_ = -2;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called. repeat set failed.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);
    EXPECT_FALSE(flushEventMock_->setRepeatSucc_);
}

/**
 * @tc.name: AnimationRepeatTest006
 * @tc.desc: test repeat animation for no repeat times with very large start delay and duration
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest006 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitAnimationAndAnimator(1, 1, INT32_MAX, INT32_MAX);
    platformWindowRaw_->TriggerOneFrame();

    /**
     * @tc.steps: step2. trigger long time to reach start delay's end
     * @tc.expected: step2. check reaches start delay's end
     */
    platformWindowRaw_->TriggerFrame(((uint64_t)(INT32_MAX - 1)) * NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step3. trigger long time to make duration's end
     * @tc.expected: step3. check reaches start duration's end
     */
    platformWindowRaw_->TriggerFrame(((uint64_t)(INT32_MAX - 1)) * NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationRepeatTest007
 * @tc.desc: test repeat animation for no repeat times with very large repeat times
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationRepeatTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationRepeatTest007 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitAnimationAndAnimator(1, INT32_MAX, 1, 1);

    /**
     * @tc.steps: step2. trigger one frame to reach start delay's end
     * @tc.expected: step2.  check reaches start delay's end
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->repeatDoneTimes_);

    /**
     * @tc.steps: step3. trigger very long frame to reach duration's end
     * @tc.expected: step3.  check reaches animation's end
     */
    platformWindowRaw_->TriggerFrame(((uint64_t)(INT32_MAX - 2)) * NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_EQ(2, flushEventMock_->repeatDoneTimes_);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->repeatDoneTimes_ + 1);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationStartDelayTest001
 * @tc.desc: test start delay for animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationStartDelayTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationStartDelayTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->startDelay_ = frameTimeMs;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger start delay and check
     * @tc.expected: step3. check start dely
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * (frameTimeMs - 1), 1);
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step4. go with the left animation
     * @tc.expected: step4. check timestamp at start millisecond
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND, 1);
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step5. check animation at the end
     * @tc.expected: step5. check the end
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationStartDelayTest002
 * @tc.desc: test repeat & start delay animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationStartDelayTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationStartDelayTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->startDelay_ = frameTimeMs;
    flushEventMock_->iteration_ = 2;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger start delay and check
     * @tc.expected: step3. check start dely
     */
    platformWindowRaw_->TriggerFrame(NANOSECOND_TO_MILLISECOND * (frameTimeMs - 1), 1);
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step4. go with repeat loops and check
     * @tc.expected: step4. check timestamp at last millisecond
     */
    for (int32_t loop = 0; loop < 2; loop++) {
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(1, flushEventMock_->animationIntValue_);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(2, flushEventMock_->animationIntValue_);
        platformWindowRaw_->TriggerOneFrame();
        EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    }

    /**
     * @tc.steps: step5. check animation at the end
     * @tc.expected: step5. check the end
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationStartDelayTest003
 * @tc.desc: test repeat & start delay animation with time scale
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationStartDelayTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationStartDelayTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    flushEventMock_->startDelay_ = frameTimeMs;
    auto controller = flushEventMock_->GetAnimator();
    controller->SetDurationScale(2.0f);
    context_->AddPostFlushListener(flushEventMock_);
    /**
     * @tc.steps: step2. trigger start delay and check
     * @tc.expected: step2. check start delay
     */
    platformWindowRaw_->TriggerFrames(2);
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step3. go forward and check
     * @tc.expected: step3. check timestamp at last millisecond
     */
    platformWindowRaw_->TriggerFrames(2);
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    platformWindowRaw_->TriggerFrames(2);
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);
    platformWindowRaw_->TriggerFrames(2);
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step4. check animation at the end
     * @tc.expected: step4. check the end
     */
    platformWindowRaw_->TriggerFrames(2);
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    controller->SetDurationScale(1.0f);
}

/**
 * @tc.name: AnimationPauseTest001
 * @tc.desc: test pause animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPauseTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationPauseTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger frame after resume ahead
     * @tc.expected: step3. check linear curve's value in every frame
     */
    auto controller = flushEventMock_->GetAnimator();
    controller->Resume();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    controller->Pause();

    /**
     * @tc.steps: step4. pause animator
     * @tc.expected: step4. check linear curve's value do not change.
     */
    for (int32_t i = 0; i < 10; i++) {
        platformWindowRaw_->TriggerOneFrame();
        controller->Pause();
        EXPECT_EQ(2, flushEventMock_->animationIntValue_);
    }

    /**
     * @tc.steps: step5. resume animator and trigger last frame
     * @tc.expected: step5. check linear curve's value reach the end value.
     */
    controller->Resume();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: AnimationPauseTest002
 * @tc.desc: test pause animation
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPauseTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationPauseTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * ANIMATION_DURATION_MULTIPLE;
    flushEventMock_->animationInt_ = AceType::MakeRefPtr<CurveAnimation<int>>(1, 4, Curves::LINEAR);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger frames to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step3. pause -> stop -> goforward
     */
    auto controller = flushEventMock_->GetAnimator();
    controller->Pause();
    controller->Stop();
    controller->Play();

    /**
     * @tc.steps: step4. trigger one frame
     * @tc.expected: step4. check linear curve's value change.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(2, flushEventMock_->animationIntValue_);

    /**
     * @tc.steps: step5. trigger the last frame
     * @tc.expected: step5. check linear curve's value change.
     */
    controller->Resume();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
}

/**
 * @tc.name: KeyframeAnimationTest001
 * @tc.desc: Verify the keyframe animation with only one keyframe.
 * @tc.type: FUNC
 * @tc.require: AR000DBTLS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, KeyframeAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest KeyframeAnimationTest001 start";
    /**
     * @tc.steps: step1. init keyframe animation and animator.
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * KEYFRAME_ANIMATION_DURATION_MULTIPLE;
    float begin = 1.0f;
    auto kf1 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, begin);
    flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf1);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 3 frames to verify keyframe animation
     * @tc.expected: step3. the value of each frame remains the same as the beginning value.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(begin, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(begin, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(begin, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: KeyframeAnimationTest002
 * @tc.desc: Verify the keyframe animation with two keyframes and set curve to linear curve.
 * @tc.type: FUNC
 * @tc.require: AR000DBTLS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, KeyframeAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest KeyframeAnimationTest002 start";
    /**
     * @tc.steps: step1. init keyframe animation and animator,set curve to linear curve.
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * KEYFRAME_ANIMATION_DURATION_MULTIPLE;
    float begin = 1.0f;
    float end = 10.0f;
    auto kf1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
    auto kf2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);
    kf2->SetCurve(Curves::LINEAR);
    flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf1);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf2);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to verify keyframe animation
     * @tc.expected: step3. the value of each frame changes linearly
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR((begin + end) / 2.0f, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(end, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: KeyframeAnimationTest003
 * @tc.desc: Verify the keyframe animation with two keyframes and without setting curve.
 * @tc.type: FUNC
 * @tc.require: AR000DBTLS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, KeyframeAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest KeyframeAnimationTest003 start";
    /**
     * @tc.steps: step1. init keyframe animation and animator.
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * KEYFRAME_ANIMATION_DURATION_MULTIPLE;
    float begin = 1.0f;
    float end = 10.0f;
    auto kf1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
    auto kf2 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);

    flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf1);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf2);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to verify keyframe animation
     * @tc.expected: step3. the value of each frame changes linearly
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(8.21793, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(end, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: KeyframeAnimationTest004
 * @tc.desc: Verify the keyframe animation with three keyframes and set all curves to linear curve.
 * @tc.type: FUNC
 * @tc.require: AR000DBTLS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, KeyframeAnimationTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest KeyframeAnimationTest004 start";
    /**
     * @tc.steps: step1. init keyframe animation and animator.
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * KEYFRAME_ANIMATION_DURATION_MULTIPLE;
    float begin = 1.0f;
    float middle = 5.0f;
    float end = 10.0f;

    auto kf1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
    auto kf2 = AceType::MakeRefPtr<Keyframe<float>>(0.5f, middle);
    auto kf3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);

    kf2->SetCurve(Curves::LINEAR);
    kf3->SetCurve(Curves::LINEAR);

    flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf1);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf2);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf3);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to verify keyframe animation
     * @tc.expected: step3. the value of each frame changes linearly
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(middle, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(end, flushEventMock_->keyframeAnimationValue_, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: KeyframeAnimationTest005
 * @tc.desc: Verify the keyframe animation with three keyframes and set different curve.
 * @tc.type: FUNC
 * @tc.require: AR000DBTLS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, KeyframeAnimationTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest KeyframeAnimationTest005 start";
    /**
     * @tc.steps: step1. init keyframe animation and animator.
     */
    int32_t frameTimeMs = FRAME_TIME_IN_MILLISECOND;
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs);
    flushEventMock_->InitController(context_);
    int32_t duration = frameTimeMs * KEYFRAME_ANIMATION_DURATION_MULTIPLE;
    float begin = 1.0f;
    float middle = 5.0f;
    float end = 10.0f;

    auto kf1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, begin);
    auto kf2 = AceType::MakeRefPtr<Keyframe<float>>(0.5f, middle);
    auto kf3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, end);

    kf2->SetCurve(Curves::LINEAR);
    kf3->SetCurve(Curves::SINE);

    flushEventMock_->keyframeAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf1);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf2);
    flushEventMock_->keyframeAnimation_->AddKeyframe(kf3);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 2 frames to verify keyframe animation
     * @tc.expected: step3. the value of each frame changes correctly
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(middle, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(end, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
}

/**
 * @tc.name: AnimationInterpolateTest001
 * @tc.desc: duration equals zero
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationInterpolateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationInterpolateTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 0;
    // default curve is linear. test it with nullptr.
    flushEventMock_->animationFloat_ = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 6.0f, nullptr);
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 1 frame to let animation done
     * @tc.expected: step3. stop status is true
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationInterpolateTest002
 * @tc.desc: start interpolate animation without set it.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationInterpolateTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationInterpolateTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    flushEventMock_->InitController(context_);
    int32_t duration = 0;
    // default curve is linear. test it with nullptr.
    flushEventMock_->animationDuration_ = duration;
    context_->AddPostFlushListener(flushEventMock_);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. trigger 1 frame to let animation done
     * @tc.expected: step3. check no animation get listener callback
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(0.0f, flushEventMock_->animationFloatValue_, FLT_EPSILON);
}

/**
 * @tc.name: AnimationFrictionMotionTest001
 * @tc.desc: play friction motion in a very short duration (less than a frame)
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrictionMotionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrictionMotionTest001 start";
    /**
     * @tc.steps: step1. init friction motion and animator
     */
    platformWindowRaw_->SetNanoFrameTime(NANO_FRAME_TIME * 1000);
    flushEventMock_->InitController(context_);
    auto frictionMotion = AceType::MakeRefPtr<FrictionMotion>(0.0, 1.0, 1.0);
    auto controller = flushEventMock_->GetAnimator();
    controller->PlayMotion(frictionMotion);

    /**
     * @tc.steps: step2. trigger one frame to make it done
     * @tc.expected: step2. scroll has been done.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_TRUE(frictionMotion->IsCompleted());
}

/**
 * @tc.name: AnimationFrictionMotionTest002
 * @tc.desc: play friction motion in positive velocity.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationFrictionMotionTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationFrictionMotionTest002 start";
    /**
     * @tc.steps: step1. init friction motion and animator
     */
    platformWindowRaw_->SetNanoFrameTime(NANO_FRAME_TIME * 1000);
    flushEventMock_->InitController(context_);
    auto frictionMotion = AceType::MakeRefPtr<FrictionMotion>(1.0, 100.0, 100.0);

    EXPECT_NEAR(100.0, frictionMotion->GetPosition(0.0), 0.1);
    EXPECT_NEAR(100.0, frictionMotion->GetVelocity(0.0), 0.1);

    EXPECT_NEAR(120.89, frictionMotion->GetPosition(0.5), 1.0);
    EXPECT_NEAR(113.09, frictionMotion->GetFinalPosition(), 0.1);
}

/**
 * @tc.name: AnimationSpringMotionTest001
 * @tc.desc: Verify critical damped motion
 * @tc.type: FUNC
 * @tc.require: AR000DBULF
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationSpringMotionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationSpringMotionTest001 start";
    /**
     * @tc.steps: step1. init SpringMotion with critical damped SpringProperty.
     */
    auto springDescription = AceType::MakeRefPtr<SpringProperty>(1.0, 100.0, 20.0);
    auto springMotion = AceType::MakeRefPtr<SpringMotion>(0.0, 500.0, 0.0, springDescription);
    EXPECT_EQ(springMotion->GetType(), SpringModelType::CRITICAL_DAMPED);

    /**
     * @tc.steps: step2. verify springMotion start point.
     * @tc.expected: step2. result is correct.
     */
    springMotion->Move(0.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 0.0, DBL_EPSILON);
    EXPECT_NEAR(springMotion->GetCurrentVelocity(), 5000.0, DBL_EPSILON);

    /**
     * @tc.steps: step3. verify springMotion middle point.
     * @tc.expected: step3. result is correct.
     */
    springMotion->Move(500.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_EQ(floor(springMotion->GetCurrentPosition()), 496);
    EXPECT_EQ(floor(springMotion->GetCurrentVelocity()), 33);

    /**
     * @tc.steps: step4. verify springMotion end point.
     * @tc.expected: step4. result is correct.
     */
    springMotion->Move(1600.0f);
    EXPECT_TRUE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 500.0, DBL_EPSILON);
    EXPECT_NEAR(springMotion->GetCurrentVelocity(), 0.0, DBL_EPSILON);
}

/**
 * @tc.name: AnimationSpringMotionTest002
 * @tc.desc: Verify overdamped motion
 * @tc.type: FUNC
 * @tc.require: AR000DBULF
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationSpringMotionTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationSpringMotionTest002 start";
    /**
     * @tc.steps: step1. init SpringMotion with overdamped SpringProperty.
     */
    auto springDescription = AceType::MakeRefPtr<SpringProperty>(1.0, 100.0, 25.0);
    auto springMotion = AceType::MakeRefPtr<SpringMotion>(0.0, 500.0, 0.0, springDescription);
    EXPECT_EQ(springMotion->GetType(), SpringModelType::OVER_DAMPED);

    /**
     * @tc.steps: step2. verify springMotion start point.
     * @tc.expected: step2. result is correct.
     */
    springMotion->Move(0.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 0.0, DBL_EPSILON);

    /**
     * @tc.steps: step3. verify springMotion middle point.
     * @tc.expected: step3. result is correct.
     */
    springMotion->Move(500.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_EQ(floor(springMotion->GetCurrentPosition()), 445);
    EXPECT_EQ(floor(springMotion->GetCurrentVelocity()), 273);

    /**
     * @tc.steps: step4. verify springMotion end point.
     * @tc.expected: step4. result is correct.
     */
    springMotion->Move(3100.0f);
    EXPECT_TRUE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 500.0, DBL_EPSILON);
    EXPECT_NEAR(springMotion->GetCurrentVelocity(), 0.0, DBL_EPSILON);
}

/**
 * @tc.name: AnimationSpringMotionTest003
 * @tc.desc: Verify underdamped motion
 * @tc.type: FUNC
 * @tc.require: AR000DBULF
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationSpringMotionTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationSpringMotionTest003 start";
    /**
     * @tc.steps: step1. init SpringMotion with underdamped SpringProperty.
     */
    auto springDescription = AceType::MakeRefPtr<SpringProperty>(1.0, 100.0, 5.0);
    auto springMotion = AceType::MakeRefPtr<SpringMotion>(0.0, 300.0, 0.0, springDescription);
    EXPECT_EQ(springMotion->GetType(), SpringModelType::UNDER_DAMPED);

    /**
     * @tc.steps: step2. verify springMotion start point.
     * @tc.expected: step2. result is correct.
     */
    springMotion->Move(0.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 0.0, DBL_EPSILON);

    /**
     * @tc.steps: step3. verify springMotion middle point.
     * @tc.expected: step3. result is correct.
     */
    springMotion->Move(1000.0f);
    EXPECT_FALSE(springMotion->IsCompleted());
    EXPECT_EQ(floor(springMotion->GetCurrentPosition()), 325);
    EXPECT_EQ(floor(springMotion->GetCurrentVelocity()), -65);

    /**
     * @tc.steps: step4. verify springMotion end point.
     * @tc.expected: step4. result is correct.
     */
    springMotion->Move(6000.0f);
    EXPECT_TRUE(springMotion->IsCompleted());
    EXPECT_NEAR(springMotion->GetCurrentPosition(), 300.0, DBL_EPSILON);
    EXPECT_NEAR(springMotion->GetCurrentVelocity(), 0.0, DBL_EPSILON);
}

/**
 * @tc.name: AnimationSpringMotionTest004
 * @tc.desc: Verify Scroll Spring motion
 * @tc.type: FUNC
 * @tc.require: AR000DBULF
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationSpringMotionTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationSpringMotionTest004 start";
    /**
     * @tc.steps: step1. init ScrollSpringMotion with critical damped SpringProperty.
     */
    auto springDescription = AceType::MakeRefPtr<SpringProperty>(1.0, 100.0, 20.0);
    auto scrollSpringMotion = AceType::MakeRefPtr<ScrollSpringMotion>(0.0, 500.0, 0.0, springDescription);
    EXPECT_EQ(scrollSpringMotion->GetType(), SpringModelType::CRITICAL_DAMPED);

    /**
     * @tc.steps: step2. verify ScrollSpringMotion start point.
     * @tc.expected: step2. result is correct.
     */
    scrollSpringMotion->Move(0.0f);
    EXPECT_FALSE(scrollSpringMotion->IsCompleted());
    EXPECT_NEAR(scrollSpringMotion->GetCurrentPosition(), 0.0, DBL_EPSILON);

    /**
     * @tc.steps: step3. verify ScrollSpringMotion middle point.
     * @tc.expected: step3. result is correct.
     */
    scrollSpringMotion->Move(500.0f);
    EXPECT_FALSE(scrollSpringMotion->IsCompleted());
    EXPECT_EQ(floor(scrollSpringMotion->GetCurrentPosition()), 496);

    /**
     * @tc.steps: step4. verify ScrollSpringMotion end point.
     * @tc.expected: step4. result is correct.
     */
    scrollSpringMotion->Move(1500.0f);
    EXPECT_TRUE(scrollSpringMotion->IsCompleted());
    EXPECT_NEAR(scrollSpringMotion->GetCurrentPosition(), 500.0, DBL_EPSILON);
}

/**
 * @tc.name: AnimationScrollMotionTest001
 * @tc.desc: play scroll motion: always spring
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationScrollMotionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationScrollMotionTest001 start";
    /**
     * @tc.steps: step1. init motion and animator
     */
    auto spring = AceType::MakeRefPtr<SpringProperty>(1.0, 170.0, 28.684);
    platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * 1000);
    flushEventMock_->InitController(context_);
    auto scrollMotion =
        AceType::MakeRefPtr<ScrollMotion>(500.0, -7500.0, ExtentPair(0.0, 1000.0), ExtentPair(0.0, 1000.0), spring);
    scrollMotion->AddListener([this](const double& position) { flushEventMock_->SetPositionResult(position); });
    auto controller = flushEventMock_->GetAnimator();
    controller->PlayMotion(scrollMotion);

    /**
     * @tc.steps: step2. trigger frames to make it done
     * @tc.expected: step2. friction has been done.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_FALSE(scrollMotion->IsCompleted());

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_FALSE(scrollMotion->IsCompleted());
    EXPECT_NEAR(500.0, flushEventMock_->GetPositionResult(), CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: AnimationBasicPropertyTest001
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: none, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 1, AnimationOperation::REVERSE, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest002
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: none, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::REVERSE, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest003
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: forward, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 1, AnimationOperation::REVERSE, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest004
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: forward, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest004 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::REVERSE, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest005
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: none, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest005 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 1, AnimationOperation::PLAY, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest006
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: none, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest006 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::PLAY, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest007
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: forward, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest007 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 1, AnimationOperation::PLAY, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest008
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: forward, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest008 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::PLAY, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest009
 * @tc.desc: Test combination for init: null, from 1 -> 9; reverse, fill-mode: none, duration: 0，repeat: 1, delay: 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest009 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 2, AnimationOperation::REVERSE, FillMode::NONE, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest010
 * @tc.desc: Test combination for init: null, from 1 -> 9; reverse, fill-mode: none, duration: 20，repeat: 1, delay: 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest010 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 2, AnimationOperation::REVERSE, FillMode::NONE, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationBasicPropertyTest011
 * @tc.desc: Test combination for init: null, from 1 -> 9; reverse, fill-mode: forwards, duration: 0，repeat: 1, delay:
 * 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest011 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 2, AnimationOperation::REVERSE, FillMode::FORWARDS, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest012
 * @tc.desc: Test combination for init: null, from 1 -> 9; reverse, fill-mode: forwards, duration: 20，repeat: 1, delay:
 * 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest012 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 2, AnimationOperation::REVERSE, FillMode::FORWARDS, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationBasicPropertyTest013
 * @tc.desc: Test combination for init: null, from 1 -> 9; normal, fill-mode: none, duration: 0，repeat: 1, delay: 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest013 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 2, AnimationOperation::PLAY, FillMode::NONE, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest014
 * @tc.desc: Test combination for init: null, from 1 -> 9; normal, fill-mode: none, duration: 20，repeat: 1, delay: 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest014 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 2, AnimationOperation::PLAY, FillMode::NONE, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationBasicPropertyTest015
 * @tc.desc: Test combination for init: null, from 1 -> 9; normal, fill-mode: forwards, duration: 0，repeat: 1, delay:
 * 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest015 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(0, 2, AnimationOperation::PLAY, FillMode::FORWARDS, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationBasicPropertyTest016
 * @tc.desc: Test combination for init: null, from 1 -> 9; normal, fill-mode: forwards, duration: 20，repeat: 1, delay:
 * 10
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationBasicPropertyTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest016 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 2, AnimationOperation::PLAY, FillMode::FORWARDS, false);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationEndTest001
 * @tc.desc: End controller for animation when in start delay
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationEndTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationEndTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitAnimationAndAnimator(FRAME_TIME_IN_MILLISECOND, 2, FRAME_TIME_IN_MILLISECOND);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. end when in start delay
     */
    platformWindowRaw_->TriggerOneFrame();
    flushEventMock_->GetAnimator()->Finish();

    /**
     * @tc.expected: step4. check the end
     */
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationEndTest002
 * @tc.desc: End controller for animation when in first loop
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationEndTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationEndTest002 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitAnimationAndAnimator(FRAME_TIME_IN_MILLISECOND, 2, FRAME_TIME_IN_MILLISECOND);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. skip start delay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    flushEventMock_->GetAnimator()->Finish();

    /**
     * @tc.expected: step4. check the end
     */
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationEndTest003
 * @tc.desc: End controller for animation when in last loop
 * @tc.type: FUNC
 * @tc.require: AR000DAIGS
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationEndTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationEndTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitAnimationAndAnimator(FRAME_TIME_IN_MILLISECOND, 1, FRAME_TIME_IN_MILLISECOND);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->postFlushCallTimes_);

    /**
     * @tc.steps: step3. skip start delay.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    flushEventMock_->GetAnimator()->Finish();

    /**
     * @tc.expected: step4. check the end
     */
    EXPECT_EQ(4, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationPlayTest001
 * @tc.desc: play controller for animation when is playing
 * @tc.type: FUNC
 * @tc.require: AR000DQ20L
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPlayTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::PLAY, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationPlayTest002
 * @tc.desc: play controller for animation when is reverse playing
 * @tc.type: FUNC
 * @tc.require: AR000DQ20L
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPlayTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest001 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::REVERSE, FillMode::NONE, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationPlayTest003
 * @tc.desc: play controller for animation when is finished
 * @tc.type: FUNC
 * @tc.require: AR000DQ20L
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPlayTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest003 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::FINISH, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
    flushEventMock_->animationStopStatus_ = false;
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();

    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationPlayTest004
 * @tc.desc: play controller for animation when is paused
 * @tc.type: FUNC
 * @tc.require: AR000DQ20L
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPlayTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest004 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::PAUSE, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(-1, flushEventMock_->animationIntValue_);
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationPlayTest005
 * @tc.desc: play controller for animation when is canceled
 * @tc.type: FUNC
 * @tc.require: AR000DQ20L
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationPlayTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AnimationFrameworkTest AnimationBasicPropertyTest005 start";
    /**
     * @tc.steps: step1. init animation and animator
     */
    InitBasicPropertyTest(20, 1, AnimationOperation::CANCEL, FillMode::FORWARDS, true);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(-1, flushEventMock_->animationIntValue_);
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationDirectionTest001
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: forwards,
 * AnimationDirection: alternate, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 1, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest002
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; reverse, fill-mode: forward,
 * AnimationDirection: alternate, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 1, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest003
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: forward,
 * AnimationDirection: alternate, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 1, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationDirectionTest004
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forward, AnimationDirection:
 * alternate, duration: 0, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 2, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest005
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 2, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTest(1.0f, 2);
}

/**
 * @tc.name: AnimationDirectionTest006
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 2, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTest(1.0f, 2);
}

/**
 * @tc.name: AnimationDirectionTest007
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * normal, duration: 0, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 2, AnimationDirection::NORMAL, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationDirectionTest008
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forward, AnimationDirection:
 * alternate, duration: 0, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 3, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest009
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 3, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTest(1.0f, 3);
}

/**
 * @tc.name: AnimationDirectionTest010
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 3, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    RunAndCheckBasicAnimationDirectionPropertyTest(9.0f, 3);
}

/**
 * @tc.name: AnimationDirectionTest011
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * alternate, duration: 0, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 3, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationDirectionTest012
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: infinite
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, -1, AnimationDirection::ALTERNATE, AnimationOperation::REVERSE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

    /**
     * @tc.steps: step4. repeat animation
     * @tc.expected: step4. check repeat animation value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    /**
     * @tc.steps: step5. repeat animation
     * @tc.expected: step5. check repeat animation value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();
}

/**
 * @tc.name: AnimationDirectionTest013
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * alternate, duration: 20, repeat: infinite
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, -1, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    /**
     * @tc.steps: step3. trigger frames to make animation done
     * @tc.expected: step3. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

    /**
     * @tc.steps: step4. repeat animation
     * @tc.expected: step4. check repeat animation value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    /**
     * @tc.steps: step5. repeat animation
     * @tc.expected: step5. check repeat animation value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();
}

/**
 * @tc.name: AnimationDirectionTest014
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forward, AnimationDirection:
 * alternate, duration: 40, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        40, -1, AnimationDirection::ALTERNATE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    flushEventMock_->GetAnimator()->Reverse();

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    /**
     * @tc.steps: step3. repeat animation
     * @tc.expected: step3. check repeat animation value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(7, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(7.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    flushEventMock_->GetAnimator()->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
    flushEventMock_->GetAnimator()->Play();

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(7, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(7.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationDirectionTest015
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: none,
 * AnimationDirection: normal, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: wanyanglan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(0, 1, AnimationDirection::NORMAL, AnimationOperation::PLAY, FillMode::NONE);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationDirectionTest016
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: forwards,
 * AnimationDirection: alternate-reverse, duration: 0
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 1, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest017
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: forwards,
 * AnimationDirection: alternate-reverse, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 1, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest018
 * @tc.desc: Test combination for no delay, no repeat. init: 0, from 1 -> 9; normal, fill-mode: none,
 * AnimationDirection: alternate-reverse, duration: 20
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 1, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::NONE);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(0.0f);
}

/**
 * @tc.name: AnimationDirectionTest019
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forwards, AnimationDirection:
 * alternate-reverse, duration: 0, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 2, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(9.0f);
}

/**
 * @tc.name: AnimationDirectionTest020
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forwards, AnimationDirection:
 * alternate-reverse, duration: 20, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VN
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 2, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

    RunAndCheckBasicPropertyTestInterpolateValueForward(9.0f);
}

/**
 * @tc.name: AnimationDirectionTest021
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forwards, AnimationDirection:
 * alternate-reverse, duration: 20, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VK
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 3, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    RunAndCheckBasicPropertyTestInterpolateValueReverse(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest022
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: none, AnimationDirection:
 * alternate-reverse, duration: 20, repeat: 3
 * @tc.type: FUNC
 * @tc.require: AR000FL0VJ
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 3, AnimationDirection::ALTERNATE_REVERSE, AnimationOperation::PLAY, FillMode::NONE);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicAnimationDirectionPropertyTestReverse();

    RunAndCheckBasicAnimationDirectionPropertyTestForward();

    RunAndCheckBasicPropertyTestInterpolateValueReverse(0.0f);
}

/**
 * @tc.name: AnimationDirectionTest023
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; reverse, fill-mode: forwards, AnimationDirection:
 * reverse, duration: 0, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VI
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        0, 2, AnimationDirection::REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    RunAndCheckBasicPropertyTestInterpolateValue(1.0f);
}

/**
 * @tc.name: AnimationDirectionTest024
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: forwards, AnimationDirection:
 * reverse, duration: 20, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VH
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 2, AnimationDirection::REVERSE, AnimationOperation::PLAY, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(1, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(1.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationDirectionTest025
 * @tc.desc: Test combination for no delay. init: 0, from 1 -> 9; normal, fill-mode: none, AnimationDirection:
 * reverse, duration: 20, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VM
 * @tc.author: jiangdayuan
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        20, 2, AnimationDirection::REVERSE, AnimationOperation::PLAY, FillMode::NONE);

    /**
     * @tc.steps: step2. trigger frames to make animation done
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(9, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(9.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(0, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(0.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_TRUE(flushEventMock_->animationStopStatus_);
}

/**
 * @tc.name: AnimationDirectionTest026
 * @tc.desc: Test for Forward and Backward twice
 * reverse, duration: 40, repeat: 2
 * @tc.type: FUNC
 * @tc.require: AR000FL0VM
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        40, 1, AnimationDirection::NORMAL, AnimationOperation::NONE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. first round. trigger forward and backward
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    auto controller = flushEventMock_->GetAnimator();
    controller->Forward();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    controller->Backward();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);

    /**
     * @tc.steps: step3. second round. trigger forward and backward
     * @tc.expected: step3. check interpolateValue and end value is correct
     */
    controller->Forward();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);

    controller->Backward();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
}

/**
 * @tc.name: AnimationDirectionTest027
 * @tc.desc: Test for play / Forward
 * normal, duration: 40, repeat: 0
 * @tc.type: FUNC
 * @tc.require: AR000FL0VM
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest027, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        40, 1, AnimationDirection::NORMAL, AnimationOperation::NONE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. first round. trigger play and forward
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    auto controller = flushEventMock_->GetAnimator();
    controller->Play();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    controller->Forward();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(7, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(7.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
}

/**
 * @tc.name: AnimationDirectionTest028
 * @tc.desc: Test for Reverse / Backward
 * normal, duration: 40, repeat: 0
 * @tc.type: FUNC
 * @tc.require: AR000FL0VM
 * @tc.author: zhouzebin
 */
HWTEST_F(AnimationFrameworkTest, AnimationDirectionTest028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init animation and simulation controller
     */
    InitBasicAnimationDirectionPropertyTest(
        40, 1, AnimationDirection::NORMAL, AnimationOperation::NONE, FillMode::FORWARDS);

    /**
     * @tc.steps: step2. first round. trigger reverse and backward
     * @tc.expected: step2. check interpolateValue and end value is correct
     */
    auto controller = flushEventMock_->GetAnimator();
    controller->Reverse();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);

    controller->Backward();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(3, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(3.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);

    controller->Reverse();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(5, flushEventMock_->animationIntValue_);
    EXPECT_NEAR(5.0f, flushEventMock_->keyframeAnimationValue_, FLT_EPSILON);
    EXPECT_FALSE(flushEventMock_->animationStopStatus_);
}

} // namespace OHOS::Ace
