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

let Inputerdata = new Uint8Array([1,2,3,4,5,6]);

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
        * @tc.number    : Security_IAM_Face_AddCred_DFX_0103
        * @tc.name      : Can't add face with no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Face_AddCred_DFX_0103', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info("Security_IAM_Face_AddCred_DFX_0103 challenge = " + challenge)
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    console.info('testFace aaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbb');
                    console.info('testFace data = ' + data);
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let info101 = data;
                        console.info("Security_IAM_Face_AddCred_DFX_0103 info101 = " + info101);
                        let token = info101.authextr.token;
                        console.info("Security_IAM_Face_AddCred_DFX_0103 token = " + token)
                        CredentialInfoface2d.token = token;
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            let deluserresult = data.delUserresult;
                            console.info("Security_IAM_Face_AddCred_DFX_0103 deluserresult = " + deluserresult)
                            publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (data) {
                                let addfaceresult = data.addCredresult
                                console.info("Security_IAM_Face_AddCred_DFX_0103 addfaceresult = " + addfaceresult);
                                console.info("Face_AddCred_DFX_0103 addfaceresult = " + typeof(addfaceresult));
                                console.info("Face_AddCred_DFX_0103 ResultCode.Authfail = " + ResultCode.Authfail);
                                console.info("Face_AddCred_DFX_0103  = " + typeof(ResultCode.Authfail));
                                expect(ResultCode.FAIL).assertEqual(addfaceresult);
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info("Security_IAM_Face_AddCred_DFX_0103 closesession = " + data)
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info("Security_IAM_Face_AddCred_DFX_0103 unRegist = " + data)
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
            })
        } catch (e) {
            console.log("Security_IAM_Face_AddCred_DFX_0103 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_AddCred_DFX_0104
        * @tc.name      : Can't add face with no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_Face_AddCred_DFX_0104', 2, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info("Security_IAM_Face_AddCred_DFX_0104 challenge = " + challenge)
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        console.info("Security_IAM_Face_AddCred_DFX_0104 info101 = " + JSON.stringify(data))
                        let token = data.authextr.token
                        CredentialInfoface2d.token = token
                        console.info("Security_IAM_Face_AddCred_DFX_0104 token = " + token)
                        publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (data) {
                            console.info("Face_AddCred_DFX_0104 addfaceresult1 = " + JSON.stringify(data))
                            expect(ResultCode.SUCCESS).assertEqual(data.addCredresult);
                            publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (data) {
                                console.info("Face_AddCred_DFX_0104 addfaceresult2 = " + JSON.stringify(data))
                                expect(ResultCode.FAIL).assertEqual(data.addCredresult);
                                publicFC.publicdelUser(UserIDM, token, function (data) {
                                    let deluserresult = data.delUserresult
                                    console.info("Security_IAM_Face_AddCred_DFX_0104 deluserresult =" + deluserresult)
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info("Security_IAM_Face_AddCred_DFX_0104 closesession = " + data)
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info("Security_IAM_Face_AddCred_DFX_0104 unRegist = " + data)
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
            console.log("Security_IAM_Face_AddCred_DFX_0104 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_Delet_DFX_0104
        * @tc.name      : Delete face with no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Face_Delet_DFX_0104', 3, async function (done) {
		console.info('testFace Security_IAM_Face_Delet_DFX_0104 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info("Security_IAM_Face_Delet_DFX_0104 challenge = " + challenge)
                publicFC.publicdelCred(UserIDM, "credentialId", "token", function (data) {
                    console.info("Security_IAM_Face_Delet_DFX_0104 info101 = " + JSON.stringify(data))
                    console.info("Security_IAM_Face_Delet_DFX_0104 delCredresult = " + data.delCredresult)
                    console.info("Security_IAM_Face_Delet_DFX_0104 ResultCode.FAIL = " + ResultCode.FAIL)
                    expect(ResultCode.Authfail).assertEqual(data.delCredresult);
                    publicFC.publicCloseSession(UserIDM, function (data) {
                        console.info("Security_IAM_Face_Delet_DFX_0104 closesession = " + data)
                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                            console.info("Security_IAM_Face_Delet_DFX_0104 unRegist = " + data)
                            done();
                        })
                    })
                }, function (data) {
                })

            })

        } catch (e) {
            console.log("Security_IAM_Face_Delet_DFX_0104 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_Delet_DFX_0105
        * @tc.name      : Delete face with no face
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Delet_DFX_0105', 1, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data
                console.info("testFace Security_IAM_Face_Delet_DFX_0105 challenge = " + challenge)
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    publicFC.publicdelCred(UserIDM, "credentialId", "token", function (data) {
                        let info102 = data
                        console.info("testFace Security_IAM_Face_Delet_DFX_0105 info102 = " + JSON.stringify(data))
                        console.info("testFace Security_IAM_Face_Delet_DFX_0105 delCredresult ="+ data.delCredresult);
                        console.info("testFace Face_Delet_DFX_0105 ResultCode.FAIL = " + ResultCode.Authfail);
                        expect(ResultCode.Authfail).assertEqual(info102.delCredresult);
                        publicFC.publicauth(UserAuth, challenge, AuthType.FACE, AuthTurstLevel.ATL1, function (data){
                            let info101 = data
                            console.info("testFace Security_IAM_Face_Delet_DFX_0105 info101 = " + JSON.stringify(data))
                            let token = info101.authextr.token
                            publicFC.publicdelUser(UserIDM, token, function (data) {
                                let deluserresult = data.delUserresult
                                console.info("testFace Face_Delet_DFX_0105 deluserresult = " + deluserresult)
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info("testFace Security_IAM_Face_Delet_DFX_0105 closesession = " + data)
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info("testFace Security_IAM_Face_Delet_DFX_0105 unRegist = " + data)
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

            })

        } catch (e) {
            console.log("Security_IAM_Face_Delet_DFX_0105 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_Auth_DFX_0101
        * @tc.name      : Auth face with no pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Face_Auth_DFX_0101', 3, async function (done) {
        console.info('testFace Security_IAM_Face_Auth_DFX_0101 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data;
                console.info("Security_IAM_Face_Auth_DFX_0101 challenge = " + challenge)
                publicFC.publicauth(UserAuth, "challenge", AuthType.FACE, AuthTurstLevel.ATL1, function (data) {
                    console.info("Security_IAM_Face_Auth_DFX_0101 info101 = " + JSON.stringify(data))
                    console.info("Security_IAM_Face_Auth_DFX_0101 delCredresult = " + data.authresult)
                    console.info("Security_IAM_Face_Auth_DFX_0101 ResultCode.FAIL = " + ResultCode.FAIL)
                    expect(ResultCode.FAIL).assertEqual(data.authresult);
                    publicFC.publicCloseSession(UserIDM, function (data) {
                        console.info("Security_IAM_Face_Auth_DFX_0101 closesession = " + data)
                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                            console.info("Security_IAM_Face_Auth_DFX_0101 unRegist = " + data)
                            done();
                        })
                    })
                }, function (data) {
                })

            })

        } catch (e) {
            console.log("Security_IAM_Face_Auth_DFX_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_Auth_DFX_0102
        * @tc.name      : Auth face with no face
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Face_Auth_DFX_0102', 2, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data
                console.info("Security_IAM_Face_Auth_DFX_0102 challenge = " + JSON.stringify(data))
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let info101 = data
                        console.info("Security_IAM_Face_Auth_DFX_0102 info101 = " + JSON.stringify(data))
                        let token = info101.authextr.token
                        CredentialInfoface2d.token = token
                        console.info("Security_IAM_Face_Auth_DFX_0102 token = " + token)
                        publicFC.publicauth(UserAuth, challenge, AuthType.FACE, AuthTurstLevel.ATL1, function (data) {
                            let info102 = data
                            console.info("Security_IAM_Face_Auth_DFX_0102 info102 = " + JSON.stringify(data))
                            console.info("Security_IAM_Face_Auth_DFX_0102 info102.authresult = " + data.authresult)
                            expect(ResultCode.FAIL).assertEqual(info102.authresult);
                            publicFC.publicdelUser(UserIDM, token, function (data) {
                                let deluserresult = data.delUserresult
                                console.info("Security_IAM_Face_Auth_DFX_0102 deluserresult = " + deluserresult)
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info("Security_IAM_Face_Auth_DFX_0102 closesession = " + data)
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info("Security_IAM_Face_Auth_DFX_0102 unRegist = " + data)
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
            })
        } catch (e) {
            console.log("Security_IAM_Face_Auth_DFX_0102 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_Auth_DFX_0104
        * @tc.name      : Auth face after delete pin
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 4
    */
    it('Security_IAM_Face_Auth_DFX_0104', 4, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge = data
                console.info("Security_IAM_Face_Auth_DFX_0104 challenge = " + challenge)
                publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                    publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let info101 = data
                        console.info("Security_IAM_Face_Auth_DFX_0104 info101 = " + JSON.stringify(data))
                        let token = info101.authextr.token
                        CredentialInfoface2d.token = token
                        console.info("Security_IAM_Face_Auth_DFX_0104 token = " + token)
                        publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (data) {
                            let addfaceresult = data
                            console.info("Security_IAM_Face_Auth_DFX_0104 addfaceresult = " + JSON.stringify(data))
                            expect(ResultCode.SUCCESS).assertEqual(addfaceresult.addCredresult);
                            publicFC.publicdelUser(UserIDM, token, function (data) {
                                let deluserresult = data.delUserresult
                                console.info("Security_IAM_Face_Auth_DFX_0104 deluserresult = " + deluserresult)
                                publicFC.publicauth(UserAuth, challenge, AuthType.FACE, AuthTurstLevel.ATL1,
                                function (data) {
                                    let info102 = data;
                                    console.info("Security_IAM_Face_Auth_DFX_0104 info102 = " + JSON.stringify(data))
                                    expect(ResultCode.FAIL).assertEqual(info102.authresult);
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info("Security_IAM_Face_Auth_DFX_0104 closesession = " + data)
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info("Security_IAM_Face_Auth_DFX_0104 unRegist = " + data)
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
            console.log("Security_IAM_Face_Auth_DFX_0104 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_AddCred_DFX_0101
        * @tc.name      : Can't add face by old token
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 4
    */
    it('Security_IAM_Face_AddCred_DFX_0101', 0, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge2;
                challenge2 = data;
                let challenge1;
                publicFC.publicOpenSession(UserIDM, function (data) {
                    console.info('Security_IAM_Face_AddCred_DFX_0101 openSession challenge = ' + JSON.stringify(data));
                    challenge1 = data;
                    publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (onresult) {
                        console.info('Face_AddCred_DFX_0101 addCredential Result1 = ' + JSON.stringify(onresult));
                        let info101;
                        let info102;
                        publicFC.publicauth(UserAuth, challenge2, AuthType.PIN, AuthTurstLevel.ATL1, function (data){
                            console.info('Security_IAM_Face_AddCred_DFX_0101 auth= ' + JSON.stringify(data));
                            info102 = data;
                            let token1 = info102.authextr.token;
                            CredentialInfoface2d.token = token1;
                            publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
                            function (data) {
                                console.info('Security_IAM_Face_AddCred_DFX_0101 auth = ' + JSON.stringify(data));
                                info101 = data;
                                let token2 = info101.authextr.token;
                                let addfaceresult;
                                publicFC.publicaddCredential(UserIDM, CredentialInfoface2d, function (onresult) {
                                    console.info('Face_AddCred_DFX_0101 addCredential=' + JSON.stringify(onresult));
                                    console.info('Face_AddCred_DFX_0101 addCredential=' + onresult.addCredresult);
                                    console.info('Face_AddCred_DFX_0101 ResultCode.FAIL=' + ResultCode.FAIL);
                                    addfaceresult = onresult;
                                    expect(ResultCode.FAIL).assertEqual(addfaceresult.addCredresult);
                                    publicFC.publicdelUser(UserIDM, token2, function (data) {
                                        console.info('Face_AddCred_DFX_0101 delUser= ' + JSON.stringify(data));
                                        publicFC.publicCloseSession(UserIDM, function (data) {
                                            console.info('Security_IAM_Face_AddCred_DFX_0101 closeSession');
                                            publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                                console.info('Security_IAM_Face_AddCred_DFX_0101 unRegister');
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
            console.log("Security_IAM_Face_AddCred_DFX_0101 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Face_AddCred_DFX_0101
        * @tc.name      : Can't delete face by wrong token
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_Face_Delet_DFX_0101', 2, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('Security_IAM_Face_Delet_DFX_0101 openSession challenge = ' + JSON.stringify(data));
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                    console.info('Face_Delet_DFX_0101 addCredential Result1 = ' + JSON.stringify(onresult));
                    let info101;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                        console.info('Security_IAM_Face_Delet_DFX_0101 auth onResult = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                            console.info('Security_IAM_Face_Delet_DFX_0101 addCredential='+ JSON.stringify(onresult));
                            addfaceresult = onresult;
                                let credentialId = addfaceresult.credentialId;
                                let delcredresult ;
                                let wrongtoken = token + 'wrong';
                                publicFC.publicdelCred(UserIDM,credentialId,wrongtoken, function (data) {
                                    console.info('testFace Face_Delet_DFX_0101 del=' + JSON.stringify(data));
                                    console.info('testFace Face_Delet_DFX_0101 ResultCode.FAIL=' + ResultCode.FAIL);
                                    delcredresult = data;
                                    expect(ResultCode.Authfail).assertEqual(delcredresult.delCredresult);
                                    publicFC.publicdelUser(UserIDM,token, function (data) {
                                        console.info('Face_Delet_DFX_0101 delUser= ' + JSON.stringify(data));
                                        publicFC.publicCloseSession(UserIDM, function (data) {
                                            console.info('Security_IAM_Face_Delet_DFX_0101 closeSession');
                                            publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                                console.info('Security_IAM_Face_Delet_DFX_0101 unRegister');
                                                done();
                                            })
                                        })
                                    }, function (data) {
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
})
