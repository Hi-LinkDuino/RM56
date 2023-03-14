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

import call from '@ohos.telephony.call';
import observer from '@ohos.telephony.observer';
import {describe, afterAll, it, expect, beforeAll, afterEach} from 'deccjsunit/index';
import {
    AUTO_ACCEPT_NUMBER,
    AUTO_ACCEPT_NUMBER2,
    PHONE_NUMBER_LENGTH_11,
    CALL_STATUS_DIALING,
    CALL_ID_NOT_EXIST,
    CALL_STATE_UNKNOWN,
    CALL_STATE_IDLE,
    DEFAULT_SLOT_ID,
    CALL_MODE_IMS,
    SLOT_ID_INVALID,
    PHONE_LIST2,
    RTT_MSG,
    CALL_MODE_SEND_RECEIVE,
    POS_700,
    POS_10,
    POS_LENGTH_300,
    POS_LENGTH_600,
    DEVICE_EARPIECE,
    BOUNDARY_NUMBER_INT,
    SENT_STRING_C,
    MINUS_VALUE,
    PHONE_NUMBER_LONG,
    NULL_PHONE_NUMBER,
    DIAL_CARRIER_TYPE,
    DIAL_SCENCE_CALL_PRIVILEGED,
    MEDIA_TYPE_VOICE,
    ROTATION_MINUS_1,
    IMAGE_LOCAL_ERROR_PATH,
    POS_Z_ERROR,
    RESTRICTION_TYPE_ROAMING_INCOMING,
    RESTRICTION_MODE_ACTIVATION,
    RIGHT_PASSWORD,
    RESTRICTION_TYPE_ALL_INCOMING,
    TRANSFER_TYPE_NOT_REACHABLE,
    TRANSFER_TYPE_UNCONDITIONAL,
    PHONE_NUMBER_AREA_LAND,
    CALL_TRANSFER_DISABLE,
    TRANSFER_TYPE_NO_REPLY,
    COMMAND_CALL_ERROR4,
    COMMAND_CALL_ERROR,
    CARMER_ID_NOT_EXIT,
    ZOOM_RATIO_MINUS_1_0,
    DIAL_SCENCE_CALL_NORMAL,
    DIAL_TYPE_OTT,
    EVENT_OTT_FUNCTION_UNSUPPORTED,
    TEL_CONFERENCE_IDLE
} from './lib/Const.js';
import {toString} from './lib/ApiToPromise.js';
import {
    scenceInCalling,
    hangupCall2,
    hangupCall,
    callDetailsChangeOn,
    callId as gloabCallId,
    reachState,
    callDetailsChangeOff,
    reachCallEventState
} from './lib/ScenceInCalling.js';
const GETMAIN_CALLID_ERRO = -1;
const REJECT_MESSAGE_STR = 'Hi,hello?';
const REJECT_MESSAGE_NUM = 1234567890123456789012345678901234567890;
const ERR_SLOT_ID = -1;
const SLOTID = 0;
const THE_THREE_NUMBER = '112';
const DIAL_TYPE_ERR_CALL = 3;
const TIME_OUT = 20000;
const WAITING_TIME = 200;

let callId = null;
class RejectMessageOptions {
    constructor (str) {
        this.messageContent = str;
    }
}
class EmergencyNumberOptions {
    constructor (num) {
        this.slotId = num;
    }
}
class NumberFormatOptions {
    constructor (str) {
        this.countryCode = str;
    }
}
class CallAttributeOptions {
    constructor (accountNumber, speakerphoneOn, accountId, videoState, startTime,
        isEcc, callType, callId, callState, conferenceState) {
        this.accountNumber = accountNumber;
        this.speakerphoneOn = speakerphoneOn;
        this.accountId = accountId;
        this.videoState = videoState;
        this.startTime = startTime;
        this.isEcc = isEcc;
        this.callType = callType;
        this.callId = callId;
        this.callState = callState;
        this.conferenceState = conferenceState;
    }
}

const sleep = (time) => {
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            resolve();
        }, time);
    });
};

