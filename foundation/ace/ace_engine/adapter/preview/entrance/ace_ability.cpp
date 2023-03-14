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

#include "adapter/preview/entrance/ace_ability.h"

#include <thread>

#include "adapter/preview/entrance/ace_application_info.h"
#include "adapter/preview/entrance/ace_container.h"
#include "adapter/preview/inspector/inspector_client.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/js_frontend.h"
#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/src/ports/SkFontMgr_config_parser.h"

namespace OHOS::Ace::Platform {

std::atomic<bool> AceAbility::loopRunning_ = true;

namespace {

// JS frontend maintain the page ID self, so it's useless to pass page ID from platform
// layer, neither Android/OpenHarmony or Windows, we should delete here usage when Java delete it.
constexpr int32_t UNUSED_PAGE_ID = 1;

constexpr char ASSET_PATH_SHARE[] = "share";
#ifdef WINDOWS_PLATFORM
constexpr char DELIMITER[] = "\\";
constexpr char ASSET_PATH_SHARE_STAGE[] = "resources\\base\\profile";
#else
constexpr char DELIMITER[] = "/";
constexpr char ASSET_PATH_SHARE_STAGE[] = "resources/base/profile";
#endif

#ifdef USE_GLFW_WINDOW
// Screen Density Coefficient/Base Density = Resolution/ppi
constexpr double SCREEN_DENSITY_COEFFICIENT_PHONE = 1080.0 * 160.0 / 480.0;
constexpr double SCREEN_DENSITY_COEFFICIENT_WATCH = 466.0 * 160 / 320.0;
constexpr double SCREEN_DENSITY_COEFFICIENT_TABLE = 2560.0 * 160.0 / 280.0;
constexpr double SCREEN_DENSITY_COEFFICIENT_TV = 3840.0 * 160.0 / 640.0;

void AdaptDeviceType(AceRunArgs& runArgs)
{
    if (runArgs.deviceConfig.deviceType == DeviceType::PHONE) {
        runArgs.deviceConfig.density = runArgs.deviceWidth / SCREEN_DENSITY_COEFFICIENT_PHONE;
    } else if (runArgs.deviceConfig.deviceType == DeviceType::WATCH) {
        runArgs.deviceConfig.density = runArgs.deviceWidth / SCREEN_DENSITY_COEFFICIENT_WATCH;
    } else if (runArgs.deviceConfig.deviceType == DeviceType::TABLET) {
        runArgs.deviceConfig.density = runArgs.deviceWidth / SCREEN_DENSITY_COEFFICIENT_TABLE;
    } else if (runArgs.deviceConfig.deviceType == DeviceType::TV) {
        runArgs.deviceConfig.density = runArgs.deviceWidth / SCREEN_DENSITY_COEFFICIENT_TV;
    } else {
        LOGE("DeviceType not supported");
    }
}
#endif

void SetPhysicalDeviceFonts(bool& physicalDeviceFontsEnabled)
{
    if (physicalDeviceFontsEnabled) {
        LOGI("Use physical devices fonts.");
    } else {
        LOGI("Use fonts installed on pc.");
    }
    SkFontMgr::setPhysicalDeviceFonts(physicalDeviceFontsEnabled);
}

void SetFontBasePath(std::string& basePath)
{
    LOGI("Set basic path of physical device font");
    basePath = basePath.append(DELIMITER);
    SkFontMgr_Config_Parser::setFontBasePathCallback(basePath);
}

std::string GetCustomAssetPath(std::string assetPath)
{
    if (assetPath.empty()) {
        LOGE("AssetPath is null.");
        return std::string();
    }
    std::string customAssetPath;
    if (OHOS::Ace::Framework::EndWith(assetPath, DELIMITER)) {
        assetPath = assetPath.substr(0, assetPath.size() - 1);
    }
    customAssetPath = assetPath.substr(0, assetPath.find_last_of(DELIMITER) + 1);
    return customAssetPath;
}

} // namespace

AceAbility::AceAbility(const AceRunArgs& runArgs) : runArgs_(runArgs)
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        LOGI("Initialize for current process.");
        Container::UpdateCurrent(INSTANCE_ID_PLATFORM);
    });
    if (runArgs_.formsEnabled) {
        LOGI("CreateContainer with JS_CARD frontend");
        AceContainer::CreateContainer(ACE_INSTANCE_ID, FrontendType::JS_CARD, runArgs_);
    } else if (runArgs_.aceVersion == AceVersion::ACE_1_0) {
        LOGI("CreateContainer with JS frontend");
        AceContainer::CreateContainer(ACE_INSTANCE_ID, FrontendType::JS, runArgs_);
    } else if (runArgs_.aceVersion == AceVersion::ACE_2_0) {
        LOGI("CreateContainer with JSDECLARATIVE frontend");
        AceContainer::CreateContainer(ACE_INSTANCE_ID, FrontendType::DECLARATIVE_JS, runArgs_);
    } else {
        LOGE("UnKnown frontend type");
    }
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null, set configuration failed.");
        return;
    }
    container->InitDeviceInfo(ACE_INSTANCE_ID, runArgs);
    SetConfigChanges(runArgs.configChanges);
    auto resConfig = container->GetResourceConfiguration();
    resConfig.SetOrientation(SystemProperties::GetDevcieOrientation());
    resConfig.SetDensity(SystemProperties::GetResolution());
    resConfig.SetDeviceType(SystemProperties::GetDeviceType());
    container->SetResourceConfiguration(resConfig);
}

