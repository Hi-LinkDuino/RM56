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
 * Defines the AppResponse info.
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export interface AppResponse {
  /**
   * Application bundleName.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 6
   */
  appID: string;

  /**
   * Application name.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  appName: string;

  /**
   * Application version name.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  versionName: string;

  /**
   * Application version.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  versionCode: number;
}

/**
 * Defines the option of screenOnVisible interface.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export interface ScreenOnVisibleOptions {
  /**
   * Whether to keep the application visible. The default value is false.
   * @since 3
   */
  visible?: boolean;

  /**
   * Called when the application always keeps visible.
   * @since 3
   */
  success?: () => void;

  /**
   * Called when the application fails to keep visible.
   * @since 3
   */
  fail?: (data: string, code: number) => void;

  /**
   * Called when the execution is completed.
   * @since 3
   */
  complete?: () => void;
}

export interface CallbackOptions {
  callback?: (status: number) => void;

  fail?: () => void;
}

/**
 * Defines the option of RequestFullWindow interface.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export interface RequestFullWindowOptions {
  /**
   * Defines the number of animation options.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  duration: number;
}

/**
 * Defines the app class info.
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export default class App {
  /**
   * Obtains the declared information in the config.json file of an application.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  static getInfo(): AppResponse;

  /**
   * Destroys the current ability.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  static terminate(): void;
  static test_method():void;
  static test_callback(options?: CallbackOptions): void;

  /**
   * Requests the application to run in full window.
   * In some scenarios, such as semi-modal FA, the FA runs in non-full window.
   * In this case, you can call this API.
   * This API is invalid for an application already in full-window mode.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param options Transition time from non-full window to full window, in milliseconds.
   * By default, the value is in direct proportion to the distance between the non-full window and the full window.
   * @since 3
   * @systemapi
   */
  static requestFullWindow(options?: RequestFullWindowOptions): void;

  /**
   * Set image cache capacity of decoded image count.
   * if not set, the application will not cache any decoded image.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param value capacity of decoded image count.
   */
  static setImageCacheCount(value: number): void;

  /**
   * Set image cache capacity of raw image data size in bytes before decode.
   * if not set, the application will not cache any raw image data.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param value capacity of raw image data size in bytes.
   */
  static setImageRawDataCacheSize(value: number): void;

  /**
   * Set image file cache size in bytes on disk before decode.
   * if not set, the application will cache 100MB image files on disk.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @param value capacity of raw image data size in bytes.
   */
  static setImageFileCacheSize(value: number): void;
}
