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
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"
#undef private
#undef protected
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const std::string LABEL = "task executor";
const std::string THREADFIRST = "thread_1";
const std::string THREADSECOND = "thread_2";
const std::string THREADTHIRD = "thread_3";
const std::string THREADFOURTH = "thread_4";
const uint8_t ERROR = -1;
const uint8_t DEFAULT = 0;
const uint8_t PUSH = 1;
const uint8_t REPLACE = 2;
const uint8_t BACK = 3;
} // namespace
class PluginFrontendTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginFrontendTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
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
 * @tc.name: OnBackPressedTest001
 * @tc.desc: Verify the OnBackPressed Interface of PluginFrontend work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnBackPressedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Back Pressed.
     * @tc.expected: step2. On Back Pressed return false.
     */
    EXPECT_EQ(pluginFrontend.OnBackPressed(), false);
}

/**
 * @tc.name: OnShowTest001
 * @tc.desc: Verify the OnShow Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnShowTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Show.
     * @tc.expected: step2. On Show success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnShow();
}

/**
 * @tc.name: OnShowTest002
 * @tc.desc: Verify the OnShow Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnShowTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Show.
     * @tc.expected: step2. On Show fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnShow();
}

/**
 * @tc.name: OnHideTest001
 * @tc.desc: Verify the OnHide Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnHideTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Hide.
     * @tc.expected: step2. On Hide success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnHide();
}

/**
 * @tc.name: OnHideTest002
 * @tc.desc: Verify the OnHide Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnHideTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Hide.
     * @tc.expected: step2. On Hide fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnHide();
}

/**
 * @tc.name: OnConfigurationUpdatedTest001
 * @tc.desc: Verify the OnConfigurationUpdated Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnConfigurationUpdatedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Configuration Updated.
     * @tc.expected: step2. On Configuration Updated success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnConfigurationUpdated("data");
}

/**
 * @tc.name: OnConfigurationUpdatedTest002
 * @tc.desc: Verify the OnHide Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnConfigurationUpdatedTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Configuration Updated.
     * @tc.expected: step2. On Configuration Updated fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnConfigurationUpdated("data");
}

/**
 * @tc.name: OnActiveTest001
 * @tc.desc: Verify the On Active Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnActiveTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Active.
     * @tc.expected: step2. On Configuration Updated success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnActive();
    EXPECT_EQ(pluginFrontend.foregroundFrontend_, true);
}

/**
 * @tc.name: OnActiveTest002
 * @tc.desc: Verify the OnActive Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnActiveTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Active.
     * @tc.expected: step2. On Active fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnActive();
    EXPECT_EQ(pluginFrontend.foregroundFrontend_, false);
}

/**
 * @tc.name: OnInactiveTest001
 * @tc.desc: Verify the On OnInactive Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnInactiveTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Inactive.
     * @tc.expected: step2. On Inactive success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnInactive();
}

/**
 * @tc.name: OnInactiveTest002
 * @tc.desc: Verify the OnInactive Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnInactiveTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Active.
     * @tc.expected: step2. On Active fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnInactive();
}

/**
 * @tc.name: OnStartContinuationTest001
 * @tc.desc: Verify the On OnStartContinuation Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnStartContinuationTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Start Continuation.
     * @tc.expected: step2. On Start Continuation fail.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    EXPECT_EQ(pluginFrontend.OnStartContinuation(), false);
}

/**
 * @tc.name: OnCompleteContinuationTest001
 * @tc.desc: Verify the OnCompleteContinuation Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnCompleteContinuationTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2.On Complete Continuation.
     * @tc.expected: step2. On Complete Continuation success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnCompleteContinuation(0);
}

/**
 * @tc.name: OnCompleteContinuationTest002
 * @tc.desc: Verify the OnCompleteContinuation Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnCompleteContinuationTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Complete Continuation.
     * @tc.expected: step2. On Complete Continuation fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnCompleteContinuation(0);
}

/**
 * @tc.name: OnMemoryLevelTest001
 * @tc.desc: Verify the OnMemoryLevel Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnMemoryLevelTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Memory Level.
     * @tc.expected: step2. On Memory Level success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnMemoryLevel(0);
}

/**
 * @tc.name: OnMemoryLevelTest002
 * @tc.desc: Verify the OnMemoryLevel Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnMemoryLevelTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Memory Level.
     * @tc.expected: step2. On Memory Level fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnMemoryLevel(0);
}

/**
 * @tc.name: OnSaveDataTest001
 * @tc.desc: Verify the OnSaveData Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSaveDataTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Save Data.
     * @tc.expected: step2. On Save Data success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string data = "data";
    pluginFrontend.OnSaveData(data);
}

/**
 * @tc.name: OnMemoryLevelTest002
 * @tc.desc: Verify the OnMemoryLevel Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSaveDataTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Memory Level.
     * @tc.expected: step2. On Memory Level fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string data = "data";
    pluginFrontend.OnSaveData(data);
}

/**
 * @tc.name: GetPluginsUsedTest001
 * @tc.desc: Verify the GetPluginsUsed Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, GetPluginsUsedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Get Plugins Used.
     * @tc.expected: step2. Get Plugins Used success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string data = "data";
    pluginFrontend.GetPluginsUsed(data);
}

/**
 * @tc.name: GetPluginsUsedTest002
 * @tc.desc: Verify the GetPluginsUsed Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, GetPluginsUsedTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Get Plugins Used.
     * @tc.expected: step2. Get Plugins Used fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string data = "data";
    pluginFrontend.GetPluginsUsed(data);
}

/**
 * @tc.name: OnRestoreDataTest001
 * @tc.desc: Verify the OnRestoreDataTest001 Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnRestoreDataTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Get Plugins Used.
     * @tc.expected: step2. Get Plugins Used fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string data = "data";
    EXPECT_EQ(pluginFrontend.OnRestoreData(data), false);
}

/**
 * @tc.name: OnRemoteTerminatedTest001
 * @tc.desc: Verify the OnRemoteTerminated Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnRemoteTerminatedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Remote Terminated.
     * @tc.expected: step2. On Remote Terminated success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnRemoteTerminated();
}

/**
 * @tc.name: OnRemoteTerminatedTest002
 * @tc.desc: Verify the GetPluginsUsed Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnRemoteTerminatedTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Remote Terminated.
     * @tc.expected: step2. On Remote Terminated fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnRemoteTerminated();
}

/**
 * @tc.name: OnNewRequestTest001
 * @tc.desc: Verify the OnNewRequest Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnNewRequestTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Remote Terminated.
     * @tc.expected: step2. On Remote Terminated success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string data = "data";
    pluginFrontend.OnNewRequest(data);
}

/**
 * @tc.name: OnNewRequestTest002
 * @tc.desc: Verify the OnNewRequest Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnNewRequestTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On New Request.
     * @tc.expected: step2. On New Request fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string data = "data";
    pluginFrontend.OnNewRequest(data);
}

/**
 * @tc.name: CallRouterBackTest001
 * @tc.desc: Verify the CallRouterBack Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, CallRouterBackTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Call Router Back.
     * @tc.expected: step2. Call Router Back success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.CallRouterBack();
}

/**
 * @tc.name: CallRouterBackTest002
 * @tc.desc: Verify the CallRouterBack Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, CallRouterBackTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Call Router Back.
     * @tc.expected: step2. Call Router Back fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.CallRouterBack();
}

/**
 * @tc.name: CallRouterBackTest003
 * @tc.desc: Verify the CallRouterBack Interface of PluginFrontend work fail because this is the last page of sub
 *           window.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, CallRouterBackTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Call Router Back.
     * @tc.expected: step2. Call Router Back fail.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.isSubWindow_ = true;
    pluginFrontend.CallRouterBack();
}

/**
 * @tc.name: OnSurfaceChangedTest001
 * @tc.desc: Verify the OnSurfaceChanged Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSurfaceChangedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Surface Changed.
     * @tc.expected: step2. On Surface Changed success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnSurfaceChanged(0, 0);
}

/**
 * @tc.name: OnSurfaceChangedTest002
 * @tc.desc: Verify the OnSurfaceChanged Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSurfaceChangedTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Surface Changed.
     * @tc.expected: step2. On Surface Changed fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnSurfaceChanged(0, 0);
}

/**
 * @tc.name: DumpFrontendTest001
 * @tc.desc: Verify the DumpFrontend Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, DumpFrontendTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Dump Frontend.
     * @tc.expected: step2. Dump Frontend success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.DumpFrontend();
}

/**
 * @tc.name: DumpFrontendTest002
 * @tc.desc: Verify the DumpFrontend Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, DumpFrontendTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Dump Frontend.
     * @tc.expected: step2. Dump Frontend fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.DumpFrontend();
}

/**
 * @tc.name: SetColorModeTest002
 * @tc.desc: Verify the DumpFrontend Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, SetColorModeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Set Color Mode.
     * @tc.expected: step2. Set Color Mode fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.SetColorMode(ColorMode::LIGHT);
}

/**
 * @tc.name: RebuildAllPagesTest001
 * @tc.desc: Verify the RebuildAllPages Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, RebuildAllPagesTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Rebuild AllPagese.
     * @tc.expected: step2. Rebuild AllPages success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.RebuildAllPages();
}

/**
 * @tc.name: RebuildAllPagesTest002
 * @tc.desc: Verify the RebuildAllPages Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, RebuildAllPagesTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Rebuild All Pages.
     * @tc.expected: step2. Rebuild All Pages fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.RebuildAllPages();
}

/**
 * @tc.name: NotifyAppStorageTest001
 * @tc.desc: Verify the NotifyAppStorage Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NotifyAppStorageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Notify App Storage.
     * @tc.expected: step2. Notify App Storage success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.NotifyAppStorage("key", "value");
}

/**
 * @tc.name: NotifyAppStorageTest002
 * @tc.desc: Verify the NotifyAppStorage Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NotifyAppStorageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Notify App Storage.
     * @tc.expected: step2. Notify App Storage fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.NotifyAppStorage("key", "value");
}

/**
 * @tc.name: RunPageTest001
 * @tc.desc: Verify the RunPage Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, RunPageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Run Page.
     * @tc.expected: step2. Run Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.RunPage(0, "url", "params");
}

/**
 * @tc.name: RunPageTest002
 * @tc.desc: Verify the RunPage Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, RunPageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Run Page.
     * @tc.expected: step2. Run Page fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.RunPage(0, "url", "params");
}

/**
 * @tc.name: ReplacePageTest001
 * @tc.desc: Verify the ReplacePage Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, ReplacePageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Replace Page.
     * @tc.expected: step2. Replace Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.ReplacePage("url", "params");
}

/**
 * @tc.name: ReplacePageTest002
 * @tc.desc: Verify the Replace Page Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, ReplacePageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Replace Page.
     * @tc.expected: step2. Replace Page fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.ReplacePage("url", "params");
}

/**
 * @tc.name: PushPageTest001
 * @tc.desc: Verify the PushPage Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, PushPageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Push Page.
     * @tc.expected: step2. Push Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.ReplacePage("url", "params");
}

/**
 * @tc.name: PushPageTest002
 * @tc.desc: Verify the Push Page Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, PushPageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Push Page.
     * @tc.expected: step2. Push Page fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.PushPage("url", "params");
}

/**
 * @tc.name: NavigatePageTest001
 * @tc.desc: Verify the NavigatePage Interface of PluginFrontend work success type is PUSH.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NavigatePageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Navigate Page.
     * @tc.expected: step2. Navigate Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.NavigatePage(PUSH, PageTarget("uri"), "params");
}

/**
 * @tc.name: NavigatePageTest002
 * @tc.desc: Verify the NavigatePage Interface of PluginFrontend work success type is REPLACE.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NavigatePageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Navigate Page.
     * @tc.expected: step2. Navigate Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.NavigatePage(REPLACE, PageTarget("uri"), "params");
}

/**
 * @tc.name: NavigatePageTest003
 * @tc.desc: Verify the NavigatePage Interface of PluginFrontend work success type is BACK.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NavigatePageTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Navigate Page.
     * @tc.expected: step2. Navigate Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend. NavigatePage(BACK, PageTarget("uri"), "params");
}

/**
 * @tc.name: NavigatePageTest004
 * @tc.desc: Verify the NavigatePage Interface of PluginFrontend work success type is DEFAULT.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NavigatePageTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Navigate Page.
     * @tc.expected: step2. Navigate Page success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend. NavigatePage(DEFAULT, PageTarget("uri"), "params");
}

/**
 * @tc.name: NavigatePageTest005
 * @tc.desc: Verify the NavigatePage Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, NavigatePageTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Send Callback Message.
     * @tc.expected: step2. Send Callback Message fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.NavigatePage(DEFAULT, PageTarget("uri"), "params");
}

/**
 * @tc.name: SendCallbackMessageTest001
 * @tc.desc: Verify the SendCallbackMessage Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, SendCallbackMessageTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Send Callback Message.
     * @tc.expected: step2. Send Callback Message success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.SendCallbackMessage("callbackId", "data");
}

/**
 * @tc.name: SendCallbackMessageTest002
 * @tc.desc: Verify the SendCallbackMessage Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, SendCallbackMessageTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Send Callback Message.
     * @tc.expected: step2. Send Callback Message fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.SendCallbackMessage("callbackId", "data");
}

/**
 * @tc.name: TransferComponentResponseDataTest001
 * @tc.desc: Verify the TransferComponentResponseData Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferComponentResponseDataTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();

    /**
     * @tc.steps: step2. Transfer Component Response Data.
     * @tc.expected: step2. Transfer Component Response Data success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend->Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend->delegate_ != nullptr);
    std::vector<uint8_t> vec = { 0, 0 };
    pluginFrontend->delegate_->AttachPipelineContext(PluginFrontendTest::GetPipelineContext(pluginFrontend));
    pluginFrontend->TransferComponentResponseData(0, 0, std::move(vec));
}

/**
 * @tc.name: TransferComponentResponseDataTest002
 * @tc.desc: Verify the TransferComponentResponseData Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferComponentResponseDataTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Component Response Data.
     * @tc.expected: step2. Transfer Component Response Data fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::vector<uint8_t> vec { 0, 0 };
    pluginFrontend.TransferComponentResponseData(0, 0, std::move(vec));
}

/**
 * @tc.name: TransferJsResponseDataTest001
 * @tc.desc: Verify the TransferJsResponseData Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsResponseDataTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Response Data.
     * @tc.expected: step2. Transfer Js ResponseData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::vector<uint8_t> vec = { 0, 0 };
    pluginFrontend.TransferJsResponseData(0, 0, std::move(vec));
}

/**
 * @tc.name: TransferJsResponseDataTest002
 * @tc.desc: Verify the TransferJsResponseData Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsResponseDataTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Response Data.
     * @tc.expected: step2. Transfer Js Response Data fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::vector<uint8_t> vec { 0, 0 };
    pluginFrontend.TransferJsResponseData(0, 0, std::move(vec));
}

/**
 * @tc.name: TransferJsPluginGetErrorTest001
 * @tc.desc: Verify the TransferJsPluginGetError Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsPluginGetErrorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Response Data.
     * @tc.expected: step2. Transfer Js ResponseData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string errorMessage {"errorMessage"};
    pluginFrontend.TransferJsPluginGetError(0, 0, std::move(errorMessage));
}

/**
 * @tc.name: TransferJsPluginGetErrorTest002
 * @tc.desc: Verify the TransferJsPluginGetError Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsPluginGetErrorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Response Data.
     * @tc.expected: step2. Transfer Js Response Data fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string errorMessage {"errorMessage"};
    pluginFrontend.TransferJsPluginGetError(0, 0, std::move(errorMessage));
}

/**
 * @tc.name: TransferJsEventDataTest001
 * @tc.desc: Verify the TransferJsEventData Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsEventDataTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::vector<uint8_t> vec { 0, 0 };
    pluginFrontend.TransferJsEventData(0, 0, std::move(vec));
}

/**
 * @tc.name: TransferJsEventDataTest002
 * @tc.desc: Verify the TransferJsEventData Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, TransferJsEventDataTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js Event Data fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::vector<uint8_t> vec { 0, 0 };
    pluginFrontend.TransferJsEventData(0, 0, std::move(vec));
}

/**
 * @tc.name: LoadPluginJsCodeTest001
 * @tc.desc: Verify the LoadPluginJsCode Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, LoadPluginJsCodeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string vecJsCode { "JsCode" };
    pluginFrontend.LoadPluginJsCode(std::move(vecJsCode));
}

/**
 * @tc.name: LoadPluginJsCodeTest002
 * @tc.desc: Verify the LoadPluginJsCode Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, LoadPluginJsCodeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Load Plugin Js Code.
     * @tc.expected: step2. Load Plugin Js Code fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string vecJsCode {"JsCode"};
    pluginFrontend.LoadPluginJsCode(std::move(vecJsCode));
}

/**
 * @tc.name: UpdateStateTest001
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work success type is ON_CREATE.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, UpdateStateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.UpdateState(Frontend::State::ON_CREATE);
}

/**
 * @tc.name: UpdateStateTest002
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work success type is ON_SHOW.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, UpdateStateTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.UpdateState(Frontend::State::ON_SHOW);
}

/**
 * @tc.name: UpdateStateTest003
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work success type is ON_HIDE.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, LoadPluginJsCodeTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.UpdateState(Frontend::State::ON_HIDE);
}

/**
 * @tc.name: UpdateStateTest004
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work success type is ON_DESTROY.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, UpdateStateTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.UpdateState(Frontend::State::ON_DESTROY);
}

/**
 * @tc.name: UpdateStateTest005
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work success type is ERROR.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, UpdateStateTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Transfer Js Event Data.
     * @tc.expected: step2. Transfer Js EventData success.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.UpdateState(static_cast<Frontend::State>(ERROR));
}

/**
 * @tc.name: UpdateStateTest006
 * @tc.desc: Verify the UpdateState Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, UpdateStateTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Load Plugin Js Code.
     * @tc.expected: step2. Load Plugin Js Code fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.UpdateState(Frontend::State::ON_HIDE);
}

/**
 * @tc.name: LoadPluginJsByteCodeTest001
 * @tc.desc: Verify the LoadPluginJsByteCode Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, LoadPluginJsByteCodeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Load Plugin Js Byte Code.
     * @tc.expected: step2. Load Plugin Js Byte Code success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::vector<uint8_t> vecJsCode {0, 0};
    std::vector<int32_t> vecJsCodeLen {0, 0};
    pluginFrontend.LoadPluginJsByteCode(std::move(vecJsCode), std::move(vecJsCodeLen));
}

/**
 * @tc.name: LoadPluginJsByteCodeTest002
 * @tc.desc: Verify the LoadPluginJsByteCode Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, LoadPluginJsByteCodeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. Load Plugin Js Code.
     * @tc.expected: step2. Load Plugin Js Code fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::vector<uint8_t> vecJsCode {0, 0};
    std::vector<int32_t> vecJsCodeLen {0, 0};
    pluginFrontend.LoadPluginJsByteCode(std::move(vecJsCode), std::move(vecJsCodeLen));
}

/**
 * @tc.name: OnWindowDisplayModeChangedTest001
 * @tc.desc: Verify the OnWindowDisplayModeChanged Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnWindowDisplayModeChangedTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Window Display ModeChanged.
     * @tc.expected: step2. On Window Display Mode Changed success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnWindowDisplayModeChanged(true, "data");
}

/**
 * @tc.name: OnWindowDisplayModeChangedTest002
 * @tc.desc: Verify the OnWindowDisplayModeChanged Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnWindowDisplayModeChangedTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Window Display ModeChanged.
     * @tc.expected: step2. On Window Display ModeChanged fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnWindowDisplayModeChanged(true, "data");
}

/**
 * @tc.name: OnSaveAbilityStateTest001
 * @tc.desc: Verify the OnSaveAbilityState Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSaveAbilityStateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Save Ability State.
     * @tc.expected: step2.On Save Ability State success.
     */
    RefPtr<TaskExecutor> taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    std::string data = "data";
    pluginFrontend.OnSaveAbilityState(data);
}

