/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_ability.h"

#include "hks_log.h"

static struct HksAbility g_abilityList[HKS_ABILITY_MAX_SIZE] = {0};

int32_t RegisterAbility(uint32_t id, void *func)
{
    for (int i = 0; i < HKS_ABILITY_MAX_SIZE; i++) {
        if (g_abilityList[i].id == id) {
            return HKS_ERROR_ALREADY_EXISTS;
        } else if (g_abilityList[i].id != 0) {
            continue;
        }
        g_abilityList[i].id = id;
        g_abilityList[i].func = func;
        HKS_LOG_I("register ability i = %d, id = 0x%x", i, id);
        return HKS_SUCCESS;
    }
    HKS_LOG_E("register failed: exceed max number of abilities, id = 0x%x", id);
    return HKS_ERROR_BAD_STATE;
}

void *GetAbility(uint32_t id)
{
    for (int i = 0; i < HKS_ABILITY_MAX_SIZE; i++) {
        if (g_abilityList[i].id == id) {
            return g_abilityList[i].func;
        }
    }
    return NULL;
}