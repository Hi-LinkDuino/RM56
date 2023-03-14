/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from "./basic";

/**
 * Provides methods to operate or manage Bluetooth.
 * @since 7
 * @import import bluetooth frome '@ohos.bluetooth';
 * @syscap SystemCapability.Communication.Bluetooth.Core
 */
declare namespace bluetooth {
    /**
     * Obtains the Bluetooth status of a device.
     *
     * @return Returns the Bluetooth status, which can be {@link BluetoothState#STATE_OFF},
     * {@link BluetoothState#STATE_TURNING_ON}, {@link BluetoothState#STATE_ON}, {@link BluetoothState#STATE_TURNING_OFF},
     * {@link BluetoothState#STATE_BLE_TURNING_ON}, {@link BluetoothState#STATE_BLE_ON},
     * or {@link BluetoothState#STATE_BLE_TURNING_OFF}.
     * @since 7
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getState(): BluetoothState;

    /**
     * Get the local device connection state to any profile of any remote device.
     *
     * @return One of {@link ProfileConnectionState#STATE_DISCONNECTED},
     * {@link ProfileConnectionState#STATE_CONNECTING}, {@link ProfileConnectionState#STATE_CONNECTED},
     * {@link ProfileConnectionState#STATE_DISCONNECTING}.
     * @since 7
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getBtConnectionState(): ProfileConnectionState;

    /**
     * Starts pairing with a remote Bluetooth device.
     *
     * @param deviceId The address of the remote device to pair.
     * @return Returns {@code true} if the pairing process is started; returns {@code false} otherwise.
     * @since 7
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function pairDevice(deviceId: string): boolean;

    /**
     * Remove a paired remote device.
     *
     * @param deviceId The address of the remote device to be removed.
     * @return Returns {@code true} if the cancel process is started; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     * @systemapi Hide this for inner system use
     */
    function cancelPairedDevice(deviceId: string): boolean;

    /**
     * Obtains the name of a peer Bluetooth device.
     *
     * @param deviceId The address of the remote device.
     * @return Returns the device name in character string format.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getRemoteDeviceName(deviceId: string): string;

    /**
     * Obtains the class of a peer Bluetooth device.
     *
     * @param deviceId The address of the remote device.
     * @return The class of the remote device, {@link DeviceClass}.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getRemoteDeviceClass(deviceId: string): DeviceClass;

    /**
     * Enables Bluetooth on a device.
     *
     * @return Returns {@code true} if Bluetooth is being enabled; returns {@code false} if an error occurs.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function enableBluetooth(): boolean;

    /**
     * Disables Bluetooth on a device.
     *
     * @return Returns {@code true} if Bluetooth is being disabled; returns {@code false} if an error occurs.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function disableBluetooth(): boolean;

    /**
     * Obtains the Bluetooth local name of a device.
     *
     * @return Returns the name the device.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getLocalName(): string;

    /**
     * Obtains the list of Bluetooth devices that have been paired with the current device.
     *
     * @return Returns a list of paired Bluetooth devices's address.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getPairedDevices(): Array<string>;

    /**
     * Obtains the connection state of profile.
     *
     * @param profileId The profile id.
     * @return Returns the connection state.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getProfileConnState(profileId: ProfileId): ProfileConnectionState;

    /**
     * Sets the confirmation of pairing with a certain device.
     *
     * @param device The address of the remote device.
     * @param accept Indicates whether to accept the pairing request, {@code true} indicates accept or {@code false} otherwise.
     * @return Returns {@code true} if the pairing confirmation is set; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.MANAGE_BLUETOOTH
     */
    function setDevicePairingConfirmation(device: string, accept: boolean): boolean;

    /**
     * Sets the Bluetooth friendly name of a device.
     *
     * @param name Indicates a valid Bluetooth name.
     * @return Returns {@code true} if the Bluetooth name is set successfully; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function setLocalName(name: string): boolean;

    /**
     * Sets the Bluetooth scan mode for a device.
     *
     * @param mode Indicates the Bluetooth scan mode to set, {@link ScanMode}.
     * @param duration Indicates the duration in seconds, in which the host is discoverable.
     * @return Returns {@code true} if the Bluetooth scan mode is set; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function setBluetoothScanMode(mode: ScanMode, duration: number): boolean;

    /**
     * Obtains the Bluetooth scanning mode of a device.
     *
     * @return Returns the Bluetooth scanning mode, {@link ScanMode}.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function getBluetoothScanMode(): ScanMode;

    /**
     * Starts scanning Bluetooth devices.
     *
     * @return Returns {@code true} if the scan is started successfully; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     * @permission ohos.permission.LOCATION
     */
    function startBluetoothDiscovery(): boolean;

