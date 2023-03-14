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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_H

#include <memory>

#include "base/memory/ace_type.h"
#include "core/common/ace_page.h"
#include "core/common/frontend.h"
#include "frameworks/bridge/card_frontend/card_frontend_delegate.h"
#include "frameworks/bridge/card_frontend/js_card_parser.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"
#include "frameworks/bridge/common/utils/page_id_pool.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

namespace OHOS::Ace {

class ACE_EXPORT CardFrontend : public Frontend {
    DECLARE_ACE_TYPE(CardFrontend, Frontend);

public:
    CardFrontend() = default;
    ~CardFrontend() override;

    bool Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor) override;

    void Destroy() override;

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override;
    void SetAssetManager(const RefPtr<AssetManager>& assetManager) override;

    void RunPage(int32_t pageId, const std::string& url, const std::string& params) override;
    void ReplacePage(const std::string& url, const std::string& params) override {}
    void PushPage(const std::string& url, const std::string& params) override {}
    void AddPage(const RefPtr<AcePage>& page) override {}
    RefPtr<AcePage> GetPage(int32_t pageId) const override;
    void TriggerGarbageCollection() override {}

    void SendCallbackMessage(const std::string& callbackId, const std::string& data) const override {}
    // platform channel.
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const override {}
    void TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const override {}
    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const override {}

    void LoadPluginJsCode(std::string&& jsCode) const override {}
    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const override {}
    std::string GetFormSrcPath(const std::string& uri, const std::string& suffix) const;

    // application lifecycle.
    void UpdateState(Frontend::State state) override {}

    // page lifecycle.
    bool OnBackPressed() override
    {
        return false;
    }

    void OnSaveAbilityState (std::string& data) override {}

    void OnRestoreAbilityState (const std::string& data) override {}

    void OnShow() override
    {
        foregroundFrontend_ = true;
    }
    void OnHide() override
    {
        foregroundFrontend_ = false;
    }

    void OnActive() override {}
    void OnInactive() override {}
    bool OnStartContinuation() override
    {
        return false;
    }
    void OnCompleteContinuation(int32_t code) override {}
    void OnMemoryLevel(const int32_t level) override {}
    void OnSaveData(std::string& data) override {}
    bool OnRestoreData(const std::string& data) override
    {
        return false;
    }
    void OnRemoteTerminated() override {}
    void OnNewRequest(const std::string& data) override {}
    void OnNewWant(const std::string& data) override {}
    void CallRouterBack() override {}
    void OnSurfaceChanged(int32_t width, int32_t height) override;
    void DumpFrontend() const override {}
    WindowConfig& GetWindowConfig() override;
    void UpdateData(const std::string& dataList);
    void SetColorMode(ColorMode colorMode) override;
    void RebuildAllPages() override;

    RefPtr<AceEventHandler> GetEventHandler() override
    {
        return eventHandler_;
    }

    FrontendType GetType() override
    {
        return type_;
    }

    // judge frontend is foreground frontend.
    bool IsForeground() override
    {
        return foregroundFrontend_;
    }

    RefPtr<AccessibilityManager> GetAccessibilityManager() const override
    {
        if (!delegate_) {
            LOGE("GetAccessibilityManager delegate is null");
            return nullptr;
        }
        return delegate_->GetJsAccessibilityManager();
    }

    void SetDensity(double density)
    {
        density_ = density;
    }

    void ResetPageLoadState()
    {
        pageLoaded_ = false;
    }

    void SetCardHapPath(const std::string& path)
    {
        cardHapPath_ = path;
    }

    std::string GetFormSrc() const
    {
        return formSrc_;
    }

    void SetFormSrc(std::string formSrc)
    {
        formSrc_ = formSrc;
    }

    WindowConfig& GetCardWindowConfig()
    {
        return cardWindowConfig_;
    }

    void SetCardWindowConfig(WindowConfig cardWindowConfig)
    {
        cardWindowConfig_ = cardWindowConfig;
    }

private:
    void UpdatePageData(const std::string& dataList);
    void LoadPage(const std::string& urlPath, const std::string& params);
    void ParsePage(const RefPtr<PipelineContext>& context, const std::string& pageContent, const std::string& params,
        const RefPtr<Framework::JsAcePage>& page);
    void OnPageLoaded(const RefPtr<Framework::JsAcePage>& page);
    void ParseManifest() const;
    void HandleSurfaceChanged(int32_t width, int32_t height);
    void OnMediaFeatureUpdate();

    ColorMode colorMode_ = ColorMode::LIGHT;
    FrontendType type_ = FrontendType::JS_CARD;
    bool foregroundFrontend_ = false;
    bool pageLoaded_ = false;
    double density_ = 1.0;
    std::string cardHapPath_;

    Framework::PipelineContextHolder holder_;
    RefPtr<AssetManager> assetManager_;
    RefPtr<Framework::ManifestParser> manifestParser_;

    mutable std::once_flag onceFlag_;
    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<AceEventHandler> eventHandler_;
    RefPtr<Framework::CardFrontendDelegate> delegate_;
    Framework::PageIdPool pageIdPool_;
    RefPtr<Framework::JsCardParser> parseJsCard_;
    std::string formSrc_;
    WindowConfig cardWindowConfig_;
};

class CardEventHandler : public AceEventHandler {
public:
    explicit CardEventHandler(const RefPtr<Framework::CardFrontendDelegate>& delegate) : delegate_(delegate)
    {
        ACE_DCHECK(delegate_);
    }

    ~CardEventHandler() override = default;

    void HandleAsyncEvent(const EventMarker& eventMarker) override
    {
        LOGI("HandleAsyncEvent cardId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
            eventMarker.GetData().eventId.c_str());
        delegate_->FireCardEvent(eventMarker);
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override
    {
        LOGI("HandleAsyncEvent cardId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
            eventMarker.GetData().eventId.c_str());
        delegate_->FireCardEvent(eventMarker);
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override
    {
        LOGI("HandleAsyncEvent cardId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
            eventMarker.GetData().eventId.c_str());
        delegate_->FireCardEvent(eventMarker, param);
    }

    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override
    {
        LOGW("card event handler does not support this event type!");
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override
    {
        LOGW("card event handler does not support this event type!");
    }

private:
    RefPtr<Framework::CardFrontendDelegate> delegate_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_CARD_FRONTEND_H
