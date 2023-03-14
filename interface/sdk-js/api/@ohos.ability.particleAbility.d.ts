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

import { AsyncCallback } from './basic';
import { StartAbilityParameter } from './ability/startAbilityParameter';
import { DataAbilityHelper } from './ability/dataAbilityHelper';
import { NotificationRequest } from './notification/notificationRequest';
import { ConnectOptions } from './ability/connectOptions';
import { Want } from './ability/want';

/**
 * A Particle Ability represents an ability with service.
 * @name particleAbility
 * @since 7
 * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
 * @permission N/A
 * @FAModelOnly
 */
declare namespace particleAbility {
  /**
   * Service ability uses this method to start a specific ability.
   *
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
   * @param parameter Indicates the ability to start.
   * @return -
   * @FAModelOnly
   */
  function startAbility(parameter: StartAbilityParameter, callback: AsyncCallback<void>): void;
  function startAbility(parameter: StartAbilityParameter): Promise<void>;

  /**
   * Destroys this service ability.
   *
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
   * @return -
   * @FAModelOnly
   */
  function terminateSelf(callback: AsyncCallback<void>): void;
  function terminateSelf(): Promise<void>;

  /**
   * Obtains the dataAbilityHelper.
   *
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
   * @param uri Indicates the path of the file to open.
   * @return Returns the dataAbilityHelper.
   * @FAModelOnly
   */
  function acquireDataAbilityHelper(uri: string): DataAbilityHelper;

  /**
   * Keep this Service ability in the background and display a notification bar.
   *
   * @since 7
   * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
   * @permission ohos.permission.KEEP_BACKGROUND_RUNNING
   * @param id Identifies the notification bar information.
   * @param request Indicates the notificationRequest instance containing information for displaying a notification bar.
   * @FAModelOnly
   * @deprecated
   */
  function startBackgroundRunning(id: number, request: NotificationRequest, callback: AsyncCallback<void>): void;
  function startBackgroundRunning(id: number, request: NotificationRequest): Promise<void>;

  /**
   * Cancel background running of this ability to free up system memory.
   *
   * @since 7
   * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
   * @FAModelOnly
   * @deprecated
   */
  function cancelBackgroundRunning(callback: AsyncCallback<void>): void;
  function cancelBackgroundRunning(): Promise<void>;

  /**
   * Connects an ability to a Service ability.
   *
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
   * @param request Indicates the Service ability to connect.
   * @param options Callback object for the client. If this parameter is null, an exception is thrown.
   * @return unique identifier of the connection between the client and the service side.
   * @FAModelOnly
   */
   function connectAbility(request: Want, options:ConnectOptions): number;

  /**
  * Disconnects ability to a Service ability.
  * @since 7
  * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
  * @param connection the connection id returned from connectAbility api.
  * @FAModelOnly
  */
   function disconnectAbility(connection: number, callback:AsyncCallback<void>): void;
   function disconnectAbility(connection: number): Promise<void>;

  /**
  * Obtain the errorCode.
  *
  * @since 7
  * @syscap SystemCapability.Ability.AbilityRuntime.FAModel
  * @FAModelOnly
  */
  export enum ErrorCode {
    INVALID_PARAMETER = -1
  }
}
export default particleAbility;
