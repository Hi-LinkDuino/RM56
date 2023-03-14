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

import { AsyncCallback } from './basic'

export default filemanager;

declare namespace filemanager {
    export { listFile };
    export { getRoot };
    export { createFile };
    export { FileInfo };
    export { DevInfo };
}

/**
 * listFile.
 *
 * @note N/A
 * @syscap SystemCapability.FileManagement.UserFileService
 * @since 9
 * @permission N/A
 * @function listFile
 * @param {string} path - path.
 * @param {string} type - type.
 * @param {Object} options - options
 * @param {DevInfo} [options.dev = {name: "local"}] - dev name.
 * @param {number} [options.offset = 0] - offset.
 * @param {number} [options.count = 0] - count.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<FileInfo[]>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 * @systemapi
 */
declare function listFile(path: string, type: string, options?: {dev?: DevInfo, offset?: number, count?: number}): Promise<FileInfo[]>;
declare function listFile(path: string, type: string, options?: {dev?: DevInfo, offset?: number, count?: number}, callback: AsyncCallback<FileInfo[]>): void;

/**
 * getRoot.
 *
 * @note N/A
 * @syscap SystemCapability.FileManagement.UserFileService
 * @since 9
 * @permission N/A
 * @function getRoot
 * @param {Object} options - options
 * @param {DevInfo} [options.dev = {name: "local"}] - dev name.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<FileInfo>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 * @systemapi
 */
declare function getRoot(options?: {dev?: DevInfo}): Promise<FileInfo[]>;
declare function getRoot(options?: {dev?: DevInfo}, callback: AsyncCallback<FileInfo[]>): void;

/**
 * createFile.
 *
 * @note N/A
 * @syscap SystemCapability.FileManagement.UserFileService
 * @since 9
 * @permission N/A
 * @function createFile
 * @param {string} path - album uri.
 * @param {string} filename- file name.
 * @param {Object} options - options
 * @param {DevInfo} [options.dev = {name: "local"}] - dev name.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<string>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 * @systemapi
 */
declare function createFile(path: string, filename: string, options?: {dev?: DevInfo}): Promise<string>;
declare function createFile(path: string, filename: string, options?: {dev?: DevInfo}, callback: AsyncCallback<string>): void;

/**
 * FileInfo
 * @note N/A
 * @syscap SystemCapability.FileManagement.UserFileService
 * @since 9
 * @permission N/A
 * @systemapi
 */
declare interface FileInfo {
    /**
     * @type {string}
     * @readonly
     */
    name: string;
    /**
     * @type {string}
     * @readonly
     */
    path: string;
    /**
     * @type {string}
     * @readonly
     */
    type: string;
    /**
     * @type {string}
     * @readonly
     */
    size: number;
    /**
     * @type {string}
     * @readonly
     */
    addedTime: number;
    /**
     * @type {string}
     * @readonly
     */
    modifiedTime: number;
}

/**
 * DevInfo
 * @note N/A
 * @syscap SystemCapability.FileManagement.UserFileService
 * @since 9
 * @permission N/A
 * @systemapi
 */
 declare interface DevInfo {
    /**
     * @type {string}
     */
     name: string;
 }