var callState = -1;
var timing = 0;
var endTime = 0;
describe('CallManageImsCall', function () {
    beforeAll(async function (done) {
        observer.on('callStateChange', {slotId: DEFAULT_SLOT_ID}, function (error, data) {});
        try {
            await call.enableImsSwitch(DEFAULT_SLOT_ID);
            console.log('Telephony_CallManager enableImsSwitch success');
            await call.setCallPreferenceMode(DEFAULT_SLOT_ID, CALL_MODE_IMS);
            console.log('Telephony_CallManager setCallPreferenceMode success');
        } catch (error) {
            console.log(`Telephony_CallManager setCallPreferenceMode or enableImsSwitch error,error:${
                toString(error)}`);
        }
        done();
    });

    afterEach(async function () {
        try {
            const CURRENT_TIME = new Date().valueOf();
            const TEMP_CALL_ID = 10;
            const IS_ECC = true;
            const SPEAK_ON = true;
            let callAttributeOptions = new CallAttributeOptions(AUTO_ACCEPT_NUMBER, SPEAK_ON, DEFAULT_SLOT_ID,
                MEDIA_TYPE_VOICE, CURRENT_TIME, IS_ECC, DIAL_TYPE_OTT, TEMP_CALL_ID, CALL_STATUS_DIALING,
                TEL_CONFERENCE_IDLE);
            console.log(`Telephony_CallManager ${toString(callAttributeOptions)}`);
            expect(callAttributeOptions.accountNumber === AUTO_ACCEPT_NUMBER).assertTrue();
            expect(callAttributeOptions.speakerphoneOn === SPEAK_ON).assertTrue();
            expect(callAttributeOptions.accountId === DEFAULT_SLOT_ID).assertTrue();
            expect(callAttributeOptions.videoState === MEDIA_TYPE_VOICE).assertTrue();
            expect(callAttributeOptions.startTime === CURRENT_TIME).assertTrue();
            expect(callAttributeOptions.isEcc === IS_ECC).assertTrue();
            expect(callAttributeOptions.callType === DIAL_TYPE_OTT).assertTrue();
            expect(callAttributeOptions.callId === TEMP_CALL_ID).assertTrue();
            expect(callAttributeOptions.callState === CALL_STATUS_DIALING).assertTrue();
            expect(callAttributeOptions.conferenceState === TEL_CONFERENCE_IDLE).assertTrue();
        } catch (error) {
            console.log(`Telephony_CallManager ${toString(error)}`);
        }

        try {
            let callState = await call.getCallState();
            console.log(`Telephony_CallManager callState ${callState} ${gloabCallId}`);
            if (callState === CALL_STATE_UNKNOWN || callState === CALL_STATE_IDLE) {
                return;
            }
            if (gloabCallId) {
                let data = await hangupCall('Telephony_CallManager', gloabCallId);
                console.log(`Telephony_CallManager hangupCall success ${toString(data)}`);
            }
        } catch (error) {
            console.log('Telephony_CallManager hangupCall or getCallState error');
        }
    });

    afterAll(function () {
        observer.off('callStateChange');
        callDetailsChangeOff();
        console.log('Telephony_CallManager all 54 case is over for callmanager CallManageImsCall');
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_combineConference_Async_0100
     * @tc.name    Run the function combineConference by args callId CALL_ID_NOT_EXIST by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_combineConference_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_combineConference_Async_0100';
        call.combineConference(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} combineConference ${CALL_ID_NOT_EXIST} error,case success,error:${
                    toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} combineConference success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_combineConference_Promise_0100
     * @tc.name    Run the function combineConference by args callId CALL_ID_NOT_EXIST by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_combineConference_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_combineConference_Promise_0100';
        call.combineConference(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} combineConference success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} combineConference ${CALL_ID_NOT_EXIST} error,case success,error:${
                toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_separateConference_Async_0100
     * @tc.name    Run the function separateConference by args callId CALL_ID_NOT_EXIST by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_separateConference_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_separateConference_Async_0100';
        call.separateConference(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} separateConference ${CALL_ID_NOT_EXIST} error,case success,error:${
                    toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} separateConference success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_separateConference_Promise_0100
     * @tc.name    Run the function separateConference by args callId CALL_ID_NOT_EXIST by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_separateConference_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_separateConference_Promise_0100';
        call.separateConference(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} separateConference success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} separateConference ${CALL_ID_NOT_EXIST} error,case success,error:${
                toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_joinConference_Async_0100
     * @tc.name    Run the function joinConference by args mainCallId CALL_ID_NOT_EXIST,
     *             callNumberList PHONE_LIST2 by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_joinConference_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_joinConference_Async_0100';
        call.joinConference(CALL_ID_NOT_EXIST, PHONE_LIST2, (error, data) => {
            if (error) {
                console.log(`${caseName} joinConference ${CALL_ID_NOT_EXIST} error,case success,error:${
                    toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} joinConference success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_joinConference_Promise_0100
     * @tc.name    Run the function joinConference by args mainCallId CALL_ID_NOT_EXIST,
     *             callNumberList PHONE_LIST2 by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_joinConference_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_joinConference_Promise_0100';
        call.joinConference(CALL_ID_NOT_EXIST, PHONE_LIST2).then(data => {
            expect().assertFail();
            console.log(`${caseName} joinConference success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} joinConference ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_enableLteEnhanceMode_Async_0400
     * @tc.name    Run the function enableLteEnhanceMode by args slotId SLOT_ID_INVALID,
     *             by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_enableLteEnhanceMode_Async_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_enableLteEnhanceMode_Async_0400';
        call.enableLteEnhanceMode(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} enableLteEnhanceMode error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} enableLteEnhanceMode success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_enableLteEnhanceMode_Promise_0400
     * @tc.name    Run the function enableLteEnhanceMode by args slotId SLOT_ID_INVALID by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_enableLteEnhanceMode_Promise_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_enableLteEnhanceMode_Promise_0400';
        call.enableLteEnhanceMode(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} enableLteEnhanceMode success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} enableLteEnhanceMode error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_disableLteEnhanceMode_Async_0400
     * @tc.name    Run the function disableLteEnhanceMode by args slotId SLOT_ID_INVALID,
     *             by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_disableLteEnhanceMode_Async_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_disableLteEnhanceMode_Async_0400';
        call.disableLteEnhanceMode(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} disableLteEnhanceMode error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} disableLteEnhanceMode success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_disableLteEnhanceMode_Promise_0400
     * @tc.name    Run the function disableLteEnhanceMode by args slotId SLOT_ID_INVALID by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_disableLteEnhanceMode_Promise_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_disableLteEnhanceMode_Promise_0400';
        call.disableLteEnhanceMode(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} disableLteEnhanceMode success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} disableLteEnhanceMode error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Async_0400
     * @tc.name    Run the function isLteEnhanceModeEnabled by args slotId SLOT_ID_INVALID by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Async_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Async_0400';
        call.isLteEnhanceModeEnabled(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} isLteEnhanceModeEnabled error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} isLteEnhanceModeEnabled success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Promise_0400
     * @tc.name    Run the function isLteEnhanceModeEnabled by args slotId SLOT_ID_INVALID by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Promise_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_isLteEnhanceModeEnabled_Promise_0400';
        call.isLteEnhanceModeEnabled(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} isLteEnhanceModeEnabled success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} isLteEnhanceModeEnabled error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_startRTT_Async_0200
     * @tc.name    Run function startRTT by args callId CALL_ID_NOT_EXIST,msg RTT_MSG by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_startRTT_Async_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_startRTT_Async_0200';
        call.startRTT(CALL_ID_NOT_EXIST, RTT_MSG, (error, data) => {
            if (error) {
                console.log(`${caseName} startRTT ${callId} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} startRTT success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_startRTT_Promise_0200
     * @tc.name    Run function startRTT by args callId CALL_ID_NOT_EXIST,msg RTT_MSG by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_startRTT_Promise_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_startRTT_Promise_0200';
        call.startRTT(CALL_ID_NOT_EXIST, RTT_MSG).then(data => {
            expect().assertFail();
            console.log(`${caseName} startRTT success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} startRTT ${callId} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_stopRTT_Async_0200
     * @tc.name    Run function stopRTT by args callId CALL_ID_NOT_EXIST by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_stopRTT_Async_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_stopRTT_Async_0200';
        call.stopRTT(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} stopRTT ${callId} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} stopRTT success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_stopRTT_Promise_0200
     * @tc.name    Run function stopRTT by args callId CALL_ID_NOT_EXIST by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_stopRTT_Promise_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_stopRTT_Promise_0200';
        call.stopRTT(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} stopRTT success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} stopRTT ${callId} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_updateImsCallMode_Async_0700
     * @tc.name    Run function updateImsCallMode by args callId CALL_ID_NOT_EXIST,
     *             mode CALL_MODE_SEND_RECEIVE by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_updateImsCallMode_Async_0700', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_updateImsCallMode_Async_0700';
        call.updateImsCallMode(CALL_ID_NOT_EXIST, CALL_MODE_SEND_RECEIVE, (error, data) => {
            if (error) {
                console.log(`${caseName} updateImsCallMode2 error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} updateImsCallMode2 success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_updateImsCallMode_Promise_0700
     * @tc.name    Run function updateImsCallMode by args callId CALL_ID_NOT_EXIST,
     *             mode CALL_MODE_SEND_RECEIVE by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_updateImsCallMode_Promise_0700', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_updateImsCallMode_Promise_0700';
        call.updateImsCallMode(CALL_ID_NOT_EXIST, CALL_MODE_SEND_RECEIVE).then(data => {
            expect().assertFail();
            console.log(`${caseName} updateImsCallMode success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} updateImsCallMode error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_holdCall_Async_0100
     * @tc.name    Run the function holdCall by args callId CALL_ID_NOT_EXIST by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_holdCall_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_holdCall_Async_0100';
        call.holdCall(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} holdCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} holdCall success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_holdCall_Promise_0100
     * @tc.name    Run the function holdCall by args callId CALL_ID_NOT_EXIST by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_holdCall_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_holdCall_Promise_0100';
        call.holdCall(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} holdCall success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} holdCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_unHoldCall_Async_0100
     * @tc.name    Run the function unHoldCall by args callId CALL_ID_NOT_EXIST by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_unHoldCall_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_unHoldCall_Async_0100';
        call.unHoldCall(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} unHoldCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} unHoldCall success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_unHoldCall_Promise_0100
     * @tc.name    Run the function unHoldCall by args callId CALL_ID_NOT_EXIST by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_unHoldCall_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_unHoldCall_Promise_0100';
        call.unHoldCall(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} unHoldCall success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} unHoldCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_switchCall_Async_0100
     * @tc.name    Run the function switchCall by args callId CALL_ID_NOT_EXIST by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_switchCall_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_switchCall_Async_0100';
        call.switchCall(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} switchCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} switchCall ${CALL_ID_NOT_EXIST} success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_switchCall_Promise_0100
     * @tc.name    Run the function switchCall by args callId CALL_ID_NOT_EXIST by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_switchCall_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_switchCall_Promise_0100';
        call.switchCall(CALL_ID_NOT_EXIST).then(data => {
            expect().assertFail();
            console.log(`${caseName} switchCall ${CALL_ID_NOT_EXIST} success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} switchCall ${CALL_ID_NOT_EXIST} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_USSD_dial_Async_0600
     * @tc.name    Run the function dial by args phoneNumber COMMAND_CALL_ERROR options
     *             {accountId:DEFAULT_SLOT_ID,videoState:MEDIA_TYPE_VOICE,dialScene:DIAL_SCENCE_CALL_PRIVILEGED,
     *             dialType:DIAL_CARRIER_TYPE} by callback,the function return true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_USSD_dial_Async_0600', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_USSD_dial_Async_0600';
        call.dial(COMMAND_CALL_ERROR, {accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
            dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE}, (error, data) => {
            if (error) {
                expect().assertFail();
                console.log(`${caseName} dial error,case failed,error:${toString(error)}`);
                done();
                return;
            }
            expect(data).assertTrue();
            console.log(`${caseName} dial success,case ${data === true ? 'success' :
                'failed'},data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_USSD_dial_Promise_0600
     * @tc.name    Run the function dial by args phoneNumber COMMAND_CALL_ERROR4 options
     *             {accountId:DEFAULT_SLOT_ID,videoState:MEDIA_TYPE_VOICE,dialScene:DIAL_SCENCE_CALL_PRIVILEGED,
     *             dialType:DIAL_CARRIER_TYPE} by promise,the function return true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_USSD_dial_Promise_0600', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_USSD_dial_Promise_0600';
        call.dial(COMMAND_CALL_ERROR4, {accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
            dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE}).then(data => {
            expect(data).assertTrue();
            console.log(`${caseName} dial success,case ${data === true ? 'success' : 'failed'},data:${toString(data)}`);
            done();
        }).catch(error => {
            expect().assertFail();
            console.log(`${caseName} dial error,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_enableImsSwitch_Async_0400
     * @tc.name    Run the function enableImsSwitch by args callId SLOT_ID_INVALID by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_enableImsSwitch_Async_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_enableImsSwitch_Async_0400';
        call.enableImsSwitch(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} enableImsSwitch error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} enableImsSwitch success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_enableImsSwitch_Promise_0400
     * @tc.name    Run the function enableImsSwitch by args callId SLOT_ID_INVALID by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_enableImsSwitch_Promise_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_enableImsSwitch_Promise_0400';
        call.enableImsSwitch(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} enableImsSwitch success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} enableImsSwitch error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_disableImsSwitch_Async_0600
     * @tc.name    Run the function disableImsSwitch by args callId SLOT_ID_INVALID by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_disableImsSwitch_Async_0600', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_disableImsSwitch_Async_0600';
        call.disableImsSwitch(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} disableImsSwitch error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} disableImsSwitch success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_disableImsSwitch_Promise_0600
     * @tc.name    Run the function disableImsSwitch by args callId SLOT_ID_INVALID by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_disableImsSwitch_Promise_0600', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_disableImsSwitch_Promise_0600';
        call.disableImsSwitch(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} disableImsSwitch success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} disableImsSwitch error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_isImsSwitchEnabled_Async_0500
     * @tc.name    Run the function isImsSwitchEnabled by args callId SLOT_ID_INVALID by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_isImsSwitchEnabled_Async_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_isImsSwitchEnabled_Async_0500';
        call.isImsSwitchEnabled(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} isImsSwitchEnabled error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} isImsSwitchEnabled success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_isImsSwitchEnabled_Promise_0500
     * @tc.name    Run the function isImsSwitchEnabled by args callId SLOT_ID_INVALID by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_isImsSwitchEnabled_Promise_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_isImsSwitchEnabled_Promise_0500';
        call.isImsSwitchEnabled(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} isImsSwitchEnabled success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} isImsSwitchEnabled error,case success,error:${toString(error)}`);
            done();
        });
    });


    /**
     * @tc.number  Telephony_CallManager_IMS_setCallTransfer_Async_1300
     * @tc.name    Run function setCallTransfer by args slotId SLOT_ID_INVALID,settingType CALL_TRANSFER_DISABLE,
     *             type TRANSFER_TYPE_NO_REPLY,transferNum PHONE_NUMBER_LENGTH_11 by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallTransfer_Async_1300', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallTransfer_Async_1300';
        call.setCallTransfer(SLOT_ID_INVALID, {
            settingType:CALL_TRANSFER_DISABLE,
            type:TRANSFER_TYPE_NO_REPLY,
            transferNum:PHONE_NUMBER_LENGTH_11
        }, (error, data) => {
            if (error) {
                console.log(`${caseName} case success, error:${toString(error)}`);
                done();
                return;
            }
            console.log(`${caseName} case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_setCallTransfer_Promise_1300
     * @tc.name    Run function setCallTransfer by args slotId SLOT_ID_INVALID,settingType CALL_TRANSFER_DISABLE,
     *             type TRANSFER_TYPE_UNCONDITIONAL,transferNum PHONE_NUMBER_AREA_LAND by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallTransfer_Promise_1300', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallTransfer_Promise_1300';
        try {
            let data = await call.setCallTransfer(SLOT_ID_INVALID, {
                settingType:CALL_TRANSFER_DISABLE,
                type:TRANSFER_TYPE_UNCONDITIONAL,
                transferNum:PHONE_NUMBER_AREA_LAND
            });
            console.log(`${caseName} case failed,data:${toString(data)}`);
            expect().assertFail();
        } catch (err) {
            console.log(`${caseName} case success, err:${toString(err)}`);
        }
        done();
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_getCallTransferInfo_Async_1500
     * @tc.name    Run function getCallTransferInfo by args slotId SLOT_ID_INVALID,
     *             type TRANSFER_TYPE_UNCONDITIONAL  by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallTransferInfo_Async_1500', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallTransferInfo_Async_1500';
        call.getCallTransferInfo(SLOT_ID_INVALID, TRANSFER_TYPE_UNCONDITIONAL, (error, data) => {
            if (error) {
                console.log(`${caseName} getCallTransferInfo error, case success. error:${toString(error)}`);
                done();
                return;
            }
            console.log(`${caseName} getCallTransferInfo success,case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_getCallTransferInfo_Promise_1500
     * @tc.name    Run function getCallTransferInfo by args slotId SLOT_ID_INVALID,
     *             type TRANSFER_TYPE_NOT_REACHABLE by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallTransferInfo_Promise_1500', 0, async function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallTransferInfo_Promise_1500';
        try {
            var data = await call.getCallTransferInfo(SLOT_ID_INVALID, TRANSFER_TYPE_NOT_REACHABLE);
            console.log(`${caseName} case failed,data:${toString(data)}`);
            expect().assertFail();
        } catch (err) {
            console.log(`${caseName} case success. error:${toString(err)}`);
        }
        done();
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_setCallWaiting_Async_0700
     * @tc.name    Run the function setCallWaiting by args slotId SLOT_ID_INVALID activate true by callback to open,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallWaiting_Async_0700', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallWaiting_Async_0700';
        call.setCallWaiting(SLOT_ID_INVALID, true, (error, data) => {
            if (error) {
                console.log(`${caseName} setCallWaiting error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} setCallWaiting success,case failed,data:${toString(data)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_setCallWaiting_Promise_0700
     * @tc.name    Run the function setCallWaiting by args slotId SLOT_ID_INVALID activate true by promise to open,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallWaiting_Promise_0700', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallWaiting_Promise_0700';
        call.setCallWaiting(SLOT_ID_INVALID, true).then(data => {
            expect().assertFail();
            console.log(`${caseName} setCallWaiting success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} setCallWaiting error,case success,error:${toString(error)}`);
            done();
        });
    });


    /**
     * @tc.number  Telephony_CallManager_IMS_setCallRestriction_Async_1200
     * @tc.name    Run function setCallRestriction by callback
     *             by arg slotId SLOT_ID_INVALID type RESTRICTION_TYPE_ALL_INCOMING password RIGHT_PASSWORD
     *             mode RESTRICTION_MODE_ACTIVATION,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallRestriction_Async_1200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallRestriction_Async_1200';
        call.setCallRestriction(SLOT_ID_INVALID, {
            type:RESTRICTION_TYPE_ALL_INCOMING,
            password:RIGHT_PASSWORD,
            mode:RESTRICTION_MODE_ACTIVATION
        }, (error, data) => {
            if (error) {
                console.log(`${caseName} success,case ${error ? 'success' : 'failed'},error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
                return;
            }
            console.log(`${caseName} error,case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_setCallRestriction_Promise_1200
     * @tc.name    Run function setCallRestriction by promise
     *             by arg slotId SLOT_ID_INVALID type RESTRICTION_TYPE_ALL_INCOMING password RIGHT_PASSWORD
     *             mode RESTRICTION_MODE_ACTIVATION,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_setCallRestriction_Promise_1200', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_setCallRestriction_Promise_1200';
        call.setCallRestriction(SLOT_ID_INVALID, {
            type:RESTRICTION_TYPE_ALL_INCOMING,
            password:RIGHT_PASSWORD,
            mode:RESTRICTION_MODE_ACTIVATION
        })
            .then(data => {
                console.log(`${caseName} error,case failed,data:${toString(data)}`);
                expect().assertFail();
                done();
            })
            .catch((error) => {
                console.log(`${caseName} success,case ${error ? 'success' : 'failed'},error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_getCallRestrictionStatus_Async_1100
     * @tc.name    Run function getCallRestrictionStatus by callback by arg slotId
     *             that is SLOT_ID_INVALID CallRestrictionType RESTRICTION_TYPE_ROAMING_INCOMING ,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallRestrictionStatus_Async_1100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallRestrictionStatus_Async_1100';
        call.getCallRestrictionStatus(SLOT_ID_INVALID, RESTRICTION_TYPE_ROAMING_INCOMING, (error, data) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            console.log(`${caseName} success,case faild,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_getCallRestrictionStatus_Promise_1100
     * @tc.name    Run function getCallRestrictionStatus by promise by arg slotId (that is at SLOT_ID_INVALID)
     *             CallRestrictionType RESTRICTION_TYPE_ROAMING_INCOMING ,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallRestrictionStatus_Promise_1100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallRestrictionStatus_Promise_1100';
        call.getCallRestrictionStatus(SLOT_ID_INVALID, RESTRICTION_TYPE_ROAMING_INCOMING)
            .then(data => {
                console.log(`${caseName} success,case failed,data:${toString(data)}`);
                expect().assertFail();
                done();
            })
            .catch(error => {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_getCallWaitingStatus_Async_0500
     * @tc.name    Run the function getCallWaitingStatus by args slotId SLOT_ID_INVALID by callback,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallWaitingStatus_Async_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallWaitingStatus_Async_0500';
        call.getCallWaitingStatus(SLOT_ID_INVALID, (error, data) => {
            if (error) {
                console.log(`${caseName} getCallWaitingStatus error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} getCallWaitingStatus success,case failed,data:${toString(data)}`);
            done();
        });
    });


    /**
     * @tc.number  Telephony_CallManager_IMS_getCallWaitingStatus_Promise_0500
     * @tc.name    Run the function getCallWaitingStatus by args slotId SLOT_ID_INVALID by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_getCallWaitingStatus_Promise_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_getCallWaitingStatus_Promise_0500';
        call.getCallWaitingStatus(SLOT_ID_INVALID).then(data => {
            expect().assertFail();
            console.log(`${caseName} getCallWaitingStatus success,case failed,data:${toString(data)}`);
            done();
        }).catch(error => {
            console.log(`${caseName} getCallWaitingStatus error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_controlCamera_Async_0200
     * @tc.name    Dial a call and after answering the call,run function controlCamera by
     *             args cameraId CARMER_ID_NOT_EXIT by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_controlCamera_Async_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_controlCamera_Async_0200';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:AUTO_ACCEPT_NUMBER2,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            let cameraId = CARMER_ID_NOT_EXIT;
            call.controlCamera(cameraId, (error) => {
                if (error) {
                    console.log(`${caseName} error,case success,error:${toString(error)}`);
                    hangupCall2(caseName, done, callId);
                    return;
                }
                console.log(`${caseName} case failed`);
                expect().assertFail();
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_controlCamera_Promise_0200
     * @tc.name    Dial a call and after answering the call,run function controlCamera by args cameraId
     *             CARMER_ID_NOT_EXIT  by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_controlCamera_Promise_0200', 0, async function (done) {
        let caseName = 'Telephony_CallManager_controlCamera_Promise_0200';
        let cameraId = CARMER_ID_NOT_EXIT;
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER2,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            await call.controlCamera(cameraId);
            console.log(`${caseName} case failed`);
            expect().assertFail();
        } catch (err) {
            console.log(`${caseName} case success. error:${toString(err)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setPreviewWindow_Async_0300
     * @tc.name    Dial a call and after answering the call,run function setPreviewWindow by args
     *             x POS_700,y POS_10,z POS_Z_ERROR,width POS_LENGTH_300,height  POS_LENGTH_600 by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setPreviewWindow_Async_0300', 0, function (done) {
        let caseName = 'Telephony_CallManager_setPreviewWindow_Async_0300';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:AUTO_ACCEPT_NUMBER,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            let info = {x: POS_700, y: POS_10, z: POS_Z_ERROR, width: POS_LENGTH_300, height:  POS_LENGTH_600};
            call.setPreviewWindow(info, (error) => {
                if (error) {
                    console.log(`${caseName} error,case success,error:${toString(error)}`);
                    hangupCall2(caseName, done, callId);
                    return;
                }
                console.log(`${caseName} case faild`);
                expect().assertFail();
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setPreviewWindow_Promise_0300
     * @tc.name    Dial a call and after answering the call,run function setPreviewWindow by args
     *             x POS_700,y POS_10,z POS_Z_ERROR,width POS_LENGTH_300,height  POS_LENGTH_600 by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setPreviewWindow_Promise_0300', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setPreviewWindow_Promise_0300';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            let info = {x: POS_700, y: POS_10, z: POS_Z_ERROR, width: POS_LENGTH_300, height:  POS_LENGTH_600};
            await call.setPreviewWindow(info);
            console.log(`${caseName} case faild`);
            expect().assertFail();
        } catch (error) {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setDisplayWindow_Async_0300
     * @tc.name    Dial a call and after answering the call,run function setDisplayWindow by args
     *             x POS_700,y POS_10,z POS_Z_ERROR,width POS_LENGTH_300,height  POS_LENGTH_600 by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setDisplayWindow_Async_0300', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setDisplayWindow_Async_0300';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            let info = {x: POS_700, y: POS_10, z: POS_Z_ERROR, width: POS_LENGTH_300, height:  POS_LENGTH_600};
            await call.setDisplayWindow(info);
            console.log(`${caseName} case faild`);
            expect().assertFail();
        } catch (error) {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setDisplayWindow_Promise_0300
     * @tc.name    Dial a call and after answering the call,run function setDisplayWindow by args
     *             x POS_700,y POS_10,z POS_Z_ERROR,width POS_LENGTH_300,height  POS_LENGTH_600 by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setDisplayWindow_Promise_0300', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setDisplayWindow_Promise_0300';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            let info = {x: POS_700, y: POS_10, z: POS_Z_ERROR, width: POS_LENGTH_300, height:  POS_LENGTH_600};
            await call.setDisplayWindow(info);
            console.log(`${caseName} case faild`);
            expect().assertFail();
        } catch (error) {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setCameraZoom_Async_0400
     * @tc.name    Dial a call and after answering the call,run function setCameraZoom by args
     *             zoomRatio ZOOM_RATIO_MINUS_1_0 by callback,the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setCameraZoom_Async_0400', 0, function (done) {
        let caseName = 'Telephony_CallManager_setCameraZoom_Async_0400';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:AUTO_ACCEPT_NUMBER2,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            call.setCameraZoom(ZOOM_RATIO_MINUS_1_0, (error) => {
                if (error) {
                    console.log(`${caseName} error,case success,error:${toString(error)}`);
                    hangupCall2(caseName, done, callId);
                    return;
                }
                console.log(`${caseName} case failed`);
                expect().assertFail();
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setCameraZoom_Promise_0400
     * @tc.name    Dial a call and after answering the call,run function setCameraZoom by args
     *             zoomRatio ZOOM_RATIO_MINUS_1_0 by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setCameraZoom_Promise_0400', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setCameraZoom_Promise_0400';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            await call.setCameraZoom(ZOOM_RATIO_MINUS_1_0);
            console.log(`${caseName} case failed`);
            expect().assertFail();
        } catch (error) {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setPausePicture_Async_0500
     * @tc.name    Dial a call and after answering the call,run function setPausePicture by args
     *             path IMAGE_LOCAL_ERROR_PATH by callback,the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setPausePicture_Async_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_setPausePicture_Async_0500';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:AUTO_ACCEPT_NUMBER,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            call.setPausePicture(IMAGE_LOCAL_ERROR_PATH, (error) => {
                if (error) {
                    console.log(`${caseName} case success,error:${toString(error)}`);
                    hangupCall2(caseName, done, callId);
                    return;
                }
                console.log(`${caseName} success,case failed`);
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setPausePicture_Promise_0500
     * @tc.name    Dial a call and after answering the call,run function setPausePicture
     *             by args path IMAGE_LOCAL_ERROR_PATH by promise,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setPausePicture_Promise_0500', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setPausePicture_Promise_0500';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            await call.setPausePicture(IMAGE_LOCAL_ERROR_PATH);
            console.log(`${caseName} success,case failed`);
        } catch (error) {
            console.log(`${caseName} case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_setDeviceDirection_Async_0500
     * @tc.name    Dial a call and after answering the call,run function setDeviceDirection
     *             by args rotation ROTATION_MINUS_1 by callback,the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setDeviceDirection_Async_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_setDeviceDirection_Async_0500';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:AUTO_ACCEPT_NUMBER,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            call.setDeviceDirection(ROTATION_MINUS_1, (error) => {
                if (error) {
                    console.log(`${caseName} success, case success`);
                    hangupCall2(caseName, done, callId);
                    return;
                }
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setDeviceDirection_Promise_0500
     * @tc.name    Dial a call and after answering the call,run function setDeviceDirection
     *             by args rotation ROTATION_MINUS_1 by promise,the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setDeviceDirection_Promise_0500', 0, async function (done) {
        let caseName = 'Telephony_CallManager_setDeviceDirection_Promise_0500';
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER2,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
        } catch (error) {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
            return;
        }
        try {
            await call.setDeviceDirection(ROTATION_MINUS_1);
            console.log(`${caseName} success,case error`);
            expect().assertFail();
        } catch (error) {
            console.log(`${caseName} case success,error:${toString(error)}`);
        }
        hangupCall2(caseName, done, callId);
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_dial_Async_0600
     * @tc.name    Dial a call by function dial by args phoneNumber NULL_PHONE_NUMBER options
     *             {accountId: DEFAULT_SLOT_ID, videoState: MEDIA_TYPE_VOICE, dialScene: DIAL_SCENCE_CALL_PRIVILEGED,
     *             dialType:DIAL_CARRIER_TYPE} by callback,the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_dial_Async_0600', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_dial_Async_0600';
        let flag = true;
        reachState(caseName, CALL_STATUS_DIALING, '', false).then(data => {
            callId = data.callId;
            console.log(`${caseName} reachState success,case failed,data:${toString(data)}`);
            expect().assertFail();
            hangupCall2(caseName, done, callId);
        }).catch(error => {
            console.log(`${caseName} reachState error,case ${flag ? 'success' : 'failed'},error:${toString(error)}`);
            expect(flag).assertTrue();
            done();
        });
        let obj = {accountId: DEFAULT_SLOT_ID, videoState: MEDIA_TYPE_VOICE,
            dialScene: DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_CARRIER_TYPE};
        call.dial(NULL_PHONE_NUMBER, obj, (error, data) => {
            if (error) {
                console.log(`${caseName} dial error,error:${toString(error)}`);
                flag = true;
                return;
            }
            flag = false;
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_dial_Promise_0100
     * @tc.name    Dial a call by function dial by args phoneNumber PHONE_NUMBER_LONG by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_dial_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_dial_Promise_0100';
        let flag = true;
        reachState(caseName, CALL_STATUS_DIALING, '', false).then(data => {
            callId = data.callId;
            console.log(`${caseName} reachState success,case failed,data:${toString(data)}`);
            expect().assertFail();
            hangupCall2(caseName, done, callId);
        }).catch(error => {
            console.log(`${caseName} reachState error,case ${flag ? 'success' : 'failed'},error:${toString(error)}`);
            expect(flag).assertTrue();
            done();
        });
        call.dial(PHONE_NUMBER_LONG).then(data => {
            flag = false;
        }).catch(error => {
            console.log(`${caseName} dial error,error:${toString(error)}`);
            flag = true;
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_answer_Async_0100
     * @tc.name    Run function answer by args callId BOUNDARY_NUMBER_INT by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_answer_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_answer_Async_0100';
        call.answer(BOUNDARY_NUMBER_INT, (error) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_answer_Promise_0100
     * @tc.name    Run function answer by args callId CALL_ID_NOT_EXIST by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_answer_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_answer_Promise_0100';
        call.answer(CALL_ID_NOT_EXIST).then(() => {
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        }).catch(error => {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_reject_Async_0100
     * @tc.name    Run function reject by args callId BOUNDARY_NUMBER_INT by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_reject_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_reject_Async_0100';
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
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_reject_Promise_0100
     * @tc.name    Run function reject by args callId CALL_ID_NOT_EXIST by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_reject_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_reject_Promise_0100';
        call.reject(CALL_ID_NOT_EXIST).then(() => {
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        }).catch(error => {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_reject_Promise_0400
     * @tc.name    Test the function runs 10 times, stability test
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_reject_Promise_0400', 0, async function (done) {
        let caseName = 'Telephony_CallManager_reject_Promise_0400';
        let callId = null;
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
            try {
                data = await call.reject(callId, new RejectMessageOptions(REJECT_MESSAGE_STR));
                console.log(`${caseName} reject success,case failed,data:${toString(data)}`);
                done();
            } catch (err) {
                console.log(`${caseName} reject error,case success,err:${toString(err)}`);
                done();
            }
        } catch (error) {
            console.log(`${caseName} scenceInCalling error,case failed,error:${toString(error)}`);
            done();
        }
    });

    /**
     * @tc.number  Telephony_CallManager_reject_Async_0400
     * @tc.name    Test the function runs 10 times, stability test
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_reject_Async_0400', 0, async function (done) {
        let caseName = 'Telephony_CallManager_reject_Async_0400';
        let callId = null;
        try {
            let data = await scenceInCalling({
                caseName:caseName,
                phoneNumber:AUTO_ACCEPT_NUMBER,
                checkState:CALL_STATUS_DIALING
            });
            callId = data.callId;
            call.reject(callId, new RejectMessageOptions(REJECT_MESSAGE_NUM), (err, data) => {
                if (err) {
                    console.log(`${caseName} reject error,case success,err:${toString(err)}`);
                    done();
                    return;
                }
                console.log(`${caseName} reject success,case failed,data:${toString(data)}`);
                done();
            });
        } catch (error) {
            console.log(`${caseName} scenceInCalling error,case failed,error:${toString(error)}`);
            done();
        }
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_hangup_Async_0100
     * @tc.name    Run function hangup by args callId BOUNDARY_NUMBER_INT by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_hangup_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_hangup_Async_0100';
        call.hangup(BOUNDARY_NUMBER_INT, (error) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_hangup_Promise_0100
     * @tc.name    Run function hangup by args callId CALL_ID_NOT_EXIST by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_hangup_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_hangup_Promise_0100';
        call.hangup(CALL_ID_NOT_EXIST).then(() => {
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        }).catch(error => {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_startDTMF_Async_1300
     * @tc.name    Run function startDTMF by args callId MINUS_VALUE  character C by callback,
     *             the callback function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_startDTMF_Async_1300', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_startDTMF_Async_1300';
        call.startDTMF(MINUS_VALUE, SENT_STRING_C, (error) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            expect().assertFail();
            console.log(`${caseName} success,case failed`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_startDTMF_Promise_1300
     * @tc.name    Dial a call by function dial by args phoneNumber PHONE_NUMBER_LENGTH_11 when before being accepted,
     *             run function startDTMF by args callId BOUNDARY_NUMBER_INT character C by promise,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_startDTMF_Promise_1300', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_startDTMF_Promise_1300';
        scenceInCalling({
            caseName:caseName,
            phoneNumber:PHONE_NUMBER_LENGTH_11,
            checkState:CALL_STATUS_DIALING
        }).then(data => {
            callId = data.callId;
            call.startDTMF(BOUNDARY_NUMBER_INT, SENT_STRING_C).then(data => {
                console.log(`${caseName} success,case error,data:${toString(data)}`);
                expect().assertFail();
                hangupCall2(caseName, done, callId);
            }).catch(error => {
                console.log(`${caseName} case success`);
                hangupCall2(caseName, done, callId);
            });
        }).catch(error => {
            console.log(`${caseName} scenceInCalling error ,case failed,error:${toString(error)}`);
            done();
        });
    });

    /**
      * @tc.number  Telephony_CallManager_IMS_stopDTMF_Async_0100
      * @tc.name    Run function stopDTMF by args callId CALL_ID_NOT_EXIST  by callback,the function return error
      * @tc.desc    Function test
      */
    it('Telephony_CallManager_IMS_stopDTMF_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_stopDTMF_Async_0100';
        call.stopDTMF(CALL_ID_NOT_EXIST, (error, data) => {
            if (error) {
                console.log(`${caseName} case success`);
                done();
                return;
            }
            console.log(`${caseName} success,case error,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_stopDTMF_Promise_0100
     * @tc.name    Run function stopDTMF by args callId BOUNDARY_NUMBER_INT by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_stopDTMF_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_stopDTMF_Promise_0100';
        call.stopDTMF(BOUNDARY_NUMBER_INT).then(data => {
            console.log(`${caseName} success,case error,data:${toString(data)}`);
            expect().assertFail();
            done();
        }).catch(error => {
            console.log(`${caseName} error,case success,error:${toString(error)}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_muteRinger_Async_0100
     * @tc.name    Unwanted state,run function muteRinger by callback,the function return void
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_muteRinger_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_muteRinger_Async_0100';
        call.muteRinger((error, data) => {
            if (error) {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
                return;
            }
            console.log(`${caseName} success,case success`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_muteRinger_Promise_0100
     * @tc.name    Unwanted state,run function muteRinger by promise,the function return void
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_muteRinger_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_muteRinger_Promise_0100';
        call.muteRinger()
            .then(data => {
                console.log(`${caseName} success,case success`);
                done();
            })
            .catch((error) => {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_setAudioDevice_Async_0500
     * @tc.name    Unwanted state,run function setAudioDevice by callback and by arg DEVICE_EARPIECE,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setAudioDevice_Async_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_setAudioDevice_Async_0500';
        call.setAudioDevice(DEVICE_EARPIECE, (error, data) => {
            if (error) {
                console.log(`${caseName} success,case ${error ? 'success' : 'failed'},error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
                return;
            }
            console.log(`${caseName} error,case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setAudioDevice_Promise_0500
     * @tc.name    Unwanted state,run function setAudioDevice by promise and by arg DEVICE_EARPIECE,
     *             the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setAudioDevice_Promise_0500', 0, function (done) {
        let caseName = 'Telephony_CallManager_setAudioDevice_Promise_0500';
        call.setAudioDevice(DEVICE_EARPIECE)
            .then(data => {
                console.log(`${caseName} error,case failed,data:${toString(data)}`);
                expect().assertFail();
                done();
            })
            .catch(error => {
                console.log(`${caseName} success,case ${error ? 'success' : 'failed'},error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_setMuted_Async_0300
     * @tc.name    Unwanted state,run function setMuted by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setMuted_Async_0300', 0, function (done) {
        let caseName = 'Telephony_CallManager_setMuted_Async_0300';
        call.setMuted((error, data) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
                return;
            }
            console.log(`${caseName} success,case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_setMuted_Promise_0300
     * @tc.name    Unwanted state,run function setMuted by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_setMuted_Promise_0300', 0, function (done) {
        let caseName = 'Telephony_CallManager_setMuted_Promise_0300';
        call.setMuted()
            .then(data => {
                console.log(`${caseName} success,case failed,data:${toString(data)}`);
                expect().assertFail();
                done();
            })
            .catch(error => {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                expect(!!error).assertTrue();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_cancelMuted_Async_0300
     * @tc.name    Unwanted state,run function cancelMuted by callback,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_cancelMuted_Async_0300', 0, function (done) {
        let caseName = 'Telephony_CallManager_cancelMuted_Async_0300';
        call.cancelMuted((error, data) => {
            if (error) {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
                return;
            }
            console.log(`${caseName} success,case failed,data:${toString(data)}`);
            expect().assertFail();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_cancelMuted_Promise_0300
     * @tc.name    Unwanted state,run function cancelMuted by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_cancelMuted_Promise_0300', 0, function (done) {
        let caseName = 'Telephony_CallManager_cancelMuted_Promise_0300';
        call.cancelMuted()
            .then(data => {
                console.log(`${caseName} success,case failed,data:${toString(data)}`);
                expect().assertFail();
                done();
            })
            .catch(error => {
                console.log(`${caseName} error,case success,error:${toString(error)}`);
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_isNewCallAllowed_Async_0200
     * @tc.name    Unwanted state ,run the function isNewCallAllowed by callback ,the function return true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isNewCallAllowed_Async_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_isNewCallAllowed_Async_0200';
        call.isNewCallAllowed(function (error, data) {
            if (error) {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
                return;
            }
            console.log(`${caseName} success,case ${data === true ? 'success' : 'failed'},data:${toString(data)}`);
            expect(data === true).assertTrue();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_isNewCallAllowed_Promise_0200
     * @tc.name    Unwanted state ,run the function isNewCallAllowed by promise ,the function return true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isNewCallAllowed_Promise_0200', 0, function (done) {
        let caseName = 'Telephony_CallManager_isNewCallAllowed_Promise_0200';
        call.isNewCallAllowed()
            .then(data => {
                console.log(`${caseName} success,case ${data === true ? 'success' : 'failed'},data:${toString(data)}`);
                expect(data === true).assertTrue();
                done();
            })
            .catch(error => {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_isRinging_Async_0100
     * @tc.name    Unwanted state ,run the function isRinging by callback ,the function return false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isRinging_Async_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_isRinging_Async_0100';
        call.isRinging(function (error, data) {
            if (error) {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
                return;
            }
            console.log(`${caseName} success,case ${data === false ? 'success' : 'failed'},data:${toString(data)}`);
            expect(data === false).assertTrue();
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_isRinging_Promise_0100
     * @tc.name    Unwanted state ,run the function isRinging by promise ,the function return false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isRinging_Promise_0100', 0, function (done) {
        let caseName = 'Telephony_CallManager_isRinging_Promise_0100';
        call.isRinging()
            .then(data => {
                console.log(`${caseName} success,case ${data === false ? 'success' : 'failed'},data:${toString(data)}`);
                expect(data === false).assertTrue();
                done();
            })
            .catch(error => {
                console.log(`${caseName} error,case failed,error:${toString(error)}`);
                expect().assertFail();
                done();
            });
    });

    /**
     * @tc.number  Telephony_CallManager_getCallState_Async_0100
     * @tc.name    To get the idle call status, call getCallState() to get the current call status.
     *             call.CALL_STATE_IDLE is returned
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Async_0100', 0, async function (done) {
        call.getCallState((err, data) => {
            if (err) {
                console.log(`Telephony_CallManager_getCallState_Async_0100 : err = ${err.message}`);
                expect().assertFail();
                done();
                return;
            }
            expect(data === call.CALL_STATE_IDLE).assertTrue();
            console.log(`Telephony_CallManager_getCallState_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_getCallState_Promise_0100
     * @tc.name    To get the idle call status, call getCallState() to get the current call status.
     *             call.CALL_STATE_IDLE is returned
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallState_Promise_0100', 0, async function (done) {
        try {
            var data = await call.getCallState();
            expect(data === call.CALL_STATE_IDLE).assertTrue();
            console.log(`Telephony_CallManager_getCallState_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            console.log(`Telephony_CallManager_getCallState_Promise_0100 : err = ${err.message}`);
            expect().assertFail();
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_hasCall_Async_0400
     * @tc.name    When idle, hasCall() is called to confirm that there is no current call,returning false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_hasCall_Async_0400', 0, async function (done) {
        call.hasCall((err, data) => {
            if (err) {
                console.log('Telephony_CallManager_hasCall_Async_0400 fail');
                expect().assertFail();
                done();
                return;
            }
            expect(data === false).assertTrue();
            console.log(`Telephony_CallManager_hasCall_Async_0400 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_hasCall_Promise_0400
     * @tc.name    When idle, hasCall() is called to confirm that there is no current call, returning false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_hasCall_Promise_0400', 0, async function (done) {
        try {
            var data = await call.hasCall();
            expect(data === false).assertTrue();
            console.log(`Telephony_CallManager_hasCall_Promise_0400 finish data = ${data}`);
            done();
        } catch (err) {
            console.log('Telephony_CallManager_hasCall_Promise_0400 fail');
            expect().assertFail();
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_getSubCallIdList_Async_0100
     * @tc.name    If callId is 999, call getSubCallIdList() to get the call list of subcalls and return the empty list
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getSubCallIdList_Async_0100', 0, async function (done) {
        call.getSubCallIdList(CALL_ID_NOT_EXIST, (err, data) => {
            if (err) {
                expect().assertFail();
                console.log('Telephony_CallManager_getSubCallIdList_Async_0100 fail');
                done();
                return;
            }
            expect(data.length === 0).assertTrue();
            console.log(`Telephony_CallManager_getSubCallIdList_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_getSubCallIdList_Promise_0100
     * @tc.name    If callId is 999, call getSubCallIdList() to get the sub-call list
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getSubCallIdList_Promise_0100', 0, async function (done) {
        try {
            var data = await call.getSubCallIdList(CALL_ID_NOT_EXIST);
            expect(data.length === 0).assertTrue();
            console.log(`Telephony_CallManager_getSubCallIdList_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            console.log('Telephony_CallManager_getSubCallIdList_Promise_0100 fail');
            expect().assertFail();
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_getCallIdListForConference_Async_0100
     * @tc.name    CallId for 999, the callback call getCallIdListForConference () to obtain a list all the phone call
     *             in the conference call, returns an empty list
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallIdListForConference_Async_0100', 0, async function (done) {
        call.getCallIdListForConference(CALL_ID_NOT_EXIST, (err, data) => {
            if (err) {
                console.log('Telephony_CallManager_getCallIdListForConference_Async_0100 fail');
                expect().assertFail();
                done();
                return;
            }
            expect(data.length === 0).assertTrue();
            console.log(`Telephony_CallManager_getCallIdListForConference_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_getCallIdListForConference_Promise_0100
     * @tc.name    GetCallIdListForConference callId for 999, call () to obtain a list all the phone call in the
     *             conference call, returns an empty list
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getCallIdListForConference_Promise_0100', 0, async function (done) {
        try {
            var data = await call.getCallIdListForConference(CALL_ID_NOT_EXIST);
            expect(data.length === 0).assertTrue();
            console.log(`Telephony_CallManager_getCallIdListForConference_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            console.log('Telephony_CallManager_getCallIdListForConference_Promise_0100 fail');
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_getMainCallId_Async_0100
     * @tc.name    If the mainCallId is 999, call getMainCallId() to get the call and return -1
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getMainCallId_Async_0100', 0, async function (done) {
        call.getMainCallId(CALL_ID_NOT_EXIST, (err, data) => {
            if (err) {
                expect().assertFail();
                console.log('Telephony_CallManager_getMainCallId_Async_0100 fail');
                done();
                return;
            }
            expect(data === GETMAIN_CALLID_ERRO).assertTrue();
            console.log(`Telephony_CallManager_getMainCallId_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_getMainCallId_Promise_0100
     * @tc.name    If mainCallId is 999, call getMainCallId() to get the call and return -1
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_getMainCallId_Promise_0100', 0, async function (done) {
        try {
            var data = await call.getMainCallId(CALL_ID_NOT_EXIST);
            expect(data === GETMAIN_CALLID_ERRO).assertTrue();
            console.log(`Telephony_CallManager_getMainCallId_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            expect().assertFail();
            console.log(`Telephony_CallManager_getMainCallId_Promise_0100 finish data = ${data}`);
            done();
        }
    });

    /**
     * @tc.number  Telephony_CallManager_formatPhoneNumber_Async_0100
     * @tc.name    PhoneNumber is 100000000000. Call formatPhoneNumber() to format the number.
     *             The return value is 10 000 000 0000
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_formatPhoneNumber_Async_0100', 0, async function (done) {
        call.formatPhoneNumber('100000000000', (err, data) => {
            if (err) {
                console.log(`Telephony_CallManager_formatPhoneNumber_Async_0100 err = ${err.message}`);
                console.log('Telephony_CallManager_formatPhoneNumber_Async_0100 fail');
                expect().assertFail();
                done();
                return;
            }
            expect(data === '10 000 000 0000').assertTrue();
            console.log(`Telephony_CallManager_formatPhoneNumber_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_formatPhoneNumber_Promise_0100
     * @tc.name    PhoneNumber is 2000000. Call formatPhoneNumber() to format the number.
     *             The return value is 200 0000
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_formatPhoneNumber_Promise_0100', 0, async function (done) {
        try {
            var data = await call.formatPhoneNumber('2000000');
            expect(data === '200 0000').assertTrue();
            console.log(`Telephony_CallManager_formatPhoneNumber_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            console.log('Telephony_CallManager_formatPhoneNumber_Promise_0100 fail');
            expect().assertFail();
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_formatPhoneNumberToE164_Async_0100
     * @tc.name    PhoneNumber is 010-0000-0000, options: CN, call formatPhoneNumberToE164() to format the number,
     *             and return +861000000000
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_formatPhoneNumberToE164_Async_0100', 0, async function (done) {
        let numberFormatOptions = new NumberFormatOptions('CN');
        call.formatPhoneNumberToE164('010-0000-0000', numberFormatOptions.countryCode, (err, data) => {
            if (err) {
                console.log('Telephony_CallManager_formatPhoneNumberToE164_Async_0100 fail');
                expect().assertFail();
                done();
                return;
            }
            expect(data === '+861000000000').assertTrue();
            console.log(`Telephony_CallManager_formatPhoneNumberToE164_Async_0100 finish data = ${data}`);
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_formatPhoneNumberToE164_Promise_0100
     * @tc.name    PhoneNumber is 52300000000, options: CN, call formatPhoneNumberToE164() to format the number,
     *             return +8652300000000
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_formatPhoneNumberToE164_Promise_0100', 0, async function (done) {
        let numberFormatOptions = new NumberFormatOptions('CN');
        try {
            var data = await call.formatPhoneNumberToE164('52300000000', numberFormatOptions.countryCode);
            expect(data === '+8652300000000').assertTrue();
            console.log(`Telephony_CallManager_formatPhoneNumberToE164_Promise_0100 finish data = ${data}`);
            done();
        } catch (err) {
            console.log('Telephony_CallManager_formatPhoneNumberToE164_Promise_0100 fail');
            expect().assertFail();
            done();

        }
    });

    /**
     * @tc.number  Telephony_CallManager_isEmergencyPhoneNumber_Async_1300
     * @tc.name    PhoneNumber: 110, options -1. Call isEmergencyPhoneNumber() to check whether it is an emergency
     *             number. The return value is false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isEmergencyPhoneNumber_Async_1300', 0, async function (done) {
        let emergencyNumberOptions = new EmergencyNumberOptions(ERR_SLOT_ID);
        call.isEmergencyPhoneNumber('110', emergencyNumberOptions, (err) => {
            if (err) {
                console.log(`Telephony_CallManager_isEmergencyPhoneNumber_Async_1300 finish err = ${err}`);
                done();
                return;
            }
            expect().assertFail();
            console.log('Telephony_CallManager_isEmergencyPhoneNumber_Async_1300 fail ');
            done();
        });
    });

    /**
     * @tc.number  Telephony_CallManager_isEmergencyPhoneNumber_Promise_1300
     * @tc.name    PhoneNumber: 120, options -1. Call isEmergencyPhoneNumber() to check whether it is an emergency
     *             number. The return value is false
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isEmergencyPhoneNumber_Promise_1300', 0, async function (done) {
        let emergencyNumberOptions = new EmergencyNumberOptions(ERR_SLOT_ID);
        try {
            var data = await call.isEmergencyPhoneNumber('120', emergencyNumberOptions);
            expect().assertFail();
            console.log('Telephony_CallManager_isEmergencyPhoneNumber_Promise_1300 fail ');
            done();
        } catch (err) {
            console.log(`Telephony_CallManager_isEmergencyPhoneNumber_Promise_1300 finish err = ${err}`);
            done();
        }
    });

    /**
     * @tc.number  Telephony_CallManager_isInEmergencyCall_Async_0200
     * @tc.name    Dial an emergency number (THE_THREE_NUMBER) and call isInEmergencyCall() to determine whether
     *             an emergency call is made, returning true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isInEmergencyCall_Async_0200', 0, async function (done) {
        const options = {accountId:SLOTID, videoState:MEDIA_TYPE_VOICE, dialScene:DIAL_SCENCE_CALL_PRIVILEGED,
            dialType:DIAL_CARRIER_TYPE};
        call.dial(THE_THREE_NUMBER, options, async (err, data) => {
            if (err) {
                console.log(`Telephony_CallManager_isInEmergencyCall_Async_0200 dial fail : err = ${err}`);
                done();
                return;
            }
            console.log(`Telephony_CallManager_isInEmergencyCall_Async_0200  dial finish : data = ${data}`);
            const startTime = new Date().getTime();
            while (true) {
                if (callState === call.CALL_STATUS_DIALING || callState === call.CALL_STATUS_ACTIVE ||
                     callState === call.CALL_STATUS_ALERTING) {
                    call.isInEmergencyCall((err, data) => {
                        if (err) {
                            console.log(
                                `Telephony_CallManager_isInEmergencyCall_Async_0200 isInEmrgencyCall fail : err = ${
                                    err}`
                            );
                            done();
                            return;
                        }
                        console.log(
                            `Telephony_CallManager_isInEmergencyCall_Async_0200 isInEmrgencyCall finish : data = ${
                                data}`
                        );
                        done();

                    });
                    return;
                }
                await sleep(WAITING_TIME);
                endTime = new Date().getTime();
                timing = endTime - startTime;
                if (timing > TIME_OUT) {
                    done();
                    break;
                }
            }
        });
    });

    /**
     * @tc.number  Telephony_CallManager_isInEmergencyCall_Promise_0200
     * @tc.name    Dial an emergency number (THE_THREE_NUMBER) and call isInEmergencyCall() to determine whether
     *             an emergency call is made. The return value is true
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_isInEmergencyCall_Promise_0200', 0, async function (done) {
        try {
            const options = {accountId:SLOTID, videoState:MEDIA_TYPE_VOICE, dialScene:DIAL_SCENCE_CALL_PRIVILEGED,
                dialType:DIAL_CARRIER_TYPE};
            var data = await call.dial(THE_THREE_NUMBER, options);
            console.log(`Telephony_CallManager_isInEmergencyCall_Promise_0200 dial finish : data = ${data}`);
            const startTime = new Date().getTime();
            while (true) {
                if (callState === call.CALL_STATUS_DIALING || callState === call.CALL_STATUS_ACTIVE ||
                     callState === call.CALL_STATUS_ALERTING) {
                    try {
                        var data = await call.isInEmergencyCall();
                        console.log(`Telephony_CallManager_isInEmergencyCall_Promise_0200 finish : data = ${data}`);
                        done();
                    } catch (err) {
                        console.log(`Telephony_CallManager_isInEmergencyCall_Promise_0200 fail : err = ${err}`);
                        done();
                        return;
                    }
                    return;
                }
                await sleep(WAITING_TIME);
                endTime = new Date().getTime();
                timing = endTime - startTime;
                if (timing > TIME_OUT) {
                    done();
                    break;
                }
            }
        } catch (err) {
            console.log(`Telephony_CallManager_isInEmergencyCall_Promise_0200 dial fail : err = ${err}`);
            done();
        }
    });

    /**
     * @tc.number  Telephony_CallManager_IMS_dial_Promise_0900
     * @tc.name    Dial a call by function dial by args phoneNumber PHONE_NUMBER_LENGTH_11 options
     *             {accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE, dialScene:DIAL_SCENCE_CALL_PRIVILEGED,
     *             dialType:DIAL_TYPE_ERR_CALL} by promise,the function return error
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_IMS_dial_Promise_0900', 0, function (done) {
        let caseName = 'Telephony_CallManager_IMS_dial_Promise_0900';
        let flag = true;
        reachState(caseName, CALL_STATUS_DIALING, '', false).then(data => {
            callId = data.callId;
            console.log(`${caseName} reachState success,case failed,data:${toString(data)}`);
            hangupCall2(caseName, done, callId);
        }).catch(error => {
            console.log(`${caseName} reachState error,case ${flag ? 'success' : 'failed'},error:${toString(error)}`);
            done();
        });
        let obj = {accountId:DEFAULT_SLOT_ID, videoState:MEDIA_TYPE_VOICE,
            dialScene:DIAL_SCENCE_CALL_PRIVILEGED, dialType:DIAL_TYPE_ERR_CALL};
        call.dial(PHONE_NUMBER_LENGTH_11, obj).then(data => {
            flag = false;
        }).catch(error => {
            console.log(`${caseName} dial error,error:${toString(error)}`);
            flag = true;
        });
    });

    /**
     * @tc.number  Telephony_CallManager_on_0900
     * @tc.name    Dial a call by number AUTO_ACCEPT_NUMBER options {accountId: DEFAULT_SLOT_ID,
     *             videoState: MEDIA_TYPE_VOICE,dialScene: DIAL_SCENCE_CALL_NORMAL, dialType:DIAL_TYPE_OTT}
     *             and before being accepted,the callEventChange event get EVENT_OTT_FUNCTION_UNSUPPORTED
     * @tc.desc    Function test
     */
    it('Telephony_CallManager_on_0900', 0, function (done) {
        let caseName = 'Telephony_CallManager_on_0900';
        let callId = null;
        reachCallEventState(EVENT_OTT_FUNCTION_UNSUPPORTED).then(data => {
            console.log(`${caseName} reachCallEventState success,case ${
                data.eventId === EVENT_OTT_FUNCTION_UNSUPPORTED ?
                    'success' : 'failed'},data:${toString(data)}`);
            done();
        }).catch(error => {
            if (callId) {
                console.log(`${caseName} reachCallEventState error,case failed,error:${toString(error)}`);
                done();
            } else {
                console.log(`${caseName} scenceInCalling error,case failed,error:${toString(error)}`);
                done();
            }
        });
        let options = {accountId: DEFAULT_SLOT_ID, videoState: MEDIA_TYPE_VOICE,
            dialScene: DIAL_SCENCE_CALL_NORMAL, dialType:DIAL_TYPE_OTT};
        call.dial(AUTO_ACCEPT_NUMBER, options, (err, data) => {
            if (err) {
                console.log(`${caseName} dial err:${err}`);
                return;
            }
            console.log(`${caseName} dial success data:${data}`);
        });
    });
});