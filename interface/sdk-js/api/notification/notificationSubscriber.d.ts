/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import { NotificationRequest } from './notificationRequest';
import { NotificationSortingMap } from './notificationSortingMap';
import notification from '../@ohos.notification';

/**
 * Provides methods that will be called back when the subscriber receives a new notification or
 * a notification is canceled.
 *
 * @name NotificationSubscriber
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 * @systemapi Hide this for inner system use.
 * @since 7
 */
export interface NotificationSubscriber {
  onConsume?:(data: SubscribeCallbackData) => void;
  onCancel?:(data: SubscribeCallbackData) => void;
  onUpdate?:(data: NotificationSortingMap) => void;
  onConnect?:() => void;
  onDisconnect?:() => void;
  onDestroy?:() => void;

  /**
   * Callback when the Do Not Disturb setting changed.
   * @syscap SystemCapability.Notification.Notification
   * @since 8
   */
  onDoNotDisturbDateChange?:(mode: notification.DoNotDisturbDate) => void;

  /**
   * Callback when the notificaition permission is changed.
   * @syscap SystemCapability.Notification.Notification
   * @since 8
   */
  onEnabledNotificationChanged?:(callbackData: EnabledNotificationCallbackData) => void;
}

/**
 * Provides methods that will be called back when the subscriber receives a new notification or
 * a notification is canceled.
 *
 * @name SubscribeCallbackData
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 * @systemapi Hide this for inner system use.
 * @since 7
 */
export interface SubscribeCallbackData {
  readonly request: NotificationRequest;
  readonly sortingMap?: NotificationSortingMap;
  readonly reason?: number;
  readonly sound?: string;
  readonly vibrationValues?: Array<number>;
}

/**
 * Describes the properties of the application that the permission to send notifications has changed.
 *
 * @name EnabledNotificationCallbackData
 * @syscap SystemCapability.Notification.Notification
 * @systemapi Hide this for inner system use.
 * @since 8
 */
export interface EnabledNotificationCallbackData {
  readonly bundle: string;
  readonly uid: number;
  readonly enable: boolean;
}
