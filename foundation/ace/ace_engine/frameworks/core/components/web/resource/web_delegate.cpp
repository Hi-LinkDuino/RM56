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

#include "core/components/web/resource/web_delegate.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "core/common/container.h"
#include "core/components/web/web_event.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"
#ifdef OHOS_STANDARD_SYSTEM
#include "application_env.h"
#include "nweb_adapter_helper.h"
#include "nweb_hit_testresult.h"
#include "web_javascript_execute_callback.h"
#include "web_javascript_result_callback.h"
#endif

namespace OHOS::Ace {

namespace {

constexpr char WEB_METHOD_ROUTER_BACK[] = "routerBack";
constexpr char WEB_METHOD_UPDATEURL[] = "updateUrl";
constexpr char WEB_METHOD_CHANGE_PAGE_URL[] = "changePageUrl";
constexpr char WEB_METHOD_PAGE_PATH_INVALID[] = "pagePathInvalid";
constexpr char WEB_EVENT_PAGESTART[] = "onPageStarted";
constexpr char WEB_EVENT_PAGEFINISH[] = "onPageFinished";
constexpr char WEB_EVENT_PAGEERROR[] = "onPageError";
constexpr char WEB_EVENT_ONMESSAGE[] = "onMessage";
constexpr char WEB_EVENT_ROUTERPUSH[] = "routerPush";

constexpr char WEB_CREATE[] = "web";
constexpr char NTC_PARAM_WEB[] = "web";
constexpr char NTC_PARAM_WIDTH[] = "width";
constexpr char NTC_PARAM_HEIGHT[] = "height";
constexpr char NTC_PARAM_LEFT[] = "left";
constexpr char NTC_PARAM_TOP[] = "top";
constexpr char NTC_ERROR[] = "create error";
constexpr char NTC_PARAM_SRC[] = "src";
constexpr char NTC_PARAM_ERROR_CODE[] = "errorCode";
constexpr char NTC_PARAM_URL[] = "url";
constexpr char NTC_PARAM_PAGE_URL[] = "pageUrl";
constexpr char NTC_PARAM_PAGE_INVALID[] = "pageInvalid";
constexpr char NTC_PARAM_DESCRIPTION[] = "description";
constexpr char WEB_ERROR_CODE_CREATEFAIL[] = "error-web-delegate-000001";
constexpr char WEB_ERROR_MSG_CREATEFAIL[] = "create web_delegate failed.";

} // namespace

int ConsoleLogOhos::GetLineNumber()
{
    if (message_) {
        return message_->LineNumer();
    }
    return -1;
}

std::string ConsoleLogOhos::GetLog()
{
    if (message_) {
        return message_->Log();
    }
    return "";
}

int ConsoleLogOhos::GetLogLevel()
{
    if (message_) {
        return message_->LogLevel();
    }
    return -1;
}

std::string ConsoleLogOhos::GetSourceId()
{
    if (message_) {
        return message_->SourceId();
    }
    return "";
}

void ResultOhos::Confirm()
{
    if (result_) {
        result_->Confirm();
    }
}

void ResultOhos::Confirm(const std::string &message)
{
    if (result_) {
        result_->Confirm(message);
    }
}

void ResultOhos::Cancel()
{
    if (result_) {
        result_->Cancel();
    }
}

WebDelegate::~WebDelegate()
{
    ReleasePlatformResource();
}

void WebDelegate::ReleasePlatformResource()
{
    auto delegate = WeakClaim(this).Upgrade();
    if (delegate) {
        delegate->Stop();
        delegate->Release();
    }
}

void WebGeolocationOhos::Invoke(const std::string &origin, const bool& allow, const bool& retain)
{
    if (geolocationCallback_) {
        geolocationCallback_->GeolocationCallbackInvoke(origin, allow, retain);
    }
}

void WebDelegate::Stop()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGI("fail to get context");
        return;
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        UnregisterEvent();
    } else {
        platformTaskExecutor.PostTask([weak = WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->UnregisterEvent();
            }
        });
    }
}

void WebDelegate::UnregisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGI("fail to get context");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    resRegister->UnregisterEvent(MakeEventHash(WEB_EVENT_PAGESTART));
    resRegister->UnregisterEvent(MakeEventHash(WEB_EVENT_PAGEFINISH));
    resRegister->UnregisterEvent(MakeEventHash(WEB_EVENT_PAGEERROR));
    resRegister->UnregisterEvent(MakeEventHash(WEB_EVENT_ROUTERPUSH));
    resRegister->UnregisterEvent(MakeEventHash(WEB_EVENT_ONMESSAGE));
}

void WebDelegate::CreatePlatformResource(
    const Size& size, const Offset& position, const WeakPtr<PipelineContext>& context)
{
    ReleasePlatformResource();
    context_ = context;
    CreatePluginResource(size, position, context);

    auto reloadCallback = [weak = WeakClaim(this)]() {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        delegate->Reload();
        return true;
    };
    WebClient::GetInstance().RegisterReloadCallback(reloadCallback);

    auto updateUrlCallback = [weak = WeakClaim(this)](const std::string& url) {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        delegate->UpdateUrl(url);
        return true;
    };
    WebClient::GetInstance().RegisterUpdageUrlCallback(updateUrlCallback);
    InitWebEvent();
}

void WebDelegate::LoadUrl(const std::string& url, const std::map<std::string, std::string>& httpHeaders)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), url, httpHeaders]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->Load(
                    const_cast<std::string&>(url), const_cast<std::map<std::string, std::string>&>(httpHeaders));
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

