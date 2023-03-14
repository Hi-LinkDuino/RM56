/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 * Provides extended methods to operate or manage Wi-Fi.
 *
 * <p>The APIs involved in this file are non-general APIs.
 * These extended APIs are only used by some product types, such as routers. 
 * Common products should not use these APIs.</p>
 *
 * @since 8
 * @import import wifiext from '@ohos.wifiext';
 */
declare namespace wifiext {
    /**
     * Enables a Wi-Fi hotspot.
     *
     * @return Returns {@code true} if this method is called successfully; returns {@code false} otherwise.
     * @since 8
     * @permission ohos.permission.MANAGE_WIFI_HOTSPOT_EXT
     * @syscap SystemCapability.Communication.WiFi.AP.Extension
     */
     function enableHotspot(): boolean;

     /**
      * Disables a Wi-Fi hotspot.
      *
      * @return Returns {@code true} if this method is called successfully; returns {@code false} otherwise.
      * @since 8
      * @permission ohos.permission.MANAGE_WIFI_HOTSPOT_EXT
      * @syscap SystemCapability.Communication.WiFi.AP.Extension
      */
     function disableHotspot(): boolean;

    /**
     * Obtains the supported power model.
     *
     * @return Returns the array of supported power model.
     *
     * @since 8
     * @permission ohos.permission.GET_WIFI_INFO
     * @syscap SystemCapability.Communication.WiFi.AP.Extension
     */
     function getSupportedPowerModel(): Promise<Array<PowerModel>>;
     function getSupportedPowerModel(callback: AsyncCallback<Array<PowerModel>>): void;

    /**
     * Obtains the current Wi-Fi power mode.
     *
     * @return Returns the current Wi-Fi power mode. If a value less than zero is returned, it indicates a failure.
     *
     * @since 8
     * @permission ohos.permission.GET_WIFI_INFO
     * @syscap SystemCapability.Communication.WiFi.AP.Extension
     */
    function getPowerModel (): Promise<PowerModel>;
    function getPowerModel (callback: AsyncCallback<PowerModel>): void;

    /**
     * Set the current Wi-Fi power mode.
     *
     * @return Returns {@code true} if the Wi-Fi is active; returns {@code false} otherwise.
     *
     * @since 8
     * @permission ohos.permission.MANAGE_WIFI_HOTSPOT_EXT
     * @syscap SystemCapability.Communication.WiFi.AP.Extension
     */
    function setPowerModel(model: PowerModel) : boolean

    /**
     * The power model enumeration.
     *
     * @since 8
     * @syscap SystemCapability.Communication.WiFi.AP.Extension
     */
     export enum PowerModel {
        /** Sleeping model. */
        SLEEPING = 0,

        /** General model. */
        GENERAL = 1,

        /** Through wall model. */
        THROUGH_WALL = 2,
    }
}

export default wifiext;
