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

declare namespace usb {
    /**
     * Obtains the USB device list.
     *
     * @return USB device{@link USBDevice}list.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function getDevices(): Array<Readonly<USBDevice>>;

    /**
     * Connects the USB device.
     *
     * @param device USB device information from device list {@link getDevices()}.
     * @return USB device pipe {@link USBDevicePipe} for data transfer.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function connectDevice(device: USBDevice): Readonly<USBDevicePipe>;

    /**
     * Checks whether the user has permission to access the device.
     *
     * @param deviceName Device name，{@link USBDevice.name}.
     * @return Returns true if the user has the permission to access the device; return false otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function hasRight(deviceName: string): boolean;

    /**
     * Requests the temporary permission for the application to access the USB device.
     *
     * @param deviceName Device name，{@link USBDevice.name}.
     * @return Returns true if the temporary device access permissions are granted; return false otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function requestRight(deviceName: string): Promise<boolean>;

    /**
     * Claims a USB interface.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param iface USB interface, which is used to determine the index of the interface to claim {@link USBInterface}.
     * @param force Optional parameter that determines whether to forcibly claim the USB interface. 
     * The default value is false, indicating not to forcibly claim the USB interface.
     * @return Returns 0 if the USB interface is successfully claimed; returns an error code otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function claimInterface(pipe: USBDevicePipe, iface: USBInterface, force?: boolean): number;

    /**
     * Releases a USB interface.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param iface USB interface, which is used to determine the index of the interface to release {@link USBInterface}.
     * @return Returns 0 if the USB interface is successfully released; return an error code otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function releaseInterface(pipe: USBDevicePipe, iface: USBInterface): number;

    /**
     * Set the device configuration.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param config USB configuration to set {@link USBConfig}.
     * @return Returns 0 if the USB configuration is successfully set; return an error code otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function setConfiguration(pipe: USBDevicePipe, config: USBConfig): number;

    /**
     * Sets a USB interface.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param iface USB interface to set {@link USBInterface}.
     * @return Returns 0 if the USB interface is successfully set; return an error code otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function setInterface(pipe: USBDevicePipe, iface: USBInterface): number;

    /**
     * Obtains the raw USB descriptor.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @return Raw descriptor data.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function getRawDescriptor(pipe: USBDevicePipe): Uint8Array;

    /**
     * Obtains the file descriptor.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @return File descriptor of the USB device.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function getFileDescriptor(pipe: USBDevicePipe): number;

    /**
     * Performs control transfer.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param contrlparam control transfer parameters.
     * @param timeout Timeout duration. This parameter is optional. The default value is 0, indicating no timeout.
     * @return Returns the size of the transmitted or received data block if the control transfer is successful; 
     * return -1 if an exception occurs. 
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function controlTransfer(pipe: USBDevicePipe, contrlparam: USBControlParams, timeout?: number): Promise<number>;

    /**
     * Performs bulk transfer.
     *
     * @param pipe Device pipe, which is used to determine the bus number and device address {@link USBDevicePipe}.
     * @param endpoint USB endpoint, which is used to determine the USB port for data transfer {@link USBEndpoint}.
     * @param buffer Buffer for writing or reading data.
     * @param timeout Timeout duration. This parameter is optional. The default value is 0, indicating no timeout.
     * @return Returns the size of the transmitted or received data block if the control transfer is successful; 
     * return -1 if an exception occurs. 
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function bulkTransfer(pipe: USBDevicePipe, endpoint: USBEndpoint, buffer: Uint8Array,
        timeout?: number): Promise<number>;

    /**
     * Closes a USB device pipe.
     *
     * @param pipe USB device pipe {@link USBDevicePipe}.
     * @return Returns 0 if the USB device pipe is closed successfully; return an error code otherwise.
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    function closePipe(pipe: USBDevicePipe): number;

    /**
     * Represents the USB endpoint from which data is sent or received.
     * You can obtain the USB endpoint through USBInterface {@link USBInterface}.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    interface USBEndpoint {
        /**
        * Endpoint address.
        * 
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        address: number;

        /**
        * Endpoint attributes.
        * 
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        attributes: number;

        /**
        * Endpoint interval.
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        interval: number;

        /**
        * Maximun size of data packets on the endpoint.
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        maxPacketSize: number;

        /**
        * Endpoint direction.
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        direction: USBRequestDirection;

        /**
        * Endpoint number.
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        number: number;

        /**
        * Endpoint type
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        type: number;

        /**
        * Unique ID of the interface to which the endpoint belongs {@link USBInterface.id}
        *
        * @syscap SystemCapability.USB.USBManager
        * @since 8
        */
        interfaceId: number;
    }


    /**
     * Represents a USB interface. One USBconfig {@link USBConfig} can contain multiple USBInterface instances, 
     * each providing a specific function.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    interface USBInterface {
        /**
         * Unique ID of the USB interface.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        id: number;

        /**
         * Interface protocol.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        protocol: number;

        /**
         * Device type.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        clazz: number;

        /**
         * Device subclass.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        subClass: number;

        /**
         * Alternating between descripors of the same USB interface.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        alternateSetting: number;

        /**
         * Interface name.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        name: string;

        /**
         * Endpoints {@link USBEndpoint} that belongs to the USB interface.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        endpoints: Array<USBEndpoint>;
    }

    /**
     * Represents the USB configuration. One USBDevice{@link USBDevice} can contain multiple USBConfig instances.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    interface USBConfig {
        /**
         * Unique ID if the USB configuration.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        id: number;

        /**
         * Configuration attributes.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        attributes: number;

        /**
         * Maximum power consumption, in mA.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        maxPower: number;

        /**
         * Configuration name, which can be left empty.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        name: string;

        /**
         * Support for remote wakeup.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        isRemoteWakeup: boolean;

        /**
         * Support for independent power supplies.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        isSelfPowered: boolean;

        /**
         * Supported interface attributes {@link USBInterface}.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        interfaces: Array<USBInterface>;
    }

    /**
     * Represents a USB device.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    interface USBDevice {
        /**
         * Bus address.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        busNum: number;
        /**
         * Device address.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        devAddress: number;
        /**
         * Device SN.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        serial: string;
        /**
         * Device name.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        name: string;
        /**
         * Device manufacturer.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        manufacturerName: string;
        /**
         * Product name.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        productName: string;
        /**
         * Product version.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        version: string;
        /**
         * Vendor ID.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        vendorId: number;
        /**
         * Product ID.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        productId: number;
        /**
         * Device class.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        clazz: number;
        /**
         * Device subclass.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        subClass: number;
        /**
         * Device protocol code.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        protocol: number;
        /**
         * Device configuration descriptor information {@link USBConfig}.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        configs: Array<USBConfig>;
    }

    /**
     * Represents a USB device pipe, which is used to determine the USB device.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    interface USBDevicePipe {
        /**
         * Bus address.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        busNum: number;
        /**
         * Device address.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        devAddress: number;
    }

    /**
    * Represents control transfer parameters.
    *
    * @syscap SystemCapability.USB.USBManager
    * @since 8
    */
    interface USBControlParams {
        /**
         * Request type.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        request: number;
        /**
         * Request target tyoe.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        target: USBRequestTargetType;
        /**
         * Request control type.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        reqType: USBControlRequestType;
        /**
         * Request parameter value.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
           */
        value: number;
        /**
         * Index of the parameter value.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        index: number;
        /**
         * Data written to or read from the buffer.
         * 
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        data: Uint8Array;
    }

    /**
     * Enumerates USB request target types.
     *
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    enum USBRequestTargetType {
        /**
         * Device.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TARGET_DEVICE = 0,
        /**
         * Interface.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TARGET_INTERFACE,
        /**
         * Endpoint.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TARGET_ENDPOINT,
        /**
         * Others.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TARGET_OTHER
    }

    /**
     * Enumerates control request types.
     * 
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    enum USBControlRequestType {
        /**
         * Standard.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TYPE_STANDARD = 0,
        /**
         * Class.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TYPE_CLASS,
        /**
         * Verdor.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_TYPE_VENDOR
    }

    /**
     * Enumerates request directions.
     * 
     * @syscap SystemCapability.USB.USBManager
     * @since 8
     */
    enum USBRequestDirection {
        /**
         * Request for writing data from the host to the device.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_DIR_TO_DEVICE = 0,
        /**
         * Request for reading data from the device to the host.
         *
         * @syscap SystemCapability.USB.USBManager
         * @since 8
         */
        USB_REQUEST_DIR_FROM_DEVICE = 0x80
    }
}

export default usb;
