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
import userIDM from '@ohos.useridm'
import pinAuth from '@ohos.pinauth'
import userAuth from '@ohos.userauth'

let UserIDM = null
let PinAuth = null
let UserAuth = null

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function publicRegisterInputer(PinAuth,AuthSubType,Inputerdata){
    console.info('testFace publicRegisterInputer in');
    try {
        console.info('testFace publicRegisterInputer in try');
        console.info('testFace publicRegisterInputer PinAuth = ' + PinAuth);
        let registerresult = PinAuth.registerInputer({
            onGetData: (AuthSubType, IInputData) => {
                console.info('testFace faceDemo registerInputer AuthSubType');
                console.info('testFace faceDemo registerInputer AuthSubType111');
                IInputData.onSetData(AuthSubType, Inputerdata)
            }
        })
        console.log("testFace publicRegisterInputer result is: " + registerresult);
        return registerresult;
    }
    catch (e) {
        console.log("testFace publicRegisterInputer fail " + e);
        expect(null).assertFail();
    }
}

function publicunRegisterInputer(PinAuth,callback){
    console.info('testFace publicunRegisterInputer in');
    try {
        console.info('testFace publicunRegisterInputer in try');
        console.info('testFace publicunRegisterInputer PinAuth = ' + PinAuth);
        let unRegist = 1;
        PinAuth.unregisterInputer();
        callback(unRegist)
    }
    catch (e) {
        console.log("testFace unpublicRegisterInputer fail " + e);
        expect(null).assertFail();
    }
}

function publicOpenSession(UserIDM,callback){
    console.info('testFace publicOpenSession in');
    try {
        console.info('testFace publicOpenSession in try');
        console.info('testFace faceDemo UserIDM = ' + UserIDM);
        let challenge = null
        UserIDM.openSession( function(data){
            try{
                console.log("testFace before get challenge");
                console.log("testFace + " + data);
                challenge = data;
                console.log("testFace end ");
                console.log("testFace after get challenge");
                callback(challenge);
                expect(true).assertEqual(challenge != null);
            }
            catch(e) {
                console.info('testFace faceDemo openSession error = ' + JSON.stringify(e));
            }
        });
    }
    catch (e) {
        console.log("testFace publicOpenSession fail " + e);
        console.log(e);
        expect(null).assertFail();
    }
}

function publicCloseSession(UserIDM,callback){
    console.info('testFace publicCloseSession in');
    try {
        console.info('testFace publicCloseSession in try');
        console.info('testFace publicCloseSession UserIDM = ' + UserIDM);
        let closesession = 0;
        UserIDM.closeSession();
        callback(closesession);
    }
    catch (e) {
        console.log("testFace publicCloseSession fail " + e);
        expect(null).assertFail();
    }}

async function publicaddCredential(UserIDM,CredentialInfo,callback1,callback2){
    console.info('testFace publicaddCredential in');
    try {
        console.info('testFace publicaddCredential in try');
        let onresult = {
            addCredresult: null,
            credentialId: null,
        }
        let onAcquireInfo = {
            addCredmodule : null,
            addCredacquire : null,
            addCredextr : null
        }
        console.info('faceTest publicaddCredential UserIDM = ' + UserIDM);
        UserIDM.addCredential(CredentialInfo, {
            onResult: function(result,extraInfo){
                console.info('testFace aaaaaaaaaaaaa');
                console.info("testFace addCredential result = " + result);
                console.info("testFace addCredential extraInfo = " + JSON.stringify(extraInfo));
                console.log(result)
                onresult.addCredresult= result;
                if(extraInfo != undefined) {
                    onresult.credentialId = extraInfo.credentialId
                } else {
                    onresult.credentialId = null;
                }
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.info('testFace publicaddCredential onAcquireInfo in');
                onAcquireInfo.addCredmodule = modulea;
                onAcquireInfo.addCredacquire = acquire;
                onAcquireInfo.addCredextr = extr;
                console.info('testFace publicaddCredential onAcquireInfo =' + JSON.stringify(onAcquireInfo));
                console.info(modulea);
                callback2(onAcquireInfo)
            },
        })
    }
    catch (e) {
        console.log("testFace publicaddCredential fail " + e);
        expect(null).assertFail();
    }
}

