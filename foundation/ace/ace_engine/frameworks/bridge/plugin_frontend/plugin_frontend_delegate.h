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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_FRONTEND_DELEGATE_DECLARATIVE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_FRONTEND_DELEGATE_DECLARATIVE_H

#include <future>
#include <mutex>
#include <unordered_map>

#include "base/memory/ace_type.h"
#include "base/thread/cancelable_callback.h"
#include "core/common/js_message_dispatcher.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/accessibility/accessibility_node_manager.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
class PluginFrontendDelegate : public FrontendDelegate {
    DECLARE_ACE_TYPE(PluginFrontendDelegate, FrontendDelegate);

public:
    PluginFrontendDelegate(const RefPtr<TaskExecutor>& taskExecutor, const LoadJsCallback& loadCallback,
        const JsMessageDispatcherSetterCallback& transferCallback, const EventCallback& asyncEventCallback,
        const EventCallback& syncEventCallback, const UpdatePageCallback& updatePageCallback,
        const ResetStagingPageCallback& resetLoadingPageCallback, const DestroyPageCallback& destroyPageCallback,
        const DestroyApplicationCallback& destroyApplicationCallback,
        const UpdateApplicationStateCallback& updateApplicationStateCallback, const TimerCallback& timerCallback,
        const MediaQueryCallback& mediaQueryCallback, const RequestAnimationCallback& requestAnimationCallback,
        const JsCallback& jsCallback, const OnWindowDisplayModeChangedCallBack& onWindowDisplayModeChangedCallBack,
        const OnConfigurationUpdatedCallBack& onConfigurationUpdatedCallBack,
        const OnSaveAbilityStateCallBack& onSaveAbilityStateCallBack,
        const OnRestoreAbilityStateCallBack& onRestoreAbilityStateCallBack,
        const OnNewWantCallBack& onNewWantCallBack,
        const OnActiveCallBack& onActiveCallBack, const OnInactiveCallBack& onInactiveCallBack,
        const OnMemoryLevelCallBack& onMemoryLevelCallBack,
        const OnStartContinuationCallBack& onStartContinuationCallBack,
        const OnCompleteContinuationCallBack& onCompleteContinuationCallBack,
        const OnRemoteTerminatedCallBack& onRemoteTerminatedCallBack,
        const OnSaveDataCallBack& onSaveDataCallBack,
        const OnRestoreDataCallBack& onRestoreDataCallBack);
    ~PluginFrontendDelegate() override;

    void AttachPipelineContext(const RefPtr<PipelineContext>& context) override;

    // JSFrontend delegate functions.
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
    void OnJSCallback(const std::string& callbackId, const std::string& data);
    bool OnPageBackPress();
    void OnBackGround();
    void OnForground();
    void OnConfigurationUpdated(const std::string& data);
    void OnSaveAbilityState(std::string& data);
    void OnRestoreAbilityState(const std::string& data);
    void OnNewWant(const std::string& data);
    void OnSuspended();
    bool OnStartContinuation();
    void OnCompleteContinuation(int32_t code);
    void OnMemoryLevel(const int32_t level);
    void OnSaveData(std::string& data);
    void GetPluginsUsed(std::string& data);
    bool OnRestoreData(const std::string& data);
    void OnRemoteTerminated();
    void OnActive();
    void OnInactive();
    void OnNewRequest(const std::string& data);
    void SetColorMode(ColorMode colorMode);
    void CallPopPage();
    void OnApplicationDestroy(const std::string& packageName);
    void UpdateApplicationState(const std::string& packageName, Frontend::State state);
    void OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data);
    void NotifyAppStorage(const WeakPtr<Framework::JsEngine>& jsEngineWeak,
        const std::string& key, const std::string& value);

    // Accessibility delegate functions.
    RefPtr<Framework::AccessibilityNodeManager> GetJSAccessibilityManager() const
    {
        return jsAccessibilityManager_;
    }
    void FireAccessibilityEvent(const AccessibilityEvent& accessibilityEvent);
    void InitializeAccessibilityCallback();

    void OnMediaQueryUpdate() override;
    void OnSurfaceChanged();
    // JSEventHandler delegate functions.
    void FireAsyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    bool FireSyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    void FireSyncEvent(
        const std::string& eventId, const std::string& param, const std::string& jsonArgs, std::string& result);

    // FrontendDelegate overrides.
    void Push(const PageTarget& target, const std::string& params);
    void Replace(const PageTarget& target, const std::string& params);
    void BackWithTarget(const PageTarget& target, const std::string& params);
    void Push(const std::string& uri, const std::string& params) override;
    void Replace(const std::string& uri, const std::string& params) override;
    void Back(const std::string& uri, const std::string& params) override;
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
    WindowConfig& GetWindowConfig()
    {
        return manifestParser_->GetWindowConfig();
    }

    void ShowToast(const std::string& message, int32_t duration, const std::string& bottom) override;
    void ShowDialog(const std::string& title, const std::string& message,
        const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
        const std::set<std::string>& callbacks) override;

    void EnableAlertBeforeBackPage(const std::string& message, std::function<void(int32_t)>&& callback) override {}

    void DisableAlertBeforeBackPage() override {}

    void ShowActionMenu(const std::string& title, const std::vector<ButtonInfo>& button,
        std::function<void(int32_t, int32_t)>&& callback) override {}

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

    int32_t GetMinPlatformVersion() override;

    void LoadResourceConfiguration(std::map<std::string, std::string>& mediaResourceFileMap,
        std::unique_ptr<JsonValue>& currentResourceData) override;

    void ChangeLocale(const std::string& language, const std::string& countryOrRegion) override;

    void RegisterFont(const std::string& familyName, const std::string& familySrc) override;

    void HandleImage(const std::string& src, std::function<void(bool, int32_t, int32_t)>&& callback) override;

    void RequestAnimationFrame(const std::string& callbackId) override;

    void CancelAnimationFrame(const std::string& callbackId) override;

    SingleTaskExecutor GetAnimationJsTask() override;

    SingleTaskExecutor GetUiTask() override;

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

    void RebuildAllPages();

    void UpdatePlugin(const std::string& content);

