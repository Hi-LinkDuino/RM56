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

#include "adapter/ohos/entrance/subwindow/subwindow_ohos.h"

#include "dm/display_manager.h"
#include "interfaces/innerkits/ace/viewport_config.h"
#include "render_service_client/core/ui/rs_ui_director.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/entrance/flutter_ace_view.h"
#include "adapter/ohos/entrance/utils.h"
#include "base/utils/system_properties.h"
#include "core/common/container_scope.h"
#include "core/common/frontend.h"
#include "core/components/popup/popup_component.h"

namespace OHOS::Ace {

int32_t SubwindowOhos::id_ = 0;

RefPtr<Subwindow> Subwindow::CreateSubwindow(int32_t instanceId)
{
    LOGI("Create Subwindow, parent container id is %{public}d", instanceId);
    return AceType::MakeRefPtr<SubwindowOhos>(instanceId);
}

SubwindowOhos::SubwindowOhos(int32_t instanceId)
{
    parentContainerId_ = instanceId;
    windowId_ = id_;
    SetSubwindowId(windowId_);
    id_++;
}

void SubwindowOhos::InitContainer()
{
    LOGI("Subwindow start initialize container");
    if (!window_) {
        LOGI("Window is null, need create a new window");
        OHOS::sptr<OHOS::Rosen::WindowOption> windowOption = new OHOS::Rosen::WindowOption();
        auto parentWindowName = Platform::AceContainer::GetContainer(parentContainerId_)->GetWindowName();
        auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
        sptr<OHOS::Rosen::Window> parentWindow = OHOS::Rosen::Window::Find(parentWindowName);
        if (parentWindow == nullptr) {
            return;
        }
        auto windowType = parentWindow->GetType();
        if (windowType == Rosen::WindowType::WINDOW_TYPE_DESKTOP) {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_FLOAT);
        } else {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
            windowOption->SetParentName(parentWindowName);
        }
        windowOption->SetWindowRect({ 0, 0, defaultDisplay->GetWidth(), defaultDisplay->GetHeight() });
        windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
        window_ = OHOS::Rosen::Window::Create(
            "ARK_APP_SUBWINDOW_" + parentWindowName + std::to_string(windowId_), windowOption);
        if (window_ == nullptr) {
            LOGI("create window error return");
            return;
        }
    }
    std::string url = "";
    window_->SetUIContent(url, nullptr, nullptr, false);
    childContainerId_ = SubwindowManager::GetInstance()->GetContainerId(window_->GetWindowId());
    SubwindowManager::GetInstance()->AddParentContainerId(childContainerId_, parentContainerId_);
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    if (!parentContainer) {
        LOGE("Get container failed, container is null");
        return;
    }

    auto container = Platform::AceContainer::GetContainer(childContainerId_);
    if (!container) {
        LOGE("Get container failed, container is null");
        return;
    }

    container->SetParentId(parentContainerId_);
    container->GetSettings().SetUsingSharedRuntime(true);
    container->SetSharedRuntime(parentContainer->GetSharedRuntime());
    container->Initialize();
    container->SetAssetManager(parentContainer->GetAssetManager());
    container->SetResourceConfiguration(parentContainer->GetResourceConfiguration());
    container->SetPackagePathStr(parentContainer->GetPackagePathStr());
    container->SetIsSubContainer(true);
    container->InitializeSubContainer(parentContainerId_);
    ViewportConfig config;
    // create ace_view
    auto flutterAceView =
        Platform::FlutterAceView::CreateView(childContainerId_, true, container->GetSettings().usePlatformAsUIThread);
    Platform::FlutterAceView::SurfaceCreated(flutterAceView, window_);

    int32_t width = static_cast<int32_t>(window_->GetRect().width_);
    int32_t height = static_cast<int32_t>(window_->GetRect().height_);
    LOGI("UIContent Initialize: width: %{public}d, height: %{public}d", width, height);

