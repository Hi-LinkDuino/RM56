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

#include "core/components/plugin/plugin_component_manager.h"

#include <map>
#include <vector>

#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "ui_service_mgr_client.h"

#include "base/json/json_util.h"
#include "base/log/log.h"

namespace OHOS::Ace {
constexpr int64_t FOO_MAX_LEN = 20 * 1024 * 1024;
std::shared_ptr<PluginComponentManager> PluginComponentManager::instance_ = nullptr;
std::mutex PluginComponentManager::mutex_;

std::shared_ptr<PluginComponentManager> PluginComponentManager::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = std::make_shared<PluginComponentManager>();
        }
    }
    return instance_;
}

int PluginComponentManager::Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
    const std::string& data, const std::string& extraData)
{
    LOGD("PluginComponentManager::Push");
    return Ace::UIServiceMgrClient::GetInstance()->Push(want, name, jsonPath, data, extraData);
}

int PluginComponentManager::Request(
    const AAFwk::Want& want, const std::string& name, const std::string& jsonPath, const std::string& data)
{
    LOGD("PluginComponentManager::Request: jsonPath=%{public}s", jsonPath.c_str());
    if (jsonPath.empty()) {
        return Ace::UIServiceMgrClient::GetInstance()->Request(want, name, data);
    } else {
        PluginComponentTemplate pluginTemplate;
        // Read external.json(assets\external.json) or jsonPath
        std::string jsonStr;
        auto packagePathStr = GetPackagePath(want);
        GetTemplatePathFromJsonFile(packagePathStr, name, jsonPath, jsonStr);
        pluginTemplate.SetSource(jsonStr);
        pluginTemplate.SetAbility(want.GetElement().GetBundleName() + "/" + want.GetElement().GetAbilityName());

        for (auto iter = listener_->GetPluginComponentCallBack().begin();
             iter != listener_->GetPluginComponentCallBack().end();) {
            if (iter->second == CallBackType::RequestCallBack && iter->first != nullptr) {
                iter->first->OnRequestCallBack(pluginTemplate, data, "");
                listener_->GetPluginComponentCallBack().erase(iter++);
            } else {
                iter++;
            }
        }
        return 0;
    }
}

int PluginComponentManager::ReturnRequest(
    const AAFwk::Want& want, const std::string& pluginName, const std::string& data, const std::string& extraData)
{
    LOGD("PluginComponentManager::ReturnRequest");
    return Ace::UIServiceMgrClient::GetInstance()->ReturnRequest(want, pluginName, data, extraData);
}

void PluginComponentManager::RegisterCallBack(
    const AAFwk::Want& want, const std::shared_ptr<PluginComponentCallBack>& callback, CallBackType callBackType)
{
    LOGD("PluginComponentManager::RegisterCallBack");
    if (listener_) {
        listener_->ResgisterListener(callback, callBackType);
        Ace::UIServiceMgrClient::GetInstance()->RegisterCallBack(want, listener_);
    }
}

void PluginComponentManager::UnregisterCallBack(const AAFwk::Want& want)
{
    Ace::UIServiceMgrClient::GetInstance()->UnregisterCallBack(want);
}

sptr<AppExecFwk::IBundleMgr> PluginComponentManager::GetBundleManager()
{
    auto bundleObj =
        OHOS::DelayedSingleton<AAFwk::SaMgrClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        LOGE("failed to get bundle manager service");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
}

void PluginComponentManager::UIServiceListener::ResgisterListener(
    const std::shared_ptr<PluginComponentCallBack>& callback, CallBackType callBackType)
{
    LOGD("PluginComponentManager::UIServiceListener::ResgisterListener");
    std::lock_guard<std::mutex> lock(mutex_);
    callbackVec_.try_emplace(callback, callBackType);
}

void PluginComponentManager::UIServiceListener::OnPushCallBack(const AAFwk::Want& want,
    const std::string& name, const std::string& jsonPath, const std::string& data, const std::string& extraData)
{
    LOGD("PluginComponentManager::UIServiceListener::OnPushCallBack");
    PluginComponentTemplate pluginTemplate;
    if (!jsonPath.empty()) {
        std::string jsonStr;
        auto packagePathStr = PluginComponentManager::GetInstance()->GetPackagePath(want);
        PluginComponentManager::GetInstance()->GetTemplatePathFromJsonFile(packagePathStr, name, jsonPath, jsonStr);
        pluginTemplate.SetSource(jsonStr);
    } else {
        pluginTemplate.SetSource(name);
    }
    pluginTemplate.SetAbility(want.GetElement().GetBundleName() + "/" + want.GetElement().GetAbilityName());

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &callback : callbackVec_) {
        if (callback.second == CallBackType::PushEvent && callback.first != nullptr) {
            callback.first->OnPushEvent(want, pluginTemplate, data, extraData);
        }
    }
}

