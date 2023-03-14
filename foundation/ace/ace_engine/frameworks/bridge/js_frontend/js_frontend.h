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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_FRONTEND_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_FRONTEND_H

#include <string>
#include <unordered_map>

#include "base/memory/ace_type.h"
#include "base/utils/string_utils.h"
#include "core/common/ace_page.h"
#include "core/common/container.h"
#include "core/common/frontend.h"
#include "core/common/js_message_dispatcher.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace {

// JsFrontend is the unique entrance from ACE backend to frontend.
// The relationship between AceActivity, AceContainer and JsFrontend is 1:1:1.
// So JsFrontend would be responsible for below things:
// - Create and initialize QuickJS engine.
// - Load pages of a JS app, and parse the manifest.json before loading main page.
// - Maintain the page stack of JS app by FrontendDelegateImpl.
// - Lifecycle of JS app (also AceActivity).
class JsFrontend : public Frontend {
    DECLARE_ACE_TYPE(JsFrontend, Frontend);

public:
    JsFrontend() = default;
    ~JsFrontend() override;

    bool Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor) override;

    void Destroy() override;

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override;

    void SetAssetManager(const RefPtr<AssetManager>& assetManager) override;

    void RunPage(int32_t pageId, const std::string& url, const std::string& params) override;

    void ReplacePage(const std::string& url, const std::string& params) override;

    void PushPage(const std::string& url, const std::string& params) override;

    // Js frontend manages all pages self.
    void AddPage(const RefPtr<AcePage>& page) override {};

    RefPtr<AcePage> GetPage(int32_t pageId) const override
    {
        return delegate_->GetPage(pageId);
    };

    WeakPtr<AcePage> GetCurrentReadyPage() const
    {
        return delegate_->GetCurrentReadyPage();
    }

    void TriggerGarbageCollection() override;

    void SendCallbackMessage(const std::string& callbackId, const std::string& data) const override;
    // platform channel.
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const override;
    void TransferComponentResponseData(int32_t callbackId, int32_t code,
        std::vector<uint8_t>&& data) const override;
    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void RunNativeEngineLoop() override
    {
        if (jsEngine_) {
            jsEngine_->RunNativeEngineLoop();
        }
    }
    void TransferJsResponseDataPreview(int32_t callbackId, int32_t code, ResponseData responseData) const;
#endif
    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const override;
    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override;
    void LoadPluginJsCode(std::string&& jsCode) const override;
    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const override;

    // application lifecycle.
    void UpdateState(Frontend::State state) override;

    // page lifecycle.
    bool OnBackPressed() override;
    void OnSaveAbilityState(std::string& data) override {}
    void OnRestoreAbilityState(const std::string& data) override {}
    void OnShow() override;
    void OnHide() override;
    void OnConfigurationUpdated(const std::string& data) override;
    void OnActive() override;
    void OnInactive() override;
    bool OnStartContinuation() override;
    void OnCompleteContinuation(int32_t code) override;
    void OnRemoteTerminated() override;
    void OnSaveData(std::string& data) override;
    bool OnRestoreData(const std::string& data) override;

    void OnMemoryLevel(const int32_t level) override {}
    void OnNewRequest(const std::string& data) override;
    void OnNewWant(const std::string& data) override {}
    void CallRouterBack() override;
    void SetColorMode(ColorMode colorMode) override;

    void OnSurfaceChanged(int32_t width, int32_t height) override;

    void DumpFrontend() const override;

    RefPtr<AceEventHandler> GetEventHandler() override
    {
        return handler_;
    };

    FrontendType GetType() override
    {
        return type_;
    }

    // judge frontend is foreground frontend.
    bool IsForeground() override
    {
        return foregroundFrontend_;
    }

    RefPtr<AccessibilityManager> GetAccessibilityManager() const override;
    WindowConfig& GetWindowConfig() override;

    void SetJsEngine(const RefPtr<Framework::JsEngine>& jsEngine)
    {
        jsEngine_ = jsEngine;
    }

    void SetNeedDebugBreakPoint(bool value)
    {
        if (jsEngine_) {
            jsEngine_->SetNeedDebugBreakPoint(value);
        }
    }

    void SetDebugVersion(bool value)
    {
        if (jsEngine_) {
            jsEngine_->SetDebugVersion(value);
        }
    }

    void SetInstanceName(const std::string& name)
    {
        if (jsEngine_) {
            jsEngine_->SetInstanceName(name);
        }
    }

    void RebuildAllPages() override;

    void GetPluginsUsed(std::string& data) override;

private:
    void InitializeFrontendDelegate(const RefPtr<TaskExecutor>& taskExecutor);

    FrontendType type_ = FrontendType::JS;
    RefPtr<Framework::FrontendDelegateImpl> delegate_;
    RefPtr<AceEventHandler> handler_;
    RefPtr<Framework::JsEngine> jsEngine_;
    bool foregroundFrontend_ = false;
};

class JsEventHandler : public AceEventHandler {
public:
    explicit JsEventHandler(const RefPtr<Framework::FrontendDelegateImpl>& delegate) : delegate_(delegate)
    {
        ACE_DCHECK(delegate_);
    }

    ~JsEventHandler() override = default;

    void HandleAsyncEvent(const EventMarker& eventMarker) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, const GestureEvent& info) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, const RotationEvent& info) override;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override;

    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override;

    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override;

    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result) override;

    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override;

    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override;

private:
    RefPtr<Framework::FrontendDelegateImpl> delegate_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_FRONTEND_H
