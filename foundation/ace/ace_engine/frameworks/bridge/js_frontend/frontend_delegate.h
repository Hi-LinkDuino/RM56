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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_H

#include <vector>

#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/media_query/media_query_info.h"
#include "frameworks/bridge/js_frontend/engine/common/group_js_bridge.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {

// A virtual interface which must be implemented as a backing for
// FrontendDelegateImpl instances.
//
// This is the primary interface by which the JsFrontend delegates
// FrontendDelegateImpl behavior out to QjsEngine for js to native calls.
class FrontendDelegate : public AceType {
    DECLARE_ACE_TYPE(FrontendDelegate, AceType);

public:
    FrontendDelegate() = default;
    ~FrontendDelegate() override = default;

    virtual void AttachPipelineContext(const RefPtr<PipelineContext>& context) = 0;
    ACE_EXPORT void SetAssetManager(const RefPtr<AssetManager>& assetManager)
    {
        assetManager_ = assetManager;
    }

    // ----------------
    // system.router
    // ----------------
    // Jump to the specified page.
    virtual void Push(const std::string& uri, const std::string& params) = 0;
    // Jump to the specified page, but current page will be removed from the stack.
    virtual void Replace(const std::string& uri, const std::string& params) = 0;
    // Back to specified page or the previous page if url not set.
    virtual void Back(const std::string& uri, const std::string& params = "") = 0;
    // Postpone page transition after Begin called, usually to wait some async operation
    virtual void PostponePageTransition() = 0;
    // Begin page transition after Postpone called, usually to wait some async operation
    virtual void LaunchPageTransition() = 0;
    // Clear all the pages in stack except the top page, that is current page.
    virtual void Clear() = 0;
    // Gets the number of pages in the page stack.
    virtual int32_t GetStackSize() const = 0;
    // Gets current page's states
    virtual void GetState(int32_t& index, std::string& name, std::string& path) = 0;
    // Gets current page's params
    virtual std::string GetParams()
    {
        return "";
    }

    // distribute
    virtual std::string RestoreRouterStack(const std::string& contentInfo)
    {
        return "";
    }
    virtual std::string GetContentInfo()
    {
        return "";
    }

    virtual void TriggerPageUpdate(int32_t pageId, bool directExecute = false) = 0;

    // posting js task from jsengine
    virtual void PostJsTask(std::function<void()>&& task) = 0;

    virtual void RemoveVisibleChangeNode(NodeId id) = 0;

    // ----------------
    // system.app
    // ----------------
    virtual const std::string& GetAppID() const = 0;
    virtual const std::string& GetAppName() const = 0;
    virtual const std::string& GetVersionName() const = 0;
    virtual int32_t GetVersionCode() const = 0;

    // ----------------
    // system.prompt
    // ----------------
    virtual void ShowToast(const std::string& message, int32_t duration, const std::string& bottom) = 0;
    virtual void ShowDialog(const std::string& title, const std::string& message,
        const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
        const std::set<std::string>& callbacks) = 0;

    virtual void EnableAlertBeforeBackPage(const std::string& message, std::function<void(int32_t)>&& callback) = 0;
    virtual void DisableAlertBeforeBackPage() = 0;

    virtual void ShowActionMenu(const std::string& title, const std::vector<ButtonInfo>& button,
        std::function<void(int32_t, int32_t)>&& callback) = 0;

    virtual Rect GetBoundingRectData(NodeId nodeId) = 0;

    virtual std::string GetInspector(NodeId nodeId) = 0;

    virtual void PushJsCallbackToRenderNode(NodeId id, double ratio, std::function<void(bool, double)>&& callback) = 0;

    // ----------------
    // system.configuration
    // ----------------
    virtual void ChangeLocale(const std::string& language, const std::string& countryOrRegion) = 0;

    // ----------------
    // system.image
    // ----------------
    virtual void HandleImage(const std::string& src, std::function<void(bool, int32_t, int32_t)>&& callback) = 0;
    // ----------------
    // internal.jsResult
    // ----------------
    virtual void SetCallBackResult(const std::string& callBackId, const std::string& result) = 0;

    // ----------------
    // system.animation
    // ----------------
    virtual void RequestAnimationFrame(const std::string& callbackId) = 0;
    virtual void CancelAnimationFrame(const std::string& callbackId) = 0;

    virtual bool GetAssetContent(const std::string& url, std::string& content) = 0;
    virtual bool GetAssetContent(const std::string& url, std::vector<uint8_t>& content) = 0;
    virtual std::string GetAssetPath(const std::string& url) = 0;

    virtual void WaitTimer(const std::string& callbackId, const std::string& delay, bool isInterval, bool isFirst) = 0;
    virtual void ClearTimer(const std::string& callbackId) = 0;

    virtual void PostSyncTaskToPage(std::function<void()>&& task) = 0;

    virtual void AddTaskObserver(std::function<void()>&& task) = 0;

    virtual void RemoveTaskObserver() = 0;

    virtual void GetI18nData(std::unique_ptr<JsonValue>& json) = 0;

    virtual void GetResourceConfiguration(std::unique_ptr<JsonValue>& json) = 0;

    virtual void GetConfigurationCommon(const std::string& filePath, std::unique_ptr<JsonValue>& data) = 0;

    virtual const RefPtr<MediaQueryInfo>& GetMediaQueryInfoInstance() = 0;

    virtual void OnMediaQueryUpdate() = 0;

    virtual void RegisterFont(const std::string& familyName, const std::string& familySrc) = 0;

    virtual SingleTaskExecutor GetAnimationJsTask() = 0;

    virtual SingleTaskExecutor GetUiTask() = 0;

    virtual RefPtr<PipelineContext> GetPipelineContext() = 0;

    virtual const RefPtr<GroupJsBridge>& GetGroupJsBridge() = 0;

    virtual RefPtr<JsAcePage> GetPage(int32_t pageId) const = 0;

    virtual int32_t GetMinPlatformVersion() = 0;

    template<typename T>
    bool ACE_EXPORT GetResourceData(const std::string& fileUri, T& content);

    template<typename T>
    bool ACE_EXPORT GetResourceData(const std::string& fileUri, T& content, std::string& ami);

    template<typename T>
    ACE_EXPORT static bool GetResourceData(const std::string& fileUri, const RefPtr<AssetManager>& assetManager,
        T& content);

    ACE_EXPORT RefPtr<AssetManager> GetAssetManager() const
    {
        return assetManager_;
    }

    virtual void LoadResourceConfiguration(std::map<std::string, std::string>& sortedResourcePath,
        std::unique_ptr<JsonValue>& currentResourceData) = 0;

    void DisallowPopLastPage()
    {
        disallowPopLastPage_ = true;
    }

    virtual void CallNativeHandler(const std::string& event, const std::string& params) {}

protected:
    RefPtr<AssetManager> assetManager_;
    bool disallowPopLastPage_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(FrontendDelegate);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_FRONTEND_DELEGATE_H
