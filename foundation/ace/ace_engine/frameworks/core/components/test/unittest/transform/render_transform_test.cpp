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
#include "base/json/json_util.h"
#include "core/components/button/button_component.h"
#include "core/components/button/render_button.h"
#include "core/components/test/unittest/display/display_test_utils.h"
#include "core/components/text/text_component.h"
#include "core/components/transform/render_transform.h"
#include "core/components/transform/transform_component.h"

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

const Offset RESULT_ORIGIN_CENTER = Offset(75, 50);
const Offset RESULT_ORIGIN_MIX = Offset(50, 40);
const Offset RESULT_ORIGIN_TEST = Offset(25, 20);
const Offset RESULT_ORIGIN_PERCENT = Offset(60, 60);

const DimensionOffset TRANSFORM_ORIGIN_PERCENT =
    DimensionOffset(Dimension(0.4, DimensionUnit::PERCENT), Dimension(0.6, DimensionUnit::PERCENT));
const DimensionOffset TRANSFORM_ORIGIN_MIX =
    DimensionOffset(Dimension(50, DimensionUnit::PX), Dimension(0.4, DimensionUnit::PERCENT));
const DimensionOffset TRANSFORM_ORIGIN_TEST = DimensionOffset(Offset(25, 20));
const DimensionOffset TRANSFORM_ORIGIN_CENTER = DimensionOffset(Offset(75, 50));
const Size TRANSFORM_TEST_SIZE_BUTTON = Size(150, 100);
const Size TRANSFORM_TEST_SIZE_DEFAULT = Size(0, 0);
const Size TRANSFORM_TEST_SIZE_MAX = Size(600, 600);
const Dimension BUTTON_SIZE = Dimension(10.0, DimensionUnit::FP);

class MockRenderButton final : public RenderButton {
public:
    MockRenderButton() = default;
    ~MockRenderButton() override = default;
    Size Measure() override;
};

Size MockRenderButton::Measure()
{
    return TRANSFORM_TEST_SIZE_BUTTON;
}

class MockRenderTranform final : public RenderTransform {
public:
    MockRenderTranform() = default;
    ~MockRenderTranform() override = default;
    Offset GetOrigin() const
    {
        return origin_;
    }

    Matrix4 GetTransform() const
    {
        return transform_;
    }
};

class RenderTransformTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderTransformTest::SetUpTestCase() {}
void RenderTransformTest::TearDownTestCase() {}
void RenderTransformTest::SetUp() {}
void RenderTransformTest::TearDown() {}

/**
 * @tc.name: RenderTransformTest001
 * @tc.desc: Verify PerformLayout can calculate transform properties with a child where tansfrom origin set in PX
 * and transform origin set to (25,25).
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderTransformTest, RenderTransformTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a button child.
     */
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    buttonChildren.emplace_back(text);
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    transform->SetChild(button);
    transform->SetOriginDimension(TRANSFORM_ORIGIN_TEST);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderButton> renderButton = AceType::MakeRefPtr<MockRenderButton>();
    renderButton->Attach(mockContext);
    renderButton->Update(button);
    renderTransform->AddChild(renderButton);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());

    renderTransform->PerformLayout();
    renderTransform->UpdateTransformOrigin();
    ASSERT_TRUE(renderTransform->GetOrigin() == RESULT_ORIGIN_TEST);
}

/**
 * @tc.name: RenderTransformTest002
 * @tc.desc: Verify PerformLayout can calculate transform properties with a child where tansform origin is default
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderTransformTest, RenderTransformTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a button child.
     */
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    buttonChildren.emplace_back(text);
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetWidth(BUTTON_SIZE);
    button->SetHeight(BUTTON_SIZE);
    transform->SetChild(button);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderButton> renderButton = AceType::MakeRefPtr<MockRenderButton>();
    renderButton->Attach(mockContext);
    renderButton->Update(button);
    renderTransform->AddChild(renderButton);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());

    renderTransform->PerformLayout();
    renderTransform->UpdateTransformOrigin();
    ASSERT_TRUE(renderTransform->GetOrigin() == RESULT_ORIGIN_CENTER);
}

/**
 * @tc.name: RenderTransformTest003
 * @tc.desc: Verify PerformLayout can calculate transform properties with a child where origin is set by percent
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderTransformTest, RenderTransformTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a button child.
     */
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    buttonChildren.emplace_back(text);
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetWidth(BUTTON_SIZE);
    button->SetHeight(BUTTON_SIZE);
    transform->SetChild(button);
    transform->SetOriginDimension(TRANSFORM_ORIGIN_PERCENT);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderButton> renderButton = AceType::MakeRefPtr<MockRenderButton>();
    renderButton->Attach(mockContext);
    renderButton->Update(button);
    renderTransform->AddChild(renderButton);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());

    renderTransform->PerformLayout();
    renderTransform->UpdateTransformOrigin();
    ASSERT_TRUE(renderTransform->GetOrigin() == RESULT_ORIGIN_PERCENT);
}

