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

#include "ui_service_mgr_client.h"

#include "dialog_callback_stub.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

// external dependence
#include "ability_manager_client.h"
#include "display_manager.h"

namespace OHOS {
namespace Ace {
constexpr int UI_MGR_SERVICE_SA_ID = 7001;
constexpr int UI_DIALOG_PICKER_WIDTH = 519 * 2; // 519 vp
constexpr int UI_DIALOG_PICKER_HEIGHT = 256 * 2; // 256 vp
constexpr int UI_DIALOG_PICKER_WIDTH_NARROW = 360 * 2; // 360 vp
constexpr int UI_DIALOG_PICKER_HEIGHT_NARROW = 347 * 2; // 347 vp
constexpr int UI_DEFAULT_WIDTH = 2560;
constexpr int UI_DEFAULT_HEIGHT = 1600;
constexpr int UI_DEFAULT_BUTTOM_CLIP = 50 * 2; // 48vp
constexpr int UI_WIDTH_780DP = 780 * 2; // 780vp
constexpr int UI_HALF = 2;
std::shared_ptr<UIServiceMgrClient> UIServiceMgrClient::instance_ = nullptr;
std::mutex UIServiceMgrClient::mutex_;

std::shared_ptr<UIServiceMgrClient> UIServiceMgrClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<UIServiceMgrClient>();
        }
    }
    return instance_;
}

UIServiceMgrClient::UIServiceMgrClient()
{}

UIServiceMgrClient::~UIServiceMgrClient()
{}

ErrCode UIServiceMgrClient::RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->RegisterCallBack(want, uiService);
}

ErrCode UIServiceMgrClient::UnregisterCallBack(const AAFwk::Want& want)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->UnregisterCallBack(want);
}

ErrCode UIServiceMgrClient::Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
    const std::string& data, const std::string& extraData)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->Push(want, name, jsonPath, data, extraData);
}

ErrCode UIServiceMgrClient::Request(const AAFwk::Want& want, const std::string& name, const std::string& data)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->Request(want, name, data);
}

ErrCode UIServiceMgrClient::ReturnRequest(const AAFwk::Want& want, const std::string& source,
    const std::string& data, const std::string& extraData)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->ReturnRequest(want, source, data, extraData);
}

ErrCode UIServiceMgrClient::ShowDialog(const std::string& name,
                                       const std::string& params,
                                       OHOS::Rosen::WindowType windowType,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       DialogCallback callback,
                                       int* id)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    const sptr<DialogCallbackStub> dialogCallbackStub(new (std::nothrow)DialogCallbackStub(callback));
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    if (doms == nullptr) {
        HILOG_ERROR("doms is nullptr");
        return UI_SERVICE_GET_PROXY_FAILED;
    }
    return doms->ShowDialog(name, params, windowType, x, y, width, height, dialogCallbackStub, id);
}

ErrCode UIServiceMgrClient::CancelDialog(int32_t id)
{
    if (id < 0) {
        HILOG_INFO("invalid parameter");
        return UI_SERVICE_INVALID_PARAMETER;
    }

    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }

    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    if (doms == nullptr) {
        HILOG_ERROR("doms is nullptr");
        return UI_SERVICE_GET_PROXY_FAILED;
    }
    return doms->CancelDialog(id);
}

ErrCode UIServiceMgrClient::ShowAppPickerDialog(
    const AAFwk::Want& want, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos, int32_t userId)
{
    if (abilityInfos.size() == 0) {
        HILOG_WARN("abilityInfos size is zero");
        return UI_SERVICE_INVALID_PARAMETER;
    }
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t width = UI_DIALOG_PICKER_WIDTH;
    int32_t height = UI_DIALOG_PICKER_HEIGHT;
    bool wideScreen = true;
    GetDisplayPosition(offsetX, offsetY, width, height, wideScreen);
    const std::string param = GetPickerDialogParam(want, abilityInfos, wideScreen);
    HILOG_DEBUG("share dialog position:[%{public}d,%{public}d,%{public}d,%{public}d],str: %{public}s",
        offsetX, offsetY, width, height, param.c_str());
    const std::string jsBundleName = "dialog_picker_service";
    return ShowDialog(jsBundleName, param, OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, offsetX, offsetY,
        width, height, [want, userId](int32_t id, const std::string& event, const std::string& params) mutable {
            HILOG_DEBUG("dialog callback: event: %{public}s, params: %{public}s", event.c_str(), params.c_str());
            if (event == "SHARE_EVENT") {
                std::string bundleName;
                std::string abilityName;
                auto pos = params.find(";");
                if (pos != std::string::npos) {
                    bundleName = params.substr(0, pos);
                    abilityName = params.substr(pos + 1, params.length() - (pos + 1));
                }
                AAFwk::Want shareWant = want;
                shareWant.SetAction("");
                shareWant.SetElementName(bundleName, abilityName);
                auto abilityClient = AAFwk::AbilityManagerClient::GetInstance();
                if (abilityClient != nullptr) {
                    HILOG_INFO("dialog callback: %{public}s-%{public}s", bundleName.c_str(), abilityName.c_str());
                    abilityClient->StartAbility(shareWant, AAFwk::DEFAULT_INVAL_VALUE, userId);
                }
            }
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        });
}

