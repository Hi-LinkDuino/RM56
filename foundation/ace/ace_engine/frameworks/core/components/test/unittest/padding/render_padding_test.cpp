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
#include "core/components/button/button_component.h"
#include "core/components/button/render_button.h"
#include "core/components/padding/padding_component.h"
#include "core/components/padding/render_padding.h"
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

const Offset PADDING_TEST_OFFSET = Offset(40, 50);
const Size PADDING_TEST_SIZE = Size(500, 500);
const Size PADDING_TEST_LAYOUT = Size(140, 200);
const Size PADDING_TEST_SIZE_DEFAULT = Size(0, 0);
const Size PADDING_TEST_SIZE_MAX = Size(600, 600);
const Size PADDING_TEST_SIZE_BUTTON = Size(40, 100);

class MockRenderButton final : public RenderButton {
public:
    MockRenderButton() = default;
    ~MockRenderButton() override = default;
    Size Measure() override;
};

Size MockRenderButton::Measure()
{
    return PADDING_TEST_SIZE_BUTTON;
}

class RenderPaddingTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RenderPaddingTest::SetUpTestCase() {}
void RenderPaddingTest::TearDownTestCase() {}
void RenderPaddingTest::SetUp() {}
void RenderPaddingTest::TearDown() {}

/**
 * @tc.name: RenderPaddingTest001
 * @tc.desc: Verify PerformLayout can calculate padding properties with a child.
 * @tc.type: FUNC
 */
HWTEST_F(RenderPaddingTest, RenderPaddingTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with padding and child.
     */
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    buttonChildren.emplace_back(text);
    RefPtr<PaddingComponent> padding = AceType::MakeRefPtr<PaddingComponent>();
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetHeight(Dimension(100.0));
    button->SetWidth(Dimension(40.0));
    padding->SetPadding(Edge(40.0, 50.0, 60.0, 50.0));
    padding->SetChild(button);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderPadding> renderPadding = AceType::MakeRefPtr<RenderPadding>();
    renderPadding->Attach(mockContext);
    renderPadding->Update(padding);
    RefPtr<RenderButton> renderButton = AceType::MakeRefPtr<MockRenderButton>();
    renderButton->Attach(mockContext);
    renderButton->Update(button);
    renderPadding->AddChild(renderButton);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(PADDING_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(PADDING_TEST_SIZE_MAX);
    renderPadding->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderPadding->NeedLayout());

    renderPadding->PerformLayout();
    ASSERT_TRUE(renderButton->GetLayoutParam().GetMaxSize() == PADDING_TEST_SIZE);
    ASSERT_TRUE(renderButton->GetPosition() == PADDING_TEST_OFFSET);
    ASSERT_TRUE(renderPadding->GetLayoutSize() == PADDING_TEST_LAYOUT);
}

} // namespace OHOS::Ace