    /**
     * Stops Bluetooth device scanning.
     *
     * @return Returns {@code true} if scanning is stopped successfully; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function stopBluetoothDiscovery(): boolean;

    /**
     * Subscribe the event reported when a remote Bluetooth device is discovered.
     *
     * @param type Type of the discovering event to listen for.
     * @param callback Callback used to listen for the discovering event.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function on(type: "bluetoothDeviceFind", callback: Callback<Array<string>>): void;

    /**
     * Unsubscribe the event reported when a remote Bluetooth device is discovered.
     *
     * @param type Type of the discovering event to listen for.
     * @param callback Callback used to listen for the discovering event.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function off(type: "bluetoothDeviceFind", callback?: Callback<Array<string>>): void;

    /**
     * Subscribe the event reported when a remote Bluetooth device is bonded.
     *
     * @param type Type of the bond state event to listen for.
     * @param callback Callback used to listen for the bond state event, {@link BondStateParam}.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function on(type: "bondStateChange", callback: Callback<BondStateParam>): void;

    /**
     * Unsubscribe the event reported when a remote Bluetooth device is bonded.
     *
     * @param type Type of the bond state event to listen for.
     * @param callback Callback used to listen for the bond state event.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function off(type: "bondStateChange", callback?: Callback<BondStateParam>): void;

    /**
     * Subscribe the event of a pairing request from a remote Bluetooth device.
     *
     * @param type Type of the pairing request event to listen for.
     * @param callback Callback used to listen for the pairing request event.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function on(type: "pinRequired", callback: Callback<PinRequiredParam>): void;

    /**
     * Unsubscribe the event of a pairing request from a remote Bluetooth device.
     *
     * @param type Type of the pairing request event to listen for.
     * @param callback Callback used to listen for the pairing request event.
     * @since 8
     * @permission ohos.permission.DISCOVER_BLUETOOTH
     */
    function off(type: "pinRequired", callback?: Callback<PinRequiredParam>): void;

    /**
     * Subscribe the event reported when the Bluetooth state changes.
     *
     * @param type Type of the Bluetooth state changes event to listen for.
     * @param callback Callback used to listen for the Bluetooth state event.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function on(type: "stateChange", callback: Callback<BluetoothState>): void;

    /**
     * Unsubscribe the event reported when the Bluetooth state changes.
     *
     * @param type Type of the Bluetooth state changes event to listen for.
     * @param callback Callback used to listen for the Bluetooth state event.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function off(type: "stateChange", callback?: Callback<BluetoothState>): void;

    /**
     * Creates a Bluetooth server listening socket.
     *
     * @param name Indicates the service name.
     * @param option Indicates the listen parameters {@link SppOption}.
     * @param callback Callback used to return a server socket ID.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function sppListen(name: string, option: SppOption, callback: AsyncCallback<number>): void;

    /**
     * Waits for a remote device to connect.
     *
     * @param serverSocket Indicates the server socket ID, returned by {@link sppListen}.
     * @param callback Callback used to return a client socket ID.
     * @since 8
     */
    function sppAccept(serverSocket: number, callback: AsyncCallback<number>): void;

    /**
     * Connects to a remote device over the socket.
     *
     * @param device The address of the remote device to connect.
     * @param option Indicates the connect parameters {@link SppOption}.
     * @param callback Callback used to return a client socket ID.
     * @since 8
     * @permission ohos.permission.USE_BLUETOOTH
     */
    function sppConnect(device: string, option: SppOption, callback: AsyncCallback<number>): void;

    /**
     * Disables an spp server socket and releases related resources.
     *
     * @param socket Indicates the server socket ID, returned by {@link sppListen}.
     * @since 8
     */
    function sppCloseServerSocket(socket: number): void;

    /**
     * Disables an spp client socket and releases related resources.
     *
     * @param socket Indicates the client socket ID, returned by {@link sppAccept} or {@link sppConnect}.
     * @since 8
     */
    function sppCloseClientSocket(socket: number): void;

    /**
     * Write data through the socket.
     *
     * @param clientSocket Indicates the client socket ID, returned by {@link sppAccept} or {@link sppConnect}.
     * @param data Indicates the data to write.
     * @return Returns {@code true} if the data is write successfully; returns {@code false} otherwise.
     * @since 8
     */
    function sppWrite(clientSocket: number, data: ArrayBuffer): boolean;

    /**
     * Subscribe the event reported when data is read from the socket.
     *
     * @param type Type of the spp read event to listen for.
     * @param callback Callback used to listen for the spp read event.
     * @since 8
     */
    function on(type: "sppRead", clientSocket: number, callback: Callback<ArrayBuffer>): void;

    /**
     * Unsubscribe the event reported when data is read from the socket.
     *
     * @param type Type of the spp read event to listen for.
     * @param callback Callback used to listen for the spp read event.
     * @since 8
     */
    function off(type: "sppRead", clientSocket: number, callback?: Callback<ArrayBuffer>): void;

    /**
     * Obtains the instance of profile.
     *
     * @param profileId The profile id..
     * @return Returns instance of profile.
     * @since 8
     */
    function getProfile(profileId: ProfileId): A2dpSourceProfile | HandsFreeAudioGatewayProfile;

    /**
     * Base interface of profile.
     */
    interface BaseProfile {
        /**
         * Obtains the connected devices list of profile.
         *
         * @return Returns the address of connected devices list.
         * @since 8
         * @permission ohos.permission.USE_BLUETOOTH
         */
        getConnectionDevices(): Array<string>;

        /**
         * Obtains the profile state of device.
         *
         * @param device The address of bluetooth device.
         * @return Returns {@link ProfileConnectionState} of device.
         * @since 8
         * @permission ohos.permission.USE_BLUETOOTH
         */
        getDeviceState(device: string): ProfileConnectionState;
    }

    /**
     * Manager a2dp source profile.
     */
    interface A2dpSourceProfile extends BaseProfile {
        /**
         * Connect to device with a2dp.
         *
         * @param device The address of the remote device to connect.
         * @return Returns {@code true} if the connect is in process; returns {@code false} otherwise.
         * @since 8
         * @permission permission ohos.permission.DISCOVER_BLUETOOTH
         */
        connect(device: string): boolean;