#ifdef OHOS_STANDARD_SYSTEM
void WebDelegate::Backward()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Get context failed, it is null.");
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                LOGE("Get delegate failed, it is null.");
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->NavigateBack();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::Forward()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Get context failed, it is null.");
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                LOGE("Get delegate failed, it is null.");
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->NavigateForward();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

bool WebDelegate::AccessStep(int32_t step)
{
    auto delegate = WeakClaim(this).Upgrade();
    if (!delegate) {
        LOGE("Get delegate failed, it is null.");
        return false;
    }
    if (delegate->nweb_) {
        return delegate->nweb_->CanNavigateBackOrForward(step);
    }
    return false;
}

bool WebDelegate::AccessBackward()
{
    auto delegate = WeakClaim(this).Upgrade();
    if (!delegate) {
        LOGE("Get delegate failed, it is null.");
        return false;
    }
    if (delegate->nweb_) {
        return delegate->nweb_->IsNavigatebackwardAllowed();
    }
    return false;
}

bool WebDelegate::AccessForward()
{
    auto delegate = WeakClaim(this).Upgrade();
    if (!delegate) {
        LOGE("Get delegate failed, it is null.");
        return false;
    }
    if (delegate->nweb_) {
        return delegate->nweb_->IsNavigateForwardAllowed();
    }
    return false;
}

#endif

