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

function inputererror(inputer,done){
    try {
        let registerresult = publicFC.publicRegisterInputer(PinAuth, inputer, inputer)
        expect(true).assertEqual(registerresult);
        publicFC.publicunRegisterInputer(PinAuth, function (data) {
            console.info("testFace inputererror publicunRegisterInputer unRegist = " + data)
        })
        done();
    } catch (e) {
        console.log("testFace inputererror fail " + e);
        expect(null).assertFail();
    }
}

function addCredentialerr(inputer,done){
    console.info('testFace addCredentialerr start');
    try {
        let challenge;
        publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata);
        publicFC.publicOpenSession(UserIDM, function (data) {
            console.info('testFace addCredentialerr publicOpenSession data = ' + data);
            challenge = data;
            publicFC.publicaddCredential(UserIDM, inputer, function (data) {
                console.info('testFace addCredentialerr callback data = ' + JSON.stringify(data));
                let addcredresult101 = data.addCredresult;
                console.info("testFace addCredentialerr addcredresult101 = " + addcredresult101);
                expect(ResultCode.FAIL).assertEqual(addcredresult101);
                publicFC.publicCloseSession(UserIDM, function (data) {
                    console.info("testFace addCredentialerr publicCloseSession = " + data);
                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                        console.info("testFace  addCredentialerr unRegister = " + data);
                        done();
                    })
                })
            }, function (data) {
            })
        })
    } catch (e) {
        console.log("addCredentialerr fail " + e);
        expect(null).assertFail();
    }
}

