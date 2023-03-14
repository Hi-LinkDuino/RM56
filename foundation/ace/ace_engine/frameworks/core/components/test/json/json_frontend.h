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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_FRONTEND_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_FRONTEND_H

#include <unordered_map>

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/common/ace_page.h"
#include "core/common/container.h"
#include "core/common/frontend.h"
#include "core/common/js_message_dispatcher.h"

namespace OHOS::Ace {

class ACE_EXPORT JsonFrontend : public Frontend {
    DECLARE_ACE_TYPE(JsonFrontend, Frontend);

public:
    JsonFrontend() = default;
    ~JsonFrontend() override = default;

    bool Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor) override;

    void Destroy() override {}

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override;

    void SetAssetManager(const RefPtr<AssetManager>& assetManager) override;

    void AddPage(const RefPtr<AcePage>& page) override;

    RefPtr<AcePage> GetPage(int32_t pageId) const override;

    void RunPage(int32_t pageId, const std::string& content, const std::string& params) override;

    void PushPage(const std::string& content, const std::string& params) override;

    void UpdatePage(int32_t pageId, const std::string& content);

    RefPtr<AceEventHandler> GetEventHandler() override;

    void UpdateState(Frontend::State state) override;

    bool IsForeground() override;

    RefPtr<AccessibilityManager> GetAccessibilityManager() const override;

    bool OnBackPressed() override;

    void OnShow() override;

    void OnHide() override;

    void OnActive() override;

    void OnInactive() override;

    bool OnStartContinuation() override;

    void OnCompleteContinuation(int32_t code) override;

    void OnMemoryLevel(const int32_t level) override {}

    void OnSaveData(std::string& data) override;

    void OnSaveAbilityState (std::string& data) override {}

    void OnRestoreAbilityState (const std::string& data) override {}

    bool OnRestoreData(const std::string& data) override;

    void OnRemoteTerminated() override {}

    void OnNewRequest(const std::string& data) override;

    void OnNewWant(const std::string& data) override {}

    void CallRouterBack() override;

    void OnSurfaceChanged(int32_t width, int32_t height) override {}

    void ReplacePage(const std::string& url, const std::string& params) override {}

    void DumpFrontend() const override
    {
        // maybe will implement later,
        LOGI("json not support dump now.");
    }

    void SetEventHandler(const RefPtr<AceEventHandler>& handler)
    {
        handler_ = handler;
    }

    WindowConfig& GetWindowConfig() override
    {
        return windowConfig_;
    }

    FrontendType GetType() override
    {
        return type_;
    }

    void SendCallbackMessage(const std::string& callbackId, const std::string& data) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override
    {
        // maybe will implement later
        LOGI("json frontend not support.");
    }

    void LoadPluginJsCode(std::string&& jsCode) const override
    {
        LOGI("json frontend not support.");
    }

    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const override
    {
        LOGI("json frontend not support.");
    }

private:
    FrontendType type_ { FrontendType::JSON };
    static constexpr int32_t JSON_DESIGN_WIDTH = 1080;
    std::unordered_map<int, RefPtr<AcePage>> pageMap_;
    RefPtr<PipelineContext> pipelineContext_;
    RefPtr<AceEventHandler> handler_;
    WindowConfig windowConfig_ { .designWidth = JSON_DESIGN_WIDTH, .autoDesignWidth = false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_FRONTEND_H