        /**
         * Disconnect to device with a2dp.
         *
         * @param device The address of the remote device to disconnect.
         * @return Returns {@code true} if the disconnect is in process; returns {@code false} otherwise.
         * @since 8
         * @permission permission ohos.permission.DISCOVER_BLUETOOTH
         */
        disconnect(device: string): boolean;

        /**
         * Subscribe the event reported when the profile connection state changes .
         *
         * @param type Type of the profile connection state changes event to listen for .
         * @param callback Callback used to listen for event.
         * @since 8
         */
        on(type: "connectionStateChange", callback: Callback<StateChangeParam>): void;

        /**
         * Unsubscribe the event reported when the profile connection state changes .
         *
         * @param type Type of the profile connection state changes event to listen for .
         * @param callback Callback used to listen for event.
         * @since 8
         */
        off(type: "connectionStateChange", callback?: Callback<StateChangeParam>): void;

        /**
         * Obtains the playing state of device.
         *
         * @param device The address of the remote device.
         * @return Returns {@link PlayingState} of the remote device.
         *
         */
        getPlayingState(device: string): PlayingState;
    }

    /**
     * Manager handsfree AG profile.
     */
    interface HandsFreeAudioGatewayProfile extends BaseProfile {
        /**
         * Connect to device with hfp.
         *
         * @param device The address of the remote device to connect.
         * @return Returns {@code true} if the connect is in process; returns {@code false} otherwise.
         * @since 8
         * @permission permission ohos.permission.DISCOVER_BLUETOOTH
         */
        connect(device: string): boolean;

        /**
         * Disconnect to device with hfp.
         *
         * @param device The address of the remote device to disconnect.
         * @return Returns {@code true} if the disconnect is in process; returns {@code false} otherwise.
         * @since 8
         * @permission permission ohos.permission.DISCOVER_BLUETOOTH
         */
        disconnect(device: string): boolean;

        /**
         * Subscribe the event reported when the profile connection state changes .
         *
         * @param type Type of the profile connection state changes event to listen for .
         * @param callback Callback used to listen for event.
         * @since 8
         */
        on(type: "connectionStateChange", callback: Callback<StateChangeParam>): void;

        /**
         * Unsubscribe the event reported when the profile connection state changes .
         *
         * @param type Type of the profile connection state changes event to listen for .
         * @param callback Callback used to listen for event.
         * @since 8
         */
        off(type: "connectionStateChange", callback?: Callback<StateChangeParam>): void;
    }

    namespace BLE {
        /**
         * create a JavaScript Gatt server instance.
         *
         * @return Returns a JavaScript Gatt server instance {@code GattServer}.
         * @since 7
         */
        function createGattServer(): GattServer;

        /**
         * create a JavaScript Gatt client device instance.
         *
         * @param deviceId The address of the remote device.
         * @return Returns a JavaScript Gatt client device instance {@code GattClientDevice}.
         * @since 7
         */
        function createGattClientDevice(deviceId: string): GattClientDevice;

        /**
         * Obtains the list of devices in the connected status.
         *
         * @return Returns the list of device address.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        function getConnectedBLEDevices(): Array<string>;

        /**
         * Starts scanning for specified BLE devices with filters.
         *
         * @param filters Indicates the list of filters used to filter out specified devices.
         * If you do not want to use filter, set this parameter to {@code null}.
         * @param options Indicates the parameters for scanning and if the user does not assign a value, the default value will be used.
         * {@link ScanOptions#interval} set to 0, {@link ScanOptions#dutyMode} set to {@link SCAN_MODE_LOW_POWER}
         * and {@link ScanOptions#matchMode} set to {@link MATCH_MODE_AGGRESSIVE}.
         * @since 7
         * @permission ohos.permission.DISCOVER_BLUETOOTH
         * @permission ohos.permission.MANAGE_BLUETOOTH
         * @permission ohos.permission.LOCATION
         */
        function startBLEScan(filters: Array<ScanFilter>, options?: ScanOptions): void;

        /**
         * Stops BLE scanning.
         *
         * @since 7
         * @permission ohos.permission.DISCOVER_BLUETOOTH
         */
        function stopBLEScan(): void;

        /**
         * Subscribe BLE scan result.
         *
         * @param type Type of the scan result event to listen for.
         * @param callback Callback used to listen for the scan result event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        function on(type: "BLEDeviceFind", callback: Callback<Array<ScanResult>>): void;

        /**
         * Unsubscribe BLE scan result.
         *
         * @param type Type of the scan result event to listen for.
         * @param callback Callback used to listen for the scan result event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        function off(type: "BLEDeviceFind", callback?: Callback<Array<ScanResult>>): void;
    }

    /**
     * Manages GATT server. Before calling an Gatt server method, you must use {@link createGattServer} to create an GattServer instance.
     */
    interface GattServer {
        /**
         * Starts BLE advertising.
         *
         * @param setting Indicates the settings for BLE advertising.
         * If you need to use the default value, set this parameter to {@code null}.
         * @param advData Indicates the advertising data.
         * @param advResponse Indicates the scan response associated with the advertising data.
         * @since 7
         * @permission ohos.permission.DISCOVER_BLUETOOTH
         */
        startAdvertising(setting: AdvertiseSetting, advData: AdvertiseData, advResponse?: AdvertiseData): void;

        /**
         * Stops BLE advertising.
         *
         * @since 7
         * @permission ohos.permission.DISCOVER_BLUETOOTH
         */
        stopAdvertising(): void;

