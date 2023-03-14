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

#include "ui_mgr_service.h"

#include <atomic>
#include <i_input_event_consumer.h>
#include <key_event.h>

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/entrance/flutter_ace_view.h"
#include "adapter/ohos/entrance/utils.h"
#include "core/components/theme/app_theme.h"

#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "init_data.h"
#include "ipc_skeleton.h"
#include "locale_config.h"
#ifdef ENABLE_ROSEN_BACKEND
#include "render_service_client/core/ui/rs_ui_director.h"
#endif
#include "res_config.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "ui_service_mgr_errors.h"
#include "wm/window.h"

namespace OHOS {
namespace Ace {
constexpr int UI_MGR_SERVICE_SA_ID = 7001;
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<UIMgrService>::GetInstance().get());

UIMgrService::UIMgrService()
    : SystemAbility(UI_MGR_SERVICE_SA_ID, true),
      eventLoop_(nullptr),
      handler_(nullptr),
      state_(UIServiceRunningState::STATE_NOT_START)
{
}

UIMgrService::~UIMgrService()
{
    callbackMap_.clear();
}

static std::atomic<int32_t> gDialogId = 0;

class UIMgrServiceWindowChangeListener : public Rosen::IWindowChangeListener {
public:
    void OnSizeChange(OHOS::Rosen::Rect rect, OHOS::Rosen::WindowSizeChangeReason reason) override
    {
        HILOG_INFO("UIMgrServiceWindowChangeListener size change");
    }
    void OnModeChange(OHOS::Rosen::WindowMode mode) override
    {
        HILOG_INFO("UIMgrServiceWindowChangeListener mode change");
    }
};

class UIMgrServiceInputEventConsumer : public MMI::IInputEventConsumer {
public:
    explicit UIMgrServiceInputEventConsumer(Ace::Platform::FlutterAceView* flutterAceView)
    {
        flutterAceView_ = flutterAceView;
    }
    ~UIMgrServiceInputEventConsumer() override = default;

    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
    {
        if (flutterAceView_ != nullptr) {
            flutterAceView_->DispatchTouchEvent(flutterAceView_, pointerEvent);
        }
    }
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override {}
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override {}

private:
    Ace::Platform::FlutterAceView* flutterAceView_ = nullptr;
};

using AcePlatformFinish = std::function<void()>;
class AcePlatformEventCallback final : public Ace::Platform::PlatformEventCallback {
public:
    explicit AcePlatformEventCallback(AcePlatformFinish onFinish) : onFinish_(onFinish) {}

    ~AcePlatformEventCallback() = default;

    virtual void OnFinish() const
    {
        LOGI("AcePlatformEventCallback OnFinish");
        if (onFinish_) {
            onFinish_();
        }
    }

    virtual void OnStatusBarBgColorChanged(uint32_t color)
    {
        LOGI("AcePlatformEventCallback OnStatusBarBgColorChanged");
    }

private:
    AcePlatformFinish onFinish_;
};

void UIMgrService::InitResourceManager()
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        HILOG_ERROR("InitResourceManager create resourceManager failed");
        return;
    }

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(Global::I18n::LocaleConfig::GetSystemLanguage(), status);
    resConfig->SetLocaleInfo(locale);
    if (resConfig->GetLocaleInfo() != nullptr) {
        HILOG_DEBUG("InitResourceManager language: %{public}s, script: %{public}s, region: %{public}s,",
            resConfig->GetLocaleInfo()->getLanguage(),
            resConfig->GetLocaleInfo()->getScript(),
            resConfig->GetLocaleInfo()->getCountry());
    } else {
        HILOG_ERROR("InitResourceManager language: GetLocaleInfo is null.");
    }
    resourceManager->UpdateResConfig(*resConfig);
    resourceManager_ = resourceManager;
}

std::shared_ptr<OHOS::AppExecFwk::Ability> UIMgrService::CreateAbility()
{
    auto ability = OHOS::AppExecFwk::Ability::Create(nullptr);
    if (ability == nullptr) {
        HILOG_ERROR("create ability failed");
        return nullptr;
    }

    auto deal = std::make_shared<OHOS::AppExecFwk::ContextDeal>();
    if (deal == nullptr) {
        HILOG_ERROR("create deal failed");
        delete ability;
        return nullptr;
    }

    deal->initResourceManager(resourceManager_);
    ability->AttachBaseContext(deal);
    std::shared_ptr<OHOS::AppExecFwk::Ability> sharedAbility(ability);
    return sharedAbility;
}