    Ace::Platform::UIEnvCallback callback = nullptr;
    // set view
    Platform::AceContainer::SetView(flutterAceView, config.Density(), width, height, window_->GetWindowId(), callback);
    Platform::FlutterAceView::SurfaceChanged(flutterAceView, width, height, config.Orientation());

#ifdef ENABLE_ROSEN_BACKEND
    if (SystemProperties::GetRosenBackendEnabled()) {
        rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
        if (rsUiDirector != nullptr) {
            rsUiDirector->SetRSSurfaceNode(window_->GetSurfaceNode());
            rsUiDirector->SetUITaskRunner(
                [taskExecutor = container->GetTaskExecutor()](
                    const std::function<void()>& task) { taskExecutor->PostTask(task, TaskExecutor::TaskType::UI); });
            auto context = container->GetPipelineContext();
            if (context != nullptr) {
                LOGI("Init RSUIDirector");
                context->SetRSUIDirector(rsUiDirector);
            }
            rsUiDirector->Init();
            LOGI("UIContent Init Rosen Backend");
        }
    }
#endif
    auto subPipelineContext = Platform::AceContainer::GetContainer(childContainerId_)->GetPipelineContext();
    if (!subPipelineContext) {
        LOGE("Get SubPipelineContext failed, pipelineContext is null");
    }
    subPipelineContext->SetupSubRootElement();
}

void SubwindowOhos::ShowWindow()
{
    LOGI("Show the subwindow");
    if (!window_) {
        LOGE("Show window failed, window is null");
        return;
    }

    OHOS::Rosen::WMError ret = window_->Show();

    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("Show window failed with errCode: %{public}d", static_cast<int32_t>(ret));
        return;
    }
    LOGI("Show the subwindow successfully.");
    SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));
}

void SubwindowOhos::HideWindow()
{
    LOGI("Hide the subwindow");
    if (!window_) {
        LOGE("Hide window failed, window is null");
        return;
    }

    OHOS::Rosen::WMError ret = window_->Hide();

    if (ret != OHOS::Rosen::WMError::WM_OK) {
        LOGE("Hide window failed with errCode: %{public}d", static_cast<int32_t>(ret));
        return;
    }
    LOGI("Hide the subwindow successfully.");
}

void SubwindowOhos::AddMenu(const RefPtr<Component>& newComponent)
{
    LOGI("Subwindow push new component start.");
    auto stack = GetStack();
    if (!stack) {
        LOGE("Get stack failed, it is null");
        return;
    }
    // Push the component
    stack->PopMenu();
    stack->PushComponent(newComponent);
    popup_ = AceType::DynamicCast<SelectPopupComponent>(newComponent);
    if (!popup_) {
        LOGE("Add menu failed, this is not a popup component.");
    }
    LOGI("Subwindow push new component end.");
}

void SubwindowOhos::ClearMenu()
{
    LOGI("Subwindow Clear menu start.");
    auto stack = GetStack();
    if (!stack) {
        LOGE("Get stack failed, it is null");
        return;
    }
    // Pop the component
    stack->PopMenu();
    auto context = stack->GetContext().Upgrade();
    if (!context) {
        LOGE("Get context failed, it is null");
    }
    context->FlushPipelineImmediately();
    HideWindow();
    LOGI("Subwindow clear menu end.");
}

void SubwindowOhos::ShowMenu(const RefPtr<Component>& newComponent)
{
    LOGI("Show the menu");
    ShowWindow();
    AddMenu(newComponent);
}

void SubwindowOhos::CloseMenu()
{
    LOGI("Close the menu");
    if (popup_) {
        popup_->CloseContextMenu();
    }
}

RefPtr<StackElement> SubwindowOhos::GetStack()
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        LOGE("Get container failed, it is null");
        return nullptr;
    }

    auto context = aceContainer->GetPipelineContext();
    if (!context) {
        LOGE("Get context failed, it is null");
        return nullptr;
    }
    return context->GetLastStack();
}

} // namespace OHOS::Ace