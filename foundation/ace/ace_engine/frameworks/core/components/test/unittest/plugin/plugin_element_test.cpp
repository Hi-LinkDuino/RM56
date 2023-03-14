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

#include "core/common/flutter/flutter_task_executor.h"
#define private public
#define protected public
#include "core/components/plugin/plugin_element.h"
#undef private
#undef protected
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class PluginElementTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginElementTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    taskExecutor->InitPlatformThread();
    taskExecutor->InitJsThread(true);
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
}

/**
 * @tc.name: PluginElementCreateRenderNodeTest001
 * @tc.desc: Verify the CreateRenderNode Interface of PluginElement work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementCreateRenderNodeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Create Element.
     * @tc.expected: step2. Create Element success.
     */
    RefPtr<RenderNode> renderNode = element.CreateRenderNode();
    EXPECT_TRUE(renderNode != nullptr);
}

/**
 * @tc.name: PluginElementPerformBuild001
 * @tc.desc: Verify the PerformBuild Interface of PluginElement work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementPerformBuild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

     /**
     * @tc.steps: step2. Perform Build.
     * @tc.expected: step2. Perform Build success.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);
    element.PerformBuild();
    EXPECT_TRUE(element.GetPluginSubContainer() != nullptr);
}

/**
 * @tc.name: PluginElementInitEvent001
 * @tc.desc: Verify the InitEvent Interface of PluginElement work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementInitEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. InitEvent.
     * @tc.expected: step2. InitEvent success.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);
    const EventMarker eventMaker("eventId");
    pluginComponent->SetOnCompleteEventId(eventMaker);
    pluginComponent->SetOnErrorEventId(eventMaker);
    EXPECT_EQ(pluginComponent->GetOnCompleteEventId().GetData().eventId, "eventId");
    EXPECT_EQ(pluginComponent->GetOnErrorEvent().GetData().eventId, "eventId");
    element.InitEvent(pluginComponent);
    EXPECT_TRUE(element.onCompleteEvent_ != nullptr);
    EXPECT_TRUE(element.onErrorEvent_ != nullptr);
}

/**
 * @tc.name: PluginElementPrepare001
 * @tc.desc: Verify the Prepare Interface of PluginElement work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementPrepare001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Prepare.
     * @tc.expected: step2. Prepare success.
     */
    WeakPtr<Element> parent = WeakPtr<Element>();
    element.Prepare(parent);
    EXPECT_TRUE(element.pluginManagerBridge_ != nullptr);
}

/**
 * @tc.name: PluginElementGetPackagePath001
 * @tc.desc: Verify the GetPackagePath Interface of PluginElement work correctly app bundleName or abilityName is empty.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementGetPackagePath001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Get Package Path.
     * @tc.expected: step2. Get Package Path correctly.
     */
    RefPtr<PluginElement> elementParam = Referenced::MakeRefPtr<PluginElement>();
    elementParam->context_ = PluginElementTest::GetPipelineContext(pluginFrontend);
    elementParam->pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);
    const EventMarker eventMaker("eventId");
    pluginComponent->SetOnCompleteEventId(eventMaker);
    pluginComponent->SetOnErrorEventId(eventMaker);
    elementParam->InitEvent(pluginComponent);
    EXPECT_TRUE(elementParam->onCompleteEvent_ != nullptr);
    EXPECT_TRUE(elementParam->onErrorEvent_ != nullptr);
    RequestPluginInfo info;
    info.bundleName = "";
    EXPECT_EQ(element.GetPackagePath(elementParam, info), "");
}

/**
 * @tc.name: PluginElementGetPackagePath002
 * @tc.desc: Verify the GetPackagePath Interface of PluginElement work correctly app bundleName or abilityName is not
 *           empty and strList size is one.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementGetPackagePath002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Get Package Path.
     * @tc.expected: step2. Get Package Path correctly.
     */
    RefPtr<PluginElement> elementParam = Referenced::MakeRefPtr<PluginElement>();
    elementParam->context_ = PluginElementTest::GetPipelineContext(pluginFrontend);
    elementParam->pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);
    const EventMarker eventMaker("eventId");
    pluginComponent->SetOnCompleteEventId(eventMaker);
    pluginComponent->SetOnErrorEventId(eventMaker);
    elementParam->InitEvent(pluginComponent);
    EXPECT_TRUE(elementParam->onCompleteEvent_ != nullptr);
    EXPECT_TRUE(elementParam->onErrorEvent_ != nullptr);
    RequestPluginInfo info;
    info.bundleName = "bundleName";
    EXPECT_EQ(element.GetPackagePath(elementParam, info), "");
}

/**
 * @tc.name: PluginElementGetPackagePath003
 * @tc.desc: Verify the GetPackagePath Interface of PluginElement work correctly app bundleName or abilityName is not
 *           empty and strList size over one.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementGetPackagePath003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Get Package Path.
     * @tc.expected: step2. Get Package Path correctly.
     */
    RefPtr<PluginElement> elementParam = Referenced::MakeRefPtr<PluginElement>();
    elementParam->context_ = PluginElementTest::GetPipelineContext(pluginFrontend);
    elementParam->pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);
    const EventMarker eventMaker("eventId");
    pluginComponent->SetOnCompleteEventId(eventMaker);
    pluginComponent->SetOnErrorEventId(eventMaker);
    elementParam->InitEvent(pluginComponent);
    EXPECT_TRUE(elementParam->onCompleteEvent_ != nullptr);
    EXPECT_TRUE(elementParam->onErrorEvent_ != nullptr);
    RequestPluginInfo info;
    info.bundleName = "bundleName/bundleName";
    EXPECT_EQ(element.GetPackagePath(elementParam, info), "");
}

/**
 * @tc.name: PluginElementGetPackagePath003
 * @tc.desc: Verify the GetPackagePath Interface of PluginElement work correctly pluginElement Upgrade fail
 *           empty and strList size over one.
 * @tc.type: FUNC
 */
HWTEST_F(PluginElementTest, PluginElementGetPackagePath004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginElement.
     */
    PluginElement element;

    // for element destroy
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginElementTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    element.pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);

    /**
     * @tc.steps: step2. Get Package Path.
     * @tc.expected: step2. Get Package Path correctly.
     */
    WeakPtr<PluginElement> elementParam = WeakPtr<PluginElement>();
    RequestPluginInfo info;
    info.bundleName = "bundleName";
    EXPECT_EQ(element.GetPackagePath(elementParam, info), "");
}
} // namespace OHOS::Ace