// @ts-nocheck
/**
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index.ets'
import call from '@ohos.telephony.call';
import utils from './Utils.ets'

export const PHONE_NUMBER = '';
export const DEFAULT_SLOT_ID = 0;
export const MEDIA_TYPE_VOICE = 0;
export const DIAL_SCENCE_CALL_NORMAL = 0;
export const DIAL_CARRIER_TYPE = 0;
export const DIAL_SCENCE_CALL_PRIVILEGED = 1;
export const BOUNDARY_NUMBER_INT = 2147483649;
export const CALL_ID_NOT_EXIST = 999;

export function toString (data) {
  if (typeof data === 'object') {
    return JSON.stringify(data);
  } else {
    return data;
  }
}

class CallAttributeOptions {
  constructor (callId) {
    this.callId = callId;
  }
}

export default function callJsunit(){
  describe('callTest', function(){
    console.log("==========> call Test start ==========>");

    /**
       * @tc.number  Telephony_Call_Call_Dial_0100
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Dial_0100",0,async function(done){
      let caseName:string = "Telephony_Call_Call_Dial_0100";
      console.log(`==========> ${caseName} Test start ==========>`);
      call.dial(PHONE_NUMBER,(err,data) => {
        if(err){
          console.log(`${caseName} error,case success,error:${toString(err)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case fail`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_Dial_0200
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Dial_0200",0,async function(done){
      let caseName:string = "Telephony_Call_Call_Dial_0200";
      console.log(`==========> ${caseName} Test start ==========>`);
      let options = {extras:false, accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
        dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE};
      call.dial(PHONE_NUMBER,options,(err,data) => {
        if(err){
          console.log(`${caseName} error,case success,error:${toString(err)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case fail`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_Dial_0200
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Dial_0200",0,async function(done){
      let caseName:string = "Telephony_Call_Call_Dial_0200";
      console.log(`==========> ${caseName} Test start ==========>`);
      let options = {extras:false, accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
        dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE};
      call.dial(PHONE_NUMBER,options,(err,data) => {
        if(err){
          console.log(`${caseName} error,case success,error:${toString(err)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case fail`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });



    /**
       * @tc.number  Telephony_Call_Call_Dial_0300
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Dial_0300",0,async function(done){
      let caseName:string = "Telephony_Call_Call_Dial_0300";
      console.log(`==========> ${caseName} Test start ==========>`);
      let options = {extras:false, accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
        dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE};
      call.dial(PHONE_NUMBER,options).then(data=>{
        expect(true).assertTrue();
      }).catch(error =>{
        console.log(caseName+ " dial error :"+ toString(error));
        done();
        return;
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_MakeCall_0100
       * @tc.name    Make a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_MakeCall_0100",0,async function(done){
      let caseName:string = "Telephony_Call_Call_MakeCall_0100";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        let telNumber = "";
        call.makeCall(telNumber,(error) => {
          if(error){
            console.log(`${caseName} fail,case success,error:${toString(error)}`);
            expect(true).assertTrue();
            done();
            return;
          }
          expect().assertFail();
          console.log(`${caseName} success,case fail`);
          done();
        });
      }catch(err){
        console.log(`${caseName} fail,case success,error:${toString(err)}`);
        expect(true).assertTrue();
        done();
        return;
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_MakeCall_0200
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_MakeCall_0200",0,async function(done){
      let caseName:string = "Telephony_Call_Call_MakeCall_0200";
      console.log(`==========> ${caseName} Test start ==========>`);
      let telNumber = "";
      try{
        call.makeCall(telNumber).then(()=>{
          console.log(`${caseName} success,case fail`);
          done();
        }).catch(error =>{
          console.log(`${caseName} fail,case success,error:${toString(error)}`);
          expect(true).assertTrue();
          done();
          return;
        });
      }catch(err){
        console.log(`${caseName} fail,case success,error:${toString(err)}`);
        expect(true).assertTrue();
        done();
        return;
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_HasVoiceCapability_HasVoiceCapability
       * @tc.name    Checks whether a device supports voice calls.
       * @tc.desc    Return type test
       */
    it("Telephony_Call_HasVoiceCapability_HasVoiceCapability",0,async function(done){
      let caseName = 'Telephony_Call_HasVoiceCapability_HasVoiceCapability';
      console.log(`==========> ${caseName} Test start ==========>`);
      let ret = false;
      let voiceCapablity = call.hasVoiceCapability();
      if(voiceCapablity === true || voiceCapablity === false){
        ret = true;
      }
      expect(ret).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_Reject_0100
       * @tc.name    call_reject
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Reject_0100",0,async function(done){
      let caseName = 'Telephony_Call_Call_Reject_0100';
      console.log(`==========> ${caseName} Test start ==========>`);
      call.reject(BOUNDARY_NUMBER_INT, (error) => {
        if (error) {
          console.log(`${caseName} error,case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case failed`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_Reject_0200
       * @tc.name    call_reject
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Reject_0200",0,async function(done){
      let caseName = 'Telephony_Call_Call_Reject_0200';
      console.log(`==========> ${caseName} Test start ==========>`);
      let options = {messageContent:"hello"}
      call.reject(BOUNDARY_NUMBER_INT,options, (error) => {
        if (error) {
          console.log(`${caseName} error,case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case failed`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Call_Reject_0300
       * @tc.name    call_reject
       * @tc.desc    Function test
       */
    it("Telephony_Call_Call_Reject_0300",0,async function(done){
      let caseName = 'Telephony_Call_Call_Reject_0300';
      console.log(`==========> ${caseName} Test start ==========>`);
      call.reject(CALL_ID_NOT_EXIST).then(() => {
        expect().assertFail();
        console.log(`${caseName} success,case failed`);
        done();
      }).catch(error => {
        console.log(`${caseName} error,case success,error:${toString(error)}`);
        done();
        return;
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_On_CallDetailsChange_0100
       * @tc.name    Method in call_on of type callDetailsChange
       * @tc.desc    Function test
       */
    it("Telephony_Call_On_CallDetailsChange_0100",0,async function(done){
      let caseName:string = "Telephony_Call_On_CallDetailsChange_0100";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.on("callDetailsChange",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          } else {
            let callAttributeOptions = data;
            console.log(`${caseName} success, startTime = :` + callAttributeOptions.startTime);
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Off_callDetailsChange_0100
       * @tc.name    Method in call_off of type callDetailsChange
       * @tc.desc    Function test
       */
    it("Telephony_Call_Off_CallDetailsChange_0100",0,async function(done){
      let caseName:string = "Telephony_Call_Off_CallDetailsChange_0100";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.off("callDetailsChange",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_On_CallEventChange_0200
       * @tc.name    Method in call_on of type callEventChange
       * @tc.desc    Function test
       */
    it("Telephony_Call_On_CallEventChange_0200",0,async function(done){
      let caseName:string = "Telephony_Call_On_CallEventChange_0200";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.on("callEventChange",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Off_CallEventChange_0200
       * @tc.name    Method in call_off of type callEventChange
       * @tc.desc    Function test
       */
    it("Telephony_Call_Off_CallEventChange_0200",0,async function(done){
      let caseName:string = "Telephony_Call_Off_CallEventChange_0200";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.off("callEventChange",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Off_CallEventChange_0200
       * @tc.name    Method in call_off of type callDisconnectedCause
       * @tc.desc    Function test
       */
    it("Telephony_Call_On_CallDisconnectedCause_0300",0,async function(done){
      let caseName:string = "Telephony_Call_On_CallDisconnectedCause_0300";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.on("callDisconnectedCause",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Off_CallEventChange_0200
       * @tc.name    Method in call_off of type callDisconnectedCause
       * @tc.desc    Function test
       */
    it("Telephony_Call_Off_CallDisconnectedCause_0300",0,async function(done){
      let caseName:string = "Telephony_Call_Off_CallDisconnectedCause_0300";
      console.log(`==========> ${caseName} Test start ==========>`);
      try{
        call.off("callDisconnectedCause",(err,data) => {
          if(err){
            console.log(`${caseName} error,error :${toString(err)}`);
            done();
            return;
          }
          expect(true).assertTrue();
          done();
        });
        done();
      }catch(err){
        expect().assertFail();
        console.log(`${caseName} fail, err:${err}`);
        done();
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_AudioDevice_DEVICE_MIC
       * @tc.name    Enum AudioDevice
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_AudioDevice_DEVICE_MIC",0,async function(done){
      let caseName = 'Telephony_Call_AudioDevice_DEVICE_MIC';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(0).assertEqual(call.AudioDevice.DEVICE_MIC);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_Dial_Dial_0200
       * @tc.name    Makes a call.
       * @tc.desc    Function test
       */
    it("Telephony_Call_DialOptions_Extras",0,async function(done){
      let caseName:string = "Telephony_Call_DialOptions_extras";
      console.log(`==========> ${caseName} Test start ==========>`);
      let options = {extras:false, accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
        dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE};
      call.dial(PHONE_NUMBER,options,(err,data) => {
        if(err){
          console.log(`${caseName} error,case success,error:${toString(err)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} success,case fail`);
        done();
      });
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferType_TRANSFER_TYPE_BUSY
       * @tc.name    Enum CallTransferType
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_CallTransferType_TRANSFER_TYPE_BUSY",0,async function(done){
      let caseName = 'Telephony_Call_CallTransferType_TRANSFER_TYPE_BUSY';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(1).assertEqual(call.CallTransferType.TRANSFER_TYPE_BUSY);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferSettingType_CALL_TRANSFER_REGISTRATION
       * @tc.name    Enum CallTransferSettingType
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_CallTransferSettingType_CALL_TRANSFER_REGISTRATION",0,async function(done){
      let caseName = 'Telephony_Call_CallTransferSettingType_CALL_TRANSFER_REGISTRATION';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(3).assertEqual(call.CallTransferSettingType.CALL_TRANSFER_REGISTRATION);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferSettingType_CALL_TRANSFER_ERASURE
       * @tc.name    Enum CallTransferSettingType
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_CallTransferSettingType_CALL_TRANSFER_ERASURE",0, function(done){
      let caseName = 'Telephony_Call_CallTransferSettingType_CALL_TRANSFER_ERASURE';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(4).assertEqual(call.CallTransferSettingType.CALL_TRANSFER_ERASURE);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallAbilityEventId_EVENT_DIAL_NO_CARRIER
       * @tc.name    Enum CallAbilityEventId
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_CallAbilityEventId_EVENT_DIAL_NO_CARRIER",0, function(done){
      let caseName = 'Telephony_Call_CallAbilityEventId_EVENT_DIAL_NO_CARRIER';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(1).assertEqual(call.CallAbilityEventId.EVENT_DIAL_NO_CARRIER);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallAbilityEventId_EVENT_INVALID_FDN_NUMBER
       * @tc.name    Enum CallAbilityEventId
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_CallAbilityEventId_EVENT_INVALID_FDN_NUMBER",0, function(done){
      let caseName = 'Telephony_Call_CallAbilityEventId_EVENT_INVALID_FDN_NUMBER';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(2).assertEqual(call.CallAbilityEventId.EVENT_INVALID_FDN_NUMBER);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferInfo_Type
       * @tc.name    Interface CallTransferInfo
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallTransferInfo_Type', 0, function (done) {
      let caseName = 'Telephony_Call_CallTransferInfo_Type';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callTransferInfo = {transferNum:'10000000001', type: 2, settingType: 1}
      expect(callTransferInfo.type === 2).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferResult_Status
       * @tc.name    Interface CallTransferResult
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallTransferResult_Status', 0, function (done) {
      let caseName = 'Telephony_Call_CallTransferResult_Status';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callTransferResult = {status:0, number: '10000000001'}
      expect(callTransferResult.status === 0).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallTransferResult_Number
       * @tc.name    Interface CallTransferResult
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallTransferResult_Number', 0, function (done) {
      let caseName = 'Telephony_Call_CallTransferResult_Number';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callTransferResult = {status:0, number: '10000000001'}
      expect(callTransferResult.number === '10000000001').assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallAttributeOptions_CallId
       * @tc.name    Interface CallAttributeOptions
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallAttributeOptions_CallId', 0, function (done) {
      let caseName = 'Telephony_Call_CallAttributeOptions_CallId';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callAttributeOptions = {callId:1};
      expect(callAttributeOptions.callId === 1).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallRestrictionInfo_Type
       * @tc.name    Interface CallRestrictionInfo
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallRestrictionInfo_Type', 0, function (done) {
      let caseName = 'Telephony_Call_CallRestrictionInfo_Type';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callRestrictionInfo = {type:0, password: '123456', mode:1}
      expect(callRestrictionInfo.type === 0).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallRestrictionInfo_Mode
       * @tc.name    Interface CallRestrictionInfo
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallRestrictionInfo_Mode', 0, function (done) {
      let caseName = 'Telephony_Call_CallRestrictionInfo_Mode';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callRestrictionInfo = {type:0, password: '123456', mode:1}
      expect(callRestrictionInfo.mode === 1).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_CallEventOptions_EventId
       * @tc.name    Interface CallEventOptions
       * @tc.desc    Interface value test
       */
    it('Telephony_Call_CallEventOptions_EventId', 0, function (done) {
      let caseName = 'Telephony_Call_CallEventOptions_EventId';
      console.log(`==========> ${caseName} Test start ==========>`);
      let callEventOptions = {eventId:1}
      expect(callEventOptions.eventId === 1).assertTrue();
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_NO_ROUTE_TO_DESTINATION
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_UNASSIGNED_NUMBER",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_UNASSIGNED_NUMBER';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(1).assertEqual(call.DisconnectedDetails.UNASSIGNED_NUMBER);
      expect(3).assertEqual(call.DisconnectedDetails.NO_ROUTE_TO_DESTINATION);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });


    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_CHANNEL_UNACCEPTABLE
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_CHANNEL_UNACCEPTABLE",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_CHANNEL_UNACCEPTABLE';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(6).assertEqual(call.DisconnectedDetails.CHANNEL_UNACCEPTABLE);
      expect(8).assertEqual(call.DisconnectedDetails.OPERATOR_DETERMINED_BARRING);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_NORMAL_CALL_CLEARING
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_NORMAL_CALL_CLEARING",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_NORMAL_CALL_CLEARING';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(16).assertEqual(call.DisconnectedDetails.NORMAL_CALL_CLEARING);
      expect(17).assertEqual(call.DisconnectedDetails.USER_BUSY);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_NO_USER_RESPONDING
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_NO_USER_RESPONDING",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_NO_USER_RESPONDING';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(18).assertEqual(call.DisconnectedDetails.NO_USER_RESPONDING);
      expect(19).assertEqual(call.DisconnectedDetails.USER_ALERTING_NO_ANSWER);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_CALL_REJECTED
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_CALL_REJECTED",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_CALL_REJECTED';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(21).assertEqual(call.DisconnectedDetails.CALL_REJECTED);
      expect(22).assertEqual(call.DisconnectedDetails.NUMBER_CHANGED);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_DESTINATION_OUT_OF_ORDER
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_DESTINATION_OUT_OF_ORDER",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_DESTINATION_OUT_OF_ORDER';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(27).assertEqual(call.DisconnectedDetails.DESTINATION_OUT_OF_ORDER);
      expect(28).assertEqual(call.DisconnectedDetails.INVALID_NUMBER_FORMAT);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_DESTINATION_NETWORK_OUT_OF_ORDER
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_DESTINATION_NETWORK_OUT_OF_ORDER",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_NETWORK_OUT_OF_ORDER';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(38).assertEqual(call.DisconnectedDetails.NETWORK_OUT_OF_ORDER);
      expect(41).assertEqual(call.DisconnectedDetails.TEMPORARY_FAILURE);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_INVALID_PARAMETER
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_INVALID_PARAMETER",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_INVALID_PARAMETER';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(1025).assertEqual(call.DisconnectedDetails.INVALID_PARAMETER);
      expect(1026).assertEqual(call.DisconnectedDetails.SIM_NOT_EXIT);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_SIM_PIN_NEED
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_SIM_PIN_NEED",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_SIM_PIN_NEED';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(1027).assertEqual(call.DisconnectedDetails.SIM_PIN_NEED);
      expect(1029).assertEqual(call.DisconnectedDetails.CALL_NOT_ALLOW);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Call_DisconnectedDetails_SIM_INVALID
       * @tc.name    Enum DisconnectedDetails
       * @tc.desc    Enum value test
       */
    it("Telephony_Call_DisconnectedDetails_SIM_INVALID",0,async function(done){
      let caseName = 'Telephony_Call_DisconnectedDetails_SIM_INVALID';
      console.log(`==========> ${caseName} Test start ==========>`);
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(1045).assertEqual(call.DisconnectedDetails.SIM_INVALID);
      expect(1045).assertEqual(call.DisconnectedDetails.SIM_INVALID);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

  });

}