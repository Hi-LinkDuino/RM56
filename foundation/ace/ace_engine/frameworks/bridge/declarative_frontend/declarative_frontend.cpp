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

#include "frameworks/bridge/declarative_frontend/declarative_frontend.h"

#include <memory>

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/common/thread_checker.h"
#include "core/components/navigator/navigator_component.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"

namespace OHOS::Ace {
namespace {

/*
 * NOTE:
 * This function is needed to copy the values from BaseEventInfo
 * It is observed, that the owner of BaseEventInfo will delete the pointer before it is ultimately
 * processed by the EventMarker callback. In order to avoid this, a copy of all data needs to be made.
 */
std::shared_ptr<BaseEventInfo> CopyEventInfo(const BaseEventInfo& info)
{
    const auto* touchInfo = TypeInfoHelper::DynamicCast<TouchEventInfo>(&info);
    if (touchInfo != nullptr) {
        return std::make_shared<TouchEventInfo>(*touchInfo);
    }

    const auto* dragStartInfo = TypeInfoHelper::DynamicCast<DragStartInfo>(&info);
    if (dragStartInfo != nullptr) {
        return std::make_shared<DragStartInfo>(*dragStartInfo);
    }

    const auto* dragUpdateInfo = TypeInfoHelper::DynamicCast<DragUpdateInfo>(&info);
    if (dragUpdateInfo != nullptr) {
        return std::make_shared<DragUpdateInfo>(*dragUpdateInfo);
    }

    const auto* dragEndInfo = TypeInfoHelper::DynamicCast<DragEndInfo>(&info);
    if (dragEndInfo != nullptr) {
        return std::make_shared<DragEndInfo>(*dragEndInfo);
    }

    const auto* clickInfo = TypeInfoHelper::DynamicCast<ClickInfo>(&info);
    if (clickInfo != nullptr) {
        return std::make_shared<ClickInfo>(*clickInfo);
    }
    return nullptr;
}

void TouchInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{\"touches\":[{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<TouchEventInfo>(&info);
    if (touchInfo) {
        auto touchList = touchInfo->GetTouches();
        for (const auto& location : touchList) {
            auto globalLocation = location.GetGlobalLocation();
            eventParam.append("\"globalX\":")
                .append(std::to_string(globalLocation.GetX()))
                .append(",\"globalY\":")
                .append(std::to_string(globalLocation.GetY()))
                .append(",");
            auto localLocation = location.GetLocalLocation();
            eventParam.append("\"localX\":")
                .append(std::to_string(localLocation.GetX()))
                .append(",\"localY\":")
                .append(std::to_string(localLocation.GetY()))
                .append(",");
            eventParam.append("\"size\":").append(std::to_string(location.GetSize())).append(",");
        }
        if (eventParam.back() == ',') {
            eventParam.pop_back();
        }
        eventParam.append("}],\"changedTouches\":[{");
        auto changeTouch = touchInfo->GetChangedTouches();
        for (const auto& change : changeTouch) {
            auto globalLocation = change.GetGlobalLocation();
            eventParam.append("\"globalX\":")
                .append(std::to_string(globalLocation.GetX()))
                .append(",\"globalY\":")
                .append(std::to_string(globalLocation.GetY()))
                .append(",");
            auto localLocation = change.GetLocalLocation();
            eventParam.append("\"localX\":")
                .append(std::to_string(localLocation.GetX()))
                .append(",\"localY\":")
                .append(std::to_string(localLocation.GetY()))
                .append(",");
            eventParam.append("\"size\":").append(std::to_string(change.GetSize())).append(",");
        }
        if (eventParam.back() == ',') {
            eventParam.pop_back();
        }
    }
    eventParam.append("}]}");
}

void MouseInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    const auto mouseInfo = TypeInfoHelper::DynamicCast<MouseEventInfo>(&info);
    eventParam.append("{\"mouse\":{");
    if (mouseInfo) {
        auto globalMouse = mouseInfo->GetGlobalMouse();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalMouse.x))
            .append(",\"globalY\":")
            .append(std::to_string(globalMouse.y))
            .append(",\"globalZ\":")
            .append(std::to_string(globalMouse.z))
            .append(",\"localX\":")
            .append(std::to_string(globalMouse.x))
            .append(",\"localY\":")
            .append(std::to_string(globalMouse.y))
            .append(",\"localZ\":")
            .append(std::to_string(globalMouse.z))
            .append(",\"deltaX\":")
            .append(std::to_string(globalMouse.deltaX))
            .append(",\"deltaY\":")
            .append(std::to_string(globalMouse.deltaY))
            .append(",\"deltaZ\":")
            .append(std::to_string(globalMouse.deltaZ))
            .append(",\"scrollX\":")
            .append(std::to_string(globalMouse.scrollX))
            .append(",\"scrollY\":")
            .append(std::to_string(globalMouse.scrollY))
            .append(",\"scrollZ\":")
            .append(std::to_string(globalMouse.scrollZ))
            .append(",\"action\":")
            .append(std::to_string(static_cast<int32_t>(globalMouse.action)))
            .append(",\"button\":")
            .append(std::to_string(static_cast<int32_t>(globalMouse.button)))
            .append(",\"pressedButtons\":")
            .append(std::to_string(globalMouse.pressedButtons));
    }
    eventParam.append("}}");
}

void SwipeInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    const auto& swipeInfo = TypeInfoHelper::DynamicCast<SwipeEventInfo>(&info);
    eventParam = swipeInfo->ToJsonParamInfo();
}

} // namespace

DeclarativeFrontend::~DeclarativeFrontend() noexcept
{
    LOG_DESTROY();
}

void DeclarativeFrontend::Destroy()
{
    CHECK_RUN_ON(JS);
    LOGI("DeclarativeFrontend Destroy begin.");
    // To guarantee the jsEngine_ and delegate_ released in js thread
    delegate_.Reset();
    handler_.Reset();
    jsEngine_->Destroy();
    jsEngine_.Reset();
    LOGI("DeclarativeFrontend Destroy end.");
}

bool DeclarativeFrontend::Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor)
{
    LOGI("DeclarativeFrontend initialize begin.");
    type_ = type;
    ACE_DCHECK(type_ == FrontendType::DECLARATIVE_JS);
    InitializeFrontendDelegate(taskExecutor);
    taskExecutor->PostTask(
        [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_), delegate = delegate_] {
            auto jsEngine = weakEngine.Upgrade();
            if (!jsEngine) {
                return;
            }
            jsEngine->Initialize(delegate);
        },
        TaskExecutor::TaskType::JS);

    LOGI("DeclarativeFrontend initialize end.");
    return true;
}

void DeclarativeFrontend::AttachPipelineContext(const RefPtr<PipelineContext>& context)
{
    LOGI("DeclarativeFrontend AttachPipelineContext.");
    if (!delegate_) {
        return;
    }
    handler_ = AceType::MakeRefPtr<DeclarativeEventHandler>(delegate_);
    context->RegisterEventHandler(handler_);
    delegate_->AttachPipelineContext(context);
}

void DeclarativeFrontend::AttachSubPipelineContext(const RefPtr<PipelineContext>& context)
{
    LOGI("DeclarativeFrontend AttachSubPipelineContext.");
    if (!delegate_) {
        return;
    }
    context->RegisterEventHandler(handler_);
}

void DeclarativeFrontend::SetAssetManager(const RefPtr<AssetManager>& assetManager)
{
    LOGI("DeclarativeFrontend SetAssetManager.");
    if (delegate_) {
        delegate_->SetAssetManager(assetManager);
    }
}

