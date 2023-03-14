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
    User1: 1000,
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

    /*
        * @tc.number    : Security_IAM_PIN_AddCred_DFX_0102
        * @tc.name      : Add pin after add pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_PIN_AddCred_DFX_0102', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info('Security_IAM_PIN_AddCred_DFX_0102 publicOpenSession challenge = ' + challenge);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    let addcredresult101 = data.addCredresult;
                    console.info('Security_IAM_PIN_AddCred_DFX_0102 addcredresult101 = ' + addcredresult101);
                    expect(ResultCode.SUCCESS).assertEqual(addcredresult101);
                    publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                        let addcredresult102 = data.addCredresult;
                        expect(ResultCode.FAIL).assertEqual(addcredresult102);
                        publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                            let info101 = data;
                            console.info('Security_IAM_PIN_AddCred_DFX_0102 publicauth= ' + JSON.stringify(info101));
                            let token = info101.authextr.token;
                            publicFC.publicdelUser(UserIDM, token, function (data) {
                                let deluserresult = data.delUserresult;
                                console.info('Security_IAM_PIN_AddCred_DFX_0102 publicdelUser = ' + deluserresult);
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info('Security_IAM_PIN_AddCred_DFX_0102 publicCloseSession= ' + data);
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info('Security_IAM_PIN_AddCred_DFX_0102 unRegisterInputer= ' + data);
                                        done();
                                    })
                                })
                            })
                        }, function (data) {
                            console.info('Security_IAM_PIN_AddCred_DFX_0102 publicauth data = ' + data);
                        })
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_PIN_AddCred_DFX_0102 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_AddCred_DFX_0103
        * @tc.name      : Add pin after add pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_PIN_AddCred_DFX_0103', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info('Security_IAM_PIN_AddCred_DFX_0103 publicOpenSession challenge = ' + challenge);
                let CredentialInfopinsix2 = {
                    credType: AuthType.PIN,
                    credSubType: AuthSubType.PIN_SIX,
                }
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix2, function (data) {
                    let addcredresult101 = data.addCredresult;
                    expect(ResultCode.SUCCESS).assertEqual(addcredresult101);
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let info101 = data;
                        console.info('Security_IAM_PIN_AddCred_DFX_0103 publicauth= ' + JSON.stringify(info101));
                        let token = info101.authextr.token;
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            let deluserresult = data.delUserresult;
                            console.info('Security_IAM_PIN_AddCred_DFX_0103 publicdelUser = ' + deluserresult);
                            publicFC.publicCloseSession(UserIDM, function (data) {
                                console.info('Security_IAM_PIN_AddCred_DFX_0103 publicCloseSession data = ' + data);
                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                    console.info('Security_IAM_PIN_AddCred_DFX_0103 unRegisterInputer data= ' + data);
                                    done();
                                })
                            })
                        })
                    }, function (data) {
                        console.info('Security_IAM_PIN_AddCred_DFX_0103 publicauth data = ' + data);
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_PIN_AddCred_DFX_0103 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_Update_DFX_0101
        * @tc.name      : Update pin with no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_PIN_Update_DFX_0101', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info('Security_IAM_PIN_Update_DFX_0101 publicOpenSession challenge = ' + challenge);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    let addcredresult102 = data.addCredresult;
                    console.info('Security_IAM_PIN_Update_DFX_0101 publicaddCredential= ' + addcredresult102);
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1,
                        function (data) {
                        let addcredresult102 = data;
                        console.info('Security_IAM_PIN_Update_DFX_0101 auth= ' + JSON.stringify(addcredresult102));
                        let token = addcredresult102.authextr.token
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            let deluserresult = data.delUserresult;
                            console.info('Security_IAM_PIN_Update_DFX_0101 publicdelUser= ' + deluserresult);
                            publicFC.publicupdateCred(UserIDM, CredentialInfopinsix, function (data) {
                                let updateresult = data.updateCredresult;
                                expect(ResultCode.FAIL).assertEqual(updateresult);
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info('Security_IAM_PIN_Update_DFX_0101 publicCloseSession= ' + data);
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info('Security_IAM_PIN_Update_DFX_0101 unRegisterInputer= ' + data);
                                        done();
                                    })
                                })
                            })
                        })
                    }, function (data) {
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_PIN_Update_DFX_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_PIN_Update_DFX_0103
        * @tc.name      : Update same pin with no error
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_PIN_Update_DFX_0103', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info('Security_IAM_PIN_Update_DFX_0103 publicOpenSession challenge = ' + challenge);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    let addcredresult102 = data.addCredresult;
                    console.info('Security_IAM_PIN_Update_DFX_0103 publicaddCredential = ' + addcredresult102);
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let auth101 = data;
                        console.info('Security_IAM_PIN_Update_DFX_0103 publicauth = ' + JSON.stringify(auth101));
                        let token = auth101.authextr.token;
                        CredentialInfopinsix.token = token;
                        publicFC.publicupdateCred(UserIDM, CredentialInfopinsix, function (data) {
                            let updateresult = data.updateCredresult;
                            expect(ResultCode.SUCCESS).assertEqual(updateresult);
                            publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1,
                                function (data) {
                                auth101 = data;
                                console.info('Security_IAM_PIN_Update_DFX_0103 auth= ' + JSON.stringify(auth101));
                                let token1 = auth101.authextr.token
                                publicFC.publicdelUser(UserIDM, token1, function (data) {
                                    let deluserresult = data.delUserresult;
                                    console.info('Security_IAM_PIN_Update_DFX_0103 publicdelUser= ' + deluserresult);
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info('Security_IAM_PIN_Update_DFX_0103 CloseSession = ' + data);
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info('Security_IAM_PIN_Update_DFX_0103 unRegisterInputer='+ data);
                                            done();
                                        })
                                    })
                                })
                            })
                        })
                    }, function (data) {
                    })
                })
            })

        } catch (e) {
            console.log("Security_IAM_PIN_Update_DFX_0103 fail " + e);
            expect(null).assertFail();
        }
    })


    /*
        * @tc.number    : Security_IAM_PIN_Delet_DFX_0101
        * @tc.name      : Can't delete pin wirh no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_PIN_Delet_DFX_0101', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info('Security_IAM_PIN_Delet_DFX_0101 publicOpenSession challenge = ' + challenge);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    let addcredresult101 = data.addCredresult;
                    console.info('Security_IAM_PIN_Delet_DFX_0101 publicaddCredential= ' + addcredresult101);
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1,
                        function (data) {
                        let addcredresult101 = data;
                        console.info('Security_IAM_PIN_Delet_DFX_0101 auth='+ JSON.stringify(addcredresult101));
                        let token = addcredresult101.authextr.token
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            let deluserresult = data.delUserresult;
                            console.info('Security_IAM_PIN_Delet_DFX_0101 publicdelUser= ' + deluserresult);
                            expect(ResultCode.SUCCESS).assertEqual(deluserresult);
                            publicFC.publicdelUser(UserIDM, token, function (data) {
                                deluserresult = data.delUserresult;
                                expect(ResultCode.Authfail).assertEqual(deluserresult);
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info('Security_IAM_PIN_Delet_DFX_0101 publicCloseSession data = ' + data);
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info('Security_IAM_PIN_Delet_DFX_0101 unRegisterInputer = ' + data);
                                        done();
                                    })
                                })
                            })
                        })
                    }, function (data) {
                    })
                })
            })
        } catch (e) {
            console.log("Security_IAM_PIN_Delet_DFX_0101 fail " + e);
            expect(null).assertFail();
        }
    })
})
