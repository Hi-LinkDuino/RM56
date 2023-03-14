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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_DELEGATE_H

#include <list>
#include <map>

#ifdef OHOS_STANDARD_SYSTEM
#include <ui/rs_surface_node.h>
#endif

#include "core/components/common/layout/constants.h"
#include "core/components/web/resource/web_client_impl.h"
#include "core/components/web/resource/web_resource.h"
#include "core/components/web/web_component.h"
#include "core/components/web/web_event.h"
#ifdef OHOS_STANDARD_SYSTEM
#include "nweb_helper.h"
#include "window.h"
#endif

namespace OHOS::Ace {

class ConsoleLogOhos : public WebConsoleLog {
    DECLARE_ACE_TYPE(ConsoleLogOhos, WebConsoleLog)

public:
    ConsoleLogOhos(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> message) : message_(message) {}

    int GetLineNumber() override;

    std::string GetLog() override;

    int GetLogLevel() override;

    std::string GetSourceId() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebConsoleLog> message_;
};

class ResultOhos : public Result {
    DECLARE_ACE_TYPE(ResultOhos, Result)

public:
    ResultOhos(std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) : result_(result) {}

    void Confirm() override;
    void Confirm(const std::string &message) override;
    void Cancel() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result_;
};

class WebGeolocationOhos : public WebGeolocation {
    DECLARE_ACE_TYPE(WebGeolocationOhos, WebGeolocation)

public:
    WebGeolocationOhos(OHOS::NWeb::NWebGeolocationCallbackInterface* callback) : geolocationCallback_(callback) {}
    
    void Invoke(const std::string& origin, const bool& allow, const bool& retain) override;
private:
    OHOS::NWeb::NWebGeolocationCallbackInterface* geolocationCallback_;
};

class WebDelegate : public WebResource {
    DECLARE_ACE_TYPE(WebDelegate, WebResource);

public:
    using CreatedCallback = std::function<void()>;
    using ReleasedCallback = std::function<void(bool)>;
    using EventCallback = std::function<void(const std::string&)>;
    using EventCallbackV2 = std::function<void(const std::shared_ptr<BaseEventInfo>&)>;
    enum class State: char {
        WAITINGFORSIZE,
        CREATING,
        CREATED,
        CREATEFAILED,
        RELEASED,
    };

    WebDelegate() = delete;
    ~WebDelegate() override;
    WebDelegate(const WeakPtr<PipelineContext>& context, ErrorCallback&& onError, const std::string& type)
        : WebResource(type, context, std::move(onError)), state_(State::WAITINGFORSIZE)
    {
        ACE_DCHECK(!type.empty());
    }

    void CreatePlatformResource(const Size& size, const Offset& position,
        const WeakPtr<PipelineContext>& context);
    void CreatePluginResource(const Size& size, const Offset& position,
        const WeakPtr<PipelineContext>& context);
    void AddCreatedCallback(const CreatedCallback& createdCallback);
    void RemoveCreatedCallback();
    void AddReleasedCallback(const ReleasedCallback& releasedCallback);
    void SetComponent(const RefPtr<WebComponent>& component);
    void RemoveReleasedCallback();
    void Reload();
    void UpdateUrl(const std::string& url);
#ifdef OHOS_STANDARD_SYSTEM
    void InitOHOSWeb(const WeakPtr<PipelineContext>& context, sptr<Surface> surface = nullptr);
    void InitWebViewWithWindow();
    void ShowWebView();
    void HideWebView();
    void Resize(const double& width, const double& height);
    void UpdateUserAgent(const std::string& userAgent);
    void UpdateJavaScriptEnabled(const bool& isJsEnabled);
    void UpdateAllowFileAccess(const bool& isFileAccessEnabled);
    void UpdateBlockNetworkImage(const bool& onLineImageAccessEnabled);
    void UpdateLoadsImagesAutomatically(const bool& isImageAccessEnabled);
    void UpdateMixedContentMode(const MixedModeContent& mixedMode);
    void UpdateSupportZoom(const bool& isZoomAccessEnabled);
    void UpdateDomStorageEnabled(const bool& isDomStorageAccessEnabled);
    void UpdateGeolocationEnabled(const bool& isGeolocationAccessEnabled);
    void LoadUrl();
    void HandleTouchDown(const int32_t& id, const double& x, const double& y);
    void HandleTouchUp(const int32_t& id, const double& x, const double& y);
    void HandleTouchMove(const int32_t& id, const double& x, const double& y);
    void HandleTouchCancel();
#endif
    void OnErrorReceive(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> error);
    void OnHttpErrorReceive(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response);
    void OnPageStarted(const std::string& param);
    void OnPageFinished(const std::string& param);
    void OnProgressChanged(int param);
    void OnReceivedTitle(const std::string& param);
    void OnGeolocationPermissionsHidePrompt();
    void OnGeolocationPermissionsShowPrompt(const std::string& origin,
        OHOS::NWeb::NWebGeolocationCallbackInterface* callback);
    void OnRequestFocus();
    bool OnCommonDialog(const BaseEventInfo* info, DialogEventType dialogEventType);
    void OnDownloadStart(const std::string& url, const std::string& userAgent, const std::string& contentDisposition,
        const std::string& mimetype, long contentLength);
    void OnPageError(const std::string& param);
    void OnMessage(const std::string& param);
    bool OnConsoleLog(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> message);
    void OnRouterPush(const std::string& param);
private:
    void InitWebEvent();
    void RegisterWebEvent();
    void ReleasePlatformResource();
    void Stop();
    void UnregisterEvent();
    std::string GetUrlStringParam(const std::string& param, const std::string& name) const;
    void CallWebRouterBack();
    void CallPopPageSuccessPageUrl(const std::string& url);
    void CallIsPagePathInvalid(const bool& isPageInvalid);