AceAbility::~AceAbility()
{
    if (controller_) {
        FlutterDesktopDestroyWindow(controller_);
    }
    FlutterDesktopTerminate();
}

std::unique_ptr<AceAbility> AceAbility::CreateInstance(AceRunArgs& runArgs)
{
    LOGI("Start create AceAbility instance");
    bool initSucceeded = FlutterDesktopInit();
    if (!initSucceeded) {
        LOGE("Could not create window; AceDesktopInit failed.");
        return nullptr;
    }

    AceApplicationInfo::GetInstance().SetLocale(runArgs.language, runArgs.region, runArgs.script, "");

    // To check if use physical device fonts. Use Pc font by default.
    SetPhysicalDeviceFonts(runArgs.physicalDeviceFontsEnabled);
    if (runArgs.physicalDeviceFontsEnabled) {
        SetFontBasePath(runArgs.fontBasePath);
    }

    // Initialize FlutterSvgPainter static member variable fontTypeChinese_ and fontTypeNormal_ here
    // to make sure the font base path is already passed to SkFontMgr_Config_Parser, or previewer will crash.
    std::string fontTypeChinesePath = runArgs.fontBasePath + "HwChinese-Medium.ttf";
    std::string fontTypeNormalPath = runArgs.fontBasePath + "DroidSans.ttf";
    FlutterSvgPainter::fontTypeChinese_ = SkTypeface::MakeFromFile(fontTypeChinesePath.c_str());
    FlutterSvgPainter::fontTypeNormal_ = SkTypeface::MakeFromFile(fontTypeNormalPath.c_str());

    auto controller = FlutterDesktopCreateWindow(
        runArgs.deviceWidth, runArgs.deviceHeight, runArgs.windowTitle.c_str(), runArgs.onRender);
    auto aceAbility = std::make_unique<AceAbility>(runArgs);
    aceAbility->SetGlfwWindowController(controller);
    return aceAbility;
}

