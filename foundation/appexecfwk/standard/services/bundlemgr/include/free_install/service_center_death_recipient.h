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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_DEATH_RECIPIENT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_DEATH_RECIPIENT_H

#include <map>
#include <string>

#include "free_install_params.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {
class ServiceCenterDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    ServiceCenterDeathRecipient(std::map<std::string, FreeInstallParams> &freeInstallParamsMap)
        : freeInstallParamsMap_(freeInstallParamsMap)
    {
    }
    void OnRemoteDied(const wptr<IRemoteObject> &wptrDeath) override;

private:
    /**
     * @brief Send atomic service status callback to ability manager service.
     * @param freeInstallParams Relevant information sent to ability manager service.
     * @param transactId The key of ability manager service Call Back Map.
     */
    void SendCallBack(FreeInstallParams &freeInstallParams, std::string transactId);
    std::map<std::string, FreeInstallParams> &freeInstallParamsMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_DEATH_RECIPIENT_H