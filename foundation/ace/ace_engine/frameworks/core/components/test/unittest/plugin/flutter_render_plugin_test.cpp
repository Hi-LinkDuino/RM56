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

#include "core/components/plugin/flutter_render_plugin.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class FlutterRenderPluginTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: FlutterRenderGetRenderLayer001
 * @tc.desc: Verify the GetRenderLayer Interface of FlutterRenderPlugin work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(FlutterRenderPluginTest, GetRenderLayer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a FlutterRenderPlugin.
     */
    RefPtr<FlutterRenderPlugin> flutterRenderPlugin = AceType::MakeRefPtr<FlutterRenderPlugin>();
    EXPECT_TRUE(flutterRenderPlugin != nullptr);

    /**
     * @tc.steps: step2. Get Render Layer.
     * @tc.expected: step2. Get Render Layer success.
     */
    RenderLayer renderLayer = flutterRenderPlugin->GetRenderLayer();
    EXPECT_TRUE(renderLayer != nullptr);
}

/**
 * @tc.name: FlutterRenderGetDrawDelegate001
 * @tc.desc: Verify the GetDrawDelegate Interface work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(FlutterRenderPluginTest, FlutterRenderPluginGetDrawDelegate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a FlutterRenderPlugin.
     */
    RefPtr<FlutterRenderPlugin> flutterRenderPlugin = AceType::MakeRefPtr<FlutterRenderPlugin>();
    EXPECT_TRUE(flutterRenderPlugin != nullptr);

     /**
     * @tc.steps: step2. Get Draw Delegate.
     * @tc.expected: step2. Get Draw Delegate success.
     */
    std::unique_ptr<DrawDelegate> drawDelegate = flutterRenderPlugin->GetDrawDelegate();
    EXPECT_TRUE(drawDelegate != nullptr);
}

/**
 * @tc.name: FlutterRenderRemoveChildren001
 * @tc.desc: Verify the PerformLayout Interface of RemoveChildren work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(FlutterRenderPluginTest, FlutterRenderRemoveChildren001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a FlutterRenderPlugin.
     */
    RefPtr<FlutterRenderPlugin> flutterRenderPlugin = AceType::MakeRefPtr<FlutterRenderPlugin>();

    /**
     * @tc.steps: step2. Remove Children.
     * @tc.expected: step2. Remove Children success.
     */
    flutterRenderPlugin->RemoveChildren();
    EXPECT_EQ(flutterRenderPlugin->NeedLayout(), true);
}
} // namespace OHOS::Ace