        /**
         * Adds a specified service to be hosted.
         *
         * <p>The added service and its characteristics are provided by the local device.
         *
         * @param service Indicates the service to add.
         * @return Returns {@code true} if the service is added; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        addService(service: GattService): boolean;

        /**
         * Removes a specified service from the list of GATT services provided by this device.
         *
         * @param serviceUuid Indicates the UUID of the service to remove.
         * @return Returns {@code true} if the service is removed; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        removeService(serviceUuid: string): boolean;

        /**
         * Closes this {@code GattServer} object and unregisters its callbacks.
         *
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        close(): void;

        /**
         * Sends a notification of a change in a specified local characteristic.
         *
         * <p>This method should be called for every BLE peripheral device that has requested notifications.
         *
         * @param deviceId Indicates the address of the BLE peripheral device to receive the notification.
         * @param notifyCharacteristic Indicates the local characteristic that has changed.
         * @return Returns {@code true} if the notification is sent successfully; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        notifyCharacteristicChanged(deviceId: string, notifyCharacteristic: NotifyCharacteristic): boolean;

        /**
         * Sends a response to a specified read or write request to a given BLE peripheral device.
         *
         * @param serverResponse Indicates the response parameters {@link ServerResponse}.
         * @return Returns {@code true} if the response is sent successfully; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        sendResponse(serverResponse: ServerResponse): boolean;

        /**
         * Subscribe characteristic read event.
         *
         * @param type Type of the characteristic read event to listen for.
         * @param callback Callback used to listen for the characteristic read event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "characteristicRead", callback: Callback<CharacteristicReadReq>): void;

        /**
         * Unsubscribe characteristic read event.
         *
         * @param type Type of the characteristic read event to listen for.
         * @param callback Callback used to listen for the characteristic read event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "characteristicRead", callback?: Callback<CharacteristicReadReq>): void;

        /**
         * Subscribe characteristic write event.
         *
         * @param type Type of the characteristic write event to listen for.
         * @param callback Callback used to listen for the characteristic write event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "characteristicWrite", callback: Callback<CharacteristicWriteReq>): void;

        /**
         * Unsubscribe characteristic write event.
         *
         * @param type Type of the characteristic write event to listen for.
         * @param callback Callback used to listen for the characteristic write event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "characteristicWrite", callback?: Callback<CharacteristicWriteReq>): void;

        /**
         * Subscribe descriptor read event.
         *
         * @param type Type of the descriptor read event to listen for.
         * @param callback Callback used to listen for the descriptor read event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "descriptorRead", callback: Callback<DescriptorReadReq>): void;

        /**
         * Unsubscribe descriptor read event.
         *
         * @param type Type of the descriptor read event to listen for.
         * @param callback Callback used to listen for the descriptor read event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "descriptorRead", callback?: Callback<DescriptorReadReq>): void;

        /**
         * Subscribe descriptor write event.
         *
         * @param type Type of the descriptor write event to listen for.
         * @param callback Callback used to listen for the descriptor write event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "descriptorWrite", callback: Callback<DescriptorWriteReq>): void;

        /**
         * Unsubscribe descriptor write event.
         *
         * @param type Type of the descriptor write event to listen for.
         * @param callback Callback used to listen for the descriptor write event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "descriptorWrite", callback?: Callback<DescriptorWriteReq>): void;

        /**
         * Subscribe server connection state changed event.
         *
         * @param type Type of the connection state changed event to listen for.
         * @param callback Callback used to listen for the connection state changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "connectStateChange", callback: Callback<BLEConnectChangedState>): void;

        /**
         * Unsubscribe server connection state changed event.
         *
         * @param type Type of the connection state changed event to listen for.
         * @param callback Callback used to listen for the connection state changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "connectStateChange", callback?: Callback<BLEConnectChangedState>): void;
    }

    /**
     * Manages GATT client. Before calling an Gatt client method, you must use {@link createGattClientDevice} to create an GattClientDevice instance.
     */
    interface GattClientDevice {

