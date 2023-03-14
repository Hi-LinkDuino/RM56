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

import { NotificationUserInput } from './notificationUserInput';
import { WantAgent } from '../@ohos.wantAgent';

/**
 * Describes an action button displayed in a notification.
 * @name NotificationActionButton
 * @since 7
 * @permission N/A
 * @syscap SystemCapability.Notification.Notification
 */
export interface NotificationActionButton {
  /**
   * Button title.
   */
  title: string;

  /**
   * WantAgent of the button.
   */
  wantAgent: WantAgent;

  /**
   * Extra information of the button.
   */
  extras?: {[key: string]: any};

  /**
   * User input
   *
   * @since 8
   */
  userInput?: NotificationUserInput;
}
