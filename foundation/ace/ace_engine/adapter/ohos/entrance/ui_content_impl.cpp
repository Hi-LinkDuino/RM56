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

#include "adapter/ohos/entrance/ui_content_impl.h"

#include <atomic>
#include <cinttypes>

#include "ability_context.h"
#include "ability_info.h"
#include "configuration.h"
#include "dm/display_manager.h"
#include "init_data.h"
#include "js_runtime_utils.h"
#include "native_reference.h"
#include "service_extension_context.h"

#ifdef ENABLE_ROSEN_BACKEND
#include "render_service_client/core/ui/rs_ui_director.h"
#endif

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/entrance/capability_registry.h"
#include "adapter/ohos/entrance/file_asset_provider.h"
#include "adapter/ohos/entrance/flutter_ace_view.h"
#include "adapter/ohos/entrance/plugin_utils_impl.h"
#include "base/geometry/rect.h"
#include "base/log/log.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/system_properties.h"
#include "core/common/ace_engine.h"
#include "core/common/container_scope.h"
#include "core/common/flutter/flutter_asset_manager.h"
#include "core/common/plugin_manager.h"

namespace OHOS::Ace {
namespace {

const std::string ABS_BUNDLE_CODE_PATH = "/data/app/el1/bundle/public/";
const std::string LOCAL_BUNDLE_CODE_PATH = "/data/storage/el1/bundle/";
const std::string FILE_SEPARATOR = "/";

} // namespace

static std::atomic<int32_t> gInstanceId = 0;
static std::atomic<int32_t> gSubInstanceId = 1000000;
const std::string SUBWINDOW_PREFIX = "ARK_APP_SUBWINDOW_";
const int32_t REQUEST_CODE = -1;

using ContentFinishCallback = std::function<void()>;
using ContentStartAbilityCallback = std::function<void(const std::string& address)>;
class ContentEventCallback final : public Platform::PlatformEventCallback {
public:
    explicit ContentEventCallback(ContentFinishCallback onFinish) : onFinish_(onFinish) {}
    ContentEventCallback(ContentFinishCallback onFinish, ContentStartAbilityCallback onStartAbility)
        : onFinish_(onFinish), onStartAbility_(onStartAbility) {}
    ~ContentEventCallback() = default;

    virtual void OnFinish() const
    {
        LOGI("UIContent OnFinish");
        if (onFinish_) {
            onFinish_();
        }
    }

    virtual void OnStartAbility(const std::string& address)
    {
        LOGI("UIContent OnStartAbility");
        if (onStartAbility_) {
            onStartAbility_(address);
        }
    }

    virtual void OnStatusBarBgColorChanged(uint32_t color)
    {
        LOGI("UIContent OnStatusBarBgColorChanged");
    }

private:
    ContentFinishCallback onFinish_;
    ContentStartAbilityCallback onStartAbility_;
};

extern "C" ACE_EXPORT void* OHOS_ACE_CreateUIContent(void* context, void* runtime)
{
    LOGI("Ace lib loaded, CreateUIContent.");
    return new UIContentImpl(reinterpret_cast<OHOS::AbilityRuntime::Context*>(context), runtime);
}

class OccupiedAreaChangeListener : public OHOS::Rosen::IOccupiedAreaChangeListener {
public:
    explicit OccupiedAreaChangeListener(int32_t instanceId) : instanceId_(instanceId) {}
    ~OccupiedAreaChangeListener() = default;