void PluginComponentManager::UIServiceListener::OnRequestCallBack(
    const AAFwk::Want& want, const std::string& name,  const std::string& data)
{
    LOGD("PluginComponentManager::UIServiceListener::OnRequestCallBack");
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &callback : callbackVec_) {
        if (callback.second == CallBackType::RequestEvent && callback.first != nullptr) {
            callback.first->OnRequestEvent(want, name, data);
        }
    }
}

void PluginComponentManager::UIServiceListener::OnReturnRequest(
    const AAFwk::Want& want, const std::string& source, const std::string& data, const std::string& extraData)
{
    LOGD("PluginComponentManager::UIServiceListener::OnReturnRequest");
    for (auto iter = callbackVec_.begin(); iter != callbackVec_.end();) {
        if (iter->second == CallBackType::RequestCallBack && iter->first != nullptr) {
            PluginComponentTemplate pluginTemplate;
            pluginTemplate.SetSource(source);
            pluginTemplate.SetAbility(want.GetElement().GetBundleName() + "/" + want.GetElement().GetAbilityName());
            iter->first->OnRequestCallBack(pluginTemplate, data, extraData);
            callbackVec_.erase(iter++);
        } else {
            iter++;
        }
    }
}

bool PluginComponentManager::GetTemplatePathFromJsonFile(
    const std::string& packagePathStr, const std::string& pluginName, const std::string& jsonPath, std::string& jsonStr)
{
    std::string external = std::string("/external.json");
    auto externalPath = packagePathStr + external;
    if (!jsonPath.empty()) {
        externalPath = jsonPath;
    }
    char realPath[PATH_MAX] = { 0x00 };
    if (realpath(externalPath.c_str(), realPath) == nullptr) {
        LOGE("realpath fail! filePath: %{private}s, fail reason: %{public}s",
            externalPath.c_str(), strerror(errno));
        return false;
    }
    std::unique_ptr<FILE, decltype(&std::fclose)> file(std::fopen(realPath, "rb"), std::fclose);
    if (!file) {
        LOGE("open file failed, filePath: %{private}s, fail reason: %{public}s",
            externalPath.c_str(), strerror(errno));
        return false;
    }
    if (std::fseek(file.get(), 0, SEEK_END) != 0) {
        LOGE("seek file tail error.");
        return false;
    }

    int64_t size = std::ftell(file.get());
    if (size == -1L || size == 0L || size > FOO_MAX_LEN) {
        return false;
    }
    char* fileData = new (std::nothrow) char[size];
    if (fileData == nullptr) {
        LOGE("new json buff failed.");
        return false;
    }
    rewind(file.get());
    std::unique_ptr<char[]> jsonStream(fileData);
    size_t result = std::fread(jsonStream.get(), 1, size, file.get());
    if (result != (size_t)size) {
        LOGE("read file failed.");
        return false;
    }

    std::string jsonString(jsonStream.get(), jsonStream.get() + size);
    if (pluginName.empty()) {
        jsonStr = jsonString;
    } else {
        auto rootJson = JsonUtil::ParseJsonString(jsonString);
        if (rootJson) {
            jsonStr = rootJson->GetString(pluginName);
        }
    }

    return true;
}

std::string PluginComponentManager::GetPackagePath(const AAFwk::Want& want) const
{
    std::string packagePathStr;

    if (want.GetElement().GetBundleName().empty()) {
        LOGE("App bundleName is empty.");
        return packagePathStr;
    }
    auto bms = PluginComponentManager::GetInstance()->GetBundleManager();
    if (!bms) {
        LOGE("Bms bundleManager is nullptr.");
        return packagePathStr;
    }

    if (want.GetElement().GetAbilityName().empty()) {
        AppExecFwk::BundleInfo bundleInfo;
        bool ret = bms->GetBundleInfo(
            want.GetElement().GetBundleName(), AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
        if (!ret) {
            LOGE("Bms get bundleName failed!");
            return packagePathStr;
        }
        packagePathStr = bundleInfo.applicationInfo.entryDir + "/";
    } else {
        AppExecFwk::AbilityInfo abilityInfo;
        bool ret = bms->QueryAbilityInfo(want, abilityInfo);
        if (!ret) {
            LOGE("Bms get abilityInfo failed!");
            return packagePathStr;
        }
        packagePathStr = abilityInfo.applicationInfo.codePath + "/" + abilityInfo.package + "/";
    }
    return packagePathStr;
}
} // namespace OHOS::Ace