void AceAbility::InitEnv()
{
    std::vector<std::string> paths;
    paths.push_back(runArgs_.assetPath);
    std::string appResourcesPath(runArgs_.appResourcesPath);
    if (!OHOS::Ace::Framework::EndWith(appResourcesPath, DELIMITER)) {
        appResourcesPath.append(DELIMITER);
    }
    if (runArgs_.projectModel == ProjectModel::STAGE) {
        paths.push_back(appResourcesPath + ASSET_PATH_SHARE_STAGE);
    } else {
        paths.push_back(GetCustomAssetPath(runArgs_.assetPath) + ASSET_PATH_SHARE);
    }
    AceContainer::AddAssetPath(ACE_INSTANCE_ID, "", paths);

    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null, initialize the environment failed.");
        return;
    }
    if (runArgs_.projectModel == ProjectModel::STAGE) {
        if (runArgs_.formsEnabled) {
            container->SetStageCardConfig(runArgs_.pageProfile, runArgs_.url);
        } else {
            container->SetPageProfile((runArgs_.pageProfile.empty() ? "" : runArgs_.pageProfile + ".json"));
        }
    }
    AceContainer::SetResourcesPathAndThemeStyle(ACE_INSTANCE_ID, runArgs_.systemResourcesPath,
        runArgs_.appResourcesPath, runArgs_.themeId, runArgs_.deviceConfig.colorMode);

    auto view = new FlutterAceView(ACE_INSTANCE_ID);
    if (runArgs_.aceVersion == AceVersion::ACE_2_0) {
        AceContainer::SetView(view, runArgs_.deviceConfig.density, runArgs_.deviceWidth, runArgs_.deviceHeight);
        AceContainer::RunPage(ACE_INSTANCE_ID, UNUSED_PAGE_ID, runArgs_.url, "");
    } else {
        AceContainer::RunPage(ACE_INSTANCE_ID, UNUSED_PAGE_ID, runArgs_.url, "");
        AceContainer::SetView(view, runArgs_.deviceConfig.density, runArgs_.deviceWidth, runArgs_.deviceHeight);
    }

    AceContainer::AddRouterChangeCallback(ACE_INSTANCE_ID, runArgs_.onRouterChange);
    IdleCallback idleNoticeCallback = [view](int64_t deadline) { view->ProcessIdleEvent(deadline); };
    FlutterDesktopSetIdleCallback(controller_, idleNoticeCallback);
    OHOS::Ace::Framework::InspectorClient::GetInstance().RegisterFastPreviewErrorCallback(runArgs_.onError);

    // Should make it possible to update surface changes by using viewWidth and viewHeight.
    view->NotifySurfaceChanged(runArgs_.deviceWidth, runArgs_.deviceHeight);
    view->NotifyDensityChanged(runArgs_.deviceConfig.density);
}

void AceAbility::Start()
{
    RunEventLoop();
}

void AceAbility::Stop()
{
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        return;
    }

    container->GetTaskExecutor()->PostTask([]() { loopRunning_ = false; }, TaskExecutor::TaskType::PLATFORM);
}

void AceAbility::RunEventLoop()
{
    while (!FlutterDesktopWindowShouldClose(controller_) && loopRunning_) {
        FlutterDesktopWaitForEvents(controller_);
#ifdef USE_GLFW_WINDOW
        auto window = FlutterDesktopGetWindow(controller_);
        int width;
        int height;
        FlutterDesktopGetWindowSize(window, &width, &height);
        if (width != runArgs_.deviceWidth || height != runArgs_.deviceHeight) {
            AdaptDeviceType(runArgs_);
            SurfaceChanged(runArgs_.deviceConfig.orientation, runArgs_.deviceConfig.density, width, height);
        }
#endif
        auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
        if (container) {
            container->RunNativeEngineLoop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    loopRunning_ = true;

    // Currently exit loop is only to restart the AceContainer for real-time preview case.
    // Previewer background thread will release the AceAbility instance and create new one,
    // then call the InitEnv() and Start() again.
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null");
        FlutterDesktopDestroyWindow(controller_);
        controller_ = nullptr;
        return;
    }
    auto viewPtr = container->GetAceView();
    AceContainer::DestroyContainer(ACE_INSTANCE_ID);

    FlutterDesktopDestroyWindow(controller_);
    if (viewPtr != nullptr) {
        delete viewPtr;
        viewPtr = nullptr;
    }
    controller_ = nullptr;
}

bool AceAbility::DispatchTouchEvent(const TouchEvent& event)
{
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null");
        return false;
    }

    auto aceView = container->GetAceView();
    if (!aceView) {
        LOGE("aceView is null");
        return false;
    }

    std::promise<bool> touchPromise;
    std::future<bool> touchFuture = touchPromise.get_future();
    container->GetTaskExecutor()->PostTask([aceView, event, &touchPromise]() {
        bool isHandled = aceView->HandleTouchEvent(event);
        touchPromise.set_value(isHandled);
    }, TaskExecutor::TaskType::PLATFORM);
    return touchFuture.get();
}