/**
 * @tc.name: RenderTransformTest004
 * @tc.desc: Verify PerformLayout can calculate transform properties with a child where transform origin is set by px
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderTransformTest, RenderTransformTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of transform component with a button child.
     */
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    buttonChildren.emplace_back(text);
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetWidth(BUTTON_SIZE);
    button->SetHeight(BUTTON_SIZE);
    transform->SetChild(button);
    transform->SetOriginDimension(TRANSFORM_ORIGIN_MIX);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderButton> renderButton = AceType::MakeRefPtr<MockRenderButton>();
    renderButton->Attach(mockContext);
    renderButton->Update(button);
    renderTransform->AddChild(renderButton);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());

    renderTransform->PerformLayout();
    renderTransform->UpdateTransformOrigin();
    ASSERT_TRUE(renderTransform->GetOrigin() == RESULT_ORIGIN_MIX);
}

/**
 * @tc.name: RenderTransformTest005
 * @tc.desc: Verify Multi Rotate Effect can overlay to Matrix by transform Component.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct transform component with a box child.
     */
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    transform->RotateX(10.0f);
    transform->RotateY(20.0f);
    transform->RotateZ(30.0f);
    transform->RotateY(40.0f);
    transform->RotateZ(50.0f);
    transform->RotateX(60.0f);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 expect = Matrix4::CreateIdentity();
    expect = expect * Matrix4::CreateRotate(10.0, 1.0f, 0.0f, 0.0f); // RotateX(10.0)
    expect = expect * Matrix4::CreateRotate(20.0, 0.0f, 1.0f, 0.0f); // RotateY(20.0)
    expect = expect * Matrix4::CreateRotate(30.0, 0.0f, 0.0f, 1.0f); // RotateZ(30.0)
    expect = expect * Matrix4::CreateRotate(40.0, 0.0f, 1.0f, 0.0f); // RotateY(40.0)
    expect = expect * Matrix4::CreateRotate(50.0, 0.0f, 0.0f, 1.0f); // RotateZ(50.0)
    expect = expect * Matrix4::CreateRotate(60.0, 1.0f, 0.0f, 0.0f); // RotateX(60.0)
    ASSERT_TRUE(renderTransform->GetTransform() == expect);
}

/**
 * @tc.name: RenderTransformTest006
 * @tc.desc: Verify Multi Scale Effect can overlay to Matrix by transform Component.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct transform component with a box child.
     */
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    transform->Scale(0.5f, 0.5f);
    transform->ScaleX(1.5f);
    transform->ScaleY(2.5f);
    transform->ScaleX(0.5f);
    transform->ScaleY(0.8f);
    transform->Scale(5.0f, 6.0f);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 effect = renderTransform->GetTransform();
    ASSERT_TRUE(NearEqual(effect[0], 0.5 * 1.5 * 0.5 * 5.0));
    ASSERT_TRUE(NearEqual(effect[5], 0.5 * 2.5 * 0.8 * 6.0));
}

/**
 * @tc.name: RenderTransformTest007
 * @tc.desc: Verify Multi Translate Effect can overlay to Matrix by transform Component.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct transform component with a box child.
     */
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    transform->Translate(Dimension(100.0), Dimension(100.0));
    transform->TranslateX(Dimension(0.2, DimensionUnit::PERCENT));
    transform->TranslateY(Dimension(0.5, DimensionUnit::PERCENT));
    transform->TranslateX(Dimension(-50.0));
    transform->TranslateY(Dimension(-80.0));
    transform->Translate(Dimension(70.0), Dimension(-60.0));

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(200.0, 100.0));
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 effect = renderTransform->GetTransform();
    ASSERT_TRUE(NearEqual(effect[12], 100.0 + 0.2 * 200.0 - 50.0 + 70.0));
    ASSERT_TRUE(NearEqual(effect[13], 100.0 + 0.5 * 100.0 - 80.0 - 60.0));
}

