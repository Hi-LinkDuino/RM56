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

import AbilityConstant from "./@ohos.application.AbilityConstant";
import AbilityContext from "./application/AbilityContext";
import Want from './@ohos.application.Want';
import window from './@ohos.window';
import { Configuration } from './@ohos.application.Configuration';
import rpc from '/@ohos.rpc';

/**
 * The prototype of the listener function interface registered by the Caller.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
 * @permission N/A
 * @param msg Monitor status notification information.
 * @return -
 * @StageModelOnly
 */
export interface OnReleaseCallBack {
    (msg: string): void;
}

/**
 * The prototype of the message listener function interface registered by the Callee.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
 * @permission N/A
 * @param indata Notification data notified from the caller.
 * @return rpc.Sequenceable
 * @StageModelOnly
 */
export interface CaleeCallBack {
    (indata: rpc.MessageParcel): rpc.Sequenceable;
}

/**
 * The interface of a Caller.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
 * @permission N/A
 * @StageModelOnly
 */
export interface Caller {
     /**
     * Notify the server of Sequenceable type data.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param method The notification event string listened to by the callee.
     * @param data Notification data to the callee.
     * @return -
     * @StageModelOnly
     */
     call(method: string, data: rpc.Sequenceable): Promise<void>;

    /**
     * Notify the server of Sequenceable type data and return the notification result.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param method The notification event string listened to by the callee.
     * @param data Notification data to the callee.
     * @return Returns the callee's notification result data on success, and returns undefined on failure.
     * @StageModelOnly
     */
     callWithResult(method: string, data: rpc.Sequenceable): Promise<rpc.MessageParcel>;

    /**
     * Clear service records.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @return -
     * @StageModelOnly
     */
     release(): void;

    /**
     * Register death listener notification callback.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param callback Register a callback function for listening for notifications.
     * @return -
     * @StageModelOnly
     */
     onRelease(callback: OnReleaseCallBack): void;
 }

 /**
 * The interface of a Callee.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
 * @permission N/A
 * @StageModelOnly
 */
export interface Callee {

     /**
     * Register data listener callback.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param method A string registered to listen for notification events.
     * @param callback Register a callback function that listens for notification events.
     * @return -
     * @StageModelOnly
     */
     on(method: string, callback: CaleeCallBack): void;

     /**
     * Unregister data listener callback.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param method A string registered to listen for notification events.
     * @return -
     * @StageModelOnly
     */
     off(method: string): void;
 }

/**
 * The class of an ability.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
 * @permission N/A
 * @StageModelOnly
 */
export default class Ability {
    /**
     * Indicates configuration information about an ability context.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @StageModelOnly
     */
    context: AbilityContext;

    /**
     * Indicates ability launch want.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @StageModelOnly
     */
    launchWant: Want;

    /**
     * Indicates ability last request want.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @StageModelOnly
     */
    lastRequestWant: Want;

    /**
     * Call Service Stub Object.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @StageModelOnly
     */
     callee: Callee;

    /**
     * Called back when an ability is started for initialization.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param want Indicates the want info of the created ability.
     * @param param Indicates the launch param.
     * @return -
     * @StageModelOnly
     */
    onCreate(want: Want, param: AbilityConstant.LaunchParam): void;

    /**
     * Called back when an ability window stage is created.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param windowStage Indicates the created WindowStage.
     * @return -
     * @StageModelOnly
     */
    onWindowStageCreate(windowStage: window.WindowStage): void;

    /**
     * Called back when an ability window stage is destroyed.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @return -
     * @StageModelOnly
     */
    onWindowStageDestroy(): void;

    /**
     * Called back when an ability window stage is restored.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param windowStage window stage to restore
     * @return -
     * @StageModelOnly
     */
    onWindowStageRestore(windowStage: window.WindowStage): void;

    /**
     * Called back before an ability is destroyed.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @return -
     * @StageModelOnly
     */
    onDestroy(): void;

    /**
     * Called back when the state of an ability changes to foreground.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @return -
     * @StageModelOnly
     */
    onForeground(): void;

    /**
     * Called back when the state of an ability changes to background.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @return -
     * @StageModelOnly
     */
    onBackground(): void;

    /**
     * Called back when an ability prepares to continue.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param wantParam Indicates the want parameter.
     * @return 0 if ability agrees to continue and saves data successfully, otherwise errcode.
     * @StageModelOnly
     */
     onContinue(wantParam : {[key: string]: any}): AbilityConstant.OnContinueResult;

    /**
     * Called when the launch mode of an ability is set to singleton.
     * This happens when you re-launch an ability that has been at the top of the ability stack.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
     * @param want Indicates the want info of ability.
     * @return -
     * @StageModelOnly
     */
    onNewWant(want: Want): void;

     /**
      * Called when the system configuration is updated.
      *
      * @since 9
      * @syscap SystemCapability.Ability.AbilityRuntime.AbilityCore
      * @param config Indicates the updated configuration.
      * @return -
      * @StageModelOnly
      */
    onConfigurationUpdated(config: Configuration): void;
}