    void OnSizeChange(const sptr<OHOS::Rosen::OccupiedAreaChangeInfo>& info)
    {
        auto rect = info->rect_;
        auto type = info->type_;
        Rect keyboardRect = Rect(rect.posX_, rect.posY_, rect.width_, rect.height_);
        LOGI("UIContent::OccupiedAreaChange rect:%{public}s type: %{public}d", keyboardRect.ToString().c_str(), type);
        if (type == OHOS::Rosen::OccupiedAreaType::TYPE_INPUT) {
            auto container = Platform::AceContainer::GetContainer(instanceId_);
            if (!container) {
                LOGE("container may be destroyed.");
                return;
            }
            auto taskExecutor = container->GetTaskExecutor();
            if (!taskExecutor) {
                LOGE("OnSizeChange: taskExecutor is null.");
                return;
            }

            ContainerScope scope(instanceId_);
            taskExecutor->PostTask([container, keyboardRect] {
                auto context = container->GetPipelineContext();
                if (context != nullptr) {
                    context->OnVirtualKeyboardAreaChange(keyboardRect);
                }
            }, TaskExecutor::TaskType::UI);
        }
    }

private:
    int32_t instanceId_ = -1;
};

class DragWindowListener : public OHOS::Rosen::IWindowDragListener {
public:
    explicit DragWindowListener(int32_t instanceId) : instanceId_(instanceId) {}
    ~DragWindowListener() = default;
    void OnDrag(int32_t x, int32_t y, OHOS::Rosen::DragEvent event)
    {
        LOGI("DragWindowListener::OnDrag called.");
        auto flutterAceView =
            static_cast<Platform::FlutterAceView*>(Platform::AceContainer::GetContainer(instanceId_)->GetView());
        if (!flutterAceView) {
            LOGE("DragWindowListener::OnDrag flutterAceView is null");
            return;
        }

        DragEventAction action;
        switch (event) {
            case OHOS::Rosen::DragEvent::DRAG_EVENT_END:
                action = DragEventAction::DRAG_EVENT_END;
                break;
            case OHOS::Rosen::DragEvent::DRAG_EVENT_MOVE:
            case OHOS::Rosen::DragEvent::DRAG_EVENT_OUT:
                action = DragEventAction::DRAG_EVENT_MOVE;
                break;
            case OHOS::Rosen::DragEvent::DRAG_EVENT_IN:
            default:
                action = DragEventAction::DRAG_EVENT_START;
                break;
        }

        flutterAceView->ProcessDragEvent(x, y, action);
    }

private:
    int32_t instanceId_ = -1;
};

UIContentImpl::UIContentImpl(OHOS::AbilityRuntime::Context* context, void* runtime) : runtime_(runtime)
{
    if (context == nullptr) {
        LOGE("context is nullptr");
        return;
    }
    const auto& obj = context->GetBindingObject();
    auto ref = obj->Get<NativeReference>();
    auto object = AbilityRuntime::ConvertNativeValueTo<NativeObject>(ref->Get());
    auto weak = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(object->GetNativePointer());
    context_ = *weak;

    LOGI("Create UIContentImpl.");
}

void UIContentImpl::Initialize(OHOS::Rosen::Window* window, const std::string& url, NativeValue* storage)
{
    CommonInitialize(window, url, storage);
    LOGI("Initialize startUrl = %{public}s", startUrl_.c_str());
    // run page.
    Platform::AceContainer::RunPage(
        instanceId_, Platform::AceContainer::GetContainer(instanceId_)->GeneratePageId(), startUrl_, "");
    LOGI("Initialize UIContentImpl done.");
}

void UIContentImpl::Restore(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage)
{
    CommonInitialize(window, contentInfo, storage);
    startUrl_ = Platform::AceContainer::RestoreRouterStack(instanceId_, contentInfo);
    if (startUrl_.empty()) {
        LOGW("UIContent Restore start url is empty");
    }
    LOGI("Restore startUrl = %{public}s", startUrl_.c_str());
    Platform::AceContainer::RunPage(
        instanceId_, Platform::AceContainer::GetContainer(instanceId_)->GeneratePageId(), startUrl_, "");
    LOGI("Restore UIContentImpl done.");
}

std::string UIContentImpl::GetContentInfo() const
{
    LOGI("UIContent GetContentInfo");
    return Platform::AceContainer::GetContentInfo(instanceId_);
}

void UIContentImpl::CommonInitialize(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage)
{
    window_ = window;
    startUrl_ = contentInfo;
    if (!window_) {
        LOGE("Null window, can't initialize UI content");
        return;
    }
    if (StringUtils::StartWith(window->GetWindowName(), SUBWINDOW_PREFIX)) {
        InitializeSubWindow(window_);
        return;
    }
    auto context = context_.lock();
    if (!context) {
        LOGE("context is null");
        return;
    }
    LOGI("Initialize UIContentImpl start.");
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [&context]() {
        LOGI("Initialize for current process.");
        SetHwIcuDirectory();
        Container::UpdateCurrent(INSTANCE_ID_PLATFORM);
        AceApplicationInfo::GetInstance().SetProcessName(context->GetBundleName());
        AceApplicationInfo::GetInstance().SetPackageName(context->GetBundleName());
        AceApplicationInfo::GetInstance().SetDataFileDirPath(context->GetFilesDir());
        CapabilityRegistry::Register();
        ImageCache::SetImageCacheFilePath(context->GetCacheDir());
        ImageCache::SetCacheFileInfo();
    });

