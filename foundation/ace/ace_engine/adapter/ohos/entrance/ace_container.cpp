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

#include "adapter/ohos/entrance/ace_container.h"

#include "ability_info.h"
#include "flutter/lib/ui/ui_dart_state.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/data_ability_helper_standard.h"
#include "adapter/ohos/entrance/file_asset_provider.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/frame_report.h"
#include "base/log/log.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/ace_engine.h"
#include "core/common/container_scope.h"
#include "core/common/flutter/flutter_asset_manager.h"
#include "core/common/flutter/flutter_task_executor.h"
#include "core/common/hdc_register.h"
#include "core/common/platform_window.h"
#include "core/common/text_field_manager.h"
#include "core/common/watch_dog.h"
#include "core/common/window.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/card_frontend/card_frontend.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/declarative_frontend/declarative_frontend.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine_loader.h"
#include "frameworks/bridge/js_frontend/js_frontend.h"

namespace OHOS::Ace::Platform {
namespace {

constexpr char QUICK_JS_ENGINE_SHARED_LIB[] = "libace_engine_qjs.z.so";
constexpr char ARK_ENGINE_SHARED_LIB[] = "libace_engine_ark.z.so";
constexpr char DECLARATIVE_JS_ENGINE_SHARED_LIB[] = "libace_engine_declarative.z.so";
constexpr char DECLARATIVE_ARK_ENGINE_SHARED_LIB[] = "libace_engine_declarative_ark.z.so";

#ifdef _ARM64_
const std::string ASSET_LIBARCH_PATH = "/lib/arm64";
#else
const std::string ASSET_LIBARCH_PATH = "/lib/arm";
#endif

const char* GetEngineSharedLibrary(bool isArkApp)
{
    if (isArkApp) {
        return ARK_ENGINE_SHARED_LIB;
    } else {
        return QUICK_JS_ENGINE_SHARED_LIB;
    }
}

const char* GetDeclarativeSharedLibrary(bool isArkApp)
{
    if (isArkApp) {
        return DECLARATIVE_ARK_ENGINE_SHARED_LIB;
    } else {
        return DECLARATIVE_JS_ENGINE_SHARED_LIB;
    }
}

} // namespace

AceContainer::AceContainer(int32_t instanceId, FrontendType type, bool isArkApp,
    std::shared_ptr<OHOS::AppExecFwk::Ability> aceAbility, std::unique_ptr<PlatformEventCallback> callback,
    bool useCurrentEventRunner)
    : instanceId_(instanceId), type_(type), isArkApp_(isArkApp), aceAbility_(aceAbility),
      useCurrentEventRunner_(useCurrentEventRunner)
{
    ACE_DCHECK(callback);
    InitializeTask();
    platformEventCallback_ = std::move(callback);
    useStageModel_ = false;
}

AceContainer::AceContainer(int32_t instanceId, FrontendType type, bool isArkApp,
    std::weak_ptr<OHOS::AbilityRuntime::Context> runtimeContext,
    std::weak_ptr<OHOS::AppExecFwk::AbilityInfo> abilityInfo, std::unique_ptr<PlatformEventCallback> callback,
    bool useCurrentEventRunner, bool isSubAceContainer)
    : instanceId_(instanceId), type_(type), isArkApp_(isArkApp), runtimeContext_(runtimeContext),
      abilityInfo_(abilityInfo), useCurrentEventRunner_(useCurrentEventRunner), isSubContainer_(isSubAceContainer)
{
    ACE_DCHECK(callback);
    if (!isSubContainer_) {
        InitializeTask();
    }
    platformEventCallback_ = std::move(callback);
    useStageModel_ = true;
}

void AceContainer::InitializeTask()
{
    auto flutterTaskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    flutterTaskExecutor->InitPlatformThread(useCurrentEventRunner_);
    taskExecutor_ = flutterTaskExecutor;
    // No need to create JS Thread for DECLARATIVE_JS
    if (type_ == FrontendType::DECLARATIVE_JS) {
        GetSettings().useUIAsJSThread = true;
    } else {
        flutterTaskExecutor->InitJsThread();
    }
}

void AceContainer::Initialize()
{
    ContainerScope scope(instanceId_);
    // For DECLARATIVE_JS frontend use UI as JS Thread, so InitializeFrontend after UI thread created.
    if (type_ != FrontendType::DECLARATIVE_JS) {
        InitializeFrontend();
    }
}

void AceContainer::Destroy()
{
    ContainerScope scope(instanceId_);
    if (pipelineContext_ && taskExecutor_) {
        // 1. Destroy Pipeline on UI thread.
        RefPtr<PipelineContext> context;
        context.Swap(pipelineContext_);
        taskExecutor_->PostTask([context]() { context->Destroy(); }, TaskExecutor::TaskType::UI);

        if (isSubContainer_) {
            // SubAcecontainer just return.
            return;
        }
        // 2. Destroy Frontend on JS thread.
        RefPtr<Frontend> frontend;
        frontend_.Swap(frontend);
        taskExecutor_->PostTask(
            [frontend]() {
                frontend->UpdateState(Frontend::State::ON_DESTROY);
                frontend->Destroy();
            },
            TaskExecutor::TaskType::JS);
    }
    resRegister_.Reset();
    assetManager_.Reset();
}

void AceContainer::DestroyView()
{
    ContainerScope scope(instanceId_);
    if (aceView_ != nullptr) {
        delete aceView_;
        aceView_ = nullptr;
    }
}

void AceContainer::InitializeFrontend()
{
    auto aceAbility = aceAbility_.lock();
    if (type_ == FrontendType::JS) {
        frontend_ = Frontend::Create();
        auto jsFrontend = AceType::DynamicCast<JsFrontend>(frontend_);
        auto& loader = Framework::JsEngineLoader::Get(GetEngineSharedLibrary(isArkApp_));
        auto jsEngine = loader.CreateJsEngine(instanceId_);
        jsEngine->AddExtraNativeObject("ability", aceAbility.get());
        EngineHelper::AddEngine(instanceId_, jsEngine);
        jsFrontend->SetJsEngine(jsEngine);
        jsFrontend->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
        jsFrontend->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
    } else if (type_ == FrontendType::JS_CARD) {
        AceApplicationInfo::GetInstance().SetCardType();
        frontend_ = AceType::MakeRefPtr<CardFrontend>();
    } else if (type_ == FrontendType::DECLARATIVE_JS) {
        if (!isSubContainer_) {
            frontend_ = AceType::MakeRefPtr<DeclarativeFrontend>();
            auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(frontend_);
            auto& loader = Framework::JsEngineLoader::GetDeclarative(GetDeclarativeSharedLibrary(isArkApp_));
            RefPtr<Framework::JsEngine> jsEngine;
            if (GetSettings().usingSharedRuntime) {
                jsEngine = loader.CreateJsEngineUsingSharedRuntime(instanceId_, sharedRuntime_);
                LOGI("Create engine using runtime, engine %{public}p", RawPtr(jsEngine));
            } else {
                jsEngine = loader.CreateJsEngine(instanceId_);
            }
            jsEngine->AddExtraNativeObject("ability", aceAbility.get());
            EngineHelper::AddEngine(instanceId_, jsEngine);
            declarativeFrontend->SetJsEngine(jsEngine);
            declarativeFrontend->SetPageProfile(pageProfile_);
            declarativeFrontend->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
            declarativeFrontend->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
        } else {
            frontend_ = OHOS::Ace::Platform::AceContainer::GetContainer(parentId_)->GetFrontend();
            return;
        }
    } else {
        LOGE("Frontend type not supported");
        EventReport::SendAppStartException(AppStartExcepType::FRONTEND_TYPE_ERR);
        return;
    }
    ACE_DCHECK(frontend_);
    auto abilityInfo = abilityInfo_.lock();
    std::shared_ptr<AppExecFwk::AbilityInfo> info = aceAbility ? aceAbility->GetAbilityInfo() : abilityInfo;
    if (info && info->isLauncherAbility) {
        frontend_->DisallowPopLastPage();
    }
    frontend_->Initialize(type_, taskExecutor_);
}

RefPtr<AceContainer> AceContainer::GetContainer(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (container != nullptr) {
        auto aceContainer = AceType::DynamicCast<AceContainer>(container);
        return aceContainer;
    } else {
        return nullptr;
    }
}

bool AceContainer::OnBackPressed(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return false;
    }