/**
 * @tc.name: RenderTransformTest008
 * @tc.desc: Verify Rotate/Scale/Translate Effect can overlay to Matrix by transform Component.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct transform component with a box child.
     */
    RefPtr<TransformComponent> transform = AceType::MakeRefPtr<TransformComponent>();
    transform->RotateX(10.0f);
    transform->ScaleX(0.5f);
    transform->RotateY(20.0f);
    transform->TranslateX(Dimension(0.2, DimensionUnit::PERCENT));
    transform->RotateZ(30.0f);
    transform->ScaleY(0.8f);
    transform->TranslateY(Dimension(0.5, DimensionUnit::PERCENT));

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(transform);
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(200.0, 100.0));
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 expect = Matrix4::CreateIdentity();
    expect = expect * Matrix4::CreateRotate(10.0, 1.0f, 0.0f, 0.0f); // RotateX(10.0)
    expect = expect * Matrix4::CreateScale(0.5f, 1.0f, 1.0f);        // ScaleX(0.5)
    expect = expect * Matrix4::CreateRotate(20.0, 0.0f, 1.0f, 0.0f); // RotateY(20.0)
    expect = expect * Matrix4::CreateTranslate(40.0f, 0.0f, 0.0f);   // TranslateX(20%)
    expect = expect * Matrix4::CreateRotate(30.0, 0.0f, 0.0f, 1.0f); // RotateZ(30.0)
    expect = expect * Matrix4::CreateScale(1.0f, 0.8f, 1.0f);        // ScaleY(0.8)
    expect = expect * Matrix4::CreateTranslate(0.0f, 50.0f, 0.0f);   // TranslateY(50%)
    ASSERT_TRUE(renderTransform->GetTransform() == expect);

    /**
     * @tc.steps: step3. Reset the transform component and update to render node.
     * @tc.expected: step3. Check the render node Matrix is the Identity matrix.
     */
    transform->ResetTransform();
    renderTransform->Update(transform);
    renderTransform->UpdateTransform();
    expect = Matrix4::CreateIdentity();
    ASSERT_TRUE(renderTransform->GetTransform() == expect);
}