void DeclarativeFrontend::InitializeFrontendDelegate(const RefPtr<TaskExecutor>& taskExecutor)
{
    const auto& loadCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](const std::string& url,
                                   const RefPtr<Framework::JsAcePage>& jsPage, bool isMainPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->LoadJs(url, jsPage, isMainPage);
    };

    const auto& setPluginMessageTransferCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                       const RefPtr<JsMessageDispatcher>& dispatcher) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->SetJsMessageDispatcher(dispatcher);
    };

    const auto& asyncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                         const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireAsyncEvent(eventId, param);
    };

    const auto& syncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                        const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireSyncEvent(eventId, param);
    };

    const auto& updatePageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                         const RefPtr<Framework::JsAcePage>& jsPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateRunningPage(jsPage);
        jsEngine->UpdateStagingPage(jsPage);
    };

    const auto& resetStagingPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->ResetStagingPage();
    };

    const auto& destroyPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](int32_t pageId) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyPageInstance(pageId);
    };

    const auto& destroyApplicationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                 const std::string& packageName) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyApplication(packageName);
    };

    const auto& updateApplicationStateCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                     const std::string& packageName, Frontend::State state) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateApplicationState(packageName, state);
    };

    const auto& onWindowDisplayModeChangedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                         bool isShownInMultiWindow, const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnWindowDisplayModeChanged(isShownInMultiWindow, data);
    };

    const auto& onSaveAbilityStateCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnSaveAbilityState(data);
    };
    const auto& onRestoreAbilityStateCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                    const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnRestoreAbilityState(data);
    };

    const auto& onNewWantCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            LOGE("the js engine is nullptr");
            return;
        }
        jsEngine->OnNewWant(data);
    };
    const auto& onActiveCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnActive();
    };

    const auto& onInactiveCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnInactive();
    };

    const auto& onConfigurationUpdatedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                     const std::string& data) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnConfigurationUpdated(data);
    };

    const auto& timerCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                    const std::string& callbackId, const std::string& delay, bool isInterval) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->TimerCallback(callbackId, delay, isInterval);
    };

    const auto& mediaQueryCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                         const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->MediaQueryCallback(callbackId, args);
    };

    const auto& requestAnimationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                               const std::string& callbackId, uint64_t timeStamp) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->RequestAnimationCallback(callbackId, timeStamp);
    };

    const auto& jsCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                 const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->JsCallback(callbackId, args);
    };

    const auto& onMemoryLevelCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](const int32_t level) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnMemoryLevel(level);
    };

    const auto& onStartContinuationCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnStartContinuation();
    };
    const auto& onCompleteContinuationCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](int32_t code) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnCompleteContinuation(code);
    };
    const auto& onRemoteTerminatedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnRemoteTerminated();
    };
    const auto& onSaveDataCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](std::string& savedData) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnSaveData(savedData);
    };
    const auto& onRestoreDataCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                            const std::string& data) -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnRestoreData(data);
    };

    const auto& externalEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                            const std::string& componentId, const uint32_t nodeId) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireExternalEvent(componentId, nodeId);
    };

    delegate_ = AceType::MakeRefPtr<Framework::FrontendDelegateDeclarative>(taskExecutor, loadCallback,
        setPluginMessageTransferCallback, asyncEventCallback, syncEventCallback, updatePageCallback,
        resetStagingPageCallback, destroyPageCallback, destroyApplicationCallback, updateApplicationStateCallback,
        timerCallback, mediaQueryCallback, requestAnimationCallback, jsCallback, onWindowDisplayModeChangedCallBack,
        onConfigurationUpdatedCallBack, onSaveAbilityStateCallBack, onRestoreAbilityStateCallBack, onNewWantCallBack,
        onActiveCallBack, onInactiveCallBack, onMemoryLevelCallBack, onStartContinuationCallBack,
        onCompleteContinuationCallBack, onRemoteTerminatedCallBack, onSaveDataCallBack, onRestoreDataCallBack,
        externalEventCallback);
    if (disallowPopLastPage_) {
        delegate_->DisallowPopLastPage();
    }
    if (jsEngine_) {
        delegate_->SetGroupJsBridge(jsEngine_->GetGroupJsBridge());
    } else {
        LOGE("the js engine is nullptr");
        EventReport::SendAppStartException(AppStartExcepType::JS_ENGINE_CREATE_ERR);
    }
}

void DeclarativeFrontend::RunPage(int32_t pageId, const std::string& url, const std::string& params)
{
    // Not use this pageId from backend, manage it in FrontendDelegateDeclarative.
    if (delegate_) {
        delegate_->RunPage(url, params, pageProfile_);
    }
}

void DeclarativeFrontend::ReplacePage(const std::string& url, const std::string& params)
{
    if (delegate_) {
        delegate_->Replace(url, params);
    }
}

