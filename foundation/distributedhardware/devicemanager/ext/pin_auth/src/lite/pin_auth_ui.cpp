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

namespace OHOS {
namespace DistributedHardware {
PinAuthUi::PinAuthUi()
{
    LOGI("AuthUi constructor");
}

int32_t PinAuthUi::ShowPinDialog(int32_t code, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("ShowConfigDialog end");
    return DM_OK;
}

int32_t PinAuthUi::InputPinDialog(int32_t code, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("InputPinDialog start");
    std::shared_ptr<DmAbilityManager> dmAbilityManager_ = std::make_shared<DmAbilityManager>();
    if (dmAbilityManager_ == nullptr) {
        LOGE("PinAuthUi::dmAbilityManager is null");
        return DM_FAILED;
    }
    LOGI("InputPinDialog end");
    return StartFaUiService(dmAbilityManager_);
}

int32_t ClosePage(const int32_t &pageId, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("ClosePage hap start");
    if (authManager == nullptr) {
        LOGE("PinAuthUi::authManager is null");
        return DM_FAILED;
    }
    LOGI("ClosePage hap end");
    authManager->CancelDisplay();
    return DM_OK;
}

int32_t PinAuthUi::StartFaUiService(std::shared_ptr<DmAbilityManager> dmAbilityManager)
{
    AbilityStatus status = dmAbilityManager->StartAbility(AbilityRole::ABILITY_ROLE_INITIATIVE);
    if (status != AbilityStatus::ABILITY_STATUS_SUCCESS) {
        LOGE("PinAuthUi::StartFaUiService timeout");
        return DM_FAILED;
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