        /**
         * Connects to a BLE peripheral device.
         *
         * <p>The 'BLEConnectionStateChange' event is subscribed to return the connection state.
         *
         * @return Returns {@code true} if the connection process starts; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        connect(): boolean;

        /**
         * Disconnects from or stops an ongoing connection to a BLE peripheral device.
         *
         * @return Returns {@code true} if the disconnection process starts; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        disconnect(): boolean;

        /**
         * Disables a BLE peripheral device.
         *
         * <p> This method unregisters the device and clears the registered callbacks and handles.
         *
         * @return Returns {@code true} if the the device is disabled; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        close(): boolean;

        /**
         * Obtains the name of BLE peripheral device.
         *
         * @return Returns a string representation of the name if obtained;
         * returns {@code null} if the name fails to be obtained or the name does not exist.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        getDeviceName(callback: AsyncCallback<string>): void;
        getDeviceName(): Promise<string>;

        /**
         * Starts discovering services.
         *
         * @return Returns the list of services {@link GattService} of the BLE peripheral device.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        getServices(callback: AsyncCallback<Array<GattService>>): void;
        getServices(): Promise<Array<GattService>>;

        /**
         * Reads the characteristic of a BLE peripheral device.
         *
         * @param characteristic Indicates the characteristic to read.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        readCharacteristicValue(characteristic: BLECharacteristic, callback: AsyncCallback<BLECharacteristic>): void;
        readCharacteristicValue(characteristic: BLECharacteristic): Promise<BLECharacteristic>;

        /**
         * Reads the descriptor of a BLE peripheral device.
         *
         * @param descriptor Indicates the descriptor to read.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        readDescriptorValue(descriptor: BLEDescriptor, callback: AsyncCallback<BLEDescriptor>): void;
        readDescriptorValue(descriptor: BLEDescriptor): Promise<BLEDescriptor>;

        /**
         * Writes the characteristic of a BLE peripheral device.
         *
         * @param characteristic Indicates the characteristic to write.
         * @return Returns {@code true} if the characteristic is written successfully; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        writeCharacteristicValue(characteristic: BLECharacteristic): boolean;

        /**
         * Writes the descriptor of a BLE peripheral device.
         *
         * @param descriptor Indicates the descriptor to write.
         * @return Returns {@code true} if the descriptor is written successfully; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        writeDescriptorValue(descriptor: BLEDescriptor): boolean;

        /**
         * Get the RSSI value of this BLE peripheral device.
         *
         * @return Returns the RSSI value.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        getRssiValue(callback: AsyncCallback<number>): void;
        getRssiValue(): Promise<number>;

        /**
         * Set the mtu size of a BLE peripheral device.
         *
         * @param mtu The maximum transmission unit.
         * @return Returns {@code true} if the set mtu is successfully; returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        setBLEMtuSize(mtu: number): boolean;

        /**
         * Enables or disables notification of a characteristic when value changed.
         *
         * @param enable Specifies whether to enable notification of the characteristic. The value {@code true} indicates
         * that notification is enabled, and the value {@code false} indicates that notification is disabled.
         * @return Returns {@code true} if notification of the characteristic is enabled or disabled;
         * returns {@code false} otherwise.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        setNotifyCharacteristicChanged(characteristic: BLECharacteristic, enable: boolean): boolean;

        /**
         * Subscribe characteristic value changed event.
         *
         * @param type Type of the characteristic value changed event to listen for.
         * @param callback Callback used to listen for the characteristic value changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "BLECharacteristicChange", callback: Callback<BLECharacteristic>): void;

        /**
         * Unsubscribe characteristic value changed event.
         *
         * @param type Type of the characteristic value changed event to listen for.
         * @param callback Callback used to listen for the characteristic value changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "BLECharacteristicChange", callback?: Callback<BLECharacteristic>): void;

        /**
         * Subscribe client connection state changed event.
         *
         * @param type Type of the connection state changed event to listen for.
         * @param callback Callback used to listen for the connection state changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        on(type: "BLEConnectionStateChange", callback: Callback<BLEConnectChangedState>): void;

        /**
         * Unsubscribe client connection state changed event.
         *
         * @param type Type of the connection state changed event to listen for.
         * @param callback Callback used to listen for the connection state changed event.
         * @since 7
         * @permission ohos.permission.USE_BLUETOOTH
         */
        off(type: "BLEConnectionStateChange", callback?: Callback<BLEConnectChangedState>): void;
    }

    /**
     * Describes the Gatt service.
     *
     * @since 7
     */
    interface GattService {
        /** The UUID of a GattService instance */
        serviceUuid: string;
        /** Indicates whether the GattService instance is primary or secondary. */
        isPrimary: boolean;
        /** The {@link BLECharacteristic} list belongs to this GattService instance */
        characteristics: Array<BLECharacteristic>;
        /** The list of GATT services contained in the service */
        includeServices?: Array<GattService>;
    }

    /**
     * Describes the Gatt characteristic.
     *
     * @since 7
     */
    interface BLECharacteristic {
        /** The UUID of the {@link GattService} instance to which the characteristic belongs */
        serviceUuid: string;
        /** The UUID of a BLECharacteristic instance */
        characteristicUuid: string;
        /** The value of a BLECharacteristic instance */
        characteristicValue: ArrayBuffer;
        /** The list of {@link BLEDescriptor} contained in the characteristic */
        descriptors: Array<BLEDescriptor>;
    }

    /**
     * Describes the Gatt descriptor.
     *
     * @since 7
     */
    interface BLEDescriptor {
        /** The UUID of the {@link GattService} instance to which the descriptor belongs */
        serviceUuid: string;
        /** The UUID of the {@link BLECharacteristic} instance to which the descriptor belongs */
        characteristicUuid: string;
        /** The UUID of the BLEDescriptor instance */
        descriptorUuid: string;
        /** The value of the BLEDescriptor instance */
        descriptorValue: ArrayBuffer;
    }

    /**
     * Describes the value of the indication or notification sent by the Gatt server.
     *
     * @since 7
     */
    interface NotifyCharacteristic {
        /** The UUID of the {@link GattService} instance to which the characteristic belongs */
        serviceUuid: string;
        /** The UUID of a NotifyCharacteristic instance */
        characteristicUuid: string;
        /** The value of a NotifyCharacteristic instance */
        characteristicValue: ArrayBuffer;
        /**
         * Specifies whether to request confirmation from the BLE peripheral device (indication) or
         * send a notification. Value {@code true} indicates the former and {@code false} indicates the latter.
         */
        confirm: boolean;
    }

    /**
     * Describes the parameters of the Gatt client's characteristic read request.
     *
     * @since 7
     */
    interface CharacteristicReadReq {
        /** Indicates the address of the client that initiates the read request */
        deviceId: string;
        /** The Id of the read request */
        transId: number;
        /** Indicates the byte offset of the start position for reading characteristic value */
        offset: number;
        /** The UUID of a CharacteristicReadReq instance */
        characteristicUuid: string;
        /** The UUID of the service to which the characteristic belongs */
        serviceUuid: string;
    }

