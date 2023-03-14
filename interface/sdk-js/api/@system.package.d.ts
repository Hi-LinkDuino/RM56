/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @since 3
 */
export interface CheckPackageHasInstalledResponse {
    /**
     * Whether the application exists, or whether the native application has been installed.
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @since 3
     */
    result: boolean;
}

/**
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @since 3
 */
export interface CheckPackageHasInstalledOptions {
    /**
     * Application bundle name.
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @since 3
     */
    bundleName: string;

    /**
     * Called when native applications are installed.
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @since 3
     */
    success?: (data: CheckPackageHasInstalledResponse) => void;

    /**
     * Called when native applications fail to be installed.
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @since 3
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the excution is completed.
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @since 3
     */
    complete?: () => void;
}

/**
 * @syscap SystemCapability.BundleManager.BundleFramework
 * @since 3
 * @import package from '@system.package';
 */
export default class Package {
    /**
     * Checks whethers an application exists, or whether a native application has been installed.
     * @param options Options
     * @syscap SystemCapability.BundleManager.BundleFramework
     * @permission ohos.permission.GET_BUNDLE_INFO_PRIVILEGED or ohos.permission.GET_BUNDLE_INFO
     */
    static hasInstalled(options: CheckPackageHasInstalledOptions): void;
}