/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 * @name Indicates the defined permission details in file config.json
 * @since 8
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 * @systemapi
 */
 export interface PermissionDef {
  /**
    * @default Indicates the name of this permission
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  permissionName: string;

  /**
    * @default Indicates the grant mode of this permission
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  grantMode: number;

  /**
    * @default Indicates the labelId of this permission
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  labelId: number;

  /**
    * @default Indicates the descriptionId of this permission
    * @since 8
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  descriptionId: number;
}