void DeclarativeFrontend::PushPage(const std::string& url, const std::string& params)
{
    if (delegate_) {
        delegate_->Push(url, params);
    }
}

// navigator component call router
void DeclarativeFrontend::NavigatePage(uint8_t type, const PageTarget& target, const std::string& params)
{
    if (!delegate_) {
        return;
    }
    switch (static_cast<NavigatorType>(type)) {
        case NavigatorType::PUSH:
            delegate_->Push(target, params);
            break;
        case NavigatorType::REPLACE:
            delegate_->Replace(target, params);
            break;
        case NavigatorType::BACK:
            delegate_->BackWithTarget(target, params);
            break;
        default:
            LOGE("Navigator type is invalid!");
            delegate_->BackWithTarget(target, params);
    }
}

std::string DeclarativeFrontend::RestoreRouterStack(const std::string& contentInfo)
{
    if (delegate_) {
        return delegate_->RestoreRouterStack(contentInfo);
    } else {
        return "";
    }
}

std::string DeclarativeFrontend::GetContentInfo() const
{
    if (delegate_) {
        return delegate_->GetContentInfo();
    } else {
        return "";
    }
}

void DeclarativeFrontend::SendCallbackMessage(const std::string& callbackId, const std::string& data) const
{
    if (delegate_) {
        delegate_->OnJSCallback(callbackId, data);
    }
}

void DeclarativeFrontend::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const
{
    if (delegate_) {
        delegate_->SetJsMessageDispatcher(dispatcher);
    }
}

void DeclarativeFrontend::TransferComponentResponseData(int callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferComponentResponseData(callbackId, code, std::move(data));
    }
}

void DeclarativeFrontend::TransferJsResponseData(int callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferJsResponseData(callbackId, code, std::move(data));
    }
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void DeclarativeFrontend::TransferJsResponseDataPreview(int callbackId, int32_t code, ResponseData responseData) const
{
    delegate_->TransferJsResponseDataPreview(callbackId, code, responseData);
}
#endif

void DeclarativeFrontend::TransferJsPluginGetError(int callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    if (delegate_) {
        delegate_->TransferJsPluginGetError(callbackId, errorCode, std::move(errorMessage));
    }
}

void DeclarativeFrontend::TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferJsEventData(callbackId, code, std::move(data));
    }
}

void DeclarativeFrontend::LoadPluginJsCode(std::string&& jsCode) const
{
    if (delegate_) {
        delegate_->LoadPluginJsCode(std::move(jsCode));
    }
}

void DeclarativeFrontend::LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const
{
    if (delegate_) {
        delegate_->LoadPluginJsByteCode(std::move(jsCode), std::move(jsCodeLen));
    }
}

void DeclarativeFrontend::UpdateState(Frontend::State state)
{
    if (!delegate_) {
        return;
    }
    switch (state) {
        case Frontend::State::ON_CREATE:
            break;
        case Frontend::State::ON_SHOW:
        case Frontend::State::ON_HIDE:
            delegate_->UpdateApplicationState(delegate_->GetAppID(), state);
            break;
        case Frontend::State::ON_DESTROY:
            delegate_->OnApplicationDestroy(delegate_->GetAppID());
            break;
        default:
            LOGE("error State: %d", state);
            break;
    }
}

void DeclarativeFrontend::OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data)
{
    delegate_->OnWindowDisplayModeChanged(isShownInMultiWindow, data);
}

void DeclarativeFrontend::OnSaveAbilityState(std::string& data)
{
    if (delegate_) {
        delegate_->OnSaveAbilityState(data);
    }
}

void DeclarativeFrontend::OnRestoreAbilityState(const std::string& data)
{
    if (delegate_) {
        delegate_->OnRestoreAbilityState(data);
    }
}

void DeclarativeFrontend::OnNewWant(const std::string& data)
{
    if (delegate_) {
        delegate_->OnNewWant(data);
    }
}

RefPtr<AccessibilityManager> DeclarativeFrontend::GetAccessibilityManager() const
{
    if (!delegate_) {
        LOGE("GetAccessibilityManager delegate is null");
        return nullptr;
    }
    return delegate_->GetJSAccessibilityManager();
}

