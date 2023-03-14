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
#include "core/components/plugin/resource/plugin_manager_resource.h"
#undef private
#undef protected
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class PluginManagerResourceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginManagerResourceTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
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
 * @tc.name: PluginManagerResourceGetId001
 * @tc.desc: Verify the GetId Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceGetId001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Get Id.
     * @tc.expected: step2. Get Id success.
     */
    pluginManagerResource.id_ = 0;
    EXPECT_EQ(pluginManagerResource.GetId(), 0);
}

/**
 * @tc.name: PluginManagerResourceGetHashCode001
 * @tc.desc: Verify the GetHashCode Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceGetHashCode001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Get Hash Code.
     * @tc.expected: step2. Get HashCode success.
     */
    pluginManagerResource.hash_ = "hashcode";
    EXPECT_EQ(pluginManagerResource.GetHashCode(), "hashcode");
}

/**
 * @tc.name: PluginManagerResourceGetIntParam001
 * @tc.desc: Verify the GetIntParam Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceGetIntParam001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Get Int Param.
     * @tc.expected: step2. Get Int Param Method success.
     */

    int32_t result = pluginManagerResource.GetIntParam("paramname", "p");
    GTEST_LOG_(INFO) << "PluginManagerResourceGetIntParam001 is begin2:"<<result;
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: PluginManagerResourceMakeResourceHash001
 * @tc.desc: Verify the MakeResourceHash Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceMakeResourceHash001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Get String Param.
     * @tc.expected: step2.Get String Param Method success.
     */

    std::string result = pluginManagerResource.MakeResourceHash();
    EXPECT_EQ(result, "type@-1");
}

/**
 * @tc.name: PluginManagerResourceMakeEventHash001
 * @tc.desc: Verify the MakeEventHash Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceMakeEventHash001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Make Event Hash.
     * @tc.expected: step2.Make Event Hash success.
     */

    std::string result = pluginManagerResource.MakeEventHash("event");
    EXPECT_EQ(result, "event#HWJS-=-#event#HWJS-?-#");
}

/**
 * @tc.name: PluginManagerResourceMakeMethodHash001
 * @tc.desc: Verify the MakeMethodHash Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceMakeMethodHash001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Make Method Hash.
     * @tc.expected: step2.Make Method Hash success.
     */

    std::string result = pluginManagerResource.MakeMethodHash("method");
    EXPECT_EQ(result, "method#HWJS-=-#method#HWJS-?-#");
}

/**
 * @tc.name: PluginManagerResourceGetStringParam001
 * @tc.desc: Verify the GetStringParam Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerResourceGetStringParam001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Make Method Hash.
     * @tc.expected: step2.Make Method Hash success.
     */

    std::string result = pluginManagerResource.GetStringParam("paramname", "p");
    GTEST_LOG_(INFO) << "PluginManagerResourceGetStringParam001 is begin2:"<<result;
    EXPECT_EQ(result, "ramname");
}

/**
 * @tc.name: PluginManagerParseMapFromString001
 * @tc.desc: Verify the ParseMapFromString Interface of PluginManagerResource work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerResourceTest, PluginManagerParseMapFromString001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Resource.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerResourceTest::GetPipelineContext(pluginFrontend);
    PluginManagerResource pluginManagerResource("type", pipelineContext);

    /**
     * @tc.steps: step2. Parse Map From String.
     * @tc.expected: step2.Parse Map From String.
     */

    std::map<std::string, std::string> result = pluginManagerResource.ParseMapFromString("1#HWJS-=-#");
    auto it = result.find("1");
    if (it != result.end()) {
        GTEST_LOG_(INFO) << "PluginManagerParseMapFromString001";
        EXPECT_EQ(it->second, "");
    }
}
} // namespace OHOS::Ace