static void SetDialogBackgroundColor(const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context)
{
    auto themeManager = context->GetThemeManager();
    if (themeManager) {
        auto appTheme = themeManager->GetTheme<AppTheme>();
        if (appTheme) {
            HILOG_INFO("set bg color TRANSPARENT");
            appTheme->SetBackgroundColor(Color::TRANSPARENT);
        }
    }
}

int UIMgrService::ShowDialog(const std::string& name,
                             const std::string& params,
                             OHOS::Rosen::WindowType windowType,
                             int x,
                             int y,
                             int width,
                             int height,
                             const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
                             int* id)
{
    HILOG_INFO("Show dialog in service start");
    if (handler_ == nullptr) {
        HILOG_ERROR("Show dialog failed! handler is nullptr");
        return UI_SERVICE_HANDLER_IS_NULL;
    }

    int32_t dialogId = gDialogId.fetch_add(1, std::memory_order_relaxed);
    if (id != nullptr) {
        *id = dialogId;
    }
    HILOG_INFO("Show dialog id: %{public}d", dialogId);
    sptr<OHOS::Rosen::Window> dialogWindow = nullptr;
    auto showDialogCallback = [&]() {
        SetHwIcuDirectory();
        InitResourceManager();

        std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
        if (resourceManager_ != nullptr) {
            resourceManager_->GetResConfig(*resConfig);
            auto localeInfo = resConfig->GetLocaleInfo();
            Ace::Platform::AceApplicationInfoImpl::GetInstance().SetResourceManager(resourceManager_);
            if (localeInfo != nullptr) {
                auto language = localeInfo->getLanguage();
                auto region = localeInfo->getCountry();
                auto script = localeInfo->getScript();
                Ace::AceApplicationInfo::GetInstance().SetLocale((language == nullptr) ? "" : language,
                    (region == nullptr) ? "" : region, (script == nullptr) ? "" : script, "");
            }
        }

        std::string resPath;
        // create container
        auto ability = CreateAbility();
        Ace::Platform::AceContainer::CreateContainer(dialogId, Ace::FrontendType::JS, false, "", ability,
            std::make_unique<AcePlatformEventCallback>([]() {}), true);
        abilityMaps_[dialogId] = ability;
        auto container = Ace::Platform::AceContainer::GetContainer(dialogId);
        if (!container) {
            HILOG_ERROR("container is null, set configuration failed.");
        } else {
            auto aceResCfg = container->GetResourceConfiguration();
            aceResCfg.SetOrientation(Ace::SystemProperties::GetDevcieOrientation());
            aceResCfg.SetDensity(Ace::SystemProperties::GetResolution());
            aceResCfg.SetDeviceType(Ace::SystemProperties::GetDeviceType());
            container->SetResourceConfiguration(aceResCfg);
            container->SetPackagePathStr(resPath);
        }

        Ace::Platform::AceContainer::SetDialogCallback(dialogId,
            [callback = dialogCallback, id = dialogId](const std::string& event, const std::string& params) {
                HILOG_INFO("Dialog callback from service");
                callback->OnDialogCallback(id, event, params);
            });

        // create view.
        auto flutterAceView = Ace::Platform::FlutterAceView::CreateView(dialogId, true);

        sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
        option->SetWindowRect({ x, y, width, height });
        option->SetWindowType(windowType);
        std::string windowName = "system_dialog_window";
        windowName += std::to_string(dialogId);
        dialogWindow = OHOS::Rosen::Window::Create(windowName, option);
        if (dialogWindow == nullptr) {
            HILOG_ERROR("Create window failed");
            return;
        }

        // register surface change callback
        OHOS::sptr<OHOS::Rosen::IWindowChangeListener> listener = new UIMgrServiceWindowChangeListener();
        dialogWindow->RegisterWindowChangeListener(listener);

        std::shared_ptr<MMI::IInputEventConsumer> inputEventListener =
            std::make_shared<UIMgrServiceInputEventConsumer>(flutterAceView);
        dialogWindow->AddInputEventListener(inputEventListener);

        Ace::Platform::FlutterAceView::SurfaceCreated(flutterAceView, dialogWindow);

        // set metrics
        int32_t windowWidth = dialogWindow->GetRect().width_;
        int32_t windowHeight = dialogWindow->GetRect().height_;
        HILOG_INFO("Show dialog: windowConfig: width: %{public}d, height: %{public}d", windowWidth, windowHeight);

        flutter::ViewportMetrics metrics;
        metrics.physical_width = windowWidth;
        metrics.physical_height = windowHeight;
        metrics.device_pixel_ratio = density_;
        Ace::Platform::FlutterAceView::SetViewportMetrics(flutterAceView, metrics);

        std::string packagePathStr = "/system/etc/SADialog/";
        std::vector<std::string> assetBasePathStr = { name + "/" };
        Ace::Platform::AceContainer::AddAssetPath(dialogId, packagePathStr, assetBasePathStr);

        Ace::Platform::UIEnvCallback callback = nullptr;
        callback =
#ifdef ENABLE_ROSEN_BACKEND
            [dialogWindow, listener, dialogId] (const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context) mutable {
                if (SystemProperties::GetRosenBackendEnabled()) {
                    auto rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
                    if (rsUiDirector != nullptr) {
                        rsUiDirector->SetRSSurfaceNode(dialogWindow->GetSurfaceNode());
                        dialogWindow->RegisterWindowChangeListener(listener);

                        rsUiDirector->SetUITaskRunner(
                            [taskExecutor = Ace::Platform::AceContainer::GetContainer(dialogId)->GetTaskExecutor()]
                                (const std::function<void()>& task) {
                                    taskExecutor->PostTask(task, TaskExecutor::TaskType::UI);
                                });
                        if (context != nullptr) {
                            context->SetRSUIDirector(rsUiDirector);
                        }
                        rsUiDirector->Init();
                        HILOG_INFO("Init Rosen Backend");
                    }
                } else {
                    HILOG_INFO("not Init Rosen Backend");
                }
                SetDialogBackgroundColor(context);
            };
#else
            [] (const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context) {
                SetDialogBackgroundColor(context);
            }
#endif

        // set view
        Ace::Platform::AceContainer::SetView(
            flutterAceView, density_, windowWidth, windowHeight, dialogWindow->GetWindowId(), callback);
        Ace::Platform::AceContainer::SetUIWindow(dialogId, dialogWindow);
        Ace::Platform::FlutterAceView::SurfaceChanged(flutterAceView, windowWidth, windowHeight, 0);

        // run page.
        Ace::Platform::AceContainer::RunPage(
            dialogId, Ace::Platform::AceContainer::GetContainer(dialogId)->GeneratePageId(), "", params);
    };

    if (!handler_->PostSyncTask(showDialogCallback)) {
        HILOG_ERROR("Post sync task error");
        return UI_SERVICE_POST_TASK_FAILED;
    }

    if (dialogWindow == nullptr) {
        HILOG_ERROR("No window available");
        return UI_SERVICE_CREATE_WINDOW_FAILED;
    }

    int32_t windowWidth = static_cast<int32_t>(dialogWindow->GetRect().width_);
    int32_t windowHeight = static_cast<int32_t>(dialogWindow->GetRect().height_);
    int32_t windowX = static_cast<int32_t>(dialogWindow->GetRect().posX_);
    int32_t windowY = static_cast<int32_t>(dialogWindow->GetRect().posY_);
    HILOG_INFO("Show dialog: size: width: %{public}d, height: %{public}d, pos: x: %{public}d, y: %{public}d",
        windowWidth, windowHeight, windowX, windowY);
    dialogWindow->Show();
    dialogWindow->MoveTo(windowX, windowY);
    dialogWindow->Resize(windowWidth, windowHeight);

    HILOG_INFO("Show dialog in service end");
    return NO_ERROR;
}