WindowConfig& DeclarativeFrontend::GetWindowConfig()
{
    if (!delegate_) {
        static WindowConfig windowConfig;
        LOGW("delegate is null, return default config");
        return windowConfig;
    }
    return delegate_->GetWindowConfig();
}

bool DeclarativeFrontend::OnBackPressed()
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnPageBackPress();
}

void DeclarativeFrontend::OnShow()
{
    if (delegate_) {
        delegate_->OnForground();
    }
}

void DeclarativeFrontend::OnHide()
{
    if (delegate_) {
        delegate_->OnBackGround();
        foregroundFrontend_ = false;
    }
}

void DeclarativeFrontend::OnConfigurationUpdated(const std::string& data)
{
    if (delegate_) {
        delegate_->OnConfigurationUpdated(data);
    }
}

void DeclarativeFrontend::OnActive()
{
    if (delegate_) {
        foregroundFrontend_ = true;
        delegate_->OnActive();
        delegate_->InitializeAccessibilityCallback();
    }
}

void DeclarativeFrontend::OnInactive()
{
    if (delegate_) {
        delegate_->OnInactive();
        delegate_->OnSuspended();
    }
}

bool DeclarativeFrontend::OnStartContinuation()
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnStartContinuation();
}

void DeclarativeFrontend::OnCompleteContinuation(int32_t code)
{
    if (delegate_) {
        delegate_->OnCompleteContinuation(code);
    }
}

void DeclarativeFrontend::OnMemoryLevel(const int32_t level)
{
    if (delegate_) {
        delegate_->OnMemoryLevel(level);
    }
}

void DeclarativeFrontend::OnSaveData(std::string& data)
{
    if (delegate_) {
        delegate_->OnSaveData(data);
    }
}

void DeclarativeFrontend::GetPluginsUsed(std::string& data)
{
    if (delegate_) {
        delegate_->GetPluginsUsed(data);
    }
}

bool DeclarativeFrontend::OnRestoreData(const std::string& data)
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnRestoreData(data);
}

void DeclarativeFrontend::OnRemoteTerminated()
{
    if (delegate_) {
        delegate_->OnRemoteTerminated();
    }
}

void DeclarativeFrontend::OnNewRequest(const std::string& data)
{
    if (delegate_) {
        delegate_->OnNewRequest(data);
    }
}

void DeclarativeFrontend::CallRouterBack()
{
    if (delegate_) {
        if (delegate_->GetStackSize() == 1 && isSubWindow_) {
            LOGW("Can't back because this is the last page of sub window!");
            return;
        }
        delegate_->CallPopPage();
    }
}

void DeclarativeFrontend::OnSurfaceChanged(int32_t width, int32_t height)
{
    if (delegate_) {
        delegate_->OnSurfaceChanged();
    }
}

void DeclarativeFrontend::DumpFrontend() const
{
    if (!delegate_) {
        return;
    }
    int32_t routerIndex = 0;
    std::string routerName;
    std::string routerPath;
    delegate_->GetState(routerIndex, routerName, routerPath);

    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpLog::GetInstance().AddDesc("Path: " + routerPath);
        DumpLog::GetInstance().AddDesc("Length: " + std::to_string(routerIndex));
        DumpLog::GetInstance().Print(0, routerName, 0);
    }
}

void DeclarativeFrontend::TriggerGarbageCollection()
{
    if (jsEngine_) {
        jsEngine_->RunGarbageCollection();
    }
}

void DeclarativeFrontend::SetColorMode(ColorMode colorMode)
{
    if (delegate_) {
        delegate_->SetColorMode(colorMode);
    }
}

void DeclarativeFrontend::RebuildAllPages()
{
    if (delegate_) {
        delegate_->RebuildAllPages();
    }
}

void DeclarativeFrontend::NotifyAppStorage(const std::string& key, const std::string& value)
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return;
    }
    delegate_->NotifyAppStorage(jsEngine_, key, value);
}

RefPtr<Component> DeclarativeFrontend::GetNewComponentWithJsCode(const std::string& jsCode)
{
    if (jsEngine_) {
        return jsEngine_->GetNewComponentWithJsCode(jsCode);
    }
    return nullptr;
}

