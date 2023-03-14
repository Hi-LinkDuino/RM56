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
#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/test/unittest/display/display_test_utils.h"
#include "core/components/text/text_component.h"

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

const uint8_t OPACITY = 128;
const uint8_t OPACITY_MIN = 0;
const uint8_t OPACITY_MAX = 255;
const double BOX_WIDTH = 100;
const double BOX_HEIGHT = 200;
const Size LAYOUT_SIZE = Size(100, 200);
const Size LAYOUT_SIZE_MIN = Size(0, 0);
const Offset OFFSET_VALUE = Offset(0, 0);

}

class RenderDisplayTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RenderDisplayUpdate001
 * @tc.desc: Verify the Update Interface of RenderDisplay work correctly with display component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DisplayComponent and RenderDisplay.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(text);
    display->SetVisible(VisibleType::VISIBLE);
    display->SetOpacity(0.0);
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDisplay.
     * @tc.expected: step2. renderDisplay is set as needLayout, visible and opacity is set correctly.
     */
    renderDisplay->Update(display);
    EXPECT_TRUE(renderDisplay->NeedLayout());
    EXPECT_EQ(renderDisplay->GetVisibleType(), VisibleType::VISIBLE);
    EXPECT_EQ(renderDisplay->GetOpacity(), OPACITY_MIN);
}

/**
 * @tc.name: RenderDisplayUpdate002
 * @tc.desc: Verify the Update Interface of RenderDisplay work correctly with display component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DisplayComponent and RenderDisplay.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(text);
    display->SetVisible(VisibleType::INVISIBLE);
    display->SetOpacity(0.5);
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDisplay.
     * @tc.expected: step2. renderDisplay is set as needLayout, visible and opacity is set correctly.
     */
    renderDisplay->Update(display);
    EXPECT_TRUE(renderDisplay->NeedLayout());
    EXPECT_EQ(renderDisplay->GetVisibleType(), VisibleType::INVISIBLE);
    EXPECT_EQ(renderDisplay->GetOpacity(), OPACITY);
}

/**
 * @tc.name: RenderDisplayUpdate003
 * @tc.desc: Verify the Update Interface of RenderDisplay work correctly with display component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayUpdate003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DisplayComponent and RenderDisplay.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(text);
    display->SetVisible(VisibleType::GONE);
    display->SetOpacity(1.0);
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDisplay.
     * @tc.expected: step2. renderDisplay is set as needLayout, visible and opacity is set correctly.
     */
    renderDisplay->Update(display);
    EXPECT_TRUE(renderDisplay->NeedLayout());
    EXPECT_EQ(renderDisplay->GetVisibleType(), VisibleType::GONE);
    EXPECT_EQ(renderDisplay->GetOpacity(), OPACITY_MAX);
}

/**
 * @tc.name: RenderDisplayUpdate004
 * @tc.desc: Verify the Update Interface of RenderDisplay work correctly with display component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayUpdate004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DisplayComponent and RenderDisplay.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(text);
    display->SetVisible(VisibleType::VISIBLE);
    display->SetOpacity(-1.0);
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDisplay.
     * @tc.expected: step2. renderDisplay is set as needLayout, visible is set correctly,
     *                      opacity is set as min.
     */
    renderDisplay->Update(display);
    EXPECT_TRUE(renderDisplay->NeedLayout());
    EXPECT_EQ(renderDisplay->GetVisibleType(), VisibleType::VISIBLE);
    EXPECT_EQ(renderDisplay->GetOpacity(), OPACITY_MIN);
}