int UIMgrService::CancelDialog(int id)
{
    auto cancelDialogCallback = [id, this]() {
        HILOG_INFO("Cancel dialog id: %{public}d", id);
        int ret = abilityMaps_.erase(id);
        if (ret == 0) {
            HILOG_ERROR("Cancel dialog failed: no such dialog(%{public}d)", id);
            return;
        }
        auto container = Platform::AceContainer::GetContainer(id);
        if (!container) {
            HILOG_INFO("Container(%{public}d) not found.", id);
            return;
        }
        auto dialogWindow = Platform::AceContainer::GetUIWindow(id);
        if (dialogWindow) {
            dialogWindow->Destroy();
        }
#ifdef ENABLE_ROSEN_BACKEND
        auto taskExecutor = Platform::AceContainer::GetContainer(id)->GetTaskExecutor();
        taskExecutor->PostTask(
            [id]() {
                if (auto context = Ace::Platform::AceContainer::GetContainer(id)->GetPipelineContext()) {
                    context->SetRSUIDirector(nullptr);
                }
            },
            TaskExecutor::TaskType::UI);
#endif
        Platform::AceContainer::DestroyContainer(id);
    };

    if (!handler_->PostTask(cancelDialogCallback)) {
        return UI_SERVICE_POST_TASK_FAILED;
    }

    return NO_ERROR;
}

