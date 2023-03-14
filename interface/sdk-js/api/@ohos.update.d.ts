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

import { AsyncCallback, BussinessError } from "./basic";

/**
 * A static class to do update for device.
 *
 * @since 6
 * @syscap SystemCapability.Update.UpdateService
 * @systemapi hide for inner use.
 */
declare namespace update {
    /**
     * Enumerates new version package types.
     *
     * @since 6
     */
    export enum PackageTypes {
        PACKAGE_TYPE_NORMAL = 1,
        PACKAGE_TYPE_BASE = 2,
        PACKAGE_TYPE_CUST = 3,
        PACKAGE_TYPE_PRELOAD = 4,
        PACKAGE_TYPE_COTA = 5,
        PACKAGE_TYPE_VERSION = 6,
        PACKAGE_TYPE_PATCH = 7
    }

    /**
     * Represents new version results after update version check.
     *
     * @since 6
     */
    export interface CheckResult {
        /**
         * New version name
         *
         * @since 6
         */
        versionName: number;

        /**
         * New version code
         *
         * @since 6
         */
        versionCode: string;

        /**
         * New version package size
         *
         * @since 6
         */
        size: number;

        /**
         * New version verify information
         *
         * @since 6
         */
        verifyInfo: string;

        /**
         * New version package type
         *
         * @since 6
         */
        packageType: PackageTypes;

        /**
         * New version description ID
         *
         * @since 6
         */
        descriptionId: string;
    }

    /**
     * Represents new version description information.
     *
     * @since 6
     */
    export interface DescriptionInfo {
        /**
         * description ID
         *
         * @since 6
         */
        descriptionId: string;

        /**
         * description content
         *
         * @since 6
         */
        content: string;
    }

    /**
     * Enumerates new version status.
     *
     * @since 6
     */
    export enum NewVersionStatus {
        /**
         * New version check with system error
         *
         * @since 6
         */
        VERSION_STATUS_ERR = -1,

        /**
         * New version detected
         *
         * @since 6
         */
        VERSION_STATUS_NEW = 0,

        /**
         * No New version
         *
         * @since 6
         */
        VERSION_STATUS_NONE = 1,

        /**
         * Server busy
         *
         * @since 6
         */
        VERSION_STATUS_BUSY = 2
    }

    /**
     * Represents new version information.
     *
     * @since 6
     */
    export interface NewVersionInfo {
        /**
         * Update Check Status
         *
         * @since 6
         */
        status: NewVersionStatus;

        /**
         * New version check error message
         *
         * @since 6
         */
        errMsg: string;

        /**
         * New version check results
         *
         * @since 6
         */
        checkResults: Array<CheckResult>;

        /**
         * New version check description
         *
         * @since 6
         */
        descriptionInfo: Array<DescriptionInfo>;
    }

    /**
     * Enumerates update status.
     *
     * @since 6
     */
    export enum UpdateState {
        UPDATE_STATE_INIT = 0,
        UPDATE_STATE_CHECK_VERSION_ON = 10,
        UPDATE_STATE_CHECK_VERSION_FAIL,
        UPDATE_STATE_CHECK_VERSION_SUCCESS,
        UPDATE_STATE_DOWNLOAD_ON = 20,
        UPDATE_STATE_DOWNLOAD_PAUSE,
        UPDATE_STATE_DOWNLOAD_CANCEL,
        UPDATE_STATE_DOWNLOAD_FAIL,
        UPDATE_STATE_DOWNLOAD_SUCCESS,
        UPDATE_STATE_VERIFY_ON = 30,
        UPDATE_STATE_VERIFY_FAIL,
        UPDATE_STATE_VERIFY_SUCCESS,
        UPDATE_STATE_PACKAGE_TRANS_ON = 70,
        UPDATE_STATE_PACKAGE_TRANS_FAIL,
        UPDATE_STATE_PACKAGE_TRANS_SUCCESS,
        UPDATE_STATE_INSTALL_ON = 80,
        UPDATE_STATE_INSTALL_FAIL,
        UPDATE_STATE_INSTALL_SUCCESS,
        UPDATE_STATE_UPDATE_ON = 90,
        UPDATE_STATE_UPDATE_FAIL,
        UPDATE_STATE_UPDATE_SUCCESS
    }

    /**
     * Represents update progress information.
     *
     * @since 6
     */
    export interface Progress {
        /**
         * update progress percent
         *
         * @since 6
         */
        percent: number;

        /**
         * update status
         *
         * @since 6
         */
        status: UpdateState;

        /**
         * update end reason
         *
         * @since 6
         */
        endReason: string;
    }

    /**
     * Enumerates install mode for new version packages.
     *
     * @since 6
     */
    export enum InstallMode {
        /**
         * Normal update.
         *
         * @since 6
         */
        INSTALL_MODE_NORMAL,

        /**
         * Update at night
         *
         * @since 6
         */
        INSTALL_MODE_NIGHT,

