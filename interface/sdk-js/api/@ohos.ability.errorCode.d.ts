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

 /**
  * Defines error codes used when starting an ability, for example, featureAbility.ErrorCode.NO_ERROR.
  * @since 6
  * @syscap SystemCapability.Ability.AbilityRuntime.Core
  */
export enum ErrorCode {
  /**
   * Permission denied.
   * @since 6
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   */
  PERMISSION_DENY = -3,

  /**
   * Ability not found.
   * @since 6
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   */
  ABILITY_NOT_FOUND = -2,

  /**
   * Invalid parameter.
   * @since 6
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   */
  INVALID_PARAMETER = -1,

  /**
   * No error.
   * @since 6
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   */
  NO_ERROR = 0
}
