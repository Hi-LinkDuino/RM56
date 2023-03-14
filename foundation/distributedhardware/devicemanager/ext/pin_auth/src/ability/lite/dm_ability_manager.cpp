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

#include "dm_ability_manager.h"

namespace OHOS {
namespace DistributedHardware {
AbilityRole DmAbilityManager::GetAbilityRole()
{
    return mAbilityStatus_;
}

AbilityStatus DmAbilityManager::StartAbility(AbilityRole role)
{
    // not support for L1 yet, do nothing. just save status and role
    mAbilityStatus_ = role;
    mStatus_ = AbilityStatus::ABILITY_STATUS_SUCCESS;
    return mStatus_;
}

void DmAbilityManager::WaitForTimeout(uint32_t timeout_s)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (int32_t)timeout_s;
    sem_timedwait(&mSem_, &ts);
}

void DmAbilityManager::StartAbilityDone()
{
    mStatus_ = AbilityStatus::ABILITY_STATUS_SUCCESS;
    sem_post(&mSem_);
}
} // namespace DistributedHardware
} // namespace OHOS
