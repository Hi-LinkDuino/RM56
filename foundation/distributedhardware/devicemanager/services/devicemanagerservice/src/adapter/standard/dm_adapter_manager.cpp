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

#include "dm_adapter_manager.h"

#include "dm_config_manager.h"
#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
DmAdapterManager &DmAdapterManager::GetInstance()
{
    static DmAdapterManager instance;
    return instance;
}

std::shared_ptr<IDecisionAdapter> DmAdapterManager::GetDecisionAdapter(const std::string &soName)
{
    DmConfigManager &dmConfigManager = DmConfigManager::GetInstance();
    return dmConfigManager.GetDecisionAdapter(soName);
}

std::shared_ptr<IProfileAdapter> DmAdapterManager::GetProfileAdapter(const std::string &soName)
{
    DmConfigManager &dmConfigManager = DmConfigManager::GetInstance();
    return dmConfigManager.GetProfileAdapter(soName);
}

std::shared_ptr<ICryptoAdapter> DmAdapterManager::GetCryptoAdapter(const std::string &soName)
{
    DmConfigManager &dmConfigManager = DmConfigManager::GetInstance();
    return dmConfigManager.GetCryptoAdapter(soName);
}
} // namespace DistributedHardware
} // namespace OHOS
