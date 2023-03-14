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

#include "account_helper.h"

#include <vector>

#include "app_log_wrapper.h"
#include "bundle_constants.h"

#ifdef ACCOUNT_ENABLE
#include "os_account_manager.h"
#endif

namespace OHOS {
namespace AppExecFwk {
int AccountHelper::IsOsAccountExists(const int id, bool &isOsAccountExists)
{
#ifdef ACCOUNT_ENABLE
    return AccountSA::OsAccountManager::IsOsAccountExists(id, isOsAccountExists);
#else
    APP_LOGI("ACCOUNT_ENABLE is false");
    // ACCOUNT_ENABLE is false, do nothing and return -1.
    return -1;
#endif
}

int32_t AccountHelper::GetCurrentActiveUserId()
{
#ifdef ACCOUNT_ENABLE
    std::vector<int32_t> activeIds;
    int ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activeIds);
    if (ret != 0) {
        APP_LOGE("QueryActiveOsAccountIds failed ret:%{public}d", ret);
        return Constants::INVALID_USERID;
    }
    if (activeIds.empty()) {
        APP_LOGE("QueryActiveOsAccountIds activeIds empty");
        return Constants::INVALID_USERID;
    }
    return activeIds[0];
#else
    APP_LOGI("ACCOUNT_ENABLE is false");
    return 0;
#endif
}
}  // namespace AppExecFwk
}  // namespace OHOS
