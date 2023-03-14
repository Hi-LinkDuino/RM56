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

var GrantStatus = {
    PERMISSION_DENIED: -1,
    PERMISSION_GRANTED: 0,
};
const RESULT_SUCCESS = 0
const RESULT_FAIL = -1
const TIMEOUT = 5000;

const DEFAULT_PERMISSION_FALG = 0
var permissionNameUser = "ohos.permission.ALPHA";
var permissionNameSystem = "ohos.permission.BETA";
var tokenID = undefined
describe('AccessTokenTest', function () {
    console.log('##########start AccessTokenTest');
    beforeAll(async function (done){
        var appInfo = await bundle.getApplicationInfo('ohos.acts.security.access_token.normal', 0, 100);
        tokenID = appInfo.accessTokenId;
        console.log("AccessTokenTest accessTokenId:" + appInfo.accessTokenId + ", name:" + appInfo.name
            + ", bundleName:" + appInfo.bundleName)
        // setTimeout(done(),TIMEOUT);

        console.info("sleep begin");
        sleep(TIMEOUT);
        console.info("sleep end");
        done();
    })
    afterEach(function () {
    })

    function sleep(delay) {
        var start = (new Date()).getTime();
        var next = (new Date()).getTime();
        while (next - start < delay) {
            next = (new Date()).getTime();
        }
    }

    /**
     * @tc.number Test_verifyAccessToken_001
     * @tc.name Test atManager.verifyAccessToken.
     * @tc.desc After the installation, user_grant permission is not granted by default(Promise).
     */
    it('Test_verifyAccessToken_001', 0, async function(done){
        console.log("Test_verifyAccessToken_001 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.verifyAccessToken(tokenID, permissionNameUser);
        console.log("Test_verifyAccessToken_001 tokenID" + tokenID + "-" + result)
        expect(result).assertEqual(GrantStatus.PERMISSION_DENIED);

        done();
    })

    /**
     * @tc.number Test_verifyAccessToken_002
     * @tc.name Test atManager.verifyAccessToken.
     * @tc.desc After the installation, system_grant permission is not granted by default(Promise).
     */
    it('Test_verifyAccessToken_002', 0, async function(done){
        console.log("Test_verifyAccessToken_002 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.verifyAccessToken(tokenID, permissionNameSystem);
        console.log("Test_verifyAccessToken_002 tokenID" + tokenID + "-" + result)
        expect(result).assertEqual(GrantStatus.PERMISSION_GRANTED);

        done();
    })

    /**
     * @tc.number Test_verifyAccessToken_003
     * @tc.name Test atManager.verifyAccessToken.
     * @tc.desc Test invalid TokenID(0)(Promise).
     */
    it('Test_verifyAccessToken_003', 0, async function(done){
        console.log("Test_verifyAccessToken_003 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.verifyAccessToken(0, permissionNameUser);
        console.log("Test_verifyAccessToken_003 tokenID" + tokenID + "-" + result)
        expect(result).assertEqual(GrantStatus.PERMISSION_DENIED);

        done();
    })

    /**
     * @tc.number Test_verifyAccessToken_004
     * @tc.name Test atManager.verifyAccessToken.
     * @tc.desc Test invalid permission(empty)(Promise).
     */
    it('Test_verifyAccessToken_004', 0, async function(done){
        console.log("Test_verifyAccessToken_004 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.verifyAccessToken(tokenID, "");
        console.log("Test_verifyAccessToken_004 tokenID" + tokenID + "-" + result)
        expect(result).assertEqual(GrantStatus.PERMISSION_DENIED);

        done();
    })

    /**
     * @tc.number Test_verifyAccessToken_005
     * @tc.name Test atManager.verifyAccessToken.
     * @tc.desc Test invalid permission(length exceeds 256)(Promise).
     */
    it('Test_verifyAccessToken_005', 0, async function(done){
        console.log("Test_verifyAccessToken_005 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var permissionName = "ohos.permission.testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest";

        var result = await atManager.verifyAccessToken(tokenID, permissionName);
        console.log("Test_verifyAccessToken_005 tokenID" + tokenID + "-" + result)
        expect(result).assertEqual(GrantStatus.PERMISSION_DENIED);

        done();
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_001
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test grant user_grant permission(Promise).
     */
    it('Test_grantUserGrantedPermission_001', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_001 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionNameUser,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_001 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_SUCCESS);

        var result1 = await atManager.verifyAccessToken(tokenID, permissionNameUser);
        console.log("Test_grantUserGrantedPermission_001 tokenID" + tokenID + "-result1" + result1)
        expect(result1).assertEqual(GrantStatus.PERMISSION_GRANTED);

        done();
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_002
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test grant user_grant permission(Promise).
     */
    it('Test_grantUserGrantedPermission_002', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_002 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionNameSystem,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_002 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_003
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test grant user_grant permission(Promise).
     */
    it('Test_grantUserGrantedPermission_003', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_003 start");
        var atManager = abilityAccessCtrl.createAtManager();
        atManager.grantUserGrantedPermission(tokenID, permissionNameUser, PermissionFlag.PERMISSION_USER_SET,
            function (result) {
                console.log("Test_grantUserGrantedPermission_003 tokenID" + tokenID + "-" + result)
                expect(result).assertEqual(RESULT_SUCCESS);

                atManager.verifyAccessToken(tokenID, permissionNameUser).then(function (result1) {
                    console.log("Test_grantUserGrantedPermission_003 tokenID" + tokenID + "-result1" + result1)
                    expect(result1).assertEqual(GrantStatus.PERMISSION_GRANTED);
                });
                done();
            });
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_004
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test invalid TokenID(0)(Promise).
     */
    it('Test_grantUserGrantedPermission_004', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_004 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(0, permissionNameUser,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_004 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_005
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test invalid permission(empty)(Promise).
     */
    it('Test_grantUserGrantedPermission_005', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_005 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, "", PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_005 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_grantUserGrantedPermission_006
     * @tc.name Test atManager.grantUserGrantedPermission.
     * @tc.desc Test invalid permission(length exceeds 256)(Promise).
     */
    it('Test_grantUserGrantedPermission_006', 0, async function(done){
        console.log("Test_grantUserGrantedPermission_006 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var permissionName = "ohos.permission.testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest";
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionName,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_grantUserGrantedPermission_006 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_001
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test revoke user_grant permission(Promise).
     */
    it('Test_revokeUserGrantedPermission_001', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_001 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.revokeUserGrantedPermission(tokenID, permissionNameUser,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_001 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_SUCCESS);

        var result1 = await atManager.verifyAccessToken(tokenID, permissionNameUser);
        console.log("Test_revokeUserGrantedPermission_001 tokenID" + tokenID + "-result1" + result1)
        expect(result1).assertEqual(GrantStatus.PERMISSION_DENIED);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_002
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test revoke system_grant permission(Promise).
     */
    it('Test_revokeUserGrantedPermission_002', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_002 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.revokeUserGrantedPermission(tokenID, permissionNameSystem,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_002 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_003
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test revoke user_grant permission(AsyncCallback).
     */
    it('Test_revokeUserGrantedPermission_003', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_003 start");
        var atManager = abilityAccessCtrl.createAtManager();
        atManager.revokeUserGrantedPermission(tokenID, permissionNameUser, PermissionFlag.PERMISSION_USER_SET,
            function (result) {
                console.log("Test_revokeUserGrantedPermission_003 tokenID" + tokenID + "-" + result)
                expect(result).assertEqual(RESULT_SUCCESS);

                atManager.verifyAccessToken(tokenID, permissionNameUser).then(function (result1) {
                    console.log("Test_revokeUserGrantedPermission_003 tokenID" + tokenID + "-result1" + result1)
                    expect(result1).assertEqual(GrantStatus.PERMISSION_DENIED);
                });
                done();
            });
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_004
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test invalid TokenID(0)(Promise).
     */
    it('Test_revokeUserGrantedPermission_004', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_004 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.revokeUserGrantedPermission(0, permissionNameUser,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_004 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_005
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test invalid permission(empty)(Promise).
     */
    it('Test_revokeUserGrantedPermission_005', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_005 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.revokeUserGrantedPermission(tokenID, "", PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_005 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_revokeUserGrantedPermission_006
     * @tc.name Test atManager.revokeUserGrantedPermission.
     * @tc.desc Test invalid permission(length exceeds 256)(Promise).
     */
    it('Test_revokeUserGrantedPermission_006', 0, async function(done){
        console.log("Test_revokeUserGrantedPermission_006 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var permissionName = "ohos.permission.testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest";
        var result = await atManager.revokeUserGrantedPermission(tokenID, permissionName,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_revokeUserGrantedPermission_006 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_FAIL);

        done();
    })

    /**
     * @tc.number Test_getPermissionFlag_001
     * @tc.name Test atManager.getPermissionFlags.
     * @tc.desc Test get permission flag(Promise).
     */
    it('Test_getPermissionFlag_001', 0, async function(done){
        console.log("Test_getPermissionFlag_001 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionNameUser,
            PermissionFlag.PERMISSION_USER_SET);
        console.log("Test_getPermissionFlag_001 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(RESULT_SUCCESS);

        var result1 = await atManager.getPermissionFlags(tokenID, permissionNameUser);
        console.log("Test_getPermissionFlag_001 tokenID" + tokenID + "-result1" + result1)
        expect(result1).assertEqual(PermissionFlag.PERMISSION_USER_SET);

        done();
    })

    /**
     * @tc.number Test_getPermissionFlag_002
     * @tc.name Test atManager.getPermissionFlags.
     * @tc.desc Test invalid TokenID(0)(Promise).
     */
    it('Test_getPermissionFlag_002', 0, async function(done){
        console.log("Test_getPermissionFlag_002 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.getPermissionFlags(0, permissionNameUser);
        console.log("Test_getPermissionFlag_002 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(DEFAULT_PERMISSION_FALG);

        done();
    })

    /**
     * @tc.number Test_getPermissionFlag_003
     * @tc.name Test atManager.getPermissionFlags.
     * @tc.desc Test invalid permission(empty)(Promise).
     */
    it('Test_getPermissionFlag_003', 0, async function(done){
        console.log("Test_getPermissionFlag_003 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.getPermissionFlags(tokenID, "");
        console.log("Test_getPermissionFlag_003 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(DEFAULT_PERMISSION_FALG);

        done();
    })

    /**
     * @tc.number Test_getPermissionFlag_004
     * @tc.name Test atManager.getPermissionFlags.
     * @tc.desc Test invalid permission(length exceeds 256)(Promise).
     */
    it('Test_getPermissionFlag_004', 0, async function(done){
        console.log("Test_getPermissionFlag_004 start");
        var atManager = abilityAccessCtrl.createAtManager();
        var permissionName = "ohos.permission.testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest"
            + "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest";
        var result = await atManager.getPermissionFlags(tokenID, permissionName);
        console.log("Test_getPermissionFlag_004 tokenID" + tokenID + "-" + result);
        expect(result).assertEqual(DEFAULT_PERMISSION_FALG);

        done();
    })
})
