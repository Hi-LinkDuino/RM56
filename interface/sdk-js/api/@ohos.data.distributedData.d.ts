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

import { AsyncCallback, Callback } from './basic';

/**
 * Providers interfaces to creat a {@link KVManager} istances.
 * @since 7
 * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
 * @import N/A
 */

declare namespace distributedData {
    /**
     * Provides configuration information for {@link KVManager} instances,
     * including the caller's package name and distributed network type.
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface KVManagerConfig {
        /**
         * Indicates the user information
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        userInfo: UserInfo;

        /**
         * Indicates the bundleName
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        bundleName: string;
    }

    /**
     * Manages user information.
     *
     * <p>This class provides methods for obtaining the user ID and type, setting the user ID and type,
     * and checking whether two users are the same.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface UserInfo {
        /** 
         * Indicates the user ID to set 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        userId?: string;

        /** 
         * Indicates the user type to set 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        userType?: UserType;
    }

    /**
     * Enumerates user types.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum UserType {
        /** 
         * Indicates a user that logs in to different devices using the same account. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        SAME_USER_ID = 0
    }

    /**
     * KVStore constants
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    namespace Constants {
        /**
         * max key length.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_KEY_LENGTH = 1024;

        /**
         * max value length.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_VALUE_LENGTH = 4194303;

        /**
         * max device coordinate key length.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_KEY_LENGTH_DEVICE = 896;

        /**
         * max store id length.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_STORE_ID_LENGTH = 128;

        /**
         * max query length.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_QUERY_LENGTH = 512000;

        /**
         * max batch operation size.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        const MAX_BATCH_SIZE = 128;
    }

    /**
     * Indicates the {@code ValueType}.
     *
     * <p>{@code ValueType} is obtained based on the value.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum ValueType {
        /** 
         * Indicates that the value type is string. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        STRING = 0,

        /**
         * Indicates that the value type is int. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        INTEGER = 1,

        /** 
         * Indicates that the value type is float. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        FLOAT = 2,

        /** 
         * Indicates that the value type is byte array. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * */
        BYTE_ARRAY = 3,

        /** 
         * Indicates that the value type is boolean. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * */
        BOOLEAN = 4,

        /** 
         * Indicates that the value type is double. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        DOUBLE = 5
    }

    /**
     * Obtains {@code Value} objects stored in a {@link KVStore} database.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface Value {
        /**
         * Indicates value type
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @see ValueType
         * @type {number}
         * @memberof Value
         */
        type: ValueType;
        /**
         * Indicates value
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        value: Uint8Array | string | number | boolean;
    }

    /**
     * Provides key-value pairs stored in the distributed database.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface Entry {
        /**
         * Indicates key
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        key: string;
        /**
         * Indicates value
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        value: Value;
    }

    /**
     * Receives notifications of all data changes, including data insertion, update, and deletion.
     *
     * <p>If you have subscribed to {@code KVStore}, you will receive data change notifications and obtain the changed data
     * from the parameters in callback methods upon data insertion, update, or deletion.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface ChangeNotification {
        /** 
         * Indicates data addition records. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        insertEntries: Entry[];
        /** 
         * Indicates data update records.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        updateEntries: Entry[];
        /** 
         * Indicates data deletion records. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        deleteEntries: Entry[];
        /**
         * Indicates from device id.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        deviceId: string;
    }

    /**
     * Indicates the database synchronization mode.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum SyncMode {
        /** 
         * Indicates that data is only pulled from the remote end.
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        PULL_ONLY = 0,
        /** 
         * Indicates that data is only pushed from the local end. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        PUSH_ONLY = 1,
        /** 
         * Indicates that data is pushed from the local end, and then pulled from the remote end. 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        PUSH_PULL = 2
    }

    /**
     * Describes the subscription type.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum SubscribeType {
        /** 
         * Subscription to local data changes 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
        */
        SUBSCRIBE_TYPE_LOCAL = 0,

        /** 
         * Subscription to remote data changes 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
        */
        SUBSCRIBE_TYPE_REMOTE = 1,

        /** 
         * Subscription to both local and remote data changes 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        SUBSCRIBE_TYPE_ALL = 2,
    }

    /**
     * Describes the {@code KVStore} type.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum KVStoreType {
        /** 
         * Device-collaborated database, as specified by {@code DeviceKVStore} 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @import N/A
         */
        DEVICE_COLLABORATION = 0,

