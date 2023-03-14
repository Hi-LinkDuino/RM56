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

import { AsyncCallback } from './../basic';
import bundle from './../@ohos.bundle';

/**
 * @name Provides parameters required for installing or uninstalling an application.
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 * @systemapi Hide this for inner system use
 */
export interface InstallParam {
  /**
    * @default Indicates the user id
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  userId: number;

  /**
    * @default Indicates the install flag
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  installFlag: number;

  /**
    * @default Indicates whether the param has data
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  isKeepData: boolean;
}

/**
 * @name Indicates the install or uninstall status
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 * @systemapi Hide this for inner system use
 */
export interface InstallStatus {

  /**
    * @default Indicates the install or uninstall error code
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  status: bundle.InstallErrorCode;

  /**
    * @default Indicates the install or uninstall result string message
    * @since 7
    * @syscap SystemCapability.BundleManager.BundleFramework
    */
  statusMessage: string;
}

/**
 * @name Offers install, upgrade, and remove bundles on the devices.
 * @since 7
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @permission NA
 * @systemapi Hide this for inner system use
 */
export interface BundleInstaller {
  /**
   * Install an application in a HAP.
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   *
   * @param bundleFilePaths Indicates the path where the bundle of the application is stored. The path should be the
   *                        relative path to the data directory of the current application.
   * @param installParam Indicates other parameters required for the installation.
   * @return InstallStatus
   * @permission ohos.permission.INSTALL_BUNDLE
   */
  install(bundleFilePaths: Array<string>, param: InstallParam, callback: AsyncCallback<InstallStatus>): void;

  /**
   * Uninstall an application.
   *
   * @since 7
   * @syscap SystemCapability.BundleManager.BundleFramework
   *
   * @param bundleName Indicates the bundle name of the application to be uninstalled.
   * @param installParam Indicates other parameters required for the uninstallation.
   * @return InstallStatus
   * @permission ohos.permission.INSTALL_BUNDLE
   */
  uninstall(bundleName: string, param: InstallParam, callback: AsyncCallback<InstallStatus>): void;

  /**
   * recover an application.
   *
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   *
   * @param bundleName Indicates the bundle name of the application to be recovered.
   * @param installParam Indicates other parameters required for the recover.
   * @return InstallStatus
   * @permission ohos.permission.INSTALL_BUNDLE
   * @systemapi Hide this for inner system use
   */
   recover(bundleName: string, param: InstallParam, callback: AsyncCallback<InstallStatus>): void;
}