    /**
     * Describes the parameters of the of the Gatt client's characteristic write request.
     *
     * @since 7
     */
    interface CharacteristicWriteReq {
        /** Indicates the address of the client that initiates the write request */
        deviceId: string;
        /** The Id of the write request */
        transId: number;
        /** Indicates the byte offset of the start position for writing characteristic value */
        offset: number;
        /** Whether this request should be pending for later operation */
        isPrep: boolean;
        /** Whether the remote client need a response */
        needRsp: boolean;
        /** Indicates the value to be written */
        value: ArrayBuffer;
        /** The UUID of a CharacteristicWriteReq instance */
        characteristicUuid: string;
        /** The UUID of the service to which the characteristic belongs */
        serviceUuid: string;
    }

    /**
     * Describes the parameters of the Gatt client's descriptor read request.
     *
     * @since 7
     */
    interface DescriptorReadReq {
        /** Indicates the address of the client that initiates the read request */
        deviceId: string;
        /** The Id of the read request */
        transId: number;
        /** Indicates the byte offset of the start position for reading characteristic value */
        offset: number;
        /** The UUID of a DescriptorReadReq instance */
        descriptorUuid: string;
        /** The UUID of the characteristic to which the descriptor belongs */
        characteristicUuid: string;
        /** The UUID of the service to which the descriptor belongs */
        serviceUuid: string;
    }

    /**
     * Describes the parameters of the Gatt client's characteristic write request.
     *
     * @since 7
     */
    interface DescriptorWriteReq {
        /** Indicates the address of the client that initiates the write request */
        deviceId: string;
        /** The Id of the write request */
        transId: number;
        /** Indicates the byte offset of the start position for writing characteristic value */
        offset: number;
        /** Whether this request should be pending for later operation */
        isPrep: boolean;
        /** Whether the remote client need a response */
        needRsp: boolean;
        /** Indicates the value to be written */
        value: ArrayBuffer;
        /** The UUID of a DescriptorWriteReq instance */
        descriptorUuid: string;
        /** The UUID of the characteristic to which the descriptor belongs */
        characteristicUuid: string;
        /** The UUID of the service to which the descriptor belongs */
        serviceUuid: string;
    }

    /**
     * Describes the parameters of a response send by the server to a specified read or write request.
     *
     * @since 7
     */
    interface ServerResponse {
        /** Indicates the address of the client to which to send the response */
        deviceId: string;
        /** The Id of the write request */
        transId: number;
        /** Indicates the status of the read or write request, set this parameter to '0' in normal cases */
        status: number;
        /** Indicates the byte offset of the start position for reading or writing operation */
        offset: number;
        /** Indicates the value to be sent */
        value: ArrayBuffer;
    }

    /**
     * Describes the Gatt profile connection state.
     *
     * @since 7
     */
    interface BLEConnectChangedState {
        /** Indicates the peer device address */
        deviceId: string;
        /** Connection state of the Gatt profile */
        state: ProfileConnectionState;
    }

    /**
     * Describes the contents of the scan results.
     *
     * @since 7
     */
    interface ScanResult {
        /** Address of the scanned device */
        deviceId: string;
        /** RSSI of the remote device */
        rssi: number;
        /** The raw data of broadcast packet */
        data: ArrayBuffer;
    }

    /**
     * Describes the settings for BLE advertising.
     *
     * @since 7
     */
    interface AdvertiseSetting {
        /**
         * Minimum slot value for the advertising interval, which is {@code 32} (20 ms)
         * Maximum slot value for the advertising interval, which is {@code 16777215} (10485.759375s)
         * Default slot value for the advertising interval, which is {@code 1600} (1s)
         */
        interval?: number;
        /**
         * Minimum transmission power level for advertising, which is {@code -127}
         * Maximum transmission power level for advertising, which is {@code 1}
         * Default transmission power level for advertising, which is {@code -7}
         */
        txPower?: number;
        /** Indicates whether the BLE is connectable, default is {@code true} */
        connectable?: boolean;
    }

    /**
     * Describes the advertising data.
     *
     * @since 7
     */
    interface AdvertiseData {
        /** The specified service UUID list to this advertisement */
        serviceUuids: Array<string>;
        /** The specified manufacturer data list to this advertisement */
        manufactureData: Array<ManufactureData>;
        /** The specified service data list to this advertisement */
        serviceData: Array<ServiceData>;
    }

    /**
     * Describes the manufacturer data.
     *
     * @since 7
     */
    interface ManufactureData {
        /** Indicates the manufacturer ID assigned by Bluetooth SIG */
        manufactureId: number;
        /** Indicates the manufacturer data to add */
        manufactureValue: ArrayBuffer;
    }

    /**
     * Describes the service data.
     *
     * @since 7
     */
    interface ServiceData {
        /** Indicates the UUID of the service data to add */
        serviceUuid: string;
        /** Indicates the service data to add */
        serviceValue: ArrayBuffer;
    }

    /**
     * Describes the criteria for filtering scanning results can be set.
     *
     * @since 7
     */
    interface ScanFilter {
        /** The address of a BLE peripheral device */
        deviceId?: string;
        /** The name of a BLE peripheral device */
        name?: string;
        /** The service UUID of a BLE peripheral device */
        serviceUuid?: string;
    }

    /**
     * Describes the parameters for scan.
     *
     * @since 7
     */
    interface ScanOptions {
        /** Time of delay for reporting the scan result */
        interval?: number;
        /** Bluetooth LE scan mode */
        dutyMode?: ScanDuty;
        /** Match mode for Bluetooth LE scan filters hardware match */
        matchMode?: MatchMode;
    }

