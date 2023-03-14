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

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/animation/card_transition_controller.h"
#include "core/components/box/box_component.h"
#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/page_transition/page_transition_component.h"
#include "core/components/semi_modal/render_semi_modal.h"
#include "core/components/semi_modal/semi_modal_element.h"
#include "core/components/shadow/render_shadow.h"
#include "core/components/stage/stage_element.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/mock/window_mock.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t FRAME_TIME_IN_MILLISECOND = 100;
constexpr int32_t TEST_SURFACE_WIDTH = 1080;
constexpr int32_t TEST_SURFACE_HEIGHT = 1920;
constexpr double TEST_STATUS_BAR_HEIGHT = 72.0;
constexpr double TEST_NAVIGATION_BAR_HEIGHT = 122.0;
constexpr double ERROR_BOUND = 0.01;
int32_t g_pageId = 0; // for alloc page id.

class MockRenderFocusAnimation : public RenderFocusAnimation {};
class MockRenderShadow : public RenderShadow {};
class MockRenderDragBar : public RenderDragBar {};

} // namespace

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

RefPtr<RenderNode> RenderFocusAnimation::Create()
{
    return AceType::MakeRefPtr<MockRenderFocusAnimation>();
}

RefPtr<RenderNode> RenderShadow::Create()
{
    return AceType::MakeRefPtr<MockRenderShadow>();
}

RefPtr<RenderNode> RenderDragBar::Create()
{
    return AceType::MakeRefPtr<MockRenderDragBar>();
}

class RenderSemiModalTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}

    void CreateAndBuildSemiModal()
    {
        std::unique_ptr<PlatformWindow> platformWindow = TweenTestUtils::CreatePlatformWindow();
        platformWindowRaw_ = reinterpret_cast<MockPlatformWindow*>(platformWindow.get());
        auto window = TweenTestUtils::CreateWindow(std::move(platformWindow));
        auto taskExecutor = AceType::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        RefPtr<Frontend> frontend = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
        context_->SetTimeProvider(
            [this] { return this->platformWindowRaw_->GetCurrentTimestampNano() + NANOSECOND_TO_MILLISECOND * 10; });
        context_->SetWindowModal(WindowModal::SEMI_MODAL);

        auto pageComponent = CreatePushPageWithShared();

        context_->SetupRootElement();
        GetSemiModal();
        context_->PushPage(pageComponent);
        context_->OnSystemBarHeightChanged(TEST_STATUS_BAR_HEIGHT, TEST_NAVIGATION_BAR_HEIGHT);
        context_->OnSurfaceChanged(TEST_SURFACE_WIDTH, TEST_SURFACE_HEIGHT);

        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs_);
    }

    void GetSemiModal()
    {
        RefPtr<Element> stage = context_->GetStageElement();
        auto parent = stage->GetElementParent().Upgrade();
        semiModalElement_ = AceType::DynamicCast<SemiModalElement>(parent);
        while (parent && !semiModalElement_) {
            parent = parent->GetElementParent().Upgrade();
            semiModalElement_ = AceType::DynamicCast<SemiModalElement>(parent);
        }
        // semi modal element must be found.
        EXPECT_TRUE(!!semiModalElement_);
    }

    static RefPtr<PageComponent> CreatePushPageWithShared()
    {
        g_pageId++;
        // make content.
        auto box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(TEST_SURFACE_WIDTH);
        box->SetHeight(TEST_SURFACE_HEIGHT / 2.0);

        std::list<RefPtr<Component>> stackChildren { box };
        auto rootStackComponent = AceType::MakeRefPtr<StackComponent>(
            Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, stackChildren);
        rootStackComponent->SetMainStackSize(MainStackSize::MAX);
        auto composed = AceType::MakeRefPtr<ComposedComponent>("semiModalTest", "Init");
        composed->SetChild(rootStackComponent);

        auto collaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        collaboration->InsertChild(0, composed);
        // make page.
        auto transitionComponent = AceType::MakeRefPtr<PageTransitionComponent>();
        transitionComponent->SetSeparation(true);
        transitionComponent->SetContent(collaboration);
        return AceType::MakeRefPtr<PageComponent>(g_pageId, "", transitionComponent);
    }

protected:
    RefPtr<PipelineContext> context_;
    uint32_t frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
    RefPtr<SemiModalElement> semiModalElement_;
};

/**
 * @tc.name: SemiModalAnimationTest001
 * @tc.desc: Test Semi Modal enter transition
 * @tc.type: FUNC
 * @tc.require: AR000ELMHS
 * @tc.author: zhouzebin
 */
