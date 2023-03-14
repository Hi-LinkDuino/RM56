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
import { AsyncCallback } from './basic';

/**
 * upload and download
 * 
 * @import request from '@ohos.request';
 * @permission ohos.permission.INTERNET
 */
declare namespace request {

  /**
   * Bit flag indicating download is allowed when using the cellular network.
   * @syscap SystemCapability.MiscServices.Download
   * @since 6
   * @permission ohos.permission.INTERNET
   */
  const NETWORK_MOBILE: number;

  /**
  * Bit flag indicating download is allowed when using the WLAN.
  * @syscap SystemCapability.MiscServices.Download
  * @since 6
  * @permission ohos.permission.INTERNET
  */
  const NETWORK_WIFI: number;

  /**
   * Indicates that the download cannot be resumed for some temporary errors.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_CANNOT_RESUME: number;

  /**
   * Indicates that no storage device, such as an SD card, is found.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_DEVICE_NOT_FOUND: number;

  /**
   * Indicates that files to be downloaded already exist, and that the download session cannot overwrite the existing files.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_FILE_ALREADY_EXISTS: number;

  /**
   * Indicates that a file operation fails.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_FILE_ERROR: number;

  /**
   * Indicates that the HTTP transmission fails.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_HTTP_DATA_ERROR: number;

  /**
   * Indicates insufficient storage space.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_INSUFFICIENT_SPACE: number;

  /**
   * Indicates an error caused by too many network redirections.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_TOO_MANY_REDIRECTS: number;

  /**
   * Indicates an HTTP code that cannot be identified.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_UNHANDLED_HTTP_CODE: number;

  /**
   * Indicates an undefined error.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const ERROR_UNKNOWN: number;

  /**
   * Indicates that the download is paused and waiting for a WLAN connection, because the file size exceeds the maximum allowed for a session using the cellular network.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const PAUSED_QUEUED_FOR_WIFI: number;

  /**
   * Indicates that the download is paused for some reasons.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const PAUSED_UNKNOWN: number;

  /**
   * Indicates that the download is paused due to a network problem, for example, network disconnection.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const PAUSED_WAITING_FOR_NETWORK: number;

  /**
   * Indicates that a network error occurs, and the download session will be retried.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const PAUSED_WAITING_TO_RETRY: number;

  /**
   * Indicates that the download session has failed and will not be retried.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const SESSION_FAILED: number;

  /**
   * Indicates that the download session has been paused.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const SESSION_PAUSED: number;

  /**
   * Indicates that the download session is being scheduled.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const SESSION_PENDING: number;

  /**
   * Indicates that the download session is in progress.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const SESSION_RUNNING: number;

  /**
   * Indicates that the download session is completed.
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  const SESSION_SUCCESSFUL: number;

  /**
   * Starts a download session.
   * @syscap SystemCapability.MiscServices.Download
   * @since 6
   * @param config download config
   * @param callback Indicate the callback function to receive DownloadTask.
   * @permission ohos.permission.INTERNET
   * @return -
   */
  function download(config: DownloadConfig, callback: AsyncCallback<DownloadTask>): void;

  /**
   * Starts a download session.
   * @syscap SystemCapability.MiscServices.Download
   * @since 6
   * @param config download config
   * @permission ohos.permission.INTERNET
   * @return -
   */
  function download(config: DownloadConfig): Promise<DownloadTask>;

  /**
   * Starts a upload session.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 6
   * @param config upload config
   * @param callback Indicate the callback function to receive UploadTask.
   * @permission ohos.permission.INTERNET
   * @return -
   */
  function upload(config: UploadConfig, callback: AsyncCallback<UploadTask>): void;

  /**
   * Starts a upload session.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 6
   * @param config upload config
   * @permission ohos.permission.INTERNET
   * @return -
   */
  function upload(config: UploadConfig): Promise<UploadTask>;

