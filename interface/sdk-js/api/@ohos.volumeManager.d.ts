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
 * Provides volumemanager statistics APIs
 *
 * @since 9
 * @syscap SystemCapability.FileManagement.StorageService.Volume
 * @systemapi
 */
declare namespace volumeManager {

/**
   * Get All Volumes
   * 
   * @since 9
   * @systemapi
   */
 export interface Volume {
  id: string;
  uuid: string;
  description: string;
  removable: boolean;
  state: number;
  path: string;
}
/**
   * Get All Volumes
   * 
   * @since 9
   * @systemapi
   */
function getAllVolumes(callback: AsyncCallback<Array<Volume>>): void;
function getAllVolumes(): Promise<Array<Volume>>;


 /**
   * Mount
   * 
   * @since 9
   * @systemapi
   */
function mount(volumeId: string, callback: AsyncCallback<boolean>): void;
function mount(volumeId: string): Promise<boolean>;

/**
   * UnMount
   * 
   * @since 9
   * @systemapi
   */
function unmount(volumeId: string, callback: AsyncCallback<boolean>): void;
function unmount(volumeId: string): Promise<boolean>;

}

export default volumeManager;