    // When the container is for menu, it need close the menu first.
    if (container->IsSubContainer()) {
        SubwindowManager::GetInstance()->CloseMenu();
        return true;
    }
    ContainerScope scope(instanceId);
    auto context = container->GetPipelineContext();
    if (!context) {
        return false;
    }
    return context->CallRouterBackToPopPage();
}

void AceContainer::OnShow(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();

    // When it is subContainer, no need call the OnShow, because the frontend is the same the parent container.
    if (front && !container->IsSubContainer()) {
        front->UpdateState(Frontend::State::ON_SHOW);
        front->OnShow();
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return;
    }
    context->OnShow();
}

void AceContainer::OnHide(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();

    // When it is subContainer, no need call the OnHide, because the frontend is the same the parent container.
    if (front && !container->IsSubContainer()) {
        front->UpdateState(Frontend::State::ON_HIDE);
        front->OnHide();
        auto taskExecutor = container->GetTaskExecutor();
        if (taskExecutor) {
            taskExecutor->PostTask([front]() { front->TriggerGarbageCollection(); }, TaskExecutor::TaskType::JS);
        }
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return;
    }
    context->OnHide();
}

void AceContainer::OnActive(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("container is null, OnActive failed.");
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();

    // When it is subContainer, no need call the OnActive, because the frontend is the same the parent container.
    if (front && !container->IsSubContainer()) {
        front->OnActive();
    }
}

