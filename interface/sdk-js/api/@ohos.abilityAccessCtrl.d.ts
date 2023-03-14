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

import { AsyncCallback } from "./basic";

/**
 * @syscap SystemCapability.Security.AccessToken
 */
 declare namespace abilityAccessCtrl {
    /**
     * Obtains the AtManager instance.
     * @return returns the instance of the AtManager.
     * @since 8
     */
    function createAtManager(): AtManager;
  
    /**
     * Provides methods for managing access_token.
     * @name AtManager
     */
    interface AtManager {
        /**
         * Checks whether a specified application has been granted the given permission.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be verified.
         * @return Returns permission verify result
         * @since 8
         */
        verifyAccessToken(tokenID: number, permissionName: string): Promise<GrantStatus>;

        /**
         * Grants a specified user_grant permission to the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be granted.
         * @param permissionFlag Flag of permission state.
         * @permission ohos.permission.GRANT_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        grantUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number): Promise<number>;
        grantUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number, callback: AsyncCallback<number>): void;

        /**
         * Revokes a specified user_grant permission to the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be revoked.
         * @param permissionFlag Flag of permission state.
         * @permission ohos.permission.REVOKE_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        revokeUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number): Promise<number>;
        revokeUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number, callback: AsyncCallback<number>): void;

        /**
         * Queries specified permission flag of the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be granted.
         * @return Return permission flag.
         * @permission ohos.permission.GET_SENSITIVE_PERMISSIONS or ohos.permission.GRANT_SENSITIVE_PERMISSIONS or ohos.permission.REVOKE_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        getPermissionFlags(tokenID: number, permissionName: string): Promise<number>;
    }
  
    /**
     * GrantStatus.
     * @since 8
     */
    export enum GrantStatus {
        /**
         * access_token permission check fail
         */
        PERMISSION_DENIED = -1,
        /**
         * access_token permission check success
         */
        PERMISSION_GRANTED = 0,
    }
  }

  export default abilityAccessCtrl;
