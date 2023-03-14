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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FRONTEND_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FRONTEND_H

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/accessibility/accessibility_manager.h"
#include "core/common/ace_page.h"
#include "core/common/js_message_dispatcher.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/base/utils/resource_configuration.h"

using FrontendDialogCallback = std::function<void(const std::string& event, const std::string& param)>;

namespace OHOS::Ace {

#ifndef WEARABLE_PRODUCT
constexpr int32_t DEFAULT_DESIGN_WIDTH = 720;
#else
constexpr int32_t DEFAULT_DESIGN_WIDTH = 454;
#endif

// Window config of frontend.
struct WindowConfig {
    // NOT runtime real design width, this is config value set by user.
    // Runtime design width should be considered together with autoDesignWidth.
    int32_t designWidth = DEFAULT_DESIGN_WIDTH;
    bool autoDesignWidth = false;
    bool boxWrap = false;
    double designWidthScale = 0.0;
};

enum class FrontendType { JSON, JS, JS_CARD, DECLARATIVE_JS, JS_PLUGIN };

class ACE_EXPORT Frontend : public AceType {
    DECLARE_ACE_TYPE(Frontend, AceType);

public:
    Frontend() = default;
    ~Frontend() override = default;

    enum class State { ON_CREATE, ON_DESTROY, ON_SHOW, ON_HIDE };

    static RefPtr<Frontend> Create();
    static RefPtr<Frontend> CreateDefault();

    virtual bool Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor) = 0;

    virtual void Destroy() = 0;

    virtual void AttachPipelineContext(const RefPtr<PipelineContext>& context) = 0;

    virtual void SetAssetManager(const RefPtr<AssetManager>& assetManager) = 0;

    virtual void AddPage(const RefPtr<AcePage>& page) = 0;

    virtual RefPtr<AcePage> GetPage(int32_t pageId) const = 0;

    virtual void RunPage(int32_t pageId, const std::string& content, const std::string& params) = 0;

    virtual void ReplacePage(const std::string& url, const std::string& params) = 0;

    virtual void PushPage(const std::string& url, const std::string& params) = 0;

    // Gets front-end event handler to handle ace event.
    virtual RefPtr<AceEventHandler> GetEventHandler() = 0;

    // Get window config of front end, which is used to calculate the pixel ratio of the real device.
    virtual WindowConfig& GetWindowConfig() = 0;

    virtual FrontendType GetType() = 0;

    // inform the frontend that onCreate or onDestroy
    virtual void UpdateState(State) = 0;

    // dump frontend info
    virtual void DumpFrontend() const = 0;

    // send the message by js callback
    virtual void SendCallbackMessage(const std::string& callbackId, const std::string& data) const = 0;

    // set the message transfer to js instance
    virtual void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& transfer) const = 0;

    // transfer data back from platform side to component side
    virtual void TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const = 0;

    // transfer data back from platform side to js side
    virtual void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const = 0;

    // transfer error message get in plugin from platform side to js side
    virtual void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const = 0;

    // transfer event data from platform side to js side
    virtual void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const = 0;

    // get system plugin used in application
    virtual void GetPluginsUsed(std::string& data) {}

    // get js code from plugin and load in js engine
    virtual void LoadPluginJsCode(std::string&& jsCode) const = 0;

    virtual void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const = 0;

    // when this is foreground frontend
    virtual bool IsForeground() = 0;

    // get accessibility manager handler.
    virtual RefPtr<AccessibilityManager> GetAccessibilityManager() const = 0;

    // when back pressed
    virtual bool OnBackPressed() = 0;

    // when interface show up
    virtual void OnShow() = 0;

    // when interface hide
    virtual void OnHide() = 0;

    // when configuration update
    virtual void OnConfigurationUpdated(const std::string& data) {}

    virtual void OnSaveAbilityState (std::string& data) = 0;

    virtual void OnMemoryLevel(const int32_t level) = 0;

    virtual void OnRestoreAbilityState (const std::string& data) = 0;

    // when front on active
    virtual void OnActive() = 0;

    // when front on inactive
    virtual void OnInactive() = 0;

    // when front on asks a user whether to start the migration
    virtual bool OnStartContinuation() = 0;

    // when front on a local ability migration is complete
    virtual void OnCompleteContinuation(int32_t code) = 0;

    // interface to save the user data
    virtual void OnSaveData(std::string& data) = 0;

    // interface to restores the user data on the remote device
    virtual bool OnRestoreData(const std::string& data) = 0;

    virtual void OnRemoteTerminated() = 0;

    // start the ability when it's running
    virtual void OnNewRequest(const std::string& data) = 0;

    virtual void OnNewWant(const std::string& data) = 0;

    // call router back
    virtual void CallRouterBack() = 0;

    virtual void OnSurfaceChanged(int32_t width, int32_t height) = 0;

    virtual void TriggerGarbageCollection() {}

    virtual void RebuildAllPages() {}

    virtual void SetColorMode(ColorMode colorMode) {}

    // navigator component call router
    virtual void NavigatePage(uint8_t type, const PageTarget& target, const std::string& params) {}

    // distribute
    virtual std::string RestoreRouterStack(const std::string& contentInfo)
    {
        return "";
    }
    virtual std::string GetContentInfo() const
    {
        return "";
    }

    virtual void NotifyAppStorage(const std::string& key, const std::string& value) {}
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    virtual void RunNativeEngineLoop() {}
#endif

    // Disallow pop last page
    void DisallowPopLastPage()
    {
        disallowPopLastPage_ = true;
    }

    virtual void SetDialogCallback(FrontendDialogCallback callback)
    {
        dialogCallback_ = callback;
    }

protected:
    bool disallowPopLastPage_ = false;
    FrontendDialogCallback dialogCallback_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FRONTEND_H
