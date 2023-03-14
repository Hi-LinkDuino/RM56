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

#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

#include <atomic>
#include <regex>
#include <string>

#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/resource/ace_res_config.h"
#include "base/thread/background_task_executor.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/common/platform_bridge.h"
#include "core/common/thread_checker.h"
#include "core/components/toast/toast_component.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t INVALID_PAGE_ID = -1;
constexpr int32_t MAX_ROUTER_STACK = 32;
constexpr int32_t TOAST_TIME_MAX = 10000;    // ms
constexpr int32_t TOAST_TIME_DEFAULT = 1500; // ms
constexpr int32_t MAX_PAGE_ID_SIZE = sizeof(uint64_t) * 8;
constexpr int32_t NANO_TO_MILLI = 1000000; // nanosecond to millisecond
constexpr int32_t TO_MILLI = 1000;         // second to millisecond
constexpr int32_t COMPATIBLE_VERSION = 7;
constexpr int32_t WEB_FEATURE_VERSION = 6;

const char MANIFEST_JSON[] = "manifest.json";
const char FILE_TYPE_JSON[] = ".json";
const char I18N_FOLDER[] = "i18n/";
const char RESOURCES_FOLDER[] = "resources/";
const char STYLES_FOLDER[] = "styles/";
const char I18N_FILE_SUFFIX[] = "/properties/i18n.json";

} // namespace

int32_t FrontendDelegateImpl::GenerateNextPageId()
{
    for (int32_t idx = 0; idx < MAX_PAGE_ID_SIZE; ++idx) {
        uint64_t bitMask = (1ULL << idx);
        if ((bitMask & pageIdPool_.fetch_or(bitMask, std::memory_order_relaxed)) == 0) {
            return idx;
        }
    }
    return INVALID_PAGE_ID;
}

void FrontendDelegateImpl::RecyclePageId(int32_t pageId)
{
    if (pageId < 0 && pageId >= MAX_PAGE_ID_SIZE) {
        return;
    }
    uint64_t bitMask = (1ULL << pageId);
    pageIdPool_.fetch_and(~bitMask, std::memory_order_relaxed);
}

FrontendDelegateImpl::FrontendDelegateImpl(const FrontendDelegateImplBuilder& builder)
    : loadJs_(builder.loadCallback), dispatcherCallback_(builder.transferCallback),
      asyncEvent_(builder.asyncEventCallback), syncEvent_(builder.syncEventCallback),
      externalEvent_(builder.externalEventCallback),
      updatePage_(builder.updatePageCallback), resetStagingPage_(builder.resetStagingPageCallback),
      destroyPage_(builder.destroyPageCallback), destroyApplication_(builder.destroyApplicationCallback),
      updateApplicationState_(builder.updateApplicationStateCallback),
      onStartContinuationCallBack_(builder.onStartContinuationCallBack),
      onCompleteContinuationCallBack_(builder.onCompleteContinuationCallBack),
      onRemoteTerminatedCallBack_(builder.onRemoteTerminatedCallBack),
      onSaveDataCallBack_(builder.onSaveDataCallBack),
      onRestoreDataCallBack_(builder.onRestoreDataCallBack),
      timer_(builder.timerCallback), mediaQueryCallback_(builder.mediaQueryCallback),
      requestAnimationCallback_(builder.requestAnimationCallback), jsCallback_(builder.jsCallback),
      manifestParser_(AceType::MakeRefPtr<ManifestParser>()),
      jsAccessibilityManager_(AccessibilityNodeManager::Create()),
      mediaQueryInfo_(AceType::MakeRefPtr<MediaQueryInfo>()), taskExecutor_(builder.taskExecutor),
      callNativeHandler_(builder.callNativeHandler)
{}

FrontendDelegateImpl::~FrontendDelegateImpl()
{
    CHECK_RUN_ON(JS);
    LOG_DESTROY();
}

void FrontendDelegateImpl::ParseManifest()
{
    std::call_once(onceFlag_, [weak = AceType::WeakClaim(this)]() {
        std::string jsonContent;
        auto delegate = weak.Upgrade();
        if (delegate) {
            if (!delegate->GetAssetContent(MANIFEST_JSON, jsonContent)) {
                LOGE("RunPage parse manifest.json failed");
                EventReport::SendFormException(FormExcepType::RUN_PAGE_ERR);
                return;
            }
            delegate->manifestParser_->Parse(jsonContent);
            auto task = [delegate]() {
                delegate->pipelineContextHolder_.Get(); // Wait until Pipeline Context is attached.
                delegate->manifestParser_->GetAppInfo()->ParseI18nJsonInfo();
            };
            delegate->taskExecutor_->PostTask(task, TaskExecutor::TaskType::JS);
        }
    });
}

void FrontendDelegateImpl::RunPage(const std::string& url, const std::string& params)
{
    ACE_SCOPED_TRACE("FrontendDelegateImpl::RunPage");

    auto routerBackCallback = [weak = WeakClaim(this)](const std::string& urlPath) {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        delegate->Push(urlPath, "");
        return true;
    };
    DelegateClient::GetInstance().RegisterRouterPushCallback(routerBackCallback);

    auto getWebPageUrlCallback = [weak = WeakClaim(this)](std::string& pageUrl, int32_t& pageId) {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        pageUrl = delegate->GetRunningPageUrl();
        pageId = delegate->GetRunningPageId();
        return true;
    };
    DelegateClient::GetInstance().RegisterGetWebPageUrlCallback(getWebPageUrlCallback);

    auto isPagePathInvalidCallback = [weak = WeakClaim(this)](bool& isPageEmpty) {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return false;
        }
        isPageEmpty = delegate->GetPagePathInvalidFlag();
        return true;
    };
    DelegateClient::GetInstance().RegisterIsPagePathInvalidCallback(isPagePathInvalidCallback);

    LOGD("FrontendDelegateImpl RunPage url=%{private}s", url.c_str());
    ParseManifest();
    if (!url.empty()) {
        mainPagePath_ = manifestParser_->GetRouter()->GetPagePath(url);
        if (mainPagePath_.empty()) {
            mainPagePath_ = manifestParser_->GetRouter()->GetEntry();
        }
    } else {
        mainPagePath_ = manifestParser_->GetRouter()->GetEntry();
    }
    LoadPage(GenerateNextPageId(), mainPagePath_, true, params);
}

void FrontendDelegateImpl::ChangeLocale(const std::string& language, const std::string& countryOrRegion)
{
    LOGD("JsFrontend ChangeLocale");
    taskExecutor_->PostTask(
        [language, countryOrRegion]() { AceApplicationInfo::GetInstance().ChangeLocale(language, countryOrRegion); },
        TaskExecutor::TaskType::PLATFORM);
}

void FrontendDelegateImpl::GetI18nData(std::unique_ptr<JsonValue>& json)
{
    auto data = JsonUtil::CreateArray(true);
    GetConfigurationCommon(I18N_FOLDER, data);
    auto i18nData = JsonUtil::Create(true);
    i18nData->Put("resources", data);
    json->Put("i18n", i18nData);
}

void FrontendDelegateImpl::GetResourceConfiguration(std::unique_ptr<JsonValue>& json)
{
    auto data = JsonUtil::CreateArray(true);
    GetConfigurationCommon(RESOURCES_FOLDER, data);
    json->Put("resourcesConfiguration", data);
}

void FrontendDelegateImpl::GetConfigurationCommon(const std::string& filePath, std::unique_ptr<JsonValue>& data)
{
    std::vector<std::string> files;
    if (assetManager_) {
        assetManager_->GetAssetList(filePath, files);
    }

    std::vector<std::string> fileNameList;
    for (const auto& file : files) {
        if (EndWith(file, FILE_TYPE_JSON) && !StartWith(file, STYLES_FOLDER)) {
            fileNameList.emplace_back(file.substr(0, file.size() - (sizeof(FILE_TYPE_JSON) - 1)));
        }
    }

    std::vector<std::string> priorityFileName;
    if (filePath.compare(I18N_FOLDER) == 0) {
        auto localeTag = AceApplicationInfo::GetInstance().GetLocaleTag();
        priorityFileName = AceResConfig::GetLocaleFallback(localeTag, fileNameList);
    } else {
        priorityFileName = AceResConfig::GetResourceFallback(fileNameList);
    }

    for (const auto& fileName : priorityFileName) {
        auto fileFullPath = filePath + fileName + std::string(FILE_TYPE_JSON);
        std::string content;
        if (GetAssetContent(fileFullPath, content)) {
            auto fileData = ParseFileData(content);
            if (fileData == nullptr) {
                LOGW("parse %{private}s.json i18n content failed", filePath.c_str());
            } else {
                data->Put(fileData);
            }
        }
    }
}

