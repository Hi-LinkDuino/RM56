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
import * as publicFC from './Publicfunction-n.js'

let UserIDM = new userIDM.UserIdentityManager();
let PinAuth = new pinAuth.PINAuth();
let UserAuth = new userAuth.UserAuth();

let AuthType = {
    PIN : 1,
    FACE : 2
}
let AuthSubType = {
    PIN_SIX : 10000,
    PIN_NUMBER : 10001,
    PIN_MIXED : 10002,
    FACE_2D : 20000,
    FACE_3D : 20001
}
let AuthTurstLevel = {
    ATL1 : 10000,
    ATL2 : 20000,
    ATL3 : 30000,
    ATL4 : 40000
}

let SetPropertyType = {
    PROCESS_ALGORITHM : 1,
}

let userID = {
    User1 : 100,
    User2 : 2,
    User3 : 3,
    User4 : 4,
    User5 : 5
}

let ResultCode = {
    SUCCESS : 0,
    FAIL : 1,
    GENERAL_ERROR : 2,
    CANCELED : 3,
    TIMEOUT : 4,
    TYPE_NOT_SUPPORT : 5,
    TRUST_LEVEL_NOT_SUPPORT : 6,
    BUSY : 7,
    INVALID_PARAMETERS : 8,
    LOCKED : 9,
    NOT_ENROLLED : 10,
    Authfail: 14
}

let GetPropertyType = {
    AUTH_SUB_TYPE : 1,
    REMAIN_TIMES : 2,
    FREEZING_TIME : 3
}

let Inputerdata = new Uint8Array([1,2,3,4,5,6]);

let GetPropertyTypearray=new Array();
GetPropertyTypearray[0]=GetPropertyType.AUTH_SUB_TYPE;      
GetPropertyTypearray[1]=GetPropertyType.FREEZING_TIME;
GetPropertyTypearray[2]=GetPropertyType.REMAIN_TIMES;

let GetPropertyRequestpin = {
    authType:AuthType.PIN,
    keys:GetPropertyTypearray
    }

let GetPropertyRequestface = {
    authType:AuthType.FACE,
    keys:GetPropertyTypearray
    }