void WebDelegate::ExecuteTypeScript(const std::string& jscode, const std::function<void(const std::string)>&& callback)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), jscode, callback]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                auto callbackImpl = std::make_shared<WebJavaScriptExecuteCallBack>(Container::CurrentId());
                if (callbackImpl && callback) {
                    callbackImpl->SetCallBack([weak, func = std::move(callback)](std::string result) {
                        auto delegate = weak.Upgrade();
                        if (!delegate) {
                            return;
                        }
                        auto context = delegate->context_.Upgrade();
                        if (context) {
                            context->GetTaskExecutor()->PostTask(
                                [callback = std::move(func), result]() { callback(result); },
                                TaskExecutor::TaskType::JS);
                        }
                    });
                }
                delegate->nweb_->ExecuteJavaScript(jscode, callbackImpl);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::LoadDataWithBaseUrl(const std::string& baseUrl, const std::string& data, const std::string& mimeType,
    const std::string& encoding, const std::string& historyUrl)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), baseUrl, data, mimeType, encoding, historyUrl]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                if (baseUrl.empty() && historyUrl.empty()) {
                    delegate->nweb_->LoadWithData(data, mimeType, encoding);
                } else {
                    delegate->nweb_->LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
                }
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::LoadDataWithRichText(const std::string& data)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), data]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->LoadWithDataAndBaseUrl("", data, "", "", "");
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::Refresh()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->Reload();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::StopLoading()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->Stop();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::AddJavascriptInterface(const std::string& objectName, const std::vector<std::string>& methodList)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), objectName, methodList]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->RegisterArkJSfunction(objectName, methodList);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}
void WebDelegate::RemoveJavascriptInterface(const std::string& objectName, const std::vector<std::string>& methodList)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), objectName, methodList]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->UnregisterArkJSfunction(objectName, methodList);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::SetWebViewJavaScriptResultCallBack(
    const WebController::JavaScriptCallBackImpl&& javaScriptCallBackImpl)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), javaScriptCallBackImpl]() {
            auto delegate = weak.Upgrade();
            if (delegate == nullptr || delegate->nweb_ == nullptr) {
                return;
            }
            auto webJSResultCallBack =
                std::make_shared<WebJavaScriptResultCallBack>(Container::CurrentId());
            if (webJSResultCallBack) {
                LOGI("WebDelegate SetWebViewJavaScriptResultCallBack");
                webJSResultCallBack->SetJavaScriptCallBack(std::move(javaScriptCallBackImpl));
                delegate->nweb_->SetNWebJavaScriptResultCallBack(webJSResultCallBack);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::RequestFocus()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->webComponent_) {
                delegate->webComponent_->RequestFocus();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

int WebDelegate::GetHitTestResult()
{
    WebHitTestType webHitType = WebHitTestType::UNKNOWN;
    if (nweb_) {
        int hitType = nweb_->GetHitTestResult().GetType();
        switch (hitType) {
            case OHOS::NWeb::HitTestResult::UNKNOWN_TYPE:
                webHitType = WebHitTestType::UNKNOWN;
                break;
            case OHOS::NWeb::HitTestResult::ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP;
                break;
            case OHOS::NWeb::HitTestResult::PHONE_TYPE:
                webHitType = WebHitTestType::PHONE;
                break;
            case OHOS::NWeb::HitTestResult::GEO_TYPE:
                webHitType = WebHitTestType::MAP;
                break;
            case OHOS::NWeb::HitTestResult::EMAIL_TYPE:
                webHitType = WebHitTestType::EMAIL;
                break;
            case OHOS::NWeb::HitTestResult::IMAGE_TYPE:
                webHitType = WebHitTestType::IMG;
                break;
            case OHOS::NWeb::HitTestResult::IMAGE_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP_IMG;
                break;
            case OHOS::NWeb::HitTestResult::SRC_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP;
                break;
            case OHOS::NWeb::HitTestResult::SRC_IMAGE_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP_IMG;
                break;
            case OHOS::NWeb::HitTestResult::EDIT_TEXT_TYPE:
                webHitType = WebHitTestType::EDIT;
                break;
            default:
                LOGW("unknow hit test type:%{public}d", static_cast<int>(hitType));
                break;
        }
    }
    return static_cast<int>(webHitType);
}

void WebDelegate::CreatePluginResource(
    const Size& size, const Offset& position, const WeakPtr<PipelineContext>& context)
{
    state_ = State::CREATING;

    auto webCom = webComponent_;
    if (!webCom) {
        LOGI("webCom is null");
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to webComponent is null");
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        LOGI("pipelineContext is null");
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to context is null");
        return;
    }
    context_ = context;
    auto platformTaskExecutor =
        SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([weakWeb = AceType::WeakClaim(this), weakRes, size, position] {
        auto webDelegate = weakWeb.Upgrade();
        if (webDelegate == nullptr) {
            LOGI("webDelegate is null!");
            return;
        }
        auto webCom = webDelegate->webComponent_;
        if (!webCom) {
            LOGI("webCom is null!");
            webDelegate->OnError(NTC_ERROR, "fail to call WebDelegate::SetSrc PostTask");
            return;
        }
        auto resRegister = weakRes.Upgrade();
        if (!resRegister) {
            if (webDelegate->onError_) {
                webDelegate->onError_(WEB_ERROR_CODE_CREATEFAIL, WEB_ERROR_MSG_CREATEFAIL);
            }
            return;
        }
        auto context = webDelegate->context_.Upgrade();
        if (!context) {
            LOGI("context is null");
            return;
        }

        std::string pageUrl;
        int32_t pageId;
        OHOS::Ace::Framework::DelegateClient::GetInstance().GetWebPageUrl(pageUrl, pageId);

        std::stringstream paramStream;
        paramStream << NTC_PARAM_WEB << WEB_PARAM_EQUALS << webDelegate->id_ << WEB_PARAM_AND << NTC_PARAM_WIDTH
                    << WEB_PARAM_EQUALS << size.Width() * context->GetViewScale() << WEB_PARAM_AND << NTC_PARAM_HEIGHT
                    << WEB_PARAM_EQUALS << size.Height() * context->GetViewScale() << WEB_PARAM_AND << NTC_PARAM_LEFT
                    << WEB_PARAM_EQUALS << position.GetX() * context->GetViewScale() << WEB_PARAM_AND << NTC_PARAM_TOP
                    << WEB_PARAM_EQUALS << position.GetY() * context->GetViewScale() << WEB_PARAM_AND << NTC_PARAM_SRC
                    << WEB_PARAM_EQUALS << webCom->GetSrc() << WEB_PARAM_AND << NTC_PARAM_PAGE_URL << WEB_PARAM_EQUALS
                    << pageUrl;

        std::string param = paramStream.str();
        webDelegate->id_ = resRegister->CreateResource(WEB_CREATE, param);
        if (webDelegate->id_ == INVALID_ID) {
            if (webDelegate->onError_) {
                webDelegate->onError_(WEB_ERROR_CODE_CREATEFAIL, WEB_ERROR_MSG_CREATEFAIL);
            }
            return;
        }
        webDelegate->state_ = State::CREATED;
        webDelegate->hash_ = webDelegate->MakeResourceHash();
        webDelegate->RegisterWebEvent();
        webDelegate->BindRouterBackMethod();
        webDelegate->BindPopPageSuccessMethod();
        webDelegate->BindIsPagePathInvalidMethod();
    });
}

void WebDelegate::InitWebEvent()
{
    auto webCom = webComponent_;
    if (!webCom) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to webComponent is null");
        return;
    }
    if (!webCom->GetPageStartedEventId().IsEmpty()) {
        onPageStarted_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageStartedEventId(), context_);
    }
    if (!webCom->GetPageFinishedEventId().IsEmpty()) {
        onPageFinished_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageFinishedEventId(), context_);
    }
    if (!webCom->GetPageErrorEventId().IsEmpty()) {
        onPageError_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageErrorEventId(), context_);
    }
    if (!webCom->GetMessageEventId().IsEmpty()) {
        onMessage_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetMessageEventId(), context_);
    }
}

#ifdef OHOS_STANDARD_SYSTEM
void WebDelegate::ShowWebView()
{
    if (window_) {
        window_->Show();
    }

    LOGI("OnContinue webview");
    OnActive();
}

void WebDelegate::HideWebView()
{
    if (window_) {
        window_->Hide();
    }

    LOGI("OnPause webview");
    OnInactive();
}

