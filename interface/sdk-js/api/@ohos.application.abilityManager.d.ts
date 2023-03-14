/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import { AsyncCallback } from './basic';
import { Configuration } from './@ohos.application.Configuration';
import { AbilityRunningInfo } from './application/AbilityRunningInfo';
import { ExtensionRunningInfo } from './application/ExtensionRunningInfo';

/**
 * The class of an ability manager.
 *
 * @since 8
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 */
declare namespace abilityManager {

    /**
     * @name AbilityState
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @systemapi Hide this for inner system use.
     * @permission N/A
     */
    export enum AbilityState {
        INITIAL = 0,
        FOREGROUND = 9,
        BACKGROUND = 10,
        FOREGROUNDING = 11,
        BACKGROUNDING = 12
    }

    /**
     * Updates the configuration by modifying the configuration.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param config Indicates the new configuration.
     * @systemapi Hide this for inner system use.
     * @return -
     * @permission ohos.permission.UPDATE_CONFIGURATION
     */
    function updateConfiguration(config: Configuration, callback: AsyncCallback<void>): void;
    function updateConfiguration(config: Configuration): Promise<void>;

    /**
     * Get information about running abilitys
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @systemapi Hide this for inner system use.
     * @return Returns the array of {@link AbilityRunningInfo}.
     * @permission ohos.permission.GET_RUNNING_INFO
     */
    function getAbilityRunningInfos(): Promise<Array<AbilityRunningInfo>>;
    function getAbilityRunningInfos(callback: AsyncCallback<Array<AbilityRunningInfo>>): void;

     /**
      * Get information about running extensions
      *
      * @since 9
      * @syscap SystemCapability.Ability.AbilityRuntime.Core
      * @param upperLimit Get the maximum limit of the number of messages
      * @systemapi Hide this for inner system use.
      * @return Returns the array of {@link ExtensionRunningInfo}.
      * @permission ohos.permission.GET_RUNNING_INFO
      */
    function getExtensionRunningInfos(upperLimit: number): Promise<Array<ExtensionRunningInfo>>;
    function getExtensionRunningInfos(upperLimit: number, callback: AsyncCallback<Array<ExtensionRunningInfo>>): void;
}

export default abilityManager;