  /**
   * DownloadConfig data Structure
   *
   * @name DownloadConfig
   * @since 6
   * @syscap SystemCapability.MiscServices.Download
   * @permission ohos.permission.INTERNET
   */
  interface DownloadConfig {
    /**
     * Resource address.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    url: string;
    /**
     * Adds an HTTP or HTTPS header to be included with the download request.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    header?: Object;
    /**
     * Allows download under a metered connection.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    enableMetered?: boolean;
    /**
     * Allows download in a roaming network.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    enableRoaming?: boolean;
    /**
     * Sets the description of a download session.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    description?: string;
    /**
     * Sets the network type allowed for download.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    networkType?: number;
    /**
     * Sets the path for downloads.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    filePath?: string;
    /**
     * Sets a download session title.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    title?: string;
  }

  /**
   * DownloadInfo data Structure
   *
   * @name DownloadInfo
   * @syscap SystemCapability.MiscServices.Download
   * @since 7
   * @permission ohos.permission.INTERNET
   */
  interface DownloadInfo {
    /**
     * the description of a file to be downloaded.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    description: string;
    /**
     * the real-time downloads size (in bytes).
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */	
    downloadedBytes: number;
    /**
     * the ID of a file to be downloaded.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    downloadId: number;
    /**
     * a download failure cause, which can be any DownloadSession.ERROR_* constant.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    failedReason: number;
    /**
     * the name of a file to be downloaded.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    fileName: string;
    /**
     * the URI of a stored file.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    filePath: string;
    /**
     * the reason why a session is paused, which can be any DownloadSession.PAUSED_* constant.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    pausedReason: number;
    /**
     * the download status code, which can be any DownloadSession.SESSION_* constant.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    status: number;
    /**
     * the URI of files to be downloaded.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    targetURI: string;
    /**
     * the title of a file to be downloaded.
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    downloadTitle: string;
    /**
     * the total size of files to be downloaded (in bytes).
     *
     * @since 7
     * @permission ohos.permission.INTERNET
     */
    downloadTotalBytes: number;
  }

  interface DownloadTask {
    /**
     * Called when the current download session is in process.
     * @syscap SystemCapability.MiscServices.Download
     * @since 6
     * @param type progress Indicates the download task progress.
     * @param callback The callback function for the download progress change event
     *        receivedSize the length of downloaded data, in bytes
     *        totalSize he length of data expected to be downloaded, in bytes.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    on(type: 'progress', callback: (receivedSize: number, totalSize: number) => void): void;

    /**
     * Called when the current download session is in process.
     * @syscap SystemCapability.MiscServices.Download
     * @since 6
     * @param type progress Indicates the download task progress.
     * @param callback The callback function for the download progress change event
     *        receivedSize the length of downloaded data, in bytes
     *        totalSize he length of data expected to be downloaded, in bytes.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    off(type: 'progress', callback?: (receivedSize: number, totalSize: number) => void): void;

    /**
     * Called when the current download session complete、pause or remove.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param type Indicates the download session event type
     *        complete: download task completed,
     *        pause: download task stopped,
     *        remove: download task deleted.
     * @param callback The callback function for the download complete、pause or remove change event.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    on(type: 'complete' | 'pause' | 'remove', callback: () => void): void;

    /**
     * Called when the current download session complete、pause or remove.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param type Indicates the download session event type
     *        complete: download task completed,
     *        pause: download task stopped,
     *        remove: download task deleted.
     * @param callback The callback function for the download complete、pause or remove change event.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    off(type: 'complete' | 'pause' | 'remove', callback?: () => void): void;

    /**
     * Called when the current download session fails.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param type Indicates the download session type, fail: download task has failed.
     * @param callback The callback function for the download fail change event
     *        err The error code for download task.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    on(type: 'fail', callback: (err: number) => void): void;

    /**
     * Called when the current download session fails.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param type Indicates the download session type, fail: download task has failed.
     * @param callback Indicate the callback function to receive err.
     *        err The error code for download task.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    off(type: 'fail', callback?: (err: number) => void): void;

    /**
     * Deletes a download session and the downloaded files.
     * @syscap SystemCapability.MiscServices.Download
     * @since 6
     * @param callback Indicates asynchronous invoking Result.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    remove(callback: AsyncCallback<boolean>): void;

    /**
     * Deletes a download session and the downloaded files.
     * @syscap SystemCapability.MiscServices.Download
     * @since 6
     * @permission ohos.permission.INTERNET
     * @return -
     */
    remove(): Promise<boolean>;

    /**
     * Pause a download session.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param callback Indicates asynchronous invoking Result.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    pause(callback: AsyncCallback<void>): void;

    /**
     * Pause a download session.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @permission ohos.permission.INTERNET
     * @return -
     */
    pause(): Promise<void>;

    /**
     * Resume a paused download session.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param callback Indicates asynchronous invoking Result.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    resume(callback: AsyncCallback<void>): void;

    /**
     * Resume a paused download session.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @permission ohos.permission.INTERNET
     * @return -
     */
    resume(): Promise<void>;