    void BindRouterBackMethod();
    void BindPopPageSuccessMethod();
    void BindIsPagePathInvalidMethod();

#ifdef OHOS_STANDARD_SYSTEM
    sptr<OHOS::Rosen::Window> CreateWindow();
    void LoadUrl(const std::string& url, const std::map<std::string, std::string>& httpHeaders);
    void ExecuteTypeScript(const std::string& jscode, const std::function<void(std::string)>&& callback);
    void LoadDataWithBaseUrl(const std::string& baseUrl, const std::string& data, const std::string& mimeType,
        const std::string& encoding, const std::string& historyUrl);
    void LoadDataWithRichText(const std::string& data);
    void Refresh();
    void StopLoading();
    void AddJavascriptInterface(const std::string& objectName, const std::vector<std::string>& methodList);
    void RemoveJavascriptInterface(const std::string& objectName, const std::vector<std::string>& methodList);
    void SetWebViewJavaScriptResultCallBack(const WebController::JavaScriptCallBackImpl&& javaScriptCallBackImpl);
    void RequestFocus();
    void OnFocus();
    void OnInactive();
    void OnActive();
    void Zoom(float factor);
    int GetHitTestResult();
    void RegisterOHOSWebEventAndMethord();
    void SetWebCallBack();

    // Backward and forward
    void Backward();
    void Forward();
    bool AccessStep(int32_t step);
    bool AccessBackward();
    bool AccessForward();
#if defined(ENABLE_ROSEN_BACKEND)
    void InitWebViewWithSurface(sptr<Surface> surface);
#endif
#endif

    RefPtr<WebComponent> webComponent_;
    std::list<CreatedCallback> createdCallbacks_;
    std::list<ReleasedCallback> releasedCallbacks_;
    EventCallback onPageStarted_;
    EventCallback onPageFinished_;
    EventCallback onPageError_;
    EventCallback onMessage_;
    Method reloadMethod_;
    Method updateUrlMethod_;
    Method routerBackMethod_;
    Method changePageUrlMethod_;
    Method isPagePathInvalidMethod_;
    State state_ {State::WAITINGFORSIZE};
#ifdef OHOS_STANDARD_SYSTEM
    std::shared_ptr<OHOS::NWeb::NWeb> nweb_;
    sptr<Rosen::Window> window_;
    bool isCreateWebView_ = false;

    EventCallbackV2 onPageFinishedV2_;
    EventCallbackV2 onPageStartedV2_;
    EventCallbackV2 onProgressChangeV2_;
    EventCallbackV2 onTitleReceiveV2_;
    EventCallbackV2 onGeolocationHideV2_;
    EventCallbackV2 onGeolocationShowV2_;
    EventCallbackV2 onRequestFocusV2_;
    EventCallbackV2 onErrorReceiveV2_;
    EventCallbackV2 onHttpErrorReceiveV2_;
    EventCallbackV2 onDownloadStartV2_;
    EventCallbackV2 onFocusV2_;

    std::string bundlePath_;
    std::string bundleDataPath_;

#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_DELEGATE_H
