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

import { AbilityInfo } from './abilityInfo';
import { ApplicationInfo } from './applicationInfo';
import { ExtensionAbilityInfo } from './extensionAbilityInfo';
import { HapModuleInfo } from './hapModuleInfo';

/**
 * @name The scene which is used
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 *
 */
export interface UsedScene {
  /**
    * @default Indicates the abilities that need the permission
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  abilities: Array<string>;

  /**
    * @default Indicates the time when the permission is used
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  when: string;
}

/**
 * @name Indicates the required permissions details defined in file config.json
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 *
 */
export interface ReqPermissionDetail {
  /**
    * @default Indicates the name of this required permissions
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  name: string;

  /**
    * @default Indicates the reason of this required permissions
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  reason: string;

  /**
    * @default Indicates the used scene of this required permissions
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  usedScene: UsedScene;
}

/**
 * @name Obtains configuration information about a bundle
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 *
 */
export interface BundleInfo {
  /**
    * @default Indicates the name of this bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly name: string;

  /**
    * @default Indicates the name of this original bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly type: string;

  /**
    * @default Indicates the ID of the application to which this bundle belongs
    * The application ID uniquely identifies an application. It is determined by the bundle name and signature
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly appId: string;

  /**
    * @default Indicates the UID of the application to which this bundle belongs
    * The UID uniquely identifies an application. It is determined by the process and user IDs of the application
    * After an application is installed, its UID remains unchanged unless it is uninstalled and then reinstalled
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly uid: number;

  /**
    * @default Indicates the hap install time
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly installTime: number;

  /**
    * @default Indicates the hap update time
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly updateTime: number;

  /**
    * @default Obtains configuration information about an application
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly appInfo: ApplicationInfo;

  /**
    * @default Obtains configuration information about an ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly abilityInfos: Array<AbilityInfo>;

  /**
    * @default Indicates the required permissions name defined in file config.json
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly reqPermissions: Array<string>;

  /**
    * @default Indicates the required permissions details defined in file config.json
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly reqPermissionDetails: Array<ReqPermissionDetail>;

  /**
    * @default Describes the bundle vendor
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly vendor: string;

  /**
    * @default Indicates the version number of the bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly versionCode: number;

  /**
    * @default Indicates the text description of the bundle version
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly versionName: string;

  /**
    * @default Indicates the compatible version number of the bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly compatibleVersion: number;

  /**
    * @default Indicates the target version number of the bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly targetVersion: number;

  /**
    * @default Indicates is compress native libs
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly isCompressNativeLibs: boolean;

  /**
    * @default Obtains configuration information about an module
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly hapModuleInfos: Array<HapModuleInfo>;

  /**
    * @default Indicates entry module name
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly entryModuleName: string;

  /**
    * @default Indicates the cpuAbi information of this bundle.
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly cpuAbi: string;

  /**
    * @default Indicates is silent installation
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly isSilentInstallation: string;

  /**
    * @default Indicates the earliest historical version compatible with the bundle
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly minCompatibleVersionCode: number;

  /**
    * @default Indicates whether free installation of the entry is supported
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly entryInstallationFree: boolean;

  /**
    * @default Indicates the grant status of required permissions
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly reqPermissionStates: Array<number>;

  /**
    * @default Obtains configuration information about an ability
    * @since 9
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly extensionAbilityInfo: Array<ExtensionAbilityInfo>;
}