void AceContainer::OnInactive(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();

    // When it is subContainer, no need call the OnInactive, because the frontend is the same the parent container.
    if (front && !container->IsSubContainer()) {
        front->OnInactive();
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return;
    }
    context->GetTaskExecutor()->PostTask([context]() { context->RootLostFocus(); }, TaskExecutor::TaskType::UI);
}

bool AceContainer::OnStartContinuation(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnStartContinuation failed.");
        return false;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnStartContinuation failed.");
        return false;
    }
    return front->OnStartContinuation();
}

std::string AceContainer::OnSaveData(int32_t instanceId)
{
    std::string result = "false";
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnSaveData failed.");
        return result;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnSaveData failed.");
        return result;
    }
    front->OnSaveData(result);
    return result;
}

bool AceContainer::OnRestoreData(int32_t instanceId, const std::string& data)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnRestoreData failed.");
        return false;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnRestoreData failed.");
        return false;
    }
    return front->OnRestoreData(data);
}

void AceContainer::OnCompleteContinuation(int32_t instanceId, int result)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnCompleteContinuation failed.");
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnCompleteContinuation failed.");
        return;
    }
    front->OnCompleteContinuation(result);
}

void AceContainer::OnRemoteTerminated(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnRemoteTerminated failed.");
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnRemoteTerminated failed.");
        return;
    }
    front->OnRemoteTerminated();
}

void AceContainer::OnConfigurationUpdated(int32_t instanceId, const std::string& configuration)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGI("container is null, OnConfigurationUpdated failed.");
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (!front) {
        LOGI("front is null, OnConfigurationUpdated failed.");
        return;
    }
    front->OnConfigurationUpdated(configuration);
}

void AceContainer::OnNewRequest(int32_t instanceId, const std::string& data)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }

    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (front) {
        front->OnNewRequest(data);
    }
}

