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
#include "core/mock/fake_asset_manager.h"
#include "core/mock/mock_resource_register.h"
#define private public
#define protected public
#include "frameworks/bridge/plugin_frontend/plugin_frontend_delegate.h"
#undef private
#undef protected
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
constexpr int32_t INVALID_PAGE_ID = -1;
} // namespace
class PluginFrontendDelegateTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    static RefPtr<Framework::PluginFrontendDelegate> GetPluginFrontendDelegate
                                                    (const RefPtr<TaskExecutor>& taskExecutor);
    static RefPtr<PipelineContext> GetPipelineContext(const RefPtr<PluginFrontend>& frontend);
};

RefPtr<PipelineContext> PluginFrontendDelegateTest::GetPipelineContext(const RefPtr<PluginFrontend>& frontend)
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

RefPtr<Framework::PluginFrontendDelegate> PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                        (const RefPtr<TaskExecutor>& taskExecutor)
{
    auto& loader = Framework::JsEngineLoader::Get("libace_engine_qjs.z.so");
    auto jsEngine = loader.CreateJsEngine(0);
    const auto& loadCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](const std::string& url,
                                   const RefPtr<Framework::JsAcePage>& jsPage, bool isMainPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->LoadJs(url, jsPage, isMainPage);
    };

    const auto& setPluginMessageTransferCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                       const RefPtr<JsMessageDispatcher>& dispatcher) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->SetJsMessageDispatcher(dispatcher);
    };

    const auto& asyncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                         const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireAsyncEvent(eventId, param);
    };

    const auto& syncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                        const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireSyncEvent(eventId, param);
    };

    const auto& updatePageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                         const RefPtr<Framework::JsAcePage>& jsPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateRunningPage(jsPage);
        jsEngine->UpdateStagingPage(jsPage);
    };

    const auto& resetStagingPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->ResetStagingPage();
    };

    const auto& destroyPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](int32_t pageId) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyPageInstance(pageId);
    };

    const auto& destroyApplicationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                 const std::string& packageName) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyApplication(packageName);
    };

    const auto& updateApplicationStateCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                     const std::string& packageName, Frontend::State state) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateApplicationState(packageName, state);
    };

    const auto& onWindowDisplayModeChangedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                         bool isShownInMultiWindow, const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnWindowDisplayModeChanged(isShownInMultiWindow, data);
    };

    const auto& onSaveAbilityStateCallBack  = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                    std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnSaveAbilityState(data);
    };
    const auto& onRestoreAbilityStateCallBack  = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                    const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnRestoreAbilityState(data);
    };

    const auto& onNewWantCallBack  = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                    const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnNewWant(data);
    };
    const auto& onActiveCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnActive();
    };

    const auto& onInactiveCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnInactive();
    };

    const auto& onConfigurationUpdatedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                     const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnConfigurationUpdated(data);
    };

    const auto& timerCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                    const std::string& callbackId, const std::string& delay, bool isInterval) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->TimerCallback(callbackId, delay, isInterval);
    };

    const auto& mediaQueryCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                         const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->MediaQueryCallback(callbackId, args);
    };

    const auto& requestAnimationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                               const std::string& callbackId, uint64_t timeStamp) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->RequestAnimationCallback(callbackId, timeStamp);
    };

    const auto& jsCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                 const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->JsCallback(callbackId, args);
    };

    const auto& onMemoryLevelCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](
                                                     const int32_t level) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnMemoryLevel(level);
    };

    const auto& onStartContinuationCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)]() -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnStartContinuation();
    };
    const auto& onCompleteContinuationCallBack =
        [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](int32_t code) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnCompleteContinuation(code);
    };
    const auto& onRemoteTerminatedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnRemoteTerminated();
    };
    const auto& onSaveDataCallBack =
        [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](std::string& savedData) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnSaveData(savedData);
    };
    const auto& onRestoreDataCallBack =
        [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine)](const std::string& data) -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnRestoreData(data);
    };
    RefPtr<Framework::PluginFrontendDelegate> delegate = AceType::MakeRefPtr<Framework::PluginFrontendDelegate>(
        taskExecutor, loadCallback,
        setPluginMessageTransferCallback, asyncEventCallback, syncEventCallback, updatePageCallback,
        resetStagingPageCallback, destroyPageCallback, destroyApplicationCallback, updateApplicationStateCallback,
        timerCallback, mediaQueryCallback, requestAnimationCallback, jsCallback, onWindowDisplayModeChangedCallBack,
        onConfigurationUpdatedCallBack, onSaveAbilityStateCallBack, onRestoreAbilityStateCallBack,
        onNewWantCallBack, onActiveCallBack, onInactiveCallBack, onMemoryLevelCallBack, onStartContinuationCallBack,
        onCompleteContinuationCallBack, onRemoteTerminatedCallBack, onSaveDataCallBack, onRestoreDataCallBack);
    return delegate;
}

