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

#include "frameworks/bridge/js_frontend/js_frontend.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/common/thread_checker.h"

namespace OHOS::Ace {
namespace {

void TouchInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{\"touches\":[");
    const auto touchInfo = TypeInfoHelper::DynamicCast<TouchEventInfo>(&info);
    if (touchInfo) {
        auto touchList = touchInfo->GetTouches();
        for (const auto& location : touchList) {
            eventParam.append("{");
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
            eventParam.append("\"identifier\":").append(std::to_string(location.GetFingerId())).append(",");
            eventParam.append("\"force\":").append(std::to_string(location.GetForce())).append("},");
        }
        if (eventParam.back() == ',') {
            eventParam.pop_back();
        }
        eventParam.append("],");
        eventParam.append("\"deviceId\":").append(std::to_string(touchInfo->GetDeviceId())).append(",");
        eventParam.append("\"changedTouches\":[");
        auto changeTouch = touchInfo->GetChangedTouches();
        for (const auto& change : changeTouch) {
            eventParam.append("{");
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
            eventParam.append("\"identifier\":").append(std::to_string(change.GetFingerId())).append(",");
            eventParam.append("\"force\":").append(std::to_string(change.GetForce())).append("},");
        }
        if (eventParam.back() == ',') {
            eventParam.pop_back();
        }
        eventParam.append("]");
    }
    eventParam.append("}");
}

void DragStartInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragStartInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragUpdateInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragUpdateInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragEnterInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragUpdateInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragOverInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragUpdateInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragLeaveInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragUpdateInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragDropInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragEndInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
}

void DragEndInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    eventParam.append("{");
    const auto touchInfo = TypeInfoHelper::DynamicCast<DragEndInfo>(&info);
    if (touchInfo) {
        auto globalLocation = touchInfo->GetGlobalLocation();
        eventParam.append("\"globalX\":")
            .append(std::to_string(globalLocation.GetX()))
            .append(",\"globalY\":")
            .append(std::to_string(globalLocation.GetY()));
    }
    eventParam.append("}");
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
            .append(std::to_string(globalMouse.pressedButtons))
            .append("},")
            .append("\"deviceId\":")
            .append(std::to_string(mouseInfo->GetDeviceId()))
            .append("}");
    } else {
        eventParam.append("}}");
    }
}

void SwipeInfoToString(const BaseEventInfo& info, std::string& eventParam)
{
    const auto& swipeInfo = TypeInfoHelper::DynamicCast<SwipeEventInfo>(&info);
    eventParam = swipeInfo->ToJsonParamInfo();
}

} // namespace

RefPtr<Frontend> Frontend::Create()
{
    return AceType::MakeRefPtr<JsFrontend>();
}

JsFrontend::~JsFrontend() noexcept
{
    LOG_DESTROY();
}

void JsFrontend::Destroy()
{
    CHECK_RUN_ON(JS);
    LOGI("JsFrontend Destroy begin.");
    // To guarantee the jsEngine_ and delegate_ released in js thread
    delegate_.Reset();
    handler_.Reset();
    jsEngine_.Reset();
    LOGI("JsFrontend Destroy end.");
}

bool JsFrontend::Initialize(FrontendType type, const RefPtr<TaskExecutor>& taskExecutor)
{
    LOGI("JsFrontend initialize begin.");
    type_ = type;
    ACE_DCHECK(type_ == FrontendType::JS);
    InitializeFrontendDelegate(taskExecutor);
    auto weakEngine = AceType::WeakClaim(AceType::RawPtr(jsEngine_));
    auto weakDelegate = AceType::WeakClaim(AceType::RawPtr(delegate_));
    taskExecutor->PostTask(
        [weakEngine, weakDelegate] {
            auto jsEngine = weakEngine.Upgrade();
            if (!jsEngine) {
                return;
            }
            jsEngine->Initialize(weakDelegate.Upgrade());
        },
        TaskExecutor::TaskType::JS);
    LOGI("JsFrontend initialize end.");
    return true;
}

void JsFrontend::AttachPipelineContext(const RefPtr<PipelineContext>& context)
{
    if (!delegate_) {
        return;
    }
    handler_ = AceType::MakeRefPtr<JsEventHandler>(delegate_);
    context->RegisterEventHandler(handler_);
    delegate_->AttachPipelineContext(context);
}

void JsFrontend::SetAssetManager(const RefPtr<AssetManager>& assetManager)
{
    if (delegate_) {
        delegate_->SetAssetManager(assetManager);
    }
}