function publicupdateCred(UserIDM,CredentialInfo,callback1,callback2){
    console.info('testFace publicupdateCred in');
    try {
        console.info('testFace publicupdateCred in try');
        console.info('testFace publicupdateCred UserIDM = ' + UserIDM);
        let onresult = {
            updateCredresult: null,
            CredentialId: null,
        }
        let onacquireinfo = {
            updateCredmodule : null,
            updateCredacquire : null,
            updateCredextr : null
        }
        UserIDM.updateCredential(CredentialInfo, {
            onResult: function(result,extraInfo){
                console.log("testFace faceDemo updateCredential result = " + result)
                onresult.updateCredresult = result
                console.log("testFace faceDemo updateCredential credentialId = " + extraInfo.credentialId)
                if(extraInfo != undefined) {
                    onresult.CredentialId = extraInfo.credentialId
                } else {
                    onresult.CredentialId = null;
                }
                console.info('testFace publicupdateCred updateCredential  onResult = ' + JSON.stringify(onresult));
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.info('testFace publicupdateCred updateCredential  onAcquireInfo in ');
                onacquireinfo.updateCredmodule = modulea
                onacquireinfo.updateCredacquire = acquire
                onacquireinfo.updateCredextr = extr
                console.info('testFace public updateCredential onacquireinfo = ' + JSON.stringify(onacquireinfo));
                console.log("testFace faceDemo updateCredential module = " + modulea)
                callback2(onacquireinfo)
            }
        })
    }
    catch (e) {
        console.log("testFace publicupdateCred fail " + e);
        expect(null).assertFail();
    }
}

function publiccancel(UserIDM,challenge){
    console.info('testFace publiccancel in');
    try {
        console.info('testFace publiccancel in try');
        console.info('testFace publiccancel UserIDM = ' + UserIDM);
        let cancelresult = UserIDM.cancel(challenge)
        console.log("testFace cancelresult is " + cancelresult);
        return cancelresult
    }
    catch (e) {
        console.log("testFace publiccancel fail " + e);
        expect(null).assertFail();
    }
}

function publicdelUser(UserIDM,token,callback1,callback2){
    console.info('testFace publicdelUser in');
    try {
        console.info('testFace publicdelUser in try');
        console.info('testFace publicdelUser UserIDM = ' + UserIDM);
        let onresult = {
            delUserresult: null,
            CredentialId: null,
        }
        let onacquireinfo = {
            delUsermodule : null,
            delUseracquire : null,
            delUserextr : null
        }
        UserIDM.delUser(token, {
            onResult: function(result,extraInfo){
                console.log("testFace faceDemo delUser result = " + result)
                onresult.delUserresult = result
                if(extraInfo != undefined) {
                    onresult.CredentialId = extraInfo.credentialId
                } else {
                    onresult.CredentialId = null;
                }
                console.info('testFace publicdelUser delUser = ' + JSON.stringify(onresult));
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.log("testFace faceDemo delUser onAcquireInfo in ");
                onacquireinfo.delUsermodule = modulea
                onacquireinfo.delUseracquire = acquire
                onacquireinfo.delUserextr = extr
                console.log("testFace faceDemo delUser onAcquireInfo = " + JSON.stringify(onacquireinfo));
                callback2(onacquireinfo)
            }
        })
    }
    catch (e) {
        console.log("testFace publicdelUser fail " + e);
        expect(null).assertFail();
    }
}