/**
 * @tc.name: OnSaveAbilityStateTest002
 * @tc.desc: Verify the Send OnSaveAbilityState Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnSaveAbilityStateTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Save Ability State.
     * @tc.expected: step2. On Save Ability State fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    std::string data = "data";
    pluginFrontend.OnSaveAbilityState(data);
}

/**
 * @tc.name: OnRestoreAbilityStateTest001
 * @tc.desc: Verify the OnRestoreAbilityState Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnRestoreAbilityStateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Restore Ability State.
     * @tc.expected: step2. On Restore Ability State success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnRestoreAbilityState("data");
}

/**
 * @tc.name: OnRestoreAbilityStateTest002
 * @tc.desc: Verify the Send OnRestoreAbilityState Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnRestoreAbilityStateTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On Restore Ability State.
     * @tc.expected: step2. On Restore Ability State fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnRestoreAbilityState("data");
}

/**
 * @tc.name: OnNewWantTest001
 * @tc.desc: Verify the OnNewWant Interface of PluginFrontend work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnNewWantTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On New Want Message.
     * @tc.expected: step2. On New Want success.
     */
    auto taskExecutor =  Referenced::MakeRefPtr<FlutterTaskExecutor>();
    std::string label = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform = ThreadFirst->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu = ThreadSecond->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui = ThreadThird->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io = ThreadFourth->GetTaskRunner();
    flutter::TaskRunners taskRunner(label, platform, gpu, ui, io);
    taskExecutor->InitJsThread(true);
    taskExecutor->InitOtherThreads(taskRunner);
    pluginFrontend.Initialize(FrontendType::JS_PLUGIN, taskExecutor);
    EXPECT_TRUE(pluginFrontend.delegate_ != nullptr);
    pluginFrontend.OnNewWant("data");
}

/**
 * @tc.name: OnNewWantTest002
 * @tc.desc: Verify the Send Callback Message Interface of PluginFrontend work fail because delegate is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendTest, OnNewWantTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontend.
     */
    PluginFrontend pluginFrontend;

    /**
     * @tc.steps: step2. On New Want.
     * @tc.expected: step2. On New Want fail.
     */
    EXPECT_TRUE(pluginFrontend.delegate_ == nullptr);
    pluginFrontend.OnNewWant("data");
}
} // namespace OHOS::Ace