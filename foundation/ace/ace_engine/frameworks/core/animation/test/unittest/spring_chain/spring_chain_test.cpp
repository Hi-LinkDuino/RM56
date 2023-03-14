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
#include "core/animation/simple_spring_adapter.h"
#include "core/animation/simple_spring_chain.h"
#include "core/animation/simple_spring_node.h"
#include "core/animation/spring_motion.h"
#include "core/animation/test/unittest/mock/animation_test_utils.h"
#include "core/animation/test/unittest/mock/spring_chain_item.h"
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

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

namespace {

constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t CHAIN_ITEM_COUNT = 10;
constexpr int32_t CHAIN_ITEM_DISTANCE_DEFAULT = 50;
constexpr double CONTROL_DAMPING_DEFAULT = 500.0;
constexpr double CONTROL_STIFFNESS_DEFAULT = 200.0;
constexpr float ERROR_BOUND = 0.01f;

} // namespace

class SpringChainTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SpringChainTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "SpringChainTest TearDownTestCase";
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

    void TearDown() override
    {
        nodes_.clear();
    }

    void InitSpringChain(int32_t chainItemCount, bool useLinearTransfer)
    {
        adapter_ = AceType::MakeRefPtr<SimpleSpringAdapter>();
        chain_ = AceType::MakeRefPtr<SimpleSpringChain>(adapter_);
        chain_->SetControlDamping(CONTROL_DAMPING_DEFAULT);
        chain_->SetControlStiffness(CONTROL_STIFFNESS_DEFAULT);
        if (useLinearTransfer) {
            chain_->SetDampingTransfer(AceType::MakeRefPtr<LinearParamTransfer>(0.0));
            chain_->SetStiffnessTransfer(AceType::MakeRefPtr<LinearParamTransfer>(1.0));
        }
        for (int32_t idx = 0; idx < chainItemCount; idx++) {
            RefPtr<SpringChainItem> item = AceType::MakeRefPtr<SpringChainItem>(idx);
            nodes_[idx] = item;
            RefPtr<SimpleSpringNode> node =
                AceType::MakeRefPtr<SimpleSpringNode>(context_, idx, idx * CHAIN_ITEM_DISTANCE_DEFAULT);
            node->AddUpdateListener([idx, item](double value, double velocity) {
                LOGI("Spring Node Update. index: %{public}d, value: %{public}lf, velocity: %{public}lf", idx, value,
                    velocity);
                item->SetValue(value);
                item->SetVelocity(velocity);
            });
            node->SetValue(idx * CHAIN_ITEM_DISTANCE_DEFAULT);
            adapter_->AddNode(node);
        }
    }

protected:
    RefPtr<PipelineContext> context_;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
    RefPtr<SimpleSpringAdapter> adapter_;
    RefPtr<SimpleSpringChain> chain_;
    std::unordered_map<int32_t, RefPtr<SpringChainItem>> nodes_;
};

/**
 * @tc.name: SpringChainTest001
 * @tc.desc: Test SpringChain init
 * @tc.type: FUNC
 */
HWTEST_F(SpringChainTest, SpringChainTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SpringChainTest SpringChainTest001 start";
    /**
     * @tc.steps: step1. init spring chain
     * @tc.expected: step1. check size and node index
     */
    InitSpringChain(CHAIN_ITEM_COUNT, false);
    EXPECT_EQ(CHAIN_ITEM_COUNT, adapter_->GetSize());
    for (int32_t idx = 0; idx < CHAIN_ITEM_COUNT; idx++) {
        auto node = adapter_->GetNode(idx);
        EXPECT_EQ(idx, node->GetIndex());
        EXPECT_EQ(idx * CHAIN_ITEM_DISTANCE_DEFAULT, nodes_[idx]->Value());
    }
}

/**
 * @tc.name: SpringChainTest002
 * @tc.desc: Test SpringChain default Param Transfer
 * @tc.type: FUNC
 */
HWTEST_F(SpringChainTest, SpringChainTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SpringChainTest SpringChainTest002 start";
    /**
     * @tc.steps: step1. init spring chain
     * @tc.expected: step1. check transfer params in chain node
     */
    InitSpringChain(CHAIN_ITEM_COUNT, false);
    auto controlNode = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetControlNode());

    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, controlNode->GetTransferParams()->Damping());
    auto node = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetNext(controlNode));
    EXPECT_EQ(1, node->GetIndex());
    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, node->GetTransferParams()->Damping());
    EXPECT_NEAR(176.54, node->GetTransferParams()->Stiffness(), ERROR_BOUND);
    node = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetNode(CHAIN_ITEM_COUNT - 1));
    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, node->GetTransferParams()->Damping());
    EXPECT_NEAR(132.13, node->GetTransferParams()->Stiffness(), ERROR_BOUND);
}

/**
 * @tc.name: SpringChainTest003
 * @tc.desc: Test SpringChain Linear Param Transfer
 * @tc.type: FUNC
 */
HWTEST_F(SpringChainTest, SpringChainTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SpringChainTest SpringChainTest003 start";
    /**
     * @tc.steps: step1. init spring chain with linear param transfer
     * @tc.expected: step1. check param in node with linear spread
     */
    InitSpringChain(CHAIN_ITEM_COUNT, true);
    auto controlNode = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetControlNode());

    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, controlNode->GetTransferParams()->Damping());
    auto node = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetNext(controlNode));
    EXPECT_EQ(1, node->GetIndex());
    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, node->GetTransferParams()->Damping());
    EXPECT_NEAR(199.0, node->GetTransferParams()->Stiffness(), ERROR_BOUND);
    node = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetNode(CHAIN_ITEM_COUNT - 1));
    EXPECT_EQ(CONTROL_DAMPING_DEFAULT, node->GetTransferParams()->Damping());
    EXPECT_NEAR(191.0, node->GetTransferParams()->Stiffness(), ERROR_BOUND);
}

/**
 * @tc.name: SpringChainTest004
 * @tc.desc: Test SpringChain Transfer Next
 * @tc.type: FUNC
 */
HWTEST_F(SpringChainTest, SpringChainTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SpringChainTest SpringChainTest004 start";
    /**
     * @tc.steps: step1. init spring chain
     */
    int32_t chainItemCount = 3;
    InitSpringChain(chainItemCount, false);

    /**
     * @tc.steps: step2. end to position
     * @tc.expected: step2. spring chain node moves
     */
    chain_->EndToPosition(20, -100);
    platformWindowRaw_->TriggerOneFrame();
    auto controlNode = AceType::DynamicCast<SimpleSpringNode>(adapter_->GetControlNode());
    EXPECT_NEAR(0.111, nodes_[adapter_->GetControlIndex()]->Value(), ERROR_BOUND);
}

} // namespace OHOS::Ace