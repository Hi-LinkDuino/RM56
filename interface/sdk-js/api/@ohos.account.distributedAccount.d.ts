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

import {AsyncCallback} from './basic'

/**
 * This module provides the capability to manage distributed accounts.
 *
 * @since 7
 * @syscap SystemCapability.Account.OsAccount
 */
declare namespace distributedAccount {
    /**
     * Get the ability of the distributed account.
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     * @permission N/A
     * @return Ability to manage operations of distributed account.
     */
    function getDistributedAccountAbility(): DistributedAccountAbility;

    /**
     * Defines distributed account functions and interfaces.
     *
     * @name DistributedAccountAbility
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS
     */
    interface DistributedAccountAbility {
        /**
         * Queries the distributed information of the current OS account.
         *
         * @since 7
         * @return The distributed information of the current OS account.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS or ohos.permission.DISTRIBUTED_DATASYNC
         */
        queryOsAccountDistributedInfo(callback: AsyncCallback<DistributedInfo>): void;
        queryOsAccountDistributedInfo(): Promise<DistributedInfo>;

        /**
         * Updates the distributed information of the OS account.
         *
         * @since 7
         * @param accountInfo Indicates the information of the OS account used for a distributed system.
         * @return void
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        updateOsAccountDistributedInfo(accountInfo: DistributedInfo, callback: AsyncCallback<void>): void;
        updateOsAccountDistributedInfo(accountInfo: DistributedInfo): Promise<void>;
    }

    /**
     * Provides the distributed information of the OS account.
     *
     * @name DistributedInfo
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     * @permission N/A
     */
    interface DistributedInfo {
        /**
         * The name in the distributed information of the OS account.
         */
        name: string;

        /**
         * The ID in the distributed information of the OS account.
         */
        id: string;

        /**
         * The event string in the distributed information of the OS account.
         */
        event: string;

        /**
         * The scalable data in the distributed information of the OS account.
         */
        scalableData?: object;
    }
}

export default distributedAccount;