let CredentialInfopinsix = {
    credType: AuthType.PIN,
    credSubType: AuthSubType.PIN_SIX,
    token:new Uint8Array([1,2,3,4,5,6])
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
        * @tc.number    : Security_IAM_Face_AddCred_DFX_0101
        * @tc.name      : Test add face, auth face, authuser face, delete face
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_AddCred_Func_0101', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('Security_IAM_Face_AddCred_Func_0101 openSession challenge = ' + data);
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                    console.info('Face_AddCred_Func_0101 addCredential Result1 = ' + JSON.stringify(onresult));
                    let info101;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                        console.info('Security_IAM_Face_AddCred_Func_0101 auth onResult = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                            console.info('Face_AddCred_Func_0101 addCredential Result2=' + JSON.stringify(onresult));
                            addfaceresult = onresult;
                            expect(ResultCode.SUCCESS).assertEqual(addfaceresult.addCredresult);
                            publicFC.publicauth(UserAuth,challenge,AuthType.FACE,AuthTurstLevel.ATL1, function (data) {
                                console.info('testFace Face_AddCred_Func_0101 onResult = ' + JSON.stringify(data));
                                let faceauth101 = data;
                                expect(ResultCode.SUCCESS).assertEqual(faceauth101.authresult);
                                publicFC.publicauthUser(
                                    UserAuth,userID.User1,challenge,AuthType.FACE,AuthTurstLevel.ATL1,function (data) {
                                    console.info('testFace Face_AddCred_Func_0101 onResult= ' + JSON.stringify(data));
                                    faceauth101 = data;
                                    expect(ResultCode.SUCCESS).assertEqual(faceauth101.authresult);
                                    let credentialId = addfaceresult.credentialId;
                                    let delcredresult ;
                                    publicFC.publicdelCred(UserIDM,credentialId,token, function (data) {
                                        console.info('testFace Face_AddCred_Func_0101 del=' + JSON.stringify(data));
                                        delcredresult = data;
                                        expect(ResultCode.SUCCESS).assertEqual(delcredresult.delCredresult);
                                        publicFC.publicdelUser(UserIDM,token, function (data) {
                                            console.info('Face_AddCred_Func_0101 delUser= ' + JSON.stringify(data));
                                            publicFC.publicCloseSession(UserIDM, function (data) {
                                                console.info('Security_IAM_Face_AddCred_Func_0101 closeSession');
                                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                                    console.info('Security_IAM_Face_AddCred_Func_0101 unRegister');
                                                    done();
                                                })
                                            })
                                        }, function (data) {
                                        })
                                    }, function (onacquireinfo) {
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
            console.log("Security_IAM_Face_AddCred_Func_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_AddCred_Func_0103
        * @tc.name      : Cancel add face
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_AddCred_Func_0103', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('Security_IAM_Face_AddCred_Func_0103 openSession challenge = ' + data);
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                    console.info('Face_AddCred_Func_0103 addCredential Result1 = ' + JSON.stringify(onresult));
                    let info101;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, async function (data) {
                        console.info('Security_IAM_Face_AddCred_Func_0103 auth onResult = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                            console.info('Face_AddCred_Func_0103 addCredential Result2=' + JSON.stringify(onresult));
                            addfaceresult = onresult;
                        }, function (onacquireinfo) {
                        })
                        let cancelresult = publicFC.publiccancel(UserIDM,challenge);
                        await sleep(2500);
                        if(cancelresult == 1){
                            console.info('Face_AddCred_Func_0103 cancel1 authresult = ' + addfaceresult.addCredresult);
                            expect(ResultCode.SUCCESS).assertEqual(addfaceresult.addCredresult);
                        }else if(cancelresult == 0){
                            console.info('Face_AddCred_Func_0103 cancel0 authresult = ' + addfaceresult.addCredresult);
                            expect(ResultCode.CANCELED).assertEqual(addfaceresult.addCredresult);
                        }
                        await publicFC.publicdelUser(UserIDM,token, function (data) {
                            console.info('Face_AddCred_Func_0103 delUser= ' + JSON.stringify(data));
                            publicFC.publicCloseSession(UserIDM, function (data) {
                                console.info('Security_IAM_Face_AddCred_Func_0103 closeSession');
                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                    console.info('Security_IAM_Face_AddCred_Func_0103 unRegister');
                                    done();
                                })
                            })
                        }, function (data) {
                        })
                    }, function (data) {
                    })
                }, function (onacquireinfo) {
                })
            })
        } catch (e) {
            console.log("Security_IAM_Face_AddCred_Func_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_AddCred_Func_0102
        * @tc.name      : Can't add face by wrong token
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Face_AddCred_Func_0102', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata);
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('testFace Security_IAM_Face_AddCred_Func_0102 publicOpenSession challenge = ' + data);
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (data) {
                    console.info('testFace Face_AddCred_Func_0102 addCredential= ' + JSON.stringify(data));
                    let info101 ;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                        console.info('testFace Face_AddCred_Func_0102 publicauth = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult ;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (data) {
                            console.info('testFace Face_AddCred_Func_0102 addCredential=' + JSON.stringify(data));
                            addfaceresult = data;
                            let credentialId = addfaceresult.credentialId;
                            let token1 = token + "Wrong Word";
                            let delcredresult ;
                            publicFC.publicdelCred(UserIDM,credentialId,token1, function (data) {
                                console.info('testFace Face_AddCred_Func_0102 publicdelCred=' + JSON.stringify(data));
                                delcredresult = data;
                                expect(ResultCode.Authfail).assertEqual(delcredresult.delCredresult);
                                publicFC.publicdelUser(UserIDM,token, function (data) {
                                    console.info('testFace Face_AddCred_Func_0102 delUser= ' + JSON.stringify(data));
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info('testFace Face_AddCred_Func_0102 publicCloseSession ');
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info('testFace Face_AddCred_Func_0102 publicunRegister ');
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
        } catch (e) {
            console.log("testdelface102 fail " + e);
            expect(null).assertFail();
        }
    })
})
