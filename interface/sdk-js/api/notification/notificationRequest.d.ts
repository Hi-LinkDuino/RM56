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

import notification from '../@ohos.notification';
import image from '../@ohos.multimedia.image';
import { WantAgent } from '../@ohos.wantAgent';
import { NotificationContent } from './notificationContent';
import { NotificationActionButton } from './notificationActionButton';
import { NotificationTemplate } from './notificationTemplate';
import { NotificationFlags } from './notificationFlags';

/**
 * Defines a NotificationRequest instance.
 *
 * @name NotificationRequest
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationRequest {
  /**
   * Notification content.
   */
  content: NotificationContent;

  /**
   * Notification ID.
   */
  id?: number;

  /**
   * Notification slot type.
   */
  slotType?: notification.SlotType;

  /**
   * Whether the notification is an ongoing notification.
   */
  isOngoing?: boolean;

  /**
   * Whether the notification can be removed.
   */
  isUnremovable?: boolean;

  /**
   * Time when the notification is sent.
   */
  deliveryTime?: number;

  /**
   * Whether the notification is automatically cleared.
   */
  tapDismissed?: boolean;

  /**
   * Time when the notification is automatically cleared.
   */
  autoDeletedTime?: number;

  /**
   * WantAgent instance to which the notification will be redirected after being clicked.
   */
  wantAgent?: WantAgent;

  /**
   * Extended parameter.
   */
  extraInfo?: {[key: string]: any};

  /**
   * Background color of the notification.
   */
  color?: number;

  /**
   * Whether the notification background color can be enabled.
   */
  colorEnabled?: boolean;

  /**
   * Whether the notification triggers an alert only once.
   */
  isAlertOnce?: boolean;

  /**
   * Whether to display the stopwatch.
   */
  isStopwatch?: boolean;

  /**
   * Whether to display the countdown time.
   */
  isCountDown?: boolean;

  /**
   * Whether the notification is displayed as a floating icon.
   */
  isFloatingIcon?: boolean;

  /**
   * Notification label.
   */
  label?: string;

  /**
   * Notification badge type.
   */
  badgeIconStyle?: number;

  /**
   * Whether to display the time when the notification is delivered.
   */
  showDeliveryTime?: boolean;

  /**
   * Buttons in the notification. Up to two buttons are allowed.
   */
  actionButtons?: Array<NotificationActionButton>;

  /**
   * Small notification icon.
   */
  smallIcon?: image.PixelMap;

  /**
   * Large notification icon.
   */
  largeIcon?: image.PixelMap;

  /**
   * The group information for this notification.
   *
   * @since 8
   */
  groupName?: string;

  /**
   * Read-only name of the package for which a notification is created.
   */
  readonly creatorBundleName?: string;

  /**
   * Read-only UID of the notification creator.
   */
  readonly creatorUid?: number;

  /**
   * Read-only PID of the notification creator.
   */
  readonly creatorPid?: number;

  /**
   * @since 8
   * Read-only UserId of the notification creator.
   */
  readonly creatorUserId?: number;

  /**
   * Obtains the classification of this notification.
   *
   * @systemapi Hide this for inner system use.
   */
  classification?: string;

  /**
   * Obtains the unique hash code of a notification in the current application.
   */
  readonly hashCode?: string;

  /**
   * Whether the notification can be remove.
   *
   * @default true
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  readonly isRemoveAllowed?: boolean;

  /**
   * Notification source. enum SourceType
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  readonly source?: number;

  /**
   * Obtains the template of this notification.
   *
   * @since 8
   */
  template?: NotificationTemplate;

  /**
   * The options to distributed notification.
   *
   * @since 8
   */
  distributedOption?: DistributedOptions;

  /**
   * The device ID of the notification source.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   */
  readonly deviceId?: string;

  /**
   * Obtains the set of identifiers for the notification.
   *
   * @since 8
   */
   readonly notificationFlags?: NotificationFlags;
}

/**
 * Describes distributed options.
 *
 * @name DistributedOptions
 * @since 8
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface DistributedOptions {
  /**
   * Obtains whether is the distributed notification.
   *
   * @default true
   */
  isDistributed?: boolean;

  /**
   * Obtains the types of devices to which the notification can be synchronized.
   */
  supportDisplayDevices?: Array<string>;

  /**
   * Obtains the devices on which notifications can be open.
   */
  supportOperateDevices?: Array<string>;

  /**
   * Obtains the remind mode of the notification. enum DeviceRemindType.

   * @systemapi Hide this for inner system use.
   */
  readonly remindType?: number;
}
