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

#ifndef OHOS_BUNDLE_MANAGER_INNER_H
#define OHOS_BUNDLE_MANAGER_INNER_H

#include "ability_info.h"
#include "slite_ability.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Install a simulated bundle for the native ability .
 *
 * @param abilityInfo Indicates the info of the ability.
 * @param ability Indicates the object of the ability.
 * @return Returns the install result.
 *
 * @since 4.0
 * @version 4.0
 */
int InstallNativeAbility(const AbilityInfo *abilityInfo, const OHOS::SliteAbility *ability);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OHOS_BUNDLE_MANAGER_INNER_H */
/** @} */