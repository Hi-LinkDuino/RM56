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

#define private public
#define protected public
#include "core/components/plugin/render_plugin.h"
#undef private
#undef protected
#include "core/components/plugin/plugin_component.h"
#include "frameworks/base/geometry/point.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const Dimension PLUGIN_WIDTH = Dimension(1000.0);
const Dimension PLUGIN_HEIGHT = Dimension(1000.0);
} // namespace
class RenderPluginTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderPluginUpdate001
 * @tc.desc: Verify the Update Interface of RenderPlugin work correctly with plugin component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderPluginTest, UpdatePlugin001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin component and update it.
     */
    RefPtr<RenderPlugin> renderPlugin = AceType::MakeRefPtr<RenderPlugin>();
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(renderPlugin != nullptr);
    EXPECT_TRUE(pluginComponent != nullptr);
    RequestPluginInfo pluginInfo;

    pluginInfo.width = PLUGIN_WIDTH;
    pluginInfo.height = PLUGIN_HEIGHT;
    pluginComponent->SetPluginRequestionInfo(pluginInfo);

    /**
     * @tc.steps: step2. Update RenderPlugin with PluginComponent.
     * @tc.expected: step2. RenderPlugin is updated with correct settings.
     */
    renderPlugin->Update(pluginComponent);
    EXPECT_EQ(renderPlugin->rootWidth_, PLUGIN_WIDTH);
    EXPECT_EQ(renderPlugin->rootHeight_, PLUGIN_HEIGHT);
}

/**
 * @tc.name: RenderPluginPerformLayout001
 * @tc.desc: Verify the PerformLayout Interface of RenderPlugin work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderPluginTest, RenderPluginPerformLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin component and update it.
     */
    RefPtr<RenderPlugin> renderPlugin = AceType::MakeRefPtr<RenderPlugin>();
    RefPtr<PluginComponent> pluginComponent = AceType::MakeRefPtr<PluginComponent>();
    EXPECT_TRUE(renderPlugin != nullptr);
    EXPECT_TRUE(pluginComponent != nullptr);
    RequestPluginInfo pluginInfo;

    pluginInfo.width = PLUGIN_WIDTH;
    pluginInfo.height = PLUGIN_HEIGHT;
    pluginInfo.pluginName = "cardName";
    pluginInfo.bundleName = "bundleName";
    pluginInfo.abilityName = "abilityName";
    pluginInfo.moduleName = " moduleName";
    GTEST_LOG_(INFO) << "pluginInfo is:"<< pluginInfo.ToString();

    pluginComponent->SetPluginRequestionInfo(pluginInfo);

    /**
     * @tc.steps: step2. Update RenderPlugin with PluginComponent.
     * @tc.expected: step2. RenderPlugin is updated with correct settings.
     */
    renderPlugin->Update(pluginComponent);
    EXPECT_EQ(renderPlugin->rootWidth_, PLUGIN_WIDTH);
    EXPECT_EQ(renderPlugin->rootHeight_, PLUGIN_HEIGHT);

    /**
     * @tc.steps: step3. Perform Layout.
     * @tc.expected: step3. Perform Layout correctly.
     */
    renderPlugin->PerformLayout();
    EXPECT_EQ(renderPlugin->NeedLayout(), false);
    EXPECT_EQ(renderPlugin->NeedRender(), true);
}

/**
 * @tc.name: RenderPluginPerformLayout002
 * @tc.desc: Verify the PerformLayout Interface of RenderPlugin work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderPluginTest, RenderPluginPerformLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a RenderPlugin.
     */
    RefPtr<RenderPlugin> renderPlugin = AceType::MakeRefPtr<RenderPlugin>();
    EXPECT_TRUE(renderPlugin != nullptr);
    RefPtr<RenderNode> renderNode = renderPlugin->Create();
    EXPECT_TRUE(renderNode != nullptr);

    /**
     * @tc.steps: step2. Perform Layout.
     * @tc.expected: step2. Perform Layout correctly.
     */
    renderPlugin->PerformLayout();
    EXPECT_EQ(renderPlugin->NeedLayout(), false);
    EXPECT_EQ(renderPlugin->NeedRender(), false);
}

/**
 * @tc.name: RenderPluginTouchTest001
 * @tc.desc: Verify the TouchTest Interface of RenderPlugin work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(RenderPluginTest, RenderPluginTouchTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a RenderPlugin.
     */
    RefPtr<RenderPlugin> renderPlugin = AceType::MakeRefPtr<RenderPlugin>();
    EXPECT_TRUE(renderPlugin != nullptr);

    /**
     * @tc.steps: step2. Touch Test.
     * @tc.expected: step2. Touch Test correctly.
     */
    Point globalPoint, parentLocalPoint;
    TouchRestrict touchRestrict;
    TouchTestResult result;
    EXPECT_EQ(renderPlugin->TouchTest(globalPoint, parentLocalPoint, touchRestrict, result), true);
}
} // namespace OHOS::Ace