    int32_t deviceWidth = 0;
    int32_t deviceHeight = 0;
    float density = 1.0f;
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplay) {
        density = defaultDisplay->GetVirtualPixelRatio();
        deviceWidth = defaultDisplay->GetWidth();
        deviceHeight = defaultDisplay->GetHeight();
        LOGI("UIContent: deviceWidth: %{public}d, deviceHeight: %{public}d, default density: %{public}f", deviceWidth,
            deviceHeight, density);
    }
    SystemProperties::InitDeviceInfo(deviceWidth, deviceHeight, deviceHeight >= deviceWidth ? 0 : 1, density, false);
    SystemProperties::SetColorMode(ColorMode::LIGHT);

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    auto resourceManager = context->GetResourceManager();
    if (resourceManager != nullptr) {
        resourceManager->GetResConfig(*resConfig);
        auto localeInfo = resConfig->GetLocaleInfo();
        Platform::AceApplicationInfoImpl::GetInstance().SetResourceManager(resourceManager);
        if (localeInfo != nullptr) {
            auto language = localeInfo->getLanguage();
            auto region = localeInfo->getCountry();
            auto script = localeInfo->getScript();
            AceApplicationInfo::GetInstance().SetLocale((language == nullptr) ? "" : language,
                (region == nullptr) ? "" : region, (script == nullptr) ? "" : script, "");
        }
    }

    auto abilityContext = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(context);
    std::shared_ptr<OHOS::AppExecFwk::AbilityInfo> info;

    if (abilityContext) {
        info = abilityContext->GetAbilityInfo();
    } else {
        auto extensionContext =
            OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::ExtensionContext>(context);
        if (!extensionContext) {
            LOGE("context is not AbilityContext or ExtensionContext.");
            return;
        }
        info = extensionContext->GetAbilityInfo();
    }

    RefPtr<FlutterAssetManager> flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
    bool isModelJson = info != nullptr ? info->isModuleJson : false;
    std::string moduleName = info != nullptr ? info->moduleName : "";
    auto appInfo = context->GetApplicationInfo();
    auto bundleName = info != nullptr ? info->bundleName : "";
    std::string resPath;
    std::string pageProfile;
    LOGI("Initialize UIContent isModelJson:%{public}s", isModelJson ? "true" : "false");
    if (isModelJson) {
        if (appInfo) {
            std::vector<OHOS::AppExecFwk::ModuleInfo> moduleList = appInfo->moduleInfos;
            for (const auto& module : moduleList) {
                if (module.moduleName == moduleName) {
                    std::regex pattern(ABS_BUNDLE_CODE_PATH + bundleName + FILE_SEPARATOR);
                    auto moduleSourceDir = std::regex_replace(module.moduleSourceDir, pattern, LOCAL_BUNDLE_CODE_PATH);
                    resPath = moduleSourceDir + "/";
                    break;
                }
            }
        }
        LOGI("In stage mode, resPath:%{private}s", resPath.c_str());
        auto assetBasePathStr = { std::string("ets/"), std::string("resources/base/profile/") };
        if (flutterAssetManager && !resPath.empty()) {
            auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
            if (assetProvider->Initialize(resPath, assetBasePathStr)) {
                LOGI("Push AssetProvider to queue.");
                flutterAssetManager->PushBack(std::move(assetProvider));
            }
        }
        auto hapInfo = context->GetHapModuleInfo();
        if (hapInfo) {
            pageProfile = hapInfo->pages;
            const std::string profilePrefix = "@profile:";
            if (pageProfile.compare(0, profilePrefix.size(), profilePrefix) == 0) {
                pageProfile = pageProfile.substr(profilePrefix.length()).append(".json");
            }
            LOGI("In stage mode, pageProfile:%{public}s", pageProfile.c_str());
        } else {
            LOGE("In stage mode, can't get hap info.");
        }
    } else {
        auto packagePathStr = context->GetBundleCodeDir();
        auto moduleInfo = context->GetHapModuleInfo();
        if (moduleInfo != nullptr) {
            packagePathStr += "/" + moduleInfo->name + "/";
        }
        std::string srcPath = "";
        if (info != nullptr && !info->srcPath.empty()) {
            srcPath = info->srcPath;
        }

        auto assetBasePathStr = { "assets/js/" + (srcPath.empty() ? "default" : srcPath) + "/",
            std::string("assets/js/share/") };

        if (flutterAssetManager && !packagePathStr.empty()) {
            auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
            if (assetProvider->Initialize(packagePathStr, assetBasePathStr)) {
                LOGI("Push AssetProvider to queue.");
                flutterAssetManager->PushBack(std::move(assetProvider));
            }
        }

        if (appInfo) {
            std::vector<OHOS::AppExecFwk::ModuleInfo> moduleList = appInfo->moduleInfos;
            for (const auto& module : moduleList) {
                if (module.moduleName == moduleName) {
                    std::regex pattern(ABS_BUNDLE_CODE_PATH + bundleName + FILE_SEPARATOR);
                    auto moduleSourceDir = std::regex_replace(module.moduleSourceDir, pattern, LOCAL_BUNDLE_CODE_PATH);
                    resPath = moduleSourceDir + "/assets/" + module.moduleName + "/";
                    break;
                }
            }
        }
    }
    if (appInfo && flutterAssetManager) {
        std::string nativeLibraryPath = appInfo->nativeLibraryPath;
        if (!nativeLibraryPath.empty()) {
            if (nativeLibraryPath.back() == '/') {
                nativeLibraryPath.pop_back();
            }
            std::string libPath = context->GetBundleCodeDir();
            libPath += (libPath.back() == '/') ? nativeLibraryPath : "/" + nativeLibraryPath;
            LOGI("napi lib path = %{private}s", libPath.c_str());
            flutterAssetManager->SetLibPath(libPath);
        }
    }

    auto pluginUtils = std::make_shared<PluginUtilsImpl>();
    PluginManager::GetInstance().SetAceAbility(nullptr, pluginUtils);
    // create container
    instanceId_ = gInstanceId.fetch_add(1, std::memory_order_relaxed);
    auto container = AceType::MakeRefPtr<Platform::AceContainer>(instanceId_, FrontendType::DECLARATIVE_JS, true,
        context_, info, std::make_unique<ContentEventCallback>([context = context_] {
            auto sharedContext = context.lock();
            if (!sharedContext) {
                return;
            }
            auto abilityContext =
                OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(sharedContext);
            if (abilityContext) {
                abilityContext->CloseAbility();
            }
        }, [context = context_](const std::string& address) {
            auto sharedContext = context.lock();
            if (!sharedContext) {
                return;
            }
            auto abilityContext =
                OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(sharedContext);
            if (abilityContext) {
                LOGI("start ability with url = %{private}s", address.c_str());
                AAFwk::Want want;
                want.AddEntity(Want::ENTITY_BROWSER);
                want.SetParam("address", address);
                abilityContext->StartAbility(want, REQUEST_CODE);
            }
        }));
    if (!container) {
        LOGE("Create container is null.");
        return;
    }
    container->SetWindowName(window_->GetWindowName());

    // Mark the relationship between windowId and containerId, it is 1:1
    SubwindowManager::GetInstance()->AddContainerId(window->GetWindowId(), instanceId_);
    AceEngine::Get().AddContainer(instanceId_, container);
    container->GetSettings().SetUsingSharedRuntime(true);
    container->SetSharedRuntime(runtime_);
    container->SetPageProfile(pageProfile);
    container->Initialize();
    ContainerScope scope(instanceId_);
    auto front = container->GetFrontend();
    if (front) {
        front->UpdateState(Frontend::State::ON_CREATE);
        front->SetJsMessageDispatcher(container);
    }
    auto aceResCfg = container->GetResourceConfiguration();
    aceResCfg.SetOrientation(SystemProperties::GetDevcieOrientation());
    aceResCfg.SetDensity(SystemProperties::GetResolution());
    aceResCfg.SetDeviceType(SystemProperties::GetDeviceType());
    container->SetResourceConfiguration(aceResCfg);
    container->SetPackagePathStr(resPath);
    container->SetAssetManager(flutterAssetManager);
    container->SetBundlePath(context->GetBundleCodeDir());
    container->SetFilesDataPath(context->GetFilesDir());

    if (window_->IsDecorEnable() && SystemProperties::GetDeviceType() == DeviceType::TABLET) {
        LOGI("Container modal is enabled.");
        container->SetWindowModal(WindowModal::CONTAINER_MODAL);
    }

    // create ace_view
    auto flutterAceView =
        Platform::FlutterAceView::CreateView(instanceId_, false, container->GetSettings().usePlatformAsUIThread);
    Platform::FlutterAceView::SurfaceCreated(flutterAceView, window_);

    Ace::Platform::UIEnvCallback callback = nullptr;
