/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components/web/resource/web_client_impl.h"

#include "core/common/container.h"
#include "core/components/web/resource/web_delegate.h"

namespace OHOS::Ace {

bool OnJsCommonDialog(
    const std::string &url,
    const std::string &message,
    std::shared_ptr<NWeb::NWebJSDialogResult> result,
    DialogEventType dialogEventType,
    const WebClientImpl* webClientImpl)
{
    bool jsResult = false;
    auto param = std::make_shared<WebDialogEvent>(url, message, AceType::MakeRefPtr<ResultOhos>(result));
    auto task = Container::CurrentTaskExecutor();
    if (task == nullptr) {
        LOGW("can't get task executor");
        return false;
    }
    task->PostSyncTask([&webClientImpl, dialogEventType, &param, &jsResult] {
        if (webClientImpl == nullptr) {
            return;
        }
        auto delegate = webClientImpl->GetWebDelegate();
        if (delegate) {
            jsResult = delegate->OnCommonDialog(param.get(), dialogEventType);
        }
        },
        OHOS::Ace::TaskExecutor::TaskType::JS);
    LOGI("OnJsCommonDialog result:%{public}d", jsResult);
    return jsResult;
}

void DownloadListenerImpl::OnDownloadStart(const std::string& url, const std::string& userAgent,
    const std::string& contentDisposition, const std::string& mimetype, long contentLength)
{
    ContainerScope scope(instanceId_);
    LOGI("OnDownloadStart.");
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnDownloadStart(url, userAgent, contentDisposition, mimetype, contentLength);
}

void WebClientImpl::OnPageLoadEnd(int httpStatusCode, const std::string& url)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnPageFinished(url);
}

void WebClientImpl::OnFocus()
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnRequestFocus();
}

bool WebClientImpl::OnConsoleLog(const OHOS::NWeb::NWebConsoleLog& message)
{
    ContainerScope scope(instanceId_);
    bool jsMessage = false;
    auto task = Container::CurrentTaskExecutor();
    if (!task) {
        return false;
    }
    task->PostSyncTask([webClient = this, &message, &jsMessage] {
        if (!webClient) {
            return;
        }
        auto delegate = webClient->webDelegate_.Upgrade();
        if (delegate) {
            jsMessage = delegate->OnConsoleLog(std::make_shared<OHOS::NWeb::NWebConsoleLog>(message));
        }
        },
        OHOS::Ace::TaskExecutor::TaskType::JS);

    return jsMessage;
}

void WebClientImpl::OnPageLoadBegin(const std::string& url)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnPageStarted(url);
}

void WebClientImpl::OnLoadingProgress(int newProgress)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnProgressChanged(newProgress);
}

void WebClientImpl::OnPageTitle(const std::string &title)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnReceivedTitle(title);
}

void WebClientImpl::OnGeolocationHide()
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnGeolocationPermissionsHidePrompt();
}

void WebClientImpl::OnGeolocationShow(const std::string& origin,
    OHOS::NWeb::NWebGeolocationCallbackInterface* callback)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnGeolocationPermissionsShowPrompt(origin, callback);
}

void WebClientImpl::SetNWeb(std::shared_ptr<OHOS::NWeb::NWeb> nweb)
{
    webviewWeak_ = nweb;
}

void WebClientImpl::OnProxyDied()
{
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
}

void WebClientImpl::OnResourceLoadError(
    std::shared_ptr<NWeb::NWebUrlResourceRequest> request, std::shared_ptr<NWeb::NWebUrlResourceError> error)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnErrorReceive(request, error);
}

void WebClientImpl::OnHttpError(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnHttpErrorReceive(request, response);
}

void WebClientImpl::OnMessage(const std::string& param)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnMessage(param);
}

void WebClientImpl::OnRouterPush(const std::string& param)
{
    ContainerScope scope(instanceId_);
    auto delegate = webDelegate_.Upgrade();
    if (!delegate) {
        return;
    }
    delegate->OnRouterPush(param);
}

bool WebClientImpl::OnAlertDialogByJS(
    const std::string &url, const std::string &message, std::shared_ptr<NWeb::NWebJSDialogResult> result)
{
    LOGI("OnAlertDialogByJS");
    ContainerScope scope(instanceId_);
    return OnJsCommonDialog(url, message, result, DialogEventType::DIALOG_EVENT_ALERT, this);
}

bool WebClientImpl::OnBeforeUnloadByJS(
    const std::string &url, const std::string &message, std::shared_ptr<NWeb::NWebJSDialogResult> result)
{
    LOGI("OnBeforeUnloadByJS");
    ContainerScope scope(instanceId_);
    return OnJsCommonDialog(url, message, result, DialogEventType::DIALOG_EVENT_BEFORE_UNLOAD, this);
}

bool WebClientImpl::OnConfirmDialogByJS(
    const std::string &url, const std::string &message, std::shared_ptr<NWeb::NWebJSDialogResult> result)
{
    LOGI("OnConfirmDialogByJS");
    ContainerScope scope(instanceId_);
    return OnJsCommonDialog(url, message, result, DialogEventType::DIALOG_EVENT_CONFIRM, this);
}

} // namespace OHOS::Ace