function publicdelCred(UserIDM,credentialId,token,callback1,callback2){
    console.info('testFace publicdelCred in');
    try {
        console.info('testFace publicdelCred in try');
        console.info('testFace publicdelCred UserIDM = ' + UserIDM);
        let onresult = {
            delCredresult: null,
            CredentialId: null,
        }
        let onacquireinfo = {
            delCredmodule : null,
            delCredacquire : null,
            delCredextr : null
        }
        UserIDM.delCred(credentialId, token,{
            onResult: function(result,extraInfo){
                console.log("testFace faceDemo delCred result = " + result)
                onresult.delCredresult = result
                console.log("testFace faceDemo delCred extraInfo = " + extraInfo)
                if(extraInfo != undefined) {
                    onresult.CredentialId = extraInfo.credentialId
                } else {
                    onresult.CredentialId = null;
                }
                console.log("testFace faceDemo delCred onresult = " + JSON.stringify(onresult));
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.info('testFace publicdelCred delCred onAcquireInfo in');
                onacquireinfo.delCredmodule = modulea
                onacquireinfo.delCredacquire = acquire
                onacquireinfo.delCredextr = extr
                console.log("testFace faceDemo delCred module = " + modulea);
                console.log("testFace faceDemo delCred onacquireinfo = " + JSON.stringify(onacquireinfo));
                callback2(onacquireinfo)
            }
        })
    }
    catch (e) {
        console.log("testFace publicdelCred fail " + e);
        expect(null).assertFail();
    }
}

function publicgetAuthInfo(UserIDM,authType,callback){
    console.info('testFace publicgetAuthInfo in');
    try {
        console.info('testFace publicgetAuthInfo in try');
        console.info('testFace publicgetAuthInfo UserIDM = ' + UserIDM);
        UserIDM.getAuthInfo(authType, function (AsyncCallback) {
            console.log("testFace faceDemo getAuthInfo AsyncCallback = " + JSON.stringify(AsyncCallback))
            callback(AsyncCallback)
        })
    }
    catch (e) {
        console.log("testFace publicgetAuthInfo fail " + e);
        expect(null).assertFail();
    }
}

function publicgetallAuthInfo(UserIDM,callback){
    console.info('testFace publicgetallAuthInfo in');
    try {
        console.info('testFace publicgetallAuthInfo in try');
        console.info('testFace publicgetallAuthInfo UserIDM = ' + UserIDM);
        UserIDM.getAuthInfo(function (AsyncCallback) {
            console.log("testFace faceDemo getallAuthInfo AsyncCallback = " + JSON.stringify(AsyncCallback))
            callback(AsyncCallback)
        })
    }
    catch (e) {
        console.log("testFace publicgetallAuthInfo fail " + e);
        expect(null).assertFail();
    }
}

function publicgetAvailabeStatus(UserAuth,authType,authTrustLevel){
    console.info('testFace publicgetAvailableStatus in');
    try {
        console.info('testFace publicgetAvailableStatus in try');
        console.info('testFace publicgetAvailableStatus UserAuth = ' + UserAuth);
        let AvailabeStatus = null
        AvailabeStatus = UserAuth.getAvailableStatus(authType, authTrustLevel);
        console.info('testFace publicgetAvailableStatus result = ' + JSON.stringify(AvailabeStatus));
        return AvailabeStatus
    }
    catch (e) {
        console.log("testFace publicgetAvailableStatus fail " + e);
        expect(null).assertFail();
    }
}

function publicgetProperty(UserAuth,GetPropertyRequest,callback){
    console.info('testFace publicgetProperty in');
    try {
        console.info('testFace publicgetProperty in try');
        console.info('testFace publicgetProperty UserAuth = ' + UserAuth);
        UserAuth.getProperty(GetPropertyRequest,function (AsyncCallback) {
            console.log("testFace faceDemo getallAuthInfo AsyncCallback = " + JSON.stringify(AsyncCallback))
            callback(AsyncCallback)
        })
    }
    catch (e) {
        console.log("testFace  publicgetProperty fail " + e);
        expect(null).assertFail();
    }
}


function publicsetProperty(UserAuth,SetPropertyRequest,callback){
    console.info('testFace publicsetProperty in');
    try {
        console.info('testFace publicsetProperty in try');
        console.info('testFace publicsetProperty UserAuth = ' + UserAuth);
        UserAuth.setProperty(SetPropertyRequest,function (AsyncCallback) {
            console.log("testFace faceDemo setProperty inside = ")
            console.log("testFace faceDemo setProperty AsyncCallback = " + JSON.stringify(AsyncCallback))
            callback(AsyncCallback)
        })
    }
    catch (e) {
        console.log("testFace publicsetProperty fail " + e);
        expect(null).assertFail();
    }
}

