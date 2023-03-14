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

#ifndef OHOS_ABILITY_MANAGER_H
#define OHOS_ABILITY_MANAGER_H

#include "want.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Starts an ability based on the specified {@link Want} information.
 *
 * @param want Indicates the pointer to the {@link Want} structure containing information about the ability to start.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int StartAbility(const Want *want);

/**
 * @brief Terminate an ability based on the specified token information.
 *
 * @param token Indicates the token of the ability.
 * @return Returns <b>0</b> if this function is successfully called; returns another value otherwise.
 */
int TerminateAbility(uint64_t token);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OHOS_ABILITY_MANAGER_H */
