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

#ifndef OHOS_ABILITY_MANAGER_INNER_H
#define OHOS_ABILITY_MANAGER_INNER_H

#include "stdint.h"

#include "element_name.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef int (*StartCheckFunc)(const char *bundleName);

/**
 * @brief Register the check function for the ability starting.
 *
 * @param startChecktCallback Indicates the check function for the ability starting.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int RegAbilityCallback(StartCheckFunc startChecktCallback);

/**
 * @brief Schedule the lifecycle of the ability.
 *
 * @param token Indicates the token of the ability.
 * @param state Indicates the state of the lifecycle.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int SchedulerLifecycleDone(uint64_t token, int state);

/**
 * @brief Forcestop an ability based on the specified token information.
 *
 * @param token Indicates the token of the ability.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int ForceStopBundle(uint64_t token);

/**
 * @brief Get the element name of the top ability.
 *
 * @return Returns the element name of the top ability.
 */
ElementName *GetTopAbility();

/**
 * @brief Forcestop an ability based on the specified bundlename information.
 *
 * @param bundlename Indicates the bundlename of the ability.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int ForceStop(char *bundlename);

/**
 * @brief get ability callback function.
 *
 * @return Returns the ability callback function.
 */
StartCheckFunc getAbilityCallback(void);

void setCleanAbilityDataFlag(bool cleanFlag);

bool getCleanAbilityDataFlag();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif // OHOS_ABILITY_MANAGER_INNER_H
