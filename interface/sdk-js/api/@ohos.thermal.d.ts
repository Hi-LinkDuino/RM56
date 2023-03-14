/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from './basic';

/**
 * Provides thermal level-related callback and query APIs to obtain the information required for 
 * temperature control. The APIs are as follows:
 * {@link subscribeThermalLevel}: subscribes to callbacks of thermal level changes.
 * {@link getThermalLevel}: obtains the thermal level of the system in real time.
 *
 * @syscap SystemCapability.PowerManager.ThermalManager
 * @since 8
 */
declare namespace thermal {
    /**
     * Enumerates the {@link ThermalLevel} types.
     *
     * @since 8
     */
    export enum ThermalLevel {
        /**
         * The device is cool, and services are not restricted.
         */
        COOL = 0,
        /**
         * The device is operational but is not cool. You need to pay attention to its heating.
         */
        NORMAL = 1,
        /**
         * The device is warm. You need to stop or delay some imperceptible services.
         */
        WARM = 2,
        /**
         * The device is heating up. You need to stop all imperceptible services and downgrade 
         * or reduce the load of other services.
         */
        HOT = 3,
        /**
         * The device is overheated. You need to stop all imperceptible services and downgrade 
         * or reduce the load of major services.
         */
        OVERHEATED = 4,
        /**
         * The device is overheated and is about to enter the emergency state. You need to stop 
         * all imperceptible services and downgrade major services to the maximum extent.
         */
        WARNING = 5,
        /**
         * The device has entered the emergency state. You need to stop all services except those 
         * for the emergency help purposes.
         */
        EMERGENCY = 6,
    }
    /**
     * Subscribes to callbacks of thermal level changes.
     *
     * @param callback Callback of thermal level changes.
     * @return Returns the thermal level.
     * @since 8
     */
    function subscribeThermalLevel(callback: AsyncCallback<ThermalLevel>): void;

    /**
     * Unsubscribes from the callbacks of thermal level changes.
     *
     * @param callback Callback of thermal level changes.
      
     * @since 8
     */
    function unsubscribeThermalLevel(callback?: AsyncCallback<void>): void;

    /**
     * Obtains the current thermal level.
     *
     * @return Returns the thermal level.
     * @since 8
     */
    function getThermalLevel(): ThermalLevel;
}
export default thermal;
