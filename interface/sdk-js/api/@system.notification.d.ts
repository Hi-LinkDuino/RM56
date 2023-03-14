/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

/**
 * @name ActionResult
 * @since 3
 * @syscap SystemCapability.Notification.Notification
 */
export interface ActionResult {
  /**
   * Bundle name of the application to be redirected to after the notification is clicked.
   */
  bundleName: string;

  /**
   * Ability name of the application to be redirected to after the notification is clicked.
   */
  abilityName: string;

  /**
   * URI of the page to be redirected to. The supported URI formats are as follows:
   * 1. Absolute path of the page, which is provided by the pages list in the config.json file. Example:
   * pages/index/index
   * pages/detail/detail
   * 2. Particular path. If the value is a slash (/), the home page is displayed.
   */
  uri: string;
}

/**
 * @name ShowNotificationOptions
 * @since 3
 * @syscap SystemCapability.Notification.Notification
 */
export interface ShowNotificationOptions {
  /**
   * Notification title.
   */
  contentTitle?: string;

  /**
   * Notification content.
   */
  contentText?: string;

  /**
   * Action triggered after the notification is clicked.
   */
  clickAction?: ActionResult;
}

/**
 * @name Notification
 * @since 3
 * @syscap SystemCapability.Notification.Notification
 */
export default class Notification {
  /**
   * Displays the notification.
   * @param options Options.
   */
  static show(options?: ShowNotificationOptions): void;
}