void DeclarativeEventHandler::HandleAsyncEvent(const EventMarker& eventMarker)
{
    LOGI("HandleAsyncEvent pageId: %{private}d, eventId: %{private}s, eventType: %{private}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string param = eventMarker.GetData().GetEventParam();
    if (eventMarker.GetData().isDeclarativeUi) {
        if (delegate_) {
            delegate_->GetUiTask().PostTask([eventMarker] { eventMarker.CallUiFunction(); });
        }
    } else {
        delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param.append("null"), std::string(""));
    }

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info)
{
    std::string eventParam;
    if (eventMarker.GetData().eventType.find("touch") != std::string::npos) {
        TouchInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType.find("mouse") != std::string::npos) {
        MouseInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "swipe") {
        SwipeInfoToString(info, eventParam);
    }

    LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s, eventType: %{public}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string param = eventMarker.GetData().GetEventParam();
    if (eventParam.empty()) {
        param.append("null");
    } else {
        param.append(eventParam);
    }

    if (eventMarker.GetData().isDeclarativeUi) {
        if (delegate_) {
            auto cinfo = CopyEventInfo(info);
            delegate_->GetUiTask().PostTask([eventMarker, cinfo] { eventMarker.CallUiArgFunction(cinfo.get()); });
        }
    } else {
        delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param, "");
    }

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleAsyncEvent(
    const EventMarker& eventMarker, const std::shared_ptr<BaseEventInfo>& info)
{
    if (eventMarker.GetData().isDeclarativeUi) {
        if (delegate_) {
            delegate_->GetUiTask().PostTask([eventMarker, info] { eventMarker.CallUiArgFunction(info.get()); });
        }
    }
}

void DeclarativeEventHandler::HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result)
{
    LOGD("HandleSyncEvent pageId: %{public}d, eventId: %{public}s, eventType: %{public}s", eventMarker.GetData().pageId,
        eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string param = std::string("\"")
                            .append(eventMarker.GetData().eventType)
                            .append("\",{\"code\":")
                            .append(std::to_string(static_cast<int32_t>(info.code)))
                            .append(",\"action\":")
                            .append(std::to_string(static_cast<int32_t>(info.action)))
                            .append(",\"repeatCount\":")
                            .append(std::to_string(static_cast<int32_t>(info.repeatTime)))
                            .append(",\"timestamp\":")
                            .append(std::to_string(info.timeStamp.time_since_epoch().count()))
                            .append(",\"key\":\"")
                            .append(info.key)
                            .append("\"},");

    result = delegate_->FireSyncEvent(eventMarker.GetData().eventId, param, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = std::to_string(static_cast<int32_t>(info.code));
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, int32_t param)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
{
    LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
        eventMarker.GetData().eventId.c_str());

    if (eventMarker.GetData().isDeclarativeUi) {
        std::string fixParam(param);
        std::string::size_type startPos = param.find_first_of("{");
        std::string::size_type endPos = param.find_last_of("}");
        if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
            fixParam = fixParam.substr(startPos, endPos - startPos + 1);
        }
        if (delegate_) {
            delegate_->GetUiTask().PostTask([eventMarker, fixParam] { eventMarker.CallUiStrFunction(fixParam); });
        }
    } else {
        delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param, "");
    }

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleSyncEvent(const EventMarker& eventMarker, bool& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleSyncEvent(
    const EventMarker& eventMarker, const std::shared_ptr<BaseEventInfo>& info)
{
    if (delegate_) {
        delegate_->GetUiTask().PostSyncTask([eventMarker, info] { eventMarker.CallUiArgFunction(info.get()); });
    }
}

void DeclarativeEventHandler::HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void DeclarativeEventHandler::HandleSyncEvent(
    const EventMarker& eventMarker, const std::string& param, std::string& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
    delegate_->FireSyncEvent(eventMarker.GetData().eventId, param, std::string(""), result);
}

void DeclarativeEventHandler::HandleSyncEvent(
    const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId)
{
    if (delegate_) {
        delegate_->FireExternalEvent(eventMarker.GetData().eventId, componentId, nodeId);
    }
}

} // namespace OHOS::Ace