        /** 
         * Single-version database, as specified by {@code SingleKVStore} 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        SINGLE_VERSION = 1,

        /** 
         * Multi-version database, as specified by {@code MultiKVStore} 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @import N/A
         */
        MULTI_VERSION = 2,
    }

    /**
     * Describes the {@code KVStore} type.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    enum SecurityLevel {
        /**
         * NO_LEVEL: mains not set the security level.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @import N/A
         */
        NO_LEVEL = 0,

        /**
         * S0: mains the db is public.
         * There is no impact even if the data is leaked.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        S0 = 1,

        /**
         * S1: mains the db is low level security
         * There are some low impact, when the data is leaked.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        S1 = 2,

        /**
         * S2: mains the db is middle level security
         * There are some major impact, when the data is leaked.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        S2 = 3,

        /**
         * S3: mains the db is high level security
         * There are some severity impact, when the data is leaked.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        S3 = 5,

        /**
         * S4: mains the db is critical level security
         * There are some critical impact, when the data is leaked.
         *
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        S4 = 6,
    }

    /**
     * Provides configuration options for creating a {@code KVStore}.
     *
     * <p>You can determine whether to create another database if a {@code KVStore} database is missing,
     * whether to encrypt the database, and the database type.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface Options {
        /**
         * Indicates whether to createa database when the database file does not exist
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        createIfMissing?: boolean;
        /**
         * Indicates setting whether database files are encrypted
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        encrypt?: boolean;
        /**
         * Indicates setting whether to back up database files
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        backup?: boolean;
        /**
         * Indicates setting whether database files are automatically synchronized
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @permission ohos.permission.DISTRIBUTED_DATASYNC
         * @import N/A
         */
        autoSync?: boolean;
        /**
         * Indicates setting the databse type
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        kvStoreType?: KVStoreType;
        /**
         * Indicates setting the database security level
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        securityLevel?: SecurityLevel;
        /**
         * Indicates schema object 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @import N/A
         */
        schema?: Schema;
    }

    /**
     * Represents the database schema.
     * 
     * You can create Schema objects and put them in Options when creating or opening the database.
     * 
     * @since 8
     * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
     * @import N/A
     */
    class Schema {
        /**
         * A constructor used to create a Schema instance.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
        constructor()
        /**
         * Indicates the root json object.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
		root: FieldNode;  
        /**
         * Indicates the string array of json.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
        indexes: Array<string>;    
        /**
         * Indicates the mode of schema.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
        mode: number;
        /**
         * Indicates the skipsize of schema.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
        skip: number;
    }

    /**
     * Represents a node of a {@link Schema} instance.
     * 
     * <p>Through the {@link Schema} instance, you can define the fields contained in the values stored in a database.
     * 
     * <p>A FieldNode of the {@link Schema} instance is either a leaf or a non-leaf node.
     * 
     * <p>The leaf node must have a value; the non-leaf node must have a child {@code FieldNode}.
     * 
     * @since 8
     * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
     * @import N/A
     */
    class FieldNode {
        /**
         * A constructor used to create a FieldNode instance with the specified field.
         * name Indicates the field node name.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
        constructor(name: string)
        /**
         * Adds a child node to this {@code FieldNode}.
         * 
         * <p>Adding a child node makes this node a non-leaf node. Field value will be ignored if it has child node.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @param child The field node to append. 
         * @returns Returns true if the child node is successfully added to this {@code FieldNode}; returns false otherwise.
         */
        appendChild(child: FieldNode): boolean;
        /**
         * Indicates the default value of fieldnode.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         */
         default: string;
         /**
          * Indicates the nullable of database field.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          */
         nullable: boolean;
         /**
          * Indicates the type of value.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          */
         type: number;
    }

    /**
     * Provide methods to obtain the result set of the {@code KvStore} database.
     * 
     * <p>The result set is created by using the {@code getResultSet} method in the {@code DeviceKVStore} class. This interface also provides
     * methods for moving the data read position in the result set.
     * 
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    interface KvStoreResultSet {
        /**
         * Obtains the number of lines in a result set.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns the number of lines.
         */
        getCount(): number;
        /**
         * Obtains the current read position in a result set.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns the current read position. The read position starts with 0.
         */
        getPosition(): number;
        /**
         * Moves the read position to the first line.
         * 
         * <p>If the result set is empty, false is returned.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        moveToFirst(): boolean;
        /**
         * Moves the read position to the last line.
         * 
         * <p>If the result set is empty, false is returned.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        moveToLast(): boolean;
        /**
         * Moves the read position to the next line.
         * 
         * <p>If the result set is empty or the data in the last line is being read, false is returned.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        moveToNext(): boolean;
        /**
         * Moves the read position to the previous line.
         * 
         * <p>If the result set is empty or the data in the first line is being read, false is returned.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        moveToPrevious(): boolean;
        /**
         * Moves the read position by a relative offset to the current position.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param offset Indicates the relative offset to the current position. Anegative offset indicates moving backwards, and a
         * positive offset indicates moving forewards. Forexample, if the current position is entry 1 and thisoffset is 2,
         * the destination position will be entry 3; ifthe current position is entry 3 and this offset is -2,
         * the destination position will be entry 1. The valid final position after moving forwards starts with 0. If the
         * final position is invalid, false will be returned.
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        move(offset: number): boolean;
        /**
         * Moves the read position from 0 to an absolute position.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param position Indicates the absolute position.
         * @returns Returns true if the operation succeeds; return false otherwise.
         */
        moveToPosition(position: number): boolean;
        /**
         * Checks whether the read position is the first line.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
        
         * @returns Returns true if the read position is the first line; returns false otherwise.
         */
        isFirst(): boolean;
        /**
         * Checks whether the read position is the last line.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the read position is the last line; returns false otherwise.
         */
        isLast(): boolean;
        /**
         * Checks whether the read position is before the last line.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the read position is before the first line; returns false otherwise.
         */
        isBeforeFirst(): boolean;
        /**
         * Checks whether the read position is after the last line.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns true if the read position is after the last line; returns false otherwise. 
         */
        isAfterLast(): boolean;
        /**
         * Obtains a key-value pair.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @returns Returns a key-value pair.
         */
        getEntry(): Entry;
    }

    /**
     * Represents a database query using a predicate.
     * 
     * <p>This class provides a constructor used to create a {@code Query} instance, which is used to query data matching specified
     * conditions in the database.
     * 
     * <p>This class also provides methods for adding predicates to the {@code Query} instance.
     * 
     * @since 8
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     */
    class Query {
        /**
         * A constructor used to create a Query instance.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         */
        constructor()        
        /**
         * Resets this {@code Query} object.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @returns Returns the reset {@code Query} object.
         */
        reset(): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is equal to the specified long value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value IIndicates the long value.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid.
         */
        equalTo(field: string, value: number|string|boolean): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is not equal to the specified int value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the int value.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid.
         */
        notEqualTo(field: string, value: number|string|boolean): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is greater than or equal to the
         * specified int value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the int value.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid.
         */
        greaterThan(field: string, value: number|string|boolean): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is less than the specified int value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the int value.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid.
         */
        lessThan(field: string, value: number|string): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is greater than or equal to the
         * specified int value.
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A  
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the int value.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid.
         */
        greaterThanOrEqualTo(field: string, value: number|string): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is less than or equal to the
         * specified int value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A  
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the int value.
         * @returns Returns the {@coed Query} object. 
         * @throws Throws this exception if input is invalid.
         */
        lessThanOrEqualTo(field: string, value: number|string): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is null.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A   
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @returns Returns the {@coed Query} object.  
         * @throws Throws this exception if input is invalid.
         */
        isNull(field: string): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is within the specified int value list.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param valueList Indicates the int value list.
         * @returns Returns the {@coed Query} object. 
         * @throws Throws this exception if input is invalid.
         */
        inNumber(field: string, valueList: number[]): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is within the specified string value list.
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
        
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param valueList Indicates the string value list.
         * @returns Returns the {@coed Query} object. 
         * @throws Throws this exception if input is invalid.
         */
        inString(field: string, valueList: string[]): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is not within the specified int value list.
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param valueList Indicates the int value list.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid. 
         */
        notInNumber(field: string, valueList: number[]): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is not within the specified string value list.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param valueList Indicates the string value list.
         * @returns Returns the {@coed Query} object. 
         * @throws Throws this exception if input is invalid. 
         */
        notInString(field: string, valueList: string[]): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is similar to the specified string value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the string value.
         * @returns Returns the {@coed Query} object.  
         * @throws Throws this exception if input is invalid. 
         */
        like(field: string, value: string): Query;
        /**
         * Constructs a {@code Query} object to query entries with the specified field whose value is not similar to the specified string value.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @param value Indicates the string value.
         * @returns Returns the {@coed Query} object.  
         * @throws Throws this exception if input is invalid. 
         */
        unlike(field: string, value: string): Query;
        /**
         * Constructs a {@code Query} object with the and condition.
         * 
         * <p>Multiple predicates should be connected using the and or or condition.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @returns Returns the {@coed Query} object. 
         */
        and(): Query;
        /**
         * Constructs a {@code Query} object with the or condition.
         * 
         * <p>Multiple predicates should be connected using the and or or condition.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @returns Returns the {@coed Query} object. 
         */
        or(): Query;
        /**
         * Constructs a {@code Query} object to sort the query results in ascending order.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @returns Returns the {@coed Query} object. 
         * @throws Throws this exception if input is invalid. 
         */
        orderByAsc(field: string): Query;
        /**
         * Constructs a {@code Query} object to sort the query results in descending order.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param field Indicates the field, which must start with $. and cannot contain ^.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid. 
         */
        orderByDesc(field: string): Query;
        /**
         * Constructs a {@code Query} object to specify the number of results and the start position.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A  
         * @param total Indicates the number of results.
         * @param offset Indicates the start position.
         * @returns Returns the {@coed Query} object.
         */
        limit(total: number, offset: number): Query;
        /**
         * Creates a {@code query} condition with a specified field that is not null.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A  
         * @param field Indicates the specified field.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid. 
         */
        isNotNull(field: string): Query;
        /**
         * Creates a query condition group with a left bracket.
         * 
         * <p>Multiple query conditions in an {@code Query} object can be grouped. The query conditions in a group can be used as a
         * whole to combine with other query conditions.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @returns Returns the {@coed Query} object.
         */
        beginGroup(): Query;
        /**
         * Creates a query condition group with a right bracket.
         * 
         * <p>Multiple query conditions in an {@code Query} object can be grouped. The query conditions in a group can be used as a
         * whole to combine with other query conditions.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @returns Returns the {@coed Query} object.
         */
        endGroup(): Query;
        /**
         * Creates a query condition with a specified key prefix.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param prefix Indicates the specified key prefix.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid. 
         */
        prefixKey(prefix: string): Query;
        /**
         * Sets a specified index that will be preferentially used for query.
         *
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param index Indicates the index to set.
         * @returns Returns the {@coed Query} object.
         * @throws Throws this exception if input is invalid. 
         */
        setSuggestIndex(index: string): Query;
		/**
		 * Add device ID key prefix.Used by {@code DeviceKVStore}.
		 *
		 * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
		 * @param deviceId Specify device id to query from.
		 * @return Returns the {@code Query} object with device ID prefix added.
		 * @throw Throws this exception if input is invalid.
		 */
		deviceId(deviceId:string):Query;
		/**
		 * Get a String that repreaents this {@code Query}.
		 *
		 * <p>The String would be parsed to DB query format.
		 * The String length should be no longer than 500kb.
		 * 
		 * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
		 * @return String representing this {@code Query}. 
		 */
		getSqlLike():string;
    }

    /**
     * Represents a key-value distributed database and provides methods for adding, deleting, modifying, querying,
     * and subscribing to distributed data.
     *
     * <p>You can create distributed databases of different types by {@link KVManager#getKVStore (Options, String)}
     * with input parameter {@code Options}. Distributed database types are defined in {@code KVStoreType},
     * including {@code SingleKVStore}.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     * @version 1
     */
    interface KVStore {
        /**
         * Writes a key-value pair of the string type into the {@code KvStore} database.
         *
         * <p>If you do not want to synchronize this key-value pair to other devices, set the write option in the local database.
         * 
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param key Indicates the key. The length must be less than {@code MAX_KEY_LENGTH}.
         * Spaces before and after the key will be cleared.
         * @param value Indicates the string value, which must be less than 4 MB as a UTF-8 byte array.
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and
         * {@code DB_ERROR}.
         */
        put(key: string, value: Uint8Array | string | number | boolean, callback: AsyncCallback<void>): void;
        put(key: string, value: Uint8Array | string | number | boolean): Promise<void>;

        /**
         * Deletes the key-value pair based on a specified key.
         *
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param key Indicates the key. The length must be less than {@code MAX_KEY_LENGTH}.
         * Spaces before and after the key will be cleared.
         *  @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and
         * {@code DB_ERROR}, and {@code KEY_NOT_FOUND}.
         */
        delete(key: string, callback: AsyncCallback<void>): void;
        delete(key: string): Promise<void>;

        /**
         * Registers a {@code KvStoreObserver} for the database. When data in the distributed database changes, the callback in 
         * {@code KvStoreObserver} will be invoked.
         *
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param type Indicates the subscription type, which is defined in {@code SubscribeType}.
         * @param observer Indicates the observer of data change events in the distributed database.
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR},
         * {@code DB_ERROR}, and {@code STORE_ALREADY_SUBSCRIBE}.
         */
        on(event: 'dataChange', type: SubscribeType, observer: Callback<ChangeNotification>): void;

        /**
         * Subscribes from the {@code KvStore} database based on the specified subscribeType and {@code KvStoreObserver}.
         * 
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR},
         * {@code DB_ERROR}, and {@code STORE_ALREADY_SUBSCRIBE}.
         */
        on(event: 'syncComplete', syncCallback: Callback<Array<[string, number]>>): void;
		
        /**
         * Unsubscribes from the {@code KvStore} database based on the specified subscribeType and {@code KvStoreObserver}.
         *
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param observer Indicates the data change observer registered by {#subscribe(SubscribeType, KvStoreObserver)}.
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR},
         * {@code DB_ERROR}, and {@code STORE_ALREADY_SUBSCRIBE}.
         */
        off(event:'dataChange', observer?: Callback<ChangeNotification>): void;

        /**
         * Inserts key-value pairs into the {@code KvStore} database in batches.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param entries Indicates the key-value pairs to be inserted in batches.
         * @throws Throws this exception if a database error occurs.
         */
        putBatch(entries: Entry[], callback: AsyncCallback<void>): void;
        putBatch(entries: Entry[]): Promise<void>;

        /**
         * Deletes key-value pairs in batches from the {@code KvStore} database.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param keys Indicates the key-value pairs to be deleted in batches.
         * @throws Throws this exception if a database error occurs.
         */
        deleteBatch(keys: string[], callback: AsyncCallback<void>): void;
        deleteBatch(keys: string[]): Promise<void>;

        /**
         * Starts a transaction operation in the {@code KvStore} database.
         * 
         * <p>After the database transaction is started, you can submit or roll back the operation.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @throws Throws this exception if a database error occurs.
         */
        startTransaction(callback: AsyncCallback<void>): void;
        startTransaction(): Promise<void>;

        /**
         * Submits a transaction operation in the {@code KvStore} database.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param callback 
         * @throws Throws this exception if a database error occurs.
         */
        commit(callback: AsyncCallback<void>): void;
        commit(): Promise<void>;

        /**
         * Rolls back a transaction operation in the {@code KvStore} database.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @throws Throws this exception if a database error occurs.
         */
        rollback(callback: AsyncCallback<void>): void;
        rollback(): Promise<void>;

        /**
         * Sets whether to enable synchronization.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param enabled Specifies whether to enable synchronization. The value true means to enable
         * synchronization, and false means the opposite.
         * @throws Throws this exception if an internal service error occurs.
         */
        enableSync(enabled: boolean, callback: AsyncCallback<void>): void;
        enableSync(enabled: boolean): Promise<void>;

        /**
         * Sets synchronization range labels.
         * 
         * <p>The labels determine the devices with which data will be synchronized.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param localLabels Indicates the synchronization labels of the local device.
         * @param remoteSupportLabels Indicates the labels of the devices with which data will be synchronized.
         * @throws Throws this exception if an internal service error occurs.
         */
        setSyncRange(localLabels: string[], remoteSupportLabels: string[], callback: AsyncCallback<void>): void;
        setSyncRange(localLabels: string[], remoteSupportLabels: string[]): Promise<void>;
    }

    /**
     * Provides methods related to single-version distributed databases.
     *
     * <p>To create a {@code SingleKVStore} database,
     * you can use the {@link data.distributed.common.KVManager#getKVStore​(Options, String)} method
     * with {@code KVStoreType} set to {@code SINGLE_VERSION} for the input parameter {@code Options}.
     * This database synchronizes data to other databases in time sequence.
     * The {@code SingleKVStore} database does not support
     * synchronous transactions, or data search using snapshots.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     * @version 1
     */
    interface SingleKVStore extends KVStore {
        /**
         * Obtains the {@code String} value of a specified key.
         * 
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param key Indicates the key of the boolean value to be queried.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}, and {@code KEY_NOT_FOUND}.
         */
        get(key: string, callback: AsyncCallback<Uint8Array | string | boolean | number>): void;
        get(key: string): Promise<Uint8Array | string | boolean | number>;

        /**
         * Obtains all key-value pairs that match a specified key prefix.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param keyPrefix Indicates the key prefix to match.
         * @returns Returns the list of all key-value pairs that match the specified key prefix.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        getEntries(keyPrefix: string, callback: AsyncCallback<Entry[]>): void;
        getEntries(keyPrefix: string): Promise<Entry[]>;

        /**
         * Obtains the list of key-value pairs matching the specified {@code Query} object.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         * @param query Indicates the {@code Query} object.
         * @returns Returns the list of key-value pairs matching the specified {@code Query} object.
         * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        getEntries(query: Query, callback: AsyncCallback<Entry[]>): void;
        getEntries(query: Query): Promise<Entry[]>;

        /**
         * Obtains the result sets with a specified prefix from a {@code KvStore} database. The {@code KvStoreResultSet} object can be used to
         * query all key-value pairs that meet the search criteria. Each {@code KvStore} instance can have a maximum of four
         * {@code KvStoreResultSet} objects at the same time. If you have created four objects, calling this method will return a
         * failure. Therefore, you are advised to call the closeResultSet method to close unnecessary {@code KvStoreResultSet} objects
         * in a timely manner.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param keyPrefix Indicates the key prefix to match.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        getResultSet(keyPrefix: string, callback: AsyncCallback<KvStoreResultSet>): void;
        getResultSet(keyPrefix: string): Promise<KvStoreResultSet>;

        /**
         * Obtains the {@code KvStoreResultSet} object matching the specified {@code Query} object.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A  
         * @param query Indicates the {@code Query} object.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        getResultSet(query: Query, callback: AsyncCallback<KvStoreResultSet>): void;
        getResultSet(query: Query): Promise<KvStoreResultSet>;

        /**
         * Closes a {@code KvStoreResultSet} object returned by getResultSet.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param resultSet Indicates the {@code KvStoreResultSet} object to close.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        closeResultSet(resultSet: KvStoreResultSet, callback: AsyncCallback<void>): void;
        closeResultSet(resultSet: KvStoreResultSet): Promise<void>;

        /**
         * Obtains the number of results matching the specified {@code Query} object.
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A 
         * @param query Indicates the {@code Query} object.
         * @returns Returns the number of results matching the specified {@code Query} object.
         * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
        getResultSize(query: Query, callback: AsyncCallback<number>): void;
        getResultSize(query: Query): Promise<number>;

        /**
         * void removeDeviceData​({@link String} deviceId) throws {@link KvStoreException}
         * 
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @import N/A
         */
        removeDeviceData(deviceId: string, callback: AsyncCallback<void>): void;
        removeDeviceData(deviceId: string): Promise<void>;

        /**
         * Synchronizes the database to the specified devices with the specified delay allowed.
         *
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param deviceIdList Indicates the list of devices to which to synchronize the database.
         * @param mode Indicates the synchronization mode. The value can be {@code PUSH}, {@code PULL}, or {@code PUSH_PULL}.
         * @param allowedDelayMs Indicates the delay allowed for the synchronization, in milliseconds.
         * @throws Throws this exception if any of the following errors
         * @permission ohos.permission.DISTRIBUTED_DATASYNC
         * occurs: {@code INVALID_ARGUMENT}, 
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
         */
         sync(deviceIdList: string[], mode: SyncMode, allowedDelayMs?: number): void;

         /**
          * Register Synchronizes SingleKvStore databases callback.
          * 
          * <p> Sync result is returned through asynchronous callback.
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.Core
          * @param syncCallback Indicates the callback used to send the synchronization result to the caller.
          * @throws Throws this exception if no {@code SingleKvStore} database is available.
          */
         on(event: 'syncComplete', syncCallback: Callback<Array<[string, number]>>): void;

         /**
          * UnRegister Synchronizes SingleKvStore databases callback.
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.Core
          * @throws Throws this exception if no {@code SingleKvStore} database is available.
          */
         off(event: 'syncComplete', syncCallback?: Callback<Array<[string, number]>>): void;
	 
	 
         /**
          * Sets the default delay allowed for database synchronization
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.Core
          * @param defaultAllowedDelayMs Indicates the default delay allowed for the database synchronization, in milliseconds.
          * @throws Throws this exception if any of the following errors occurs:{@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, and {@code DB_ERROR}.
          */
         setSyncParam(defaultAllowedDelayMs: number, callback: AsyncCallback<void>): void;
         setSyncParam(defaultAllowedDelayMs: number): Promise<void>;

         /**
          * Get the security level of the database.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.Core
          * @returns SecurityLevel {@code SecurityLevel} the security level of the database.
          * @throws Throws this exception if any of the following errors occurs:{@code SERVER_UNAVAILABLE},
          * {@code IPC_ERROR}, and {@code DB_ERROR}.
          */
         getSecurityLevel(callback: AsyncCallback<SecurityLevel>): void;
         getSecurityLevel(): Promise<SecurityLevel>;
    }

    /**
     * Manages distributed data by device in a distributed system.
     * 
     * <p>To create a {@code DeviceKVStore} database, you can use the {@link data.distributed.common.KVManager.getKvStore(Options, String)}
     * method with {@code KvStoreType} set to {@code DEVICE_COLLABORATION} for the input parameter Options. This database manages distributed
     * data by device, and cannot modify data synchronized from remote devices. When an application writes a key-value pair entry
     * into the database, the system automatically adds the ID of the device running the application to the key.
     * 
     * @since 8
     * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
     * @import N/A
     */
    interface DeviceKVStore extends KVStore {
        /**
         * Obtains the {@code String} value matching a specified device ID and key.
         *
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @param deviceId Indicates the device to be queried.
         * @param key Indicates the key of the value to be queried.
         * @return Returns the value matching the given criteria.
         * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}, and {@code KEY_NOT_FOUND}.
         */
         get(deviceId: string, key: string, callback: AsyncCallback<boolean|string|number|Uint8Array>): void;
         get(deviceId: string, key: string): Promise<boolean|string|number|Uint8Array>;

         /**
          * Obtains all key-value pairs matching a specified device ID and key prefix.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param deviceId Identifies the device whose data is to be queried.
          * @param keyPrefix Indicates the key prefix to match.
          * @returns Returns the list of all key-value pairs meeting the given criteria.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         getEntries(deviceId: string, keyPrefix: string, callback: AsyncCallback<Entry[]>): void;
         getEntries(deviceId: string, keyPrefix: string): Promise<Entry[]>;

         /**
          * Obtains the list of key-value pairs matching the specified {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param query Indicates the {@code Query} object.
          * @returns Returns the list of key-value pairs matching the specified {@code Query} object.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         getEntries(query: Query, callback: AsyncCallback<Entry[]>): void;
         getEntries(query: Query): Promise<Entry[]>;

         /**
          * Obtains the list of key-value pairs matching a specified device ID and {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore 
          * @param deviceId Indicates the ID of the device to which the key-value pairs belong.
          * @param query Indicates the {@code Query} object.
          * @returns Returns the list of key-value pairs matching the specified {@code Query} object.
          */
         getEntries(deviceId: string, query: Query, callback: AsyncCallback<Entry[]>): void;
         getEntries(deviceId: string, query: Query): Promise<Entry[]>;

         /**
          * Obtains the {@code KvStoreResultSet} object matching the specified device ID and key prefix.
          * 
          * <p>The {@code KvStoreResultSet} object can be used to query all key-value pairs that meet the search criteria. Each {@code KvStore}
          * instance can have a maximum of four {@code KvStoreResultSet} objects at the same time. If you have created four objects,
          * calling this method will return a failure. Therefore, you are advised to call the closeResultSet method to close unnecessary
          * {@code KvStoreResultSet} objects in a timely manner.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param deviceId Identifies the device whose data is to be queried.
          * @param keyPrefix Indicates the key prefix to match.
          * @returns Returns the {@code KvStoreResultSet} objects.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         getResultSet(deviceId: string, keyPrefix: string, callback: AsyncCallback<KvStoreResultSet>): void;
         getResultSet(deviceId: string, keyPrefix: string): Promise<KvStoreResultSet>;

         /**
          * Obtains the {@code KvStoreResultSet} object matching the specified {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param query Indicates the {@code Query} object.
          * @returns Returns the {@code KvStoreResultSet} object matching the specified {@code Query} object.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         getResultSet(query: Query, callback: AsyncCallback<KvStoreResultSet>): void;
         getResultSet(query: Query): Promise<KvStoreResultSet>;

         /**
          * Obtains the {@code KvStoreResultSet} object matching a specified device ID and {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param deviceId Indicates the ID of the device to which the {@code KvStoreResultSet} object belongs.
          * @param query Indicates the {@code Query} object.
          * @returns Returns the {@code KvStoreResultSet} object matching the specified {@code Query} object.
          */
         getResultSet(deviceId: string, query: Query, callback: AsyncCallback<KvStoreResultSet>): void;
         getResultSet(deviceId: string, query: Query): Promise<KvStoreResultSet>;

         /**
          * Closes a {@code KvStoreResultSet} object returned by getResultSet.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param resultSet Indicates the {@code KvStoreResultSet} object to close.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         closeResultSet(resultSet: KvStoreResultSet, callback: AsyncCallback<void>): void;
         closeResultSet(resultSet: KvStoreResultSet): Promise<void>;

         /**
          * Obtains the number of results matching the specified {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param query Indicates the {@code Query} object.
          * @returns Returns the number of results matching the specified {@code Query} object.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         getResultSize(query: Query, callback: AsyncCallback<number>): void;
         getResultSize(query: Query): Promise<number>;

         /**
          * Obtains the number of results matching a specified device ID and {@code Query} object.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param deviceId Indicates the ID of the device to which the results belong.
          * @param query Indicates the {@code Query} object.
          * @returns Returns the number of results matching the specified {@code Query} object.
          */
         getResultSize(deviceId: string, query: Query, callback: AsyncCallback<number>): void;
         getResultSize(deviceId: string, query: Query): Promise<number>;

         /**
          * Removes data of a specified device from the current database. This method is used to remove only the data
          * synchronized from remote devices. This operation does not synchronize data to other databases or affect
          * subsequent data synchronization.
          * 
          * @note N/A
          * @since 8
          * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
          * @param deviceId Identifies the device whose data is to be removed. The value cannot be the current device ID.
          * @throws Throws this exception if any of the following errors occurs: {@code INVALID_ARGUMENT},
          * {@code SERVER_UNAVAILABLE}, {@code IPC_ERROR}, {@code DB_ERROR}.
          */
         removeDeviceData(deviceId: string, callback: AsyncCallback<void>): void;
         removeDeviceData(deviceId: string): Promise<void>;
         
        /**
         * Synchronizes {@code DeviceKVStore} databases.
         *
         * <p>This method returns immediately and sync result will be returned through asynchronous callback.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param deviceIdList Indicates the list of IDs of devices whose
         * {@code DeviceKVStore} databases are to be synchronized.
         * @param mode Indicates the synchronization mode, {@code PUSH}, {@code PULL}, or
         * {@code PUSH_PULL}.
         * @permission ohos.permission.DISTRIBUTED_DATASYNC
         * @throws Throws this exception if no DeviceKVStore database is available.
         */
        sync(deviceIdList: string[], mode: SyncMode, allowedDelayMs?: number): void;

        /**
         * Register Synchronizes DeviceKVStore databases callback.
         * 
         * <p>Sync result is returned through asynchronous callback.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param syncCallback Indicates the callback used to send the synchronization result to the caller.
         * @throws Throws this exception if no DeviceKVStore database is available.
         */
        on(event: 'syncComplete', syncCallback: Callback<Array<[string, number]>>): void;

        /**
         * UnRegister Synchronizes DeviceKVStore databases callback.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @throws Throws this exception if no DeviceKVStore database is available.
         */
        off(event: 'syncComplete', syncCallback?: Callback<Array<[string, number]>>): void;
    }
    
    /**
     * Creates a {@link KVManager} instance based on the configuration information.
     *
     * <p>You must pass {@link KVManagerConfig} to provide configuration information
     * for creating the {@link KVManager} instance.
     *
     * @note N/A
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @param config Indicates the {@link KVStore} configuration information,
     * including the user information and package name.
     * @return Returns the {@code KVManager} instance.
     * @throws Throws exception if input is invalid.
     */
    function createKVManager(config: KVManagerConfig, callback: AsyncCallback<KVManager>): void;
    function createKVManager(config: KVManagerConfig): Promise<KVManager>;

    /**
     * Provides interfaces to manage a {@code KVStore} database, including obtaining, closing, and deleting the {@code KVStore}.
     *
     * @since 7
     * @syscap SystemCapability.DistributedDataManager.KVStore.Core
     * @import N/A
     * @version 1
     */
    interface KVManager {
        /**
         * Creates and obtains a {@code KVStore} database by specifying {@code Options} and {@code storeId}.
         *
         * @note N/A
         * @since 7
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param options Indicates the options used for creating and obtaining the {@code KVStore} database,
         * including {@code isCreateIfMissing}, {@code isEncrypt}, and {@code KVStoreType}.
         * @param storeId Identifies the {@code KVStore} database.
         * The value of this parameter must be unique for the same application,
         * and different applications can share the same value.
         * @return Returns a {@code KVStore}, or {@code SingleKVStore}.
         */
        getKVStore<T extends KVStore>(storeId: string, options: Options): Promise<T>;
        getKVStore<T extends KVStore>(storeId: string, options: Options, callback: AsyncCallback<T>): void;

        /**
         * Closes the {@code KvStore} database.
         * 
         * <p>Warning: This method is not thread-safe. If you call this method to stop a KvStore database that is running, your
         * thread may crash.
         * 
         * <p>The {@code KvStore} database to close must be an object created by using the {@code getKvStore} method. Before using this 
         * method, release the resources created for the database, for example, {@code KvStoreResultSet} for {@code SingleKvStore},
         * otherwise closing the database will fail. If you are attempting to close a database that is already closed, an error
         * will be returned.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param kvStore Indicates the {@code KvStore} database to close.
         * @throws Throws this exception if any of the following errors
         * occurs:{@code INVALID_ARGUMENT}, {@code ERVER_UNAVAILABLE},
         * {@code STORE_NOT_OPEN}, {@code STORE_NOT_FOUND}, {@code DB_ERROR},
         * {@code PERMISSION_DENIED}, and {@code IPC_ERROR}.
         */
        closeKVStore(appId: string, storeId: string, kvStore: KVStore, callback: AsyncCallback<void>): void;
        closeKVStore(appId: string, storeId: string, kvStore: KVStore): Promise<void>;

        /**
         * Deletes the {@code KvStore} database identified by storeId.
         * 
         * <p>Before using this method, close all {@code KvStore} instances in use that are identified by the same storeId.
         * 
         * <p>You can use this method to delete a {@code KvStore} database not in use. After the database is deleted, all its data will be
         * lost.
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core
         * @param storeId Identifies the {@code KvStore} database to delete.
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code INVALID_ARGUMENT},
         * {@code SERVER_UNAVAILABLE}, {@code STORE_NOT_FOUND},
         * {@code DB_ERROR}, {@code PERMISSION_DENIED}, and {@code IPC_ERROR}.
         */
        deleteKVStore(appId: string, storeId: string, callback: AsyncCallback<void>): void;
        deleteKVStore(appId: string, storeId: string): Promise<void>;

        /**
         * Obtains the storeId of all {@code KvStore} databases that are created by using the {@code getKvStore} method and not deleted by
         * calling the {@code deleteKvStore} method.
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.Core 
         * @returns Returns the storeId of all created {@code KvStore} databases. 
         * @throws Throws this exception if any of the following errors 
         * occurs: {@code SERVER_UNAVAILABLE}, {@code DB_ERROR},
         * {@code PERMISSION_DENIED}, and {@code IPC_ERROR}.
         */
        getAllKVStoreId(appId: string, callback: AsyncCallback<string[]>): void;
        getAllKVStoreId(appId: string): Promise<string[]>;

        /**
         * register DeviceChangeCallback to get notification when device's status changed
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @param deathCallback device change callback {@code DeviceChangeCallback}
         * @throws 	exception maybe occurs.
         */
        on(event: 'distributedDataServiceDie', deathCallback: Callback<void>): void;

        /**
         * unRegister DeviceChangeCallback and can not receive notification
         * 
         * @note N/A
         * @since 8
         * @syscap SystemCapability.DistributedDataManager.KVStore.DistributedKVStore
         * @param deathCallback device change callback {@code DeviceChangeCallback} which has been registered.
         * @throws exception maybe occurs.
         */
        off(event: 'distributedDataServiceDie', deathCallback?: Callback<void>): void;
    }
}

export default distributedData;