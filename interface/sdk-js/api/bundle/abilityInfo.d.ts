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

import { ApplicationInfo } from './applicationInfo';
import { CustomizeData } from './customizeData'
import { Metadata } from './metadata'
import bundle from './../@ohos.bundle';

/**
 * @name Obtains configuration information about an ability
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 */
export interface AbilityInfo {
  /**
    * @default Indicates the name of the bundle containing the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly bundleName: string;

  /**
    * @default Ability simplified class name
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly name: string;

  /**
    * @default Indicates the label of the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly label: string;

  /**
    * @default Describes the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly description: string;

  /**
    * @default Indicates the icon of the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly icon: string;

  /**
    * @default Indicates the label id of the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly labelId: number;

  /**
    * @default Indicates the description id of the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly descriptionId: number;

  /**
    * @default Indicates the icon id of the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly iconId: number;

  /**
    * @default Indicates the name of the .hap package to which the capability belongs
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly moduleName: string;

  /**
    * @default Process of ability, if user do not set it ,the value equal application process
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly process: string;

  /**
    * @default Info about which ability is this nick point to
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly targetAbility: string;

  /**
    * @default Indicates the background service addressing a specific usage scenario
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly backgroundModes: number;

  /**
    * @default Indicates whether an ability can be called by other abilities
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly isVisible: boolean;

  /**
    * @default Indicates whether the ability provides the embedded card capability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly formEnabled: boolean;

  /**
    * @default Enumerates types of templates that can be used by an ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly type: bundle.AbilityType;

  /**
    * @default Enumerates the subType of templates used by an ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly subType: bundle.AbilitySubType;

  /**
    * @default Enumerates ability display orientations
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly orientation: bundle.DisplayOrientation;

  /**
    * @default Enumerates ability launch modes
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly launchMode: bundle.LaunchMode;

  /**
    * @default The permissions that others need to launch this ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly permissions: Array<string>;

  /**
    * @default The device types that this ability can run on
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly deviceTypes: Array<string>;

  /**
    * @default The device capability that this ability needs
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly deviceCapabilities: Array<string>;

  /**
    * @default Indicates the permission required for reading ability data
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly readPermission: string;

  /**
    * @default Indicates the permission required for writing data to the ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly writePermission: string;

  /**
    * @default Obtains configuration information about an application
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly applicationInfo: ApplicationInfo;

  /**
    * @default Uri of ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    * @FAModelOnly
    */
  readonly uri: string;

  /**
    * @default Indicates the metadata of ability
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly metaData: Array<CustomizeData>;

  /**
    * @default Indicates the metadata of ability
    * @since 9
    * @syscap SystemCapability.BundleManager.BundleFramework
    * 
    */
  readonly metadata: Array<Metadata>;

  /**
    * @default Indicates whether the ability is enabled
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
   readonly enabled: boolean;
}