private:
    int32_t GenerateNextPageId();
    void RecyclePageId(int32_t pageId);

    void LoadPage(int32_t pageId, const PageTarget& target, bool isMainPage, const std::string& params);
    void LoadJS(const RefPtr<Framework::JsAcePage>& page, const std::string& url, bool isMainPage);
    void OnPageReady(
        const RefPtr<Framework::JsAcePage>& page, const std::string& url, bool isMainPage);
    void FlushPageCommand(
        const RefPtr<Framework::JsAcePage>& page, const std::string& url, bool isMainPage);
    void AddPageLocked(const RefPtr<JsAcePage>& page);
    void OnPrePageChange(const RefPtr<JsAcePage>& page);
    void SetCurrentPage(int32_t pageId);

    void OnPushPageSuccess(const RefPtr<JsAcePage>& page, const std::string& url);
    void OnPopToPageSuccess(const std::string& url);
    void PopToPage(const std::string& url);
    int32_t OnPopPageSuccess();
    void PopPage();

    void PopPageTransitionListener(const TransitionEvent& event, int32_t destroyPageId);

    void PopToPageTransitionListener(
        const TransitionEvent& event, const std::string& url, int32_t pageId);

    int32_t OnClearInvisiblePagesSuccess();
    void ClearInvisiblePages();

    void OnReplacePageSuccess(const RefPtr<JsAcePage>& page, const std::string& url);
    void ReplacePage(const RefPtr<JsAcePage>& page, const std::string& url);
    void LoadReplacePage(int32_t pageId, const PageTarget& url, const std::string& params);

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

    std::atomic<uint64_t> pageIdPool_ = 0;
    int32_t callbackCnt_ = 0;
    int32_t pageId_ = -1;
    bool isRouteStackFull_ = false;
    bool isStagingPageExist_ = false;
    std::string mainPagePath_;
    std::vector<PageInfo> pageRouteStack_;
    std::unordered_map<int32_t, RefPtr<JsAcePage>> pageMap_;
    std::unordered_map<int32_t, std::string> pageParamMap_;
    std::unordered_map<int32_t, std::string> jsCallBackResult_;

    LoadJsCallback loadJs_;
    JsMessageDispatcherSetterCallback dispatcherCallback_;
    EventCallback asyncEvent_;
    EventCallback syncEvent_;
    UpdatePageCallback updatePage_;
    ResetStagingPageCallback resetStagingPage_;
    DestroyPageCallback destroyPage_;
    DestroyApplicationCallback destroyApplication_;
    UpdateApplicationStateCallback updateApplicationState_;
    TimerCallback timer_;
    std::unordered_map<std::string, CancelableCallback<void()>> timeoutTaskMap_;
    MediaQueryCallback mediaQueryCallback_;
    RequestAnimationCallback requestAnimationCallback_;
    JsCallback jsCallback_;
    OnWindowDisplayModeChangedCallBack onWindowDisplayModeChanged_;
    OnConfigurationUpdatedCallBack onConfigurationUpdated_;
    OnSaveAbilityStateCallBack onSaveAbilityState_;
    OnRestoreAbilityStateCallBack onRestoreAbilityState_;
    OnNewWantCallBack onNewWant_;
    OnActiveCallBack onActive_;
    OnInactiveCallBack onInactive_;
    OnMemoryLevelCallBack onMemoryLevel_;
    OnStartContinuationCallBack onStartContinuationCallBack_;
    OnCompleteContinuationCallBack onCompleteContinuationCallBack_;
    OnRemoteTerminatedCallBack onRemoteTerminatedCallBack_;
    OnSaveDataCallBack onSaveDataCallBack_;
    OnRestoreDataCallBack onRestoreDataCallBack_;
    RefPtr<Framework::ManifestParser> manifestParser_;
    RefPtr<Framework::AccessibilityNodeManager> jsAccessibilityManager_;
    RefPtr<MediaQueryInfo> mediaQueryInfo_;
    RefPtr<GroupJsBridge> groupJsBridge_;

    RefPtr<TaskExecutor> taskExecutor_;

    PipelineContextHolder pipelineContextHolder_;

    BaseId::IdType pageTransitionListenerId_ = 0L;
    std::queue<std::string> animationFrameTaskIds_;
    std::unordered_map<std::string, CancelableCallback<void()>> animationFrameTaskMap_;

    mutable std::mutex mutex_;
};
} // namespace OHOS::Ace::Framework
#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_FRONTEND_DELEGATE_DECLARATIVE_H
