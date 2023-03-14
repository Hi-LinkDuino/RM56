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
#include "core/components/plugin/resource/plugin_manager_delegate.h"
#undef private
#undef protected
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
class PluginManagerDelegateTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginManagerDelegateTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
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
 * @tc.name: PluginManagerDelegateAddPluginCompleteCallback001
 * @tc.desc: Verify the AddPluginCompleteCallback Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateAddPluginCompleteCallback001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Acquire Callback.
     * @tc.expected: step2. Add Plugin Acquire Callback success.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::CREATED;
    pluginManagerDelegate.AddPluginCompleteCallback(
        []() {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ != nullptr);
}

/**
 * @tc.name: PluginManagerDelegateAddPluginCompleteCallback002
 * @tc.desc: Verify the AddPluginCompleteCallback Interface of PluginManagerDelegate not work because state is RELEASED.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateCreatePlatformResource002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Acquire Callback.
     * @tc.expected: step2. Add Plugin Acquire Callback not work.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::RELEASED;
    pluginManagerDelegate.AddPluginCompleteCallback(
        []() {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ == nullptr);
}

/**
 * @tc.name: PluginManagerDelegateAddPluginUpdateCallback001
 * @tc.desc: Verify the AddPluginUpdateCallback Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateAddPluginUpdateCallback001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Update Callback.
     * @tc.expected: step2.  Add Plugin Update Callback success.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginUpdateCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::CREATED;
    pluginManagerDelegate.AddPluginUpdateCallback(
        [](int64_t id, std::string data) {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginUpdateCallback_ != nullptr);
}

/**
 * @tc.name: PluginManagerDelegateAddPluginUpdateCallback002
 * @tc.desc: Verify the AddPluginUpdateCallback Interface of PluginManagerDelegate not work because state is RELEASED.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateAddPluginUpdateCallback002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Update Callback.
     * @tc.expected: step2. Add Plugin Update Callback not work.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::RELEASED;
    pluginManagerDelegate.AddPluginUpdateCallback(
        [](int64_t id, std::string data) {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginUpdateCallback_ == nullptr);
}

/**
 * @tc.name: PluginManagerDelegateAddPluginErrorCallback001
 * @tc.desc: Verify the AddPluginErrorCallback Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateAddPluginErrorCallback001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Error Callback.
     * @tc.expected: step2.  Add Plugin Error Callback success.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginErrorCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::CREATED;
    pluginManagerDelegate.AddPluginErrorCallback(
        [](std::string code, std::string msg) {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginErrorCallback_ != nullptr);
}

/**
 * @tc.name: PluginManagerDelegateAddPluginErrorCallback002
 * @tc.desc: Verify the AddPluginErrorCallback Interface of PluginManagerDelegate not work because state is RELEASED.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateAddPluginErrorCallback002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Add Plugin Update Callback.
     * @tc.expected: step2. Add Plugin Update Callback not work.
     */
    EXPECT_TRUE(pluginManagerDelegate.onPluginCompleteCallback_ == nullptr);
    pluginManagerDelegate.state_ = PluginManagerDelegate::State::RELEASED;
    pluginManagerDelegate.AddPluginErrorCallback(
        [](std::string code, std::string msg) {});
    EXPECT_TRUE(pluginManagerDelegate.onPluginErrorCallback_ == nullptr);
}

/**
 * @tc.name: PluginManagerDelegateOnPluginAcquired001
 * @tc.desc: Verify the OnPluginAcquired Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateOnPluginAcquired001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. On Plugin Acquired.
     * @tc.expected: step2.  On Plugin Acquired success. no expect because Plugin Complete Call Back has no param
     */
    pluginManagerDelegate.onPluginCompleteCallback_ = []() {
    };
    pluginManagerDelegate.OnPluginComplete("1#HWJS-=-#");
}

/**
 * @tc.name: PluginManagerDelegateOnPluginUpdate001
 * @tc.desc: Verify the OnPluginUpdate Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateOnPluginUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. On Plugin Update.
     * @tc.expected: step2.  On Plugin Update succcess.
     */
    pluginManagerDelegate.onPluginUpdateCallback_ = [](int64_t id, std::string data) {
        EXPECT_EQ(id, 0);
        EXPECT_EQ(data, "");
    };
    pluginManagerDelegate.OnPluginUpdate("1#HWJS-=-#");
}

/**
 * @tc.name: PluginManagerDelegateOnPluginError001
 * @tc.desc: Verify the OnPluginError Interface of PluginManagerDelegate work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateOnPluginError001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. On Plugin Error.
     * @tc.expected: step2.  On Plugin Error correctly.
     */
    pluginManagerDelegate.onPluginErrorCallback_ = [](std::string code, std::string data) {
        EXPECT_EQ(code, "-1");
        EXPECT_EQ(data, "1#HWJS-=-#");
    };
    pluginManagerDelegate.OnPluginError("1#HWJS-=-#");
}


/**
 * @tc.name: PluginManagerDelegateRegisterEvent001
 * @tc.desc: Verify the RegisterEvent Interface of PluginManagerDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginManagerDelegateTest, PluginManagerDelegateRegisterEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Plugin Manager Delegate.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginManagerDelegateTest::GetPipelineContext(pluginFrontend);
    PluginManagerDelegate pluginManagerDelegate(pipelineContext);

    /**
     * @tc.steps: step2. Register Event.
     * @tc.expected: step2.  Register Event Success.
     */
    pluginManagerDelegate.onPluginCompleteCallback_ = []() {
    };
    pluginManagerDelegate.onPluginUpdateCallback_ = [](int64_t id, std::string data) {
        EXPECT_EQ(id, 0);
        EXPECT_EQ(data, "");
    };
    pluginManagerDelegate.onPluginErrorCallback_ = [](std::string code, std::string data) {
        EXPECT_EQ(code, "");
        EXPECT_EQ(data, "");
    };
    pluginManagerDelegate.RegisterEvent();
}
} // namespace OHOS::Ace