    /**
     * Describes the spp parameters.
     *
     * @since 8
     */
    interface SppOption {
        /** Indicates the UUID in the SDP record. */
        uuid: string;
        /** Indicates secure channel or not */
        secure: boolean;
        /** Spp link type {@link SppType}*/
        type: SppType;
    }

    /**
     * Describes the bond key param.
     *
     * @since 8
     */
    interface PinRequiredParam {
        deviceId: string;
        pinCode: string;
    }

    /**
     * Describes the class of a bluetooth device.
     *
     * @since 8
     */
    interface DeviceClass {
        majorClass: MajorClass;
        majorMinorClass: MajorMinorClass;
        classOfDevice: number;
    }

    /**
     * Describes the class of a bluetooth device.
     *
     * @since 8
     */
    interface BondStateParam {
        deviceId: string;
        state: BondState;
    }

    enum ScanDuty {
        /** low power mode */
        SCAN_MODE_LOW_POWER = 0,
        /** balanced power mode */
        SCAN_MODE_BALANCED = 1,
        /** Scan using highest duty cycle */
        SCAN_MODE_LOW_LATENCY = 2
    }

    enum MatchMode {
        /** aggressive mode */
        MATCH_MODE_AGGRESSIVE = 1,
        /** sticky mode */
        MATCH_MODE_STICKY = 2
    }

    enum ProfileConnectionState {
        /** the current profile is disconnected */
        STATE_DISCONNECTED = 0,
        /** the current profile is being connected */
        STATE_CONNECTING = 1,
        /** the current profile is connected */
        STATE_CONNECTED = 2,
        /** the current profile is being disconnected */
        STATE_DISCONNECTING = 3
    }

    enum BluetoothState {
        /** Indicates the local Bluetooth is off */
        STATE_OFF = 0,
        /** Indicates the local Bluetooth is turning on */
        STATE_TURNING_ON = 1,
        /** Indicates the local Bluetooth is on, and ready for use */
        STATE_ON = 2,
        /** Indicates the local Bluetooth is turning off */
        STATE_TURNING_OFF = 3,
        /** Indicates the local Bluetooth is turning LE mode on */
        STATE_BLE_TURNING_ON = 4,
        /** Indicates the local Bluetooth is in LE only mode */
        STATE_BLE_ON = 5,
        /** Indicates the local Bluetooth is turning off LE only mode */
        STATE_BLE_TURNING_OFF = 6
    }

    /**
     * The enum of SPP type.
     *
     * @since 8
     */
    enum SppType {
        /** RFCOMM */
        SPP_RFCOMM
    }

    /**
     * The enum of BR scan mode.
     *
     * @since 8
     */
    enum ScanMode {
        /** Indicates the scan mode is none */
        SCAN_MODE_NONE = 0,
        /** Indicates the scan mode is connectable */
        SCAN_MODE_CONNECTABLE = 1,
        /** Indicates the scan mode is general discoverable */
        SCAN_MODE_GENERAL_DISCOVERABLE = 2,
        /** Indicates the scan mode is limited discoverable */
        SCAN_MODE_LIMITED_DISCOVERABLE = 3,
        /** Indicates the scan mode is connectable and general discoverable */
        SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE = 4,
        /** Indicates the scan mode is connectable and limited discoverable */
        SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE = 5
    }

    /**
     * The enum of bond state.
     *
     * @since 8
     */
    enum BondState {
        /** Indicate the bond state is invalid */
        BOND_STATE_INVALID = 0,
        /** Indicate the bond state is bonding */
        BOND_STATE_BONDING = 1,
        /** Indicate the bond state is bonded*/
        BOND_STATE_BONDED = 2
    }

    /**
     * The enum of major class of a bluetooth device.
     *
     * @since 8
     */
    enum MajorClass {
        MAJOR_MISC = 0x0000,
        MAJOR_COMPUTER = 0x0100,
        MAJOR_PHONE = 0x0200,
        MAJOR_NETWORKING = 0x0300,
        MAJOR_AUDIO_VIDEO = 0x0400,
        MAJOR_PERIPHERAL = 0x0500,
        MAJOR_IMAGING = 0x0600,
        MAJOR_WEARABLE = 0x0700,
        MAJOR_TOY = 0x0800,
        MAJOR_HEALTH = 0x0900,
        MAJOR_UNCATEGORIZED = 0x1F00
    }

    /**
     * The enum of major minor class of a bluetooth device.
     *
     * @since 8
     */
    enum MajorMinorClass {
        // The Minor Device Class field
        // Computer Major Class
        COMPUTER_UNCATEGORIZED = 0x0100,
        COMPUTER_DESKTOP = 0x0104,
        COMPUTER_SERVER = 0x0108,
        COMPUTER_LAPTOP = 0x010C,
        COMPUTER_HANDHELD_PC_PDA = 0x0110,
        COMPUTER_PALM_SIZE_PC_PDA = 0x0114,
        COMPUTER_WEARABLE = 0x0118,
        COMPUTER_TABLET = 0x011C,

        // Phone Major Class
        PHONE_UNCATEGORIZED = 0x0200,
        PHONE_CELLULAR = 0x0204,
        PHONE_CORDLESS = 0x0208,
        PHONE_SMART = 0x020C,
        PHONE_MODEM_OR_GATEWAY = 0x0210,
        PHONE_ISDN = 0x0214,