function autherror(inputer,done){
    console.info('testFace autherror start');
    try {
        let challenge;
        publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_NUMBER, Inputerdatanum);
        publicFC.publicOpenSession(UserIDM, function (data) {
            console.info('testFace autherror publicOpenSession data = ' + data);
            challenge = data;
            publicFC.publicaddCredential(UserIDM, CredentialInfopinnum, function (data) {
                console.info('testFace autherror callback data = ' + JSON.stringify(data));
                let token;
                publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                    console.info('testFace autherror publicauth');
                    token = data.authextr.token;
                    expect(ResultCode.SUCCESS).assertEqual(data.authresult);
                    console.info("testFace autherror token = " + token);
                    publicFC.publicauth(UserAuth, challenge, inputer, inputer, function (data) {
                        expect(ResultCode.TRUST_LEVEL_NOT_SUPPORT).assertEqual(data.authresult);
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            console.info("testFace autherror publicdelUser");
                            let deluserresult = data.delUserresult;
                            console.info("testFace autherror deluserresult = " + deluserresult);
                            expect(ResultCode.SUCCESS).assertEqual(deluserresult);
                            publicFC.publicCloseSession(UserIDM, function (data) {
                                console.info("testFace autherror publicCloseSession = " + data);
                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                    console.info("testFace  autherror unRegister = " + data);
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
        console.log("autherror fail " + e);
        expect(null).assertFail();
    }
}

function authusererror(inputer,done){
    console.info('testFace authusererror start');
    try {
        let challenge;
        publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_NUMBER, Inputerdatanum);
        publicFC.publicOpenSession(UserIDM, function (data) {
            console.info('testFace authusererror publicOpenSession data = ' + data);
            challenge = data;
            publicFC.publicaddCredential(UserIDM, CredentialInfopinnum, function (data) {
                console.info('testFace authusererror callback data = ' + JSON.stringify(data));
                let addcredresult101 = data.addCredresult;
                console.info("testFace authusererror addcredresult101 = " + addcredresult101);
                let token;
                console.info('testFace authusererror addSuccess');
                publicFC.publicauthUser(UserAuth, userID.User1, challenge, inputer, inputer, function (data) {
                    let auth101 = data.authresult
                    console.info("testFace authusererror auth101 = " + auth101);
                    if(auth101==ResultCode.FAIL || auth101==ResultCode.TRUST_LEVEL_NOT_SUPPORT){
                        console.log("authusererror pass result code = " + auth101);
                    }else{
                        console.log("authusererror fail result code = " + auth101);
                        expect(null).assertFail();
                    }
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,function (data) {
                        console.info('testFace authusererror publicauth');
                        token = data.authextr.token;
                        console.info("testFace authusererror token = " + token);
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            console.info("testFace authusererror publicdelUser");
                            let deluserresult = data.delUserresult;
                            console.info("testFace authusererror deluserresult = " + deluserresult);
                            publicFC.publicCloseSession(UserIDM, function (data) {
                                console.info("testFace authusererror publicCloseSession = " + data);
                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                    console.info("testFace  authusererror unRegister= " + data);
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
        console.log("authusererror fail " + e);
        expect(null).assertFail();
    }
}

function delusererror(inputer,done){
    console.info('testFace delusererror start');
    try {
        let challenge;
        publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_NUMBER, Inputerdatanum);
        publicFC.publicOpenSession(UserIDM, function (data) {
            console.info('testFace delusererror publicOpenSession data = ' + data);
            challenge = data;
            publicFC.publicaddCredential(UserIDM, CredentialInfopinnum, function (data) {
                console.info('testFace delusererror callback data = ' + JSON.stringify(data));
                let addcredresult101 = data.addCredresult;
                console.info("testFace delusererror addcredresult101 = " + addcredresult101);
                let token;
                console.info('testFace delusererror addSuccess');
                publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                    console.info('testFace delusererror publicauth');
                    token = data.authextr.token;
                    console.info("testFace delusererror token = " + token);
                    publicFC.publicdelUser(UserIDM, inputer, function (data) {
                        console.info("testFace delusererror delUserresult1 = " + data.delUserresult);
                        publicFC.publicdelUser(UserIDM, token, function (data) {
                            console.info("testFace delusererror publicdelUser");
                            let deluserresult = data.delUserresult;
                            console.info("testFace delusererror deluserresult2 = " + deluserresult);
                            publicFC.publicCloseSession(UserIDM, function (data) {
                                console.info("testFace delusererror publicCloseSession = " + data);
                                publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                    console.info("testFace  delusererror unRegister = " + data);
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
        console.log("delusererror fail " + e);
        expect(null).assertFail();
    }
}

function updateerror(inputer,done){
    console.info('testFace updateerror start');
    try {
        publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
        publicFC.publicOpenSession(UserIDM, function (data) {
            let challenge = data;
            console.info("updateerror challenge = " + challenge);
            publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                let addcredresult101 = data.addcredresult;
                console.info("updateerror addcredresult101 = " + addcredresult101);
                publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                    let auth101 = data
                    console.info("updateerror auth101 = " + auth101);
                    let token = auth101.authextr.token;
                    CredentialInfopinnum.token = null;
                    CredentialInfopinnum.credType = null;
                    CredentialInfopinnum.credSubType = null;
                    console.info("updateerror token = " + token);
                    publicFC.publicupdateCred(UserIDM, CredentialInfopinnum, function (data) {
                        let updateresult = data.updateCredresult;
                        console.info("updateerror updateresult = " + updateresult);
                        expect(ResultCode.FAIL).assertEqual(updateresult);
                        publicFC.publicauth(UserAuth, challenge, AuthType.PIN, AuthTurstLevel.ATL1,
                            function (data) {
                                auth101 = data
                                console.info("updateerror auth101 = " + auth101);
                                let token1 = auth101.authextr.token
                                console.info("updateerror token1 = " + token1);
                                publicFC.publicdelUser(UserIDM, token1, function (data) {
                                    console.info("updateerror delUser result  = " + data.delUserresult);
                                    publicFC.publicCloseSession(UserIDM, function (data) {
                                        console.info("updateerror closesession = " + data);
                                        publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                            console.info("updateerror unRegist = " + data);
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
        console.log("updateerror fail " + e);
        expect(null).assertFail();
    }
}

function delcrederror(inputer,done){
    try {
        publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
        let challenge ;
        publicFC.publicOpenSession(UserIDM, function (data) {
            console.info('delcrederror openSession challenge = ' + data);
            challenge = data;
            publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                console.info('delcrederror addCredential Result1 = ' + JSON.stringify(onresult));
                let info101;
                publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                    console.info('delcrederror auth onResult = ' + JSON.stringify(data));
                    info101 = data;
                    let token = info101.authextr.token;
                    CredentialInfoface2d.token = token;
                    let addfaceresult;
                    publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                        console.info('delcrederror addCredential Result2=' + JSON.stringify(onresult));
                        addfaceresult = onresult;
                        let delcredresult ;
                        publicFC.publicdelCred(UserIDM, inputer, inputer, function (data) {
                            console.info('testFace delcrederror del=' + JSON.stringify(data));
                            delcredresult = data;
                            expect(ResultCode.Authfail).assertEqual(delcredresult.delCredresult);
                            publicFC.publicdelUser(UserIDM,token, function (data) {
                                console.info('delcrederror delUser= ' + JSON.stringify(data));
                                publicFC.publicCloseSession(UserIDM, function (data) {
                                    console.info('delcrederror closeSession');
                                    publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                        console.info('delcrederror unRegister');
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
        console.log("delcrederror fail " + e);
        expect(null).assertFail();
    }
}

function getauthinfoerror(inputer,done){
    try {
        publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
        publicFC.publicOpenSession(UserIDM,function(data){
            let challenge = data
            publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                function(onresult) {
                    console.info('testFace getauthinfoerror addCredresult = ' + onresult.addCredresult);
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                    function(onresult) {
                        console.info(
                            'testFace getauthinfoerror addCred onResult = ' + JSON.stringify(onresult));
                        let token = onresult.authextr.token
                        CredentialInfoface2d.token = token
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                            console.log("testFace getauthinfoerror addface= " + onresult.addCredresult)
                            publicFC.publicgetAuthInfo(UserIDM,inputer,function(AsyncCallback) {
                                console.log("testFace getauthinfoerror getAuthInfo= " +
                                JSON.stringify(AsyncCallback))
                                publicFC.publicdelUser(UserIDM,token,function(onresult){
                                    let delresult = onresult.delUserresult;
                                    console.log("testFace getauthinfoerror delUser= " + delresult)
                                    publicFC.publicCloseSession(UserIDM,function(data){
                                        console.log("getauthinfoerror CloseSession= " + data)
                                        publicFC.publicunRegisterInputer(PinAuth,function(data){
                                            done();
                                        })
                                    })
                                },function(onacquireinfo){
                                })
                            })
                        }, function(onAcquireInfo) {
                        })
                    }, function(onAcquireInfo) {
                    });
                }, function(onAcquireInfo) {
                });
        })
    } catch (e) {
        console.log("getauthinfoerror fail " + e);
        expect(null).assertFail();
    }
}

function getAvailabeStatuserror(inputer,done){
    try {
        publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
        publicFC.publicOpenSession(UserIDM,function(data){
            let challenge = data
            publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                function(onresult) {
                    console.info('testFace getAvailabeStatuserror addCredresult = ' + onresult.addCredresult);
                    console.log("testFace faceDemo getAvailabeStatuserror authUser = " + onresult.addCredresult);
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                        function(onresult) {
                            console.info('testFace getAvailabeStatuserror addCred = ' + JSON.stringify(onresult));
                            let token = onresult.authextr.token
                            CredentialInfoface2d.token = token
                            publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                                console.info('testFace getAvailabeStatuserror addface=' + onresult.authresult);
                                console.log("testFace getAvailabeStatuserror addface result="+ onresult.addCredresult)
                                let AvailabeStatus = publicFC.publicgetAvailabeStatus(
                                    UserAuth,inputer,inputer)
                                console.info('testFace getAvailabeStatuserror addCAvailabeStatusred = '
                                + JSON.stringify(AvailabeStatus));
                                expect(ResultCode.INVALID_PARAMETERS).assertEqual(AvailabeStatus);
                                publicFC.publicdelUser(UserIDM,token,function(onresult){
                                    console.log("testFace getAvailabeStatuserror delUser="+ onresult.delUserresult)
                                    publicFC.publicCloseSession(UserIDM,function(data){
                                        console.log("testFace getAvailabeStatuserror CloseSession = " + data)
                                        publicFC.publicunRegisterInputer(PinAuth, function(data){
                                            done();
                                        })
                                    })
                                },function(onacquireinfo){
                                })
                            }, function(onAcquireInfo) {
                            })
                        }, function(onAcquireInfo) {
                        }
                    );
                }, function(onAcquireInfo) {
                });
        })
    } catch (e) {
        console.log("getAvailabeStatuserror fail " + e);
        expect(null).assertFail();
    }
}

function getPropertyerror(inputer,done){
    try {
        publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
        publicFC.publicOpenSession(UserIDM,function(data){
            let challenge = data
            publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function(onresult) {
                console.info('testFace Coauth_Func_0110 addCredresult = ' + onresult.addCredresult);
                publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function(onresult) {
                    console.info('testFace Coauth_Func_0110 addCred onResult = ' + JSON.stringify(onresult));
                    let token = onresult.authextr.token
                    CredentialInfoface2d.token = token
                    publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                        console.log("testFace Coauth_Func_0110 ddCredentialface = " + onresult.addCredresult)
                        GetPropertyRequestface.authType = inputer;
                        GetPropertyRequestface.keys = inputer;
                        publicFC.publicgetProperty(UserAuth,GetPropertyRequestface,function(onresult) {
                            console.log("testFace Coauth_Func_0110 GetPropertyface = " + JSON.stringify(onresult))
                            expect(ResultCode.FAIL).assertEqual(onresult.result);
                            publicFC.publicdelUser(UserIDM,token,function(onresult){
                                console.log("testFace Coauth_Func_0110 delUser= " + onresult.delUserresult)
                                publicFC.publicCloseSession(UserIDM,function(data){
                                    console.log("testFace Coauth_Func_0110 CloseSession callback = " + data)
                                    publicFC.publicunRegisterInputer(PinAuth, function(data){
                                        done();
                                    })
                                })
                            },function(onacquireinfo){
                            })
                        })
                    }, function(onAcquireInfo) {
                    })
                }, function(onAcquireInfo) {
                })
            }, function(onAcquireInfo) {
            })
        })
    } catch (e) {
        console.log("Security_IAM_Coauth_Func_0110 fail " + e);
        expect(null).assertFail();
    }
}

describe('userauthTest', function () {

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0101
        * @tc.name      : Test regrsterInputer interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0101', 2, async function (done) {
        inputererror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0102
        * @tc.name      : Test regrsterInputer interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0102', 2, async function (done) {
        inputererror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0103
        * @tc.name      : Test addCredential interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0103', 2, async function (done) {
        addCredentialerr(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0104
        * @tc.name      : Test addCredential interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0104', 2, async function (done) {
        addCredentialerr('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0105
        * @tc.name      : Test auth interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0105', 2, async function (done) {
        autherror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0106
        * @tc.name      : Test auth interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0106', 2, async function (done) {
        autherror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0107
        * @tc.name      : Test authuser interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0107', 2, async function (done) {
        authusererror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0108
        * @tc.name      : Test authuser interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0108', 2, async function (done) {
        authusererror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0109
        * @tc.name      : Test getProperty interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0109', 2, async function (done) {
        getPropertyerror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0110
        * @tc.name      : Test getProperty interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0110', 2, async function (done) {
        getPropertyerror('Errortest',done);
    })
	
    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0111
        * @tc.name      : Test updateCredential interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */	
    it('Security_IAM_PIN_Interface_DFX_0111', 2, async function (done) {
        updateerror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0112
        * @tc.name      : Test updateCredential interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0112', 2, async function (done) {
        updateerror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0113
        * @tc.name      : Test deluser interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */	
    it('Security_IAM_PIN_Interface_DFX_0113', 2, async function (done) {
        delusererror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0114
        * @tc.name      : Test deluser interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0114', 2, async function (done) {
        delusererror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0115
        * @tc.name      : Test delcred interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */	
    it('Security_IAM_PIN_Interface_DFX_0115', 2, async function (done) {
        delcrederror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0116
        * @tc.name      : Test delcred interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0116', 2, async function (done) {
        delcrederror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0117
        * @tc.name      : Test getauthinfo interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */	
    it('Security_IAM_PIN_Interface_DFX_0117', 2, async function (done) {
        getauthinfoerror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0118
        * @tc.name      : Test delcred interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0118', 2, async function (done) {
        getauthinfoerror('Errortest',done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0119
        * @tc.name      : Test getAvailabeStatus interface DFX with null
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */	
    it('Security_IAM_PIN_Interface_DFX_0119', 2, async function (done) {
        getAvailabeStatuserror(null,done);
    })

    /*
        * @tc.number    : Security_IAM_PIN_Interface_DFX_0120
        * @tc.name      : Test getAvailabeStatus interface DFX with string
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_PIN_Interface_DFX_0120', 2, async function (done) {
        getAvailabeStatuserror('Errortest',done);
    })
})