void WebDelegate::InitOHOSWeb(const WeakPtr<PipelineContext>& context, sptr<Surface> surface)
{
    state_ = State::CREATING;
    // obtain hap data path
    auto container = Container::Current();
    if (container == nullptr) {
        LOGE("Fail to get container");
        return;
    }
    const std::string& bundlePath = container->GetBundlePath();
    const std::string& filesDataPath = container->GetFilesDataPath();
    std::string baseDir = "base";
    std::size_t baseIndex = filesDataPath.find(baseDir);
    if (baseIndex == std::string::npos) {
        LOGE("Fail to parse hap data base path");
        return;
    }
    std::string dataPath = filesDataPath.substr(0, baseIndex + baseDir.length());
    bundlePath_ = bundlePath;
    bundleDataPath_ = dataPath;
    // load webview so
    OHOS::NWeb::NWebHelper::Instance().SetBundlePath(bundlePath_);
    if (!OHOS::NWeb::NWebHelper::Instance().Init()) {
        LOGE("Fail to init NWebHelper");
        return;
    }

    auto webCom = webComponent_;
    if (!webCom) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to webComponent is null");
        return;
    }
    context_ = context;
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to context is null");
        return;
    }
    state_ = State::CREATED;

    if (!isCreateWebView_) {
        isCreateWebView_ = true;
        if (surface != nullptr) {
            InitWebViewWithSurface(surface);
        } else {
            InitWebViewWithWindow();
        }
    }

    SetWebCallBack();
    if (!pipelineContext->GetIsDeclarative()) {
        RegisterOHOSWebEventAndMethord();
    } else {
        onPageFinishedV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetPageFinishedEventId(), pipelineContext);
        onPageStartedV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetPageStartedEventId(), pipelineContext);
        onProgressChangeV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetProgressChangeEventId(), pipelineContext);
        onTitleReceiveV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetTitleReceiveEventId(), pipelineContext);
        onGeolocationHideV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetGeolocationHideEventId(), pipelineContext);
        onGeolocationShowV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetGeolocationShowEventId(), pipelineContext);
        onFocusV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetOnFocusEventId(), pipelineContext);
        onRequestFocusV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetRequestFocusEventId(), pipelineContext);
        onErrorReceiveV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetPageErrorEventId(), pipelineContext);
        onHttpErrorReceiveV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetHttpErrorEventId(), pipelineContext);
        onDownloadStartV2_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
            webComponent_->GetDownloadStartEventId(), pipelineContext);
    }
}

void WebDelegate::RegisterOHOSWebEventAndMethord()
{
    auto reloadCallback = [weak = WeakClaim(this)]() {
    auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        delegate->Reload();
        return true;
    };
    WebClient::GetInstance().RegisterReloadCallback(reloadCallback);

    auto webCom = webComponent_;
    if (!webCom->GetPageStartedEventId().IsEmpty()) {
        onPageStarted_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageStartedEventId(), context_);
    }
    if (!webCom->GetPageFinishedEventId().IsEmpty()) {
        onPageFinished_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageFinishedEventId(), context_);
    }
    if (!webCom->GetPageErrorEventId().IsEmpty()) {
        onPageError_ = AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageErrorEventId(), context_);
    }
}

void WebDelegate::SetWebCallBack()
{
    auto webController = webComponent_->GetController();
    if (webController) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        webController->SetLoadUrlImpl([weak = WeakClaim(this), uiTaskExecutor](
            std::string url, const std::map<std::string, std::string>& httpHeaders) {
            uiTaskExecutor.PostTask([weak, url, httpHeaders]() {
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->LoadUrl(url, httpHeaders);
                }
            });
        });
        webController->SetBackwardImpl([weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak]() {
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->Backward();
                }
            });
        });
        webController->SetForwardImpl([weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak]() {
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->Forward();
                }
            });
        });
        webController->SetAccessStepImpl([weak = WeakClaim(this)](int32_t step) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                return delegate->AccessStep(step);
            }
            return false;
        });
        webController->SetAccessBackwardImpl([weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (delegate) {
                return delegate->AccessBackward();
            }
            return false;
        });
        webController->SetAccessForwardImpl([weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (delegate) {
                return delegate->AccessForward();
            }
            return false;
        });
        webController->SetExecuteTypeScriptImpl([weak = WeakClaim(this), uiTaskExecutor](
            std::string jscode, std::function<void(const std::string)>&& callback) {
            uiTaskExecutor.PostTask([weak, jscode, callback]() {
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->ExecuteTypeScript(jscode, std::move(callback));
                }
            });
        });
        webController->SetLoadDataWithBaseUrlImpl(
            [weak = WeakClaim(this), uiTaskExecutor](std::string baseUrl, std::string data, std::string mimeType,
                std::string encoding, std::string historyUrl) {
                uiTaskExecutor.PostTask([weak, baseUrl, data, mimeType, encoding, historyUrl]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->LoadDataWithBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
                    }
                });
            });
        webController->SetRefreshImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->Refresh();
                    }
                });
            });
        webController->SetStopLoadingImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->StopLoading();
                    }
                });
            });
        webController->SetGetHitTestResultImpl(
            [weak = WeakClaim(this)]() {
                auto delegate = weak.Upgrade();
                if (delegate) {
                    return delegate->GetHitTestResult();
                }
                return 0;
            });
        webController->SetWebViewJavaScriptResultCallBackImpl([weak = WeakClaim(this), uiTaskExecutor](
            WebController::JavaScriptCallBackImpl&& javaScriptCallBackImpl) {
                uiTaskExecutor.PostTask([weak, javaScriptCallBackImpl]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->SetWebViewJavaScriptResultCallBack(std::move(javaScriptCallBackImpl));
                    }
                });
            });
        webController->SetAddJavascriptInterfaceImpl([weak = WeakClaim(this), uiTaskExecutor](
            std::string objectName, const std::vector<std::string>& methodList) {
                uiTaskExecutor.PostTask([weak, objectName, methodList]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->AddJavascriptInterface(objectName, methodList);
                    }
                });
            });
        webController->LoadInitJavascriptInterface();
        webController->SetRemoveJavascriptInterfaceImpl([weak = WeakClaim(this), uiTaskExecutor](
            std::string objectName, const std::vector<std::string>& methodList) {
                uiTaskExecutor.PostTask([weak, objectName, methodList]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->RemoveJavascriptInterface(objectName, methodList);
                    }
                });
            });
        webController->SetOnInactiveImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->OnInactive();
                    }
                });
            });
        webController->SetOnActiveImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->OnActive();
                    }
                });
            });
        webController->SetZoomImpl(
            [weak = WeakClaim(this), uiTaskExecutor](float factor) {
                uiTaskExecutor.PostTask([weak, factor]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->Zoom(factor);
                    }
                });
            });
        webController->SetOnFocusImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->OnFocus();
                    }
                });
            });
        webController->SetRequestFocusImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                uiTaskExecutor.PostTask([weak]() {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->RequestFocus();
                    }
                });
            });
    }
}