void AceContainer::InitializeCallback()
{
    ACE_FUNCTION_TRACE();

    ACE_DCHECK(aceView_ && taskExecutor_ && pipelineContext_);
    auto&& touchEventCallback = [context = pipelineContext_, id = instanceId_](const TouchEvent& event) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event]() {
                context->OnTouchEvent(event);
                context->NotifyDispatchTouchEventDismiss(event);
            },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterTouchEventCallback(touchEventCallback);

    auto&& keyEventCallback = [context = pipelineContext_, id = instanceId_](const KeyEvent& event) {
        ContainerScope scope(id);
        bool result = false;
        context->GetTaskExecutor()->PostSyncTask(
            [context, event, &result]() { result = context->OnKeyEvent(event); }, TaskExecutor::TaskType::UI);
        return result;
    };
    aceView_->RegisterKeyEventCallback(keyEventCallback);

    auto&& mouseEventCallback = [context = pipelineContext_, id = instanceId_](const MouseEvent& event) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event]() { context->OnMouseEvent(event); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterMouseEventCallback(mouseEventCallback);

    auto&& axisEventCallback = [context = pipelineContext_, id = instanceId_](const AxisEvent& event) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event]() { context->OnAxisEvent(event); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterAxisEventCallback(axisEventCallback);

    auto&& rotationEventCallback = [context = pipelineContext_, id = instanceId_](const RotationEvent& event) {
        ContainerScope scope(id);
        bool result = false;
        context->GetTaskExecutor()->PostSyncTask(
            [context, event, &result]() { result = context->OnRotationEvent(event); }, TaskExecutor::TaskType::UI);
        return result;
    };
    aceView_->RegisterRotationEventCallback(rotationEventCallback);

    auto&& viewChangeCallback = [context = pipelineContext_, id = instanceId_](
                                    int32_t width, int32_t height, WindowSizeChangeReason type) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("ViewChangeCallback(%d, %d)", width, height);
        context->GetTaskExecutor()->PostTask(
            [context, width, height, type]() { context->OnSurfaceChanged(width, height, type); },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterViewChangeCallback(viewChangeCallback);

    auto&& densityChangeCallback = [context = pipelineContext_, id = instanceId_](double density) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("DensityChangeCallback(%lf)", density);
        context->GetTaskExecutor()->PostTask(
            [context, density]() { context->OnSurfaceDensityChanged(density); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterDensityChangeCallback(densityChangeCallback);

    auto&& systemBarHeightChangeCallback = [context = pipelineContext_, id = instanceId_](
                                               double statusBar, double navigationBar) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("SystemBarHeightChangeCallback(%lf, %lf)", statusBar, navigationBar);
        context->GetTaskExecutor()->PostTask(
            [context, statusBar, navigationBar]() { context->OnSystemBarHeightChanged(statusBar, navigationBar); },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterSystemBarHeightChangeCallback(systemBarHeightChangeCallback);

    auto&& surfaceDestroyCallback = [context = pipelineContext_, id = instanceId_]() {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context]() { context->OnSurfaceDestroyed(); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterSurfaceDestroyCallback(surfaceDestroyCallback);

    auto&& idleCallback = [context = pipelineContext_, id = instanceId_](int64_t deadline) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, deadline]() { context->OnIdle(deadline); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterIdleCallback(idleCallback);

    auto&& dragEventCallback = [context = pipelineContext_, id = instanceId_](
                                   int32_t x, int32_t y, const DragEventAction& action) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, x, y, action]() { context->OnDragEvent(x, y, action); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterDragEventCallback(dragEventCallback);

    InitWindowCallback();
}

void AceContainer::CreateContainer(int32_t instanceId, FrontendType type, bool isArkApp, std::string instanceName,
    std::shared_ptr<OHOS::AppExecFwk::Ability> aceAbility, std::unique_ptr<PlatformEventCallback> callback,
    bool useCurrentEventRunner)
{
    auto aceContainer = AceType::MakeRefPtr<AceContainer>(
        instanceId, type, isArkApp, aceAbility, std::move(callback), useCurrentEventRunner);
    AceEngine::Get().AddContainer(instanceId, aceContainer);

    HdcRegister::Get().StartHdcRegister();
    aceContainer->Initialize();
    ContainerScope scope(instanceId);
    auto front = aceContainer->GetFrontend();
    if (front) {
        front->UpdateState(Frontend::State::ON_CREATE);
        front->SetJsMessageDispatcher(aceContainer);
    }

    auto jsFront = AceType::DynamicCast<JsFrontend>(front);
    if (!jsFront) {
        return;
    }
    jsFront->SetInstanceName(instanceName.c_str());
}

void AceContainer::DestroyContainer(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        LOGE("no AceContainer with id %{private}d in AceEngine", instanceId);
        return;
    }
    container->Destroy();
    auto taskExecutor = container->GetTaskExecutor();
    if (taskExecutor) {
        taskExecutor->PostSyncTask([] { LOGI("Wait UI thread..."); }, TaskExecutor::TaskType::UI);
        taskExecutor->PostSyncTask([] { LOGI("Wait JS thread..."); }, TaskExecutor::TaskType::JS);
    }
    container->DestroyView(); // Stop all threads(ui,gpu,io) for current ability.
    if (taskExecutor) {
        taskExecutor->PostTask([instanceId] {
            LOGI("Remove on Platform thread...");
            EngineHelper::RemoveEngine(instanceId);
            AceEngine::Get().RemoveContainer(instanceId);
        }, TaskExecutor::TaskType::PLATFORM);
    }
}

void AceContainer::SetView(
    AceView* view, double density, int32_t width, int32_t height, int32_t windowId, UIEnvCallback callback)
{
    if (view == nullptr) {
        return;
    }

    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(view->GetInstanceId()));
    if (!container) {
        return;
    }
    auto platformWindow = PlatformWindow::Create(view);
    if (!platformWindow) {
        LOGE("Create PlatformWindow failed!");
        return;
    }
    std::unique_ptr<Window> window = std::make_unique<Window>(std::move(platformWindow));
    container->AttachView(std::move(window), view, density, width, height, windowId, callback);
}

void AceContainer::SetUIWindow(int32_t instanceId, sptr<OHOS::Rosen::Window> uiWindow)
{
    if (uiWindow == nullptr) {
        return;
    }

    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }
    container->SetUIWindowInner(uiWindow);
}

sptr<OHOS::Rosen::Window> AceContainer::GetUIWindow(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return nullptr;
    }
    return container->GetUIWindowInner();
}

OHOS::AppExecFwk::Ability* AceContainer::GetAbility(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return nullptr;
    }
    return container->GetAbilityInner().lock().get();
}

