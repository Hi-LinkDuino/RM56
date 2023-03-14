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

#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/test/unittest/box/box_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double BOX_WIDTH = 500.0;
const double BOX_HEIGHT = 500.0;

} // namespace

class MouseHoverTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "MouseHoverTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "MouseHoverTest TearDownTestCase";
    }

    void SetUp() override {}

    void TearDown() override {}
};

/**
 * @tc.name: MouseHoverTest001
 * @tc.desc: test mouse enter callback.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverTest, MouseHoverTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverTest MouseHoverTest001";
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox and RenderRoot.
     * @tc.expected: step2. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<MockRenderBoxBase> renderBox = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);

    /**
     * @tc.steps: step3. Verify that the mouse enter callback called correctly.
     * @tc.expected: step3. mouse enter callback called correctly.
     */
    renderRoot->PerformLayout();
    renderBox->MouseHoverTest(Point(100.0, 100.0));
    EXPECT_EQ(renderBox->enterCallBackCount_, 1);
    EXPECT_EQ(renderBox->exitCallBackCount_, 0);
}

/**
 * @tc.name: MouseHoverTest002
 * @tc.desc: test mouse illegal exit callback.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverTest, MouseHoverTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverTest MouseHoverTest002";
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox and RenderRoot.
     * @tc.expected: step2. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<MockRenderBoxBase> renderBox = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);

    /**
     * @tc.steps: step3. Verify that the mouse exit callback called wrong.
     * @tc.expected: step3. mouse exit callback called wrong.
     */
    renderRoot->PerformLayout();
    renderBox->MouseHoverTest(Point(600.0, 600.0));
    EXPECT_EQ(renderBox->enterCallBackCount_, 0);
    EXPECT_EQ(renderBox->exitCallBackCount_, 0);
}

/**
 * @tc.name: MouseHoverTest003
 * @tc.desc: test mouse illegal enter callback.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverTest, MouseHoverTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverTest MouseHoverTest003";
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox and RenderRoot.
     * @tc.expected: step2. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<MockRenderBoxBase> renderBox = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);

    /**
     * @tc.steps: step3. Verify that the mouse exit callback called correctly.
     * @tc.expected: step3. mouse exit callback called correctly.
     */
    renderRoot->PerformLayout();
    renderBox->MouseHoverTest(Point(100.0, 100.0));
    renderBox->MouseHoverTest(Point(600.0, 600.0));
    EXPECT_EQ(renderBox->enterCallBackCount_, 1);
    EXPECT_EQ(renderBox->exitCallBackCount_, 1);
}

/**
 * @tc.name: MouseHoverTest004
 * @tc.desc: test mouse exit callback.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverTest, MouseHoverTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverTest MouseHoverTest004";
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox and RenderRoot.
     * @tc.expected: step2. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<MockRenderBoxBase> renderBox = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);

    /**
     * @tc.steps: step3. Verify that the mouse enter callback called wrong.
     * @tc.expected: step3. mouse enter callback called wrong.
     */
    renderRoot->PerformLayout();
    renderBox->MouseHoverTest(Point(100.0, 100.0));
    renderBox->MouseHoverTest(Point(200.0, 200.0));
    EXPECT_EQ(renderBox->enterCallBackCount_, 1);
    EXPECT_EQ(renderBox->exitCallBackCount_, 0);
}

} // namespace OHOS::Ace