    /**
     * Queries download information of a session, which is defined in DownloadSession.DownloadInfo.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param callback Indicate the callback function to receive download info.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    query(callback: AsyncCallback<DownloadInfo>): void;

    /**
     * Queries download information of a session, which is defined in DownloadSession.DownloadInfo.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @permission ohos.permission.INTERNET
     * @return -
     */
    query(): Promise<DownloadInfo>;

    /**
     * Queries the MIME type of the download file.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @param callback Indicate the callback function to receive download file MIME type.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    queryMimeType(callback: AsyncCallback<string>): void;

    /**
     * Queries the MIME type of the download file.
     * @syscap SystemCapability.MiscServices.Download
     * @since 7
     * @permission ohos.permission.INTERNET
     * @return -
     */
    queryMimeType(): Promise<string>;
  }

  /**
   * File data Structure
   *
   * @name File
   * @since 6
   * @syscap SystemCapability.MiscServices.Download
   * @permission ohos.permission.INTERNET
   */
  interface File {
    /**
     * When multipart is submitted, the file name in the request header.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    filename: string;
    /**
     * When multipart is submitted, the name of the form item. The default is file.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    name: string;
    /**
     * The local storage path of the file (please refer to the storage directory definition for path usage).
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    uri: string;
    /**
     * The content type of the file is obtained by default according to the suffix of the file name or path.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    type: string;
  }

  /**
   * RequestData data Structure
   *
   * @name RequestData
   * @since 6
   * @syscap SystemCapability.MiscServices.Download
   * @permission ohos.permission.INTERNET
   */
  interface RequestData {
    /**
     * Represents the name of the form element.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    name: string;
    /**
     * Represents the value of the form element.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    value: string;
  }

  /**
   * UploadConfig data Structure
   *
   * @name UploadConfig
   * @since 6
   * @syscap SystemCapability.MiscServices.Upload
   * @permission ohos.permission.INTERNET
   */
  interface UploadConfig {
    /**
     * Resource address.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    url: string;
    /**
     * Adds an HTTP or HTTPS header to be included with the upload request.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    header: Object;
    /**
     * Request method: POST, PUT. The default POST.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    method: string;
    /**
     * A list of files to be uploaded. Please use multipart/form-data to submit.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    files: Array<File>;
    /**
     * The requested form data.
     *
     * @since 6
     * @permission ohos.permission.INTERNET
     */
    data: Array<RequestData>;
  }

  interface UploadTask {
    /**
     * Called when the current upload session is in process.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 6
     * @param type progress Indicates the upload task progress.
     * @param callback The callback function for the upload progress change event
     *        uploadedSize The length of uploaded data, in bytes
     *        totalSize The length of data expected to be uploaded, in bytes.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    on(type: 'progress', callback: (uploadedSize: number, totalSize: number) => void): void;

    /**
     * Called when the current upload session is in process.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 6
     * @param type progress Indicates the upload task progress.
     * @param callback The callback function for the upload progress change event
     *        uploadedSize The length of uploaded data, in bytes
     *        totalSize The length of data expected to be uploaded, in bytes.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    off(type: 'progress', callback?: (uploadedSize: number, totalSize: number) => void): void;

    /**
     * Called when the header of the current upload session has been received.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 7
     * @param type headerReceive Indicates the upload task headed receive.
     * @param callback The callback function for the HTTP Response Header event
     *        header HTTP Response Header returned by the developer server.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    on(type: 'headerReceive', callback: (header: object) => void): void;

    /**
     * Called when the header of the current upload session has been received.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 7
     * @param type headerReceive Indicates the upload task headed receive.
     * @param callback The callback function for the HTTP Response Header event
     *        header HTTP Response Header returned by the developer server.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    off(type: 'headerReceive', callback?: (header: object) => void): void;

    /**
     * Deletes a upload session.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 6
     * @param callback Indicates asynchronous invoking Result.
     * @permission ohos.permission.INTERNET
     * @return -
     */
    remove(callback: AsyncCallback<boolean>): void;

    /**
     * Deletes a upload session.
     * @syscap SystemCapability.MiscServices.Upload
     * @since 6
     * @permission ohos.permission.INTERNET
     * @return -
     */
    remove(): Promise<boolean>;
  }
}

export default request;
	
