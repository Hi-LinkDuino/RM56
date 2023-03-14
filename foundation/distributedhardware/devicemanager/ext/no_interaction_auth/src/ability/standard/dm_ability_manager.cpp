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

#include "ability_manager_client.h"
#include "ability_manager_service.h"
#include "ability_record.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "parameter.h"
#include "semaphore.h"

namespace OHOS {
namespace DistributedHardware {
namespace {
const int32_t ABILITY_START_TIMEOUT = 3; // 3 second
const std::string bundleUiName = "com.ohos.devicemanagerui";
const std::string abilityUiName = "com.ohos.devicemanagerui.MainAbility";
} // namespace

AbilityRole DmAbilityManager::GetAbilityRole()
{
    return mAbilityStatus_;
}

AbilityStatus DmAbilityManager::StartAbility(AbilityRole role)
{
    mAbilityStatus_ = role;
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    std::string deviceId = localDeviceId;
    mStatus_ = ABILITY_STATUS_START;
    AAFwk::Want want;
    AppExecFwk::ElementName element(deviceId, bundleUiName, abilityUiName);
    want.SetElement(element);
    AAFwk::AbilityManagerClient::GetInstance()->Connect();
    ErrCode result = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (result != OHOS::ERR_OK) {
        LOGE("Start Ability failed");
        mStatus_ = ABILITY_STATUS_FAILED;
        return mStatus_;
    }
    WaitForTimeout(ABILITY_START_TIMEOUT);
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
    mStatus_ = ABILITY_STATUS_SUCCESS;
    sem_post(&mSem_);
}
} // namespace DistributedHardware
} // namespace OHOS
