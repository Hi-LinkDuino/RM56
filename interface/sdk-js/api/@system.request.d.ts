/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * @syscap SystemCapability.MiscServices.Upload
 */
export interface UploadResponse {
  /**
   * Status code returned by the server.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  code: number;

  /**
   * Content returned by the server.
   * The value type is determined by the returned content.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  data: string;

  /**
   * Headers returned by the server.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  headers: Object;
}

/**
 * @syscap SystemCapability.MiscServices.Download
 */
export interface DownloadResponse {
  /**
   * Download token, which is used to obtain the download status.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  token: string;
}

/**
 * @syscap SystemCapability.MiscServices.Download
 */
export interface OnDownloadCompleteResponse {
  /**
   * URI of the download file.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  uri: string;
}

/**
 * @syscap SystemCapability.MiscServices.Upload
 */
export interface RequestFile {
  /**
   * File name in the header when multipart is used.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  filename?: string;

  /**
   * Name of a form item when multipart is used. The default value is file.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  name?: string;

  /**
   * Local storage directory of a file.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  uri: string;

  /**
   * Type of the file content.
   * By default, the type is obtained based on the suffix of the file name or URI.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  type?: string;
}

/**
 * @syscap SystemCapability.MiscServices.Upload
 */
export interface RequestData {
  /**
   * Name of the form element.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  name: string;

  /**
   * Value of the form element.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  value: string;
}

/**
 * @syscap SystemCapability.MiscServices.Upload
 */
export interface UploadRequestOptions {
  /**
   * Resource URL.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  url: string;

  /**
   * Form data in the request body.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  data?: Array<RequestData>;

  /**
   * List of files to upload, which is submitted through multipart/form-data.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  files: Array<RequestFile>;

  /**
   * Request header.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  header?: Object;

  /**
   * Request methods available: POST and PUT. The default value is POST.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  method?: string;

  /**
   * Called when the files are uploaded successfully.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  success?: (data: UploadResponse) => void;

  /**
   * Called when uploading fails.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  fail?: (data: any, code: number) => void;

  /**
   * Called when the execution is completed.
   * @syscap SystemCapability.MiscServices.Upload
   * @since 3
   */
  complete?: () => void;
}

/**
 * @syscap SystemCapability.MiscServices.Download
 */
export interface DownloadRequestOptions {
  /**
   * Resource URL.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  url: string;

  /**
   * Name of the file to downloaded.
   * The value is obtained from the current request or resource URL by default.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  filename?: string;

  /**
   * Request header.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  header?: string;

  /**
   * Download description.
   * The default value is the file name.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  description?: string;

  /**
   * Called when the files are successfully downloaded.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  success?: (data: DownloadResponse) => void;

  /**
   * Called when downloading fails.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  fail?: (data: any, code: number) => void;

  /**
   * Called when the execution is completed.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  complete?: () => void;
}

/**
 * @syscap SystemCapability.MiscServices.Download
 */
export interface OnDownloadCompleteOptions {
  /**
   * Token of the result returned by the download function.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  token: string;

  /**
   * Called when the downloads are successfully obtained
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  success?: (data: OnDownloadCompleteResponse) => void;

  /**
   * Called when the downloads fail to be obtained.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  fail?: (data: any, code: number) => void;

  /**
   * Called when the execution is completed.
   * @syscap SystemCapability.MiscServices.Download
   * @since 3
   */
  complete?: () => void;
}

export default class Request {
  /**
   * Upload files.
   * @param options Options.
   * @syscap SystemCapability.MiscServices.Upload
   */
  static upload(options: UploadRequestOptions): void;

  /**
   * This API is used to download files.
   * @param options Options.
   * @syscap SystemCapability.MiscServices.Download
   */
  static download(options: DownloadRequestOptions): void;

  /**
   * Listens to download task status.
   * @param options Options.
   * @syscap SystemCapability.MiscServices.Download
   */
  static onDownloadComplete(options: OnDownloadCompleteOptions): void;
}