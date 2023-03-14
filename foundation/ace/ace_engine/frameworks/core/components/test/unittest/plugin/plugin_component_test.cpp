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
#include "core/components/plugin/plugin_component.h"
#include "core/components/plugin/plugin_element.h"
#undef private
#undef protected

#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const Dimension PLUGIN_WIDTH = Dimension(1000.0);
const Dimension PLUGIN_HEIGHT = Dimension(1000.0);
} // namespace
class PluginComponentTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginComponentTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    taskExecutor->InitJsThread(false);
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
}

/**
 * @tc.name: PluginComponentCreateElementTest001
 * @tc.desc: Verify the CreateElement Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentCreateElement001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Create Element.
     * @tc.expected: step2. Create Element success.
     */
    RefPtr<Element> element = pluginComponent->CreateElement();
    EXPECT_TRUE(element != nullptr);

    // for element destroy
    auto pluginElement = AceType::DynamicCast<PluginElement>(element);
    EXPECT_TRUE(pluginElement != nullptr);
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginComponentTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);
    pluginElement->pluginSubContainer_ = AceType::MakeRefPtr<PluginSubContainer>(pipelineContext);
}

/**
 * @tc.name: PluginComponentCreateRenderNode001
 * @tc.desc: Verify the CreateRenderNode Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, FlutterRenderPluginCreateRenderNode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

     /**
     * @tc.steps: step2. Get Draw Delegate.
     * @tc.expected: step2. Get Draw Delegate success.
     */
    RefPtr<RenderNode> renderNode = pluginComponent->CreateRenderNode();
    EXPECT_TRUE(renderNode != nullptr);
}

/**
 * @tc.name: PluginComponentSetPluginRequestionInfo001
 * @tc.desc: Verify the SetPluginRequestionInfo Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, SetPluginRequestionInfo001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set Plugin Requestion Info.
     * @tc.expected: step2. Set Plugin Requestion Info success.
     */
    RequestPluginInfo pluginInfo;
    pluginInfo.width = PLUGIN_WIDTH;
    pluginInfo.height = PLUGIN_HEIGHT;
    pluginComponent->SetPluginRequestionInfo(pluginInfo);
    EXPECT_EQ(pluginComponent->GetPluginRequestionInfo().width, PLUGIN_WIDTH);
    EXPECT_EQ(pluginComponent->GetPluginRequestionInfo().height, PLUGIN_HEIGHT);
}

/**
 * @tc.name: PluginComponentSetDimension001
 * @tc.desc: Verify the SetDimension Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentSetDimension001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set Dimension.
     * @tc.expected: step2. Set Dimension Update success.
     */
    pluginComponent->SetDimension(0);
    EXPECT_EQ(pluginComponent->GetPluginRequestionInfo().dimension, 0);
}

/**
 * @tc.name: PluginComponentSetModuleName001
 * @tc.desc: Verify the SetModuleName Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentSetModuleName001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set Module Name.
     * @tc.expected: step2. Set Module Name success.
     */
    pluginComponent->SetModuleName("module name");
    EXPECT_EQ(pluginComponent->GetPluginRequestionInfo().moduleName, "module name");
}

/**
 * @tc.name: PluginComponentSetCardSize001
 * @tc.desc: Verify the SetCardSize Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentComponentSetCardSize001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set Card Size.
     * @tc.expected: step2. Set Card Size success.
     */
    pluginComponent->SetPluginSize(PLUGIN_WIDTH, PLUGIN_HEIGHT);
    EXPECT_EQ(pluginComponent->GetWidth(), PLUGIN_WIDTH);
    EXPECT_EQ(pluginComponent->GetHeight(), PLUGIN_HEIGHT);
}

/**
 * @tc.name: PluginComponentSetOnAcquirePluginEventId001
 * @tc.desc: Verify the SetOnAcquirePluginEventId Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentSetOnAcquirePluginEventId001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set On Acquire Plugin Event Id.
     * @tc.expected: step2. Set On Acquire Plugin Event Id success.
     */
    EXPECT_TRUE(pluginComponent->GetOnCompleteEventId().IsEmpty());
    const EventMarker eventMaker("eventId");
    pluginComponent->SetOnCompleteEventId(eventMaker);
    EXPECT_FALSE(pluginComponent->GetOnCompleteEventId().IsEmpty());
}

/**
 * @tc.name: PluginComponentSetOnErrorEventId001
 * @tc.desc: Verify the SetOnErrorEventId Interface of PluginComponent work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginComponentTest, PluginComponentSetOnErrorEventId001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginComponent.
     */
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(pluginComponent != nullptr);

    /**
     * @tc.steps: step2. Set On Error Event Id.
     * @tc.expected: step2. Set On Error Event Id success.
     */
    EXPECT_TRUE(pluginComponent->GetOnErrorEvent().IsEmpty());
    const EventMarker eventMaker("errorEventId");
    pluginComponent->SetOnErrorEventId(eventMaker);
    EXPECT_FALSE(pluginComponent->GetOnErrorEvent().IsEmpty());
}
} // namespace OHOS::Ace