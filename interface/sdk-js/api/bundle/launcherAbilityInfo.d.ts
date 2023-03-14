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

/**
 * Contains basic Ability information, which uniquely identifies an ability.
 * You can use this class to obtain values of the fields set in an AbilityInfo,
 * such as the application Info , elementName, labelId, iconId, userId, installTime.
 * @name Contains basic launcher Ability information, which uniquely identifies an LauncherAbilityInfo
 * @since 8
 * @syscap SystemCapability.BundleManager.BundleFramework
 *
 * @permission N/A
 * @systemapi Hide this for inner system use
 */

import { ApplicationInfo } from './applicationInfo';
import { ElementName } from './elementName'

export interface LauncherAbilityInfo {
  /**
   * @name Obtains application info information about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly applicationInfo: ApplicationInfo;

  /**
   * @name Obtains element name about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly elementName : ElementName;

  /**
   * @name Obtains labelId about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly labelId: number;

  /**
   * @name Obtains iconId about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly iconId: number;

  /**
   * @name Obtains userId about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly userId: number;

  /**
   * @name Obtains installTime about an launcher ability.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission NA
   *
   */
  readonly installTime : number;
}