#ifdef ENABLE_ROSEN_BACKEND
    callback = [window, id = instanceId_, container](const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context) {
        if (SystemProperties::GetRosenBackendEnabled()) {
            auto rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
            if (rsUiDirector != nullptr) {
                rsUiDirector->SetRSSurfaceNode(window->GetSurfaceNode());
                rsUiDirector->SetUITaskRunner(
                    [taskExecutor = container->GetTaskExecutor(), id](const std::function<void()>& task) {
                        ContainerScope scope(id);
                        taskExecutor->PostTask(task, TaskExecutor::TaskType::UI);
                    });
                auto context = container->GetPipelineContext();
                if (context != nullptr) {
                    context->SetRSUIDirector(rsUiDirector);
                }
                rsUiDirector->Init();
                LOGI("UIContent Init Rosen Backend");
            }
        }
    };
#endif
    // set view
    Platform::AceContainer::SetView(flutterAceView, density, 0, 0, window_->GetWindowId(), callback);
    Platform::FlutterAceView::SurfaceChanged(flutterAceView, 0, 0, deviceHeight >= deviceWidth ? 0 : 1);
    auto nativeEngine = reinterpret_cast<NativeEngine*>(runtime_);
    if (!storage) {
        container->SetLocalStorage(nullptr, context->GetBindingObject()->Get<NativeReference>());
    } else {
        LOGI("SetLocalStorage %{public}d", storage->TypeOf());
        container->SetLocalStorage(
            nativeEngine->CreateReference(storage, 1), context->GetBindingObject()->Get<NativeReference>());
    }
    InitWindowCallback(info);
}

