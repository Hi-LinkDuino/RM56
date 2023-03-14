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
#include "base/i18n/localization.h"
#include "core/common/flutter/flutter_task_executor.h"
#include "core/common/frontend.h"
#define private public
#define protected public
#include "core/components/plugin/plugin_sub_container.h"
#include "core/components/plugin/plugin_element.h"
#undef private
#undef protected
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const std::string PATH = "/data/accounts/account_0/applications/com.example.provide01/com.example.provide01/";
} // namespace
class PluginSubContainerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<Frontend>& frontend);
};
RefPtr<TaskExecutor> gTaskExecutor;

RefPtr<PipelineContext> PluginSubContainerTest::GetPipelineContext(const RefPtr<Frontend>& frontend)
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));

    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    taskExecutor->InitPlatformThread();
    taskExecutor->InitJsThread(true);
    gTaskExecutor = taskExecutor;

    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
}

/**
 * @tc.name: PluginSubContainerDestroyTest001
 * @tc.desc: Verify the Destroy Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerDestroyTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    auto fakeFrontend = Frontend::CreateDefault();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(fakeFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);
    pluginSubContainer.pipelineContext_ = pipelineContext;
    pluginSubContainer.taskExecutor_ = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginSubContainer.assetManager_ = Referenced::MakeRefPtr<FakeAssetManager>();
    EXPECT_TRUE(pluginSubContainer.pipelineContext_ != nullptr);
    EXPECT_TRUE(pluginSubContainer.taskExecutor_ != nullptr);
    EXPECT_TRUE(pluginSubContainer.assetManager_ != nullptr);

    /**
     * @tc.steps: step2. Destroy.
     * @tc.expected: step2. Destroy success.
     */
    pluginSubContainer.Destroy();
    EXPECT_TRUE(pluginSubContainer.pipelineContext_ == nullptr);
    EXPECT_TRUE(pluginSubContainer.assetManager_ == nullptr);
}

/**
 * @tc.name: PluginSubContainerGetTaskExecutorTest001
 * @tc.desc: Verify the GetTaskExecutor Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerGetTaskExecutorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    auto fakeFrontend = Frontend::CreateDefault();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(fakeFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);
    pluginSubContainer.taskExecutor_ = Referenced::MakeRefPtr<FlutterTaskExecutor>();

    /**
     * @tc.steps: step2. Get Task Executor.
     * @tc.expected: step2. Get Task Executor success.
     */
    EXPECT_TRUE(pluginSubContainer.GetTaskExecutor() != nullptr);
}

/**
 * @tc.name: PluginSubContainerGetPipelineContextTest001
 * @tc.desc: Verify the GetPipelineContext Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerGetPipelineContextTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    auto fakeFrontend = Frontend::CreateDefault();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(fakeFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);
    pluginSubContainer.pipelineContext_ = pipelineContext;

    /**
     * @tc.steps: step2. Get Pipeline Context.
     * @tc.expected: step2. Get Pipeline Context success.
     */
    EXPECT_TRUE(pluginSubContainer.GetPipelineContext() != nullptr);
}

/**
 * @tc.name: PluginSubContainerSetAllowUpdateTest001
 * @tc.desc: Verify the SetAllowUpdate Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerSetAllowUpdateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>();
    RefPtr<PluginSubContainer> pluginSubContainer = AceType::MakeRefPtr<PluginSubContainer>(context.Upgrade());
    EXPECT_TRUE(pluginSubContainer != nullptr);

    /**
     * @tc.steps: step2. Set Allow Update false.
     * @tc.expected: step2. Get Declarative Set Allow Update success.
     */
    pluginSubContainer->SetAllowUpdate(false);
    EXPECT_FALSE(pluginSubContainer->allowUpdate_);
}

/**
 * @tc.name: PluginSubContainerSetAllowUpdateTest002
 * @tc.desc: Verify the SetAllowUpdate Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerSetAllowUpdateTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>();
    RefPtr<PluginSubContainer> pluginSubContainer = AceType::MakeRefPtr<PluginSubContainer>(context.Upgrade());
    EXPECT_TRUE(pluginSubContainer != nullptr);

    /**
     * @tc.steps: step2. Set Allow Update true.
     * @tc.expected: step2. Set Allow Update success.
     */
    pluginSubContainer->SetAllowUpdate(true);
    EXPECT_TRUE(pluginSubContainer->allowUpdate_);
}

/**
 * @tc.name: PluginSubSetAllowUpdateTest002
 * @tc.desc: Verify the SetAllowUpdate Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerInitializeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>();
    RefPtr<PluginSubContainer> pluginSubContainer = AceType::MakeRefPtr<PluginSubContainer>(context.Upgrade());
    EXPECT_TRUE(pluginSubContainer != nullptr);

    /**
     * @tc.steps: step2. Set Allow Update true.
     * @tc.expected: step2. Get Declarative Set Allow Update success.
     */
    pluginSubContainer->SetAllowUpdate(true);
    EXPECT_TRUE(pluginSubContainer->allowUpdate_);
}

/**
 * @tc.name: PluginSubContainerUpdateRootElmentSizeTest001
 * @tc.desc: Verify the UpdateRootElmentSize Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerUpdateRootElmentSizeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    auto fakeFrontend = Frontend::CreateDefault();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(fakeFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);

    /**
     * @tc.steps: step2. Update Root Elment Size.
     * @tc.expected: step2. Update Root Elment Size success.
     */
    pluginSubContainer.rootWidth_ = 1.0_vp;
    pluginSubContainer.rootHeight_ = 1.0_vp;
    EXPECT_EQ(pluginSubContainer.surfaceWidth_, 1.0f);
    EXPECT_EQ(pluginSubContainer.surfaceHeight_, 1.0f);
    pluginSubContainer.UpdateRootElmentSize();
    EXPECT_EQ(pluginSubContainer.surfaceWidth_, 0.0f);
    EXPECT_EQ(pluginSubContainer.surfaceHeight_, 0.0f);
}