        // LAN/Network Access Point Major Class
        NETWORK_FULLY_AVAILABLE = 0x0300,
        NETWORK_1_TO_17_UTILIZED = 0x0320,
        NETWORK_17_TO_33_UTILIZED = 0x0340,
        NETWORK_33_TO_50_UTILIZED = 0x0360,
        NETWORK_60_TO_67_UTILIZED = 0x0380,
        NETWORK_67_TO_83_UTILIZED = 0x03A0,
        NETWORK_83_TO_99_UTILIZED = 0x03C0,
        NETWORK_NO_SERVICE = 0x03E0,

        // Audio/Video Major Class
        AUDIO_VIDEO_UNCATEGORIZED = 0x0400,
        AUDIO_VIDEO_WEARABLE_HEADSET = 0x0404,
        AUDIO_VIDEO_HANDSFREE = 0x0408,
        AUDIO_VIDEO_MICROPHONE = 0x0410,
        AUDIO_VIDEO_LOUDSPEAKER = 0x0414,
        AUDIO_VIDEO_HEADPHONES = 0x0418,
        AUDIO_VIDEO_PORTABLE_AUDIO = 0x041C,
        AUDIO_VIDEO_CAR_AUDIO = 0x0420,
        AUDIO_VIDEO_SET_TOP_BOX = 0x0424,
        AUDIO_VIDEO_HIFI_AUDIO = 0x0428,
        AUDIO_VIDEO_VCR = 0x042C,
        AUDIO_VIDEO_VIDEO_CAMERA = 0x0430,
        AUDIO_VIDEO_CAMCORDER = 0x0434,
        AUDIO_VIDEO_VIDEO_MONITOR = 0x0438,
        AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER = 0x043C,
        AUDIO_VIDEO_VIDEO_CONFERENCING = 0x0440,
        AUDIO_VIDEO_VIDEO_GAMING_TOY = 0x0448,

        // Peripheral Major Class
        PERIPHERAL_NON_KEYBOARD_NON_POINTING = 0x0500,
        PERIPHERAL_KEYBOARD = 0x0540,
        PERIPHERAL_POINTING_DEVICE = 0x0580,
        PERIPHERAL_KEYBOARD_POINTING = 0x05C0,
        PERIPHERAL_UNCATEGORIZED = 0x0500,
        PERIPHERAL_JOYSTICK = 0x0504,
        PERIPHERAL_GAMEPAD = 0x0508,
        PERIPHERAL_REMOTE_CONTROL = 0x05C0,
        PERIPHERAL_SENSING_DEVICE = 0x0510,
        PERIPHERAL_DIGITIZER_TABLET = 0x0514,
        PERIPHERAL_CARD_READER = 0x0518,
        PERIPHERAL_DIGITAL_PEN = 0x051C,
        PERIPHERAL_SCANNER_RFID = 0x0520,
        PERIPHERAL_GESTURAL_INPUT = 0x0522,

        // Imaging Major Class
        IMAGING_UNCATEGORIZED = 0x0600,
        IMAGING_DISPLAY = 0x0610,
        IMAGING_CAMERA = 0x0620,
        IMAGING_SCANNER = 0x0640,
        IMAGING_PRINTER = 0x0680,

        // Wearable Major Class
        WEARABLE_UNCATEGORIZED = 0x0700,
        WEARABLE_WRIST_WATCH = 0x0704,
        WEARABLE_PAGER = 0x0708,
        WEARABLE_JACKET = 0x070C,
        WEARABLE_HELMET = 0x0710,
        WEARABLE_GLASSES = 0x0714,

        // Minor Device Class field - Toy Major Class
        TOY_UNCATEGORIZED = 0x0800,
        TOY_ROBOT = 0x0804,
        TOY_VEHICLE = 0x0808,
        TOY_DOLL_ACTION_FIGURE = 0x080C,
        TOY_CONTROLLER = 0x0810,
        TOY_GAME = 0x0814,

        // Minor Device Class field - Health
        HEALTH_UNCATEGORIZED = 0x0900,
        HEALTH_BLOOD_PRESSURE = 0x0904,
        HEALTH_THERMOMETER = 0x0908,
        HEALTH_WEIGHING = 0x090C,
        HEALTH_GLUCOSE = 0x0910,
        HEALTH_PULSE_OXIMETER = 0x0914,
        HEALTH_PULSE_RATE = 0x0918,
        HEALTH_DATA_DISPLAY = 0x091C,
        HEALTH_STEP_COUNTER = 0x0920,
        HEALTH_BODY_COMPOSITION_ANALYZER = 0x0924,
        HEALTH_PEAK_FLOW_MOITOR = 0x0928,
        HEALTH_MEDICATION_MONITOR = 0x092C,
        HEALTH_KNEE_PROSTHESIS = 0x0930,
        HEALTH_ANKLE_PROSTHESIS = 0x0934,
        HEALTH_GENERIC_HEALTH_MANAGER = 0x0938,
        HEALTH_PERSONAL_MOBILITY_DEVICE = 0x093C,
    }

    /**
     * Profile state change parameters.
     *
     * @since 8
     */
    interface StateChangeParam {
        /** The address of device */
        deviceId: string;

        /** Profile state value */
        state: ProfileConnectionState;
    }

    /**
     * The enum of a2dp playing state.
     *
     * @since 8
     */
    enum PlayingState {
        STATE_NOT_PLAYING,
        STATE_PLAYING,
    }

    /**
     * The enum of profile id.
     *
     * @since 8
     */
    enum ProfileId {
        PROFILE_A2DP_SOURCE = 1,
        PROFILE_HANDS_FREE_AUDIO_GATEWAY = 4,
    }
}

export default bluetooth;
