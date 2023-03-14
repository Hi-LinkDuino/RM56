/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
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

import {AsyncCallback} from "./basic";

/**
 * Provides file path related APIS.
 *
 * @since 8
 * @syscap SystemCapability.Customization.ConfigPolicy
 */
declare namespace configPolicy {
  /**
   * Get file from the highest priority config path which contains the given file name.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @syscap SystemCapability.Customization.ConfigPolicy
   * @param relPath the relative path of the config file.
   * @return Returns the path of the highest priority config file.
   */
  function getOneCfgFile(relPath: string, callback: AsyncCallback<string>);
  function getOneCfgFile(relPath: string): Promise<string>;

  /**
   * Get config files in device architecture, ordered by priority from low to high.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @syscap SystemCapability.Customization.ConfigPolicy
   * @param relPath the relative path of the config file.
   * @return Returns paths of config files.
   */
  function getCfgFiles(relPath: string, callback: AsyncCallback<Array<string>>);
  function getCfgFiles(relPath: string): Promise<Array<string>>;

  /**
   * Get config directories in device architecture, ordered by priority from low to high.
   *
   * @since 8
   * @systemapi Hide this for inner system use.
   * @syscap SystemCapability.Customization.ConfigPolicy
   * @return Returns paths of config directories.
   */
  function getCfgDirList(callback: AsyncCallback<Array<string>>);
  function getCfgDirList(): Promise<Array<string>>;
}

export default configPolicy;