/**
 * @tc.name: PluginSubContainerUpdatePluginTest001
 * @tc.desc: Verify the UpdatePlugin Interface of PluginSubContainer work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerUpdatePluginTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);
    pluginSubContainer.frontend_ = Referenced::MakeRefPtr<PluginFrontend>();
    /**
     * @tc.steps: step2. UpdatePlugin.
     * @tc.expected: step2.UpdatePlugin success.
     */
    pluginSubContainer.UpdatePlugin("content");
    EXPECT_TRUE(pluginSubContainer.frontend_ != nullptr);
}

/**
 * @tc.name: PluginSubContainerUpdatePluginTest002
 * @tc.desc: Verify the UpdatePlugin Interface of PluginSubContainer work fail due to could not find plugin front end.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerUpdatePluginTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    PluginSubContainer pluginSubContainer(pipelineContext);
    /**
     * @tc.steps: step2. UpdatePlugin.
     * @tc.expected: step2.UpdatePlugin success.
     */
    pluginSubContainer.UpdatePlugin("content");
    EXPECT_TRUE(pluginSubContainer.frontend_ == nullptr);
}

/**
 * @tc.name: PluginSubContainerSetAssetManager001
 * @tc.desc: Verify the SetAssetManager Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContainerSetAssetManager001, TestSize.Level1)
{
 /**
     * @tc.steps: step1. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    RefPtr <PluginSubContainer> pluginSubContainer = Referenced::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Set Asset Manager.
     * @tc.expected: step2. Set Asset Manager success.
     */
    pluginSubContainer->frontend_ = AceType::MakeRefPtr<PluginFrontend>();
    RefPtr<FlutterAssetManager> flutterAssetManager = pluginSubContainer->SetAssetManager(PATH, "plugincomponent1");
    EXPECT_TRUE(flutterAssetManager != nullptr);
}

/**
 * @tc.name: PluginSubContaineRunPlugin001
 * @tc.desc: Verify the RunPlugin Interface of PluginSubContainer work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContaineRunPlugin001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetLocale.
    */
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");

    /**
     * @tc.steps: step2. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    RefPtr <PluginSubContainer> pluginSubContainer = Referenced::MakeRefPtr<PluginSubContainer>(pipelineContext);
    RefPtr<PluginElement> pluginElement = AceType::MakeRefPtr<PluginElement>();
    pluginElement->SetRenderNode(pluginElement->CreateRenderNode());
    pluginElement->pluginSubContainer_ = pluginSubContainer;
    pluginSubContainer->SetPluginElement(pluginElement);
    RefPtr<PluginComponent> mountPoint = AceType::MakeRefPtr<PluginComponent>();
    pluginSubContainer->SetPluginComponet(mountPoint);

    /**
     * @tc.steps: step3. Initialize.
     * @tc.expected: step3. Initialize success.
     */
    pluginSubContainer->Initialize();
    EXPECT_TRUE(pluginSubContainer->frontend_ != nullptr);
    /**
     * @tc.steps: step4. RunPlugin.
     * @tc.expected: step4. RunPlugin success.
     */
    pluginSubContainer->RunPlugin(PATH, "newui", "", "data");
}

/**
 * @tc.name: PluginSubContaineRunPlugin002
 * @tc.desc: Verify the RunPlugin Interface of PluginSubContainer work fail because could not get plugin element.
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContaineRunPlugin002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetLocale.
    */
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");

    /**
     * @tc.steps: step2. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    RefPtr <PluginSubContainer> pluginSubContainer = Referenced::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step3. Initialize.
     * @tc.expected: step3. Initialize success.
     */
    pluginSubContainer->Initialize();
    EXPECT_TRUE(pluginSubContainer->frontend_ != nullptr);

    /**
     * @tc.steps: step4. RunPlugin.
     * @tc.expected: step4. RunPlugin fail.
     */
    pluginSubContainer->RunPlugin(PATH, "newui", "", "data");
}

/**
 * @tc.name: PluginSubContaineRunPlugin003
 * @tc.desc: Verify the RunPlugin Interface of PluginSubContainer work fail because could not get render node
 * @tc.type: FUNC
 */
HWTEST_F(PluginSubContainerTest, PluginSubContaineRunPlugin003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetLocale.
    */
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");

    /**
     * @tc.steps: step2. Build a PluginSubContainer.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginSubContainerTest::GetPipelineContext(pluginFrontend);
    RefPtr <PluginSubContainer> pluginSubContainer = Referenced::MakeRefPtr<PluginSubContainer>(pipelineContext);
    RefPtr<PluginElement> pluginElement = AceType::MakeRefPtr<PluginElement>();
    pluginElement->pluginSubContainer_ = pluginSubContainer;
    pluginSubContainer->SetPluginElement(pluginElement);
    RefPtr<PluginComponent> mountPoint = AceType::MakeRefPtr<PluginComponent>();
    pluginSubContainer->SetPluginComponet(mountPoint);

    /**
     * @tc.steps: step3. Initialize.
     * @tc.expected: step3. Initialize success.
     */
    pluginSubContainer->Initialize();
    EXPECT_TRUE(pluginSubContainer->frontend_ != nullptr);

    /**
     * @tc.steps: step4. RunPlugin.
     * @tc.expected: step4. RunPlugin fail.
     */
    pluginSubContainer->RunPlugin(PATH, "newui", "", "data");
}
} // namespace OHOS::Ace
