/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { describe, beforeAll,afterAll, it, expect } from 'deccjsunit/index';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import bundle from '@ohos.bundle';

describe("get_permission", function () {

  /**
   * @tc.number SUB_DF_GRANT_USER_GRANTED_PERMISSION_0000
   * @tc.name grant_user_granted_permission_async_000
   * @tc.desc Test grantUserGrantedPermission() interfaces, grant permission.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
    it("grant_user_granted_permission_async_000", 0, async function (done) {
        let appInfo = await bundle.getApplicationInfo('ohos.acts.multimedia.mediaLibrary', 0, 100);
        let tokenID = appInfo.accessTokenId;
        let atManager = abilityAccessCtrl.createAtManager();
        let result1 = await atManager.grantUserGrantedPermission(tokenID, "ohos.permission.MEDIA_LOCATION",1);
        let result2 = await atManager.grantUserGrantedPermission(tokenID, "ohos.permission.READ_MEDIA",1);
        let result3 = await atManager.grantUserGrantedPermission(tokenID, "ohos.permission.WRITE_MEDIA",1);
        let isGranted1 = await atManager.verifyAccessToken(tokenID, "ohos.permission.MEDIA_LOCATION");
        let isGranted2 = await atManager.verifyAccessToken(tokenID, "ohos.permission.READ_MEDIA");
        let isGranted3 = await atManager.verifyAccessToken(tokenID, "ohos.permission.WRITE_MEDIA");
        expect(result1 == 0 && result2 == 0 && result3 == 0).assertTrue();
        expect(isGranted1 == 0 && isGranted2 == 0 && isGranted3 == 0).assertTrue();
        done();
    });
});