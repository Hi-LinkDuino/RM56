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

#include "core/components/progress/loading_progress_component.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/progress/progress_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderProgressTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<MockRenderLoadingProgress> renderNode_;
    int32_t index_ = 0;
};

void RenderProgressTest::SetUpTestCase() {}
void RenderProgressTest::TearDownTestCase() {}

void RenderProgressTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderNode_ = AceType::MakeRefPtr<MockRenderLoadingProgress>();
    renderNode_->Attach(mockContext_);
}

void RenderProgressTest::TearDown()
{
    mockContext_ = nullptr;
    renderNode_ = nullptr;
}

/**
 * @tc.name: RenderProgressTest001
 * @tc.desc: Verify the default value for loading progress.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAF
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderProgressTest, RenderProgressTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct loading progress component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<LoadingProgressComponent> loading = AceType::MakeRefPtr<LoadingProgressComponent>();
    ASSERT_TRUE(loading->GetDiameter() == Dimension());
    ASSERT_TRUE(loading->GetProgressColor() == Color(0xff666666));
    ASSERT_TRUE(NearEqual(loading->GetMoveRatio(), 0.06));
    ASSERT_TRUE(NearEqual(loading->GetRingRadius().Value(), 10.5));
    ASSERT_TRUE(NearEqual(loading->GetOrbitRadius().Value(), 17.0));
    ASSERT_TRUE(NearEqual(loading->GetCometTailLen(), 60.0));
}

/**
 * @tc.name: RenderProgressTest002
 * @tc.desc: Verify the set value for loading progress.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAG
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderProgressTest, RenderProgressTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct loading progress component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<LoadingProgressComponent> loading = AceType::MakeRefPtr<LoadingProgressComponent>();
    loading->SetDiameter(Dimension(100.0, DimensionUnit::PX));
    loading->SetProgressColor(Color(0xff111111));
    loading->SetMoveRatio(0.05);
    loading->SetRingRadius(Dimension(50.0));
    loading->SetOrbitRadius(Dimension(80.0));
    loading->SetCometTailLen(50.0);

    /**
     * @tc.steps: step2. Update the component to render node and trggier layout.
     * @tc.expected: step2. properties are Calculate correctly.
     */
    renderNode_->Update(loading);
    ASSERT_TRUE(renderNode_->CheckLoadingMode(MODE_LOOP));
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckDiameter(100.0));
    ASSERT_TRUE(renderNode_->CheckCenter(Offset(50.0, 50.0)));
    ASSERT_TRUE(renderNode_->CheckScale(100.0 / ((80.0 + renderNode_->GetCometRadius()) * 2.0)));
    ASSERT_TRUE(renderNode_->CheckMoveRatio(0.05));
    ASSERT_TRUE(renderNode_->CheckRingRadius(50.0));
    ASSERT_TRUE(renderNode_->CheckOrbitRadius(80.0));
    ASSERT_TRUE(renderNode_->CheckCometTailLen(50.0));
    ASSERT_TRUE(renderNode_->CheckProgressColor(Color(0xff111111)));
}

/**
 * @tc.name: RenderProgressTest003
 * @tc.desc: Verify the layout size for loading progress.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAH
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderProgressTest, RenderProgressTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct loading progress component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<LoadingProgressComponent> loading = AceType::MakeRefPtr<LoadingProgressComponent>();
    loading->SetDiameter(Dimension(200.0, DimensionUnit::PX));

    /**
     * @tc.steps: step2. Update the layout param to render node and trggier layout.
     * @tc.expected: step2. properties are Calculate correctly.
     */
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckDiameter(200.0));
    ASSERT_TRUE(renderNode_->CheckCenter(Offset(100.0, 100.0)));
    loading->SetDiameter(Dimension(0.0, DimensionUnit::PX));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckDiameter(0.0));
    ASSERT_TRUE(renderNode_->CheckCenter(Offset(36.0, 36.0)));
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderNode_->SetLayoutParam(layoutParam);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckDiameter(0.0));
    ASSERT_TRUE(renderNode_->CheckCenter(Offset(500.0, 500.0)));
}

