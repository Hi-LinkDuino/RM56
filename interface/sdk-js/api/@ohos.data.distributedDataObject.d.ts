/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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

import {AsyncCallback, Callback} from './basic';

/**
 * Provides interfaces to sync distributed object
 *
 * @name distributedDataObject
 * @since 8
 * @syscap SystemCapability.DistributedDataManager.DataObject.DistributedObject
 */
declare namespace distributedDataObject {
    /**
     * Create distributed object
     *
     * @param source Init data of distributed object
     * @return Returns the distributed object
     * @since 8
     */
    function createDistributedObject(source: object): DistributedObject;

    /**
     * Generate a random sessionId
     *
     * @return Returns the random sessionId
     * @since 8
     */
    function genSessionId(): string;

    /**
     * Object create by {@link createDistributedObject}.
     *
     * @syscap SystemCapability.DistributedDataManager.DataObject.DistributedObject
     * @since 8
     */
    interface DistributedObject {
        /**
         * Change object session
         *
         * @param sessionId The sessionId to be joined, if empty, leave all session
         * @return Operation result, true is success, false is failed
         * @permission ohos.permission.DISTRIBUTED_DATASYNC
         * @since 8
         */
        setSessionId(sessionId?: string): boolean;

        /**
         * On watch of change
         *
         * @param callback The callback of change
         * @since 8
         */
        on(type: 'change', callback: Callback<{ sessionId: string, fields: Array<string> }>): void;

        /**
         * Off watch of change
         *
         * @param callback If not null, off the callback, if undefined, off all callbacks
         * @since 8
         */
        off(type: 'change', callback?: Callback<{ sessionId: string, fields: Array<string> }>): void;

        /**
         * On watch of status
         *
         * @param callback Indicates the observer of object status changed.
         *                 sessionId: The sessionId of the changed object 
         *                 networkId: NetworkId of the changed device
         *                 status: 'online' The object became online on the device and data can be synced to the device
         *                         'offline' The object became offline on the device and the object can not sync any data
         * @since 8
         */
        on(type: 'status', callback: Callback<{ sessionId: string, networkId: string, status: 'online' | 'offline' }>): void;

        /**
         * Off watch of status
         *
         * @param callback If not null, off the callback, if undefined, off all callbacks
         * @since 8
         */
        off(type: 'status', callback?: Callback<{ sessionId: string, deviceId: string, status: 'online' | 'offline' }>): void;
    }
}

export default distributedDataObject;