void JsFrontend::InitializeFrontendDelegate(const RefPtr<TaskExecutor>& taskExecutor)
{
    Framework::FrontendDelegateImplBuilder builder;
    builder.loadCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                               const std::string& url, const RefPtr<Framework::JsAcePage>& jsPage, bool isMainPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->LoadJs(url, jsPage, isMainPage);
    };

    builder.transferCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                   const RefPtr<JsMessageDispatcher>& dispatcher) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->SetJsMessageDispatcher(dispatcher);
    };

    builder.asyncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                     const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireAsyncEvent(eventId, param);
    };

    builder.syncEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                    const std::string& eventId, const std::string& param) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireSyncEvent(eventId, param);
    };

    builder.externalEventCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                        const std::string& componentId, const uint32_t nodeId) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->FireExternalEvent(componentId, nodeId);
    };

    builder.updatePageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                     const RefPtr<Framework::JsAcePage>& jsPage) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateRunningPage(jsPage);
        jsEngine->UpdateStagingPage(jsPage);
    };

    builder.resetStagingPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->ResetStagingPage();
    };

    builder.destroyPageCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](int32_t pageId) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyPageInstance(pageId);
    };

    builder.destroyApplicationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                             const std::string& packageName) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->DestroyApplication(packageName);
    };

    builder.updateApplicationStateCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                                 const std::string& packageName, Frontend::State state) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->UpdateApplicationState(packageName, state);
    };

    builder.onStartContinuationCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnStartContinuation();
    };

    builder.onCompleteContinuationCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](int32_t code) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnCompleteContinuation(code);
    };

    builder.onRemoteTerminatedCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)]() {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnRemoteTerminated();
    };

    builder.onSaveDataCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](std::string& savedData) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->OnSaveData(savedData);
    };

    builder.onRestoreDataCallBack = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                        const std::string& data) -> bool {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return false;
        }
        return jsEngine->OnRestoreData(data);
    };

    builder.timerCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                const std::string& callbackId, const std::string& delay, bool isInterval) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->TimerCallback(callbackId, delay, isInterval);
    };

    builder.mediaQueryCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                     const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->MediaQueryCallback(callbackId, args);
    };

    builder.requestAnimationCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                                           const std::string& callbackId, uint64_t timeStamp) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->RequestAnimationCallback(callbackId, timeStamp);
    };

    builder.jsCallback = [weakEngine = WeakPtr<Framework::JsEngine>(jsEngine_)](
                             const std::string& callbackId, const std::string& args) {
        auto jsEngine = weakEngine.Upgrade();
        if (!jsEngine) {
            return;
        }
        jsEngine->JsCallback(callbackId, args);
    };

    builder.callNativeHandler = [this](const std::string& event, const std::string& params) {
        if (dialogCallback_ != nullptr) {
            dialogCallback_(event, params);
        }
    };

    builder.taskExecutor = taskExecutor;
    delegate_ = AceType::MakeRefPtr<Framework::FrontendDelegateImpl>(builder);
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

void JsFrontend::RunPage(int32_t pageId, const std::string& url, const std::string& params)
{
    // Not use this pageId from backend, manage it in FrontendDelegateImpl.
    if (delegate_) {
        delegate_->RunPage(url, params);
    }
}

void JsFrontend::PushPage(const std::string& url, const std::string& params)
{
    if (delegate_) {
        delegate_->Push(url, params);
    }
}

void JsFrontend::ReplacePage(const std::string& url, const std::string& params)
{
    if (delegate_) {
        delegate_->Replace(url, params);
    }
}

void JsFrontend::SendCallbackMessage(const std::string& callbackId, const std::string& data) const
{
    if (delegate_) {
        delegate_->OnJsCallback(callbackId, data);
    }
}

void JsFrontend::SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const
{
    if (delegate_) {
        delegate_->SetJsMessageDispatcher(dispatcher);
    }
}

void JsFrontend::TransferComponentResponseData(int callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferComponentResponseData(callbackId, code, std::move(data));
    }
}

void JsFrontend::TransferJsResponseData(int callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferJsResponseData(callbackId, code, std::move(data));
    }
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void JsFrontend::TransferJsResponseDataPreview(int callbackId, int32_t code, ResponseData responseData) const
{
    if (delegate_) {
        delegate_->TransferJsResponseDataPreview(callbackId, code, responseData);
    }
}
#endif

void JsFrontend::TransferJsPluginGetError(int callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    if (delegate_) {
        delegate_->TransferJsPluginGetError(callbackId, errorCode, std::move(errorMessage));
    }
}

