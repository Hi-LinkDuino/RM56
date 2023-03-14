/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "system_ability_helper.h"

#include "app_log_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
sptr<IRemoteObject> SystemAbilityHelper::GetSystemAbility(const int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        APP_LOGE("fail to get the system ability manager to get %{public}d proxy", systemAbilityId);
        return nullptr;
    }
    return systemAbilityMgr->GetSystemAbility(systemAbilityId);
}

bool SystemAbilityHelper::AddSystemAbility(const int32_t systemAbilityId, const sptr<IRemoteObject> &systemAbility)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr && (systemAbilityMgr->AddSystemAbility(systemAbilityId, systemAbility) == 0)) {
        return true;
    }
    APP_LOGE("fail to register %{public}d to system ability manager", systemAbilityId);
    return false;
}

bool SystemAbilityHelper::RemoveSystemAbility(const int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr && (systemAbilityMgr->RemoveSystemAbility(systemAbilityId) == 0)) {
        return true;
    }
    APP_LOGE("fail to remove %{public}d from system ability manager", systemAbilityId);
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS