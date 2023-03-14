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

import distributedAccount from './@ohos.account.distributedAccount'
import {AsyncCallback} from "./basic";

/**
 * This module provides the capability to manage os accounts.
 *
 * @since 7
 * @syscap SystemCapability.Account.OsAccount
 */
declare namespace osAccount {
    /**
     * Obtains the AccountManager instance.
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     * @return Returns the instance of the AccountManager.
     */
    function getAccountManager(): AccountManager;

    /**
     * Provides abilities for you to manage and perform operations on your OS accounts.
     * @name AccountManager
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     */
    interface AccountManager {
        /**
         * Activates a specified OS account.
         * <p>
         * If multiple OS accounts are available, you can call this method to enable a specific OS account
         * to run in the foreground. Then, the OS account originally running in the foreground will be
         * switched to the background.
         * </p>
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return void.
         * @permission ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS_EXTENSION.
         * @systemapi Hide this for inner system use.
         */
        activateOsAccount(localId: number, callback: AsyncCallback<void>): void;
        activateOsAccount(localId: number): Promise<void>;

        /**
         * Checks whether the function of supporting multiple OS accounts is enabled.
         *
         * @since 7
         * @return Returns {@code true} if this function is enabled; returns {@code false} otherwise.
         */
        isMultiOsAccountEnable(callback: AsyncCallback<boolean>): void;
        isMultiOsAccountEnable(): Promise<boolean>;

        /**
         * Checks whether an OS account is activated based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return Returns {@code true} if the OS account is activated; returns {@code false} otherwise.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS or ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS
         */
        isOsAccountActived(localId: number, callback: AsyncCallback<boolean>): void;
        isOsAccountActived(localId: number): Promise<boolean>;

        /**
         * Checks whether a constraint has been enabled for an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @param constraint Indicates the constraint to check. The value can be:
         *        <ul>
         *        <li>{@code constraint.wifi.set} - Indicates the constraint on configuring the Wi-Fi access point.
         *        </li>
         *        <li>{@code constraint.sms.use} - Indicates the constraint on sending and receiving short messages.
         *        </li>
         *        <li>{@code constraint.calls.outgoing} - Indicates the constraint on making calls.</li>
         *        <li>{@code constraint.unknown.sources.install} - Indicates the constraint on installing applications
         *        from unknown sources.</li>
         *        </ul>
         * @return Returns {@code true} if the constraint has been enabled for the OS account;
         *         returns {@code false} otherwise.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        isOsAccountConstraintEnable(localId: number, constraint: string, callback: AsyncCallback<boolean>): void;
        isOsAccountConstraintEnable(localId: number, constraint: string): Promise<boolean>;

        /**
         * Checks whether this OS account is a test OS account.
         *
         * @since 7
         * @return Returns {@code true} if this OS account is a test OS account; returns {@code false} otherwise.
         */
        isTestOsAccount(callback: AsyncCallback<boolean>): void;
        isTestOsAccount(): Promise<boolean>;

        /**
         * Checks whether this OS account has been verified.
         *
         * @since 7
         * @return Returns {@code true} if the OS account has been verified successfully;
         *         returns {@code false} otherwise.
         */
        isOsAccountVerified(callback: AsyncCallback<boolean>): void;

        /**
         * Checks whether an OS account has been verified based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return Returns {@code true} if the OS account has been verified successfully;
         *         returns {@code false} otherwise.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS or ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS.
         */
        isOsAccountVerified(localId: number, callback: AsyncCallback<boolean>): void;
        isOsAccountVerified(localId?: number): Promise<boolean>;

