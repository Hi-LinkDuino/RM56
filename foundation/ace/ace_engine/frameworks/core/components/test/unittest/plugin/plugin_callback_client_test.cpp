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

#include "core/common/flutter/flutter_task_executor.h"
#include "core/common/frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/components/plugin/plugin_sub_container.h"
#include "core/mock/mock_resource_register.h"
#include "core/components/plugin/resource/plugin_manager_delegate.h"
#define private public
#define protected public
#include "core/components/plugin/resource/plugin_callback_client.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class PluginCallBackClientTest
 : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<Frontend>& frontend);
};

RefPtr<PipelineContext> PluginCallBackClientTest::GetPipelineContext(const RefPtr<Frontend>& frontend)
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
}

/**
 * @tc.name: PluginCallBackClientOnDeathReceivedTest001
 * @tc.desc: Verify the OnDeathReceived Interface of PluginCallBackClientTest work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginCallBackClientTest, PluginCallBackClientOnDeathReceivedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Callback Client.
     */
    PluginCallbackClient pluginCallbackClient;

    /**
     * @tc.steps: step2. On Death Received.
     * @tc.expected: step2.On Death Received success.
     */
    auto fakeFrontend = Frontend::CreateDefault();
    RefPtr<PluginManagerDelegate> delegate = AceType::MakeRefPtr<PluginManagerDelegate>
                                                (PluginCallBackClientTest::GetPipelineContext(fakeFrontend));
    pluginCallbackClient.SetPluginManagerDelegate(delegate);
    pluginCallbackClient.OnDeathReceived();
    EXPECT_TRUE(pluginCallbackClient.delegate_.Upgrade());
}
} // namespace OHOS::Ace