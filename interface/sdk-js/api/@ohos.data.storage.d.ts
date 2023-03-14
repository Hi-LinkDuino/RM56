/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from './basic';

/**
 * Provides interfaces to obtain and modify storage data.
 *
 * @name storage
 * @since 6
 * @deprecated since 9, please use @ohos.data.preferences instead.
 * @syscap SystemCapability.DistributedDataManager.Preferences.Core
 *
 */
declare namespace storage {
    /**
     * Obtains a {@link Storage} instance matching a specified storage file name.
     *
     * <p>The {@link references} instance loads all data of the storage file and
     * resides in the memory. You can use removeStorageFromCache to remove the instance from the memory.
     *
     * @param path Indicates the path of storage file stored.
     * @return Returns the {@link Storage} instance matching the specified storage file name.
     * @throws BusinessError if invoked failed
     * @since 6
     * @deprecated since 9, please use @ohos.data.preferences instead.
     */

    function getStorageSync(path: string): Storage;
    function getStorage(path: string, callback: AsyncCallback<Storage>): void;
    function getStorage(path: string): Promise<Storage>;

    /**
     * Deletes a {@link Storage} instance matching a specified storage file name
     * from the cache which is performed by removeStorageFromCache and deletes the
     * storage file.
     *
     * <p>When deleting the {@link Storage} instance, you must release all references
     * of the instance. In addition, do not use the instance to perform data operations. Otherwise, data inconsistency
     * will occur.
     *
     * @param path Indicates the path of storage file
     * @throws BusinessError if invoked failed
     * @since 6
     * @deprecated since 9, please use @ohos.data.preferences instead.
     */
    function deleteStorageSync(path: string): void;
    function deleteStorage(path: string, callback: AsyncCallback<void>): void;
    function deleteStorage(path: string): Promise<void>;

    /**
     * Deletes a {@link Storage} instance matching a specified storage file name
     * from the cache.
     *
     * <p>When deleting the {@link Storage} instance, you must release all references
     * of the instance. In addition, do not use the instance to perform data operations. Otherwise, data inconsistency
     * will occur.
     *
     * @param path Indicates the path of storage file.
     * @throws BusinessError if invoked failed
     * @since 6
     * @deprecated since 9, please use @ohos.data.preferences instead.
     */
    function removeStorageFromCacheSync(path: string): void;
    function removeStorageFromCache(path: string, callback: AsyncCallback<void>): void;
    function removeStorageFromCache(path: string): Promise<void>;

    /**
     * Provides interfaces to obtain and modify storage data.
     *
     * <p>The storage data is stored in a file, which matches only one {@link Storage} instance in the memory.
     * You can use getStorage to obtain the {@link Storage} instance matching
     * the file that stores storage data, and use emoveStorageFromCache
     * to remove the {@link Storage} instance from the memory.
     *
     * @syscap SystemCapability.DistributedDataManager.Preferences.Core
     *
     * @since 6
     * @deprecated since 9, please use @ohos.data.preferences instead.
     */
    interface Storage {
        /**
        * Obtains the value of a storage in the int format.
        *
        * <p>If the value is {@code null} or not in the int format, the default value is returned.
        *
        * @param key Indicates the key of the storage. It cannot be {@code null} or empty.
        * @param defValue Indicates the default value to return.
        * @return Returns the value matching the specified key if it is found; returns the default value otherwise.
        * @throws BusinessError if invoked failed
        * @since 6
        * @deprecated since 9, please use @ohos.data.preferences instead.
        */
        getSync(key: string, defValue: ValueType): ValueType;
        get(key: string, defValue: ValueType, callback: AsyncCallback<ValueType>): void;
        get(key: string, defValue: ValueType): Promise<ValueType>;

        /**
         * Checks whether the {@link Storage} object contains a storage matching a specified key.
         *
         * @param key Indicates the key of the storage to check for.
         * @return Returns {@code true} if the {@link Storage} object contains a storage with the specified key;
         * returns {@code false} otherwise.
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        hasSync(key: string): boolean;
        has(key: string, callback: AsyncCallback<boolean>): boolean;
        has(key: string): Promise<boolean>;

        /**
         * Sets an int value for the key in the {@link Storage} object.
         *
         * <p>You can call the {@link #flush} or {@link #flushSync} method to save the {@link Storage} object to the
         * file.
         *
         * @param key Indicates the key of the storage to modify. It cannot be {@code null} or empty.
         * @param value Indicates the value of the storage.
         * <tt>MAX_KEY_LENGTH</tt>.
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        putSync(key: string, value: ValueType): void;
        put(key: string, value: ValueType, callback: AsyncCallback<void>): void;
        put(key: string, value: ValueType): Promise<void>;

        /**
         * Deletes the storage with a specified key from the {@link Storage} object.
         *
         * <p>You can call the {@link #flush} method to save the {@link Storage} object to the
         * file.
         *
         * @param key Indicates the key of the storage to delete. It cannot be {@code null} or empty.
         * <tt>MAX_KEY_LENGTH</tt>.
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        deleteSync(key: string): void;
        delete(key: string, callback: AsyncCallback<void>): void;
        delete(key: string): Promise<void>;

        /**
         * Clears all storage from the {@link Storage} object.
         *
         * <p>You can call the {@link #flush} method to save the {@link Storage} object to the
         * file.
         *
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        clearSync(): void;
        clear(callback: AsyncCallback<void>): void;
        clear(): Promise<void>;

        /**
         * Asynchronously saves the {@link Storage} object to the file.
         *
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        flushSync(): void;
        flush(callback: AsyncCallback<void>): void;
        flush(): Promise<void>;

        /**
         * Registers an observer to listen for the change of a {@link Storage} object.
         *
         * @param callback Indicates the callback when storage changes.
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        on(type: 'change', callback: Callback<StorageObserver>): void;

        /**
         * Unregisters an existing observer.
         *
         * @param callback Indicates the registered callback.
         * @throws BusinessError if invoked failed
         * @since 6
         * @deprecated since 9, please use @ohos.data.preferences instead.
         */
        off(type: 'change', callback: Callback<StorageObserver>): void;
    }

    /**
     * Indicates possible value types
     */
    type ValueType = number | string | boolean;

    /**
     * Define the change data information object.
     *
     * @syscap SystemCapability.DistributedDataManager.Preferences.Core
     *
     * @since 6
     * @deprecated since 9, please use @ohos.data.preferences instead.
     */
    interface StorageObserver {
        /**
         * Indicates which key changes
         */
        key: string;
    }

    /**
     * Indicates the maximum length of a key (80 characters).
     */
    const MAX_KEY_LENGTH: 80;

    /**
     * Indicates the maximum length of a string (8192 characters).
     */
    const MAX_VALUE_LENGTH: 8192;
}

export default storage;
