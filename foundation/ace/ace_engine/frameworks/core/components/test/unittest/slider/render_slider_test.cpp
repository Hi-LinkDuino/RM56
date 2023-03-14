/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "base/i18n/localization.h"
#include "core/components/slider/block_component.h"
#include "core/components/slider/render_block.h"
#include "core/components/slider/render_slider.h"
#include "core/components/slider/slider_component.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/tip/render_tip.h"
#include "core/components/tip/tip_component.h"
#include "core/components/track/render_track.h"
#include "core/components/track/track_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderSliderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RenderSliderTest::SetUpTestCase() {}
void RenderSliderTest::TearDownTestCase() {}
void RenderSliderTest::TearDown() {}
void RenderSliderTest::SetUp()
{
    Localization::GetInstance()->SetLocale("zh", "CN", "", "", "");
}

/**
 * @tc.name: RenderSliderTest001
 * @tc.desc: Verify PerformLayout can calculate slider properties.
 * @tc.type: FUNC
 * @tc.require: AR000F3L3A
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderSliderTest, RenderSliderTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the slider component.
     */
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(0.0, 1.0, 0.0, 100.0);
    slider->SetShowTips(true);
    slider->SetShowSteps(true);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderNode> node = RenderSlider::Create();
    node->Attach(mockContext);
    node->Update(slider);
    LayoutParam layoutParam;
    layoutParam.SetMaxSize(Size(500.0, 200.0));
    node->SetLayoutParam(layoutParam);
    EXPECT_TRUE(node->NeedLayout());
    EXPECT_TRUE(node->GetChildren().size() == 3);
}

/**
 * @tc.name: RenderSliderTest002
 * @tc.desc: Verify PerformLayout can calculate slider properties.
 * @tc.type: FUNC
 * @tc.require: AR000F3L3B
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderSliderTest, RenderSliderTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the slider component.
     */
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(0.0, 1.0, 0.0, 100.0);
    slider->SetStepValue(10.0);
    slider->SetShowTips(true);
    slider->SetShowSteps(true);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderNode> node = RenderSlider::Create();
    node->Attach(mockContext);
    node->Update(slider);
    LayoutParam layoutParam;
    layoutParam.SetMaxSize(Size(500.0, 200.0));
    node->SetLayoutParam(layoutParam);
    node->PerformLayout();
    EXPECT_TRUE(node->GetChildren().size() == 3);
    auto renderSlider = AceType::DynamicCast<RenderSlider>(node);
    ASSERT_TRUE(renderSlider);
    renderSlider->SyncValueToComponent(3.0);
    EXPECT_EQ(slider->GetValue(), 3.0);
}

/**
 * @tc.name: RenderSliderTest003
 * @tc.desc: Verify PerformLayout can calculate slider properties.
 * @tc.type: FUNC
 * @tc.require: AR000F3L3B
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderSliderTest, RenderSliderTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the slider component.
     */
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(0.0, 1.0, 0.0, 100.0);
    slider->SetStepValue(10.0);
    slider->SetShowTips(true);
    slider->SetShowSteps(true);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderNode> node = RenderSlider::Create();
    node->Attach(mockContext);
    node->Update(slider);
    LayoutParam layoutParam;
    layoutParam.SetMaxSize(Size(500.0, 200.0));
    node->SetLayoutParam(layoutParam);
    node->PerformLayout();

    auto renderSlider = AceType::DynamicCast<RenderSlider>(node);
    ASSERT_TRUE(renderSlider);
    renderSlider->SetTotalRatio(0.3);
    for (auto item : renderSlider->GetChildren()) {
        if (AceType::DynamicCast<RenderTip>(item)) {
            if (!item->GetChildren().empty()) {
                auto text = AceType::DynamicCast<RenderText>(item->GetChildren().front());
                if (text) {
                    EXPECT_EQ(text->GetTextData(), "30%");
                }
            }
            break;
        }
    }
}

/**
 * @tc.name: RenderSliderTest004
 * @tc.desc: Verify PerformLayout can calculate slider properties.
 * @tc.type: FUNC
 * @tc.require: AR000F3CBT
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderSliderTest, RenderSliderTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the slider component.
     */
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(0.0, 1.0, 0.0, 100.0);
    slider->SetStepValue(10.0);
    slider->SetShowTips(true);
    slider->SetShowSteps(true);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderNode> node = RenderSlider::Create();
    node->Attach(mockContext);
    node->Update(slider);
    LayoutParam layoutParam;
    layoutParam.SetMaxSize(Size(500.0, 200.0));
    node->SetLayoutParam(layoutParam);
    node->PerformLayout();

    auto renderSlider = AceType::DynamicCast<RenderSlider>(node);
    ASSERT_TRUE(renderSlider);
    EXPECT_FALSE(renderSlider->NeedSmoothMoving());
    slider->SetSliderMode(SliderMode::INSET);
    renderSlider->Update(slider);
    EXPECT_TRUE(renderSlider->NeedSmoothMoving());
}

/**
 * @tc.name: RenderSliderTest005
 * @tc.desc: Verify PerformLayout can calculate slider properties.
 * @tc.type: FUNC
 * @tc.require: AR000F3CBU
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderSliderTest, RenderSliderTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the slider component.
     */
    RefPtr<SliderComponent> slider = AceType::MakeRefPtr<SliderComponent>(0.0, 1.0, 0.0, 100.0);
    slider->SetMinValue(5.0);
    slider->SetMaxValue(20.0);
    slider->SetCurrentValue(30.0);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderNode> node = RenderSlider::Create();
    node->Attach(mockContext);
    node->Update(slider);
    auto renderSlider = AceType::DynamicCast<RenderSlider>(node);
    ASSERT_TRUE(renderSlider);
    EXPECT_EQ(renderSlider->GetValue(), 20.0);

    slider->SetCurrentValue(3.0);
    node->Update(slider);
    EXPECT_EQ(renderSlider->GetValue(), 5.0);
}

} // namespace OHOS::Ace