/**
 * @tc.name: RenderTransformTest009
 * @tc.desc: Verify Multi Rotate Effect can overlay to Matrix by render transform.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create render transform and add multi rotate Effect.
     * @tc.expected: step1. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(AceType::MakeRefPtr<TransformComponent>());
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    renderTransform->RotateX(10.0f);
    renderTransform->RotateY(20.0f);
    renderTransform->RotateZ(30.0f);
    renderTransform->RotateY(40.0f);
    renderTransform->RotateZ(50.0f);
    renderTransform->RotateX(60.0f);

    /**
     * @tc.steps: step2. Trigger PerformLayout and UpdateTransform.
     * @tc.expected: step2. The multi rotate Effect set to Matrix correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 expect = Matrix4::CreateIdentity();
    expect = expect * Matrix4::CreateRotate(10.0, 1.0f, 0.0f, 0.0f); // RotateX(10.0)
    expect = expect * Matrix4::CreateRotate(20.0, 0.0f, 1.0f, 0.0f); // RotateY(20.0)
    expect = expect * Matrix4::CreateRotate(30.0, 0.0f, 0.0f, 1.0f); // RotateZ(30.0)
    expect = expect * Matrix4::CreateRotate(40.0, 0.0f, 1.0f, 0.0f); // RotateY(40.0)
    expect = expect * Matrix4::CreateRotate(50.0, 0.0f, 0.0f, 1.0f); // RotateZ(50.0)
    expect = expect * Matrix4::CreateRotate(60.0, 1.0f, 0.0f, 0.0f); // RotateX(60.0)
    ASSERT_TRUE(renderTransform->GetTransform() == expect);
}

/**
 * @tc.name: RenderTransformTest010
 * @tc.desc: Verify Multi Scale Effect can overlay to Matrix by render transform.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create render transform and add multi scale Effect.
     * @tc.expected: step1. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(AceType::MakeRefPtr<TransformComponent>());
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);
    renderTransform->Scale(0.5f);       // X:0.5 Y:0.5
    renderTransform->Scale(1.5f, 1.0f); // X:1.5
    renderTransform->Scale(1.0f, 2.5f); // Y:2.5
    renderTransform->Scale(0.5f, 1.0f); // X:0.5
    renderTransform->Scale(1.0f, 0.8f); // Y:0.8
    renderTransform->Scale(5.0f, 6.0f); // X:5.0 Y:6.0

    /**
     * @tc.steps: step2. Trigger PerformLayout and UpdateTransform.
     * @tc.expected: step2. The multi Scale Effect set to Matrix correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(TRANSFORM_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(TRANSFORM_TEST_SIZE_MAX);
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->UpdateTransform();
    Matrix4 effect = renderTransform->GetTransform();
    ASSERT_TRUE(NearEqual(effect[0], 0.5 * 1.5 * 0.5 * 5.0));
    ASSERT_TRUE(NearEqual(effect[5], 0.5 * 2.5 * 0.8 * 6.0));
}

/**
 * @tc.name: RenderTransformTest011
 * @tc.desc: Verify Multi Translate Effect can overlay to Matrix by render transform.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create render transform and add multi translate Effect.
     * @tc.expected: step1. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(AceType::MakeRefPtr<TransformComponent>());
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);

    /**
     * @tc.steps: step2. Trigger PerformLayout and UpdateTransform.
     * @tc.expected: step2. The multi Translate Effect set to Matrix correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(200.0, 100.0));
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->Translate(Dimension(100.0), Dimension(100.0));
    renderTransform->Translate(Dimension(0.2, DimensionUnit::PERCENT), Dimension());
    renderTransform->Translate(Dimension(), Dimension(0.5, DimensionUnit::PERCENT));
    renderTransform->Translate(Dimension(-50.0), Dimension());
    renderTransform->Translate(Dimension(), Dimension(-80.0));
    renderTransform->Translate(Dimension(70.0), Dimension(-60.0));
    renderTransform->UpdateTransform();
    Matrix4 effect = renderTransform->GetTransform();
    ASSERT_TRUE(NearEqual(effect[12], 100.0 + 0.2 * 200.0 - 50.0 + 70.0));
    ASSERT_TRUE(NearEqual(effect[13], 100.0 + 0.5 * 100.0 - 80.0 - 60.0));
}

/**
 * @tc.name: RenderTransformTest012
 * @tc.desc: Verify Rotate/Scale/Translate Effect can overlay to Matrix by render transform.
 * and percent.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUJ
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderTransformTest, RenderTransformTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create render transform and add Rotate/Scale/Translate Effect.
     * @tc.expected: step1. Properties are calculated correctly.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<MockRenderTranform> renderTransform = AceType::MakeRefPtr<MockRenderTranform>();
    renderTransform->Attach(mockContext);
    renderTransform->Update(AceType::MakeRefPtr<TransformComponent>());
    RefPtr<RenderBox> renderBox = DisplayTestUtils::CreateRenderBox(200.0, 100.0);
    renderBox->Attach(mockContext);
    renderTransform->AddChild(renderBox);

    /**
     * @tc.steps: step2. Trigger PerformLayout and UpdateTransform.
     * @tc.expected: step2. The Rotate/Scale/Translate Effect set to Matrix correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(200.0, 100.0));
    renderTransform->SetLayoutParam(layoutParam);
    EXPECT_TRUE(renderTransform->NeedLayout());
    renderTransform->PerformLayout();
    renderTransform->RotateX(10.0f);
    renderTransform->Scale(0.5f, 1.0f);
    renderTransform->RotateY(20.0f);
    renderTransform->Translate(Dimension(0.2, DimensionUnit::PERCENT), Dimension());
    renderTransform->RotateZ(30.0f);
    renderTransform->Scale(1.0f, 0.8f);
    renderTransform->Translate(Dimension(), Dimension(0.5, DimensionUnit::PERCENT));
    renderTransform->UpdateTransform();
    Matrix4 expect = Matrix4::CreateIdentity();
    expect = expect * Matrix4::CreateRotate(10.0, 1.0f, 0.0f, 0.0f); // RotateX(10.0)
    expect = expect * Matrix4::CreateScale(0.5f, 1.0f, 1.0f);        // ScaleX(0.5)
    expect = expect * Matrix4::CreateRotate(20.0, 0.0f, 1.0f, 0.0f); // RotateY(20.0)
    expect = expect * Matrix4::CreateTranslate(40.0f, 0.0f, 0.0f);   // TranslateX(20%)
    expect = expect * Matrix4::CreateRotate(30.0, 0.0f, 0.0f, 1.0f); // RotateZ(30.0)
    expect = expect * Matrix4::CreateScale(1.0f, 0.8f, 1.0f);        // ScaleY(0.8)
    expect = expect * Matrix4::CreateTranslate(0.0f, 50.0f, 0.0f);   // TranslateY(50%)
    ASSERT_TRUE(renderTransform->GetTransform() == expect);

    /**
     * @tc.steps: step3. Reset the transform.
     * @tc.expected: step3. Check Matrix is Identity.
     */
    renderTransform->ResetTransform();
    renderTransform->UpdateTransform();
    expect = Matrix4::CreateIdentity();
    ASSERT_TRUE(renderTransform->GetTransform() == expect);
}

} // namespace OHOS::Ace