void WebDelegate::InitWebViewWithWindow()
{
    LOGI("Create webview with window");
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            OHOS::NWeb::NWebInitArgs initArgs;
            std::string app_path = GetDataPath();
            if (!app_path.empty()) {
                initArgs.web_engine_args_to_add.push_back(std::string("--user-data-dir=").append(app_path));
            }

            delegate->window_ = delegate->CreateWindow();
            if (!delegate->window_) {
                return;
            }
            delegate->nweb_ =
                OHOS::NWeb::NWebAdapterHelper::Instance().CreateNWeb(delegate->window_.GetRefPtr(), initArgs);
            if (delegate->nweb_ == nullptr) {
                delegate->window_ = nullptr;
                LOGE("fail to get webview instance");
                return;
            }
            auto component = delegate->webComponent_;
            if (!component) {
                return;
            }
            bool isJsEnabled = component->GetJsEnabled();
            bool isContentAccessEnabled = component->GetContentAccessEnabled();
            bool isFileAccessEnabled = component->GetFileAccessEnabled();

            auto webviewClient = std::make_shared<WebClientImpl>(Container::CurrentId());
            webviewClient->SetWebDelegate(weak);
            delegate->nweb_->SetNWebHandler(webviewClient);

            // Set downloadListenerImpl
            auto downloadListenerImpl = std::make_shared<DownloadListenerImpl>(Container::CurrentId());
            downloadListenerImpl->SetWebDelegate(weak);
            delegate->nweb_->PutDownloadCallback(downloadListenerImpl);

            std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
            setting->PutDomStorageEnabled(true);
            setting->PutIsCreateWindowsByJavaScriptAllowed(true);
            setting->PutJavaScriptEnabled(isJsEnabled);
            setting->PutEnableRawFileAccess(isFileAccessEnabled);
            setting->PutEnableContentAccess(isContentAccessEnabled);

            delegate->nweb_->Load(component->GetSrc());
            delegate->window_->Show();
        },
        TaskExecutor::TaskType::PLATFORM);
}

#if defined(ENABLE_ROSEN_BACKEND)
void WebDelegate::InitWebViewWithSurface(sptr<Surface> surface)
{
    LOGI("Create webview with surface");
    auto context = context_.Upgrade();
    if (!context || !surface) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), surface]() {
            wptr<Surface> surfaceWeak(surface);
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            OHOS::NWeb::NWebInitArgs initArgs;
            initArgs.web_engine_args_to_add.push_back(
                std::string("--user-data-dir=").append(delegate->bundleDataPath_));
            initArgs.web_engine_args_to_add.push_back(
                std::string("--bundle-installation-dir=").append(delegate->bundlePath_));
            sptr<Surface> surface = surfaceWeak.promote();
            if (surface == nullptr) {
                LOGE("surface is nullptr or has expired");
                return;
            }
            delegate->nweb_ =
                OHOS::NWeb::NWebAdapterHelper::Instance().CreateNWeb(surface, initArgs);
            if (delegate->nweb_ == nullptr) {
                LOGE("fail to get webview instance");
                return;
            }
            auto component = delegate->webComponent_;
            if (component == nullptr) {
                return;
            }
            if (!component->GetData().empty()) {
                delegate->LoadDataWithRichText(component->GetData());
            }
            auto nweb_handler = std::make_shared<WebClientImpl>(Container::CurrentId());
            nweb_handler->SetWebDelegate(weak);
            auto downloadListenerImpl = std::make_shared<DownloadListenerImpl>(Container::CurrentId());
            downloadListenerImpl->SetWebDelegate(weak);
            delegate->nweb_->SetNWebHandler(nweb_handler);
            delegate->nweb_->PutDownloadCallback(downloadListenerImpl);

             std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
             setting->PutDomStorageEnabled(component->GetDomStorageAccessEnabled());
             setting->PutIsCreateWindowsByJavaScriptAllowed(true);
             setting->PutJavaScriptEnabled(component->GetJsEnabled());
             setting->PutEnableRawFileAccess(component->GetFileAccessEnabled());
             setting->PutEnableContentAccess(component->GetContentAccessEnabled());
             setting->PutLoadImageFromNetworkDisabled(component->GetOnLineImageAccessEnabled());
             setting->PutImageLoadingAllowed(component->GetImageAccessEnabled());
             setting->PutAccessModeForSecureOriginLoadFromInsecure(
                 static_cast<OHOS::NWeb::NWebPreference::AccessMode>(component->GetMixedMode()));
            setting->PutZoomingFunctionEnabled(component->GetZoomAccessEnabled());
            setting->PutGeolocationAllowed(component->GetGeolocationAccessEnabled());
        },
        TaskExecutor::TaskType::PLATFORM);
}
#endif