        /**
         * Removes an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return void.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        removeOsAccount(localId: number, callback: AsyncCallback<void>): void;
        removeOsAccount(localId: number): Promise<void>;

        /**
         * Sets constraints for an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @param constraints Indicates the constraints to set for the OS account. The value can be:
         *        <ul>
         *        <li>{@code constraint.wifi.set} - Indicates the constraint on configuring the Wi-Fi access point.
         *        </li>
         *        <li>{@code constraint.sms.use} - Indicates the constraint on sending and receiving short messages.
         *        </li>
         *        <li>{@code constraint.calls.outgoing} - Indicates the constraint on making calls.</li>
         *        <li>{@code constraint.unknown.sources.install} - Indicates the constraint on installing applications
         *        from unknown sources.</li>
         *        </ul>
         * @param enable Specifies whether to enable the constraint.
         * @return void.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        setOsAccountConstraints(localId: number, constraints: Array<string>, enable: boolean,
                                callback: AsyncCallback<void>): void;
        setOsAccountConstraints(localId: number, constraints: Array<string>, enable: boolean): Promise<void>;

        /**
         * Sets the local name for an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @param localName Indicates the local name to set for the OS account.
         * @return void.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        setOsAccountName(localId: number, localName: string, callback: AsyncCallback<void>): void;
        setOsAccountName(localId: number, localName: string): Promise<void>;

        /**
         * Obtains the number of all OS accounts created on a device.
         *
         * @since 7
         * @return Returns the number of created OS accounts.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        getCreatedOsAccountsCount(callback: AsyncCallback<number>): void;
        getCreatedOsAccountsCount(): Promise<number>;

        /**
         * Obtains the local ID of an OS account from the current process UID.
         *
         * @since 7
         * @return Returns the local ID of the OS account.
         */
        getOsAccountLocalIdFromProcess(callback: AsyncCallback<number>): void;
        getOsAccountLocalIdFromProcess(): Promise<number>;

        /**
         * Queries the local ID of an OS account from the process UID.
         *
         * @since 7
         * @param uid Indicates the process UID.
         * @return Returns the local ID of the OS account.
         */
        getOsAccountLocalIdFromUid(uid: number, callback: AsyncCallback<number>): void;
        getOsAccountLocalIdFromUid(uid: number): Promise<number>;

        /**
         * Queries the local ID of an OS account which is bound to the specified domain account
         *
         * @since 8
         * @param domainInfo Indicates the domain account info.
         * @return Returns the local ID of the OS account.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        getOsAccountLocalIdFromDomain(domainInfo: DomainAccountInfo, callback: AsyncCallback<number>): void;
        getOsAccountLocalIdFromDomain(domainInfo: DomainAccountInfo): Promise<number>;

        /**
         * Queries the maximum number of OS accounts that can be created on a device.
         *
         * @since 7
         * @return Returns the maximum number of OS accounts that can be created.
         * @systemapi Hide this for inner system use.
         */
        queryMaxOsAccountNumber(callback: AsyncCallback<number>): void;
        queryMaxOsAccountNumber(): Promise<number>;

        /**
         * Obtains all constraints of an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return Returns a list of constraints.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        getOsAccountAllConstraints(localId: number, callback: AsyncCallback<Array<string>>): void;
        getOsAccountAllConstraints(localId: number): Promise<Array<string>>;

        /**
         * Queries the list of all the OS accounts that have been created in the system.
         *
         * @since 7
         * @return Returns a list of OS accounts.
         * @systemapi Hide this for inner system use.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        queryAllCreatedOsAccounts(callback: AsyncCallback<Array<OsAccountInfo>>): void;
        queryAllCreatedOsAccounts(): Promise<Array<OsAccountInfo>>;

        /**
         * Queries the id list of all activated OS accounts.
         *
         * @since 8
         * @return Returns a id list of OS accounts.
         */
        queryActivatedOsAccountIds(callback: AsyncCallback<Array<number>>): void;
        queryActivatedOsAccountIds(): Promise<Array<number>>;

