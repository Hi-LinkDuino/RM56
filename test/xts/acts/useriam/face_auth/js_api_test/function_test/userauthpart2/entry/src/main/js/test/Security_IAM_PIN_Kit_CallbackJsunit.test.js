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
import userAuthNorth from '@ohos.userIAM.userAuth'
import * as publicFC from './Publicfunction-n'


let UserIDM = new userIDM.UserIdentityManager();
let PinAuth = new pinAuth.PINAuth();
let UserAuth = new userAuth.UserAuth();
let UserAuthKit = new userAuthNorth.UserAuth()

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
    User1: 100,
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
let Inputerdatan = new Uint8Array([1, 1, 1, 1, 1, 1]);
let Inputerdatanum = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8]);
let Inputerdatanumn = new Uint8Array([1, 1, 1, 1, 1, 1, 1, 1]);
let Inputerdatamix = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8, 9]);
let Inputerdatamixn = new Uint8Array([1, 1, 1, 1, 1, 1, 1, 1, 1]);

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

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0101
        * @tc.name      : Kit interface get version
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0101', 0, async function (done) {
        try {
            let getversionresult = null;
            getversionresult = UserAuthKit.getVersion();
            console.info('GetVersionTest_0101 getversionresult = ' + getversionresult);
            expect(false).assertEqual(getversionresult == null);
            done();
        } catch (e) {
            console.log("testFace GetVersionTest_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0102
        * @tc.name      : Kit interface get AvailabeStatus
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0102', 0, async function (done) {
        console.info('testFace GetAvailabeStatusTest0101 start');
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            publicFC.publicOpenSession(UserIDM,function(data){
                let challenge = data
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                    function(onresult) {
                        console.info('testFace GetAvailabeStatusTest0101 addCredresult=' + onresult.addCredresult);
                        console.log("testFace GetAvailabeStatusTest0101 authUser result = " + onresult.addCredresult);
                        let Available;
						Available = publicFC.publicgetAvailabeStatus(UserAuthKit,AuthType.PIN,AuthTurstLevel.ATL1)
                        console.info('testFace GetAvailabeStatusTest0101 AvailabeStatus1 = ' + Available);
                        console.info('testFace GetAvailabeStatusTest0101 ResultCode.SUCCESS = ' + ResultCode.SUCCESS);
                        expect(ResultCode.SUCCESS).assertEqual(Available);
                        Available = publicFC.publicgetAvailabeStatus(UserAuthKit,AuthType.PIN,AuthTurstLevel.ATL2)
                        console.info('testFace GetAvailabeStatusTest0101 AvailabeStatus2 = ' + Available);
                        console.info('testFace GetAvailabeStatusTest0101 ResultCode.SUCCESS = ' + ResultCode.SUCCESS);
                        expect(ResultCode.SUCCESS).assertEqual(Available);
                        Available = publicFC.publicgetAvailabeStatus(UserAuthKit,AuthType.PIN,AuthTurstLevel.ATL3)
                        console.info('testFace GetAvailabeStatusTest0101 AvailabeStatus3 = ' + Available);
                        console.info('testFace GetAvailabeStatusTest0101 ResultCode.SUCCESS = ' + ResultCode.SUCCESS);
                        expect(ResultCode.SUCCESS).assertEqual(Available);
                        Available = publicFC.publicgetAvailabeStatus(UserAuthKit,AuthType.PIN,AuthTurstLevel.ATL4)
                        console.info('testFace GetAvailabeStatusTest0101 AvailabeStatus4 = ' + Available);
                        console.info('testFace GetAvailabeStatusTest0101 ResultCode.SUCCESS = ' + ResultCode.SUCCESS);
                        expect(ResultCode.SUCCESS).assertEqual(Available);
                        publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                            function(onresult) {
                                console.info('testFace GetAvailabeStatusTest0101 publicauth = ' + onresult.authresult);
                                let token = onresult.authextr.token
                                publicFC.publicdelUser(UserIDM,token,function(onresult){
                                    console.log("testFace GetAvailabeStatusTest0101 delUser="+ onresult.delUserresult)
                                    publicFC.publicCloseSession(UserIDM,function(data){
                                        console.log("testFace GetAvailabeStatusTest0101 CloseSession= " + data)
                                        publicFC.publicunRegisterInputer(PinAuth,function(data){
                                            done();
                                        })
                                    })
                                },function(onacquireinfo){
                                })
                            }, function(onAcquireInfo) {
                            }
                        );
                    }, function(onAcquireInfo) {
                    });
            })
        } catch (e) {
            console.log("GetAvailabeStatusTest0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0103
        * @tc.name      : Kit interface auth and cancel auth
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
it('Security_IAM_PIN_Kit_Func_0103', 0, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('AuthTest_0101 openSession challenge = ' + data);
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                    console.info('AuthTest_0101 addCredential Result1 = ' + JSON.stringify(onresult));
                    let info101;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                        console.info('AuthTest_0101 auth onResult = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                            console.info('AuthTest_0101 addCredential Result2=' + JSON.stringify(onresult));
                            addfaceresult = onresult;
                            console.info('AuthTest_0101 publicaddCredential = ' + addfaceresult.addCredresult);
                            expect(ResultCode.SUCCESS).assertEqual(addfaceresult.addCredresult);
                            publicFC.publicauth(UserAuthKit,challenge,AuthType.FACE,10000, async function (data) {
                                let faceauth101 = data;
                                console.info('AuthTest_0101 publicauth = ' + faceauth101.authresult);
                                expect(ResultCode.SUCCESS).assertEqual(faceauth101.authresult);
                                let authresult;
                                let contextID = await publicFC.publicauth(UserAuthKit,challenge,AuthType.FACE,10000,
                                function (data) {
                                console.info('AuthTest_0101 addCredential Result2=' + JSON.stringify(data.authresult));
                                authresult = data.authresult;
                                }, function (onacquireinfo) {
                                })
                                let cancelresult = publicFC.publicgecancelAuth(UserAuthKit,contextID);
                                await sleep(2000);
                                if(cancelresult == 1){
                                    console.info('AuthTest_0101 cancel = 1  authresult = ' + authresult);
                                    expect(ResultCode.SUCCESS).assertEqual(authresult);
                                }else if(cancelresult == 0){
                                    console.info('AuthTest_0101 cancel = 0  authresult = ' + authresult);
                                    console.info('AuthTest_0101 cancel = 0  ResultCode.FAIL = ' + ResultCode.FAIL);
                                    expect(ResultCode.CANCELED).assertEqual(authresult);
                                }
                                console.info('testFace AuthTest_0101 onResult = ' + JSON.stringify(data));
                                await publicFC.publicdelUser(UserIDM,token, function (data) {
                                    console.info('AuthTest_0101 delUser = ' + JSON.stringify(data));
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info('AuthTest_0101 closeSession');
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info('AuthTest_0101 unRegister');
                                            done();
                                        })
                                    })
                                }, function (onacquireinfo) {
                                })
                            }, function (onacquireinfo) {
                            })
                        }, function (onacquireinfo) {
                        })
                    }, function (data) {
                    })
                }, function (onacquireinfo) {
                })
            })
        } catch (e) {
            console.log("AuthTest_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0104
        * @tc.name      : Kit interface enum AuthTrustLevel
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0104', 0, async function (done) {
        console.info('testFace Security_IAM_PIN_Kit_Func_0104 start');
        expect(10000).assertEqual(userAuthNorth.AuthTrustLevel.ATL1);
        expect(20000).assertEqual(userAuthNorth.AuthTrustLevel.ATL2);
        expect(30000).assertEqual(userAuthNorth.AuthTrustLevel.ATL3);
        expect(40000).assertEqual(userAuthNorth.AuthTrustLevel.ATL4);
        console.info('testFace Security_IAM_PIN_Kit_Func_0104 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0105
        * @tc.name      : Kit interface enum FaceTips
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0105', 0, async function (done) {
        console.info('testFace Security_IAM_PIN_Kit_Func_0105 start');
        expect(1).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_BRIGHT);
        expect(2).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_DARK);
        expect(3).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_CLOSE);
        expect(4).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_FAR);
        expect(5).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_HIGH);
        expect(6).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_LOW);
        expect(7).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_RIGHT);
        expect(8).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_LEFT);
        expect(9).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_TOO_MUCH_MOTION);
        expect(10).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_POOR_GAZE);
        expect(11).assertEqual(userAuthNorth.FaceTips.FACE_AUTH_TIP_NOT_DETECTED);
        console.info('testFace Security_IAM_PIN_Kit_Func_0105 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0106
        * @tc.name      : Kit interface enum UserAuthType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0106', 0, async function (done) {
        console.info('testFace Security_IAM_PIN_Kit_Func_0106 start');
        expect(2).assertEqual(userAuthNorth.UserAuthType.FACE);
        expect(4).assertEqual(userAuthNorth.UserAuthType.FINGERPRINT);
        console.info('testFace Security_IAM_PIN_Kit_Func_0106 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0107
        * @tc.name      : Kit interface enum ResultCode
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0107', 0, async function (done) {
        console.info('testFace Security_IAM_PIN_Kit_Func_0107 start');
        expect(0).assertEqual(userAuthNorth.ResultCode.SUCCESS);
        expect(1).assertEqual(userAuthNorth.ResultCode.FAIL);
        expect(2).assertEqual(userAuthNorth.ResultCode.GENERAL_ERROR);
        expect(3).assertEqual(userAuthNorth.ResultCode.CANCELED);
        expect(4).assertEqual(userAuthNorth.ResultCode.TIMEOUT);
        expect(5).assertEqual(userAuthNorth.ResultCode.TYPE_NOT_SUPPORT);
        expect(6).assertEqual(userAuthNorth.ResultCode.TRUST_LEVEL_NOT_SUPPORT);
        expect(7).assertEqual(userAuthNorth.ResultCode.BUSY);
        expect(8).assertEqual(userAuthNorth.ResultCode.INVALID_PARAMETERS);
        expect(9).assertEqual(userAuthNorth.ResultCode.LOCKED);
        expect(10).assertEqual(userAuthNorth.ResultCode.NOT_ENROLLED);
        console.info('testFace Security_IAM_PIN_Kit_Func_0107 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_PIN_Kit_Func_0108
        * @tc.name      : Kit interface enum FingerprintTips
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_PIN_Kit_Func_0108', 0, async function (done) {
        console.info('testFace Security_IAM_PIN_Kit_Func_0108 start');
        expect(0).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_GOOD);
        expect(1).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_DIRTY);
        expect(2).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_INSUFFICIENT);
        expect(3).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_PARTIAL);
        expect(4).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_TOO_FAST);
        expect(5).assertEqual(userAuthNorth.FingerprintTips.FINGERPRINT_AUTH_TIP_TOO_SLOW);
        console.info('testFace Security_IAM_PIN_Kit_Func_0108 end');
        done();
    })

})