void UIMgrService::OnStart()
{
    if (state_ == UIServiceRunningState::STATE_RUNNING) {
        HILOG_INFO("ui Manager Service has already started.");
        return;
    }
    HILOG_INFO("ui Manager Service started.");
    if (!Init()) {
        HILOG_ERROR("failed to init service.");
        return;
    }
    state_ = UIServiceRunningState::STATE_RUNNING;
    eventLoop_->Run();
    /* Publish service maybe failed, so we need call this function at the last,
     * so it can't affect the TDD test program */
    bool ret = Publish(DelayedSingleton<UIMgrService>::GetInstance().get());
    if (!ret) {
        HILOG_ERROR("UIMgrService::Init Publish failed!");
        return;
    }

    HILOG_INFO("UIMgrService  start success.");
}

bool UIMgrService::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create("UIMgrService");
    if (eventLoop_ == nullptr) {
        return false;
    }

    handler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (handler_ == nullptr) {
        return false;
    }

    HILOG_INFO("init success");
    return true;
}

void UIMgrService::OnStop()
{
    HILOG_INFO("stop service");
    eventLoop_.reset();
    handler_.reset();
    state_ = UIServiceRunningState::STATE_NOT_START;
}

UIServiceRunningState UIMgrService::QueryServiceState() const
{
    return state_;
}

int UIMgrService::RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService)
{
    HILOG_INFO("UIMgrService::RegisterCallBack called start");
    if (uiService == nullptr) {
        HILOG_ERROR("UIMgrService::RegisterCallBack failed!. uiService is nullptr");
        return UI_SERVICE_IS_NULL;
    }
    if (handler_ == nullptr) {
        HILOG_ERROR("UIMgrService::RegisterCallBack failed!. handler is nullptr");
        return UI_SERVICE_HANDLER_IS_NULL;
    }
    std::function<void()> registerFunc = std::bind(&UIMgrService::HandleRegister, shared_from_this(), want, uiService);
    bool ret = handler_->PostTask(registerFunc);
    if (!ret) {
        HILOG_ERROR("DataObsMgrService::RegisterCallBack PostTask error");
        return UI_SERVICE_POST_TASK_FAILED;
    }
    HILOG_INFO("UIMgrService::RegisterCallBack called end");
    return NO_ERROR;
}

int UIMgrService::UnregisterCallBack(const AAFwk::Want& want)
{
    HILOG_INFO("UIMgrService::UnregisterCallBack called start");
    if (handler_ == nullptr) {
        HILOG_ERROR("UIMgrService::UnregisterCallBack failed!. handler is nullptr");
        return UI_SERVICE_HANDLER_IS_NULL;
    }
    std::function<void()> unregisterFunc = std::bind(&UIMgrService::HandleUnregister, shared_from_this(), want);
    bool ret = handler_->PostTask(unregisterFunc);
    if (!ret) {
        HILOG_ERROR("DataObsMgrService::UnregisterCallBack PostTask error");
        return UI_SERVICE_POST_TASK_FAILED;
    }
    HILOG_INFO("UIMgrService::UnregisterCallBack called end");
    return NO_ERROR;
}

