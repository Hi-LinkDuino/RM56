/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pin_auth_ui.h"

#include "dm_ability_manager.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"
#include "ui_service_mgr_client.h"

namespace OHOS {
namespace DistributedHardware {
PinAuthUi::PinAuthUi()
{
    LOGI("AuthUi constructor");
}
 
int32_t PinAuthUi::ShowPinDialog(int32_t code, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("ShowPinDialog start");
    if (authManager == nullptr) {
        LOGE("ShowPinDialog authManager is null");
        return DM_FAILED;
    }
    nlohmann::json jsonObj;
    jsonObj[PIN_CODE_KEY] = code;
    jsonObj.dump();
    const std::string params = jsonObj.dump();

    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        "show_pin_service",
        params,
        OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        ACE_X, ACE_Y, ACE_WIDTH, ACE_HEIGHT,
        [authManager](int32_t id, const std::string& event, const std::string& params) {
            if (params == EVENT_INIT_CODE) {
                authManager->SetPageId(id);
            }
            if (params == EVENT_CANCEL_CODE) {
                LOGI("CancelDialog start id:%d,event:%s,parms:%s", id, event.c_str(), params.c_str());
                Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
            }
        });
    LOGI("ShowConfigDialog end");
    return DM_OK;
}
 
int32_t PinAuthUi::InputPinDialog(int32_t code, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("InputPinDialog start");
    if (authManager == nullptr) {
        LOGE("ShowPinDialog InputPinDialog is null");
        return DM_FAILED;
    }
    nlohmann::json jsonObj;
    jsonObj[PIN_CODE_KEY] = code;
    jsonObj.dump();
    const std::string params = jsonObj.dump();

    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        "input_pin_service",
        params,
        OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        ACE_X, ACE_Y, ACE_WIDTH, ACE_HEIGHT,
        [authManager](int32_t id, const std::string& event, const std::string& params) {
            if (params == EVENT_INIT_CODE) {
                authManager->SetPageId(id);
            }
            if (params == EVENT_CANCEL_CODE || params == EVENT_CONFIRM_CODE) {
                Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
                LOGI("CancelDialog start id:%d,event:%s,parms:%s", id, event.c_str(), params.c_str());
                authManager->VerifyAuthentication(params.c_str());
            }
        });
    LOGI("ShowConfigDialog end");
    return DM_OK;
}

int32_t PinAuthUi::ClosePage(const int32_t &pageId, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("PinAuthUi start");
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(pageId);
    LOGI("PinAuthUi end");
    return DM_OK;
}

int32_t PinAuthUi::StartFaUiService(std::shared_ptr<DmAbilityManager> dmAbilityManager)
{
    AbilityStatus status = dmAbilityManager->StartAbility(AbilityRole::ABILITY_ROLE_INITIATIVE);
    if (status != AbilityStatus::ABILITY_STATUS_SUCCESS) {
        LOGE("PinAuthUi::StartFaService timeout");
        return DM_FAILED;
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS