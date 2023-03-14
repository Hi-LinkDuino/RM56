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

#include "adapter/ohos/entrance/ace_form_ability.h"

#include "res_config.h"
#include "resource_manager.h"

#include "adapter/ohos/entrance/pa_container.h"
#include "adapter/ohos/entrance/pa_engine/pa_backend.h"
#include "adapter/ohos/entrance/platform_event_callback.h"
#include "adapter/ohos/entrance/utils.h"
#include "base/log/log.h"
#include "core/common/backend.h"

namespace OHOS::Ace {
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using FormPlatformFinish = std::function<void()>;
class FormPlatformEventCallback final : public Platform::PlatformEventCallback {
public:
    explicit FormPlatformEventCallback(FormPlatformFinish onFinish) : onFinish_(onFinish) {}

    ~FormPlatformEventCallback() = default;

    virtual void OnFinish() const
    {
        LOGI("FormPlatformEventCallback OnFinish");
        if (onFinish_) {
            onFinish_();
        }
    }

    virtual void OnStatusBarBgColorChanged(uint32_t color)
    {
        LOGI("FormPlatformEventCallback OnStatusBarBgColorChanged");
    }

private:
    FormPlatformFinish onFinish_;
};

int32_t AceFormAbility::instanceId_ = 300000;
const std::string AceFormAbility::START_PARAMS_KEY = "__startParams";
const std::string AceFormAbility::URI = "url";

REGISTER_AA(AceFormAbility)

AceFormAbility::AceFormAbility()
{
    instanceId_++;
}

void AceFormAbility::LoadFormEnv(const OHOS::AAFwk::Want& want)
{
    // get url
    std::string parsedUrl;
    if (want.HasParameter(URI)) {
        parsedUrl = want.GetStringParam(URI);
    } else {
        parsedUrl = "form.js";
    }

    // get asset
    auto packagePathStr = GetBundleCodePath();
    auto moduleInfo = GetHapModuleInfo();
    if (moduleInfo != nullptr) {
        packagePathStr += "/" + moduleInfo->name + "/";
    }

    // init form ability
    BackendType backendType = BackendType::FORM;
    bool isArkApp = GetIsArkFromConfig(packagePathStr);
    Platform::PaContainer::CreateContainer(instanceId_, backendType, isArkApp, this,
        std::make_unique<FormPlatformEventCallback>([this]() { TerminateAbility(); }));

    std::shared_ptr<AbilityInfo> info = GetAbilityInfo();
    if (info != nullptr && !info->srcPath.empty()) {
        LOGI("AceFormAbility srcPath:%{public}s url:%{public}s", info->srcPath.c_str(), parsedUrl.c_str());
        auto assetBasePathStr = { "assets/js/" + info->srcPath + "/" };
        Platform::PaContainer::AddAssetPath(instanceId_, packagePathStr, assetBasePathStr);
    } else {
        LOGI("AceFormAbility parsedUrl:%{public}s", parsedUrl.c_str());
        auto assetBasePathStr = { std::string("assets/js/default/"), std::string("assets/js/share/") };
        Platform::PaContainer::AddAssetPath(instanceId_, packagePathStr, assetBasePathStr);
    }
    std::shared_ptr<ApplicationInfo> appInfo = GetApplicationInfo();
    if (appInfo) {
        std::string nativeLibraryPath = appInfo->nativeLibraryPath;
        if (!nativeLibraryPath.empty()) {
            if (nativeLibraryPath.back() == '/') {
                nativeLibraryPath.pop_back();
            }
            std::string libPath = GetBundleCodePath();
            if (libPath.back() == '/') {
                libPath += nativeLibraryPath;
            } else {
                libPath += "/" + nativeLibraryPath;
            }
            Platform::PaContainer::AddLibPath(instanceId_, libPath);
        }
    }

    // run form ability
    Platform::PaContainer::RunPa(instanceId_, parsedUrl, want);
}

OHOS::AppExecFwk::FormProviderInfo AceFormAbility::OnCreate(const OHOS::AAFwk::Want& want)
{
    std::string formId = want.GetStringParam(AppExecFwk::Constants::PARAM_FORM_IDENTITY_KEY);
    LOGI("AceFormAbility::OnCreate formId = %{public}s.", formId.c_str());
    Platform::PaContainer::OnCreate(instanceId_, want);
    OHOS::AppExecFwk::FormProviderInfo formProviderInfo;
    formProviderInfo.SetFormData(Platform::PaContainer::GetFormData(instanceId_));
    std::string formData = formProviderInfo.GetFormData().GetDataString();
    LOGI("AceFormAbility::OnCreate return ok, formData: %{public}s", formData.c_str());
    return formProviderInfo;
}

void AceFormAbility::OnDelete(const int64_t formId)
{
    LOGI("AceFormAbility::OnDelete called: %{public}s", std::to_string(formId).c_str());
    Platform::PaContainer::OnDelete(instanceId_, formId);
}

void AceFormAbility::OnTriggerEvent(const int64_t formId, const std::string& message)
{
    LOGI("AceFormAbility::OnTriggerEvent called: %{public}s", std::to_string(formId).c_str());
    Platform::PaContainer::OnTriggerEvent(instanceId_, formId, message);
}

void AceFormAbility::OnUpdate(const int64_t formId)
{
    LOGI("AceFormAbility::OnUpdate called: %{public}s", std::to_string(formId).c_str());
    Platform::PaContainer::OnUpdate(instanceId_, formId);
}

void AceFormAbility::OnCastTemptoNormal(const int64_t formId)
{
    LOGI("AceFormAbility::OnCastTemptoNormal called: %{public}s", std::to_string(formId).c_str());
    Platform::PaContainer::OnCastTemptoNormal(instanceId_, formId);
}

void AceFormAbility::OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap)
{
    LOGI("AceFormAbility::OnVisibilityChanged called");
    Platform::PaContainer::OnVisibilityChanged(instanceId_, formEventsMap);
}

void AceFormAbility::OnStart(const OHOS::AAFwk::Want& want)
{
    LOGI("AceFormAbility::OnStart start");
    Ability::OnStart(want);
    LoadFormEnv(want);
}

void AceFormAbility::OnStop()
{
    LOGI("AceFormAbility::OnStop start ");
    Ability::OnStop();
}

sptr<IRemoteObject> AceFormAbility::OnConnect(const Want& want)
{
    LOGI("AceFormAbility::OnConnect start");
    Ability::OnConnect(want);
    return GetFormRemoteObject();
}

void AceFormAbility::OnDisconnect(const Want& want)
{
    LOGI("AceFormAbility::OnDisconnect start");
    Ability::OnDisconnect(want);
}

} // namespace OHOS::Ace
