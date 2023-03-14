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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_IMPL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_IMPL_H

#include <future>
#include <mutex>
#include <unordered_map>

#include "base/memory/ace_type.h"
#include "base/thread/cancelable_callback.h"
#include "core/common/frontend.h"
#include "core/common/js_message_dispatcher.h"
#include "core/components/dialog/dialog_component.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {

using LoadJsCallback = std::function<void(const std::string&, const RefPtr<JsAcePage>&, bool isMainPage)>;
using JsMessageDispatcherSetterCallback = std::function<void(const RefPtr<JsMessageDispatcher>&)>;
using EventCallback = std::function<void(const std::string&, const std::string&)>;
using ExternalEventCallback = std::function<void(const std::string&, const uint32_t&)>;
using UpdatePageCallback = std::function<void(const RefPtr<JsAcePage>&)>;
using ResetStagingPageCallback = std::function<void()>;
using MediaQueryCallback = std::function<void(const std::string& callbackId, const std::string& args)>;
using DestroyPageCallback = std::function<void(int32_t pageId)>;
using DestroyApplicationCallback = std::function<void(const std::string& packageName)>;
using UpdateApplicationStateCallback = std::function<void(const std::string& packageName, Frontend::State state)>;
using TimerCallback = std::function<void(const std::string& callbackId, const std::string& delay, bool isInterval)>;
using RequestAnimationCallback = std::function<void(const std::string& callbackId, uint64_t timeStamp)>;
using JsCallback = std::function<void(const std::string& callbackId, const std::string& args)>;
using OnWindowDisplayModeChangedCallBack = std::function<void(bool isShownInMultiWindow, const std::string& data)>;
using OnConfigurationUpdatedCallBack = std::function<void(const std::string& data)>;
using OnNewWantCallBack = std::function<void(const std::string& data)>;
using OnSaveAbilityStateCallBack = std::function<void(std::string& data)>;
using OnRestoreAbilityStateCallBack = std::function<void(const std::string& data)>;
using OnActiveCallBack = std::function<void()>;
using OnInactiveCallBack = std::function<void()>;
using OnMemoryLevelCallBack = std::function<void(const int32_t level)>;
using OnStartContinuationCallBack = std::function<bool(void)>;
using OnCompleteContinuationCallBack = std::function<void(int32_t code)>;
using OnRemoteTerminatedCallBack = std::function<void(void)>;
using OnSaveDataCallBack = std::function<void(std::string& data)>;
using OnRestoreDataCallBack = std::function<bool(const std::string& data)>;
using CallNativeHandlerCallback = std::function<void(const std::string& event, const std::string& params)>;

struct PageInfo {
    int32_t pageId = -1;
    std::string url;
    bool isRestore = false;
    bool isAlertBeforeBackPage = false;
    DialogProperties dialogProperties;
};

class PipelineContextHolder {
public:
    ~PipelineContextHolder()
    {
        if (pipelineContext_) {
            auto taskExecutor = pipelineContext_->GetTaskExecutor();
            // To guarantee the pipelineContext_ destruct in platform thread
            auto context = AceType::RawPtr(pipelineContext_);
            context->IncRefCount();
            pipelineContext_.Reset();
            taskExecutor->PostTask([context] { context->DecRefCount(); }, TaskExecutor::TaskType::PLATFORM);
        }
    }

    void Attach(const RefPtr<PipelineContext>& context)
    {
        if (attached_ || !context) {
            return;
        }

        attached_ = true;
        promise_.set_value(context);
    }

    const RefPtr<PipelineContext>& Get()
    {
        if (!pipelineContext_) {
            pipelineContext_ = future_.get();
            ACE_DCHECK(pipelineContext_);
        }
        return pipelineContext_;
    }

private:
    bool attached_ = false;
    std::promise<RefPtr<PipelineContext>> promise_;
    std::future<RefPtr<PipelineContext>> future_ = promise_.get_future();
    RefPtr<PipelineContext> pipelineContext_;
};

