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

import { ElementName } from '../bundle/elementName';
import rpc from './../@ohos.rpc';

/**
 * @since 7
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 */
export interface ConnectOptions {
  /**
   * The callback interface was connect successfully.
   *
   * @default -
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   * @param elementName The element name of the service ability
   * @param remote The remote object instance
   */
  onConnect(elementName: ElementName, remote: rpc.IRemoteObject): void;

  /**
   * The callback interface was disconnect successfully.
   *
   * @default -
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   * @param elementName The element name of the service ability
   */
  onDisconnect(elementName: ElementName): void;

  /**
   * The callback interface was connect failed.
   *
   * @default -
   * @since 7
   * @syscap SystemCapability.Ability.AbilityRuntime.Core
   * @param code The error code of the failed.
   */
  onFailed(code: number): void;
}
