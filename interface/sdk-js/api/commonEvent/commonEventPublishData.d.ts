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
 * containing the common event content and attributes
 * @name CommonEventPublishData
 * @since 7
 * @syscap SystemCapability.Notification.CommonEvent
 * @permission N/A
 */
export interface CommonEventPublishData {
  /**
   * bundle name
   * @default -
   * @since 7
   */
  bundleName?: string;

  /**
   * The custom result code of the common event.
   *
   * @default 0
   * @since 7
   */
  code?: number;

  /**
   * The custom result data of the common event.
   *
   * @default ""
   * @since 7
   */
  data?: string;

  /**
   * The permissions for subscribers. Only subscribers with required permissions can receive published common events.
   *
   * @default -
   * @since 7
   */
  subscriberPermissions?: Array<string>;

  /**
   * Whether the type of a common event is ordered or not.
   *
   * @default false
   * @since 7
   */
  isOrdered?: boolean;

  /**
   * Whether the type of a common event is sticky or not.
   *
   * @default false
   * @since 7
   */
  isSticky?: boolean;

  /**
   * The description of the parameters in a common event.
   *
   * @default -
   * @since 7
   */
  parameters?: {[key: string]: any};
}