struct FrontendDelegateImplBuilder {
    RefPtr<TaskExecutor> taskExecutor;
    LoadJsCallback loadCallback;
    JsMessageDispatcherSetterCallback transferCallback;
    EventCallback asyncEventCallback;
    EventCallback syncEventCallback;
    ExternalEventCallback externalEventCallback;
    UpdatePageCallback updatePageCallback;
    ResetStagingPageCallback resetStagingPageCallback;
    DestroyPageCallback destroyPageCallback;
    DestroyApplicationCallback destroyApplicationCallback;
    UpdateApplicationStateCallback updateApplicationStateCallback;
    OnStartContinuationCallBack onStartContinuationCallBack;
    OnCompleteContinuationCallBack onCompleteContinuationCallBack;
    OnRemoteTerminatedCallBack onRemoteTerminatedCallBack;
    OnSaveDataCallBack onSaveDataCallBack;
    OnRestoreDataCallBack onRestoreDataCallBack;
    TimerCallback timerCallback;
    MediaQueryCallback mediaQueryCallback;
    RequestAnimationCallback requestAnimationCallback;
    JsCallback jsCallback;
    OnWindowDisplayModeChangedCallBack onWindowDisplayModeChangedCallBack;
    OnNewWantCallBack onNewWantCallBack;
    OnSaveAbilityStateCallBack onSaveAbilityStateCallBack;
    OnRestoreAbilityStateCallBack onRestoreAbilityStateCallBack;
    OnActiveCallBack onActiveCallBack;
    OnInactiveCallBack onInactiveCallBack;
    OnMemoryLevelCallBack onMemoryLevelCallBack;
    CallNativeHandlerCallback callNativeHandler;
    void* ability;
};

class DelegateClient {
public:
    using RouterPushCallback = std::function<void(const std::string& url)>;
    using GetWebPageUrlCallback = std::function<void(std::string& url, int32_t& id)>;
    using IsPagePathInvalidCallback = std::function<void(bool& isPageInvalid)>;
    DelegateClient &operator = (const DelegateClient &) = delete;
    DelegateClient(const DelegateClient &) = delete;
    ~DelegateClient() = default;

    static DelegateClient& GetInstance()
    {
        static DelegateClient instance;
        return instance;
    }

    void RegisterRouterPushCallback(RouterPushCallback &&callback)
    {
        routerPushCallback_ = callback;
    }

    void RouterPush(const std::string& url)
    {
        if (routerPushCallback_) {
            return routerPushCallback_(url);
        }
    }

    void RegisterGetWebPageUrlCallback(GetWebPageUrlCallback &&callback)
    {
        getWebPageUrlCallback_ = callback;
    }

    void GetWebPageUrl(std::string& pageUrl, int32_t& pageId)
    {
        if (getWebPageUrlCallback_) {
            return getWebPageUrlCallback_(pageUrl, pageId);
        }
    }

    void RegisterIsPagePathInvalidCallback(IsPagePathInvalidCallback &&callback)
    {
        isPagePathInvalidCallback_ = callback;
    }

    void GetIsPagePathInvalid(bool& isPageInvalid)
    {
        if (isPagePathInvalidCallback_) {
            return isPagePathInvalidCallback_(isPageInvalid);
        }
    }

private:
    DelegateClient() = default;
    RouterPushCallback routerPushCallback_;
    GetWebPageUrlCallback getWebPageUrlCallback_;
    IsPagePathInvalidCallback isPagePathInvalidCallback_;
};

class FrontendDelegateImpl : public FrontendDelegate {
    DECLARE_ACE_TYPE(FrontendDelegateImpl, FrontendDelegate);

public:
    explicit FrontendDelegateImpl(const FrontendDelegateImplBuilder& builder);
    ~FrontendDelegateImpl() override;

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override;

    // JsFrontend delegate functions.
    void RunPage(const std::string& url, const std::string& params);
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const;
    void TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data);
    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void TransferJsResponseDataPreview(int32_t callbackId, int32_t code, ResponseData responseData) const;