void WebDelegate::UpdateUserAgent(const std::string& userAgent)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), userAgent]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutUserAgent(userAgent);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::Resize(const double& width, const double& height)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), width, height] () {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_ && !delegate->window_) {
                delegate->nweb_->Resize(width, height);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateJavaScriptEnabled(const bool& isJsEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isJsEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutJavaScriptEnabled(isJsEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateAllowFileAccess(const bool& isFileAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isFileAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutEnableRawFileAccess(isFileAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateBlockNetworkImage(const bool& onLineImageAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), onLineImageAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutLoadImageFromNetworkDisabled(onLineImageAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateLoadsImagesAutomatically(const bool& isImageAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isImageAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutImageLoadingAllowed(isImageAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateMixedContentMode(const MixedModeContent& mixedMode)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), mixedMode]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutAccessModeForSecureOriginLoadFromInsecure(
                    static_cast<OHOS::NWeb::NWebPreference::AccessMode>(mixedMode));
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::UpdateSupportZoom(const bool& isZoomAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isZoomAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutZoomingFunctionEnabled(isZoomAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}
void WebDelegate::UpdateDomStorageEnabled(const bool& isDomStorageAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isDomStorageAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutDomStorageEnabled(isDomStorageAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}
void WebDelegate::UpdateGeolocationEnabled(const bool& isGeolocationAccessEnabled)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), isGeolocationAccessEnabled]() {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                std::shared_ptr<OHOS::NWeb::NWebPreference> setting = delegate->nweb_->GetPreference();
                setting->PutGeolocationAllowed(isGeolocationAccessEnabled);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::LoadUrl()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)] () {
            auto delegate = weak.Upgrade();
            if (delegate && delegate->nweb_) {
                delegate->nweb_->Load(delegate->webComponent_->GetSrc());
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::OnInactive()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->OnPause();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::OnActive()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->OnContinue();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::Zoom(float factor)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this), factor]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                LOGE("Get delegate failed, it is null.");
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->Zoom(factor);
            }
        },
        TaskExecutor::TaskType::PLATFORM);
}

void WebDelegate::OnFocus()
{
    if (onFocusV2_) {
        onFocusV2_(std::make_shared<LoadWebOnFocusEvent>(""));
    }
}

sptr<OHOS::Rosen::Window> WebDelegate::CreateWindow()
{
    auto context = context_.Upgrade();
    if (!context) {
        return nullptr;
    }
    float scale = context->GetViewScale();

    constexpr int DEFAULT_HEIGHT = 1600;
    int DEFAULT_HEIGHT_WITHOUT_SYSTEM_BAR = (int)(scale * context->GetRootHeight());
    int DEFAULT_STATUS_BAR_HEIGHT = (DEFAULT_HEIGHT - DEFAULT_HEIGHT_WITHOUT_SYSTEM_BAR) / 2;
    constexpr int DEFAULT_LEFT = 0;
    int DEFAULT_TOP = DEFAULT_STATUS_BAR_HEIGHT;
    int DEFAULT_WIDTH = (int)(scale * context->GetRootWidth());
    sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
    option->SetWindowRect({ DEFAULT_LEFT, DEFAULT_TOP, DEFAULT_WIDTH, DEFAULT_HEIGHT_WITHOUT_SYSTEM_BAR });
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = Rosen::Window::Create("ohos_web_window", option);
    return window;
}
#endif

void WebDelegate::RegisterWebEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    resRegister->RegisterEvent(MakeEventHash(WEB_EVENT_PAGESTART), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnPageStarted(param);
        }
    });
    resRegister->RegisterEvent(MakeEventHash(WEB_EVENT_PAGEFINISH), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnPageFinished(param);
        }
    });
    resRegister->RegisterEvent(MakeEventHash(WEB_EVENT_PAGEERROR), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnPageError(param);
        }
    });
    resRegister->RegisterEvent(MakeEventHash(WEB_EVENT_ROUTERPUSH), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnRouterPush(param);
        }
    });
    resRegister->RegisterEvent(MakeEventHash(WEB_EVENT_ONMESSAGE), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnMessage(param);
        }
    });
}

// upper ui component which inherited from WebComponent
// could implement some curtain createdCallback to customized controller interface
// eg: web.loadurl.
void WebDelegate::AddCreatedCallback(const CreatedCallback& createdCallback)
{
    ACE_DCHECK(createdCallback != nullptr);
    ACE_DCHECK(state_ != State::RELEASED);
    createdCallbacks_.emplace_back(createdCallback);
}

void WebDelegate::RemoveCreatedCallback()
{
    ACE_DCHECK(state_ != State::RELEASED);
    createdCallbacks_.pop_back();
}

void WebDelegate::AddReleasedCallback(const ReleasedCallback& releasedCallback)
{
    ACE_DCHECK(releasedCallback != nullptr && state_ != State::RELEASED);
    releasedCallbacks_.emplace_back(releasedCallback);
}

void WebDelegate::RemoveReleasedCallback()
{
    ACE_DCHECK(state_ != State::RELEASED);
    releasedCallbacks_.pop_back();
}