/**
 * @tc.name: RenderDisplayUpdate005
 * @tc.desc: Verify the Update Interface of RenderDisplay work correctly with display component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayUpdate005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DisplayComponent and RenderDisplay.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>(text);
    display->SetVisible(VisibleType::VISIBLE);
    display->SetOpacity(2.0);
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDisplay.
     * @tc.expected: step2. renderDisplay is set as needLayout, visible is set correctly,
     *                      opacity is set as max.
     */
    renderDisplay->Update(display);
    EXPECT_TRUE(renderDisplay->NeedLayout());
    EXPECT_EQ(renderDisplay->GetVisibleType(), VisibleType::VISIBLE);
    EXPECT_EQ(renderDisplay->GetOpacity(), OPACITY_MAX);
}

/**
 * @tc.name: RenderDisplayPerformLayout001
 * @tc.desc: Verify PerformLayout interface of RenderDisplay works correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayPerformLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderDisplay and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderDisplay> display = DisplayTestUtils::CreateRenderDisplay(VisibleType::VISIBLE, 1.0);
    display->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    box->Attach(mockContext);
    display->AddChild(box);
    root->AddChild(display);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize of display is equal to child's size.
     */
    root->PerformLayout();
    EXPECT_EQ(display->GetLayoutSize(), LAYOUT_SIZE);
}

/**
 * @tc.name: RenderDisplayPerformLayout002
 * @tc.desc: Verify PerformLayout interface of RenderDisplay works correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayPerformLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderDisplay and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderDisplay> display = DisplayTestUtils::CreateRenderDisplay(VisibleType::INVISIBLE, 1.0);
    display->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    box->Attach(mockContext);
    display->AddChild(box);
    root->AddChild(display);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize of display is equal to child's size.
     */
    root->PerformLayout();
    EXPECT_EQ(display->GetLayoutSize(), LAYOUT_SIZE);
}

/**
 * @tc.name: RenderDisplayPerformLayout003
 * @tc.desc: Verify PerformLayout interface of RenderDisplay works correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayPerformLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderDisplay and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderDisplay> display = DisplayTestUtils::CreateRenderDisplay(VisibleType::GONE, 1.0);
    display->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    box->Attach(mockContext);
    display->AddChild(box);
    root->AddChild(display);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize of display is empty.
     */
    root->PerformLayout();
    EXPECT_EQ(display->GetLayoutSize(), LAYOUT_SIZE_MIN);
}

/**
 * @tc.name: RenderDisplayPerformLayout004
 * @tc.desc: Verify PerformLayout interface of RenderDisplay works correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayPerformLayout004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderFlex and RenderDisplay.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        DisplayTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    RefPtr<RenderDisplay> display  = DisplayTestUtils::CreateRenderDisplay(VisibleType::GONE, 1.0);
    display->Attach(mockContext);
    RefPtr<RenderBox> displayBox = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    displayBox->Attach(mockContext);
    display->AddChild(displayBox);
    row->AddChild(display);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    box->Attach(mockContext);
    row->AddChild(box);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize of display is empty, position of box is (0, 0).
     */
    root->PerformLayout();
    EXPECT_EQ(display->GetLayoutSize(), LAYOUT_SIZE_MIN);
    EXPECT_EQ(box->GetPosition(), OFFSET_VALUE);
}

/**
 * @tc.name: RenderDisplayPerformLayout005
 * @tc.desc: Verify PerformLayout interface of RenderDisplay works correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDisplayTest, RenderDisplayPerformLayout005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderFlex and RenderDisplay.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
            DisplayTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderDisplay> display  = DisplayTestUtils::CreateRenderDisplay(VisibleType::GONE, 1.0);
    display->Attach(mockContext);
    RefPtr<RenderBox> displayBox = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    displayBox->Attach(mockContext);
    display->AddChild(displayBox);
    column->AddChild(display);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    box->Attach(mockContext);
    column->AddChild(box);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize of display is empty, position of box is (0, 0).
     */
    root->PerformLayout();
    EXPECT_EQ(display->GetLayoutSize(), LAYOUT_SIZE_MIN);
    EXPECT_EQ(box->GetPosition(), OFFSET_VALUE);
}

} // namespace OHOS::Ace