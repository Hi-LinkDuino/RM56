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

#include "permission_manager.h"

#include "bundle_constants.h"
#include "bundle_info.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "access_token.h"
#include "hap_token_info.h"
#include "native_token_info.h"
#include "accesstoken_kit.h"
#include "dm_log.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::Constants;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(PermissionManager);

bool PermissionManager::CheckPermission(void)
{
    LOGI("Enter PermissionManager::CheckPermission");
    AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    if (tokenCaller == 0) {
        LOGI("PermissionManager::tokenCaller == 0");
        return false;
    }
    LOGI("PermissionManager::tokenCaller ID == %d", tokenCaller);

    ATokenTypeEnum tokenTypeFlag = AccessTokenKit::GetTokenTypeFlag(tokenCaller);
    if (tokenTypeFlag == ATokenTypeEnum::TOKEN_HAP) {
        int32_t uid = IPCSkeleton::GetCallingUid();
        if (uid < 0) {
            LOGI("app caller uid is: %d,", uid);
            return false;
        }
        
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            LOGE("failed to get system ability mgr.");
            return true;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (remoteObject == nullptr) {
            LOGE("failed to get bundle manager proxy.");
            return true;
        }

        LOGI("get bundle manager proxy success.");
        sptr<IBundleMgr> iBundleMgr = iface_cast<IBundleMgr>(remoteObject);
        if (iBundleMgr == nullptr) {
            LOGI("iBundleMgr is nullptr, caller may be a process");
            return true;
        }
        LOGI("PermissionManager::tokenTypeFlag is hap process");
        return iBundleMgr->CheckIsSystemAppByUid(uid);
    } else if (tokenTypeFlag == ATokenTypeEnum::TOKEN_NATIVE) {
        LOGI("PermissionManager::tokenTypeFlag is native process");
        return true;
    } else {
        LOGI("PermissionManager::invalid tokenTypeFlag");
        return false;
    }
}
} // namespace DistributedHardware
} // namespace OHOS