void WebDelegate::Reload()
{
#ifdef OHOS_STANDARD_SYSTEM
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)]() {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            if (delegate->nweb_) {
                delegate->nweb_->Reload();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
#else
    hash_ = MakeResourceHash();
    reloadMethod_ = MakeMethodHash("reload");
    CallResRegisterMethod(reloadMethod_, WEB_PARAM_NONE, nullptr);
#endif
}

void WebDelegate::UpdateUrl(const std::string& url)
{
    hash_ = MakeResourceHash();
    updateUrlMethod_ = MakeMethodHash(WEB_METHOD_UPDATEURL);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_SRC << WEB_PARAM_EQUALS << url;
    std::string param = paramStream.str();
    CallResRegisterMethod(updateUrlMethod_, param, nullptr);
}

void WebDelegate::CallWebRouterBack()
{
    hash_ = MakeResourceHash();
    routerBackMethod_ = MakeMethodHash(WEB_METHOD_ROUTER_BACK);
    CallResRegisterMethod(routerBackMethod_, WEB_PARAM_NONE, nullptr);
}

void WebDelegate::CallPopPageSuccessPageUrl(const std::string& url)
{
    hash_ = MakeResourceHash();
    changePageUrlMethod_ = MakeMethodHash(WEB_METHOD_CHANGE_PAGE_URL);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_PAGE_URL << WEB_PARAM_EQUALS << url;
    std::string param = paramStream.str();
    CallResRegisterMethod(changePageUrlMethod_, param, nullptr);
}

void WebDelegate::CallIsPagePathInvalid(const bool& isPageInvalid)
{
    hash_ = MakeResourceHash();
    isPagePathInvalidMethod_ = MakeMethodHash(WEB_METHOD_PAGE_PATH_INVALID);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_PAGE_INVALID << WEB_PARAM_EQUALS << isPageInvalid;
    std::string param = paramStream.str();
    CallResRegisterMethod(isPagePathInvalidMethod_, param, nullptr);
}

void WebDelegate::OnPageStarted(const std::string& param)
{
    if (onPageStarted_) {
        std::string paramStart = std::string(R"(")").append(param).append(std::string(R"(")"));
        std::string urlParam = std::string(R"("pagestart",{"url":)").append(paramStart.append("},null"));
        onPageStarted_(urlParam);
    }

    // ace 2.0
    if (onPageStartedV2_) {
        onPageStartedV2_(std::make_shared<LoadWebPageStartEvent>(param));
    }
}

void WebDelegate::OnPageFinished(const std::string& param)
{
    if (onPageFinished_) {
        std::string paramFinish = std::string(R"(")").append(param).append(std::string(R"(")"));
        std::string urlParam = std::string(R"("pagefinish",{"url":)").append(paramFinish.append("},null"));
        onPageFinished_(urlParam);
    }
    // ace 2.0
    if (onPageFinishedV2_) {
        onPageFinishedV2_(std::make_shared<LoadWebPageFinishEvent>(param));
    }
}

void WebDelegate::OnProgressChanged(int param)
{
    // ace 2.0
    if (onProgressChangeV2_) {
        onProgressChangeV2_(std::make_shared<LoadWebProgressChangeEvent>(param));
    }
}

void WebDelegate::OnReceivedTitle(const std::string& param)
{
    // ace 2.0
    if (onTitleReceiveV2_) {
        onTitleReceiveV2_(std::make_shared<LoadWebTitleReceiveEvent>(param));
    }
}

void WebDelegate::OnGeolocationPermissionsHidePrompt()
{
    // ace 2.0
    if (onGeolocationHideV2_) {
        onGeolocationHideV2_(std::make_shared<LoadWebGeolocationHideEvent>(""));
    }
}

void WebDelegate::OnGeolocationPermissionsShowPrompt(const std::string& origin,
    OHOS::NWeb::NWebGeolocationCallbackInterface* callback)
{
    // ace 2.0
    if (onGeolocationShowV2_) {
        auto geolocation = AceType::MakeRefPtr<WebGeolocationOhos>(callback);
        onGeolocationShowV2_(std::make_shared<LoadWebGeolocationShowEvent>(origin, geolocation));
    }
}

bool WebDelegate::OnConsoleLog(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> message)
{
    auto param = std::make_shared<LoadWebConsoleLogEvent>(AceType::MakeRefPtr<ConsoleLogOhos>(message));
    return webComponent_->OnConsole(param.get());
}

bool WebDelegate::OnCommonDialog(const BaseEventInfo* info, DialogEventType dialogEventType)
{
    return webComponent_->OnCommonDialog(info, dialogEventType);
}

void WebDelegate::OnDownloadStart(const std::string& url, const std::string& userAgent,
    const std::string& contentDisposition, const std::string& mimetype, long contentLength)
{
if (onDownloadStartV2_) {
        onDownloadStartV2_(std::make_shared<DownloadStartEvent>(url, userAgent, contentDisposition,
            mimetype, contentLength));
    }
}

void WebDelegate::OnErrorReceive(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> error)
{
    if (onPageError_) {
        std::string url = request->Url();
        int errorCode = error->ErrorCode();
        std::string description = error->ErrorInfo();
        std::string paramUrl = std::string(R"(")").append(url).append(std::string(R"(")")).append(",");
        std::string paramErrorCode = std::string(R"(")")
                                         .append(NTC_PARAM_ERROR_CODE)
                                         .append(std::string(R"(")"))
                                         .append(":")
                                         .append(std::to_string(errorCode))
                                         .append(",");

        std::string paramDesc = std::string(R"(")")
                                    .append(NTC_PARAM_DESCRIPTION)
                                    .append(std::string(R"(")"))
                                    .append(":")
                                    .append(std::string(R"(")").append(description).append(std::string(R"(")")));
        std::string errorParam =
            std::string(R"("error",{"url":)").append((paramUrl + paramErrorCode + paramDesc).append("},null"));
        onPageError_(errorParam);
    }

    if (onErrorReceiveV2_) {
        onErrorReceiveV2_(std::make_shared<ReceivedErrorEvent>(
            AceType::MakeRefPtr<WebRequest>(request->RequestHeaders(), request->Method(), request->Url(),
                request->FromGesture(), request->IsAboutMainFrame(), request->IsRequestRedirect()),
            AceType::MakeRefPtr<WebError>(error->ErrorInfo(), error->ErrorCode())));
    }
}

