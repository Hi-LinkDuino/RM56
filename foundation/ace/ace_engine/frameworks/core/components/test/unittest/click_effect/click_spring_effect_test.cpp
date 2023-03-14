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

#include <cstdint>

#include "gtest/gtest.h"
#define private public
#define protected public
#include "core/animation/animator.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/transform/click_spring_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class MockAnimator : public Animator {
    DECLARE_ACE_TYPE(MockAnimator, Animator);

public:
    explicit MockAnimator(const WeakPtr<PipelineContext>& context) : Animator(context) {}
    ~MockAnimator() = default;
    void GotoEnd()
    {
        // just go to end
        NotifyMotion(INT32_MAX);
    }
};

class ClickSpringEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ClickSpringEffectTest::SetUpTestCase() {}
void ClickSpringEffectTest::TearDownTestCase() {}
void ClickSpringEffectTest::SetUp() {}
void ClickSpringEffectTest::TearDown() {}

/**
 * @tc.name: ClickSpringEffectTest001
 * @tc.desc: Verify ClickeSpringEffect is work right.
 * @tc.type: FUNC
 */
HWTEST_F(ClickSpringEffectTest, ClickSpringEffectTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the clickEffect and controller.
     */
    auto context = MockRenderCommon::GetMockContext();
    auto clickSpringEffect = AceType::MakeRefPtr<ClickSpringEffect>(context);
    auto controller = AceType::MakeRefPtr<MockAnimator>(context);
    clickSpringEffect->SetController(controller);
    /**
     * @tc.steps: step2. call the ShowAnimation with TouchType::Down and MEDIA type.
     * @tc.expected: step2. scale should be 1.0
     */
    clickSpringEffect->ShowAnimation(TouchType::DOWN, ClickSpringEffectType::MEDIUM);
    auto scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 1.0);
    /**
     * @tc.steps: step3. call the Controller go to the end, test the target is right.
     * @tc.expected: step3. scale should be 0.95
     */
    controller->GotoEnd();
    scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 0.95);

    /**
     * @tc.steps: step4. call the ShowAnimation with TouchType::Down and MEDIA type.
     * @tc.expected: step4. scale should be 1.0;
     */
    clickSpringEffect->ShowAnimation(TouchType::UP, ClickSpringEffectType::MEDIUM);
    controller->GotoEnd();
    scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 1.0);
}

/**
 * @tc.name: ClickSpringEffectTest002
 * @tc.desc: Verify ClickeSpringEffect is work right.
 * @tc.type: FUNC
 */
HWTEST_F(ClickSpringEffectTest, ClickSpringEffectTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the clickEffect and controller.
     */
    auto context = MockRenderCommon::GetMockContext();
    auto clickSpringEffect = AceType::MakeRefPtr<ClickSpringEffect>(context);
    auto controller = AceType::MakeRefPtr<MockAnimator>(context);
    clickSpringEffect->SetController(controller);

    /**
     * @tc.steps: step2. call the ShowAnimation with TouchType::Up and SMALL type.
     * @tc.expected: step2. scale should be 1.0
     */
    clickSpringEffect->ShowAnimation(TouchType::UP, ClickSpringEffectType::SMALL);
    controller->GotoEnd();
    double scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 1.0);

    /**
     * @tc.steps: step3. call the ShowAnimation with TouchType::Down and SMALL type.
     * @tc.expected: step3. scale should be 0.9;
     */
    clickSpringEffect->ShowAnimation(TouchType::DOWN, ClickSpringEffectType::SMALL);
    controller->GotoEnd();
    scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 0.9);

    /**
     * @tc.steps: step4. call the ShowAnimation with TouchType::Down and MEDIUM type.
     * @tc.expected: step4. scale should be 0.95;
     */
    clickSpringEffect->ShowAnimation(TouchType::DOWN, ClickSpringEffectType::MEDIUM);
    controller->GotoEnd();
    scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 0.95);

    /**
     * @tc.steps: step5. call the ShowAnimation with TouchType::CANCEL and LARGE type.
     * @tc.expected: step5. scale should be 1.0;
     */
    clickSpringEffect->ShowAnimation(TouchType::CANCEL, ClickSpringEffectType::LARGE);
    controller->GotoEnd();
    scale = clickSpringEffect->GetScale();
    GTEST_LOG_(INFO) << scale;
    EXPECT_EQ(scale, 1.0);
}

} // namespace OHOS::Ace