        /**
         * Creates an OS account using the local name and account type.
         *
         * @since 7
         * @param localName Indicates the local name of the OS account to create.
         * @param type Indicates the type of the OS account to create.
         *        {@link OsAccountType} specifies the account types available in the system.
         * @return Returns information about the created OS account; returns {@code null} if the creation fails.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        createOsAccount(localName: string, type: OsAccountType, callback: AsyncCallback<OsAccountInfo>): void;
        createOsAccount(localName: string, type: OsAccountType): Promise<OsAccountInfo>;

        /**
         * Creates an OS account using the account type and domain account info
         *
         * @since 8
         * @param type Indicates the type of the OS account to create.
         *        {@link OsAccountType} specifies the account types available in the system.
         * @param domainInfo Indicates the domain account info.
         * @return Returns information about the created OS account; returns {@code null} if the creation fails.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        createOsAccountForDomain(type: OsAccountType, domainInfo: DomainAccountInfo, callback: AsyncCallback<OsAccountInfo>): void;
        createOsAccountForDomain(type: OsAccountType, domainInfo: DomainAccountInfo): Promise<OsAccountInfo>;

        /**
         * Queries information about the current OS account.
         *
         * @since 7
         * @return Returns information about the current OS account; returns {@code null} if the query fails.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         */
        queryCurrentOsAccount(callback: AsyncCallback<OsAccountInfo>): void;
        queryCurrentOsAccount(): Promise<OsAccountInfo>;

        /**
         * Queries OS account information based on the local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return Returns the OS account information; returns {@code null} if the query fails.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS or ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS_EXTENSION.
         * @systemapi Hide this for inner system use.
         */
        queryOsAccountById(localId: number, callback: AsyncCallback<OsAccountInfo>): void;
        queryOsAccountById(localId: number): Promise<OsAccountInfo>;

        /**
         * Obtains the type of this OS account from the current process.
         *
         * @since 7
         * @return Returns the OS account type. The value can be {@link OsAccountType#ADMIN},
         *         {@link OsAccountType#NORMAL}, and {@link OsAccountType#GUEST}.
         */
        getOsAccountTypeFromProcess(callback: AsyncCallback<OsAccountType>): void;
        getOsAccountTypeFromProcess(): Promise<OsAccountType>;

        /**
         * Obtains the distributed virtual device ID (DVID).
         * <p>
         * If the same OHOS account has logged in to multiple devices, these devices constitute a super device
         * through the distributed networking. On the networked devices, you can call this method to obtain the DVIDs.
         * The same application running on different devices obtains the same DVID, whereas different applications
         * obtain different DVIDs.
         * <p>
         *
         * @since 7
         * @return Returns the DVID if obtained; returns an empty string if no OHOS account has logged in.
         * @permission ohos.permission.DISTRIBUTED_DATASYNC or ohos.permission.MANAGE_LOCAL_ACCOUNTS
         */
        getDistributedVirtualDeviceId(callback: AsyncCallback<string>): void;
        getDistributedVirtualDeviceId(): Promise<string>;

        /**
         * Obtains the profile photo of an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @return Returns the profile photo if obtained;
         *         returns {@code null} if the profile photo fails to be obtained.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        getOsAccountProfilePhoto(localId: number, callback: AsyncCallback<string>): void;
        getOsAccountProfilePhoto(localId: number): Promise<string>;

        /**
         * Sets the profile photo for an OS account based on its local ID.
         *
         * @since 7
         * @param localId Indicates the local ID of the OS account.
         * @param photo Indicates the profile photo to set for the OS account.
         * @return void.
         * @permission ohos.permission.MANAGE_LOCAL_ACCOUNTS.
         * @systemapi Hide this for inner system use.
         */
        setOsAccountProfilePhoto(localId: number, photo: string, callback: AsyncCallback<void>): void;
        setOsAccountProfilePhoto(localId: number, photo: string): Promise<void>;

        /**
         * Obtain localId according to serial number
         *
         * @since 8
         * @param serialNumber Indicates serial number.
         * @return localId.
         */
        getOsAccountLocalIdBySerialNumber(serialNumber: number, callback: AsyncCallback<number>): void;
        getOsAccountLocalIdBySerialNumber(serialNumber: number): Promise<number>;

        /**
         * Obtain serial number according to localId.
         *
         * @since 8
         * @param localId Indicates the local ID of the OS account.
         * @return serial number.
         */
        getSerialNumberByOsAccountLocalId(localId: number, callback: AsyncCallback<number>): void;
        getSerialNumberByOsAccountLocalId(localId: number): Promise<number>;

        /**
         * Subscribes to the change events of OS accounts.
         * <p>
         * When user change the account, the subscriber will receive a notification
         * about the account change event.
         *
         * @since 7
         * @param type Event type.
         * @param name Indicates the name of subscriber.
         * @return void.
         * @permission ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS_EXTENSION
         * @systemapi Hide this for inner system use.
         */
        on(type: 'activate' | 'activating', name: string, callback: Callback<number>): void;

