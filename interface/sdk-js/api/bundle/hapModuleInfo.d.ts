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

import { AbilityInfo } from "./abilityInfo";
import { ExtensionAbilityInfo } from "./extensionAbilityInfo";
import { Metadata } from './metadata'

/**
 * @name Obtains configuration information about an module.
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 *
 */
export interface HapModuleInfo {
  /**
    * @default Indicates the name of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly name: string;
  /**
    * @default Describes the hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly description: string;
  /**
    * @default Indicates the description of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly descriptionId: number;
  /**
    * @default Indicates the icon of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly icon: string;
  /**
    * @default Indicates the label of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly label: string;
  /**
    * @default Indicates the label id of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly labelId: number;
  /**
    * @default Indicates the icon id of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly iconId: number;
  /**
    * @default Indicates the background img of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly backgroundImg: string;
  /**
    * @default Indicates the supported modes of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly supportedModes: number;
  /**
    * @default Indicates the req capabilities of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly reqCapabilities: Array<string>;
  /**
    * @default The device types that this hapmodule can run on
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly deviceTypes: Array<string>;
  /**
    * @default Obtains configuration information about ability
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly abilityInfo: Array<AbilityInfo>;
  /**
    * @default Indicates the name of the .hap package to which the capability belongs
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly moduleName: string;
  /**
    * @default Indicates the main ability name of this hapmodule
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly mainAbilityName: string;
  /**
    * @default Indicates whether free installation of the hapmodule is supported
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly installationFree: boolean;

  /**
    * @default Indicates main elementName of the module
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly mainElementName: string;

  /**
    * @default Obtains configuration information about extension ability
    * @since 9
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  readonly extensionAbilityInfo: Array<ExtensionAbilityInfo>;
  /**
    * @default Indicates the metadata of ability
    * @since 9
    * @syscap SystemCapability.BundleManager.BundleFramework
    * 
    */
  readonly metadata: Array<Metadata>;
}