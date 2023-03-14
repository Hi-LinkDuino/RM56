/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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
import wantConstant from "./@ohos.ability.wantConstant";

/**
 * The management class for uri of file.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 */
declare namespace uriPermissionManager {
    /**
     * Check whether the application corresponding to the accesstokenID has access rights to the URI.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param uri File URI.
     * @param flag wantConstant.Flags.FLAG_AUTH_READ_URI_PERMISSION or wantConstant.Flags.FLAG_AUTH_WRITE_URI_PERMISSION
     * @param accessTokenId Indicates the access token of the application.
     * @return Returns 0 if the verification is successful, otherwise returns -1.
     */
    function verifyUriPermission(uri: string, flag: wantConstant.Flags, accessTokenId: number, callback: AsyncCallback<number>): void;
    function verifyUriPermission(uri: string, flag: wantConstant.Flags, accessTokenId: number): Promise<number>;
}

export default uriPermissionManager;