int UIMgrService::Push(const AAFwk::Want& want, const std::string& name,
    const std::string& jsonPath, const std::string& data, const std::string& extraData)
{
    HILOG_INFO("UIMgrService::Push called start");
    std::map<std::string, sptr<IUIService>>::iterator iter;
    for (iter = callbackMap_.begin(); iter != callbackMap_.end(); ++iter) {
        sptr<IUIService> uiService = iter->second;
        if (uiService == nullptr) {
            return UI_SERVICE_IS_NULL;
        }
        uiService->OnPushCallBack(want, name, jsonPath, data, extraData);
    }
    HILOG_INFO("UIMgrService::Push called end");
    return NO_ERROR;
}

int UIMgrService::Request(const AAFwk::Want& want, const std::string& name, const std::string& data)
{
    HILOG_INFO("UIMgrService::Request called start");
    std::map<std::string, sptr<IUIService>>::iterator iter;
    for (iter = callbackMap_.begin(); iter != callbackMap_.end(); ++iter) {
        sptr<IUIService> uiService = iter->second;
        if (uiService == nullptr) {
            return UI_SERVICE_IS_NULL;
        }
        uiService->OnRequestCallBack(want, name, data);
    }
    HILOG_INFO("UIMgrService::Request called end");
    return NO_ERROR;
}

int UIMgrService::ReturnRequest(
    const AAFwk::Want& want, const std::string& source, const std::string& data, const std::string& extraData)
{
    HILOG_INFO("UIMgrService::ReturnRequest called start");
    std::map<std::string, sptr<IUIService>>::iterator iter;
    for (iter = callbackMap_.begin(); iter != callbackMap_.end(); ++iter) {
        sptr<IUIService> uiService = iter->second;
        if (uiService == nullptr) {
            return UI_SERVICE_IS_NULL;
        }
        uiService->OnReturnRequest(want, source, data, extraData);
    }
    HILOG_INFO("UIMgrService::ReturnRequest called end");
    return NO_ERROR;
}
std::shared_ptr<EventHandler> UIMgrService::GetEventHandler()
{
    return handler_;
}

int UIMgrService::HandleRegister(const AAFwk::Want& want, const sptr<IUIService>& uiService)
{
    HILOG_INFO("UIMgrService::HandleRegister called start");
    std::lock_guard<std::mutex> lock_l(uiMutex_);
    std::string keyStr = GetCallBackKeyStr(want);
    HILOG_INFO("UIMgrService::HandleRegister keyStr = %{public}s", keyStr.c_str());
    bool exist = CheckCallBackFromMap(keyStr);
    if (exist) {
        callbackMap_.erase(keyStr);
    }
    callbackMap_.emplace(keyStr, uiService);
    HILOG_INFO("UIMgrService::HandleRegister called end callbackMap_.size() %{public}zu", callbackMap_.size());
    return NO_ERROR;
}

int UIMgrService::HandleUnregister(const AAFwk::Want& want)
{
    HILOG_INFO("UIMgrService::HandleUnregister called start");
    std::lock_guard<std::mutex> lock_l(uiMutex_);
    std::string keyStr = GetCallBackKeyStr(want);
    bool exist = CheckCallBackFromMap(keyStr);
    if (!exist) {
        HILOG_ERROR("UIMgrService::HandleUnregister there is no keyStr in map.");
        return NO_CALLBACK_FOR_KEY;
    }
    callbackMap_.erase(keyStr);
    HILOG_INFO("UIMgrService::HandleUnregister called end");
    return NO_ERROR;
}

std::string UIMgrService::GetCallBackKeyStr(const AAFwk::Want& want)
{
    HILOG_INFO("UIMgrService::GetCallBackKeyStr called start");
    AppExecFwk::ElementName element =  want.GetElement();
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();
    std::string keyStr = bundleName + abilityName;
    HILOG_INFO("UIMgrService::GetCallBackKeyStr called end");
    return keyStr;
}

bool UIMgrService::CheckCallBackFromMap(const std::string& key)
{
    HILOG_INFO("UIMgrService::CheckCallBackFromMap called start");
    auto it = callbackMap_.find(key);
    if (it == callbackMap_.end()) {
        return false;
    }
    HILOG_INFO("UIMgrService::CheckCallBackFromMap called end");
    return true;
}
}  // namespace Ace
}  // namespace OHOS