bool AceAbility::DispatchBackPressedEvent()
{
    LOGI("DispatchBackPressedEvent start ");
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        return false;
    }

    auto context = container->GetPipelineContext();
    if (!context) {
        return false;
    }

    std::promise<bool> backPromise;
    std::future<bool> backFuture = backPromise.get_future();
    auto weak = AceType::WeakClaim(AceType::RawPtr(context));
    container->GetTaskExecutor()->PostTask([weak, &backPromise]() {
        auto context = weak.Upgrade();
        if (context == nullptr) {
            LOGW("context is nullptr.");
            return;
        }
        bool canBack = false;
        if (context->IsLastPage()) {
            LOGW("Can't back because this is the last page!");
        } else {
            canBack = context->CallRouterBackToPopPage();
        }
        backPromise.set_value(canBack);
    }, TaskExecutor::TaskType::PLATFORM);
    return backFuture.get();
}

void AceAbility::SetConfigChanges(const std::string& configChanges)
{
    if (configChanges == "") {
        return;
    }
    std::vector<std::string> configChangesSpliter;
    OHOS::Ace::StringUtils::StringSpliter(configChanges, ',', configChangesSpliter);
    for (const auto& singleConfig : configChangesSpliter) {
        if (singleConfig == "locale") {
            configChanges_.watchLocale = true;
            continue;
        } else if (singleConfig == "layout") {
            configChanges_.watchLayout = true;
            continue;
        } else if (singleConfig == "fontSize") {
            configChanges_.watchFontSize = true;
            continue;
        } else if (singleConfig == "orientation") {
            configChanges_.watchOrientation = true;
            continue;
        } else if (singleConfig == "density") {
            configChanges_.watchDensity = true;
            continue;
        } else {
            LOGW("unsupported config %{public}s", singleConfig.c_str());
        }
    }
}

void AceAbility::OnConfigurationChanged(const DeviceConfig& newConfig)
{
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null, change configuration failed.");
        return;
    }
    if (newConfig.colorMode != runArgs_.deviceConfig.colorMode) {
        container->UpdateDeviceConfig(newConfig);
        runArgs_.deviceConfig.colorMode = newConfig.colorMode;

        auto type = container->GetType();
        if (type == FrontendType::DECLARATIVE_JS) {
            container->NativeOnConfigurationUpdated(ACE_INSTANCE_ID);
            return;
        }
    }
}

