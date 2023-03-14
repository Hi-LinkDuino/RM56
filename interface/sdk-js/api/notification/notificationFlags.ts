/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * The status of the notification flag.
 *
 * @since 8
 * @systemapi Hide this for inner system use.
 * @syscap SystemCapability.Notification.Notification
 */
  export enum NotificationFlagStatus {
  /**
   * notification flag default value
   */
  TYPE_NONE = 0,

  /**
   * notification flag open
   */
   TYPE_OPEN = 1,

  /**
   * notification flag close
   */
   TYPE_CLOSE = 2,
}

/**
 * Describes a NotificationFlags instance.
 *
 * @name NotificationFlags
 * @since 8
 * @permission N/A
 * @syscap SystemCapability.Notification.Notification
 */
 export interface NotificationFlags {
  /**
   * Whether to enable sound reminder.
   */
  readonly soundEnabled?: NotificationFlagStatus;

  /**
   * Whether to enable vibration reminder.
   */
  readonly vibrationEnabled?: NotificationFlagStatus;
}