void FrontendDelegateImpl::OnJsCallback(const std::string& callbackId, const std::string& data)
{
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), callbackId, args = std::move(data)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->jsCallback_(callbackId, args);
            }
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const
{
    LOGD("JsFrontend SetJsMessageDispatcher");
    taskExecutor_->PostTask([dispatcherCallback = dispatcherCallback_, dispatcher] { dispatcherCallback(dispatcher); },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::TransferComponentResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data)
{
    LOGD("JsFrontend TransferComponentResponseData");
    auto pipelineContext = pipelineContextHolder_.Get();
    WeakPtr<PipelineContext> contextWeak(pipelineContext);
    taskExecutor_->PostTask(
        [callbackId, data = std::move(data), contextWeak]() mutable {
            auto context = contextWeak.Upgrade();
            if (!context) {
                LOGE("context is null");
            } else if (!context->GetMessageBridge()) {
                LOGE("messageBridge is null");
            } else {
                context->GetMessageBridge()->HandleCallback(callbackId, std::move(data));
            }
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    LOGD("JsFrontend TransferJsResponseData");
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([callbackId, code, data = std::move(data), weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->TriggerModuleJsCallback(callbackId, code, std::move(data));
        }
    }, TaskExecutor::TaskType::JS);
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void FrontendDelegateImpl::TransferJsResponseDataPreview(
    int32_t callbackId, int32_t code, ResponseData responseData) const
{
    LOGI("JsFrontend TransferJsResponseDataPreview");
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([callbackId, code, responseData, weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->TriggerModuleJsCallbackPreview(callbackId, code, responseData);
        }
    }, TaskExecutor::TaskType::JS);
}
#endif

void FrontendDelegateImpl::TransferJsPluginGetError(
    int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    LOGD("JsFrontend TransferJsPluginGetError");
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([callbackId, errorCode, errorMessage = std::move(errorMessage), weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->TriggerModulePluginGetErrorCallback(callbackId, errorCode, std::move(errorMessage));
        }
    }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([callbackId, code, data = std::move(data), weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->TriggerEventJsCallback(callbackId, code, std::move(data));
        }
    }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::LoadPluginJsCode(std::string&& jsCode) const
{
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([jsCode = std::move(jsCode), weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->LoadPluginJsCode(std::move(jsCode));
        }
    }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const
{
    LOGD("JsFrontend LoadPluginJsByteCode");
    auto weak = AceType::WeakClaim(AceType::RawPtr(groupJsBridge_));
    taskExecutor_->PostTask([jsCode = std::move(jsCode), jsCodeLen = std::move(jsCodeLen), weak]() mutable {
        auto groupJsBridge = weak.Upgrade();
        if (groupJsBridge) {
            groupJsBridge->LoadPluginJsByteCode(std::move(jsCode), std::move(jsCodeLen));
        }
    }, TaskExecutor::TaskType::JS);
}

bool FrontendDelegateImpl::OnPageBackPress()
{
    bool result = FireSyncEvent("_root", std::string("\"clickbackitem\","), std::string(""));
    LOGD("OnPageBackPress: jsframework callback result: %{public}d", result);
    return result;
}

void FrontendDelegateImpl::OnActive()
{
    LOGD("JsFrontend onActive");
    FireAsyncEvent("_root", std::string("\"viewactive\",null,null"), std::string(""));
}

void FrontendDelegateImpl::OnInactive()
{
    LOGD("JsFrontend OnInactive");
    FireAsyncEvent("_root", std::string("\"viewinactive\",null,null"), std::string(""));
    FireAsyncEvent("_root", std::string("\"viewsuspended\",null,null"), std::string(""));
}

void FrontendDelegateImpl::OnBackGround()
{
    OnPageHide();
}

void FrontendDelegateImpl::OnForground()
{
    OnPageShow();
}

bool FrontendDelegateImpl::OnStartContinuation()
{
    bool ret = false;
    taskExecutor_->PostSyncTask([weak = AceType::WeakClaim(this), &ret] {
        auto delegate = weak.Upgrade();
        if (delegate && delegate->onStartContinuationCallBack_) {
            ret = delegate->onStartContinuationCallBack_();
        }
    }, TaskExecutor::TaskType::JS);
    return ret;
}

void FrontendDelegateImpl::OnCompleteContinuation(int32_t code)
{
    taskExecutor_->PostSyncTask([weak = AceType::WeakClaim(this), code] {
        auto delegate = weak.Upgrade();
        if (delegate && delegate->onCompleteContinuationCallBack_) {
            delegate->onCompleteContinuationCallBack_(code);
        }
    }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnRemoteTerminated()
{
    taskExecutor_->PostSyncTask([weak = AceType::WeakClaim(this)] {
        auto delegate = weak.Upgrade();
        if (delegate && delegate->onRemoteTerminatedCallBack_) {
            delegate->onRemoteTerminatedCallBack_();
        }
    }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnSaveData(std::string& data)
{
    std::string savedData;
    taskExecutor_->PostSyncTask([weak = AceType::WeakClaim(this), &savedData] {
        auto delegate = weak.Upgrade();
        if (delegate && delegate->onSaveDataCallBack_) {
            delegate->onSaveDataCallBack_(savedData);
        }
    }, TaskExecutor::TaskType::JS);
    std::string pageUri = GetRunningPageUrl();
    data = std::string("{\"url\":\"").append(pageUri).append("\",\"__remoteData\":").append(savedData).append("}");
}

bool FrontendDelegateImpl::OnRestoreData(const std::string& data)
{
    bool ret = false;
    taskExecutor_->PostSyncTask([weak = AceType::WeakClaim(this), &data, &ret] {
        auto delegate = weak.Upgrade();
        if (delegate && delegate->onRestoreDataCallBack_) {
            ret = delegate->onRestoreDataCallBack_(data);
        }
    }, TaskExecutor::TaskType::JS);
    return ret;
}

void FrontendDelegateImpl::OnNewRequest(const std::string& data)
{
    FireSyncEvent("_root", std::string("\"onNewRequest\","), data);
}

void FrontendDelegateImpl::CallPopPage()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& currentPage = pageRouteStack_.back();
    if (!pageRouteStack_.empty() && currentPage.isAlertBeforeBackPage) {
        backUri_ = "";
        taskExecutor_->PostTask(
            [context = pipelineContextHolder_.Get(), dialogProperties = pageRouteStack_.back().dialogProperties,
                isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft()]() {
              if (context) {
                  context->ShowDialog(dialogProperties, isRightToLeft);
              }
            },
            TaskExecutor::TaskType::UI);
    } else {
        PopPage();
    }
}

void FrontendDelegateImpl::ResetStagingPage()
{
    taskExecutor_->PostTask([resetStagingPage = resetStagingPage_] { resetStagingPage(); }, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnApplicationDestroy(const std::string& packageName)
{
    taskExecutor_->PostSyncTask(
        [destroyApplication = destroyApplication_, packageName] { destroyApplication(packageName); },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnApplicationUpdateState(const std::string& packageName, Frontend::State state)
{
    taskExecutor_->PostSyncTask(
        [updateApplication = updateApplicationState_, packageName, state] { updateApplication(packageName, state); },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::FireAsyncEvent(
    const std::string& eventId, const std::string& param, const std::string& jsonArgs)
{
    LOGD("FireAsyncEvent eventId: %{public}s", eventId.c_str());
    std::string args = param;
    args.append(",null").append(",null"); // callback and dom changes
    if (!jsonArgs.empty()) {
        args.append(",").append(jsonArgs); // method args
    }
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), eventId, args = std::move(args)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->asyncEvent_(eventId, args);
            }
        },
        TaskExecutor::TaskType::JS);
}

bool FrontendDelegateImpl::FireSyncEvent(
    const std::string& eventId, const std::string& param, const std::string& jsonArgs)
{
    std::string resultStr;
    FireSyncEvent(eventId, param, jsonArgs, resultStr);
    return (resultStr == "true");
}

void FrontendDelegateImpl::FireSyncEvent(
    const std::string& eventId, const std::string& param, const std::string& jsonArgs, std::string& result)
{
    int32_t callbackId = callbackCnt_++;
    std::string args = param;
    args.append("{\"_callbackId\":\"").append(std::to_string(callbackId)).append("\"}").append(",null");
    if (!jsonArgs.empty()) {
        args.append(",").append(jsonArgs); // method args
    }
    taskExecutor_->PostSyncTask(
        [weak = AceType::WeakClaim(this), eventId, args = std::move(args)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->syncEvent_(eventId, args);
            }
        },
        TaskExecutor::TaskType::JS);

    result = jsCallBackResult_[callbackId];
    LOGD("FireSyncEvent eventId: %{public}s, callbackId: %{public}d", eventId.c_str(), callbackId);
    jsCallBackResult_.erase(callbackId);
}

void FrontendDelegateImpl::FireExternalEvent(
    const std::string& eventId, const std::string& componentId, const uint32_t nodeId)
{
    taskExecutor_->PostSyncTask(
        [weak = AceType::WeakClaim(this), componentId, nodeId] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->externalEvent_(componentId, nodeId);
            }
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::FireAccessibilityEvent(const AccessibilityEvent& accessibilityEvent)
{
    jsAccessibilityManager_->SendAccessibilityAsyncEvent(accessibilityEvent);
}

void FrontendDelegateImpl::InitializeAccessibilityCallback()
{
    jsAccessibilityManager_->InitializeCallback();
}

// Start FrontendDelegate overrides.
void FrontendDelegateImpl::Push(const std::string& uri, const std::string& params)
{
    if (uri.empty()) {
        LOGE("router.Push uri is empty");
        return;
    }
    if (isRouteStackFull_) {
        LOGE("the router stack has reached its max size, you can't push any more pages.");
        EventReport::SendPageRouterException(PageRouterExcepType::PAGE_STACK_OVERFLOW_ERR, uri);
        return;
    }
    std::string pagePath = manifestParser_->GetRouter()->GetPagePath(uri);
    LOGD("router.Push pagePath = %{private}s", pagePath.c_str());
    if (!pagePath.empty()) {
        isPagePathInvalid_ = true;
        LoadPage(GenerateNextPageId(), pagePath, false, params);
    } else {
        isPagePathInvalid_ = false;
        LOGW("[Engine Log] this uri not support in route push.");
    }

    if (taskExecutor_) {
        taskExecutor_->PostTask(
            [context = pipelineContextHolder_.Get(), isPagePathInvalid = isPagePathInvalid_]() {
                if (context) {
                    context->NotifyIsPagePathInvalidDismiss(isPagePathInvalid);
                }
            },
            TaskExecutor::TaskType::UI);
    }
}

void FrontendDelegateImpl::Replace(const std::string& uri, const std::string& params)
{
    if (uri.empty()) {
        LOGE("router.Replace uri is empty");
        return;
    }

    auto pipelineContext = pipelineContextHolder_.Get();
    if (pipelineContext) {
        pipelineContext->NotifyDestroyEventDismiss();
    }

    std::string pagePath = manifestParser_->GetRouter()->GetPagePath(uri);
    LOGD("router.Replace pagePath = %{private}s", pagePath.c_str());
    if (!pagePath.empty()) {
        LoadReplacePage(GenerateNextPageId(), pagePath, params);
    } else {
        LOGW("[Engine Log] this uri not support in route replace.");
    }
}

void FrontendDelegateImpl::Back(const std::string& uri, const std::string& params)
{
    auto pipelineContext = pipelineContextHolder_.Get();
    if (pipelineContext) {
        pipelineContext->NotifyDestroyEventDismiss();
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto& currentPage = pageRouteStack_.back();
    if (!pageRouteStack_.empty() && currentPage.isAlertBeforeBackPage) {
        backUri_ = uri;
        taskExecutor_->PostTask(
            [context = pipelineContextHolder_.Get(), dialogProperties = pageRouteStack_.back().dialogProperties,
                isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft()]() {
                if (context) {
                    context->ShowDialog(dialogProperties, isRightToLeft);
                }
            },
            TaskExecutor::TaskType::UI);
    } else {
        BackImplement(uri);
    }
}

void FrontendDelegateImpl::BackImplement(const std::string& uri)
{
    LOGD("router.Back path = %{private}s", uri.c_str());
    if (uri.empty()) {
        PopPage();
    } else {
        std::string pagePath = manifestParser_->GetRouter()->GetPagePath(uri);
        LOGD("router.Back pagePath = %{private}s", pagePath.c_str());
        if (!pagePath.empty()) {
            PopToPage(pagePath);
        } else {
            LOGW("[Engine Log] this uri not support in route Back.");
        }
    }

    auto context = pipelineContextHolder_.Get();
    if (context) {
        context->NotifyRouterBackDismiss();
    }
}

void FrontendDelegateImpl::PostponePageTransition()
{
    std::lock_guard<std::mutex> lock(mutex_);
    taskExecutor_->PostTask(
        [context = pipelineContextHolder_.Get()]() {
            if (context) {
                context->PostponePageTransition();
            }
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::LaunchPageTransition()
{
    std::lock_guard<std::mutex> lock(mutex_);
    taskExecutor_->PostTask(
        [context = pipelineContextHolder_.Get()]() {
          if (context) {
              context->LaunchPageTransition();
          }
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::Clear()
{
    ClearInvisiblePages();
}

int32_t FrontendDelegateImpl::GetStackSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int32_t>(pageRouteStack_.size());
}

void FrontendDelegateImpl::GetState(int32_t& index, std::string& name, std::string& path)
{
    std::string url;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pageRouteStack_.empty()) {
            return;
        }
        index = static_cast<int32_t>(pageRouteStack_.size());
        url = pageRouteStack_.back().url;
    }
    auto pos = url.rfind(".js");
    if (pos == url.length() - 3) {
        url = url.substr(0, pos);
    }
    pos = url.rfind("/");
    if (pos != std::string::npos) {
        name = url.substr(pos + 1);
        path = url.substr(0, pos + 1);
    }
}

std::string FrontendDelegateImpl::GetParams()
{
    if (pageParamMap_.find(pageId_) != pageParamMap_.end()) {
        return pageParamMap_.find(pageId_)->second;
    } else {
        return "";
    }
}

void FrontendDelegateImpl::TriggerPageUpdate(int32_t pageId, bool directExecute)
{
    auto page = GetPage(pageId);
    if (!page) {
        return;
    }

    auto jsPage = AceType::DynamicCast<Framework::JsAcePage>(page);
    ACE_DCHECK(jsPage);

    // Pop all JS command and execute them in UI thread.
    auto jsCommands = std::make_shared<std::vector<RefPtr<JsCommand>>>();
    jsPage->PopAllCommands(*jsCommands);

    auto pipelineContext = pipelineContextHolder_.Get();
    WeakPtr<Framework::JsAcePage> jsPageWeak(jsPage);
    WeakPtr<PipelineContext> contextWeak(pipelineContext);
    auto updateTask = [weak = AceType::WeakClaim(this), jsPageWeak, contextWeak, jsCommands] {
        ACE_SCOPED_TRACE("FlushUpdateCommands");
        auto delegate = weak.Upgrade();
        auto jsPage = jsPageWeak.Upgrade();
        auto context = contextWeak.Upgrade();
        if (!delegate || !jsPage || !context) {
            LOGE("Page update failed. page or context is null.");
            EventReport::SendPageRouterException(PageRouterExcepType::UPDATE_PAGE_ERR);
            return;
        }
        bool useLiteStyle = delegate->GetMinPlatformVersion() < COMPATIBLE_VERSION && delegate->IsUseLiteStyle();
        context->SetUseLiteStyle(useLiteStyle);
        jsPage->SetUseLiteStyle(useLiteStyle);
        jsPage->SetUseBoxWrap(delegate->GetMinPlatformVersion() >= WEB_FEATURE_VERSION);
        // Flush all JS commands.
        for (const auto& command : *jsCommands) {
            command->Execute(jsPage);
        }
        if (jsPage->GetDomDocument()) {
            jsPage->GetDomDocument()->HandleComponentPostBinding();
        }
        auto accessibilityManager = context->GetAccessibilityManager();
        if (accessibilityManager) {
            accessibilityManager->HandleComponentPostBinding();
        }

        jsPage->ClearShowCommand();
        std::vector<NodeId> dirtyNodes;
        jsPage->PopAllDirtyNodes(dirtyNodes);
        for (auto nodeId : dirtyNodes) {
            auto patchComponent = jsPage->BuildPagePatch(nodeId);
            if (patchComponent) {
                context->ScheduleUpdate(patchComponent);
            }
        }
    };
    auto weakContext = AceType::WeakClaim(AceType::RawPtr(pipelineContext));
    taskExecutor_->PostTask([updateTask, weakContext, directExecute]() {
        auto pipelineContext = weakContext.Upgrade();
        if (pipelineContext) {
            pipelineContext->AddPageUpdateTask(std::move(updateTask), directExecute);
        }
    }, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::PostJsTask(std::function<void()>&& task)
{
    taskExecutor_->PostTask(task, TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::RemoveVisibleChangeNode(NodeId id)
{
    auto task = [nodeId = id, pipeline = pipelineContextHolder_.Get()]() { pipeline->RemoveVisibleChangeNode(nodeId); };
    taskExecutor_->PostTask(task, TaskExecutor::TaskType::UI);
}

const std::string& FrontendDelegateImpl::GetAppID() const
{
    return manifestParser_->GetAppInfo()->GetAppID();
}

const std::string& FrontendDelegateImpl::GetAppName() const
{
    return manifestParser_->GetAppInfo()->GetAppName();
}

const std::string& FrontendDelegateImpl::GetVersionName() const
{
    return manifestParser_->GetAppInfo()->GetVersionName();
}

int32_t FrontendDelegateImpl::GetVersionCode() const
{
    return manifestParser_->GetAppInfo()->GetVersionCode();
}

WindowConfig& FrontendDelegateImpl::GetWindowConfig()
{
    ParseManifest();
    return manifestParser_->GetWindowConfig();
}

int32_t FrontendDelegateImpl::GetMinPlatformVersion()
{
    ParseManifest();
    return manifestParser_->GetMinPlatformVersion();
}

bool FrontendDelegateImpl::IsUseLiteStyle()
{
    ParseManifest();
    return manifestParser_->IsUseLiteStyle();
}

bool FrontendDelegateImpl::IsWebFeature()
{
    ParseManifest();
    return manifestParser_->IsWebFeature();
}

void FrontendDelegateImpl::ShowToast(const std::string& message, int32_t duration, const std::string& bottom)
{
    LOGD("FrontendDelegateImpl ShowToast.");
    int32_t durationTime = std::clamp(duration, TOAST_TIME_DEFAULT, TOAST_TIME_MAX);
    auto pipelineContext = pipelineContextHolder_.Get();
    bool isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
    auto weak = AceType::WeakClaim(AceType::RawPtr(pipelineContext));
    taskExecutor_->PostTask([durationTime, message, bottom, isRightToLeft, weak] {
        ToastComponent::GetInstance().Show(weak.Upgrade(), message, durationTime, bottom, isRightToLeft);
    }, TaskExecutor::TaskType::UI);
}

Rect FrontendDelegateImpl::GetBoundingRectData(NodeId nodeId)
{
    Rect rect;
    auto task = [context = pipelineContextHolder_.Get(), nodeId, &rect]() {
        context->GetBoundingRectData(nodeId, rect);
    };
    PostSyncTaskToPage(task);
    return rect;
}

std::string FrontendDelegateImpl::GetInspector(NodeId nodeId)
{
    std::string attrs;
    auto task = [weak = WeakClaim(AceType::RawPtr(jsAccessibilityManager_)), nodeId, &attrs]() {
        auto accessibilityNodeManager = weak.Upgrade();
        if (accessibilityNodeManager) {
            attrs = accessibilityNodeManager->GetInspectorNodeById(nodeId);
        }
    };
    PostSyncTaskToPage(task);
    return attrs;
}

void FrontendDelegateImpl::ShowDialog(const std::string& title, const std::string& message,
    const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
    const std::set<std::string>& callbacks)
{
    if (!taskExecutor_) {
        LOGE("task executor is null.");
        return;
    }

    std::unordered_map<std::string, EventMarker> callbackMarkers;
    if (callbacks.find(COMMON_SUCCESS) != callbacks.end()) {
        auto successEventMarker = BackEndEventManager<void(int32_t)>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void(int32_t)>::GetInstance().BindBackendEvent(
            successEventMarker, [callback, taskExecutor = taskExecutor_](int32_t successType) {
                taskExecutor->PostTask(
                    [callback, successType]() { callback(0, successType); }, TaskExecutor::TaskType::JS);
            });
        callbackMarkers.emplace(COMMON_SUCCESS, successEventMarker);
    }

    if (callbacks.find(COMMON_CANCEL) != callbacks.end()) {
        auto cancelEventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            cancelEventMarker, [callback, taskExecutor = taskExecutor_] {
                taskExecutor->PostTask([callback]() { callback(1, 0); }, TaskExecutor::TaskType::JS);
            });
        callbackMarkers.emplace(COMMON_CANCEL, cancelEventMarker);
    }

    if (callbacks.find(COMMON_COMPLETE) != callbacks.end()) {
        auto completeEventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            completeEventMarker, [callback, taskExecutor = taskExecutor_] {
                taskExecutor->PostTask([callback]() { callback(2, 0); }, TaskExecutor::TaskType::JS);
            });
        callbackMarkers.emplace(COMMON_COMPLETE, completeEventMarker);
    }

    DialogProperties dialogProperties = {
        .title = title,
        .content = message,
        .autoCancel = autoCancel,
        .buttons = buttons,
        .callbacks = std::move(callbackMarkers),
    };
    auto weak = AceType::WeakClaim(AceType::RawPtr(pipelineContextHolder_.Get()));
    taskExecutor_->PostTask([weak, dialogProperties,
        isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft()]() {
        auto context = weak.Upgrade();
        if (context) {
            context->ShowDialog(dialogProperties, isRightToLeft);
        }
    }, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::ShowActionMenu(const std::string& title,
    const std::vector<ButtonInfo>& button, std::function<void(int32_t, int32_t)>&& callback)
{
    if (!taskExecutor_) {
        LOGE("task executor is null.");
        return;
    }

    std::unordered_map<std::string, EventMarker> callbackMarkers;

    auto successEventMarker = BackEndEventManager<void(int32_t)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(int32_t)>::GetInstance().BindBackendEvent(
        successEventMarker, [callback, number = button.size(), taskExecutor = taskExecutor_](int32_t successType) {
            taskExecutor->PostTask(
                [callback, number, successType]() {
                    // if callback index is larger than button's number, cancel button is selected
                    if (static_cast<size_t>(successType) == number) {
                        callback(1, 0);
                    } else {
                        callback(0, successType);
                    }
                },
                TaskExecutor::TaskType::JS);
        });
    callbackMarkers.emplace(COMMON_SUCCESS, successEventMarker);

    auto cancelEventMarker = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        cancelEventMarker, [callback, taskExecutor = taskExecutor_] {
            taskExecutor->PostTask([callback]() { callback(1, 0); }, TaskExecutor::TaskType::JS);
        });
    callbackMarkers.emplace(COMMON_CANCEL, cancelEventMarker);

    DialogProperties dialogProperties = {
        .title = title,
        .autoCancel = true,
        .isMenu = true,
        .buttons = button,
        .callbacks = std::move(callbackMarkers),
    };
    ButtonInfo buttonInfo = {
        .text = Localization::GetInstance()->GetEntryLetters("common.cancel"),
        .textColor = "#000000"
    };
    dialogProperties.buttons.emplace_back(buttonInfo);
    auto weak = AceType::WeakClaim(AceType::RawPtr(pipelineContextHolder_.Get()));
    taskExecutor_->PostTask([weak, dialogProperties,
        isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft()]() {
        auto context = weak.Upgrade();
        if (context) {
            context->ShowDialog(dialogProperties, isRightToLeft);
        }
    }, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::EnableAlertBeforeBackPage(
    const std::string& message, std::function<void(int32_t)>&& callback)
{
    if (!taskExecutor_) {
        LOGE("task executor is null.");
        return;
    }

    std::unordered_map<std::string, EventMarker> callbackMarkers;
    auto pipelineContext = pipelineContextHolder_.Get();
    auto successEventMarker = BackEndEventManager<void(int32_t)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(int32_t)>::GetInstance().BindBackendEvent(successEventMarker,
        [weak = AceType::WeakClaim(this), callback, taskExecutor = taskExecutor_](int32_t successType) {
            taskExecutor->PostTask(
                [weak, callback, successType]() {
                    callback(successType);
                    if (!successType) {
                        LOGI("dialog choose cancel button, can not back");
                        return;
                    }
                    auto delegate = weak.Upgrade();
                    if (!delegate) {
                        return;
                    }
                    delegate->BackImplement(delegate->backUri_);
                },
                TaskExecutor::TaskType::JS);
        });
    callbackMarkers.emplace(COMMON_SUCCESS, successEventMarker);

    std::lock_guard<std::mutex> lock(mutex_);
    if (pageRouteStack_.empty()) {
        LOGE("page stack is null.");
        return;
    }

    auto& currentPage = pageRouteStack_.back();
    currentPage.isAlertBeforeBackPage = true;
    currentPage.dialogProperties = {
        .content = message,
        .autoCancel = false,
        .buttons = { { .text = Localization::GetInstance()->GetEntryLetters("common.cancel"), .textColor = "" },
            { .text = Localization::GetInstance()->GetEntryLetters("common.ok"), .textColor = "" } },
        .callbacks = std::move(callbackMarkers),
    };
}

void FrontendDelegateImpl::DisableAlertBeforeBackPage()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pageRouteStack_.empty()) {
        LOGE("page stack is null.");
        return;
    }
    auto& currentPage = pageRouteStack_.back();
    currentPage.isAlertBeforeBackPage = false;
}

void FrontendDelegateImpl::SetCallBackResult(const std::string& callBackId, const std::string& result)
{
    jsCallBackResult_.try_emplace(StringToInt(callBackId), result);
}

void FrontendDelegateImpl::WaitTimer(
    const std::string& callbackId, const std::string& delay, bool isInterval, bool isFirst)
{
    if (!isFirst) {
        auto timeoutTaskIter = timeoutTaskMap_.find(callbackId);
        // If not find the callbackId in map, means this timer already was removed,
        // no need create a new cancelableTimer again.
        if (timeoutTaskIter == timeoutTaskMap_.end()) {
            return;
        }
    }

    int32_t delayTime = StringToInt(delay);
    // CancelableCallback class can only be executed once.
    CancelableCallback<void()> cancelableTimer;
    cancelableTimer.Reset([callbackId, delay, isInterval, call = timer_] { call(callbackId, delay, isInterval); });
    auto result = timeoutTaskMap_.try_emplace(callbackId, cancelableTimer);
    if (!result.second) {
        result.first->second = cancelableTimer;
    }
    taskExecutor_->PostDelayedTask(cancelableTimer, TaskExecutor::TaskType::JS, delayTime);
}

void FrontendDelegateImpl::ClearTimer(const std::string& callbackId)
{
    auto timeoutTaskIter = timeoutTaskMap_.find(callbackId);
    if (timeoutTaskIter != timeoutTaskMap_.end()) {
        timeoutTaskIter->second.Cancel();
        timeoutTaskMap_.erase(timeoutTaskIter);
    } else {
        LOGW("ClearTimer callbackId not found");
    }
}

void FrontendDelegateImpl::PostSyncTaskToPage(std::function<void()>&& task)
{
    pipelineContextHolder_.Get(); // Wait until Pipeline Context is attached.
    TriggerPageUpdate(GetRunningPageId(), true);
    taskExecutor_->PostSyncTask(task, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::AddTaskObserver(std::function<void()>&& task)
{
    taskExecutor_->AddTaskObserver(std::move(task));
}

void FrontendDelegateImpl::RemoveTaskObserver()
{
    taskExecutor_->RemoveTaskObserver();
}

bool FrontendDelegateImpl::GetAssetContent(const std::string& url, std::string& content)
{
    return GetAssetContentImpl(assetManager_, url, content);
}

bool FrontendDelegateImpl::GetAssetContent(const std::string& url, std::vector<uint8_t>& content)
{
    return GetAssetContentImpl(assetManager_, url, content);
}

std::string FrontendDelegateImpl::GetAssetPath(const std::string& url)
{
    return GetAssetPathImpl(assetManager_, url);
}

void FrontendDelegateImpl::LoadPage(int32_t pageId, const std::string& url, bool isMainPage, const std::string& params)
{
    LOGD("FrontendDelegateImpl LoadPage[%{private}d]: %{private}s.", pageId, url.c_str());
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pageId_ = pageId;
        pageParamMap_[pageId] = params;
    }
    if (pageId == INVALID_PAGE_ID) {
        LOGE("FrontendDelegateImpl, invalid page id");
        EventReport::SendPageRouterException(PageRouterExcepType::LOAD_PAGE_ERR, url);
        return;
    }
    if (isStagingPageExist_) {
        LOGE("FrontendDelegateImpl, load page failed, waiting for current page loading finish.");
        RecyclePageId(pageId);
        return;
    }
    isStagingPageExist_ = true;
    auto document = AceType::MakeRefPtr<DOMDocument>(pageId);
    auto page = AceType::MakeRefPtr<JsAcePage>(pageId, document, url);
    page->SetPageParams(params);
    page->SetFlushCallback([weak = AceType::WeakClaim(this), isMainPage](const RefPtr<JsAcePage>& acePage) {
        auto delegate = weak.Upgrade();
        if (delegate && acePage) {
            delegate->FlushPageCommand(acePage, acePage->GetUrl(), isMainPage);
        }
    });
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), page, url, isMainPage] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                LOGE("the delegate context is nullptr");
                return;
            }
            delegate->loadJs_(url, page, isMainPage);
            page->FlushCommands();
            // just make sure the pipelineContext is created.
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            if (!pipelineContext) {
                LOGE("the pipeline context is nullptr");
                return;
            }
            pipelineContext->SetMinPlatformVersion(delegate->GetMinPlatformVersion());
            delegate->taskExecutor_->PostTask(
                [weak, page] {
                    auto delegate = weak.Upgrade();
                    if (delegate && delegate->pipelineContextHolder_.Get()) {
                        delegate->pipelineContextHolder_.Get()->FlushFocus();
                    }
                    if (page->GetDomDocument()) {
                        page->GetDomDocument()->HandlePageLoadFinish();
                    }
                },
                TaskExecutor::TaskType::UI);
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnSurfaceChanged()
{
    if (mediaQueryInfo_->GetIsInit()) {
        mediaQueryInfo_->SetIsInit(false);
    }
    mediaQueryInfo_->EnsureListenerIdValid();
    OnMediaQueryUpdate();
}

void FrontendDelegateImpl::OnMediaQueryUpdate()
{
    if (mediaQueryInfo_->GetIsInit()) {
        return;
    }

    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            const auto& info = delegate->mediaQueryInfo_->GetMediaQueryInfo();
            // request css mediaquery
            std::string param("\"viewsizechanged\",");
            param.append(info);
            delegate->asyncEvent_("_root", param);

            // request js mediaquery
            const auto& listenerId = delegate->mediaQueryInfo_->GetListenerId();
            delegate->mediaQueryCallback_(listenerId, info);
            delegate->mediaQueryInfo_->ResetListenerId();
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::OnPageReady(const RefPtr<JsAcePage>& page, const std::string& url, bool isMainPage)
{
    LOGI("OnPageReady url = %{private}s", url.c_str());
    // Pop all JS command and execute them in UI thread.
    auto jsCommands = std::make_shared<std::vector<RefPtr<JsCommand>>>();
    page->PopAllCommands(*jsCommands);

    auto pipelineContext = pipelineContextHolder_.Get();
    page->SetPipelineContext(pipelineContext);
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), page, url, jsCommands, isMainPage] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            delegate->SetCurrentReadyPage(page);
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            bool useLiteStyle = delegate->GetMinPlatformVersion() < COMPATIBLE_VERSION && delegate->IsUseLiteStyle();
            pipelineContext->SetUseLiteStyle(useLiteStyle);
            page->SetUseLiteStyle(useLiteStyle);
            page->SetUseBoxWrap(delegate->GetMinPlatformVersion() >= WEB_FEATURE_VERSION);
            // Flush all JS commands.
            for (const auto& command : *jsCommands) {
                command->Execute(page);
            }
            // Just clear all dirty nodes.
            page->ClearAllDirtyNodes();
            if (page->GetDomDocument()) {
                page->GetDomDocument()->HandleComponentPostBinding();
            }
            if (pipelineContext->GetAccessibilityManager()) {
                pipelineContext->GetAccessibilityManager()->HandleComponentPostBinding();
            }
            if (pipelineContext->CanPushPage()) {
                if (!isMainPage) {
                    delegate->OnPageHide();
                }
                pipelineContext->PushPage(page->BuildPage(url));
                delegate->OnPushPageSuccess(page, url);
                delegate->SetCurrentPage(page->GetPageId());
                delegate->OnMediaQueryUpdate();
            } else {
                // This page has been loaded but become useless now, the corresponding js instance
                // must be destroyed to avoid memory leak.
                delegate->OnPageDestroy(page->GetPageId());
                delegate->ResetStagingPage();
            }
            delegate->isStagingPageExist_ = false;
            if (isMainPage) {
                delegate->OnPageShow();
            }
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::FlushPageCommand(const RefPtr<JsAcePage>& page, const std::string& url, bool isMainPage)
{
    if (!page) {
        return;
    }
    LOGI("FlushPageCommand FragmentCount(%{public}d)", page->FragmentCount());
    if (page->FragmentCount() == 1) {
        OnPageReady(page, url, isMainPage);
    } else {
        TriggerPageUpdate(page->GetPageId());
    }
}

void FrontendDelegateImpl::AddPageLocked(const RefPtr<JsAcePage>& page)
{
    auto result = pageMap_.try_emplace(page->GetPageId(), page);
    if (!result.second) {
        LOGW("the page has already in the map");
    }
}

void FrontendDelegateImpl::SetCurrentPage(int32_t pageId)
{
    LOGD("FrontendDelegateImpl SetCurrentPage pageId=%{private}d", pageId);
    auto page = GetPage(pageId);
    if (page != nullptr) {
        jsAccessibilityManager_->SetRunningPage(page);
        taskExecutor_->PostTask([updatePage = updatePage_, page] { updatePage(page); }, TaskExecutor::TaskType::JS);
    } else {
        LOGE("FrontendDelegateImpl SetCurrentPage page is null.");
    }
}

void FrontendDelegateImpl::OnPushPageSuccess(const RefPtr<JsAcePage>& page, const std::string& url)
{
    std::lock_guard<std::mutex> lock(mutex_);
    AddPageLocked(page);
    pageRouteStack_.emplace_back(PageInfo { page->GetPageId(), url });
    if (pageRouteStack_.size() >= MAX_ROUTER_STACK) {
        isRouteStackFull_ = true;
        EventReport::SendPageRouterException(PageRouterExcepType::PAGE_STACK_OVERFLOW_ERR, page->GetUrl());
    }
    auto pipelineContext = pipelineContextHolder_.Get();
    if (pipelineContext) {
        pipelineContext->onRouterChange(url);
        pipelineContext->NotifyPopPageSuccessDismiss(url, pageRouteStack_.back().pageId);
    }
    LOGI("OnPushPageSuccess size=%{private}zu,pageId=%{private}d,url=%{private}s", pageRouteStack_.size(),
        pageRouteStack_.back().pageId, pageRouteStack_.back().url.c_str());
}

void FrontendDelegateImpl::OnPopToPageSuccess(const std::string& url)
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!pageRouteStack_.empty()) {
        if (pageRouteStack_.back().url == url) {
            break;
        }
        OnPageDestroy(pageRouteStack_.back().pageId);
        pageMap_.erase(pageRouteStack_.back().pageId);
        pageParamMap_.erase(pageRouteStack_.back().pageId);
        pageRouteStack_.pop_back();
    }
    if (isRouteStackFull_) {
        isRouteStackFull_ = false;
    }
    auto pipelineContext = pipelineContextHolder_.Get();
    if (pipelineContext) {
        pipelineContext->onRouterChange(url);
        pipelineContext->NotifyPopPageSuccessDismiss(url, pageRouteStack_.back().pageId);
    }
}

void FrontendDelegateImpl::PopToPage(const std::string& url)
{
    LOGD("FrontendDelegateImpl PopToPage url = %{private}s", url.c_str());
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), url] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            auto pageId = delegate->GetPageIdByUrl(url);
            if (pageId == INVALID_PAGE_ID) {
                return;
            }
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            if (!pipelineContext->CanPopPage()) {
                delegate->ResetStagingPage();
                return;
            }
            delegate->OnPageHide();
            pipelineContext->RemovePageTransitionListener(delegate->pageTransitionListenerId_);
            delegate->pageTransitionListenerId_ = pipelineContext->AddPageTransitionListener(
                [weak, url, pageId](
                    const TransitionEvent& event, const WeakPtr<PageElement>& in, const WeakPtr<PageElement>& out) {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->PopToPageTransitionListener(event, url, pageId);
                    }
                });
            pipelineContext->PopToPage(pageId);
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::PopToPageTransitionListener(
    const TransitionEvent& event, const std::string& url, int32_t pageId)
{
    if (event == TransitionEvent::POP_END) {
        OnPopToPageSuccess(url);
        SetCurrentPage(pageId);
        OnPageShow();
        OnMediaQueryUpdate();
    }
}

int32_t FrontendDelegateImpl::OnPopPageSuccess()
{
    std::lock_guard<std::mutex> lock(mutex_);
    pageMap_.erase(pageRouteStack_.back().pageId);
    pageParamMap_.erase(pageRouteStack_.back().pageId);
    pageRouteStack_.pop_back();
    if (isRouteStackFull_) {
        isRouteStackFull_ = false;
    }
    if (!pageRouteStack_.empty()) {
        auto context = pipelineContextHolder_.Get();
        if (context) {
            context->NotifyPopPageSuccessDismiss(pageRouteStack_.back().url, pageRouteStack_.back().pageId);
            context->onRouterChange(pageRouteStack_.back().url);
        }

        return pageRouteStack_.back().pageId;
    }
    return INVALID_PAGE_ID;
}

void FrontendDelegateImpl::PopPage()
{
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            if (delegate->GetStackSize() == 1) {
                if (delegate->disallowPopLastPage_) {
                    LOGW("Not allow back because this is the last page!");
                    return;
                }
                delegate->OnPageHide();
                delegate->OnPageDestroy(delegate->GetRunningPageId());
                delegate->OnPopPageSuccess();
                pipelineContext->Finish();
                return;
            }
            if (!pipelineContext->CanPopPage()) {
                delegate->ResetStagingPage();
                return;
            }
            delegate->OnPageHide();
            pipelineContext->RemovePageTransitionListener(delegate->pageTransitionListenerId_);
            delegate->pageTransitionListenerId_ = pipelineContext->AddPageTransitionListener(
                [weak, destroyPageId = delegate->GetRunningPageId()](
                    const TransitionEvent& event, const WeakPtr<PageElement>& in, const WeakPtr<PageElement>& out) {
                    auto delegate = weak.Upgrade();
                    if (delegate) {
                        delegate->PopPageTransitionListener(event, destroyPageId);
                    }
                });
            pipelineContext->PopPage();
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::PopPageTransitionListener(const TransitionEvent& event, int32_t destroyPageId)
{
    if (event == TransitionEvent::POP_END) {
        OnPageDestroy(destroyPageId);
        auto pageId = OnPopPageSuccess();
        SetCurrentPage(pageId);
        OnPageShow();
        OnMediaQueryUpdate();
    }
}

int32_t FrontendDelegateImpl::OnClearInvisiblePagesSuccess()
{
    std::lock_guard<std::mutex> lock(mutex_);
    PageInfo pageInfo = std::move(pageRouteStack_.back());
    pageRouteStack_.pop_back();
    for (const auto& info : pageRouteStack_) {
        OnPageDestroy(info.pageId);
        pageMap_.erase(info.pageId);
        pageParamMap_.erase(info.pageId);
    }
    pageRouteStack_.clear();
    int32_t resPageId = pageInfo.pageId;
    pageRouteStack_.emplace_back(std::move(pageInfo));
    if (isRouteStackFull_) {
        isRouteStackFull_ = false;
    }
    return resPageId;
}

void FrontendDelegateImpl::ClearInvisiblePages()
{
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            if (pipelineContext->ClearInvisiblePages()) {
                auto pageId = delegate->OnClearInvisiblePagesSuccess();
                delegate->SetCurrentPage(pageId);
            }
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::OnReplacePageSuccess(const RefPtr<JsAcePage>& page, const std::string& url)
{
    if (!page) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    AddPageLocked(page);
    if (!pageRouteStack_.empty()) {
        pageMap_.erase(pageRouteStack_.back().pageId);
        pageParamMap_.erase(pageRouteStack_.back().pageId);
        pageRouteStack_.pop_back();
    }
    pageRouteStack_.emplace_back(PageInfo { page->GetPageId(), url });
    auto pipelineContext = pipelineContextHolder_.Get();
    if (pipelineContext) {
        pipelineContext->onRouterChange(url);
        pipelineContext->NotifyPopPageSuccessDismiss(url, pageRouteStack_.back().pageId);
    }
}

void FrontendDelegateImpl::ReplacePage(const RefPtr<JsAcePage>& page, const std::string& url)
{
    LOGI("ReplacePage url = %{private}s", url.c_str());
    // Pop all JS command and execute them in UI thread.
    auto jsCommands = std::make_shared<std::vector<RefPtr<JsCommand>>>();
    page->PopAllCommands(*jsCommands);

    auto pipelineContext = pipelineContextHolder_.Get();
    page->SetPipelineContext(pipelineContext);
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), page, url, jsCommands] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            delegate->SetCurrentReadyPage(page);
            auto pipelineContext = delegate->pipelineContextHolder_.Get();
            bool useLiteStyle = delegate->GetMinPlatformVersion() < COMPATIBLE_VERSION && delegate->IsUseLiteStyle();
            pipelineContext->SetUseLiteStyle(useLiteStyle);
            page->SetUseLiteStyle(useLiteStyle);
            page->SetUseBoxWrap(delegate->GetMinPlatformVersion() >= WEB_FEATURE_VERSION);
            // Flush all JS commands.
            for (const auto& command : *jsCommands) {
                command->Execute(page);
            }
            // Just clear all dirty nodes.
            page->ClearAllDirtyNodes();
            page->GetDomDocument()->HandleComponentPostBinding();
            if (pipelineContext->GetAccessibilityManager()) {
                pipelineContext->GetAccessibilityManager()->HandleComponentPostBinding();
            }
            if (pipelineContext->CanReplacePage()) {
                delegate->OnPageHide();
                delegate->OnPageDestroy(delegate->GetRunningPageId());
                pipelineContext->ReplacePage(page->BuildPage(url));
                delegate->OnReplacePageSuccess(page, url);
                delegate->SetCurrentPage(page->GetPageId());
                delegate->OnMediaQueryUpdate();
            } else {
                // This page has been loaded but become useless now, the corresponding js instance
                // must be destroyed to avoid memory leak.
                delegate->OnPageDestroy(page->GetPageId());
                delegate->ResetStagingPage();
            }
            delegate->isStagingPageExist_ = false;
        },
        TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::LoadReplacePage(int32_t pageId, const std::string& url, const std::string& params)
{
    LOGD("FrontendDelegateImpl LoadReplacePage[%{private}d]: %{private}s.", pageId, url.c_str());
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pageId_ = pageId;
        pageParamMap_[pageId] = params;
    }
    if (pageId == INVALID_PAGE_ID) {
        LOGE("FrontendDelegateImpl, invalid page id");
        EventReport::SendPageRouterException(PageRouterExcepType::REPLACE_PAGE_ERR, url);
        return;
    }
    if (isStagingPageExist_) {
        LOGE("FrontendDelegateImpl, replace page failed, waiting for current page loading finish.");
        EventReport::SendPageRouterException(PageRouterExcepType::REPLACE_PAGE_ERR, url);
        return;
    }
    isStagingPageExist_ = true;
    auto document = AceType::MakeRefPtr<DOMDocument>(pageId);
    auto page = AceType::MakeRefPtr<JsAcePage>(pageId, document, url);
    page->SetPageParams(params);
    taskExecutor_->PostTask(
        [page, url, weak = AceType::WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->loadJs_(url, page, false);
                delegate->ReplacePage(page, url);
            }
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateImpl::RebuildAllPages()
{
    std::unordered_map<int32_t, RefPtr<JsAcePage>> pages;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pages.insert(pageMap_.begin(), pageMap_.end());
    }
    for (const auto& [pageId, page] : pages) {
        taskExecutor_->PostTask(
            [weakPage = WeakPtr<JsAcePage>(page)] {
                auto page = weakPage.Upgrade();
                if (!page) {
                    return;
                }
                auto domDoc = page->GetDomDocument();
                if (!domDoc) {
                    return;
                }
                auto rootNode = domDoc->GetDOMNodeById(domDoc->GetRootNodeId());
                if (!rootNode) {
                    return;
                }
                rootNode->UpdateStyleWithChildren();
            },
            TaskExecutor::TaskType::UI);
    }
}

void FrontendDelegateImpl::OnPageShow()
{
    FireAsyncEvent("_root", std::string("\"viewappear\",null,null"), std::string(""));
}

void FrontendDelegateImpl::OnPageHide()
{
    FireAsyncEvent("_root", std::string("\"viewdisappear\",null,null"), std::string(""));
}

void FrontendDelegateImpl::OnConfigurationUpdated(const std::string& configurationData)
{
    FireSyncEvent("_root", std::string("\"onConfigurationUpdated\","), configurationData);
}

void FrontendDelegateImpl::OnPageDestroy(int32_t pageId)
{
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), pageId] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->destroyPage_(pageId);
                delegate->RecyclePageId(pageId);
            }
        },
        TaskExecutor::TaskType::JS);
}

int32_t FrontendDelegateImpl::GetRunningPageId() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pageRouteStack_.empty()) {
        return INVALID_PAGE_ID;
    }
    return pageRouteStack_.back().pageId;
}

std::string FrontendDelegateImpl::GetRunningPageUrl() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pageRouteStack_.empty()) {
        return std::string();
    }
    const auto& pageUrl = pageRouteStack_.back().url;
    auto pos = pageUrl.rfind(".js");
    if (pos == pageUrl.length() - 3) {
        return pageUrl.substr(0, pos);
    }
    return pageUrl;
}

int32_t FrontendDelegateImpl::GetPageIdByUrl(const std::string& url)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto pageIter = std::find_if(std::rbegin(pageRouteStack_), std::rend(pageRouteStack_),
        [&url](const PageInfo& pageRoute) { return url == pageRoute.url; });
    if (pageIter != std::rend(pageRouteStack_)) {
        LOGD("GetPageIdByUrl pageId=%{private}d url=%{private}s", pageIter->pageId, url.c_str());
        return pageIter->pageId;
    }
    return INVALID_PAGE_ID;
}

RefPtr<JsAcePage> FrontendDelegateImpl::GetPage(int32_t pageId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto itPage = pageMap_.find(pageId);
    if (itPage == pageMap_.end()) {
        LOGE("the page is not in the map");
        return nullptr;
    }
    return itPage->second;
}

void FrontendDelegateImpl::RegisterFont(const std::string& familyName, const std::string& familySrc)
{
    pipelineContextHolder_.Get()->RegisterFont(familyName, familySrc);
}

void FrontendDelegateImpl::HandleImage(const std::string& src, std::function<void(bool, int32_t, int32_t)>&& callback)
{
    if (src.empty() || !callback) {
        return;
    }
    auto loadCallback = [jsCallback = std::move(callback), taskExecutor = taskExecutor_](
                            bool success, int32_t width, int32_t height) {
        taskExecutor->PostTask(
            [callback = std::move(jsCallback), success, width, height] { callback(success, width, height); },
            TaskExecutor::TaskType::JS);
    };
    pipelineContextHolder_.Get()->TryLoadImageInfo(src, std::move(loadCallback));
}

void FrontendDelegateImpl::RequestAnimationFrame(const std::string& callbackId)
{
    CancelableCallback<void()> cancelableTask;
    cancelableTask.Reset([callbackId, call = requestAnimationCallback_, weak = AceType::WeakClaim(this)] {
        auto delegate = weak.Upgrade();
        if (delegate && call) {
            call(callbackId, delegate->GetSystemRealTime());
        }
    });
    animationFrameTaskMap_.try_emplace(callbackId, cancelableTask);
    animationFrameTaskIds_.emplace(callbackId);
}

uint64_t FrontendDelegateImpl::GetSystemRealTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * TO_MILLI + ts.tv_nsec / NANO_TO_MILLI;
}

