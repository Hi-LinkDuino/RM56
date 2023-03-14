/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {describe, it, expect} from 'deccjsunit/index'
import userAuth from '@ohos.userAuth'
import userIDM from '@ohos.userIDM'
import pinAuth from '@ohos.pinAuth'
import * as publicFC from './Publicfunction-n'

let UserIDM = new userIDM.UserIdentityManager();
let PinAuth = new pinAuth.PINAuth();
let UserAuth = new userAuth.UserAuth();

let AuthType = {
    PIN: 1,
    FACE: 2
}
let AuthSubType = {
    PIN_SIX: 10000,
    PIN_NUMBER: 10001,
    PIN_MIXED: 10002,
    FACE_2D: 20000,
    FACE_3D: 20001
}
let AuthTurstLevel = {
    ATL1: 10000,
    ATL2: 20000,
    ATL3: 30000,
    ATL4: 40000
}

let SetPropertyType = {
    PROCESS_ALGORITHM: 1,
}

let userID = {
    User1: 0,
    User2: 2,
    User3: 3,
    User4: 4,
    User5: 5
}

let ResultCode = {
    SUCCESS: 0,
    FAIL: 1,
    GENERAL_ERROR: 2,
    CANCELED: 3,
    TIMEOUT: 4,
    TYPE_NOT_SUPPORT: 5,
    TRUST_LEVEL_NOT_SUPPORT: 6,
    BUSY: 7,
    INVALID_PARAMETERS: 8,
    LOCKED: 9,
    NOT_ENROLLED: 10,
    Authfail: 14
}

let GetPropertyType = {
    AUTH_SUB_TYPE: 1,
    REMAIN_TIMES: 2,
    FREEZING_TIME: 3
}

let Inputerdata = new Uint8Array([1, 2, 3, 4, 5, 6]);

let GetPropertyTypearray = new Array();
GetPropertyTypearray[0] = GetPropertyType.AUTH_SUB_TYPE;
GetPropertyTypearray[1] = GetPropertyType.FREEZING_TIME;
GetPropertyTypearray[2] = GetPropertyType.REMAIN_TIMES;

let GetPropertyRequestpin = {
    authType: AuthType.PIN,
    keys: GetPropertyTypearray
}

let GetPropertyRequestface = {
    authType: AuthType.FACE,
    keys: GetPropertyTypearray
}

let CredentialInfopinsix = {
    credType: AuthType.PIN,
    credSubType: AuthSubType.PIN_SIX,
    token: null
}

let CredentialInfopinnum = {
    credType: AuthType.PIN,
    credSubType: AuthSubType.PIN_NUMBER,
    token: null
}

let CredentialInfopinmix = {
    credType: AuthType.PIN,
    credSubType: AuthSubType.PIN_MIXED,
    token: null
}

let CredentialInfoface2d = {
    credType: AuthType.FACE,
    credSubType: AuthSubType.FACE_2D,
    token: null
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

describe('userauthTest', function () {

	/**
    * @tc.name      the token from old session is not available in the new session
    * @tc.number    Security_IAM_Coauth_DFX_0105
    * @tc.size      MEDIUM
    * @tc.type      FUNC
    * @tc.level     Level3
    */
    it('Security_IAM_Coauth_DFX_0105', 1, async function (done) {
        console.info('testFace Security_IAM_Coauth_DFX_0105 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata);
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('testFace Security_IAM_Coauth_DFX_0105 publicOpenSession data = ' + JSON.stringify(data));
                let challenge1 = data;
                publicFC.publicOpenSession(UserIDM, function (data) {
                    console.info('testFace Security_IAM_Coauth_DFX_0105 publicOpenSession data = ' + JSON.stringify(data));
                    let challenge2 = data;
                    publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                        console.info('testFace Security_IAM_Coauth_DFX_0105 callback data = ' + JSON.stringify(data));
                        let token1;
                        console.info('testFace Security_IAM_Coauth_DFX_0105 addSuccess');
                        publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                            console.info('testFace Coauth_DFX_0105 publicauth = '  + JSON.stringify(data));
                            console.info('testFace Coauth_DFX_0105 publicauth.authresult = '  + data.authresult);
                            console.info('testFace Coauth_DFX_0105 ResultCode.SUCCESS = '  + ResultCode.SUCCESS);
                            expect(ResultCode.SUCCESS).assertEqual(data.authresult);
                            token1 = data.authextr.token;
                            publicFC.publicauth(UserAuth, challenge2, AuthType.PIN, AuthTurstLevel.ATL1,
                                function (data) {
                                console.info('testFace Security_IAM_Coauth_DFX_0105 publicauth');
                                let token2;
                                token2 = data.authextr.token;
                                console.info('testFace Coauth_DFX_0105 publicauth.authresult2 = '  + data.authresult);
                                console.info('testFace Coauth_DFX_0105 ResultCode.SUCCESS = '  + ResultCode.SUCCESS);
                                expect(ResultCode.SUCCESS).assertEqual(data.authresult);
                                console.info("testFace Security_IAM_Coauth_DFX_0105 token = " + token2);
                                CredentialInfoface2d.token = token1;
                                publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (data) {
                                    console.info('testFace Coauth_DFX_0105 addCredresult = '  + data.addCredresult);
                                    expect(ResultCode.FAIL).assertEqual(data.addCredresult);
                                    publicFC.publicdelUser(UserIDM, token2, function (data) {
                                        console.info("testFace Security_IAM_Coauth_DFX_0105 publicdelUser");
                                        let deluserresult = data.delUserresult;
                                        console.info("testFace Coauth_DFX_0105 delUser="+ deluserresult);
                                        publicFC.publicCloseSession(UserIDM, function (data) {
                                            console.info("testFace Coauth_DFX_0105 CloseSession= " + data);
                                            publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                                console.info("testFace Coauth_DFX_0105 unRegister= " + data);
                                                done();
                                            })
                                        })
                                    }, function (data) {
                                    })
                                }, function (data) {
                                })
                            }, function (data) {
                            })
                        }, function (data) {
                        })
                    }, function (data) {
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_Coauth_DFX_0105 fail " + e);
            expect(null).assertFail();
        }
    })

    /**
    * @tc.name      old challenge is not available in the new session
    * @tc.number    Security_IAM_Coauth_DFX_0106
    * @tc.size      MEDIUM
    * @tc.type      FUNC
    * @tc.level     Level2
    */
    it('Security_IAM_Coauth_DFX_0106', 3, async function (done) {
        console.info('testFace Security_IAM_Coauth_DFX_0106 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge1 = data;
                console.info('Security_IAM_Coauth_DFX_0106 publicOpenSession challenge1 = ' + challenge1);
                publicFC.publicOpenSession(UserIDM, function (data) {
                    let challenge2 = data;
                    console.info('Security_IAM_Coauth_DFX_0106 publicOpenSession challenge2 = ' + challenge2);
                    publicFC.publicCloseSession(UserIDM, function (data) {
                        console.info('Security_IAM_Coauth_DFX_0106 publicCloseSession data = ' + data);
                        publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                            let addcredresult101 = data.addCredresult;
                            console.info('Security_IAM_Coauth_DFX_0106 addCredential= ' + addcredresult101);
                            expect(ResultCode.FAIL).assertEqual(addcredresult101);
                            publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                console.info('Security_IAM_Coauth_DFX_0106 publicunRegisterInputer data = ' + data);
                                done();
                            })
                        })
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_Coauth_DFX_0106 fail " + e);
            expect(null).assertFail();
        }
    })
})