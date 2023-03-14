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
    AUTH_FAIL : 14
}

let GetPropertyType = {
    AUTH_SUB_TYPE : 1,
    REMAIN_TIMES : 2,
    FREEZING_TIME : 3
}

let Inputerdata = new Uint8Array([1, 2, 3, 4, 5, 6]);
let Inputerdatan = new Uint8Array([1, 1, 1, 1, 1, 1]);
let Inputerdatanum = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8]);
let Inputerdatanumn = new Uint8Array([1, 1, 1, 1, 1, 1, 1, 1]);
let Inputerdatamix = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8, 9]);
let Inputerdatamixn = new Uint8Array([1, 1, 1, 1, 1, 1, 1, 1, 1]);

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
        * @tc.number    : Security_IAM_Coauth_Func_0102
        * @tc.name      : Can't use old challenge add face in new session
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Coauth_Func_0102', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge1 = data;
                console.info('Coauth_Func_0102 publicOpenSession challenge1 = ' + challenge1);
                publicFC.publicOpenSession(UserIDM, function (data) {
                    let challenge2 = data;
                    console.info('Security_IAM_Coauth_Func_0102 publicOpenSession challenge2 = ' + challenge2);
                    publicFC.publicaddCredential(UserIDM, CredentialInfopinsix, function (data) {
                        let addcredresult101 = data.addCredresult;
                        console.info('Security_IAM_Coauth_Func_0102 addcredresult101 = ' + addcredresult101);
                        publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                            let info101 = data.authresult;
                            let token2 = data.authextr.token
                            console.info('Security_IAM_Coauth_Func_0102 publicauth info101 = ' + info101);
                            publicFC.publicauth(UserAuth, challenge2, AuthType.PIN, AuthTurstLevel.ATL1,
							function (data) {
                                let info1011 = data.authresult;
                                console.info('Security_IAM_Coauth_Func_0102 publicauth info1011 = ' + info1011);
                                let token1 = data.authextr.token
                                publicFC.publicdelUser(UserIDM, token2, function (data) {
                                    let deluserresult = data.delUserresult;
                                    expect(ResultCode.AUTH_FAIL).assertEqual(deluserresult);
                                    publicFC.publicdelUser(UserIDM, token1, function (data) {
                                        console.info('Security_IAM_Coauth_Func_0102 deluserresult ='+ deluserresult);
                                        publicFC.publicCloseSession(UserIDM, function (data) {
                                            console.info('Security_IAM_Coauth_Func_0102 CloseSession = ' + data);
                                            publicFC.publicunRegisterInputer(PinAuth, function (data) {
                                                console.info('Security_IAM_Coauth_Func_0102 unRegister = ' + data);
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
            console.log("testtwochallenge101 fail " + e);
            expect(null).assertFail();
        }
    })
	
    /*
        * @tc.number    : Security_IAM_Coauth_Func_0104
        * @tc.name      : Inquire face credential information
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
     it('Security_IAM_Coauth_Func_0104', 3, async function (done) {
         try {
             publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
             publicFC.publicOpenSession(UserIDM,function(data){
                 let challenge = data
                 publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                     function(onresult) {
                         console.info('testFace Coauth_Func_0104 addCredresult = ' + onresult.addCredresult);
                         publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                             function(onresult) {
                                 console.info(
                                    'testFace Coauth_Func_0104 addCred onResult = ' + JSON.stringify(onresult));
                                 let token = onresult.authextr.token
                                 CredentialInfoface2d.token = token
                                 publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                                     console.log("testFace Coauth_Func_0104 addface= " + onresult.addCredresult)
                                     publicFC.publicgetAuthInfo(UserIDM,AuthType.FACE,function(AsyncCallback) {
                                         console.log("testFace Coauth_Func_0104 getAuthInfo= " +
                                         JSON.stringify(AsyncCallback))
                                         expect(AuthSubType.FACE_2D).assertEqual(AsyncCallback[0].authSubType);
                                         publicFC.publicgetAuthInfo(UserIDM,AuthType.PIN,function(AsyncCallback) {
                                             console.log("testFace Coauth_Func_0104 getAuthInfo = " +
                                             JSON.stringify(AsyncCallback))
                                             expect(AuthSubType.PIN_SIX).assertEqual(AsyncCallback[0].authSubType);
                                             publicFC.publicdelUser(UserIDM,token,function(onresult){
                                                 let delresult = onresult.delUserresult;
                                                 console.log("testFace Coauth_Func_0104 delUser= " + delresult)
                                                 publicFC.publicCloseSession(UserIDM,function(data){
                                                     console.log("testFaceCoauth_Func_0104 CloseSession= " + data)
                                                     publicFC.publicunRegisterInputer(PinAuth,function(data){
                                                         done();
                                                     })
                                                 })
                                             },function(onacquireinfo){
                                             })
                                         })
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
             console.log("Security_IAM_Coauth_Func_0104 fail " + e);
             expect(null).assertFail();
         }
     })

	/*
        * @tc.number    : Security_IAM_Coauth_Func_0105
        * @tc.name      : Inquire all credential information with one credential
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 2
    */
    it('Security_IAM_Coauth_Func_0105', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            publicFC.publicOpenSession(UserIDM,function(data){
                let challenge = data
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,function(onresult) {
                    console.info('testFace Security_IAM_Coauth_Func_0105 addCredresult = ' + onresult.addCredresult);
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function(onresult) {
                        console.info('testFace Coauth_Func_0105 addCred onResult = ' + onresult.authresult);
                        let token = onresult.authextr.token
                        CredentialInfoface2d.token = token
                        publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,function(onresult) {
                            console.info('testFace Coauth_Func_0105 addface onResult = ' + onresult.addCredresult);
                            publicFC.publicgetAuthInfo(UserIDM,AuthType.PIN,function(data){
                                let CredInfo = data;
                                console.info('testFace Coauth_Func_0105 getAuthInfo = ' + CredInfo[0].authSubType);
                                expect(AuthSubType.PIN_SIX).assertEqual(CredInfo[0].authSubType);
                                    publicFC.publicdelUser(UserIDM,token,function(onresult){
                                        console.log("testFace Coauth_Func_0105 delUser = " + onresult.delUserresult)
                                        publicFC.publicCloseSession(UserIDM,function(data){
                                            console.log("testFace Coauth_Func_0105 CloseSession callback = " + data);
                                            publicFC.publicunRegisterInputer(PinAuth,function(data){
                                                done();
                                            });
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
            console.log("Security_IAM_Coauth_Func_0105 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Coauth_Func_0106
        * @tc.name      : Inquire all credential information with two credential
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
     it('Security_IAM_Coauth_Func_0106', 3, async function (done) {
         try {
             publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
             publicFC.publicOpenSession(UserIDM,function(data){
                 let challenge = data
                 publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                     function(onresult) {
                         console.info('testFace Coauth_Func_0106 addCredresult = ' + onresult.addCredresult);
                         publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                             function(onresult) {
                                 console.info('testFace Coauth_Func_0106 addCred= ' + JSON.stringify(onresult));
                                 let token = onresult.authextr.token
                                 CredentialInfoface2d.token = token
                                 publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                                     console.log("testFace Coauth_Func_0106 addCface = " + onresult.addCredresult)
                                     publicFC.publicgetallAuthInfo(UserIDM,function(AsyncCallback) {
                                         console.log("testFace Coauth_Func_0106 getAuthInfo = " +
                                         JSON.stringify(AsyncCallback))
                                         expect(AuthSubType.PIN_SIX).assertEqual(AsyncCallback[1].authSubType);
                                         expect(AuthType.PIN).assertEqual(AsyncCallback[1].authType);
                                         expect(AuthSubType.FACE_2D).assertEqual(AsyncCallback[0].authSubType);
                                         expect(AuthType.FACE).assertEqual(AsyncCallback[0].authType);
                                         publicFC.publicdelUser(UserIDM,token,function(onresult){
                                             console.log("testFace Coauth_Func_0106delUser="+ onresult.delUserresult)
                                             publicFC.publicCloseSession(UserIDM,function(data){
                                                 console.log("testFace Coauth_Func_0106 CloseSession = " + data)
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
                             }
                         );
                     }, function(onAcquireInfo) {
                     });
             })
         } catch (e) {
             console.log("Security_IAM_Coauth_Func_0106 fail " + e);
             expect(null).assertFail();
         }
     })

    /*
        * @tc.number    : Security_IAM_Coauth_Func_0108
        * @tc.name      : Get face availabe status
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Coauth_Func_0108', 3, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            publicFC.publicOpenSession(UserIDM,function(data){
                let challenge = data
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                    function(onresult) {
                        console.info('testFace Coauth_Func_0108 addCredresult = ' + onresult.addCredresult);
                        console.log("testFace faceDemo Coauth_Func_0108 authUser = " + onresult.addCredresult);
                        publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                            function(onresult) {
                                console.info('testFace Coauth_Func_0108 addCred = ' + JSON.stringify(onresult));
                                let token = onresult.authextr.token
                                CredentialInfoface2d.token = token
                                publicFC.publicaddCredential(UserIDM,CredentialInfoface2d,function(onresult) {
                                    console.info('testFace Coauth_Func_0108 ddCredentialface=' + onresult.authresult);
                                    console.log("testFace Coauth_Func_0108 addface result =" + onresult.addCredresult)
                                    let AvailabeStatus = publicFC.publicgetAvailabeStatus(
									UserAuth,AuthType.FACE,AuthTurstLevel.ATL1)
                                    expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                                    AvailabeStatus = publicFC.publicgetAvailabeStatus(
									UserAuth,AuthType.FACE,AuthTurstLevel.ATL2)
                                    expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                                    AvailabeStatus = publicFC.publicgetAvailabeStatus(
									UserAuth,AuthType.FACE,AuthTurstLevel.ATL3)
                                    expect(ResultCode.TRUST_LEVEL_NOT_SUPPORT).assertEqual(AvailabeStatus);
                                    AvailabeStatus = publicFC.publicgetAvailabeStatus(
									UserAuth,AuthType.FACE,AuthTurstLevel.ATL4)
                                    expect(ResultCode.TRUST_LEVEL_NOT_SUPPORT).assertEqual(AvailabeStatus);
                                    publicFC.publicdelUser(UserIDM,token,function(onresult){
                                        console.log("testFace Coauth_Func_0108 delUser = " + onresult.delUserresult)
                                        publicFC.publicCloseSession(UserIDM,function(data){
                                            console.log("testFace Coauth_Func_0108 CloseSession callback = " + data)
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
            console.log("Security_IAM_Coauth_Func_0108 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Coauth_Func_0109
        * @tc.name      : Get pin availabe status
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Coauth_Func_0109', 2, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            publicFC.publicOpenSession(UserIDM,function(data){
                let challenge = data
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix,
                    function(onresult) {
                        console.info('testFace Coauth_Func_0108 addCredresult = ' + onresult.addCredresult);
                        console.log("testFace Coauth_Func_0109 authUser result = " + onresult.addCredresult);
                        let AvailabeStatus
						AvailabeStatus = publicFC.publicgetAvailabeStatus(UserAuth,AuthType.PIN,AuthTurstLevel.ATL1)
                        console.info('testFace Security_IAM_Coauth_Func_0109 AvailabeStatus1 = ' + AvailabeStatus);
                        expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                        AvailabeStatus = publicFC.publicgetAvailabeStatus(UserAuth,AuthType.PIN,AuthTurstLevel.ATL2)
                        console.info('testFace Security_IAM_Coauth_Func_0109 AvailabeStatus2 = ' + AvailabeStatus);
                        expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                        AvailabeStatus = publicFC.publicgetAvailabeStatus(UserAuth,AuthType.PIN,AuthTurstLevel.ATL3)
                        console.info('testFace Security_IAM_Coauth_Func_0109 AvailabeStatus3 = ' + AvailabeStatus);
                        expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                        AvailabeStatus = publicFC.publicgetAvailabeStatus(UserAuth,AuthType.PIN,AuthTurstLevel.ATL4)
                        console.info('testFace Security_IAM_Coauth_Func_0109 AvailabeStatus4 = ' + AvailabeStatus);
                        expect(ResultCode.SUCCESS).assertEqual(AvailabeStatus);
                        publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1,
                            function(onresult) {
                                console.info('testFace Coauth_Func_0109 addCred onResult = ' + onresult.authresult);
                                let token = onresult.authextr.token
                                publicFC.publicdelUser(UserIDM,token,function(onresult){
                                    console.log("testFace Coauth_Func_0109 delUser result= " + onresult.delUserresult)
                                    publicFC.publicCloseSession(UserIDM,function(data){
                                        console.log("testFace Coauth_Func_0109 CloseSession callback = " + data)
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
            console.log("Security_IAM_Coauth_Func_0109 fail " + e);
            expect(null).assertFail();
        }
    })

    /*
        * @tc.number    : Security_IAM_Coauth_Func_0110
        * @tc.name      : Get pin and face property
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Coauth_Func_0110', 3, async function (done) {
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
                            publicFC.publicgetProperty(UserAuth,GetPropertyRequestface,function(onresult) {
                                console.log("testFace Coauth_Func_0110 GetPropertyface = " + JSON.stringify(onresult))
                                expect(ResultCode.SUCCESS).assertEqual(onresult.result);
                                expect(AuthSubType.FACE_2D).assertEqual(onresult.authSubType);
                                publicFC.publicgetProperty(UserAuth,GetPropertyRequestpin,function(onresult) {
                                    expect(ResultCode.SUCCESS).assertEqual(onresult.result);
                                    expect(AuthSubType.PIN_SIX).assertEqual(onresult.authSubType);
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
                            })
                        }, function(onAcquireInfo) {
                        })
                    }, function(onAcquireInfo) {
                    }
                )}, function(onAcquireInfo) {
                })
            })
        } catch (e) {
            console.log("Security_IAM_Coauth_Func_0110 fail " + e);
            expect(null).assertFail();
        }
    })
    
	/*
        * @tc.number    : Security_IAM_Coauth_Func_0112
        * @tc.name      : Anti violence cracking with four times
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 3
    */
    it('Security_IAM_Coauth_Func_0112', 3, async function (done) {
        console.info('testFace Security_IAM_Coauth_Func_0112 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge1 = data;
                console.info("Security_IAM_Coauth_Func_0112 challenge1 = " + challenge1);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinnum, function (data) {
                    let addcredresult101 = data.addcredresult;
                    publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let auth101 = data
                        console.info("Security_IAM_Coauth_Func_0112 auth1 = " + auth101.authresult);
                        let token = auth101.authextr.token
                        console.info("Security_IAM_Coauth_Func_0112 token = " + token);
                        publicFC.publicunRegisterInputer(PinAuth, async function (data) {
                            console.info("Security_IAM_Coauth_Func_0112 unRegist = " + data);
                            setTimeout(
						publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdatan), 500)
                            console.info("Security_IAM_Coauth_Func_0112 challenge2 = " + challenge1);
                            await publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
							function (data) {
                                console.info("Security_IAM_Coauth_Func_0112 authresult1 = " + data.authresult);
                                let authresult1 = data.authresult
                                expect(ResultCode.FAIL).assertEqual(authresult1);
                                publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
								function (data) {
                                    console.info("Security_IAM_Coauth_Func_0112 authresult2 = " + data.authresult);
                                    let authresult2 = data.authresult
                                    expect(ResultCode.FAIL).assertEqual(authresult2);
                                    publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
									function (data) {
                                        console.info("Security_IAM_Coauth_Func_0112 authresult3 =" + data.authresult);
                                        let authresult3 = data.authresult
                                        expect(ResultCode.FAIL).assertEqual(authresult3);
                                        publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
										function (data) {
                                            console.info("Coauth_Func_0112 authresult4 = " + data.authresult);
                                            let authresult4 = data.authresult
                                            expect(ResultCode.FAIL).assertEqual(authresult4);
                                            publicFC.publicgetProperty(UserAuth,GetPropertyRequestpin, function(data){
                                                expect(1).assertEqual(data.remainTimes);
                                                publicFC.publicunRegisterInputer(PinAuth, async function (data) {
                                                    console.info("Security_IAM_Coauth_Func_0112 unRegist = " + data);
                                                    setTimeout(publicFC.publicRegisterInputer(
												PinAuth, AuthSubType.PIN_SIX, Inputerdata), 500)
                                                    console.info("Coauth_Func_0112 challenge2 = " + challenge1);
                                                    await publicFC.publicauth(
													UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
													function (data) {
                                                        let authresult5 = data.authresult
                                                        expect(ResultCode.SUCCESS).assertEqual(authresult5);
                                                        publicFC.publicdelUser(UserIDM, token, function (data) {
                                                            let delresult = data.delUserresult
                                                            console.info("Coauth_Func_0112 delresult = " + delresult);
                                                            publicFC.publicCloseSession(UserIDM, function (data) {
                                                                console.info("Coauth_Func_0112 closesession=" + data);
                                                                publicFC.publicunRegisterInputer(PinAuth,
																function (data) {
                                                                    console.info("Coauth_Func_0112 unRegist=" + data);
                                                                    done();
                                                                })
                                                            })
                                                        }, function (data) {
                                                        })
                                                    }, function (data) {
                                                    })
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
                    }, function (data) {
                    })
                }, function (data) {
                })
            })
        } catch (e) {
            console.log("Security_IAM_Coauth_Func_0112 fail " + e);
            expect(null).assertFail();
        }
    })

	/*
        * @tc.number    : Security_IAM_Coauth_Func_0122
        * @tc.name      : Cancel auth face.
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_Coauth_Func_0122', 0, async function (done) {
        try {
            publicFC.publicRegisterInputer(PinAuth,AuthSubType.PIN_SIX,Inputerdata)
            let challenge ;
            publicFC.publicOpenSession(UserIDM, function (data) {
                console.info('Security_IAM_Coauth_Func_0122 openSession challenge = ' + data);
                challenge = data;
                publicFC.publicaddCredential(UserIDM,CredentialInfopinsix, function (onresult) {
                    console.info('Security_IAM_Coauth_Func_0122 addCredential Result1 = ' + JSON.stringify(onresult));
                    let info101;
                    publicFC.publicauth(UserAuth,challenge,AuthType.PIN,AuthTurstLevel.ATL1, function (data) {
                        console.info('Coauth_Func_0122 auth onResult = ' + JSON.stringify(data));
                        info101 = data;
                        let token = info101.authextr.token;
                        CredentialInfoface2d.token = token;
                        let addfaceresult;
                        publicFC.publicaddCredential(UserIDM,CredentialInfoface2d, function (onresult) {
                            console.info('Coauth_Func_0122 addCredential Result2=' + JSON.stringify(onresult));
                            addfaceresult = onresult;
                            console.info('Coauth_Func_0122 publicaddCredential = ' + addfaceresult.addCredresult);
                            expect(ResultCode.SUCCESS).assertEqual(addfaceresult.addCredresult);
                            publicFC.publicauth(UserAuth,challenge,AuthType.FACE,10000, async function (data) {
                                let faceauth101 = data;
                                console.info('Coauth_Func_0122 publicauth = ' + faceauth101.authresult);
                                expect(ResultCode.SUCCESS).assertEqual(faceauth101.authresult);
                                let authresult;
                                let contextID = await publicFC.publicauth(UserAuth,challenge,AuthType.FACE,10000,
                                    function (data) {
                                        console.info('Coauth_Func_0122 addCred='+ JSON.stringify(data.authresult));
                                        authresult = data.authresult;
                                    }, function (onacquireinfo) {
                                    })
                                let cancelresult = publicFC.publicgecancelAuth(UserAuth,contextID);
                                await sleep(2500);
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
        * @tc.number    : Security_IAM_Coauth_Func_0113
        * @tc.name      : Anti violence cracking with five times
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('Security_IAM_Coauth_Func_0113', 3, async function (done) {
        console.info('testFace Security_IAM_Coauth_Func_0113 start');
        try {
            publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdata)
            publicFC.publicOpenSession(UserIDM, function (data) {
                let challenge1 = data;
                console.info("Security_IAM_Coauth_Func_0113 challenge1 = " + challenge1);
                publicFC.publicaddCredential(UserIDM, CredentialInfopinnum, function (data) {
                    let addcredresult101 = data.addcredresult;
                    publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                        let auth101 = data
                        console.info("Security_IAM_Coauth_Func_0113 auth1 = " + auth101.authresult);
                        let token = auth101.authextr.token
                        console.info("Security_IAM_Coauth_Func_0113 token = " + token);
                        publicFC.publicunRegisterInputer(PinAuth, async function (data) {
                            console.info("Security_IAM_Coauth_Func_0113 unRegist = " + data);
                            setTimeout(
						publicFC.publicRegisterInputer(PinAuth, AuthSubType.PIN_SIX, Inputerdatan), 500)
                            console.info("Security_IAM_Coauth_Func_0113 challenge2 = " + challenge1);
                            await publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
							function (data) {
                                let authresult1 = data.authresult
                                expect(ResultCode.FAIL).assertEqual(authresult1);
                                publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
								function (data) {
                                    let authresult2 = data.authresult
                                    expect(ResultCode.FAIL).assertEqual(authresult2);
                                    publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
									function (data) {
                                        let authresult3 = data.authresult
                                        expect(ResultCode.FAIL).assertEqual(authresult3);
                                        publicFC.publicauth(UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
										function (data) {
                                            let authresult4 = data.authresult
                                            expect(ResultCode.FAIL).assertEqual(authresult4);
                                            publicFC.publicauth(
											UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1, function (data) {
                                                let authresult5 = data.authresult
                                                expect(ResultCode.FAIL).assertEqual(authresult5);
                                                publicFC.publicgetProperty(UserAuth,GetPropertyRequestpin,
												function(data){
                                                    expect(0).assertEqual(data.remainTimes);
                                                    publicFC.publicunRegisterInputer(PinAuth, async function (data) {
                                                        console.info("Coauth_Func_0113 unRegist = " + data);
                                                        setTimeout(publicFC.publicRegisterInputer(
													PinAuth, AuthSubType.PIN_SIX, Inputerdata), 500)
                                                        console.info("Coauth_Func_0113 challenge2 = " + challenge1);
                                                        await publicFC.publicauth(
														UserAuth, challenge1, AuthType.PIN, AuthTurstLevel.ATL1,
														async function (data) {
                                                            let authresult6 = data.authresult
                                                            expect(ResultCode.LOCKED).assertEqual(authresult6);
                                                                    publicFC.publicCloseSession(UserIDM,
																	function (data) {
                                                                        console.info("closesession = " + data);
                                                                        publicFC.publicunRegisterInputer(
																		PinAuth, function (data) {
                                                                            console.info("unRegist = " + data);
                                                                            done();
                                                                        })
                                                                    })
                                                        }, function (data) {
                                                        })
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
                    }, function (data) {
                    })
                }, function (data) {
                })
            })
        } catch (e) {
            console.log("Security_IAM_Coauth_Func_0113 fail " + e);
            expect(null).assertFail();
        }
    })
})
