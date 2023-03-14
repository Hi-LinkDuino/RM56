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

#include "core/components/common/rotation/rotation_node.h"
#include "core/pipeline/base/render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

const double ROTATION_VALUE = 200.0;

class MockRotationRenderNode final : public RenderNode, public RotationNode {
    DECLARE_ACE_TYPE(MockRotationRenderNode, RenderNode, RotationNode);

public:
    explicit MockRotationRenderNode(bool accept) : accept_(accept) {}
    ~MockRotationRenderNode() override = default;

    bool OnRotation(const RotationEvent& event) override;
    bool CheckResult(const RotationEvent& event);

    void Update(const RefPtr<Component>& component) override {}
    void PerformLayout() override {}

private:
    double value_ = 0.0;
    bool accept_ = false;
};

class MockRenderNode : public RenderNode {
public:
    void Update(const RefPtr<Component>& component) override {}
    void PerformLayout() override {}
};

bool MockRotationRenderNode::OnRotation(const RotationEvent& event)
{
    if (accept_) {
        value_ = event.value;
        return true;
    }
    return false;
}

bool MockRotationRenderNode::CheckResult(const RotationEvent& event)
{
    return (accept_ && event.value == value_);
}

class RotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RotationTest::SetUpTestCase() {}
void RotationTest::TearDownTestCase() {}
void RotationTest::SetUp() {}
void RotationTest::TearDown() {}

/**
 * @tc.name: RotationTest001
 * @tc.desc: Build render tree and dispatch rotation event to rotation node, then node accept it and check result.
 * @tc.type: FUNC
 */
HWTEST_F(RotationTest, RotationTest001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. build render tree and dispatch rotation event.
     */
    RefPtr<RenderNode> renderTree = AceType::MakeRefPtr<MockRenderNode>();
    renderTree->AddChild(AceType::MakeRefPtr<MockRenderNode>());
    auto rotationNode = AceType::MakeRefPtr<MockRotationRenderNode>(true);
    renderTree->AddChild(rotationNode);
    renderTree->AddChild(AceType::MakeRefPtr<MockRenderNode>());

    RotationEvent event { ROTATION_VALUE };
    renderTree->RotationTest(event);

    /**
     * @tc.steps: step2. Verify that the event is dispatched to rotation node.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(rotationNode->CheckResult(event));
}

/**
 * @tc.name: RotationTest002
 * @tc.desc: Build render tree and dispatch rotation event to rotation node, then node refuse it and check result.
 * @tc.type: FUNC
 */
HWTEST_F(RotationTest, RotationTest002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. build render tree and dispatch rotation event.
     */
    RefPtr<RenderNode> renderTree = AceType::MakeRefPtr<MockRenderNode>();
    renderTree->AddChild(AceType::MakeRefPtr<MockRenderNode>());
    auto rotationNode = AceType::MakeRefPtr<MockRotationRenderNode>(false);
    renderTree->AddChild(rotationNode);
    renderTree->AddChild(AceType::MakeRefPtr<MockRenderNode>());

    RotationEvent event { ROTATION_VALUE };
    renderTree->RotationTest(event);

    /**
     * @tc.steps: step2. Verify that the event is dispatched to rotation node.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_FALSE(rotationNode->CheckResult(event));
}

/**
 * @tc.name: RotationTest003
 * @tc.desc: Build render tree and dispatch rotation event to rotation node, then third node accept it and check result.
 * @tc.type: FUNC
 */
HWTEST_F(RotationTest, RotationTest003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. build render tree and dispatch rotation event.
     */
    RefPtr<RenderNode> renderTree = AceType::MakeRefPtr<MockRenderNode>();
    auto secondNode = AceType::MakeRefPtr<MockRotationRenderNode>(true);
    auto thirdNode = AceType::MakeRefPtr<MockRotationRenderNode>(true);
    renderTree->AddChild(secondNode);
    secondNode->AddChild(thirdNode);

    RotationEvent event { ROTATION_VALUE };
    renderTree->RotationTest(event);

    /**
     * @tc.steps: step2. Verify that the event is dispatched to rotation node.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_FALSE(secondNode->CheckResult(event));
    ASSERT_TRUE(thirdNode->CheckResult(event));
}

/**
 * @tc.name: RotationTest004
 * @tc.desc: Build render tree and dispatch rotation event to rotation node, then third node refuse it and check result.
 * @tc.type: FUNC
 */
HWTEST_F(RotationTest, RotationTest004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. build render tree and dispatch rotation event.
     */
    RefPtr<RenderNode> renderTree = AceType::MakeRefPtr<MockRenderNode>();
    auto secondNode = AceType::MakeRefPtr<MockRotationRenderNode>(true);
    auto thirdNode = AceType::MakeRefPtr<MockRotationRenderNode>(false);
    renderTree->AddChild(secondNode);
    secondNode->AddChild(thirdNode);

    RotationEvent event { ROTATION_VALUE };
    renderTree->RotationTest(event);

    /**
     * @tc.steps: step2. Verify that the event is dispatched to rotation node.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(secondNode->CheckResult(event));
    ASSERT_FALSE(thirdNode->CheckResult(event));
}
} // namespace OHOS::Ace