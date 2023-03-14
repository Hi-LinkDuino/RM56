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
#include "base/json/json_util.h"
#include "base/log/log.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/flush_event.h"
#include "core/animation/svg_animate.h"
#include "core/animation/test/unittest/mock/animation_test_utils.h"
#include "core/components/test/json/json_frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

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
constexpr double CUBIC_ERROR_BOUND = 0.001;

} // namespace

template<class T>
class SvgAnimationMock : public FlushEvent {
    DECLARE_ACE_TYPE(SvgAnimationMock, FlushEvent);

public:
    SvgAnimationMock(T originValue, const RefPtr<SvgAnimate>& svgAnimate, const WeakPtr<PipelineContext>& context)
        : FlushEvent()
    {
        originValue_ = originValue;
        currentValue_ = originValue_;
        svgAnimate_ = svgAnimate;
        animator_ = AceType::MakeRefPtr<Animator>(context);
    }

    void CreatePropertyAnimate()
    {
        if (!svgAnimate_) {
            return;
        }
        std::function<void(T)> callback;
        callback = [weak = AceType::WeakClaim(this)](T value) {
            auto mock = weak.Upgrade();
            if (!mock) {
                LOGE("mock is null");
                return;
            }
            mock->currentValue_ = value;
        };
        svgAnimate_->CreatePropertyAnimate(std::move(callback), originValue_, animator_);
    }

    void CreateMotionAnimate()
    {
        if (!svgAnimate_) {
            return;
        }
        std::function<void(double)> callback;
        callback = [weak = AceType::WeakClaim(this)](double value) {
            auto mock = weak.Upgrade();
            if (!mock) {
                LOGE("mock is null");
                return;
            }
            mock->motionValue_ = value;
        };
        svgAnimate_->CreateMotionAnimate(std::move(callback), animator_);
    }

    void OnPostFlush() override
    {
        if (!svgAnimate_) {
            return;
        }
        if (svgAnimate_->GetSvgAnimateType() == SvgAnimateType::ANIMATE) {
            CreatePropertyAnimate();
        } else if (svgAnimate_->GetSvgAnimateType() == SvgAnimateType::MOTION) {
            CreateMotionAnimate();
        } else {
            LOGE("invalid animation type");
        }
    }

    T currentValue_;
    T originValue_;
    double motionValue_ = 0.0;

private:
    RefPtr<SvgAnimate> svgAnimate_;
    RefPtr<Animator> animator_;
};

class SvgAnimateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SvgAnimateTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "SvgAnimateTest TearDownTestCase";
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
        context_->SetupRootElement();
        context_->OnSurfaceChanged(100, 200);
    }

    void TearDown() override {}

protected:
    RefPtr<PipelineContext> context_;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
};