void AceAbility::SurfaceChanged(
    const DeviceOrientation& orientation, const double& resolution, int32_t& width, int32_t& height)
{
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null, SurfaceChanged failed.");
        return;
    }
    auto resConfig = container->GetResourceConfiguration();
    resConfig.SetOrientation(SystemProperties::GetDevcieOrientation());
    resConfig.SetDensity(SystemProperties::GetResolution());
    resConfig.SetDeviceType(SystemProperties::GetDeviceType());
    container->SetResourceConfiguration(resConfig);

    auto viewPtr = container->GetAceView();
    if (viewPtr == nullptr) {
        LOGE("aceView is null, SurfaceChanged failed.");
        return;
    }
    auto window = FlutterDesktopGetWindow(controller_);
    // Need to change the window resolution and then change the rendering resolution. Otherwise, the image may not adapt
    // to the new window after the window is modified.
    auto context = container->GetPipelineContext();
    if (context == nullptr) {
        LOGE("context is null, SurfaceChanged failed.");
        return;
    }
    context->GetTaskExecutor()->PostSyncTask(
        [window, &width, &height]() { FlutterDesktopSetWindowSize(window, width, height); },
        TaskExecutor::TaskType::PLATFORM);
    SystemProperties::InitDeviceInfo(
        width, height, orientation == DeviceOrientation::PORTRAIT ? 0 : 1, resolution, runArgs_.isRound);
    viewPtr->NotifyDensityChanged(resolution);
    viewPtr->NotifySurfaceChanged(width, height);
    if ((orientation != runArgs_.deviceConfig.orientation && configChanges_.watchOrientation) ||
        (resolution != runArgs_.deviceConfig.density && configChanges_.watchDensity) ||
        ((width != runArgs_.deviceWidth || height != runArgs_.deviceHeight) && configChanges_.watchLayout)) {
        container->NativeOnConfigurationUpdated(ACE_INSTANCE_ID);
    }
    runArgs_.deviceConfig.orientation = orientation;
    runArgs_.deviceConfig.density = resolution;
    runArgs_.deviceWidth = width;
    runArgs_.deviceHeight = height;
}

void AceAbility::ReplacePage(const std::string& url, const std::string& params)
{
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null");
        return;
    }
    container->GetFrontend()->ReplacePage(url, params);
}

void AceAbility::LoadDocument(const std::string& url, const std::string& componentName, SystemParams& systemParams)
{
    AceApplicationInfo::GetInstance().ChangeLocale(systemParams.language, systemParams.region);
    runArgs_.isRound = systemParams.isRound;
    SurfaceChanged(systemParams.orientation, systemParams.density, systemParams.deviceWidth, systemParams.deviceHeight);
    DeviceConfig deviceConfig = {
        .orientation = systemParams.orientation,
        .density = systemParams.density,
        .deviceType = systemParams.deviceType,
        .colorMode = systemParams.colorMode,
    };
    OnConfigurationChanged(  deviceConfig);
    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    if (!container) {
        LOGE("container is null");
        return;
    }
    container->LoadDocument(url, componentName);
}

std::string AceAbility::GetJSONTree()
{
    std::string jsonTreeStr;
    OHOS::Ace::Framework::InspectorClient::GetInstance().AssembleJSONTreeStr(jsonTreeStr);
    return jsonTreeStr;
}

std::string AceAbility::GetDefaultJSONTree()
{
    std::string defaultJsonTreeStr;
    OHOS::Ace::Framework::InspectorClient::GetInstance().AssembleDefaultJSONTreeStr(defaultJsonTreeStr);
    return defaultJsonTreeStr;
}

bool AceAbility::OperateComponent(const std::string& attrsJson)
{
    // fastPreview not support databind
    if (attrsJson.find("this.") != std::string::npos || attrsJson.find("$r") != std::string::npos) {
        LOGE("fastPreview is not support databind,the attrsJson contains 'this.' or '$r'");
        return false;
    }

    auto root = JsonUtil::ParseJsonString(attrsJson);
    if (!root || !root->IsValid()) {
        LOGE("the attrsJson is illegal json format");
        return false;
    }

    auto container = AceContainer::GetContainerInstance(ACE_INSTANCE_ID);
    auto taskExecutor = container->GetTaskExecutor();
    taskExecutor->PostTask(
        [attrsJson] {
          bool result = OHOS::Ace::Framework::InspectorClient::GetInstance().OperateComponent(attrsJson);
          if (!result) {
              OHOS::Ace::Framework::InspectorClient::GetInstance().CallFastPreviewErrorCallback(attrsJson);
          }
        },
        TaskExecutor::TaskType::UI);
    return true;
}

} // namespace OHOS::Ace::Platform