/**
 * Connect ui_service manager service.
 *
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode UIServiceMgrClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (remoteObject_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        HILOG_ERROR("%{private}s:fail to get Registry", __func__);
        return GET_UI_SERVICE_FAILED;
    }
    remoteObject_ = systemManager->GetSystemAbility(UI_MGR_SERVICE_SA_ID);
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
        return GET_UI_SERVICE_FAILED;
    }
    HILOG_DEBUG("connect UIMgrService success");
    return ERR_OK;
}

const std::string UIServiceMgrClient::GetPickerDialogParam(
    const AAFwk::Want& want, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos, bool wideScreen) const
{
    auto type = want.GetStringParam("ability.picker.type");
    auto text = want.GetStringParam("ability.picker.text");
    auto uri = want.GetStringParam("ability.picker.uri");
    auto fileNames = want.GetStringArrayParam("ability.picker.fileNames");
    auto fileSizes = want.GetIntArrayParam("ability.picker.fileSizes");

    std::string param = "{"; // json head
    if (!wideScreen) {
        param += "\"deviceType\": \"phone\",";
    }
    param += "\"previewCard\": { \"type\": \"";
    param += type;
    param += "\", \"icon\": \"";
    param += "";
    param += "\", \"mainText\": \"";
    param += text;
    param += "\", \"subText\": \"";
    param += uri;
    param += "\", \"fileList\": [";
    for (int i = 0; i < (int)fileNames.size() && i < (int)fileSizes.size(); i++) {
        param += "{";
        param += "\"name\": \"";
        param += fileNames[i];
        param += "\", \"size\": ";
        param += std::to_string(fileSizes[i]);
        param += "}";
        if (i != (int)fileNames.size() - 1 && i != (int)fileSizes.size() - 1) {
            param += ",";
        }
    }
    param += "]},";
    param += "\"hapList\": [";
    for (int i = 0; i < (int)abilityInfos.size(); i++) {
        const auto& abilityInfo = abilityInfos[i];
        param += "{ \"name\": \"";
        param += std::to_string(abilityInfo.labelId); // or string abilityInfo.label
        param += "\", \"icon\": \"";
        param += std::to_string(abilityInfo.iconId); // or string abilityInfo.icon
        param += "\", \"bundle\": \"";
        param += abilityInfo.bundleName;
        param += "\", \"ability\": \"";
        param += abilityInfo.name;
        param += "\" }";
        if (i != (int)abilityInfos.size() - 1) {
            param += ",";
        }
    }
    param += "]";
    param += "}"; // json tail
    return param;
}

void UIServiceMgrClient::GetDisplayPosition(
    int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height, bool& wideScreen)
{
    wideScreen = true;
    auto display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        HILOG_WARN("share dialog GetDefaultDisplay fail, try again.");
        display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    }

    if (display != nullptr) {
        if (display->GetWidth() < UI_WIDTH_780DP) {
            HILOG_INFO("share dialog narrow.");
            wideScreen = false;
            width = UI_DIALOG_PICKER_WIDTH_NARROW;
            height = UI_DIALOG_PICKER_HEIGHT_NARROW;
        }
        offsetX = (display->GetWidth() - width) / UI_HALF;
        offsetY = display->GetHeight() - height - UI_DEFAULT_BUTTOM_CLIP;
    } else {
        HILOG_WARN("share dialog get display fail, use default wide.");
        offsetX = (UI_DEFAULT_WIDTH - width) / UI_HALF;
        offsetY = UI_DEFAULT_HEIGHT - height - UI_DEFAULT_BUTTOM_CLIP;
    }
}
}  // namespace Ace
}  // namespace OHOS
