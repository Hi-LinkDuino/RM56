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

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "base/log/log.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/display/display_component.h"
#include "core/components/display/display_element.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/flex/flex_item_element.h"
#include "core/components/flex/render_flex_item.h"
#include "core/components/stage/stage_component.h"
#include "core/components/stage/stage_element.h"

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

class StageElementTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "StageElementTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "StageElementTest TearDownTestCase";
    }

    void SetUp() override
    {
        RefPtr<StageComponent> rootStage = AceType::MakeRefPtr<StageComponent>(std::list<RefPtr<Component>>());
        stage_ = AceType::MakeRefPtr<StageElement>();
        stage_->SetPipelineContext(nullptr);
        stage_->SetNewComponent(rootStage);
        stage_->Mount(nullptr);
    }

    void TearDown() override
    {
        stage_ = nullptr;
    }

protected:
    RefPtr<StageElement> stage_;
};

/**
 * @tc.name: StagePushTest001
 * @tc.desc: Verify the push interface works fine when there is no children in stage
 * @tc.type: FUNC
 * @tc.require: AR000DACL6
 * @tc.author: yangfan
 */
HWTEST_F(StageElementTest, StagePushTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init new component
     */
    RefPtr<BoxComponent> testBox = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<DisplayComponent> testDisplay = AceType::MakeRefPtr<DisplayComponent>(testBox);

    /**
     * @tc.steps: step2. call PushPage and PerformBuild interfaces
     * @tc.expected: step2. the child of stage are created.
     */
    stage_->PushPage(testDisplay);
    stage_->PerformBuild();
    auto child = stage_->GetFirstChild();
    auto displayChild = AceType::DynamicCast<DisplayElement>(child);
    ASSERT_TRUE(displayChild != nullptr);
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(displayChild->GetRenderNode());
    ASSERT_TRUE(renderDisplay != nullptr);
    EXPECT_TRUE(renderDisplay->GetVisibleType() == VisibleType::VISIBLE);
}

/**
 * @tc.name: StagePushTest002
 * @tc.desc: Verify the push interface works fine when there are children in stage
 * @tc.type: FUNC
 * @tc.require: AR000DACL6
 * @tc.author: yangfan
 */
HWTEST_F(StageElementTest, StagePushTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init two new components
     */
    RefPtr<BoxComponent> firstBox = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<DisplayComponent> firstDisplay = AceType::MakeRefPtr<DisplayComponent>(firstBox);
    RefPtr<BoxComponent> secondBox = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<DisplayComponent> secondDisplay = AceType::MakeRefPtr<DisplayComponent>(secondBox);
    /**
     * @tc.steps: step2. call PushPage and PerformBuild interfaces
     * @tc.expected: step2. the first display child is set gone.
     */
    stage_->PushPage(firstDisplay);
    stage_->PerformBuild();
    stage_->PushPage(secondDisplay);
    stage_->PerformBuild();
    auto child = stage_->GetFirstChild();
    auto displayChild = AceType::DynamicCast<DisplayElement>(child);
    ASSERT_TRUE(displayChild);
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(displayChild->GetRenderNode());
    ASSERT_TRUE(renderDisplay != nullptr);
}

/**
 * @tc.name: StagePopTest001
 * @tc.desc: Verify the pop interface works fine when there are children in stage
 * @tc.type: FUNC
 * @tc.require: AR000DACL6
 * @tc.author: yangfan
 */
HWTEST_F(StageElementTest, StagePopTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init two new components
     */
    RefPtr<BoxComponent> firstBox = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<DisplayComponent> firstDisplay = AceType::MakeRefPtr<DisplayComponent>(firstBox);
    RefPtr<BoxComponent> secondBox = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<DisplayComponent> secondDisplay = AceType::MakeRefPtr<DisplayComponent>(secondBox);
    /**
     * @tc.steps: step2. call Push, Pop and PerformBuild interfaces
     * @tc.expected: step2. only one child in the stage, the first child is set visible.
     */
    stage_->PushPage(firstDisplay);
    stage_->PerformBuild();
    stage_->PushPage(secondDisplay);
    stage_->PerformBuild();
    stage_->Pop();
    stage_->PerformBuild();
    auto child = stage_->GetFirstChild();
    auto displayChild = AceType::DynamicCast<DisplayElement>(child);
    ASSERT_TRUE(displayChild);
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(displayChild->GetRenderNode());
    ASSERT_TRUE(renderDisplay != nullptr);
    EXPECT_TRUE(renderDisplay->GetVisibleType() == VisibleType::VISIBLE);
}