void JsFrontend::TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const
{
    if (delegate_) {
        delegate_->TransferJsEventData(callbackId, code, std::move(data));
    }
}

void JsFrontend::LoadPluginJsCode(std::string&& jsCode) const
{
    if (delegate_) {
        delegate_->LoadPluginJsCode(std::move(jsCode));
    }
}

void JsFrontend::LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const
{
    if (delegate_) {
        delegate_->LoadPluginJsByteCode(std::move(jsCode), std::move(jsCodeLen));
    }
}

void JsFrontend::UpdateState(Frontend::State state)
{
    if (!delegate_) {
        return;
    }
    switch (state) {
        case Frontend::State::ON_CREATE:
            break;
        case Frontend::State::ON_DESTROY:
            LOGI("UpdateState ON_DESTROY");
            delegate_->OnApplicationDestroy(delegate_->GetAppID());
            delegate_->OnApplicationUpdateState(delegate_->GetAppID(), Frontend::State::ON_DESTROY);
            break;
        case Frontend::State::ON_SHOW:
            delegate_->OnApplicationUpdateState(delegate_->GetAppID(), Frontend::State::ON_SHOW);
            break;
        case Frontend::State::ON_HIDE:
            delegate_->OnApplicationUpdateState(delegate_->GetAppID(), Frontend::State::ON_HIDE);
            break;
        default:
            LOGE("error State: %d", state);
            break;
    }
}

RefPtr<AccessibilityManager> JsFrontend::GetAccessibilityManager() const
{
    if (!delegate_) {
        LOGD("GetAccessibilityManager delegate is null");
        return nullptr;
    }
    return delegate_->GetJsAccessibilityManager();
}

WindowConfig& JsFrontend::GetWindowConfig()
{
    if (!delegate_) {
        static WindowConfig windowConfig;
        LOGW("delegate is null, return default config");
        return windowConfig;
    }
    return delegate_->GetWindowConfig();
}

bool JsFrontend::OnBackPressed()
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnPageBackPress();
}

void JsFrontend::OnShow()
{
    if (delegate_) {
        delegate_->OnForground();
        foregroundFrontend_ = true;
    }
}

void JsFrontend::OnHide()
{
    if (delegate_) {
        delegate_->OnBackGround();
        foregroundFrontend_ = false;
    }
}

void JsFrontend::OnConfigurationUpdated(const std::string& data)
{
    if (delegate_) {
        delegate_->OnConfigurationUpdated(data);
    }
}

void JsFrontend::OnActive()
{
    if (delegate_) {
        delegate_->InitializeAccessibilityCallback();
        delegate_->OnActive();
    }
}

void JsFrontend::OnInactive()
{
    if (delegate_) {
        delegate_->OnInactive();
    }
}

bool JsFrontend::OnStartContinuation()
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnStartContinuation();
}

void JsFrontend::OnCompleteContinuation(int32_t code)
{
    if (delegate_) {
        delegate_->OnCompleteContinuation(code);
    }
}

void JsFrontend::OnRemoteTerminated()
{
    if (delegate_) {
        delegate_->OnRemoteTerminated();
    }
}

void JsFrontend::OnSaveData(std::string& data)
{
    if (delegate_) {
        delegate_->OnSaveData(data);
    }
}

bool JsFrontend::OnRestoreData(const std::string& data)
{
    if (!delegate_) {
        LOGW("delegate is null, return false");
        return false;
    }
    return delegate_->OnRestoreData(data);
}

void JsFrontend::OnNewRequest(const std::string& data)
{
    if (delegate_) {
        delegate_->OnNewRequest(data);
    }
}

void JsFrontend::CallRouterBack()
{
    if (delegate_) {
        delegate_->CallPopPage();
    }
}

void JsFrontend::OnSurfaceChanged(int32_t width, int32_t height)
{
    if (delegate_) {
        delegate_->OnSurfaceChanged();
    }
}

void JsFrontend::DumpFrontend() const
{
    int32_t routerIndex = 0;
    std::string routerName;
    std::string routerPath;
    if (delegate_) {
        delegate_->GetState(routerIndex, routerName, routerPath);
    }

    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpLog::GetInstance().AddDesc("Path: " + routerPath);
        DumpLog::GetInstance().AddDesc("Length: " + std::to_string(routerIndex));
        DumpLog::GetInstance().Print(0, routerName, 0);
    }
}

void JsFrontend::TriggerGarbageCollection()
{
    if (jsEngine_) {
        jsEngine_->RunGarbageCollection();
    }
}

void JsFrontend::RebuildAllPages()
{
    if (delegate_) {
        delegate_->RebuildAllPages();
    }
}