        /**
         * Unsubscribes from account events.
         *
         * @since 7
         * @param type Event type.
         * @param name Indicates the name of subscriber.
         * @return void.
         * @permission ohos.permission.INTERACT_ACROSS_LOCAL_ACCOUNTS_EXTENSION
         * @systemapi Hide this for inner system use.
         */
        off(type: 'activate' | 'activating', name: string, callback?: Callback<number>): void;
    }

    /**
     * Provides information about OS accounts, including the local ID, local name, and type of an OS account.
     * @name OsAccountInfo
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     */
    interface OsAccountInfo {
        /**
         * The local ID of an OS account.
         * @since 7
         */
        localId: number;

        /**
         * The local name of an OS account.
         * @since 7
         */
        localName: string;

        /**
         * Include: ADMIN, Normal, GUEST.
         * @since 7
         */
        type: OsAccountType;

        /**
         * Account constraints information.
         * @since 7
         */
        constraints: Array<string>;

        /**
         * The account is verified or not.
         * @since 8
         */
        isVerified: boolean;

        /**
         * OS account photo.
         * @since 8
         */
        photo: string;

        /**
         * Os account create time.
         * @since 8
         */
        createTime: number;

        /**
         * The last time to log in.
         * @since 8
         */
        lastLoginTime: number;

        /**
         * Os account serial number.
         * @since 8
         */
        serialNumber: number;

        /**
         * Os account is activated or not.
         * @since 8
         */
        isActived: boolean;

        /**
         * Os account create completed or not.
         * @since 8
         */
        isCreateCompleted: boolean;

        /**
         * Distributed account info.
         * @since 7
         */
        distributedInfo: distributedAccount.DistributedInfo;

         /**
          * Domain account info.
          * @since 8
          */
         domainInfo: DomainAccountInfo;
    }

    interface DomainAccountInfo {
        /**
        * The domain name
        * @since 8
        */
        domain: string;

        /**
        * The account name in the domain
        * @since 8
        */
        accountName: string;
    }

    /**
     * Enumerates OS account types.
     * @name OsAccountType
     * @since 7
     * @syscap SystemCapability.Account.OsAccount
     */
    enum OsAccountType {
        /**
         * Indicates the administrator account, which has the permission to manage other OS accounts.
         */
        ADMIN = 0,

        /**
         * Indicates a normal account, which has access to common functions of OS accounts.
         */
        NORMAL,

        /**
         * Indicates a guest account, which is used to temporarily access the device and may be deleted at any time.
         */
        GUEST
    }

    /**
     * Provides the abilities for user authentication.
     * @name UserAuth
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     */
    class UserAuth {
        /**
         * Constructor to get the UserAuth class instance.
         *
         * @since 8
         * @return Returns the UserAuth class instance.
         * @systemapi Hide this for inner system use.
         */
        constructor();

        /**
         * Gets version information.
         *
         * @since 8
         * @return Returns the version information.
         * @systemapi Hide this for inner system use.
         */
        getVersion(): number;

