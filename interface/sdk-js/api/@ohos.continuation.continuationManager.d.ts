/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import { Callback } from './basic';
import { AsyncCallback } from './basic';
import { ContinuationResult } from './continuation/continuationResult'
import { ContinuationExtraParams } from './continuation/continuationExtraParams'

/**
 * Provides methods for interacting with the continuation manager servcie, including methods for registering and
 * unregistering the ability to hop, updating the device connection state, and showing the list of devices
 * that can be selected for hopping.
 * @name continuationManager
 * @since 8
 * @syscap SystemCapability.Ability.DistributedAbilityManager
 * @permission N/A
 */
declare namespace continuationManager {
    /**
     * Called when the user selects a device from the candidate device list.
     * You can implement your own processing logic in this callback to initiate the hop process.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param type deviceConnect.
     * @return callback Indicates the information about the selected device.
     */
    function on(type: "deviceConnect", callback: Callback<ContinuationResult>): void;
    function off(type: "deviceConnect", callback?: Callback<ContinuationResult>): void;

    /**
     * Called when a device is disconnected from the continuation manager servcie.
     * You can implement your own processing logic in this callback, such as notifying the user of the disconnection.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param type deviceDisconnect.
     * @return callback Indicates the ID of the disconnected device.
     */
    function on(type: "deviceDisconnect", callback: Callback<string>): void;
    function off(type: "deviceDisconnect", callback?: Callback<string>): void;

    /**
     * Registers an ability to be hopped with the continuation manager servcie and obtains the registration token
     * assigned to the ability.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param options Indicates the {@link ExtraParams} object containing the extra parameters used to filter
     * the list of available devices.
     * @return callback Indicates the callback to be invoked when the continuation manager servcie is connected.
     */
    function register(callback: AsyncCallback<number>): void;
    function register(options: ContinuationExtraParams, callback: AsyncCallback<number>): void;
    function register(options?: ContinuationExtraParams): Promise<number>;

    /**
     * Unregisters a specified ability from the continuation manager servcie based on the token obtained during ability
     * registration.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param token Indicates the registration token of the ability.
     * @return callback Indicates the callback to be invoked when the continuation manager servcie is connected.
     */
    function unregister(token: number, callback: AsyncCallback<void>): void;
    function unregister(token: number): Promise<void>;

    /**
     * Updates the connection state of the device where the specified ability is successfully hopped.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param token Indicates the registration token of the ability.
     * @param deviceId Indicates the ID of the device whose connection state is to be updated.
     * @param status Indicates the connection state to update.
     * @return callback Indicates the callback to be invoked when the continuation manager servcie is connected.
     */
    function updateConnectStatus(token: number, deviceId: string, status: DeviceConnectState, callback: AsyncCallback<void>): void;
    function updateConnectStatus(token: number, deviceId: string, status: DeviceConnectState): Promise<void>;

    /**
     * Start to manage the devices that can be selected for continuation on the distributed network.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     * @param token Indicates the registration token of the ability.
     * @param options Indicates the extraParams object containing the extra parameters used to filter
     * the list of available devices. This parameter can be null.
     * @return callback Indicates the callback to be invoked when the continuation manager servcie is connected.
     */
    function startDeviceManager(token: number, callback: AsyncCallback<void>): void;
    function startDeviceManager(token: number, options: ContinuationExtraParams, callback: AsyncCallback<void>): void;
    function startDeviceManager(token: number, options?: ContinuationExtraParams): Promise<void>;

    /**
     * Device connection status data structure.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    export enum DeviceConnectState {
        IDLE = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        DISCONNECTING = 3
    }

    /**
     * Indicates the description of additional parameters for continuation.
     * 
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    export enum ContinuationMode {

        /**
         * Collaboration with a single device.
         */
        COLLABORATION_SINGLE = 0,

        /**
         * Collaboration with multiple devices.
         */
        COLLABORATION_MULTIPLE = 1,
    }

}
export default continuationManager;