void JsFrontend::GetPluginsUsed(std::string& data)
{
    if (!delegate_) {
        LOGW("delegate hasn't initialized, load all system plugin");
        return;
    }

    if (!delegate_->GetAssetContent("module_collection.txt", data)) {
        LOGW("read failed, will load all the system plugin");
    }
}

void JsFrontend::SetColorMode(ColorMode colorMode)
{
    if (delegate_) {
        delegate_->SetColorMode(colorMode);
    }
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker)
{
    LOGD("HandleAsyncEvent pageId: %{private}d, eventId: %{private}s, eventType: %{private}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string param = eventMarker.GetData().GetEventParam();
    delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param.append("null"), std::string(""));

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info)
{
    std::string eventParam;
    if (eventMarker.GetData().eventType.find("touch") != std::string::npos) {
        TouchInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType.find("mouse") != std::string::npos) {
        MouseInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "swipe") {
        SwipeInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "dragstart") {
        DragStartInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "drag") {
        DragUpdateInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "dragend") {
        DragEndInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "dragenter") {
        DragEnterInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "dragover") {
        DragOverInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "dragleave") {
        DragLeaveInfoToString(info, eventParam);
    } else if (eventMarker.GetData().eventType == "drop") {
        DragDropInfoToString(info, eventParam);
    }

    LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s, eventType: %{public}s, eventParam: %{public}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str(),
        eventParam.c_str());
    std::string param;
    auto adapter = TypeInfoHelper::DynamicCast<EventToJSONStringAdapter>(&info);
    if (adapter) {
        LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
            eventMarker.GetData().eventId.c_str());
        param = adapter->ToJSONString();
    } else {
        param = eventMarker.GetData().GetEventParam();
        if (eventParam.empty()) {
            param.append("null");
        } else {
            param.append(eventParam);
        }
    }
    delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result)
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
                            .append(std::to_string(static_cast<int32_t>(info.timeStamp.time_since_epoch().count())))
                            .append(",\"key\":\"")
                            .append(info.key)
                            .append("\"},");

    result = delegate_->FireSyncEvent(eventMarker.GetData().eventId, param, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = std::to_string(static_cast<int32_t>(info.code));
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const GestureEvent& info)
{
    LOGD("HandleASyncEvent pageId: %{public}d, eventId: %{public}s, eventType: %{public}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string eventParam = std::string("");
    if (eventMarker.GetData().eventType.find("pinch") != std::string::npos) {
        eventParam.append("\"")
            .append(eventMarker.GetData().eventType)
            .append("\",{\"scale\":")
            .append(std::to_string(info.GetScale()))
            .append(",\"pinchCenterX\":")
            .append(std::to_string(info.GetPinchCenter().GetX()))
            .append(",\"pinchCenterY\":")
            .append(std::to_string(info.GetPinchCenter().GetY()))
            .append("}");
    }

    delegate_->FireAsyncEvent(eventMarker.GetData().eventId, eventParam, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const RotationEvent& info)
{
    LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s, eventType: %{public}s",
        eventMarker.GetData().pageId, eventMarker.GetData().eventId.c_str(), eventMarker.GetData().eventType.c_str());
    std::string eventParam = std::string("");
    if (eventMarker.GetData().eventType == "rotate") {
        eventParam.append("\"")
            .append(eventMarker.GetData().eventType)
            .append("\",{\"value\":")
            .append(std::to_string(info.value))
            .append("}");
    }

    delegate_->FireAsyncEvent(eventMarker.GetData().eventId, eventParam, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, int32_t param)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
{
    LOGD("HandleAsyncEvent pageId: %{public}d, eventId: %{public}s", eventMarker.GetData().pageId,
        eventMarker.GetData().eventId.c_str());
    delegate_->FireAsyncEvent(eventMarker.GetData().eventId, param, "");

    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleSyncEvent(const EventMarker& eventMarker, bool& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
}

void JsEventHandler::HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result)
{
    LOGW("js event handler does not support this event type!");
    AccessibilityEvent accessibilityEvent;
    accessibilityEvent.nodeId = StringUtils::StringToInt(eventMarker.GetData().eventId);
    accessibilityEvent.eventType = eventMarker.GetData().eventType;
    delegate_->FireAccessibilityEvent(accessibilityEvent);
    delegate_->FireSyncEvent(eventMarker.GetData().eventId, param, std::string(""), result);
}

void JsEventHandler::HandleSyncEvent(
    const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId)
{
    delegate_->FireExternalEvent(eventMarker.GetData().eventId, componentId, nodeId);
}
} // namespace OHOS::Ace
