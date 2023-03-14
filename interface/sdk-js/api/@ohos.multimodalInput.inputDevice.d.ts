/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
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

import { AsyncCallback } from './basic';

 /**
 * The input device management module is configured to obtain an ID and device information of an input device.
 *
 * @since 8
 * @syscap SystemCapability.MultimodalInput.Input.InputDevice
 * @import import inputDevice from '@ohos.multimodalInput.inputDevice';
 */

declare namespace inputDevice {
    type SourceType = 'keyboard' | 'mouse' | 'touchpad' | 'touchscreen' | 'joystick' | 'trackball';

    type AxisType = 'NULL';

    /**
     * Defines axis information about events that can be reported by an input device.
     * For example, a touchscreen may report information such as x, y, and pressure,
     * which indicate the x-axis coordinate, y-axis coordinate, and pressure, respectively.
     *
     * @syscap SystemCapability.MultimodalInput.Input.InputDevice
     * @param source Input source type of the axis. For example, if a mouse reports an x-axis event, the source of the x-axis is the mouse.
     * @param axis Type of the axis. for example, the x-axis, y-axis, and pressure axis.
     * @param max Maximum value of the data reported on this axis.
     * @param min Minimum value of the data reported on this axis.
     */
    interface AxisRange {
        source: SourceType;
        axis : AxisType;
        max : number;
        min: number;
    }

    /**
     * Defines the information about an input device.
     *
     * @syscap SystemCapability.MultimodalInput.Input.InputDevice
     * @param name Name of the input device.
     * @param sources Source type supported by the input device. For example, if a keyboard is attached with a touchpad, the device has two input sources: keyboard and touchpad.
     */
    interface InputDeviceData {
        id: number;
        name: string;
        sources : Array<SourceType>;
        axisRanges : Array<AxisRange>;
    }

    /**
     * Obtains the IDs of all input devices.
     *
     * @since 8
     * @syscap SystemCapability.MultimodalInput.Input.InputDevice
     * @param callback callback function, receive reported data
     */
    function getDeviceIds(callback: AsyncCallback<Array<number>>): void;
    function getDeviceIds(): Promise<Array<number>>;

    /**
     * Obtain the information about an input device.
     *
     * @since 8
     * @syscap SystemCapability.MultimodalInput.Input.InputDevice
     * @param deviceId ID of the input device whose information is to be obtained.
     * @param callback callback function, receive reported data
     */
    function getDevice(deviceId: number, callback: AsyncCallback<InputDeviceData>): void;
    function getDevice(deviceId: number): Promise<InputDeviceData>;
}

export default inputDevice;