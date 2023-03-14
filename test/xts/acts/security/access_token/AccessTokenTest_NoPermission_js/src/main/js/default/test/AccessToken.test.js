/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permission and
 * limitations under the License.
 */

// @ts-nocheck
import {describe, beforeEach, afterEach, it, expect} from 'deccjsunit/index'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'

var PermissionFlag = {
    PERMISSION_USER_SET: 1,
    PERMISSION_USER_FIXED: 2,
    PERMISSION_SYSTEM_FIXED: 3
};

const RESULT_FAIL = -1
const TIMEOUT = 1000;
const DEFAULT_PERMISSION_FALG = 0
var permissionNameUser = "ohos.permission.ALPHA";
var tokenID = undefined
describe('AccessTokenTest',function(){
    console.log('##########start AccessTokenTests');
    beforeAll(async function (done){
        var appInfo = await bundle.getApplicationInfo('ohos.acts.security.access_token.nopermission',0 ,100);
        tokenID = appInfo.accessTokenId;
        console.log("AccessTokenTest accessTokenId:" + appInfo.accessTokenId + ", name:"+ appInfo.name
			+ ", bundleName:" + appInfo.bundleName)
        console.info("sleep begin");
        sleep(TIMEOUT);
        console.info("sleep end");
        done();
    })
    afterEach(function(){
    })
    function sleep(delay){
        var start = (new Date()).getTime();
        var next = (new Date()).getTime();
        while (next - start < delay){
            next = (new Date()).getTime();
        }
    }

    /**
     * @tc.number Test_grantUserGrantedPermission_007
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Grant permission that has ohos.permission.GRANT_SENSITIVE_PERMISSIONS.
     */
    it('Test_grantUserGrantedPermission_007', 0, async function(done){
        console.info("sleep begin");
        sleep(1000);
        console.info("sleep end");
        console.log("Test_grantUserGrantedPermission_007 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionNameUser,
			PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_007 tokenID"+ tokenID + "-" + result)
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_007
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Revoke permission that has ohos.permission.GRANT_SENSITIVE_PERMISSIONS.
     */
    it('Test_revokeUserGrantedPermission_007', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_007 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.revokeUserGrantedPermission(tokenID, permissionNameUser,
			PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_007 tokenID"+ tokenID + "-" + result)
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_getPermissionFlags_005
     * @tc.name Test atManager.getPermissionFlags.
     * @tc.desc Get permission flag that has ohos.permission.GRANT_SENSITIVE_PERMISSIONS.
     */
    it('Test_getPermissionFlags_005', 0, async function(done){
        console.log("Test_getPermissionFlags_005 start");
        console.log("Test_getPermissionFlags_005 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.getPermissionFlags(tokenID, permissionNameUser);
        console.log("Test_getPermissionFlags_005 tokenID"+ tokenID + "-" + result)
        expect(result).assertEqual(DEFAULT_PERMISSION_FALG);

        done();
    })
})