        /**
         * Checks whether the authentication capability is available.
         *
         * @since 8
         * @param authType Indicates the credential type for authentication.
         * @param authTrustLevel Indicates the trust level of authentication result.
         * @return Returns a status result.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        getAvailableStatus(authType: AuthType, authTrustLevel: AuthTrustLevel): number;

        /**
         * Gets the property based on the specified request information.
         *
         * @since 8
         * @param request Indicates the request information, including authentication type, and property type list.
         * @return Returns an executor property.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        getProperty(request: GetPropertyRequest, callback: AsyncCallback<ExecutorProperty>): void;
        getProperty(request: GetPropertyRequest): Promise<ExecutorProperty>;

        /**
         * Sets property that can be used to initialize algorithms.
         *
         * @since 8
         * @param request Indicates the request information, including authentication type and the key-value to be set.
         * @return Returns a number value indicating whether the property setting was successful.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        setProperty(request: SetPropertyRequest, callback: AsyncCallback<number>): void;
        setProperty(request: SetPropertyRequest): Promise<number>;

        /**
         * Executes authentication.
         *
         * @since 8
         * @param challenge Indicates the challenge value.
         * @param authType Indicates the authentication type.
         * @param authTrustLevel Indicates the trust level of authentication result.
         * @param callback Indicates the callback to get result and acquireInfo.
         * @return Returns a context ID for cancellation.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        auth(challenge: Uint8Array, authType: AuthType, authTrustLevel: AuthTrustLevel, callback: IUserAuthCallback): Uint8Array;

        /**
         * Executes user authentication.
         *
         * @since 8
         * @param userId Indicates the user identification.
         * @param challenge Indicates the challenge value.
         * @param authType Indicates the authentication type.
         * @param authTrustLevel Indicates the trust level of authentication result.
         * @param callback Indicates the callback to get result and acquireInfo.
         * @return Returns a context ID for cancellation.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        authUser(userId: number, challenge: Uint8Array, authType: AuthType, authTrustLevel: AuthTrustLevel, callback: IUserAuthCallback): Uint8Array;

        /**
         * Cancels authentication with context ID.
         *
         * @since 8
         * @param contextID Indicates the authentication context ID.
         * @return Returns a number indicating whether the cancellation was successful.
         * @permission ohos.permission.ACCESS_USER_AUTH_INTERNAL
         * @systemapi Hide this for inner system use.
         */
        cancelAuth(contextID: Uint8Array): number;
    }

    /**
     * Provides the abilities for Pin code authentication.
     *
     * @name PINAuth
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     */
    class PINAuth {
        /**
         * Constructor to get the PINAuth class instance.
         *
         * @since 8
         * @return Returns the PINAuth class instance.
         * @systemapi Hide this for inner system use.
         */
        constructor();

        /**
         * Register inputer.
         *
         * @param inputer Indicates the password input box callback
         * @return boolean Indicates the registration succeeded or failed.
         * @permission ohos.permission.ACCESS_PIN_AUTH
         * @systemapi Hide this for inner system use.
         */
        registerInputer(inputer: IInputer): boolean;

        /**
         * Unregister inputer.
         *
         * @permission ohos.permission.ACCESS_PIN_AUTH
         * @systemapi Hide this for inner system use.
         */
        unregisterInputer(): void;
    }

    /**
     * Provides the abilities for managing user identity.
     *
     * @name UserIdentityManager
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     */
    class UserIdentityManager {
        /**
         * Constructor to get the UserIdentityManager class instance.
         *
         * @since 8
         * @return Returns the UserIdentityManager class instance.
         * @systemapi Hide this for inner system use.
         */
        constructor();

        /**
         * Opens session.
         * <p>
         * Start an IDM operation to obtain challenge value.
         * A challenge value of 0 indicates that opensession failed.
         *
         * @since 8
         * @return Returns a challenge value.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        openSession(callback: AsyncCallback<Uint8Array>): void;
        openSession(): Promise<Uint8Array>;

        /**
         * Adds credential.
         * <p>
         * Add user credential information, pass in credential addition method and credential information
         * (credential type, subclass, if adding user's non password credentials, pass in password authentication token),
         * and get the result / acquireinfo callback.
         *
         * @since 8
         * @param credentialInfo Indicates the credential information.
         * @param callback Indicates the callback to get results and acquireInfo.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        addCredential(credentialInfo: CredentialInfo, callback: IIdmCallback): void;

        /**
         * Updates credential.
         *
         * @since 8
         * @param credentialInfo Indicates the credential information.
         * @param callback Indicates the callback to get results and acquireInfo.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        updateCredential(credentialInfo: CredentialInfo, callback: IIdmCallback): void;

        /**
         * Closes session.
         * <p>
         * End an IDM operation.
         *
         * @since 8
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        closeSession(): void;

        /**
         * Cancels entry with a challenge value.
         *
         * @since 8
         * @param challenge Indicates the challenge value.
         * @return Returns a number indicating whether the cancellation was successful.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        cancel(challenge: Uint8Array): number;

        /**
         * Deletes the user with the authentication token.
         *
         * @since 8
         * @param token Indicates the authentication token.
         * @param callback Indicates the callback to get the deletion result.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        delUser(token: Uint8Array, callback: IIdmCallback): void;

        /**
         * Deletes the user credential information.
         *
         * @since 8
         * @param credentialId Indicates the credential index.
         * @param token Indicates the authentication token.
         * @param callback Indicates the callback to get the deletion result.
         * @permission ohos.permission.MANAGE_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        delCred(credentialId: Uint8Array, token: Uint8Array, callback: IIdmCallback): void;

        /**
         * Gets authentication information.
         *
         * @since 8
         * @param authType Indicates the authentication type.
         * @param callback Indicates the callback to get all registered credential information of
         * the specified type for the current user.
         * @permission ohos.permission.ACCESS_USER_IDM
         * @systemapi Hide this for inner system use.
         */
        getAuthInfo(callback: AsyncCallback<Array<EnrolledCredInfo>>, authType?: AuthType): void;
        getAuthInfo(authType?: AuthType): Promise<Array<EnrolledCredInfo>>;
    }