void UIContentImpl::Foreground()
{
    LOGI("Show UIContent");
    Platform::AceContainer::OnShow(instanceId_);
}

void UIContentImpl::Background()
{
    LOGI("Hide UIContent");
    Platform::AceContainer::OnHide(instanceId_);
}

void UIContentImpl::Focus()
{
    LOGI("UIContent window focus");
    Platform::AceContainer::OnActive(instanceId_);
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("Window focus failed: container is null.");
        return;
    }
    auto taskExecutor = container->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("Window focus failed: taskExecutor is null.");
        return;
    }
    ContainerScope scope(instanceId_);
    taskExecutor->PostTask([container]() {
        auto pipelineContext = container->GetPipelineContext();
        if (!pipelineContext) {
            LOGE("Window focus failed, pipeline context is null.");
            return;
        }
        pipelineContext->WindowFocus(true);
    }, TaskExecutor::TaskType::UI);
}

void UIContentImpl::UnFocus()
{
    LOGI("UIContent window unFocus");
    Platform::AceContainer::OnInactive(instanceId_);
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("Window unFocus failed: container is null.");
        return;
    }
    auto taskExecutor = container->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("Window unFocus failed: taskExecutor is null.");
        return;
    }
    ContainerScope scope(instanceId_);
    taskExecutor->PostTask([container]() {
        auto pipelineContext = container->GetPipelineContext();
        if (!pipelineContext) {
            LOGE("Window unFocus failed, pipeline context is null.");
            return;
        }
        pipelineContext->WindowFocus(false);
    }, TaskExecutor::TaskType::UI);
}