/**
 * @tc.name: GetMinPlatformVersionTest001
 * @tc.desc: Verify the GetMinPlatformVersion Interface of PluginFrontendDelegate work correctly.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, GetMinPlatformVersionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. GetMinPlatformVersion.
     * @tc.expected: step2. Get Min Platform Version success.
     */
    EXPECT_EQ(delegate->GetMinPlatformVersion(), 0);
}

/**
 * @tc.name: AttachPipelineContextTest001
 * @tc.desc: Verify the AttachPipelineContext Interface of PluginFrontendDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, AttachPipelineContextTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. GetMinPlatformVersion.
     * @tc.expected: step2. Get Min Platform Version success.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginFrontendDelegateTest::GetPipelineContext(pluginFrontend);
    delegate->AttachPipelineContext(pipelineContext);
    EXPECT_TRUE(delegate->GetPipelineContext() != nullptr);
}

/**
 * @tc.name: GetUiTask001
 * @tc.desc: Verify the GetUiTask Interface of PluginFrontendDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, GetUiTask001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. Get Ui Task.
     * @tc.expected: step2. Get Ui Task success.
     */
    RefPtr<PluginFrontend> pluginFrontend = Referenced::MakeRefPtr<PluginFrontend>();
    auto pipelineContext = PluginFrontendDelegateTest::GetPipelineContext(pluginFrontend);

    EXPECT_TRUE(delegate->GetUiTask().GetTaskExecutor() != nullptr);
}

/**
 * @tc.name: GetAnimationJsTask001
 * @tc.desc: Verify the GetAnimationJsTask Interface of PluginFrontendDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, GetAnimationJsTask001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. Get Animation Js Task.
     * @tc.expected: step2. Get Animation JsTask success.
     */
    EXPECT_TRUE(delegate->GetAnimationJsTask().GetTaskExecutor() != nullptr);
}

/**
 * @tc.name: Initialize001
 * @tc.desc: Verify the Initialize Interface of PluginFrontendDelegate work success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, Initialize001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. Get Running Page Id.
     * @tc.expected: step2. Get Running PageId success.
     */
    delegate->pageRouteStack_.emplace_back(Ace::Framework::PageInfo { 0, "url"});
    EXPECT_EQ(delegate->GetRunningPageId(), 0);
}

/**
 * @tc.name: GetRunningPageId002
 * @tc.desc: Verify the GetRunningPageId Interface of PluginFrontendDelegate get invalid page id because page vector
 *           is empty.
 * @tc.type: FUNC
 */
HWTEST_F(PluginFrontendDelegateTest, GetRunningPageId002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a PluginFrontendDelegate.
     */
    auto taskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    RefPtr<Framework::PluginFrontendDelegate> delegate = PluginFrontendDelegateTest::GetPluginFrontendDelegate
                                                                                    (taskExecutor);
    EXPECT_TRUE(delegate != nullptr);
    /**
     * @tc.steps: step2. Get Running Page Id.
     * @tc.expected: step2. Get invalid page id because page vector is empty.
     */
    EXPECT_EQ(delegate->GetRunningPageId(), INVALID_PAGE_ID);
}
} // namespace OHOS::Ace