/**
 * @tc.name: StageReplaceTest001
 * @tc.desc: Verify the Replace interface works fine when there are children in stage
 * @tc.type: FUNC
 * @tc.require: AR000DACL6
 * @tc.author: yangfan
 */
HWTEST_F(StageElementTest, StageReplaceTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init two new components
     */
    RefPtr<FlexItemComponent> firstFlexItem = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0, nullptr);
    RefPtr<DisplayComponent> firstDisplay = AceType::MakeRefPtr<DisplayComponent>(firstFlexItem);
    RefPtr<FlexItemComponent> secondFlexItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 1, nullptr);
    RefPtr<DisplayComponent> secondDisplay = AceType::MakeRefPtr<DisplayComponent>(secondFlexItem);
    /**
     * @tc.steps: step2. call PushPage, replace and PerformBuild interfaces
     * @tc.expected: step2. there is only one child in the stage, the properties are correctly
     */
    stage_->PushPage(firstDisplay);
    stage_->PerformBuild();
    stage_->Replace(secondDisplay);
    stage_->PerformBuild();
    auto child = stage_->GetFirstChild();
    auto displayChild = AceType::DynamicCast<DisplayElement>(child);
    ASSERT_TRUE(displayChild);
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(displayChild->GetRenderNode());
    auto renderFlexItem = AceType::DynamicCast<RenderFlexItem>(renderDisplay->GetChildren().front());
    ASSERT_TRUE(renderDisplay != nullptr);
    ASSERT_TRUE(renderFlexItem != nullptr);
    EXPECT_TRUE(renderFlexItem->GetFlexGrow() == 1);
    EXPECT_TRUE(renderFlexItem->GetFlexShrink() == 1);
    EXPECT_TRUE(renderDisplay->GetVisibleType() == VisibleType::VISIBLE);
}

/**
 * @tc.name: StageClearTest001
 * @tc.desc: Verify the clear interface works fine when there are children in stage
 * @tc.type: FUNC
 * @tc.require: AR000DACL6
 * @tc.author: yangfan
 */
HWTEST_F(StageElementTest, StageClearTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init three new components
     */
    RefPtr<FlexItemComponent> firstFlexItem = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0, nullptr);
    RefPtr<DisplayComponent> firstDisplay = AceType::MakeRefPtr<DisplayComponent>(firstFlexItem);
    RefPtr<FlexItemComponent> secondFlexItem = AceType::MakeRefPtr<FlexItemComponent>(0, 0, 0, nullptr);
    RefPtr<DisplayComponent> secondDisplay = AceType::MakeRefPtr<DisplayComponent>(secondFlexItem);
    RefPtr<FlexItemComponent> thirdFlexItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 1, nullptr);
    RefPtr<DisplayComponent> thirdDisplay = AceType::MakeRefPtr<DisplayComponent>(thirdFlexItem);
    /**
     * @tc.steps: step2. call PushPage, clear and PerformBuild interfaces
     * @tc.expected: step2. there is only one child in the stage, the properties are set correctly
     */
    stage_->PushPage(firstDisplay);
    stage_->PerformBuild();
    stage_->PushPage(secondDisplay);
    stage_->PerformBuild();
    stage_->PushPage(thirdDisplay);
    stage_->PerformBuild();
    stage_->ClearOffStage();
    stage_->PerformBuild();
    auto child = stage_->GetFirstChild();
    ASSERT_TRUE(child);
    auto displayChild = AceType::DynamicCast<DisplayElement>(child);
    ASSERT_TRUE(displayChild);
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(displayChild->GetRenderNode());
    auto renderFlexItem = AceType::DynamicCast<RenderFlexItem>(renderDisplay->GetChildren().front());
    ASSERT_TRUE(renderDisplay != nullptr);
    ASSERT_TRUE(renderFlexItem != nullptr);
    EXPECT_TRUE(renderFlexItem->GetFlexGrow() == 1);
    EXPECT_TRUE(renderFlexItem->GetFlexShrink() == 1);
    EXPECT_TRUE(renderDisplay->GetVisibleType() == VisibleType::VISIBLE);
}

} // namespace OHOS::Ace