void UIContentImpl::Destroy()
{
    LOGI("Destroy UIContent");
    Platform::AceContainer::DestroyContainer(instanceId_);
}

bool UIContentImpl::ProcessBackPressed()
{
    LOGI("UIContent ProcessBackPressed");
    return Platform::AceContainer::OnBackPressed(instanceId_);
}

bool UIContentImpl::ProcessPointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent)
{
    LOGI("UIContent ProcessPointerEvent");
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (container) {
        auto aceView = static_cast<Platform::FlutterAceView*>(container->GetAceView());
        Platform::FlutterAceView::DispatchTouchEvent(aceView, pointerEvent);
        return true;
    }
    return false;
}

bool UIContentImpl::ProcessKeyEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& touchEvent)
{
    LOGI("AceAbility::OnKeyUp called,touchEvent info: keyCode is %{private}d,"
         "keyAction is %{public}d, keyActionTime is %{public}" PRId64,
        touchEvent->GetKeyCode(), touchEvent->GetKeyAction(), touchEvent->GetActionTime());
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (container) {
        auto aceView = static_cast<Platform::FlutterAceView*>(container->GetAceView());
        return Platform::FlutterAceView::DispatchKeyEvent(aceView, touchEvent);
    }
    return false;
}

bool UIContentImpl::ProcessAxisEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent)
{
    LOGI("UIContent ProcessAxisEvent");
    return false;
}

bool UIContentImpl::ProcessVsyncEvent(uint64_t timeStampNanos)
{
    LOGI("UIContent ProcessVsyncEvent");
    return false;
}

void UIContentImpl::UpdateConfiguration(const std::shared_ptr<OHOS::AppExecFwk::Configuration>& config)
{
    if (!config) {
        LOGE("UIContent null config");
        return;
    }
    LOGI("UIContent UpdateConfiguration %{public}s", config->GetName().c_str());
}

void UIContentImpl::UpdateViewportConfig(const ViewportConfig& config, OHOS::Rosen::WindowSizeChangeReason reason)
{
    LOGI("UIContent UpdateViewportConfig %{public}s", config.ToString().c_str());
    SystemProperties::SetResolution(config.Density());
    SystemProperties::SetDeviceOrientation(config.Height() >= config.Width() ? 0 : 1);
    SystemProperties::SetWindowPos(config.Left(), config.Top());
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("UpdateViewportConfig: container is null.");
        return;
    }
    auto taskExecutor = container->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("UpdateViewportConfig: taskExecutor is null.");
        return;
    }
    taskExecutor->PostTask([config, container, reason]() {
            auto aceView = static_cast<Platform::FlutterAceView*>(container->GetAceView());
            if (!aceView) {
                LOGE("UpdateViewportConfig: aceView is null.");
                return;
            }
            flutter::ViewportMetrics metrics;
            metrics.physical_width = config.Width();
            metrics.physical_height = config.Height();
            metrics.device_pixel_ratio = config.Density();
            Platform::FlutterAceView::SetViewportMetrics(aceView, metrics);
            Platform::FlutterAceView::SurfaceChanged(aceView, config.Width(), config.Height(), config.Orientation(),
                static_cast<WindowSizeChangeReason>(reason));
        }, TaskExecutor::TaskType::PLATFORM);
}