/**
 * @tc.name: SvgAnimateTest
 * @tc.desc: Verify the property animation process for type double
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest001 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("opacity");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    svgAnimate->SetFrom("0.1");
    svgAnimate->SetTo("1.0");
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<double>>(0.0, svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_, 0.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_, 0.193, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_, 0.38, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_, 1.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest002
 * @tc.desc: Verify the property animation process for type color
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest002 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("fill");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    svgAnimate->SetFrom("red");
    svgAnimate->SetTo("blue");
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Color>>(Color::RED, svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_TRUE(flushEventMock->currentValue_.GetValue() == Color::RED.GetValue());
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_EQ(flushEventMock->currentValue_.GetValue(), 4294049883);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_EQ(flushEventMock->currentValue_.GetValue(), 4292280470);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_TRUE(flushEventMock->currentValue_.GetValue() == Color::BLUE.GetValue());
}

/**
 * @tc.name: SvgAnimateTest003
 * @tc.desc: Verify the property animation process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest003 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    svgAnimate->SetFrom("100");
    svgAnimate->SetTo("300");
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 120.75, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 162.375, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest004
 * @tc.desc: Verify the property keyframe animation process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest004 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    std::vector<std::string> values = { "50", "100", "200", "300" };
    svgAnimate->SetValues(values);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 65.719, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 97.254, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest005
 * @tc.desc: Verify the property keyframe with keyTimes animation process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest005 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    std::vector<std::string> values = { "50", "100", "200", "300" };
    svgAnimate->SetValues(values);
    std::vector<double> keyTimes = { 0.0, 0.2, 0.7, 1.0 };
    svgAnimate->SetKeyTimes(keyTimes);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 75.937, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 122.375, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest006
 * @tc.desc: Verify the property keyframe animation with keysplines process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest006 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::SPLINE);
    std::vector<std::string> values = { "50", "100", "200", "300" };
    svgAnimate->SetValues(values);
    std::vector<double> keyTimes = { 0.0, 0.2, 0.7, 1.0 };
    svgAnimate->SetKeyTimes(keyTimes);
    std::vector<std::string> keySplines = { "0.5 0 0.5 1", "0.5 0 0.5 1", "0.5 0 0.5 1" };
    svgAnimate->SetKeySplines(keySplines);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 76.903, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 108.183, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest007
 * @tc.desc: Verify the property keyframe animation with discrete calcMode process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest007 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::DISCRETE);
    std::vector<std::string> values = { "100", "200", "300" };
    svgAnimate->SetValues(values);
    std::vector<double> keyTimes = { 0.0, 0.2, 0.7, 1.0 };
    svgAnimate->SetKeyTimes(keyTimes);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 100.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 200.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(40);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest008
 * @tc.desc: Verify the property keyframe animation with paced calcMode process for type dimension
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest008 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetAttributeName("width");
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetCalcMode(CalcMode::PACED);
    std::vector<std::string> values = { "50", "100", "200", "300" };
    svgAnimate->SetValues(values);
    std::vector<double> keyTimes = { 0.0, 0.2, 0.7, 1.0 };
    svgAnimate->SetKeyTimes(keyTimes);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::ANIMATE);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<Dimension>>(Dimension(50.0), svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 50.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 65.72, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 97.254, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->currentValue_.Value(), 300.0, CUBIC_ERROR_BOUND);
}

/**
 * @tc.name: SvgAnimateTest009
 * @tc.desc: Verify the motion animate
 * @tc.type: FUNC
 */
HWTEST_F(SvgAnimateTest, SvgAnimateTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvgAnimateTest SvgAnimateTest009 start";
    /**
     * @tc.steps: step1. init svg animate and animator
     */
    RefPtr<SvgAnimate> svgAnimate = AceType::MakeRefPtr<SvgAnimate>();
    svgAnimate->SetBegin(0);
    svgAnimate->SetDur(1600);
    svgAnimate->SetFillMode(Fill::FREEZE);
    svgAnimate->SetRepeatCount(1);
    svgAnimate->SetCalcMode(CalcMode::LINEAR);
    std::vector<std::string> keyPoints = { "0", "0.5", "1" };
    svgAnimate->SetKeyPoints(keyPoints);
    std::vector<double> keyTimes = { 0.0, 0.4, 1.0 };
    svgAnimate->SetKeyTimes(keyTimes);
    svgAnimate->SetSvgAnimateType(SvgAnimateType::MOTION);
    auto flushEventMock = AceType::MakeRefPtr<SvgAnimationMock<double>>(0.0, svgAnimate, context_);
    context_->AddPostFlushListener(flushEventMock);

    /**
     * @tc.steps: step2. trigger one frame to make prepare animation work
     * @tc.expected: step2. postFlush has been called.
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_NEAR(flushEventMock->motionValue_, 0.0, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(10);
    EXPECT_NEAR(flushEventMock->motionValue_, 0.13, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(20);
    EXPECT_NEAR(flushEventMock->motionValue_, 0.389, CUBIC_ERROR_BOUND);
    platformWindowRaw_->TriggerFrames(70);
    EXPECT_NEAR(flushEventMock->motionValue_, 1.0, CUBIC_ERROR_BOUND);
}

} // namespace OHOS::Ace
