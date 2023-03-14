/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "slite_ability.h"

#include "ability_state.h"
#include "abilityms_client.h"

namespace OHOS {
void SliteAbility::OnStart(const Want &want)
{
    abilityState_ = STATE_INITIAL;
    (void) AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, STATE_INITIAL);
}

void SliteAbility::OnInactive()
{
    abilityState_ = STATE_INACTIVE;
    (void) AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, STATE_INACTIVE);
}

void SliteAbility::OnActive(const Want &want)
{
    abilityState_ = STATE_ACTIVE;
    (void) AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, STATE_ACTIVE);
}

void SliteAbility::OnBackground()
{
    abilityState_ = STATE_BACKGROUND;
    (void) AbilityMsClient::GetInstance().SchedulerLifecycleDone(token_, STATE_BACKGROUND);
}

int SliteAbility::GetState() const
{
    return abilityState_;
}
} // namespace OHOS
