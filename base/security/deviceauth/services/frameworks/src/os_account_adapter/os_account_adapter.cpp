/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "os_account_adapter.h"

#include <vector>
#include "device_auth.h"
#include "hc_log.h"
#ifdef SUPPORT_OS_ACCOUNT
#include "os_account_manager.h"
#endif

int32_t DevAuthGetRealOsAccountLocalId(int32_t inputId)
{
    if (inputId == ANY_OS_ACCOUNT) {
#ifdef SUPPORT_OS_ACCOUNT
        std::vector<int> activatedOsAccountIds;
        LOGI("[OsAccountManager][In]: QueryActiveOsAccountIds!");
        OHOS::ErrCode res = OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
        LOGI("[OsAccountManager][Out]: QueryActiveOsAccountIds! res: %d", res);
        if ((res != OHOS::ERR_OK) || (activatedOsAccountIds.size() <= 0)) {
            LOGE("[Account]: QueryActiveOsAccountIds fail! res: %d", res);
            return INVALID_OS_ACCOUNT;
        }
        int osAccountId = activatedOsAccountIds[0];
        LOGI("[Account]: Use activated os account! [Id]: %d", osAccountId);
        return osAccountId;
#else
        return 0;
#endif
    } else if (inputId >= 0) {
        LOGI("[Account]: Use input os account! [Id]: %d", inputId);
        return inputId;
    } else {
        LOGE("[Account]: The input os account is invalid! [Id]: %d", inputId);
        return INVALID_OS_ACCOUNT;
    }
}
