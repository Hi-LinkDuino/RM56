/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides filesystem statistics APIs
 *
 * @since 8
 * @syscap SystemCapability.FileManagement.File.FileIO
 */
declare namespace Statfs {
  /**
   * Get the number of free bytes on the specified path.
   *
   * @since 8
   */
  function getFreeBytes(path: string, callback: AsyncCallback<number>): void;
  function getFreeBytes(path: string): Promise<number>;
  /**
   * Get the total number of bytes of the specified path.
   *
   * @since 8
   */
  function getTotalBytes(path: string, callback: AsyncCallback<number>): void;
  function getTotalBytes(path: string): Promise<number>;
}

export default Statfs;
