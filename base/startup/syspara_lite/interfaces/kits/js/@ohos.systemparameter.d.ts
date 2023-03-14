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

import { AsyncCallback, BusinessError } from './basic';

 /**
 * The interface of system parameters class.
 *
 * @devices phone, tablet
 * @since 6
 * @Syscap SystemCapability.Startup.SysInfo
 * @systemapi
 */
declare namespace systemParameter {
    /**
     * Gets the value of the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param def Default value.
     * @return if the parameter is empty or doesn't exist, empty string will be returned.
     * @since 6
     */
    function getSync(key: string, def?: string): string;

    /**
     * Gets the value of the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param callback Callback function.
     * @since 6
     */
    function get(key: string, callback: AsyncCallback<string>): void;

    /**
     * Gets the value of the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param def Default value.
     * @param callback Callback function.
     * @since 6
     */
    function get(key: string, def: string, callback: AsyncCallback<string>): void;

    /**
     * Gets the value of the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param def Default value.
     * @return Promise, which is used to obtain the result asynchronously.
     * @since 6
     */
    function get(key: string, def?: string): Promise<string>;

    /**
     * Sets a value for the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param value System attribute value to set.
     * @since 6
     */
    function setSync(key: string, value: string): void;

    /**
     * Sets a value for the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param value System attribute value to set.
     * @param callback Callback function.
     * @since 6
     */
    function set(key: string, value: string, callback: AsyncCallback<void>): void;

    /**
     * Sets a value for the attribute with the specified key.
     *
     * @param key Key of the system attribute.
     * @param value Default value.
     * @return Promise, which is used to obtain the result asynchronously.
     * @since 6
     */
    function set(key: string, value: string): Promise<void>;

    /**
     * Wait for a parameter with specified value.
     *
     * @param key Key of the system parameter.
     * @param value System parameter value to be wait.
     * @param timeout Indicates the timeout value, in seconds.
     * <=0 means wait for ever.
     * >0 means wait for specified seconds
     * @param callback Callback function.
     * @since 7
     */
     function wait(key: string, value: string, timeout: number, callback: AsyncCallback<void>): void;

     /**
      * Sets a value for the parameter with the specified key.
      *
      * @param key Key of the system parameter.
      * @param value System parameter value to be wait.
      * @param timeout Indicates the timeout value, in seconds.
      * <=0 means wait for ever.
      * >0 means wait for specified seconds
      * @return Promise, which is used to obtain the result asynchronously.
      * @since 7
      */
     function wait(key: string, value: string, timeout: number): Promise<void>;

     /**
      * Wait for a parameter with specified value.
      *
      * @param keyPrefix Key prefix of the system parameters to be watched.
      * @param callback Callback function.
      * @since 7
      */
     function getWatcher(keyPrefix: string): Watcher;

     /**
      * Called when the system parameter value changes. You need to implement this method in a child class.
      *
      * @param key Indicates changed system parameter key name.
      * @param value Indicates changed system parameter value.
      * @since 7
      */
     export interface ParameterChangeCallback {
         (key: string, value: string): void;
     }

     export interface Watcher {
         /**
          * Subscribe to parameter value changess
          *
          * @since 7
          */
         on(eventType: 'valueChange', callback: ParameterChangeCallback): void;
         /**
          * Unsubscribe to parameter value changess
          *
          * @since 7
          */
         off(eventType: 'valueChange', callback?: ParameterChangeCallback): void;
     }
}

export default systemParameter;
