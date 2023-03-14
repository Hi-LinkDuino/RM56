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
 * the data of the commonEvent
 * @name CommonEventData
 * @syscap SystemCapability.Notification.CommonEvent
 * @since 7
 * @permission N/A
 */
export interface CommonEventData {
  /**
   * event type
   * @default -
   * @since 7
   */
  event: string

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
   * The description of the parameters in a common event.
   *
   * @default -
   * @since 7
   */
  parameters?: {[key: string]: any};
}
