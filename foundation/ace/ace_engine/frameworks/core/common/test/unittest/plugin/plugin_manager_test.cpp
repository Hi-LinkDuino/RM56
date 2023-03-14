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

#define private public
#define protected public
#include "core/common/plugin_manager.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const int64_t PLUGIN_ID = 1000;
} // namespace
class PluginManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: AddPluginSubContainer001
 * @tc.desc: Verify the AddPluginSubContainer Interface of PluginManager work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerTest, AddPluginSubContainer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginManager.
     */
    PluginManager pluginManager;

    /**
     * @tc.steps: step2. Add Plugin Sub Container.
     * @tc.expected: step2. Add Plugin Sub Container success.
    */
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>();
    RefPtr<PluginSubContainer> pluginSubContainer = AceType::MakeRefPtr<PluginSubContainer>(context.Upgrade());
    EXPECT_TRUE(pluginSubContainer != nullptr);
    pluginManager.AddPluginSubContainer(PLUGIN_ID, pluginSubContainer);
    EXPECT_TRUE(pluginManager.GetPluginSubContainer(PLUGIN_ID) != nullptr);
    pluginManager.RemovePluginSubContainer(PLUGIN_ID);
    EXPECT_TRUE(pluginManager.GetPluginSubContainer(PLUGIN_ID) == nullptr);
}

/**
 * @tc.name: AddNonmatchedContainer001
 * @tc.desc: Verify the AddNonmatchedContainer Interface of PluginManager work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerTest, AddNonmatchedContainer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginManager.
     */
    PluginManager pluginManager;

    /**
     * @tc.steps: step2. Add Nonmatched Container.
     * @tc.expected: step2. Add Nonmatched Container success.
    */
    WeakPtr<PipelineContext> context = WeakPtr<PipelineContext>();
    RefPtr<PluginSubContainer> pluginSubContainer = AceType::MakeRefPtr<PluginSubContainer>(context.Upgrade());
    EXPECT_TRUE(pluginSubContainer != nullptr);
    pluginManager.AddNonmatchedContainer("PLUGIN_ID", pluginSubContainer);
    EXPECT_TRUE(pluginManager.MatchPluginSubContainerWithPluginId(PLUGIN_ID, "PLUGIN_ID") != nullptr);
    EXPECT_TRUE(pluginManager.GetPluginSubContainer(PLUGIN_ID) != nullptr);
    pluginManager.RemovePluginSubContainer(PLUGIN_ID);
    EXPECT_TRUE(pluginManager.GetPluginSubContainer(PLUGIN_ID) == nullptr);
}

/**
 * @tc.name: MatchPluginSubContainerWithPluginId001
 * @tc.desc: Verify the MatchPluginSubContainerWithPluginId Interface of PluginManager work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerTest, MatchPluginSubContainerWithPluginId001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginManager.
     */
    PluginManager pluginManager;

    /**
     * @tc.steps: step2. Match Plugin SubContainer With Plugin Id.
     * @tc.expected: step2. When no Match Plugin SubContainer With Plugin Id return nullptr.
    */
    EXPECT_TRUE(pluginManager.MatchPluginSubContainerWithPluginId(PLUGIN_ID, "PLUGIN_ID") == nullptr);
}
} // namespace OHOS::Ace