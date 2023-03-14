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
 * You can use this class to obtain values of the fields set in an element,
 * such as the device ID, bundle name, and ability name.
 * @name Contains basic Ability information, which uniquely identifies an ability
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 *
 * @permission N/A
 */
 export interface ElementName {
  /**
   * device id
   * @default -
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   */
  deviceId?: string;

  /**
   * bundle name
   * @default -
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   */
  bundleName: string;

  /**
   * ability name
   * @default ability class name.
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   */
  abilityName: string;

  /**
   * uri
   * @default -
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   */
  uri?: string;

  /**
   * shortName
   * @default -
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   */
  shortName?: string;
}