void FrontendDelegateImpl::CancelAnimationFrame(const std::string& callbackId)
{
    auto animationTaskIter = animationFrameTaskMap_.find(callbackId);
    if (animationTaskIter != animationFrameTaskMap_.end()) {
        animationTaskIter->second.Cancel();
        animationFrameTaskMap_.erase(animationTaskIter);
    } else {
        LOGW("cancelAnimationFrame callbackId not found");
    }
}

void FrontendDelegateImpl::FlushAnimationTasks()
{
    while (!animationFrameTaskIds_.empty()) {
        const auto& callbackId = animationFrameTaskIds_.front();
        if (!callbackId.empty()) {
            auto taskIter = animationFrameTaskMap_.find(callbackId);
            if (taskIter != animationFrameTaskMap_.end()) {
                taskExecutor_->PostTask(taskIter->second, TaskExecutor::TaskType::JS);
            }
        }
        animationFrameTaskIds_.pop();
    }

    auto pageId = GetRunningPageId();
    auto page = GetPage(pageId);
    if (!page) {
        return;
    }
    auto jsPage = AceType::DynamicCast<Framework::JsAcePage>(page);
    if (jsPage && jsPage->GetCommandSize() > 0) {
        TriggerPageUpdate(pageId);
    }
}

SingleTaskExecutor FrontendDelegateImpl::GetAnimationJsTask()
{
    return SingleTaskExecutor::Make(taskExecutor_, TaskExecutor::TaskType::JS);
}