void WebDelegate::OnHttpErrorReceive(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response)
{
    if (onHttpErrorReceiveV2_) {
        onHttpErrorReceiveV2_(std::make_shared<ReceivedHttpErrorEvent>(
            AceType::MakeRefPtr<WebRequest>(request->RequestHeaders(), request->Method(), request->Url(),
                request->FromGesture(), request->IsAboutMainFrame(), request->IsRequestRedirect()),
            AceType::MakeRefPtr<WebResponse>(response->ResponseHeaders(), response->ResponseData(),
                response->ResponseEncoding(), response->ResponseMimeType(), response->ResponseStatus(),
                response->ResponseStatusCode())));
    }
}

void WebDelegate::OnRequestFocus()
{
    if (onRequestFocusV2_) {
        onRequestFocusV2_(std::make_shared<LoadWebRequestFocusEvent>(""));
    }
}

void WebDelegate::OnPageError(const std::string& param)
{
    if (onPageError_) {
        int32_t errorCode = GetIntParam(param, NTC_PARAM_ERROR_CODE);
        std::string url = GetUrlStringParam(param, NTC_PARAM_URL);
        std::string description = GetStringParam(param, NTC_PARAM_DESCRIPTION);

        std::string paramUrl = std::string(R"(")").append(url).append(std::string(R"(")")).append(",");

        std::string paramErrorCode = std::string(R"(")")
                                         .append(NTC_PARAM_ERROR_CODE)
                                         .append(std::string(R"(")"))
                                         .append(":")
                                         .append(std::to_string(errorCode))
                                         .append(",");

        std::string paramDesc = std::string(R"(")")
                                    .append(NTC_PARAM_DESCRIPTION)
                                    .append(std::string(R"(")"))
                                    .append(":")
                                    .append(std::string(R"(")").append(description).append(std::string(R"(")")));
        std::string errorParam =
            std::string(R"("error",{"url":)").append((paramUrl + paramErrorCode + paramDesc).append("},null"));
        onPageError_(errorParam);
    }
}

void WebDelegate::OnMessage(const std::string& param)
{
    std::string removeQuotes;
    removeQuotes = param;
    removeQuotes.erase(std::remove(removeQuotes.begin(), removeQuotes.end(), '\"'), removeQuotes.end());
    if (onMessage_) {
        std::string paramMessage = std::string(R"(")").append(removeQuotes).append(std::string(R"(")"));
        std::string messageParam = std::string(R"("message",{"message":)").append(paramMessage.append("},null"));
        onMessage_(messageParam);
    }
}

void WebDelegate::OnRouterPush(const std::string& param)
{
    OHOS::Ace::Framework::DelegateClient::GetInstance().RouterPush(param);
}

#ifdef OHOS_STANDARD_SYSTEM
void WebDelegate::HandleTouchDown(const int32_t& id, const double& x, const double& y)
{
    ACE_DCHECK(nweb_ != nullptr);
    if (nweb_) {
        nweb_->OnTouchPress(id, x, y);
    }
}

void WebDelegate::HandleTouchUp(const int32_t& id, const double& x, const double& y)
{
    ACE_DCHECK(nweb_ != nullptr);
    if (nweb_) {
        nweb_->OnTouchRelease(id, x, y);
    }
}

void WebDelegate::HandleTouchMove(const int32_t& id, const double& x, const double& y)
{
    ACE_DCHECK(nweb_ != nullptr);
    if (nweb_) {
        nweb_->OnTouchMove(id, x, y);
    }
}

void WebDelegate::HandleTouchCancel()
{
    ACE_DCHECK(nweb_ != nullptr);
    if (nweb_) {
        nweb_->OnTouchCancel();
    }
}
#endif

std::string WebDelegate::GetUrlStringParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t posErrorCode = param.find(NTC_PARAM_ERROR_CODE);
    size_t pos = param.find(name);
    std::string result;

    if (pos != std::string::npos && posErrorCode != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len, posErrorCode - 5);
        ss >> result;
    }
    return result;
}

void WebDelegate::BindRouterBackMethod()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetRouterBackEventHandler([weak = WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->CallWebRouterBack();
            }
        });
    }
}

void WebDelegate::BindPopPageSuccessMethod()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetPopPageSuccessEventHandler(
            [weak = WeakClaim(this)](const std::string& pageUrl, const int32_t pageId) {
                std::string url = pageUrl.substr(0, pageUrl.length() - 3);
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->CallPopPageSuccessPageUrl(url);
                }
            });
    }
}

void WebDelegate::BindIsPagePathInvalidMethod()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetIsPagePathInvalidEventHandler([weak = WeakClaim(this)](bool& isPageInvalid) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->CallIsPagePathInvalid(isPageInvalid);
            }
        });
    }
}

void WebDelegate::SetComponent(const RefPtr<WebComponent>& component)
{
    webComponent_ = component;
}

} // namespace OHOS::Ace