function publicauth(UserAuth,challenge,authType,authTrustLevel,callback1,callback2){
    console.info('testFace publicauth in');
    try {
        console.info('testFace publicauth in try');
        console.info('testFace publicauth UserAuth = ' + UserAuth);
        let onresult = {
            authresult: null,
            authextr: null,
        }
        let onacquireinfo = {
            authmodule : null,
            authacquire : null,
            authextr : null
        }
        let contextID1 = null
        contextID1 = UserAuth.auth(challenge, authType,authTrustLevel,{
            onResult: function(result,extraInfo){
                console.log("testFace faceDemo auth result = " + result)
                onresult.authresult = result
                console.log("testFace faceDemo auth extraInfo = " + JSON.stringify(extraInfo));
                onresult.authextr = extraInfo;
                console.info('testFace publicauth auth onResult = ' + JSON.stringify(onresult));
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.info('faceTest publicauth auth onAcquireInfo in');
                onacquireinfo.authmodule = modulea;
                onacquireinfo.authacquire = acquire;
                onacquireinfo.authextr = extr;
                console.log("testFace faceDemo auth module = " + JSON.stringify(modulea));
                console.info('testFace publicauth auth onAcquireInfo = ' + JSON.stringify(onacquireinfo));
                callback2(onacquireinfo)
            }
        });
        console.info('testFace publicauth auth contextID1 = ' + contextID1);
        return contextID1
    }
    catch (e) {
        console.log("testFace publicauth fail " + e);
        expect(null).assertFail();
    }
}

function publicauthUser(UserAuth,userID,challenge,authType,authTrustLevel,callback1,callback2){
    console.info('testFace publicauthUser in');
    try {
        console.info('testFace publicauthUser in try');
        console.info('testFace publicauthUser UserAuth = ' + UserAuth);
        let onresult = {
            authresult: null,
            authextr: null,
        }
        let onacquireinfo = {
            authmodule : null,
            authacquire : null,
            authextr : null
        }
        let contextID1 = null
        contextID1 = UserAuth.authUser(userID, challenge, authType, authTrustLevel, {
            onResult: function(result,extraInfo){
                console.log("testFace faceDemo authUser result = " + result)
                onresult.authresult = result
                console.log("testFace faceDemo authUser authextr = " + JSON.stringify(extraInfo))
                onresult.authextr = extraInfo;
                console.info('testFace publicauthUser authUser onResult = ' + JSON.stringify(onresult));
                callback1(onresult)
            },
            onAcquireInfo:function (modulea,acquire,extr){
                console.info('testFace publicauthUser authUser onAcquireInfo in');
                onacquireinfo.authmodule = modulea
                onacquireinfo.authacquire = acquire
                onacquireinfo.authextr = extr
                console.log("testFace faceDemo authUser module = " + JSON.stringify(modulea));
                console.info('testFace publicauthUser authUser onacquireinfo = ' + JSON.stringify(onacquireinfo));
                callback2(onacquireinfo)
            }
        })
        console.info('testFace publicauthUser authUser contextID1 = ' + contextID1);
        return contextID1
    }
    catch (e) {
        console.log("testFace publicauthUser fail " + e);
        expect(null).assertFail();
    }
}

function publicgecancelAuth(UserAuth,contextID){
    console.info('testFace publicgecancelAuth in');
    try {
        console.info('testFace publicgecancelAuth in try');
        console.info('testFace publicgecancelAuth UserAuth = ' + UserAuth);
        let cancelAuthresult = null
        cancelAuthresult = UserAuth.cancelAuth(contextID)
        console.info('testFace publicgecancelAuth cancelAuth cancelAuthresult = ' + cancelAuthresult);
        return cancelAuthresult
    }
    catch (e) {
        console.log("testFace publiccancelAuth fail " + e);
        expect(null).assertFail();
    }
}


export{publicRegisterInputer, publicgetallAuthInfo, 
publicunRegisterInputer, publicOpenSession, publicCloseSession, 
publicaddCredential, publicupdateCred, publiccancel, publicdelUser, 
publicdelCred, publicgetAuthInfo, publicgetAvailabeStatus, publicgetProperty, 
publicsetProperty, publicauth, publicauthUser, publicgecancelAuth}