#endif
    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const;
    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const;
    void LoadPluginJsCode(std::string&& jsCode) const;
    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const;
    void OnJsCallback(const std::string& callbackId, const std::string& data);
    bool OnPageBackPress();
    void OnBackGround();
    void OnForground();
    void OnConfigurationUpdated(const std::string& configurationData);
    void OnActive();
    void OnInactive();
    bool OnStartContinuation();
    void OnCompleteContinuation(int32_t code);
    void OnRemoteTerminated();
    void OnSaveData(std::string& data);
    bool OnRestoreData(const std::string& data);
    void OnNewRequest(const std::string& data);
    void OnMemoryLevel(const int32_t level);
    void OnNewWant(const std::string& data);
    void CallPopPage();
    void OnApplicationDestroy(const std::string& packageName);
    void OnApplicationUpdateState(const std::string& packageName, Frontend::State state);
    void SetColorMode(ColorMode colorMode);

    // Accessibility delegate functions.
    RefPtr<Framework::AccessibilityNodeManager> GetJsAccessibilityManager() const
    {
        return jsAccessibilityManager_;
    }
    void FireAccessibilityEvent(const AccessibilityEvent& accessibilityEvent);
    void InitializeAccessibilityCallback();

    void OnMediaQueryUpdate() override;
    void OnSurfaceChanged();
    // JsEventHandler delegate functions.
    void FireAsyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    bool FireSyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    void FireSyncEvent(
        const std::string& eventId, const std::string& param, const std::string& jsonArgs, std::string& result);
    void FireExternalEvent(const std::string& eventId, const std::string& componentId, const uint32_t nodeId);

    // FrontendDelegate overrides.
    void Push(const std::string& uri, const std::string& params) override;
    void Replace(const std::string& uri, const std::string& params) override;
    void Back(const std::string& uri, const std::string& params = "") override;
    void PostponePageTransition() override;
    void LaunchPageTransition() override;
    void Clear() override;
    int32_t GetStackSize() const override;
    void GetState(int32_t& index, std::string& name, std::string& path) override;
    std::string GetParams() override;
    void TriggerPageUpdate(int32_t pageId, bool directExecute = false) override;

    void PostJsTask(std::function<void()>&& task) override;

    const std::string& GetAppID() const override;
    const std::string& GetAppName() const override;
    const std::string& GetVersionName() const override;
    int32_t GetVersionCode() const override;
    WindowConfig& GetWindowConfig();
    int32_t GetMinPlatformVersion() override;
    bool IsUseLiteStyle();
    bool IsWebFeature();

    void ShowToast(const std::string& message, int32_t duration, const std::string& bottom) override;
    void ShowDialog(const std::string& title, const std::string& message, const std::vector<ButtonInfo>& buttons,
        bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
        const std::set<std::string>& callbacks) override;

    void EnableAlertBeforeBackPage(const std::string& message, std::function<void(int32_t)>&& callback) override;

    void DisableAlertBeforeBackPage() override;

    void ShowActionMenu(const std::string& title, const std::vector<ButtonInfo>& button,
        std::function<void(int32_t, int32_t)>&& callback) override;

    Rect GetBoundingRectData(NodeId nodeId) override;

    std::string GetInspector(NodeId nodeId) override;

    void PushJsCallbackToRenderNode(NodeId id, double ratio, std::function<void(bool, double)>&& callback) override;
    void RemoveVisibleChangeNode(NodeId id) override;

    // For async event.
    void SetCallBackResult(const std::string& callBackId, const std::string& result) override;

    void WaitTimer(const std::string& callbackId, const std::string& delay, bool isInterval, bool isFirst) override;
    void ClearTimer(const std::string& callbackId) override;

    void PostSyncTaskToPage(std::function<void()>&& task) override;
    void AddTaskObserver(std::function<void()>&& task) override;
    void RemoveTaskObserver() override;

    bool GetAssetContent(const std::string& url, std::string& content) override;
    bool GetAssetContent(const std::string& url, std::vector<uint8_t>& content) override;
    std::string GetAssetPath(const std::string& url) override;

    // i18n
    void GetI18nData(std::unique_ptr<JsonValue>& json) override;

    void GetResourceConfiguration(std::unique_ptr<JsonValue>& json) override;

    void GetConfigurationCommon(const std::string& filePath, std::unique_ptr<JsonValue>& data) override;

    void ChangeLocale(const std::string& language, const std::string& countryOrRegion) override;

    void RegisterFont(const std::string& familyName, const std::string& familySrc) override;

    void HandleImage(const std::string& src, std::function<void(bool, int32_t, int32_t)>&& callback) override;

    void RequestAnimationFrame(const std::string& callbackId) override;

    void CancelAnimationFrame(const std::string& callbackId) override;

    SingleTaskExecutor GetAnimationJsTask() override;

    SingleTaskExecutor GetUiTask() override;

    void LoadResourceConfiguration(std::map<std::string, std::string>& sortedResourcePath,
        std::unique_ptr<JsonValue>& currentResourceData) override;

    const RefPtr<MediaQueryInfo>& GetMediaQueryInfoInstance() override
    {
        return mediaQueryInfo_;
    }

    const RefPtr<GroupJsBridge>& GetGroupJsBridge() override
    {
        return groupJsBridge_;
    }

    RefPtr<PipelineContext> GetPipelineContext() override;

    void SetGroupJsBridge(const RefPtr<GroupJsBridge>& groupJsBridge)
    {
        groupJsBridge_ = groupJsBridge;
    }

    RefPtr<JsAcePage> GetPage(int32_t pageId) const override;


    WeakPtr<JsAcePage> GetCurrentReadyPage() const
    {
        return currentReadyPage_;
    }

    bool GetPagePathInvalidFlag() const
    {
        return isPagePathInvalid_;
    }

    void RebuildAllPages();

    void CallNativeHandler(const std::string& event, const std::string& params) override;

