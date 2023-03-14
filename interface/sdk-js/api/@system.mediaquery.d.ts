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
 * Defines the MediaQuery event.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export interface MediaQueryEvent {
  /**
   * The result of match result.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  matches: boolean;
}

/**
 * Defines the MediaQuery list info.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export interface MediaQueryList {
  /**
   * Serialized media query condition.
   * This parameter is read-only.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  media?: string;

  /**
   * Whether the query is successful. True if the query condition is matched successfully, false otherwise.
   * This parameter is read-only.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  matches?: boolean;

  /**
   * Called when the matches value changes.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  onchange?: (matches: boolean) => void;

  /**
   * Adds a listening function to MediaQueryList.
   * The listening function must be added before onShow is called, that is, added to the onInit or onReady function.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  addListener(callback: (event: MediaQueryEvent) => void): void;

  /**
   * Removes a listening function from MediaQueryList.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  removeListener(callback: (event: MediaQueryEvent) => void): void;
}

/**
 * Defines the mediaqurey interface.
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 3
 */
export default class MediaQuery {
  /**
   * Queries a media item and returns a MediaQueryList object.
   * @syscap SystemCapability.ArkUI.ArkUI.Full
   * @since 3
   */
  static matchMedia(condition: string): MediaQueryList;
}
