/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback } from '../basic';
import { ApplicationInfo } from '../bundle/applicationInfo';
import { ProcessInfo } from './processInfo';
import { ElementName } from '../bundle/elementName';
import BaseContext from '../application/BaseContext';
import { HapModuleInfo } from '../bundle/hapModuleInfo';
import { AppVersionInfo } from './appVersionInfo';
import { AbilityInfo } from '../bundle/abilityInfo';
import { DisplayOrientation } from '../@ohos.bundle';


/**
 * The context of an ability or an application.  It allows access to
 * application-specific resources, request and verification permissions.
 * Can only be obtained through the ability.
 *
 * @since 6
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @import import abilityManager from 'app/context'
 * @permission N/A
 * @FAModelOnly
 */
export interface Context extends BaseContext {

    /**
    * Get the local root dir of an app. If it is the first call, the dir
    * will be created.
    * @note If in the context of the ability, return the root dir of
    * the ability; if in the context of the application, return the
    * root dir of the application.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @return the root dir
    * @FAModelOnly
    */
    getOrCreateLocalDir(): Promise<string>;
    getOrCreateLocalDir(callback: AsyncCallback<string>): void;
    /**
    * Verify whether the specified permission is allowed for a particular
    * pid and uid running in the system.
    * @param permission The name of the specified permission
    * @param pid process id
    * @param uid user id
    * @note Pid and uid are optional. If you do not pass in pid and uid,
    * it will check your own permission.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @return asynchronous callback with {@code 0} if the PID
    *         and UID have the permission; callback with {@code -1} otherwise.
    * @FAModelOnly
    */
    verifyPermission(permission: string, options?: PermissionOptions): Promise<number>;
    verifyPermission(permission: string, options: PermissionOptions, callback: AsyncCallback<number>): void;
    verifyPermission(permission: string, callback: AsyncCallback<number>): void;

    /**
    * Requests certain permissions from the system.
    * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
    * @param requestCode Indicates the request code to be passed to the PermissionRequestResult
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    requestPermissionsFromUser(permissions: Array<string>, requestCode: number, resultCallback: AsyncCallback<PermissionRequestResult>): void;
    requestPermissionsFromUser(permissions: Array<string>, requestCode: number): Promise<PermissionRequestResult>;

    /**
    * Obtains information about the current application.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getApplicationInfo(callback: AsyncCallback<ApplicationInfo>): void
    getApplicationInfo(): Promise<ApplicationInfo>;

    /**
    * Obtains the bundle name of the current ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getBundleName(callback: AsyncCallback<string>): void
    getBundleName(): Promise<string>;

    /**
    * Obtains the current display orientation of this ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    */
    getDisplayOrientation(callback: AsyncCallback<DisplayOrientation>): void
    getDisplayOrientation(): Promise<DisplayOrientation>;
    
    /**
    * Sets the display orientation of the current ability.
    * @param orientation Indicates the new orientation for the current ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    */
    setDisplayOrientation(orientation: DisplayOrientation, callback: AsyncCallback<void>): void
    setDisplayOrientation(orientation: DisplayOrientation): Promise<void>;
    
    /**
    * Sets whether to show this ability on top of the lock screen whenever the lock screen is displayed, keeping the ability in the ACTIVE state.
    * @param show Specifies whether to show this ability on top of the lock screen. The value true means to show it on the lock screen, and the value false means not.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    */
    setShowOnLockScreen(show: boolean, callback: AsyncCallback<void>): void
    setShowOnLockScreen(show: boolean): Promise<void>;
    
    /**
    * Sets whether to wake up the screen when this ability is restored.
    * @param wakeUp Specifies whether to wake up the screen. The value true means to wake it up, and the value false means not.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    */
    setWakeUpScreen(wakeUp: boolean, callback: AsyncCallback<void>): void
    setWakeUpScreen(wakeUp: boolean): Promise<void>;  

    /**
    * Obtains information about the current process, including the process ID and name.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getProcessInfo(callback: AsyncCallback<ProcessInfo>): void
    getProcessInfo(): Promise<ProcessInfo>;

    /**
    * Obtains the ohos.bundle.ElementName object of the current ability. This method is available only to Page abilities.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getElementName(callback: AsyncCallback<ElementName>): void
    getElementName(): Promise<ElementName>;

    /**
    * Obtains the name of the current process.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getProcessName(callback: AsyncCallback<string>): void
    getProcessName(): Promise<string>;

    /**
    * Obtains the bundle name of the ability that called the current ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getCallingBundle(callback: AsyncCallback<string>): void
    getCallingBundle(): Promise<string>;

    /**
    * Obtains the file directory of this application on the internal storage.
    * @since 6
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getFilesDir(callback: AsyncCallback<string>): void;
    getFilesDir(): Promise<string>;

    /**
    * Obtains the cache directory of this application on the internal storage.
    * @since 6
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getCacheDir(callback: AsyncCallback<string>): void;
    getCacheDir(): Promise<string>;

    /**
    * Obtains the distributed file path for storing ability or application data files.
    * If the distributed file path does not exist, the system will create a path and return the created path.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getOrCreateDistributedDir(): Promise<string>;
    getOrCreateDistributedDir(callback: AsyncCallback<string>): void;

    /**
    * Obtains the application type.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getAppType(callback: AsyncCallback<string>): void
    getAppType(): Promise<string>;

    /**
    * Obtains the ModuleInfo object for this application.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getHapModuleInfo(callback: AsyncCallback<HapModuleInfo>): void
    getHapModuleInfo(): Promise<HapModuleInfo>;

    /**
    * Obtains the application version information.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getAppVersionInfo(callback: AsyncCallback<AppVersionInfo>): void
    getAppVersionInfo(): Promise<AppVersionInfo>;

    /**
    * Obtains the context of this application.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getApplicationContext(): Context;

    /**
    * Checks the detailed information of this ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    getAbilityInfo(callback: AsyncCallback<AbilityInfo>): void
    getAbilityInfo(): Promise<AbilityInfo>;

    /**
    * Checks whether the configuration of this ability is changing.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @return true if the configuration of this ability is changing and false otherwise.
    * @FAModelOnly
    */
    isUpdatingConfigurations(callback: AsyncCallback<boolean>): void;
    isUpdatingConfigurations(): Promise<boolean>;

    /**
    * Informs the system of the time required for drawing this Page ability.
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    printDrawnCompleted(callback: AsyncCallback<void>): void;
    printDrawnCompleted(): Promise<void>;
}

/**
 * @name the result of requestPermissionsFromUser with asynchronous callback
 * @since 7
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 * @FAModelOnly
 */
interface PermissionRequestResult {
    /**
    * @default The request code passed in by the user
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    requestCode: number;

    /**
    * @default The permissions passed in by the user
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    permissions: Array<string>;

    /**
    * @default The results for the corresponding request permissions
    * @since 7
    * @syscap SystemCapability.Ability.AbilityRuntime.Core
    * @FAModelOnly
    */
    authResults: Array<number>;
}

/**
 * @name PermissionOptions
 * @since 7
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 * @FAModelOnly
 */
interface PermissionOptions {
 /**
  * @default The process id
  * @since 7
  * @syscap SystemCapability.Ability.AbilityRuntime.Core
  * @FAModelOnly
  */
 pid?: number;

 /**
  * @default The user id
  * @since 7
  * @syscap SystemCapability.Ability.AbilityRuntime.Core
  * @FAModelOnly
  */
 uid?: number;
}
