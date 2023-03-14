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
#include "dm_config_manager.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "pin_auth.h"

namespace OHOS {
namespace DistributedHardware {
DmConfigManager &DmConfigManager::GetInstance()
{
    static DmConfigManager instance;
    return instance;
}

DmConfigManager::DmConfigManager()
{
    LOGI("DmConfigManager constructor");
}

DmConfigManager::~DmConfigManager()
{
    LOGI("DmAdapterManager destructor");
}

std::shared_ptr<IDecisionAdapter> DmConfigManager::GetDecisionAdapter(const std::string &soName)
{
    return nullptr;
}

std::shared_ptr<IProfileAdapter> DmConfigManager::GetProfileAdapter(const std::string &soName)
{
    return nullptr;
}

std::shared_ptr<ICryptoAdapter> DmConfigManager::GetCryptoAdapter(const std::string &soName)
{
    return nullptr;
}

extern "C" IAuthentication *CreatePinAuthObject(void)
{
    return new PinAuth;
}

void DmConfigManager::GetAuthAdapter(std::map<int32_t, std::shared_ptr<IAuthentication>> &authAdapter)
{
    authAdapter.clear();
    std::shared_ptr<IAuthentication> iAuthentication(CreatePinAuthObject());
    authAdapter[AUTH_TYPE_PIN] = iAuthentication;
}
} // namespace DistributedHardware
} // namespace OHOS