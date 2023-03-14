/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import { AsyncCallback } from "./basic";
import { ResultSet } from './data/rdb/resultSet';
import ExtensionContext from "./application/ExtensionContext";
import Want from './@ohos.application.Want';
import dataAbility from './@ohos.data.dataAbility';
import rdb from './@ohos.data.rdb';

/**
 * class of datashare extension ability.
 *
 * @since 9
 * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
 * @systemapi Hide this for inner system use.
 * @StageModelOnly
 */
export default class DataShareExtensionAbility {
    /**
     * Opens a file in a specified remote path.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     *             (erasing whatever data is currently in the file), "wt" for write access that truncates any existing
     *             file, "wa" for write-only access to append to any existing data, "rw" for read and write access on
     *             any existing data, or "rwt" for read and write access that truncates any existing file.
     * @param callback Indicates the callback when openfile success
     * @return Returns the file descriptor.
     * @StageModelOnly
     */
    openFile(uri: string, mode: string, callback: AsyncCallback<number>): void;
    openFile(uri: string, mode: string): Promise<number>;
    /**
     * Indicates datashare extension ability context.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @systemapi Hide this for inner system use.
     * @StageModelOnly
     */
    context?: ExtensionContext;

    /**
     * Called back when a datashare extension ability is started for initialization.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param want Indicates connection information about the datashare extension ability.
     * @systemapi Hide this for inner system use.
     * @return -
     * @StageModelOnly
     */
    onCreate?(want: Want): void;

    /**
     * Obtains the MIME type of files. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME type of the files to obtain. This parameter cannot be set to {@code
     *     null}.
     *     <p>1. "&ast;/*": Obtains all types supported by a data share.
     *     <p>2. "image/*": Obtains files whose main type is image of any subtype.
     *     <p>3. "&ast;/jpg": Obtains files whose subtype is JPG of any main type.
     * @systemapi Hide this for inner system use.
     * @return Returns the MIME type of the matched files; returns null if there is no type that matches the Data
     * @StageModelOnly
     */
    getFileTypes?(uri: string, mimeTypeFilter: string, callback: AsyncCallback<Array<string>>): void;

    /**
     * Inserts a data record into the database. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the position where the data is to insert.
     * @param valueBucket Indicates the data to insert.
     * @systemapi Hide this for inner system use.
     * @return Returns the index of the newly inserted data record.
     * @StageModelOnly
     */
    insert?(uri: string, valueBucket: rdb.ValuesBucket, callback: AsyncCallback<number>): void;

    /**
     * Updates one or more data records in the database. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the database table storing the data to update.
     * @param valueBucket Indicates the data to update. This parameter can be null.
     * @param predicates Indicates filter criteria. If this parameter is null, all data records will be updated by
     *        default.
     * @systemapi Hide this for inner system use.
     * @return Returns the number of data records updated.
     * @StageModelOnly
     */
    update?(uri: string, valueBucket: rdb.ValuesBucket, predicates: dataAbility.DataAbilityPredicates,
        callback: AsyncCallback<number>): void;

    /**
     * Deletes one or more data records. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the database table storing the data to delete.
     * @param predicates Indicates filter criteria. If this parameter is null, all data records will be deleted by
     *     default.
     * @systemapi Hide this for inner system use.
     * @return Returns the number of data records deleted.
     * @StageModelOnly
     */
    delete?(uri: string, predicates: dataAbility.DataAbilityPredicates, callback: AsyncCallback<number>): void;

    /**
     * Queries one or more data records in the database. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the database table storing the data to query.
     * @param columns Indicates the columns to be queried, in array, for example, {"name","age"}. You should define
     *                the processing logic when this parameter is null.
     * @param predicates Indicates filter criteria. If this parameter is null, all data records will be queried by
     *                   default.
     * @systemapi Hide this for inner system use.
     * @return Returns the queried data.
     * @StageModelOnly
     */
    query?(uri: string, columns: Array<string>, predicates: dataAbility.DataAbilityPredicates,
        callback: AsyncCallback<ResultSet>): void;

    /**
     * Obtains the MIME type matching the data specified by the URI of the data share. This method should be
     * implemented by a data share.
     *
     * <p>Data abilities supports general data types, including text, HTML, and JPEG.</p>
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the uri of the data.
     * @systemapi Hide this for inner system use.
     * @return Returns the MIME type that matches the data specified by {@code uri}.
     * @StageModelOnly
     */
    getType?(uri: string, callback: AsyncCallback<string>): void;

    /**
     * Inserts multiple data records into the database. This method should be implemented by a data share.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the position where the data is to insert.
     * @param valueBuckets Indicates the data to insert.
     * @systemapi Hide this for inner system use.
     * @return Returns the number of data records inserted.
     * @StageModelOnly
     */
    batchInsert?(uri: string, valueBuckets: Array<rdb.ValuesBucket>, callback: AsyncCallback<number>): void;

    /**
     * Converts the given {@code uri} that refer to the data share into a normalized URI. A normalized URI can be
     * used across devices, persisted, backed up, and restored. It can refer to the same item in the data share
     * even if the context has changed.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the uri to normalize.
     * @systemapi Hide this for inner system use.
     * @return Returns the normalized uri if the data share supports URI normalization;
     * @StageModelOnly
     */
    normalizeUri?(uri: string, callback: AsyncCallback<string>): void;

    /**
     * Converts the given normalized {@code uri} generated by {@link #normalizeUri(uri)} into a denormalized one.
     * The default implementation of this method returns the original uri passed to it.
     *
     * @since 9
     * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
     * @param uri Indicates the uri to denormalize.
     * @systemapi Hide this for inner system use.
     * @return Returns the denormalized {@code uri} object if the denormalization is successful; returns the original
     * {@code uri} passed to this method if there is nothing to do; returns {@code null} if the data identified by
     * the original {@code uri} cannot be found in the current environment.
     * @StageModelOnly
     */
    denormalizeUri?(uri: string, callback: AsyncCallback<string>): void;
}