bool AceContainer::RunPage(int32_t instanceId, int32_t pageId, const std::string& content, const std::string& params)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return false;
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (front) {
        LOGI("RunPage content=[%{private}s]", content.c_str());
        front->RunPage(pageId, content, params);
        return true;
    }
    return false;
}

bool AceContainer::PushPage(int32_t instanceId, const std::string& content, const std::string& params)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return false;
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (front) {
        front->PushPage(content, params);
        return true;
    }
    return false;
}

bool AceContainer::UpdatePage(int32_t instanceId, int32_t pageId, const std::string& content)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return false;
    }
    ContainerScope scope(instanceId);
    auto context = container->GetPipelineContext();
    if (!context) {
        return false;
    }
    return context->CallRouterBackToPopPage();
}

void AceContainer::Dispatch(
    const std::string& group, std::vector<uint8_t>&& data, int32_t id, bool replyToComponent) const
{
    return;
}

void AceContainer::DispatchPluginError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    auto front = GetFrontend();
    if (!front) {
        LOGE("the front is nullptr");
        return;
    }

    ContainerScope scope(instanceId_);
    taskExecutor_->PostTask(
        [front, callbackId, errorCode, errorMessage = std::move(errorMessage)]() mutable {
            front->TransferJsPluginGetError(callbackId, errorCode, std::move(errorMessage));
        },
        TaskExecutor::TaskType::BACKGROUND);
}