        /**
         * Auto update
         *
         * @since 6
         */
        INSTALL_MODE_AUTO
    }

    /**
     * Represents update policy.
     *
     * @since 6
     */
    export interface UpdatePolicy {
        /**
         * Enable auto download new packages or not
         *
         * @since 6
         */
        autoDownload: boolean;

        /**
         * New packages auto installation mode
         *
         * @since 6
         */
        installMode: INSTALL_MODE;

        /**
         * Auto installation time interval
         *
         * @since 6
         */
        autoUpgradeInterval: Array<number>;
    }

    /**
     * Called when the signal status changes. You need to implement this method in a child class.
     * Unlike {@code onSignalStatus(status: number)}, a signal source is specified in this method.
     *
     * @param status Indicates the signal status.
     *               The value {@code 0} indicates that the signal is stable,
     *               {@code 1} indicates that no signal is available,
     *               {@code 2} indicates that the signal is not supported,
     *               and {@code 3} indicates that the signal is unstable.
     * @param source Indicates the signal source. For details about available values,
     *               see {@link @system.tv.SourceIndices}.
     * @since 6
     */
    export interface UpdateProgressCallback {
        (progress: Progress): void;
    }

    /**
     * A static class to do update for the specified device.
     *
     * @since 6
     * @syscap SystemCapability.Update.UpdateService
     * @systemapi hide for inner use.
     */
    export interface Updater {
        /**
        * Check new version.
        *
        * @since 6
        */
        checkNewVersion(callback: AsyncCallback<NewVersionInfo>): void;
        checkNewVersion(): Promise<NewVersionInfo>;

        /**
         * Trigger download new version packages.
         * apps should listen to downloadProgress event
         *
         * @since 6
         */
        download(): void;

        /**
         * Install packages for the device.
         * apps should listen to upgradeProgress event
         *
         * @since 6
         */
        upgrade(): void;

        /**
         * Get new version information for the newly installed package.
         *
         * @since 6
         */
        getNewVersionInfo(callback: AsyncCallback<NewVersionInfo>): void;
        getNewVersionInfo(): Promise<NewVersionInfo>;

        /**
         * Get current update policy.
         *
         * @since 6
         */
        getUpdatePolicy(callback: AsyncCallback<UpdatePolicy>): void;
        getUpdatePolicy(): Promise<UpdatePolicy>;

        /**
         * Set update policy.
         *
         * @since 6
         */
        setUpdatePolicy(policy: UpdatePolicy, callback: AsyncCallback<number>): void;
        setUpdatePolicy(policy: UpdatePolicy): Promise<number>;

        /**
         * Reboot to apply upgrade package.
         *
         * @since 6
         */
        applyNewVersion(callback: AsyncCallback<number>): void;
        applyNewVersion(): Promise<number>;

        /**
         * Reboot to clean cache.
         *
         * @since 6
	 * @deprecated since 8
         */
        rebootAndCleanCache(callback: AsyncCallback<number>): void;
        rebootAndCleanCache(): Promise<number>;
	
	/**
         * Reboot and clean user data.
         *
         * @since 8
         */
        rebootAndCleanUserData(callback: AsyncCallback<number>): void;
        rebootAndCleanUserData(): Promise<number>;

        /**
         * verify update package.
         * apps should listen to verifyProgress event
         *
         * @since 6
         */
        verifyUpdatePackage(upgradeFile: string, certsFile: string): void;

        /**
         * Subscribe to download/upgrade/verify progress events
         *
         * @since 6
         */
        on(eventType: 'downloadProgress', callback: UpdateProgressCallback): void;
        on(eventType: 'upgradeProgress', callback: UpdateProgressCallback): void;
        on(eventType: 'verifyProgress', callback: UpdateProgressCallback): void;

        /**
         * Unsubscribe to download/upgrade/verify progress events
         *
         * @since 6
         */
        off(eventType: 'downloadProgress', callback?: UpdateProgressCallback): void;
        off(eventType: 'upgradeProgress', callback?: UpdateProgressCallback): void;
        off(eventType: 'verifyProgress', callback?: UpdateProgressCallback): void;

	/**
         * cancel download packages for the device.
         *
         * @since 6
         */
        cancel(): void;
    }

    export type UpdateTypes =
        'OTA' |
        'patch';

    /**
     * Get Updater handler for the calling device.
     *
     * @return Updater handler to perform actual update
     * @since 6
     */
    function getUpdater(upgradeFile: string, updateType?: UpdateTypes): Updater;

    /**
     * Get Updater handler for the specified device.
     *
     * @return Updater handler to perform actual update
     * @since 6
     */
    function getUpdaterForOther(upgradeFile: string, device: string, updateType?: UpdateTypes): Updater;

    /**
     * Get Updater handler from other device to trigger update for the calling device.
     *
     * @return Updater handler to perform actual update
     * @since 6
     */
    function getUpdaterFromOther(upgradeFile: string, device: string, updateType?: UpdateTypes): Updater;
}

export default update;
