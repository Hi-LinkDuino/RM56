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

#include "ability_manager_helper.h"

#include "app_log_wrapper.h"
#include "system_ability_helper.h"
#include "system_ability_definition.h"

#ifdef ABILITY_RUNTIME_ENABLE
#include "ability_manager_interface.h"
#endif

namespace OHOS {
namespace AppExecFwk {
bool AbilityManagerHelper::UninstallApplicationProcesses(const std::string &bundleName, const int uid)
{
#ifdef ABILITY_RUNTIME_ENABLE
    APP_LOGI("uninstall kill running processes, app name is %{public}s", bundleName.c_str());
    sptr<AAFwk::IAbilityManager> abilityMgrProxy =
        iface_cast<AAFwk::IAbilityManager>(SystemAbilityHelper::GetSystemAbility(ABILITY_MGR_SERVICE_ID));
    if (!abilityMgrProxy) {
        APP_LOGE("fail to find the app mgr service to kill application");
        return false;
    }
    if (abilityMgrProxy->UninstallApp(bundleName, uid) != 0) {
        APP_LOGE("kill application process failed");
        return false;
    }
    return true;
#else
    APP_LOGI("ABILITY_RUNTIME_ENABLE is false");
    return true;
#endif
}
}  // namespace AppExecFwk
}  // namespace OHOS
