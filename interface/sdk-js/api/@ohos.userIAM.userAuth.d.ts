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

import { AsyncCallback } from './basic';

/**
 * User authentication
 * @since 6
 * @syscap SystemCapability.UserIAM.UserAuth.Core
 * @permission ohos.permission.ACCESS_BIOMETRIC
 */
declare namespace userAuth {
    export enum AuthenticationResult {
        /**
         * Indicates that the device does not support authentication.
         * @deprecated since 8
         */
        NO_SUPPORT = -1,

        /**
         * Indicates that authentication is success.
         * @deprecated since 8
         */
        SUCCESS = 0,

        /**
         * Indicates the authenticator fails to identify user.
         * @deprecated since 8
         */
        COMPARE_FAILURE = 1,

        /**
         * Indicates that authentication has been canceled.
         * @deprecated since 8
         */
        CANCELED = 2,

        /**
         * Indicates that authentication has timed out.
         * @deprecated since 8
         */
        TIMEOUT = 3,

        /**
         * Indicates a failure to open the camera.
         * @deprecated since 8
         */
        CAMERA_FAIL = 4,

        /**
         * Indicates that the authentication task is busy. Wait for a few seconds and try again.
         * @deprecated since 8
         */
        BUSY = 5,

        /**
         * Indicates incorrect parameters.
         * @deprecated since 8
         */
        INVALID_PARAMETERS = 6,

        /**
         * Indicates that the authenticator is locked.
         * @deprecated since 8
         */
        LOCKED = 7,

        /**
         * Indicates that the user has not enrolled the authenticator.
         * @deprecated since 8
         */
        NOT_ENROLLED = 8,

        /**
         * Indicates other errors.
         * @deprecated since 8
         */
        GENERAL_ERROR = 100,
    }

    /**
     * Auth types
     * @deprecated since 8
     */
    type AuthType = "ALL" | "FACE_ONLY";

    /**
     * Secure levels
     * @deprecated since 8
     */
    type SecureLevel = "S1" | "S2" | "S3" | "S4";

    interface Authenticator {
        /**
         * Execute authentication.
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @param type Indicates the authentication type.
         * @param level Indicates the security level.
         * @return Returns authentication result, which is specified by AuthenticationResult.
         * @deprecated since 8
         */
        execute(type: AuthType, level: SecureLevel, callback: AsyncCallback<number>): void;
        execute(type: AuthType, level: SecureLevel): Promise<number>;
    }

    /**
     * Get Authenticator instance.
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     * @return Returns an Authenticator.
     * @deprecated since 8
     */
    function getAuthenticator(): Authenticator;

    /**
     * User authentication.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    class UserAuth {
        /**
         * Constructor to get the UserAuth class instance.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @return Returns the UserAuth class instance.
         */
        constructor();

        /**
         * Get version information.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @return Returns version information.
         */
        getVersion() : number;

        /**
         * Check whether the authentication capability is available.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @permission ohos.permission.ACCESS_BIOMETRIC
         * @param authType Credential type for authentication.
         * @param authTrustLevel Trust level of authentication result.
         * @return Returns a check result, which is specified by getAvailableStatus.
         */
        getAvailableStatus(authType : UserAuthType, authTrustLevel : AuthTrustLevel) : number;

        /**
         * Executes authentication.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @permission ohos.permission.ACCESS_BIOMETRIC
         * @param challenge pass in challenge value.
         * @param authType type of authentication.
         * @param authTrustLevel Trust level of authentication result.
         * @param callback Return result and acquireinfo through callback.
         * @return Returns ContextId for cancel.
         */
        auth(challenge: Uint8Array, authType: UserAuthType, authTrustLevel: AuthTrustLevel, callback: IUserAuthCallback): Uint8Array;

