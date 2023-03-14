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
 * Provides methods to operate or manage Connected Tag.
 *
 * @since 8
 * @syscap SystemCapability.Communication.ConnectedTag
 * @import import connectedTag from '@ohos.connectedTag';
 */
declare namespace connectedTag {
    /**
     * Initializes Connected Tag.
     *
     * @return Returns true or false.
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function init(): boolean;

    /**
     * UnInitializes Connected Tag.
     *
     * @return Returns true or false.
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function uninit(): boolean;

    /**
     * Reads the NDEF Data.
     *
     * @return Returns the NDEF Data.
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function readNdefTag(): Promise<string>;
    function readNdefTag(callback: AsyncCallback<string>): void;

    /**
     * Writes the NDEF Data.
     *
     * @param data The Data to write.
     * @return Returns true or false.
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function writeNdefTag(data: string): Promise<void>;
    function writeNdefTag(data: string, callback: AsyncCallback<void>): void;

    /**
     * Subscribes NFC RF status change events.
     *
     * @type The callback type.
     * @param callback The callback function to be registered.
     * @return Returns NFC_RF_LEAVE or NFC_RF_ENTER
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function on(type: "notify", callback: Callback<number>): void;

    /**
     * Unsubscribes NFC RF status change events.
     *
     * <p>All callback functions will be deregistered If there is no specific callback parameter.</p>
     *
     * @type The callback type.
     * @param callback The callback function to be unregistered.
     * @permissions ohos.permission.NFC_TAG
     *
     * @since 8
     */
    function off(type: "notify", callback?:Callback<number>): void;

    /**
     * Describes the NFC RF type.
     *
     * @since 8
     */
    enum NfcRfType {
        /** NFC RF LEAVE */
        NFC_RF_LEAVE = 0;

        /** NFC RF ENTER */
        NFC_RF_ENTER = 1;
    }
}

export default connectedTag;
