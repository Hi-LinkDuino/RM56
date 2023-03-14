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
 * Contains basic Ability information, which uniquely identifies an launcher StatusCallback.
 * You can use this class to obtain values of the fields set in an callback function,
 * such as the add, update, remove.
 * @name Contains basic launcher status callback information, which uniquely identifies an LauncherStatusCallback
 * @since 8
 * @syscap SystemCapability.BundleManager.BundleFramework
 *
 * @permission ohos.permission.LISTEN_BUNDLE_CHANGE
 * @systemapi Hide this for inner system use
 */
declare interface BundleStatusCallback {
  /**
   * @name Obtains add callback about an launcherStatusCallback.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission ohos.permission.LISTEN_BUNDLE_CHANGE
   *
   * @systemapi Hide this for inner system use
   */
  add: (bundleName : string, userId: number) => void;

  /**
   * @name Obtains update callback about an launcherStatusCallback.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission ohos.permission.LISTEN_BUNDLE_CHANGE
   *
   * @systemapi Hide this for inner system use
   */
  update: (bundleName : string, userId: number) => void;

  /**
   * @name Obtains remove callback about an launcherStatusCallback.
   * @since 8
   * @syscap SystemCapability.BundleManager.BundleFramework
   * @permission ohos.permission.LISTEN_BUNDLE_CHANGE
   *
   * @systemapi Hide this for inner system use
   */
  remove: (bundleName : string, userId: number) => void;
}