bool AceContainer::Dump(const std::vector<std::string>& params)
{
    ContainerScope scope(instanceId_);
    if (aceView_ && aceView_->Dump(params)) {
        return true;
    }

    if (pipelineContext_) {
        pipelineContext_->Dump(params);
        return true;
    }
    return false;
}

void AceContainer::TriggerGarbageCollection()
{
    ContainerScope scope(instanceId_);
#if !defined(OHOS_PLATFORM) || !defined(ENABLE_NATIVE_VIEW)
    // GPU and IO thread is standalone while disable native view
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::GPU);
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::IO);
#endif
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::UI);
    taskExecutor_->PostTask(
        [frontend = WeakPtr<Frontend>(frontend_)] {
            auto sp = frontend.Upgrade();
            if (sp) {
                sp->TriggerGarbageCollection();
            }
            PurgeMallocCache();
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::SetLocalStorage(NativeReference* storage, NativeReference* context)
{
    ContainerScope scope(instanceId_);
    taskExecutor_->PostTask(
        [frontend = WeakPtr<Frontend>(frontend_), storage, context, id = instanceId_] {
            auto sp = frontend.Upgrade();
            if (sp) {
                auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(sp);
                auto jsEngine = declarativeFrontend->GetJsEngine();
                if (context) {
                    jsEngine->SetContext(id, context);
                }
                if (storage) {
                    jsEngine->SetLocalStorage(id, storage);
                }
            }
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::AddAssetPath(
    int32_t instanceId, const std::string& packagePath, const std::vector<std::string>& paths)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }

    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        if (container->type_ != FrontendType::DECLARATIVE_JS) {
            container->frontend_->SetAssetManager(flutterAssetManager);
        }
    }
    if (flutterAssetManager && !packagePath.empty()) {
        auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
        if (assetProvider->Initialize(packagePath, paths)) {
            LOGI("Push AssetProvider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }
}

void AceContainer::AddLibPath(int32_t instanceId, const std::string& libPath)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }

    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        if (container->type_ != FrontendType::DECLARATIVE_JS) {
            container->frontend_->SetAssetManager(flutterAssetManager);
        }
    }
    if (flutterAssetManager) {
        flutterAssetManager->SetLibPath(libPath);
    }
}

void AceContainer::AttachView(std::unique_ptr<Window> window, AceView* view, double density, int32_t width,
    int32_t height, int32_t windowId, UIEnvCallback callback)
{
    aceView_ = view;
    auto instanceId = aceView_->GetInstanceId();
    auto flutterTaskExecutor = AceType::DynamicCast<FlutterTaskExecutor>(taskExecutor_);
    if (!isSubContainer_) {
        auto state = flutter::UIDartState::Current()->GetStateById(instanceId);
        ACE_DCHECK(state != nullptr);
        flutterTaskExecutor->InitOtherThreads(state->GetTaskRunners());
        if (GetSettings().usePlatformAsUIThread) {
            ContainerScope::SetScopeNotify([](int32_t id) { flutter::UIDartState::Current()->SetCurInstance(id); });
        }
    }
    ContainerScope scope(instanceId);
    if (type_ == FrontendType::DECLARATIVE_JS) {
        // For DECLARATIVE_JS frontend display UI in JS thread temporarily.
        flutterTaskExecutor->InitJsThread(false);
        InitializeFrontend();
        auto front = GetFrontend();
        if (front) {
            front->UpdateState(Frontend::State::ON_CREATE);
            front->SetJsMessageDispatcher(AceType::Claim(this));
            front->SetAssetManager(assetManager_);
        }
    } else if (type_ != FrontendType::JS_CARD) {
        aceView_->SetCreateTime(createTime_);
    }
    resRegister_ = aceView_->GetPlatformResRegister();
    pipelineContext_ = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor_, assetManager_, resRegister_, frontend_, instanceId);
    pipelineContext_->SetRootSize(density, width, height);
    pipelineContext_->SetTextFieldManager(AceType::MakeRefPtr<TextFieldManager>());
    pipelineContext_->SetIsRightToLeft(AceApplicationInfo::GetInstance().IsRightToLeft());
    pipelineContext_->SetWindowModal(windowModal_);
    pipelineContext_->SetDrawDelegate(aceView_->GetDrawDelegate());
    pipelineContext_->SetWindowId(windowId);
    if (isSubContainer_) {
        pipelineContext_->SetIsSubPipeline(true);
    }
    InitializeCallback();

    auto&& finishEventHandler = [weak = WeakClaim(this), instanceId] {
        auto container = weak.Upgrade();
        if (!container) {
            LOGE("FinishEventHandler container is null!");
            return;
        }
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        if (!context) {
            LOGE("FinishEventHandler context is null!");
            return;
        }
        context->GetTaskExecutor()->PostTask(
            [weak = WeakPtr<AceContainer>(container)] {
                auto container = weak.Upgrade();
                if (!container) {
                    LOGE("Finish task, container is null!");
                    return;
                }
                container->OnFinish();
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetFinishEventHandler(finishEventHandler);

    auto&& startAbilityHandler = [weak = WeakClaim(this), instanceId](const std::string& address) {
        auto container = weak.Upgrade();
        if (!container) {
            LOGE("StartAbilityHandler container is null!");
            return;
        }
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        if (!context) {
            LOGE("StartAbilityHandler context is null!");
            return;
        }
        context->GetTaskExecutor()->PostTask(
            [weak = WeakPtr<AceContainer>(container), address]() {
                auto container = weak.Upgrade();
                if (!container) {
                    LOGE("Start ability task, container is null!");
                    return;
                }
                container->OnStartAbility(address);
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetStartAbilityHandler(startAbilityHandler);

    auto&& setStatusBarEventHandler = [weak = WeakClaim(this), instanceId](const Color& color) {
        auto container = weak.Upgrade();
        if (!container) {
            LOGE("StatusBarEventHandler container is null!");
            return;
        }
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        if (!context) {
            LOGE("StatusBarEventHandler context is null!");
            return;
        }
        context->GetTaskExecutor()->PostTask(
            [weak, color = color.GetValue()]() {
                auto container = weak.Upgrade();
                if (!container) {
                    LOGE("StatusBarEventHandler container is null!");
                    return;
                }
                if (container->platformEventCallback_) {
                    container->platformEventCallback_->OnStatusBarBgColorChanged(color);
                }
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetStatusBarEventHandler(setStatusBarEventHandler);

    taskExecutor_->PostTask([] { FrameReport::GetInstance().Init(); }, TaskExecutor::TaskType::UI);

    ThemeConstants::InitDeviceType();
    // Load custom style at UI thread before frontend attach, to make sure style can be loaded before building dom tree.
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    if (themeManager) {
        pipelineContext_->SetThemeManager(themeManager);
        // Init resource
        themeManager->InitResource(resourceInfo_);
        taskExecutor_->PostTask(
            [themeManager, assetManager = assetManager_, colorScheme = colorScheme_] {
                themeManager->SetColorScheme(colorScheme);
                themeManager->LoadCustomTheme(assetManager);
            },
            TaskExecutor::TaskType::UI);
    }
    taskExecutor_->PostTask(
        [context = pipelineContext_, callback, isSubContainer = isSubContainer_]() {
            if (callback != nullptr) {
                callback(context);
            }
            if (!isSubContainer) {
                context->SetupRootElement();
            }
        },
        TaskExecutor::TaskType::UI);
    aceView_->Launch();

    // Only MainWindow instance will be registered to watch dog.
    if (!isSubContainer_) {
        AceEngine::Get().RegisterToWatchDog(instanceId, taskExecutor_, GetSettings().useUIAsJSThread);
        frontend_->AttachPipelineContext(pipelineContext_);
    } else {
        auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(frontend_);
        if (declarativeFrontend) {
            declarativeFrontend->AttachSubPipelineContext(pipelineContext_);
        }
        return;
    }

    auto dataAbilityHelperImpl = [ability = GetAbilityInner(), runtimeContext = runtimeContext_,
                                     useStageModel = useStageModel_]() {
        return AceType::MakeRefPtr<DataAbilityHelperStandard>(ability.lock(), runtimeContext.lock(), useStageModel);
    };
    auto dataProviderManager = MakeRefPtr<DataProviderManagerStandard>(dataAbilityHelperImpl);
    pipelineContext_->SetDataProviderManager(dataProviderManager);
}

void AceContainer::SetUIWindowInner(sptr<OHOS::Rosen::Window> uiWindow)
{
    uiWindow_ = uiWindow;
}

sptr<OHOS::Rosen::Window> AceContainer::GetUIWindowInner() const
{
    return uiWindow_;
}

std::weak_ptr<OHOS::AppExecFwk::Ability> AceContainer::GetAbilityInner() const
{
    return aceAbility_;
}

void AceContainer::SetFontScale(int32_t instanceId, float fontScale)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }
    ContainerScope scope(instanceId);
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("fail to set font style due to context is null");
        return;
    }
    pipelineContext->SetFontScale(fontScale);
}

void AceContainer::SetWindowStyle(int32_t instanceId, WindowModal windowModal, ColorScheme colorScheme)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    if (!container) {
        return;
    }
    ContainerScope scope(instanceId);
    container->SetWindowModal(windowModal);
    container->SetColorScheme(colorScheme);
}

void AceContainer::SetDialogCallback(int32_t instanceId, FrontendDialogCallback callback)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return;
    }
    auto front = container->GetFrontend();
    if (front && front->GetType() == FrontendType::JS) {
        front->SetDialogCallback(callback);
    }
}

std::string AceContainer::RestoreRouterStack(int32_t instanceId, const std::string& contentInfo)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return "";
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (front) {
        return front->RestoreRouterStack(contentInfo);
    } else {
        return "";
    }
}

std::string AceContainer::GetContentInfo(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    if (!container) {
        return "";
    }
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    if (front) {
        return front->GetContentInfo();
    } else {
        return "";
    }
}

void AceContainer::InitializeSubContainer(int32_t parentContainerId)
{
    auto taskExec = AceEngine::Get().GetContainer(parentContainerId)->GetTaskExecutor();
    taskExecutor_ = AceType::DynamicCast<FlutterTaskExecutor>(std::move(taskExec));
    GetSettings().useUIAsJSThread = true;
}

void AceContainer::InitWindowCallback()
{
    LOGI("AceContainer InitWindowCallback");
    auto aceAbility = aceAbility_.lock();
    if (aceAbility == nullptr) {
        LOGW("AceContainer::InitWindowCallback failed, aceAbility is null.");
        return;
    }
    if (pipelineContext_ == nullptr) {
        LOGE("AceContainer::InitWindowCallback failed, pipelineContext_ is null.");
        return;
    }
    auto& window = aceAbility->GetWindow();
    if (window == nullptr) {
        LOGE("AceContainer::InitWindowCallback failed, window is null.");
        return;
    }
    std::shared_ptr<AppExecFwk::AbilityInfo> info = aceAbility->GetAbilityInfo();
    if (info != nullptr) {
        pipelineContext_->SetAppLabelId(info->labelId);
        pipelineContext_->SetAppIconId(info->iconId);
    }
    pipelineContext_->SetWindowMinimizeCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Minimize()); });
    pipelineContext_->SetWindowMaximizeCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Maximize()); });

    pipelineContext_->SetWindowRecoverCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Recover()); });

    pipelineContext_->SetWindowCloseCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Close()); });

    pipelineContext_->SetWindowStartMoveCallBack([window]() { window->StartMove(); });

    pipelineContext_->SetWindowSplitCallBack([window]() -> bool {
        return (
            OHOS::Rosen::WMError::WM_OK == window->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    });

    pipelineContext_->SetWindowGetModeCallBack([window]() -> WindowMode { return (WindowMode)window->GetMode(); });
}
} // namespace OHOS::Ace::Platform
