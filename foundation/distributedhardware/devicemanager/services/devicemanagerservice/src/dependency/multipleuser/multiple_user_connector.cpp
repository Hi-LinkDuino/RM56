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

#include "multiple_user_connector.h"

#include "dm_log.h"
#if !(defined(__LITEOS_M__) || defined(LITE_DEVICE))
#include "os_account_manager.h"
using namespace OHOS::AccountSA;
#endif

namespace OHOS {
namespace DistributedHardware {
int32_t MultipleUserConnector::oldUserId_ = -1;

int32_t MultipleUserConnector::GetCurrentAccountUserID(void)
{
#if (defined(__LITEOS_M__) || defined(LITE_DEVICE))
    return 0;
#else
    std::vector<int> ids;
    ErrCode ret = OsAccountManager::QueryActiveOsAccountIds(ids);
    if (ret != ERR_OK || ids.empty()) {
        return -1;
    }
    return ids[0];
#endif
}

void MultipleUserConnector::SetSwitchOldUserId(int32_t userId)
{
    oldUserId_ = userId;
}

int32_t MultipleUserConnector::GetSwitchOldUserId(void)
{
    return oldUserId_;
}
} // namespace DistributedHardware
} // namespace OHOS