HWTEST_F(RenderSemiModalTest, SemiModalAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderSemiModalTest SemiModalAnimationTest001";
    /**
     * @tc.steps: step1. Build a semi modal component and push.
     */
    CreateAndBuildSemiModal();
    platformWindowRaw_->TriggerFrames(3);
    /**
     * @tc.steps: step2.  wait until transition done
     * @tc.expected: step2. check blank height.
     */
    auto semiRender = AceType::DynamicCast<RenderSemiModal>(semiModalElement_->GetRenderNode());
    EXPECT_TRUE(!!semiRender);
    EXPECT_NEAR(semiRender->GetBlankHeight(), 1775.71, ERROR_BOUND);
}

/**
 * @tc.name: SemiModalAnimationTest002
 * @tc.desc: Test Semi Modal full window transition
 * @tc.type: FUNC
 * @tc.require: AR000ELMHT
 * @tc.author: zhouzebin
 */
HWTEST_F(RenderSemiModalTest, SemiModalAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderSemiModalTest SemiModalAnimationTest002";
    /**
     * @tc.steps: step1. Build a semi modal component and push.
     */
    CreateAndBuildSemiModal();
    platformWindowRaw_->TriggerFrames(3);
    /**
     * @tc.steps: step2. switch to full window
     */
    semiModalElement_->AnimateToFullWindow(-1);
    platformWindowRaw_->TriggerFrames(5);
    /**
     * @tc.steps: step3.  wait until transition done
     * @tc.expected: step3. check blank height.
     */
    auto semiRender = AceType::DynamicCast<RenderSemiModal>(semiModalElement_->GetRenderNode());
    EXPECT_TRUE(!!semiRender);
    EXPECT_EQ(semiRender->GetBlankHeight(), 0);
}

/**
 * @tc.name: SemiModalAnimationTest003
 * @tc.desc: Test Semi Modal System bar height padding
 * @tc.type: FUNC
 * @tc.require: AR000EMO97
 * @tc.author: zhouzebin
 */
HWTEST_F(RenderSemiModalTest, SemiModalAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderSemiModalTest SemiModalAnimationTest003";
    /**
     * @tc.steps: step1. Build a semi modal component and push.
     */
    CreateAndBuildSemiModal();
    platformWindowRaw_->TriggerFrames(3);
    semiModalElement_->AnimateToFullWindow(-1);
    platformWindowRaw_->TriggerFrames(5);
    auto semiRender = AceType::DynamicCast<RenderSemiModal>(semiModalElement_->GetRenderNode());
    EXPECT_TRUE(!!semiRender);
    /**
     * @tc.steps: step2.  wait until transition done
     * @tc.expected: step2. check status bar height.
     */
    auto dragBar = AceType::DynamicCast<RenderDragBar>(semiRender->GetDragBar());
    EXPECT_TRUE(!!dragBar);
    EXPECT_EQ(dragBar->GetStatusBarHeight(), TEST_STATUS_BAR_HEIGHT);
}

/**
 * @tc.name: SemiModalAnimationTest004
 * @tc.desc: Test Semi Modal with drag_bar init show mode
 * @tc.type: FUNC
 * @tc.require: AR000F3CBH
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderSemiModalTest, SemiModalAnimationTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderSemiModalTest SemiModalAnimationTest004";
    /**
     * @tc.steps: step1. Build a semi modal component and push.
     * @tc.expected: step1. check drag_bar show mode is HALF.
     */
    CreateAndBuildSemiModal();
    auto semiRender = AceType::DynamicCast<RenderSemiModal>(semiModalElement_->GetRenderNode());
    EXPECT_TRUE(semiRender != nullptr);
    auto dragBar = AceType::DynamicCast<RenderDragBar>(semiRender->GetDragBar());
    EXPECT_TRUE(dragBar != nullptr);
    EXPECT_TRUE(dragBar->GetPanelMode() == PanelMode::HALF);
}

/**
 * @tc.name: SemiModalAnimationTest005
 * @tc.desc: Test Semi Modal with drag_bar FullWindow show mode
 * @tc.type: FUNC
 * @tc.require: AR000F3CBI
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderSemiModalTest, SemiModalAnimationTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderSemiModalTest SemiModalAnimationTest005";
    /**
     * @tc.steps: step1. Build a semi modal component and push.
     * @tc.expected: step1. check the semimodal render node not null.
     */
    CreateAndBuildSemiModal();
    auto semiRender = AceType::DynamicCast<RenderSemiModal>(semiModalElement_->GetRenderNode());
    EXPECT_TRUE(semiRender != nullptr);

    /**
     * @tc.steps: step2. Animate to Full window mode.
     * @tc.expected: step2. Check drag_bar show mode is FULL.
     */
    platformWindowRaw_->TriggerFrames(3);
    semiModalElement_->AnimateToFullWindow(-1);
    platformWindowRaw_->TriggerFrames(5);
    auto dragBar = AceType::DynamicCast<RenderDragBar>(semiRender->GetDragBar());
    EXPECT_TRUE(dragBar != nullptr);
    EXPECT_TRUE(dragBar->GetPanelMode() == PanelMode::FULL);
}

} // namespace OHOS::Ace
