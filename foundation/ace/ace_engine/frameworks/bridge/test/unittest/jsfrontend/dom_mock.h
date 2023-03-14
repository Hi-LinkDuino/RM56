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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_MOCK_H

#include "base/subwindow/subwindow.h"
#include "core/common/frontend.h"
#include "core/common/platform_window.h"
#include "core/common/window.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

RefPtr<Subwindow> Subwindow::CreateSubwindow(int32_t instanceId)
{
    return nullptr;
}

} // namespace OHOS::Ace
namespace OHOS::Ace::Framework {

class MockPlatformWindow : public PlatformWindow {
public:
    MockPlatformWindow() = default;
    ~MockPlatformWindow() override = default;

    // Request next vsync.
    void RequestFrame() override {};

    // Register Vsync callback.
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override {};

    // Attach root render node to container
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override {};
};

class MockFrontend : public Frontend {
public:
    MockFrontend() = default;
    ~MockFrontend() override = default;

    bool Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor) override
    {
        return false;
    }

    void Destroy() override {}

    RefPtr<AcePage> GetPage(int32_t pageId) const override
    {
        return nullptr;
    }

    RefPtr<AceEventHandler> GetEventHandler() override
    {
        return nullptr;
    }

    WindowConfig& GetWindowConfig() override
    {
        return windowConfig_;
    }

    FrontendType GetType() override
    {
        return FrontendType::JS;
    }

    bool OnBackPressed() override
    {
        return false;
    }

    bool OnStartContinuation() override
    {
        return false;
    }

    bool IsForeground() override
    {
        return false;
    }

    RefPtr<AccessibilityManager> GetAccessibilityManager() const override
    {
        return nullptr;
    }

    bool OnRestoreData(const std::string& data) override
    {
        return false;
    }

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override {}
    void SetAssetManager(const RefPtr<AssetManager>& assetManager) override {}
    void DumpFrontend() const override {}
    void AddPage(const RefPtr<AcePage>& page) override {}
    void RunPage(int32_t pageId, const std::string& content, const std::string& params) override {}
    void PushPage(const std::string& content, const std::string& params) override {}
    void ReplacePage(const std::string& url, const std::string& params) override {}
    void UpdateState(State state) override {}
    void SendCallbackMessage(const std::string& callbackId, const std::string& data) const override {}
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& transfer) const override {}
    void TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const override {}
    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void LoadPluginJsCode(std::string&& jsCode) const override {}
    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const override {}
    void OnShow() override {}
    void OnHide() override {}
    void OnActive() override {}
    void OnInactive() override {}
    void OnCompleteContinuation(int32_t code) override {}
    void OnMemoryLevel(const int32_t level) override {}
    void OnSaveData(std::string& data) override {}
    void OnSaveAbilityState (std::string& data) override {}
    void OnRestoreAbilityState (const std::string& data) override {}
    void OnRemoteTerminated() override {}
    void OnNewRequest(const std::string& data) override {}
    void OnNewWant(const std::string& data) override {}
    void CallRouterBack() override {}
    void OnSurfaceChanged(int32_t width, int32_t height) override {}

private:
    WindowConfig windowConfig_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_JSFRONTEND_DOM_MOCK_H