        /**
         * Cancels authentication with ContextID.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @permission ohos.permission.ACCESS_BIOMETRIC
         * @param contextID Cancel authentication and pass in ContextID.
         * @return Returns a number value indicating whether Cancel authentication was successful.
         */
        cancelAuth(contextID : Uint8Array) : number;
    }

    interface IUserAuthCallback {
        /**
         * The authentication result code is returned through the callback.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @param result authentication result code.
         * @param extraInfo pass the specific information for different situation.
         * If the authentication is passed, the authentication token is returned in extrainfo,
         * If the authentication fails, the remaining authentication times are returned in extrainfo,
         * If the authentication executor is locked, the freezing time is returned in extrainfo.
         */
        onResult: (result : number, extraInfo : AuthResult) => void;

        /**
         * During an authentication, the TipsCode is returned through the callback.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         * @param module the executor type for authentication.
         * @param acquire the tip code for different authentication executor.
         * @param extraInfo reserved parameter.
         */
        onAcquireInfo ?: (module : number, acquire : number, extraInfo : any) => void;
    }

    /**
     * Authentication result: authentication token, remaining authentication times, freezing time.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     * @param token pass the authentication result if the authentication is passed.
     * @param remainTimes return the remaining authentication times if the authentication fails.
     * @param freezingTime return the freezing time if the authectication executor is locked.
     */
    interface AuthResult {
        token ?: Uint8Array;
        remainTimes ?: number;
        freezingTime ?: number;
    }

    /**
     * Result code.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    enum ResultCode {
        /**
         * Indicates that the result is success or ability is supported.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        SUCCESS = 0,

        /**
         * Indicates the the result is failure or ability is not supported.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FAIL = 1,

        /**
         * Indicates other errors.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        GENERAL_ERROR = 2,

        /**
         * Indicates that this operation has been canceled.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        CANCELED = 3,

        /**
         * Indicates that this operation has timed out.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        TIMEOUT = 4,

        /**
         * Indicates that this authentication type is not supported.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        TYPE_NOT_SUPPORT = 5,

        /**
         * Indicates that the authentication trust level is not supported.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        TRUST_LEVEL_NOT_SUPPORT = 6,

        /**
         * Indicates that the authentication task is busy. Wait for a few seconds and try again.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        BUSY = 7,

        /**
         * Indicates incorrect parameters.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        INVALID_PARAMETERS = 8,

        /**
         * Indicates that the authenticator is locked.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        LOCKED = 9,

        /**
         * Indicates that the user has not enrolled the authenticator.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        NOT_ENROLLED = 10
    }

    /**
     * Indicates the enumeration of prompt codes in the process of face authentication.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    enum FaceTips {
        /**
         * Indicates that the obtained facial image is too bright due to high illumination.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_BRIGHT = 1,

        /**
         * Indicates that the obtained facial image is too dark due to low illumination.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_DARK = 2,

        /**
         * Indicates that the face is too close to the device.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_CLOSE = 3,

        /**
         * Indicates that the face is too far away from the device.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_FAR = 4,

        /**
         * Indicates that the device is too high, and that only the upper part of the face is captured.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_HIGH = 5,

        /**
         * Indicates that the device is too low, and that only the lower part of the face is captured.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_LOW = 6,

        /**
         * Indicates that the device is deviated to the right, and that only the right part of the face is captured.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_RIGHT = 7,

        /**
         * Indicates that the device is deviated to the left, and that only the left part of the face is captured.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_LEFT = 8,

        /**
         * Indicates that the face moves too fast during facial information collection.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_TOO_MUCH_MOTION = 9,

        /**
         * Indicates that the face is not facing the device.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_POOR_GAZE = 10,

        /**
         * Indicates that no face is detected.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE_AUTH_TIP_NOT_DETECTED = 11,
    }

    /**
     * Indicates the enumeration of prompt codes in the process of fingerprint authentication.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    enum FingerprintTips {
        /**
         * Indicates that the image acquired is good.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_GOOD = 0,

        /**
         * Indicates that the fingerprint image is too noisy due to suspected or detected dirt on sensor.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_DIRTY = 1,

        /**
         * Indicates that the fingerprint image is too noisy to process due to a detected condition.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_INSUFFICIENT = 2,

        /**
         * Indicates that only a partial fingerprint image is detected.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_PARTIAL = 3,

        /**
         * Indicates that the fingerprint image is incomplete due to quick motion.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_TOO_FAST = 4,

        /**
         * Indicates that the fingerprint image is unreadable due to lack of motion.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT_AUTH_TIP_TOO_SLOW = 5
    }

    /**
     * Credential type for authentication.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    enum UserAuthType {
        /**
         * Authentication type face.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FACE = 2,

        /**
         * Authentication type fingerprint.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        FINGERPRINT = 4
    }

    /**
     * Trust level of authentication results.
     * @since 8
     * @syscap SystemCapability.UserIAM.UserAuth.Core
     */
    enum AuthTrustLevel {
        /**
         * Authentication result trusted level 1.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        ATL1 = 10000,

        /**
         * Authentication result trusted level 2.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        ATL2 = 20000,

        /**
         * Authentication result trusted level 3.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        ATL3 = 30000,

        /**
         * Authentication result trusted level 4.
         * @since 8
         * @syscap SystemCapability.UserIAM.UserAuth.Core
         */
        ATL4 = 40000
    }
}

export default userAuth;