SingleTaskExecutor FrontendDelegateImpl::GetUiTask()
{
    return SingleTaskExecutor::Make(taskExecutor_, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::AttachPipelineContext(const RefPtr<PipelineContext>& context)
{
    context->SetOnPageShow([weak = AceType::WeakClaim(this)] {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnPageShow();
        }
    });
    context->SetAnimationCallback([weak = AceType::WeakClaim(this)] {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->FlushAnimationTasks();
        }
    });
    pipelineContextHolder_.Attach(context);
    jsAccessibilityManager_->SetPipelineContext(context);
    jsAccessibilityManager_->InitializeCallback();
}

RefPtr<PipelineContext> FrontendDelegateImpl::GetPipelineContext()
{
    return pipelineContextHolder_.Get();
}

void FrontendDelegateImpl::SetColorMode(ColorMode colorMode)
{
    mediaQueryInfo_->EnsureListenerIdValid();
    OnMediaQueryUpdate();
}

void FrontendDelegateImpl::LoadResourceConfiguration(std::map<std::string, std::string>& mediaResourceFileMap,
    std::unique_ptr<JsonValue>& currentResourceData)
{
    std::vector<std::string> files;
    if (assetManager_) {
        assetManager_->GetAssetList(RESOURCES_FOLDER, files);
    }

    std::set<std::string> resourceFolderName;
    for (const auto& file : files) {
        resourceFolderName.insert(file.substr(0, file.find_first_of("/")));
    }

    std::vector<std::string> sortedResourceFolderPath =
        AceResConfig::GetDeclarativeResourceFallback(resourceFolderName);
    for (const auto& folderName : sortedResourceFolderPath) {
        auto fileFullPath = std::string(RESOURCES_FOLDER) + folderName + std::string(I18N_FILE_SUFFIX);
        std::string content;
        if (GetAssetContent(fileFullPath, content)) {
            auto fileData = ParseFileData(content);
            if (fileData == nullptr) {
                LOGW("parse %{private}s i18n content failed", fileFullPath.c_str());
            } else {
                currentResourceData->Put(fileData);
            }
        }
    }

    std::set<std::string> mediaFileName;
    for (const auto& file : files) {
        auto mediaPathName = file.substr(file.find_first_of("/"));
        std::regex mediaPartten("^\\/media\\/\\w*(\\.jpg|\\.png|\\.gif|\\.svg|\\.webp|\\.bmp)$");
        std::smatch result;
        if (std::regex_match(mediaPathName, result, mediaPartten)) {
            mediaFileName.insert(mediaPathName.substr(mediaPathName.find_first_of("/")));
        }
    }

    auto currentResTag = AceResConfig::GetCurrentDeviceResTag();
    auto currentResolutionTag = currentResTag.substr(currentResTag.find_last_of("-") + 1);
    for (auto folderName : sortedResourceFolderPath) {
        for (auto fileName : mediaFileName) {
            if (mediaResourceFileMap.find(fileName) != mediaResourceFileMap.end()) {
                continue;
            }
            auto fullFileName = folderName + fileName;
            if (std::find(files.begin(), files.end(), fullFileName) != files.end()) {
                mediaResourceFileMap.emplace(fileName.substr(fileName.find_last_of("/") + 1),
                    std::string(RESOURCES_FOLDER).append(fullFileName));
            }
        }
        if (mediaResourceFileMap.size() == mediaFileName.size()) {
            break;
        }
    }
}

void FrontendDelegateImpl::PushJsCallbackToRenderNode(NodeId id, double ratio,
    std::function<void(bool, double)>&& callback)
{
    auto visibleCallback = [jsCallback = std::move(callback), excutor = taskExecutor_] (bool visible, double ratio) {
        excutor->PostTask([task = std::move(jsCallback), visible, ratio] {
            if (task) {
                task(visible, ratio);
            }
        }, TaskExecutor::TaskType::JS);
    };
    auto uiPushTask = [id, ratio, visibleCallback, pipeline = pipelineContextHolder_.Get()]() {
        if (pipeline) {
            pipeline->PushVisibleCallback(id, ratio, visibleCallback);
        }
    };
    taskExecutor_->PostTask(uiPushTask, TaskExecutor::TaskType::UI);
}

void FrontendDelegateImpl::CallNativeHandler(const std::string& event, const std::string& params)
{
    if (callNativeHandler_ != nullptr) {
        callNativeHandler_(event, params);
    }
}

} // namespace OHOS::Ace::Framework