void UIContentImpl::UpdateWindowMode(OHOS::Rosen::WindowMode mode)
{
    LOGI("UpdateWindowMode, window mode is %{public}d", mode);
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("UpdateWindowMode failed, get container(id=%{public}d) failed", instanceId_);
        return;
    }
    auto taskExecutor = container->GetTaskExecutor();
    if (!taskExecutor) {
        LOGE("UpdateWindowMode failed: taskExecutor is null.");
        return;
    }
    ContainerScope scope(instanceId_);
    taskExecutor->PostTask([container, mode]() {
        auto pipelineContext = container->GetPipelineContext();
        if (!pipelineContext) {
            LOGE("UpdateWindowMode failed, pipeline context is null.");
            return;
        }
        if (mode == OHOS::Rosen::WindowMode::WINDOW_MODE_FULLSCREEN ||
            mode == OHOS::Rosen::WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            mode == OHOS::Rosen::WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            pipelineContext->ShowContainerTitle(false);
        } else {
            pipelineContext->ShowContainerTitle(true);
        }
    }, TaskExecutor::TaskType::UI);
}

void UIContentImpl::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("get container(id=%{public}d) failed", instanceId_);
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("get pipeline context failed");
        return;
    }
    pipelineContext->DumpInfo(params, info);
}

void UIContentImpl::InitWindowCallback(const std::shared_ptr<OHOS::AppExecFwk::AbilityInfo>& info)
{
    LOGI("UIContent InitWindowCallback");
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    if (!container) {
        LOGE("InitWindowCallback failed, container(id=%{public}d) is null.", instanceId_);
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("InitWindowCallback failed, pipelineContext is null.");
        return;
    }
    auto& window = window_;
    if (!window) {
        LOGE("InitWindowCallback failed, window is null.");
        return;
    }
    if (info != nullptr) {
        pipelineContext->SetAppLabelId(info->labelId);
        pipelineContext->SetAppIconId(info->iconId);
    }

    pipelineContext->SetWindowMinimizeCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Minimize()); });

    pipelineContext->SetWindowMaximizeCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Maximize()); });

    pipelineContext->SetWindowRecoverCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Recover()); });

    pipelineContext->SetWindowCloseCallBack(
        [window]() -> bool { return (OHOS::Rosen::WMError::WM_OK == window->Close()); });

    pipelineContext->SetWindowStartMoveCallBack([window]() { window->StartMove(); });

    pipelineContext->SetWindowSplitCallBack([window]() -> bool {
        return (
            OHOS::Rosen::WMError::WM_OK == window->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    });

    pipelineContext->SetWindowGetModeCallBack([window]() -> WindowMode { return (WindowMode)window->GetMode(); });

    pipelineContext->SetGetWindowRectImpl([window]() -> Rect {
        Rect rect;
        if (!window) {
            return rect;
        }
        auto windowRect = window->GetRect();
        rect.SetRect(windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);
        return rect;
    });

    dragWindowListener_ = new DragWindowListener(instanceId_);
    window->RegisterDragListener(dragWindowListener_);
    occupiedAreaChangeListener_ = new OccupiedAreaChangeListener(instanceId_);
    window->RegisterOccupiedAreaChangeListener(occupiedAreaChangeListener_);
}

void UIContentImpl::InitializeSubWindow(OHOS::Rosen::Window* window)
{
    window_ = window;
    LOGI("The window name is %{public}s", window->GetWindowName().c_str());
    if (!window_) {
        LOGE("Null window, can't initialize UI content");
        return;
    }

    RefPtr<Platform::AceContainer> container;
    instanceId_ = gSubInstanceId.fetch_add(1, std::memory_order_relaxed);

    std::weak_ptr<OHOS::AppExecFwk::AbilityInfo> abilityInfo;
    std::weak_ptr<OHOS::AbilityRuntime::Context> runtimeContext;
    container = AceType::MakeRefPtr<Platform::AceContainer>(instanceId_, FrontendType::DECLARATIVE_JS, true,
        runtimeContext, abilityInfo, std::make_unique<ContentEventCallback>([] {
            // Sub-window ,just return.
            LOGI("Content event callback");
        }),
        false, true);
    SubwindowManager::GetInstance()->AddContainerId(window->GetWindowId(), instanceId_);
    AceEngine::Get().AddContainer(instanceId_, container);
}

} // namespace OHOS::Ace