private:
    int32_t GenerateNextPageId();
    void RecyclePageId(int32_t pageId);

    void LoadPage(int32_t pageId, const std::string& url, bool isMainPage, const std::string& params);
    void OnPageReady(const RefPtr<Framework::JsAcePage>& page, const std::string& url, bool isMainPage);
    void FlushPageCommand(const RefPtr<Framework::JsAcePage>& page, const std::string& url, bool isMainPage);
    void AddPageLocked(const RefPtr<JsAcePage>& page);
    void SetCurrentPage(int32_t pageId);
    void SetCurrentReadyPage(const WeakPtr<JsAcePage>& page)
    {
        currentReadyPage_ = page;
    }

    void OnPushPageSuccess(const RefPtr<JsAcePage>& page, const std::string& url);
    void OnPopToPageSuccess(const std::string& url);
    void PopToPage(const std::string& url);
    int32_t OnPopPageSuccess();
    void PopPage();

    void PopPageTransitionListener(const TransitionEvent& event, int32_t destroyPageId);

    void PopToPageTransitionListener(const TransitionEvent& event, const std::string& url, int32_t pageId);

    int32_t OnClearInvisiblePagesSuccess();
    void ClearInvisiblePages();

    void OnReplacePageSuccess(const RefPtr<JsAcePage>& page, const std::string& url);
    void ReplacePage(const RefPtr<JsAcePage>& page, const std::string& url);
    void LoadReplacePage(int32_t pageId, const std::string& url, const std::string& params);

    uint64_t GetSystemRealTime();

    // Page lifecycle
    void OnPageShow();
    void OnPageHide();
    void OnPageDestroy(int32_t pageId);

    int32_t GetRunningPageId() const;
    std::string GetRunningPageUrl() const;
    int32_t GetPageIdByUrl(const std::string& url);

    void ResetStagingPage();
    void FlushAnimationTasks();
    void ParseManifest();

    void BackImplement(const std::string& uri);

    std::atomic<uint64_t> pageIdPool_ = 0;
    int32_t callbackCnt_ = 0;
    int32_t pageId_ = -1;
    bool isRouteStackFull_ = false;
    bool isStagingPageExist_ = false;
    std::string mainPagePath_;
    std::string backUri_;
    std::vector<PageInfo> pageRouteStack_;
    std::unordered_map<int32_t, RefPtr<JsAcePage>> pageMap_;
    std::unordered_map<int32_t, std::string> pageParamMap_;
    std::unordered_map<int32_t, std::string> jsCallBackResult_;
    WeakPtr<JsAcePage> currentReadyPage_;

    LoadJsCallback loadJs_;
    JsMessageDispatcherSetterCallback dispatcherCallback_;
    EventCallback asyncEvent_;
    EventCallback syncEvent_;
    ExternalEventCallback externalEvent_;
    UpdatePageCallback updatePage_;
    ResetStagingPageCallback resetStagingPage_;
    DestroyPageCallback destroyPage_;
    DestroyApplicationCallback destroyApplication_;
    UpdateApplicationStateCallback updateApplicationState_;
    OnStartContinuationCallBack onStartContinuationCallBack_;
    OnCompleteContinuationCallBack onCompleteContinuationCallBack_;
    OnRemoteTerminatedCallBack onRemoteTerminatedCallBack_;
    OnSaveDataCallBack onSaveDataCallBack_;
    OnRestoreDataCallBack onRestoreDataCallBack_;
    TimerCallback timer_;
    std::unordered_map<std::string, CancelableCallback<void()>> timeoutTaskMap_;
    MediaQueryCallback mediaQueryCallback_;
    RequestAnimationCallback requestAnimationCallback_;
    JsCallback jsCallback_;
    RefPtr<Framework::ManifestParser> manifestParser_;
    RefPtr<Framework::AccessibilityNodeManager> jsAccessibilityManager_;
    RefPtr<MediaQueryInfo> mediaQueryInfo_;
    RefPtr<GroupJsBridge> groupJsBridge_;

    RefPtr<TaskExecutor> taskExecutor_;
    CallNativeHandlerCallback callNativeHandler_;

    PipelineContextHolder pipelineContextHolder_;

    BaseId::IdType pageTransitionListenerId_ = 0L;
    std::queue<std::string> animationFrameTaskIds_;
    std::unordered_map<std::string, CancelableCallback<void()>> animationFrameTaskMap_;

    mutable std::mutex mutex_;
    mutable std::once_flag onceFlag_;

    bool isPagePathInvalid_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_IMPL_H