    /**
     * Password data callback.
     *
     * @name IInputData
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface IInputData {
        /**
         * Notifies to set data.
         *
         * @since 8
         * @param pinSubType Indicates the credential subtype for authentication.
         * @param data Indicates the data to set.
         * @systemapi Hide this for inner system use.
         */
        onSetData: (pinSubType: AuthSubType, data: Uint8Array) => void;
    }
    
    /**
     * Password input box callback.
     *
     * @name IInputer
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface IInputer {
        /**
         * Notifies to get data.
         *
         * @since 8
         * @param callback Indicates the password data callback.
         * @systemapi Hide this for inner system use.
         */
        onGetData: (callback: IInputData) => void;
    }

    /**
     * User authentication callback.
     *
     * @name IUserAuthCallback
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface IUserAuthCallback {
        /**
         * The authentication result code is returned through the callback.
         *
         * @since 8
         * @param result Indicates the authentication result code.
         * @param extraInfo Indicates the specific information for different situation.
         * If the authentication is passed, the authentication token is returned in extrainfo,
         * If the authentication fails, the remaining authentication times are returned in extrainfo,
         * If the authentication executor is locked, the freezing time is returned in extrainfo.
         * @systemapi Hide this for inner system use.
         */
        onResult: (result: number, extraInfo: AuthResult) => void;

        /**
         * During an authentication, the TipsCode is returned through the callback.
         *
         * @since 8
         * @param module Indicates the executor type for authentication.
         * @param acquire Indicates the tip code for different authentication executor.
         * @param extraInfo reserved parameter.
         * @systemapi Hide this for inner system use.
         */
        onAcquireInfo?: (module: number, acquire: number, extraInfo: any) => void;
    }

    /**
     * Identity manager callback.
     *
     * @name IIdmCallback
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface IIdmCallback {
        /**
         * The authentication result code is returned through the callback.
         *
         * @since 8
         * @param result Indicates the authentication result code.
         * @param extraInfo pass the specific information for different situation.
         * @systemapi Hide this for inner system use.
         */
        onResult: (result: number, extraInfo: RequestResult) => void;

        /**
         * During an authentication, the TipsCode is returned through the callback.
         * @since 8
         * @param module Indicates the executor type for authentication.
         * @param acquire Indicates the tip code for different authentication executor.
         * @param extraInfo reserved parameter.
         * @systemapi Hide this for inner system use.
         */
        onAcquireInfo?: (module: number, acquire: number, extraInfo: any) => void;
    }

    /**
     * Provides the information of the get property request.
     *
     * @name GetPropertyRequest
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface GetPropertyRequest {
        /**
         * Indicates the authentication credential type.
         */
        authType: AuthType;

        /**
         * Indicates the array of property types to get.
         */
        keys: Array<GetPropertyType>;
    }

    /**
     * Provides the information of the set property request.
     *
     * @name SetPropertyRequest
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface SetPropertyRequest {
        /**
         * Indicates the authentication credential type.
         */
        authType: AuthType;

        /**
         * Indicates the property type to set.
         */
        key: SetPropertyType;

        /**
         * Indicates the information to set.
         */
        setInfo: Uint8Array;
    }

    /**
     * Provides the property of executor.
     *
     * @name ExecutorProperty
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface ExecutorProperty {
        /**
         * Indicates the result.
         */
        result: number;

        /**
         * Indicates the authentication credential subtype.
         */
        authSubType: AuthSubType;

        /**
         * Indicates the remaining times.
         */
        remainTimes?: number;

        /**
         * Indicates the freezing times.
         */
        freezingTime?: number;
    }
 
    /**
     * Indicates the information of authentication result.
     *
     * @name AuthResult
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface AuthResult {
        /**
         * Indicates the authentication token.
         */
        token?: Uint8Array;

        /**
         * Indicates the remaining times.
         */
        remainTimes?: number;

        /**
         * Indicates the freezing times.
         */
        freezingTime?: number;
    }

    /**
     * Indicates the information of credential.
     *
     * @name CredentialInfo
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface CredentialInfo {
        /**
         * Indicates the credential type.
         */
        credType: AuthType;

        /**
         * Indicates the credential subtype.
         */
        credSubType: AuthSubType;

        /**
         * Indicates the authentication token.
         */
        token: Uint8Array;
    }

    /**
     * Indicates the information of request result.
     *
     * @name RequestResult
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface RequestResult {
        /**
         * Indicates the credential index.
         */
        credentialId?: Uint8Array;
    }

    /**
     * Indicates the information of enrolled credential.
     *
     * @name EnrolledCredInfo
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    interface EnrolledCredInfo {
        /**
         * Indicates the credential index.
         */
        credentialId: Uint8Array;

        /**
         * Indicates the authentication credential type.
         */
        authType: AuthType;

        /**
         * Indicates the authentication credential subtype.
         */
        authSubType: AuthSubType;

        /**
         * Indicates the credential template ID.
         */
        templateId: Uint8Array;
    }

    /**
     * Indicates the property type to get.
     *
     * @name GetPropertyType
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum GetPropertyType {
        /**
         * Indicates the authentication subtype.
         */
        AUTH_SUB_TYPE = 1,

        /**
         * Indicates the remain times.
         */
        REMAIN_TIMES = 2,

        /**
         * Indicates the freezing time.
         */
        FREEZING_TIME = 3
    }

    /**
     * Indicates the property type to set.
     *
     * @name SetPropertyType
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum SetPropertyType {
        /**
         * Indicates the init algorithm.
         */
        INIT_ALGORITHM = 1,
    }

    /**
     * Indicates the credential type for authentication.
     *
     * @name AuthType
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum AuthType {
        /**
         * Indicates the PIN authentication type.
         */
        PIN = 1,

        /**
         * Indicates the FACE authentication type.
         */
        FACE = 2
    }

    /**
     * Indicates the credential subtype for authentication.
     *
     * @name AuthSubType
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum AuthSubType {
        /**
         * Indicates the 6-digit credential.
         */
        PIN_SIX = 10000,

        /**
         * Indicates the self-defined digital credential.
         */
        PIN_NUMBER = 10001,

        /**
         * Indicates the self-defined mixed credential.
         */
        PIN_MIXED = 10002,
    
        /**
         * Indicates the 2D face credential.
         */
        FACE_2D = 20000,

        /**
         * Indicates the 3D face credential.
         */
        FACE_3D = 20001
    }

    /**
     * Indicates the trusted level of authentication results.
     *
     * @name AuthTrustLevel
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum AuthTrustLevel {
        /**
         * Indicates the trusted level 1.
         */
        ATL1 = 10000,

        /**
         * Indicates the trusted level 2.
         */
        ATL2 = 20000,

        /**
         * Indicates the trusted level 3.
         */
        ATL3 = 30000,

        /**
         * Indicates the trusted level 4.
         */
        ATL4 = 40000
    }

    /**
     * Indicates the module of acquired information.
     *
     * @name Module
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum Module {
        /**
         * Indicates the information acquired from FaceAuth.
         */
        FACE_AUTH = 1
    }

    /**
     * Indicates the enumeration of authentication result code.
     *
     * @name ResultCode
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum ResultCode {
        /**
         * Indicates that authentication is success or ability is supported.
         */
        SUCCESS = 0,

        /**
         * Indicates the authenticator fails to identify user.
         */
        FAIL = 1,

        /**
         * Indicates other errors.
         */
        GENERAL_ERROR = 2,

        /**
         * Indicates that authentication has been canceled.
         */
        CANCELED = 3,

        /**
         * Indicates that authentication has timed out.
         */
        TIMEOUT = 4,

        /**
         * Indicates that this authentication type is not supported.
         */
        TYPE_NOT_SUPPORT = 5,

        /**
         * Indicates that the authentication trust level is not supported.
         */
        TRUST_LEVEL_NOT_SUPPORT = 6,

        /**
         * Indicates that the authentication task is busy. Wait for a few seconds and try again.
         */
        BUSY = 7,

        /**
         * Indicates incorrect parameters.
         */
        INVALID_PARAMETERS = 8,

        /**
         * Indicates that the authenticator is locked.
         */
        LOCKED = 9,

        /**
         * Indicates that the user has not enrolled the authenticator.
         */
        NOT_ENROLLED = 10
    }

    /**
     * Indicates the enumeration of prompt codes in the process of face authentication.
     *
     * @name FaceTipsCode
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum FaceTipsCode {
        /**
         * Indicates that the obtained facial image is too bright due to high illumination.
         */
        FACE_AUTH_TIP_TOO_BRIGHT = 1,

        /**
         * Indicates that the obtained facial image is too dark due to low illumination.
         */
        FACE_AUTH_TIP_TOO_DARK = 2,

        /**
         * Indicates that the face is too close to the device.
         */
        FACE_AUTH_TIP_TOO_CLOSE = 3,

        /**
         * Indicates that the face is too far away from the device.
         */
        FACE_AUTH_TIP_TOO_FAR = 4,

        /**
         * Indicates that the device is too high, and that only the upper part of the face is captured.
         */
        FACE_AUTH_TIP_TOO_HIGH = 5,

        /**
         * Indicates that the device is too low, and that only the lower part of the face is captured.
         */
        FACE_AUTH_TIP_TOO_LOW = 6,

        /**
         * Indicates that the device is deviated to the right, and that only the right part of the face is captured.
         */
        FACE_AUTH_TIP_TOO_RIGHT = 7,

        /**
         * Indicates that the device is deviated to the left, and that only the left part of the face is captured.
         */
        FACE_AUTH_TIP_TOO_LEFT = 8,

        /**
         * Indicates that the face moves too fast during facial information collection.
         */
        FACE_AUTH_TIP_TOO_MUCH_MOTION = 9,

        /**
         * Indicates that the face is not facing the device.
         */
        FACE_AUTH_TIP_POOR_GAZE = 10,

        /**
         * Indicates that no face is detected.
         */
        FACE_AUTH_TIP_NOT_DETECTED = 11,
    }

    /**
     * Indicates the enumeration of prompt codes in the process of fingerprint authentication.
     *
     * @name FingerprintTips
     * @since 8
     * @syscap SystemCapability.Account.OsAccount
     * @systemapi Hide this for inner system use.
     */
    enum FingerprintTips {
        /**
         * Indicates that the image acquired is good.
         */
        FINGERPRINT_TIP_GOOD = 0,

        /**
         * Indicates that the fingerprint image is too noisy due to suspected or detected dirt on the sensor.
         */
        FINGERPRINT_TIP_IMAGER_DIRTY = 1,

        /**
         * Indicates that the fingerprint image is too noisy to process due to a detected condition.
         */
        FINGERPRINT_TIP_INSUFFICIENT = 2,

        /**
         * Indicates that only a partial fingerprint image is detected.
         */
        FINGERPRINT_TIP_PARTIAL = 3,

        /**
         * Indicates that the fingerprint image is incomplete due to quick motion.
         */
        FINGERPRINT_TIP_TOO_FAST = 4,

        /**
         * Indicates that the fingerprint image is unreadable due to lack of motion.
         */
        FINGERPRINT_TIP_TOO_SLOW = 5
    }
}

export default osAccount;