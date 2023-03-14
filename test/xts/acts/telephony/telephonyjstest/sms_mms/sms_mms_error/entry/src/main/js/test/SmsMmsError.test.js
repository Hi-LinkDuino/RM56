/*
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
import sms from '@ohos.telephony.sms';
import {
  describe,
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect
} from 'deccjsunit/index';

describe('SmsMmsErrorTest', function () {
  const FALSE_SLOT_ID = 9;
  const CORRECT_SMS_PDU = '01000F9168683106019196F400080A00680065006C006C006F';
  const eorroMessagePath = '/data/telephony/deSrc/EorroMessagePath.mms';
  const IDENTIFIER_MIN = 0;
  const IDENTIFIER_MAX = 0xFFFF;
  const RANTYPE_GSM = 1;
  var rawArrayNull = [];
  const SMS_SEND_DST_NUMBER = '';
  const MAX_CHINESE_MESSAGE_LENTH = 63;
  const MAX_MESSAGE_LENTH = 2401;

  const TRUE_SLOT_ID = 0;
  const RECEIVE_SMS_PDU = '240D91689141468496F600001270721142432302B319';

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Async_0200
   * @tc.name     When SLOTID is the wrong value,Failed to save SMS to SIM
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Async_0200', 0, async function (done) {
    let data = {
      slotId: FALSE_SLOT_ID,
      smsc: '',
      pdu: CORRECT_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_SENT
    };
    sms.addSimMessage(data, (err, result) => {
      if (err) {
        console.log('Telephony_SmsMms_addSimMessage_Async_0200 finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_addSimMessage_Async_0200 fail');
      done();
    });
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Promise_0200
   * @tc.name     When SLOTID is the wrong value,Failed to save SMS to SIM
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Promise_0200', 0, async function (done) {
    let data = {
      slotId: FALSE_SLOT_ID,
      smsc: '',
      pdu: CORRECT_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_SENT
    };
    try {
      await sms.addSimMessage(data);
      expect().assertFail();
      console.log('Telephony_SmsMms_addSimMessage_Promise_0200 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_addSimMessage_Promise_0200 finish');
      done();
    }
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Async_1400
   * @tc.name     When status is equal to the correct value of SIM_MESSAGE_STATUS_READ,
   *              Save a text message to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Async_1400', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: RECEIVE_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_READ
    };
    sms.addSimMessage(data, (addErr) => {
      if (addErr) {
        console.log('Telephony_SmsMms_addSimMessage_Async_1400 finish');
        done();
        return;
      }
      console.log('Telephony_SmsMms_addSimMessage_Async_1400 add fail ');
    });
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Promise_1400
   * @tc.name     When status is equal to the correct value of SIM_MESSAGE_STATUS_READ,
   *              Save a text message to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Promise_1400', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: RECEIVE_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_READ
    };
    try {
      await sms.addSimMessage(data);
      console.log('Telephony_SmsMms_addSimMessage_Promise_1400 fail ');
    } catch (err) {
      console.log('Telephony_SmsMms_addSimMessage_Promise_1400 addSimMessage finish');
      done();
    }
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Async_1700
   * @tc.name     When status is equal to the correct value of SIM_MESSAGE_STATUS_UNSENT,Set the PDU read type
   *              Description Failed to add SMS messages to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Async_1700', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: RECEIVE_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_UNSENT
    };
    sms.addSimMessage(data, (addErr) => {
      if (addErr) {
        sms.getAllSimMessages(TRUE_SLOT_ID, (getErr, getResult) => {
          if (getErr) {
            console.log('Telephony_SmsMms_addSimMessage_Async_1700 getAllSimMessages fail');
            done();
            return;
          }
          expect(getResult.length == 0).assertTrue();
          console.log('Telephony_SmsMms_addSimMessage_Async_1700 getAllSimMessages getResult.length = '
            + getResult.length);
          console.log('Telephony_SmsMms_addSimMessage_Async_1700 getAllSimMessages finish');
          done();
        });
      } else {
        console.log('Telephony_SmsMms_addSimMessage_Async_1700 addSimMessage fail');
        done();
      }
    });
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Promise_1700
   * @tc.name     When status is equal to the correct value of SIM_MESSAGE_STATUS_UNSENT,Set the PDU read type
   *              Description Failed to add SMS messages to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Promise_1700', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: RECEIVE_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_UNSENT
    };
    try {
      await sms.addSimMessage(data);
      console.log('Telephony_SmsMms_addSimMessage_Promise_1700 add fail');
      done();
      return;
    } catch (err) {
      console.log('Telephony_SmsMms_addSimMessage_Promise_1700 finish ');
      done();
    }
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Async_1800
   * @tc.name     When status is equal to the correct value of  SIM_MESSAGE_STATUS_UNREAD,Set the PDU sending type
   *              Description Failed to add SMS messages to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Async_1800', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: CORRECT_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_UNREAD
    };
    sms.addSimMessage(data, (addErr) => {
      if (addErr) {
        sms.getAllSimMessages(TRUE_SLOT_ID, (getErr, getResult) => {
          if (getErr) {
            console.log('Telephony_SmsMms_addSimMessage_Async_1800 getAllSimMessages fail');
            done();
            return;
          }
          expect(getResult.length == 0).assertTrue();
          console.log('Telephony_SmsMms_addSimMessage_Async_1800 getAllSimMessages getResult = ' + getResult.length);
          console.log('Telephony_SmsMms_addSimMessage_Async_1800 getAllSimMessages finish');
          done();
        });
      } else {
        console.log('Telephony_SmsMms_addSimMessage_Async_1800 addSimMessage fail');
        done();
      }
    });
  });

  /*
   * @tc.number   Telephony_SmsMms_addSimMessage_Promise_1800
   * @tc.name     When status is equal to the correct value of  SIM_MESSAGE_STATUS_UNREAD,Set the PDU sending type
   *              Description Failed to add SMS messages to the SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_addSimMessage_Promise_1800', 0, async function (done) {
    let data = {
      slotId: TRUE_SLOT_ID,
      smsc: '',
      pdu: CORRECT_SMS_PDU,
      status: sms.SIM_MESSAGE_STATUS_UNREAD
    };
    try {
      await sms.addSimMessage(data);
      console.log('Telephony_SmsMms_addSimMessage_Promise_1800 add fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_addSimMessage_Promise_1800 finish ');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_delSimMessage_Async_0200
   * @tc.name     When SLOTID is the wrong value,Deletes a text message from the SIM card fail
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_delSimMessage_Async_0200', 0, async function (done) {
    sms.delSimMessage(FALSE_SLOT_ID, 0, (err) => {
      if (err) {
        console.log('Telephony_SmsMms_delSimMessage_Async_0200 delSimMessage finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_delSimMessage_Async_0200 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_delSimMessage_Promise_0200
   * @tc.name     When SLOTID is the wrong value,Deletes a text message from the SIM card fail
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_delSimMessage_Promise_0200', 0, async function (done) {
    try {
      await sms.delSimMessage(FALSE_SLOT_ID, 0);
      expect().assertFail();
      console.log('Telephony_SmsMms_delSimMessage_Promise_0200 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_delSimMessage_Promise_0200 finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_updateSimMessage_Async_0100
   * @tc.name     When SLOTID is the wrong value,Failed to update SIM card SMS record
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_updateSimMessage_Async_0100', 0, async function (done) {
    let upData = {
      slotId: FALSE_SLOT_ID,
      msgIndex: 0,
      newStatus: sms.SIM_MESSAGE_STATUS_SENT,
      pdu: CORRECT_SMS_PDU,
      smsc: ''
    };
    sms.updateSimMessage(upData, (err) => {
      if (err) {
        console.log('Telephony_SmsMms_updateSimMessage_Async_0100 update finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_updateSimMessage_Async_0100 update fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_updateSimMessage_Promise_0100
   * @tc.name     When SLOTID is the wrong value,Failed to update SIM card SMS record
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_updateSimMessage_Promise_0100', 0, async function (done) {
    let upData = {
      slotId: FALSE_SLOT_ID,
      msgIndex: 0,
      newStatus: sms.SIM_MESSAGE_STATUS_SENT,
      pdu: CORRECT_SMS_PDU,
      smsc: ''
    };
    try {
      await sms.updateSimMessage(upData);
      expect().assertFail();
      console.log('Telephony_SmsMms_updateSimMessage_Promise_0100 fail');
      done();
      return;
    } catch (err) {
      console.log('Telephony_SmsMms_updateSimMessage_Promise_0100 getAllSimMessages cur finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_getAllSIMMessages_Async_0200
   * @tc.name     When "SLOTID" is an error value,Failed to query all SMS records for SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getAllSIMMessages_Async_0200', 0, async function (done) {
    sms.getAllSimMessages(FALSE_SLOT_ID, (err, result) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_getAllSIMMessages_Async_0200 fail');
        done();
        return;
      }
	  if (result.length > 0) {
		  except(result[0].shortMessage != null).assertTrue();
		  except(result[0].indexOnSim != -1).assertTrue();
	  }
      expect(result === undefined || result.length === 0).assertTrue();
      console.log('Telephony_SmsMms_getAllSIMMessages_Async_0200 finish');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_getAllSIMMessages_Promise_0200
   * @tc.name     When "SLOTID" is an error value,Failed to query all SMS records for SIM card
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getAllSIMMessages_Promise_0200', 0, async function (done) {
    try {
      let promise = await sms.getAllSimMessages(FALSE_SLOT_ID);
      expect(promise === null || promise === undefined || promise.length === 0).assertTrue();
      console.log('Telephony_SmsMms_getAllSIMMessages_Promise_0200 getAllSimMessages cur finish');
      done();
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_getAllSIMMessages_Promise_0200 fail');
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0200
   * @tc.name    Call interface CreateMessage,
   *             The incoming PDU is empty, the specification is 3GPP,
   *             shortMessage isn't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_0200', 0, async function (done) {
    sms.createMessage(rawArrayNull, '3gpp', (err, shortMessage) => {
      if (err) {
        console.log('Telephony_SmsMms_createMessage_Async_0200 finish');
		except(shortMessage.visibleMessageBody != 'visibleMessageBody').assertTrue();
		except(shortMessage.visibleRawAddress != 'visibleRawAddress').assertTrue();
		except(shortMessage.messageClass != 'sms.FORWARD_MESSAGE').assertTrue();
		except(shortMessage.protocolId != -1).assertTrue();
		except(shortMessage.scAddress != 'scAddress').assertTrue();
		except(shortMessage.scTimestamp != -1).assertTrue();
		except(shortMessage.isReplaceMessage != true).assertTrue();
		except(shortMessage.hasReplyPath != true).assertTrue();
		except(shortMessage.status != -1).assertTrue();
		except(shortMessage.isSmsStatusReportMessage != true).assertTrue();
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Async_0200 fail');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_sendMessage_0200
   * @tc.name    Call the interface sendMessage, set the card slot parameter "slotId" to FALSE_SLOT_ID,
   *             SMS failed to send
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_sendMessage_0200', 0, async function (done) {
    sms.sendMessage({
      slotId: FALSE_SLOT_ID,
      destinationHost: SMS_SEND_DST_NUMBER,
	  serviceCenter: '',
      content: 'hello',
	  destinationPort: 0,
      sendCallback: (err, value) => {
        if (err) {
          expect().assertFail();
          console.log('Telephony_SmsMms_sendMessage_0200 fail');
          done();
          return;
        }
        console.log(`sendCallback success sendResult = ${value.result}`);
        expect(value.result === sms.SEND_SMS_FAILURE_UNKNOWN).assertTrue();
        expect(value.isLastPart != true).assertTrue();
        console.log('Telephony_SmsMms_sendMessage_0200 finish');
        done();
      },
      deliveryCallback: () => {}
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_0200
   * @tc.name    Call interface CreateMessage,
   *             The incoming PDU is empty, the specification is 3GPP,
   *             promise returns the result Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_0200', 0, async function (done) {
    try {
      await sms.createMessage(rawArrayNull, '3gpp');
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0200 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_createMessage_Promise_0200 finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_setSmscAddr_Async_0200
   * @tc.name     When "SLOTID" is an error value,Failed to set short message service address
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setSmscAddr_Async_0200', 0, async function (done) {
    sms.setSmscAddr(FALSE_SLOT_ID, '', (err) => {
      if (err) {
        console.log('Telephony_SmsMms_setSmscAddr_Async_0200 finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_setSmscAddr_Async_0200 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_setSmscAddr_Promise_0200
   * @tc.name     When "SLOTID" is an error value,Failed to set short message service address
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setSmscAddr_Promise_0200', 0, async function (done) {
    try {
      await sms.setSmscAddr(FALSE_SLOT_ID, '');
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0200 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_createMessage_Promise_0200  finish ');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_getSmscAddr_Async_0200
   * @tc.name     When "SLOTID" is an error value,Failed to get SMS service address
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getSmscAddr_Async_0200', 0, async function (done) {
    sms.getSmscAddr(FALSE_SLOT_ID, (err, getResult) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_getSmscAddr_Async_0200 fail');
        done();
        return;
      }
      expect(getResult === undefined || getResult === '');
      console.log('Telephony_SmsMms_getSmscAddr_Async_0200 finish');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_getSmscAddr_Promise_0200
   * @tc.name     When "SLOTID" is an error value,Failed to get SMS service address
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getSmscAddr_Promise_0200', 0, async function (done) {
    try {
      let promise = await sms.getSmscAddr(FALSE_SLOT_ID);
      expect(promise === undefined || promise === '');
      console.log('Telephony_SmsMms_getSmscAddr_Promise_0200 finish');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_getSmscAddr_Promise_0200 fail');
      expect().assertFail();
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_getSmsSegmentsInfo_Async_2000
   * @tc.name    The passed argument is an error slotId, view the results
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_getSmsSegmentsInfo_Async_2000', 0, async function (done) {
    let message = '';
    for (let index = 0;index < MAX_CHINESE_MESSAGE_LENTH + 1;index++) {
      message += '中';
    }
    sms.getSmsSegmentsInfo(FALSE_SLOT_ID, message, true, (error, result) => {
      if (error) {
        console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_2000 getSmsSegmentsInfo error " + error.message);
        console.log('Telephony_SmsMms_getSmsSegmentsInfo_Async_2000 finish');
      } else {
        expect().assertFail();
        console.log('Telephony_SmsMms_getSmsSegmentsInfo_Async_2000 fail');
      }
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_getSmsSegmentsInfo_Promise_2000
   * @tc.name    The passed argument is an error slotId, view the results
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_getSmsSegmentsInfo_Promise_2000', 0, async function (done) {
    let message = '';
    for (let index = 0;index < MAX_MESSAGE_LENTH;index++) {
      message += 'a';
    }
    try {
      let result = await sms.getSmsSegmentsInfo(FALSE_SLOT_ID, message, true);
      console.log('Telephony_SmsMms_getSmsSegmentsInfo_Promise_2000 fail');
      expect().assertFail();
    } catch (err) {
      console.log('Telephony_SmsMms_getSmsSegmentsInfo_Promise_2000 finish');
    }
    done();
  });

  /*
   * @tc.number  Telephony_SmsMms_isImsSmsSupported_Async_0100
   * @tc.name    Check whether the Ims SMS function is supported
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_isImsSmsSupported_Async_0100', 0, async function (done) {
    sms.isImsSmsSupported((error, result) => {
      if (error) {
        console.log('Telephony_SmsMms_isImsSmsSupported_Async_0100 fail');
      } else {
        expect(result === false || result === true).assertTrue();
        console.log('Telephony_SmsMms_isImsSmsSupported_Async_0100 finish');
      }
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_isImsSmsSupported_Promise_0100
   * @tc.name    Check whether the Ims SMS function is supported
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_isImsSmsSupported_Promise_0100', 0, async function (done) {
    try {
      let result = await sms.isImsSmsSupported();
      expect(result === false || result === true).assertTrue();
      console.log('Telephony_SmsMms_isImsSmsSupported_Promise_0100 finish');
    } catch (err) {
      console.log('Telephony_SmsMms_isImsSmsSupported_Promise_0100 fail');
    }
    done();
  });

  /*
   * @tc.number  Telephony_SmsMms_hasSmsCapability_0100
   * @tc.name    Check whether the Ims SMS function is supported
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_hasSmsCapability_0100', 0, async function (done) {
    let result = sms.hasSmsCapability();
    console.log('Telephony_SmsMms_hasSmsCapability_0100 result : ' + result);
    expect(result === false || result === true).assertTrue();
    done();
  });

  /*
   * @tc.number  Telephony_SmsMms_getImsShortMessageFormat_Async_0100
   * @tc.name    call the interface to obtain SMS system
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_getImsShortMessageFormat_Async_0100', 0, async function (done) {
    sms.getImsShortMessageFormat((error, result) => {
      if (error) {
        console.log('Telephony_SmsMms_getImsShortMessageFormat_Async_0100 fail');
        expect().assertFail();
      } else {
        expect(result === undefined || result === '').assertTrue();
        console.log('Telephony_SmsMms_getImsShortMessageFormat_Async_0100 result = ' + result);
        console.log('Telephony_SmsMms_getImsShortMessageFormat_Async_0100 finish');
      }
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_getImsShortMessageFormat_Promise_0100
   * @tc.name    call the interface to obtain SMS system
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_getImsShortMessageFormat_Promise_0100', 0, async function (done) {
    try {
      let result = await sms.getImsShortMessageFormat();
      expect(result === undefined || result === '').assertTrue();
      console.log('Telephony_SmsMms_getImsShortMessageFormat_Async_0100 result = ' + result);
      console.log('Telephony_SmsMms_getImsShortMessageFormat_Promise_0100 finish');
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_getImsShortMessageFormat_Promise_0100 fail');
    }
    done();
  });

  /*
   * @tc.number  Telephony_SmsMms_splitMessage_Async_1600
   * @tc.name    Call the interface, set message to empty, and see the result
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_splitMessage_Async_1600', 0, async function (done) {
    let message = '';
    sms.splitMessage(message, (error, result) => {
      if (error) {
        console.log("Telephony_SmsMms_splitMessage_Async_1600 splitMessage on error because " + error.message);
        console.log('Telephony_SmsMms_splitMessage_Async_1600 fail');
      } else {
        console.log("Telephony_SmsMms_splitMessage_Async_1600 splitMessage on value = " + JSON.stringify(result));
        expect(result === undefined || result.length === 0).assertTrue();
        console.log('Telephony_SmsMms_splitMessage_Async_1600 finish');
      }
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_splitMessage_Promise_1600
   * @tc.name    Call the interface, set message to empty, and see the result
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_splitMessage_Promise_1600', 0, async function (done) {
    let message = '';
    try {
      let result = await sms.splitMessage(message);
      console.log('Telephony_SmsMms_splitMessage_Promise_1600 finish');
      expect(result === undefined || result.length === 0).assertTrue();
    } catch (err) {
      console.log('Telephony_SmsMms_splitMessage_Promise_1600 fail');
    }
    done();
  });

  /**
   * @tc.number   Telephony_SmsMms_decodeMms_Async_1000
   * @tc.name     Passing in the exception path, Parse failure
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_decodeMms_Async_1000', 0, async function (done) {
    sms.decodeMms(eorroMessagePath, (err, data) => {
      if (err) {
        console.log('Telephony_SmsMms_decodeMms_Async_1000 finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_decodeMms_Async_1000 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_decodeMms_Promise_1000
   * @tc.name     Passing in the exception path, Parse failure
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_decodeMms_Promise_1000', 0, async function (done) {
    try {
      await sms.decodeMms(eorroMessagePath);
      console.log('Telephony_SmsMms_decodeMms_Promise_1000 fail');
      expect().assertFail();
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_decodeMms_Promise_1000 finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0900
   * @tc.name     ReadOrigInd type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_encodeMms_Async_0900', 0, async function (done) {
    let mmsType = {
      version: sms.MMS_VERSION_1_1,
      messageId: "0001",
      to: [{address: "+861388888****/TYPE=PLMN", charset: sms.UTF_8}],
      "from": {address: "+861381234****/TYPE=PLMN", charset: sms.UTF_8},
      readStatus: 129,
      date: 1639378126,
    };
    sms.encodeMms({messageType: sms.TYPE_MMS_READ_ORIG_IND, mmsType}, (err, encodeData) =>{
      if (err) {
        console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms fail');
        done();
        return;
      }
      console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms finish');
      sms.decodeMms(encodeData, (err, data) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0900 fail');
          done();
          return;
        }
        expect(data.messageType === sms.TYPE_MMS_READ_ORIG_IND).assertTrue();
        expect(data.mmsType.version === sms.MMS_VERSION_1_1).assertTrue();
        expect(data.mmsType.messageId === '0001').assertTrue();
        expect(data.mmsType.to.length > 0).assertTrue();
        expect(data.mmsType.to[0].address.length > 0).assertTrue();
        expect(data.mmsType.to[0].charset === sms.UTF_8).assertTrue();
        expect(data.mmsType.from.address.length > 0).assertTrue();
        expect(data.mmsType.from.charset === sms.UTF_8).assertTrue();
        expect(data.mmsType.date > 0).assertTrue();
        expect(data.mmsType.readStatus === 129).assertTrue();
        console.log('Telephony_SmsMms_encodeMms_Async_0900 finish');
        done();
      });
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_encodeMms_Promise_0900
   * @tc.name     ReadOrigInd type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_encodeMms_Promise_0900', 0, async function (done) {
    let mmsType = {
      version: sms.MMS_VERSION_1_1,
      messageId: "0001",
      to: [{address: "+861388888****/TYPE=PLMN", charset: sms.UTF_8}],
      "from": {address: "+861381234****/TYPE=PLMN", charset: sms.UTF_8},
      date: 1639378126,
      readStatus: 129,
    };
    try {
      let encodePromise = await sms.encodeMms({messageType: sms.TYPE_MMS_READ_ORIG_IND, mmsType});
      expect(encodePromise.length > 0).assertTrue();
      console.log('Telephony_SmsMms_encodeMms_Promise_0900 encodeMms finish');
      let promise = await sms.decodeMms(encodePromise);
      expect(promise.messageType === sms.TYPE_MMS_READ_ORIG_IND).assertTrue();
      expect(promise.mmsType.version === sms.MMS_VERSION_1_1).assertTrue();
      expect(promise.mmsType.messageId === '0001').assertTrue();
      expect(promise.mmsType.to.length > 0).assertTrue();
      expect(promise.mmsType.to[0].address.length > 0).assertTrue();
      expect(promise.mmsType.to[0].charset === sms.UTF_8).assertTrue();
      expect(promise.mmsType.from.address.length > 0).assertTrue();
      expect(promise.mmsType.from.charset === sms.UTF_8).assertTrue();
      expect(promise.mmsType.date > 0).assertTrue();
      expect(promise.mmsType.readStatus === 129).assertTrue();
      console.log('Telephony_SmsMms_decodeMms_Promise_0900 finish');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_decodeMms_Promise_0900 fail');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_setCBConfig_Async_0600
   * @tc.name     When "SLOTID" is an error value,Failed to set up cell broadcast
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setCBConfig_Async_0600', 0, async function (done) {
    let data = {
      slotId: FALSE_SLOT_ID,
      enable: true,
      startMessageId: IDENTIFIER_MIN,
      endMessageId: IDENTIFIER_MAX,
      ranType: RANTYPE_GSM
    };
    sms.setCBConfig(data, (err) => {
      if (err) {
        console.log(`Telephony_SmsMms_setCBConfig_Async_0600 finish err : ${err.message}`);
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_setCBConfig_Async_0600 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_setCBConfig_Promise_0600
   * @tc.name     When "SLOTID" is an error value,Failed to set up cell broadcast
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setCBConfig_Promise_0600', 0, async function (done) {
    let data = {
      slotId: FALSE_SLOT_ID,
      enable: true,
      startMessageId: IDENTIFIER_MIN,
      endMessageId: IDENTIFIER_MAX,
      ranType: RANTYPE_GSM
    };
    try {
      await sms.setCBConfig(data);
      expect().assertFail();
      console.log('Telephony_SmsMms_setCBConfig_Promise_0600 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_setCBConfig_Promise_0600 finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_setDefaultSmsSlotId_Async_0200
   * @tc.name     The default card slot ID is set to an error value,Failed to set send SMS card slot ID
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setDefaultSmsSlotId_Async_0200', 0, async function (done) {
    sms.setDefaultSmsSlotId(FALSE_SLOT_ID, (err) => {
      if (err) {
        console.log('Telephony_SmsMms_setDefaultSmsSlotId_Async_0200 finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_setDefaultSmsSlotId_Async_0200 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_setDefaultSmsSlotId_Promise_0200
   * @tc.name     The default card slot ID is set to an error value,Failed to set send SMS card slot ID
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_setDefaultSmsSlotId_Promise_0200', 0, async function (done) {
    try {
      await sms.setDefaultSmsSlotId(FALSE_SLOT_ID);
      expect().assertFail();
      console.log('Telephony_SmsMms_setDefaultSmsSlotId_Promise_0200  fail');
      done();
      return;
    } catch (err) {
      console.log('Telephony_SmsMms_setDefaultSmsSlotId_Promise_0200  finish');
      done();
    }
  });

  /**
   * @tc.number   Telephony_SmsMms_getDefaultSmsSlotId_Async_0100
   * @tc.name     Set the default card slot ID for sending SMS
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getDefaultSmsSlotId_Async_0100', 0, async function (done) {
    sms.getDefaultSmsSlotId((getErr, getResult) => {
      if (getErr) {
        console.log('Telephony_SmsMms_getDefaultSmsSlotId_Async_0100 finish');
        done();
        return;
      }
      console.log('Telephony_SmsMms_getDefaultSmsSlotId_Async_0100 fail');
      done();
    });
  });

  /**
   * @tc.number   Telephony_SmsMms_getDefaultSmsSlotId_Promise_0100
   * @tc.name     Set the default card slot ID for sending SMS
   * @tc.desc     Function test
   */
  it('Telephony_SmsMms_getDefaultSmsSlotId_Promise_0100', 0, async function (done) {
    try {
      let getResult = await sms.getDefaultSmsSlotId();
      console.log('Telephony_SmsMms_getDefaultSmsSlotId_Promise_0100 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_getDefaultSmsSlotId_Promise_0100 finish');
      done();
    }
  });

});