/**
 * @tc.name: RenderProgressTest004
 * @tc.desc: Verify layout size effect the loading progress size.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAG
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderProgressTest, RenderProgressTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct loading progress component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<LoadingProgressComponent> loading = AceType::MakeRefPtr<LoadingProgressComponent>();
    loading->SetRingRadius(Dimension(10.5, DimensionUnit::VP));
    loading->SetOrbitRadius(Dimension(17.0, DimensionUnit::VP));

    /**
     * @tc.steps: step2. Update the component with MODE_SMALL and trggier layout.
     * @tc.expected: step2. properties are Calculate correctly.
     */
    loading->SetDiameter(Dimension(16.0, DimensionUnit::VP));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckCometRadius(3.0));
    ASSERT_TRUE(renderNode_->CheckRingWidth(2.8));
    ASSERT_TRUE(renderNode_->CheckRingBlurRadius(0.5));
    ASSERT_TRUE(renderNode_->CheckRingBgWidth(3.0));
    ASSERT_TRUE(renderNode_->CheckRingBgBlurRadius(2.0));

    /**
     * @tc.steps: step3. Update the component with MODE_MIDDLE and trggier layout.
     * @tc.expected: step3. properties are Calculate correctly.
     */
    loading->SetDiameter(Dimension(40.0, DimensionUnit::VP));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckCometRadius(3.0));
    ASSERT_TRUE(renderNode_->CheckRingWidth(1.9));
    ASSERT_TRUE(renderNode_->CheckRingBlurRadius(0.2));
    ASSERT_TRUE(renderNode_->CheckRingBgWidth(3.0));
    ASSERT_TRUE(renderNode_->CheckRingBgBlurRadius(2.0));

    /**
     * @tc.steps: step4. Update the component with MODE_LARGE and trggier layout.
     * @tc.expected: step4. properties are Calculate correctly.
     */
    loading->SetDiameter(Dimension(76.0, DimensionUnit::VP));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckCometRadius(2.2));
    ASSERT_TRUE(renderNode_->CheckRingWidth(1.2));
    ASSERT_TRUE(renderNode_->CheckRingBlurRadius(0.1));
    ASSERT_TRUE(renderNode_->CheckRingBgWidth(2.0));
    ASSERT_TRUE(renderNode_->CheckRingBgBlurRadius(2.0));
}

/**
 * @tc.name: RenderProgressTest005
 * @tc.desc: Verify layout size effect the loading progress size.
 * @tc.type: FUNC
 * @tc.require: AR000F3CAG
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderProgressTest, RenderProgressTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct loading progress component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<LoadingProgressComponent> loading = AceType::MakeRefPtr<LoadingProgressComponent>();
    loading->SetRingRadius(Dimension(10.5, DimensionUnit::VP));
    loading->SetOrbitRadius(Dimension(17.0, DimensionUnit::VP));

    /**
     * @tc.steps: step2. Update the component (MODE_SMALL - MODE_MIDDLE) and trggier layout.
     * @tc.expected: step2. properties are Calculate correctly.
     */
    loading->SetDiameter(Dimension(28.0, DimensionUnit::VP));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckCometRadius(3.0));
    ASSERT_TRUE(renderNode_->CheckRingWidth(2.35));
    ASSERT_TRUE(renderNode_->CheckRingBlurRadius(0.35));
    ASSERT_TRUE(renderNode_->CheckRingBgWidth(3.0));
    ASSERT_TRUE(renderNode_->CheckRingBgBlurRadius(2.0));

    /**
     * @tc.steps: step3. Update the component with (MODE_MIDDLE - MODE_LARGE) and trggier layout.
     * @tc.expected: step3. properties are Calculate correctly.
     */
    loading->SetDiameter(Dimension(58.0, DimensionUnit::VP));
    renderNode_->Update(loading);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckCometRadius(2.6));
    ASSERT_TRUE(renderNode_->CheckRingWidth(1.55));
    ASSERT_TRUE(renderNode_->CheckRingBlurRadius(0.15));
    ASSERT_TRUE(renderNode_->CheckRingBgWidth(2.5));
    ASSERT_TRUE(renderNode_->CheckRingBgBlurRadius(2.0));
}

} // namespace OHOS::Ace
