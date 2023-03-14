/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
 * the information of the subscriber
 * @name CommonEventSubscribeInfo
 * @since 7
 * @syscap SystemCapability.Notification.CommonEvent
 * @permission N/A
 */
export interface CommonEventSubscribeInfo {
  /**
   * The permission that the publisher must have in order to send a common event to this subscriber.
   * This subscriber receives only common events sent by publishers granted with this permission.
   *
   * @default ""
   * @since 7
   */
  events: Array<string>;

  /**
   * The permission that the publisher must have in order to send a common event to this subscriber.
   * This subscriber receives only common events sent by publishers granted with this permission.
   *
   * @default ""
   * @since 7
   */
  publisherPermission?: string;

  /**
   * deviceId Indicates the device ID. The value must be an existing device ID on the same ohos network.
   *
   * @default ""
   * @since 7
   */
  publisherDeviceId?: string;

  /**
   * Indicates the user ID. This parameter is optional, and the default value is the ID of the
  * current user. If this parameter is specified, the value must be an existing user ID in the system.
   *
   * @default ""
   * @since 7
   */
  userId?: number;

  /**
   * Indicates the subscriber priority. The value ranges from -100 to 1000.
   *
   * @default ""
   * @since 7
   */
  priority?: number;
}
