/**
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import sim from '@ohos.telephony.sim';
import * as env from './lib/Const';
import { describe, it, expect, afterEach } from 'deccjsunit/index';

describe('SimManagerTest', function () {
  afterEach(async function () {
    try {
      class IccAccountInfo {
        constructor (slotIndex, showName, showNumber) {
          this.slotIndex = slotIndex;
          this.showName = showName;
          this.showNumber = showNumber;
        }
      }
      const SLOT_INDEX = 0;
      const SHOW_NAME = 'test';
      const SHOW_NUMBER = '10';
      let iccAccountInfo = new IccAccountInfo(SLOT_INDEX, SHOW_NAME, SHOW_NUMBER);
      expect(iccAccountInfo.slotIndex === SLOT_INDEX).assertTrue();
      expect(iccAccountInfo.showName === SHOW_NAME).assertTrue();
      expect(iccAccountInfo.showNumber === SHOW_NUMBER).assertTrue();

      class LockStatusResponse {
        constructor (result, remain) {
          this.result = result;
          this.remain = remain;
        }
      }
      const RES_RESULT = 0;
      const RES_REMAIN = 0;
      let lockStatusResponse = new LockStatusResponse(RES_RESULT, RES_REMAIN);
      expect(lockStatusResponse.result === RES_RESULT).assertTrue();
      expect(lockStatusResponse.remain === RES_REMAIN).assertTrue();
    } catch (error) {
      console.log(`Telephony_Sim error`);
    }
  });

  /**
    * @tc.number  Telephony_Sim_constantValidate_0100
    * @tc.name    SIM card constant validation 
    * @tc.desc    Function test
    */
   it('Telephony_Sim_constantValidate_0100', 0, async function (done) {
    expect(sim.SIM_STATE_UNKNOWN === env.CONSTANTS.SIM_STATE_UNKNOWN).assertTrue();
    expect(sim.SIM_STATE_NOT_PRESENT === env.CONSTANTS.SIM_STATE_NOT_PRESENT).assertTrue();
    expect(sim.SIM_STATE_LOCKED === env.CONSTANTS.SIM_STATE_LOCKED).assertTrue();
    expect(sim.SIM_STATE_NOT_READY === env.CONSTANTS.SIM_STATE_NOT_READY).assertTrue();
    expect(sim.SIM_STATE_READY === env.CONSTANTS.SIM_STATE_READY).assertTrue();
    expect(sim.SIM_STATE_LOADED === env.CONSTANTS.SIM_STATE_LOADED).assertTrue();
    expect(sim.GENERAL_CONTACT === env.CONSTANTS.GENERAL_CONTACT).assertTrue();
    expect(sim.FIXED_DIALING === env.CONSTANTS.FIXED_DIALING).assertTrue();
    expect(sim.LOCK_OFF === env.CONSTANTS.LOCK_OFF).assertTrue();
    expect(sim.LOCK_ON === env.CONSTANTS.LOCK_ON).assertTrue();
    expect(sim.PIN_LOCK === env.CONSTANTS.PIN_LOCK).assertTrue();
    expect(sim.FDN_LOCK === env.CONSTANTS.FDN_LOCK).assertTrue();
    expect(sim.UNKNOWN_CARD === env.CONSTANTS.UNKNOWN_CARD).assertTrue();
    expect(sim.SINGLE_MODE_SIM_CARD === env.CONSTANTS.SINGLE_MODE_SIM_CARD).assertTrue();
    expect(sim.SINGLE_MODE_USIM_CARD === env.CONSTANTS.SINGLE_MODE_USIM_CARD).assertTrue();
    expect(sim.SINGLE_MODE_RUIM_CARD === env.CONSTANTS.SINGLE_MODE_RUIM_CARD).assertTrue();
    expect(sim.DUAL_MODE_CG_CARD === env.CONSTANTS.DUAL_MODE_CG_CARD).assertTrue();
    expect(sim.CT_NATIONAL_ROAMING_CARD === env.CONSTANTS.CT_NATIONAL_ROAMING_CARD).assertTrue();
    expect(sim.CU_DUAL_MODE_CARD === env.CONSTANTS.CU_DUAL_MODE_CARD).assertTrue();
    expect(sim.DUAL_MODE_TELECOM_LTE_CARD === env.CONSTANTS.DUAL_MODE_TELECOM_LTE_CARD).assertTrue();
    expect(sim.DUAL_MODE_UG_CARD === env.CONSTANTS.DUAL_MODE_UG_CARD).assertTrue();
    expect(sim.SINGLE_MODE_ISIM_CARD === env.CONSTANTS.SINGLE_MODE_ISIM_CARD).assertTrue();
    expect(sim.PN_PIN_LOCK === env.CONSTANTS.PN_PIN_LOCK).assertTrue();
    expect(sim.PN_PUK_LOCK === env.CONSTANTS.PN_PUK_LOCK).assertTrue();
    expect(sim.PU_PIN_LOCK === env.CONSTANTS.PU_PIN_LOCK).assertTrue();
    expect(sim.PU_PUK_LOCK === env.CONSTANTS.PU_PUK_LOCK).assertTrue();
    expect(sim.PP_PIN_LOCK === env.CONSTANTS.PP_PIN_LOCK).assertTrue();
    expect(sim.PP_PUK_LOCK === env.CONSTANTS.PP_PUK_LOCK).assertTrue();
    expect(sim.PC_PIN_LOCK === env.CONSTANTS.PC_PIN_LOCK).assertTrue();
    expect(sim.PC_PUK_LOCK === env.CONSTANTS.PC_PUK_LOCK).assertTrue();
    expect(sim.SIM_PIN_LOCK === env.CONSTANTS.SIM_PIN_LOCK).assertTrue();
    expect(sim.SIM_PUK_LOCK === env.CONSTANTS.SIM_PUK_LOCK).assertTrue();
    console.log(`Telephony_Sim_constantValidate_0100 finish`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Async_0200
    * @tc.name    Enter exception parameters to test whether the getISOCountryCodeForSim interface function
    *             can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getISOCountryCodeForSim_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getISOCountryCodeForSim_Async_0200';
    sim.getISOCountryCodeForSim(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail, data = ${data}`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getISOCountryCodeForSim_Promise_0200
    * @tc.name    Enter exception parameters to test whether the getISOCountryCodeForSim interface function
    *             can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getISOCountryCodeForSim_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getISOCountryCodeForSim_Promise_0200';
    try {
      var data = await sim.getISOCountryCodeForSim(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail, data = ${data}`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getSimOperatorNumeric_Async_0200
    * @tc.name    Enter exception parameters to test whether the getSimOperatorNumeric interface function
    *             can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimOperatorNumeric_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimOperatorNumeric_Async_0200';
    sim.getSimOperatorNumeric(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail, data = ${data}`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getSimOperatorNumeric_Promise_0200
    * @tc.name    Enter exception parameters to test whether the getSimOperatorNumeric interface function
    *             can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimOperatorNumeric_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimOperatorNumeric_Promise_0200';
    try {
      var data = await sim.getSimOperatorNumeric(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail, data = ${data}`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getSimSpn_Async_0200
    * @tc.name    Enter exception parameters to test whether the getSimSpn interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimSpn_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimSpn_Async_0200';
    sim.getSimSpn(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} fail: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail, data = ${data}`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getSimSpn_Promise_0200
    * @tc.name    Enter exception parameters to test whether the getSimSpn interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimSpn_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimSpn_Promise_0200';
    try {
      var data = await sim.getSimSpn(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail, data = ${data}`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getSimState_Async_0200
    * @tc.name    Enter exception parameters to test whether the getSimState interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimState_Async_0200', 0, async function (done) {
    sim.getSimState(env.SLOTID2, (err, data) => {
      const CASE_NAME = 'Telephony_Sim_getSimState_Async_0200';
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail, data = ${data}`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getSimState_Promise_0200
    * @tc.name    Enter exception parameters to test whether the getSimState interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimState_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimState_Promise_0200';
    try {
      var data = await sim.getSimState(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail, data = ${data}`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getSimGid1_Async_0200
    * @tc.name    Enter exception parameters to test whether the getSimGid1 interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimGid1_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimGid1_Async_0200';
    sim.getSimGid1(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail, data = ${data}`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getSimGid1_Promise_0200
    * @tc.name    Enter exception parameters to test whether the getSimGid1 interface function can execute normally.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimGid1_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimGid1_Promise_0200';
    try {
      var data = await sim.getSimGid1(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail, data = ${data}`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getSimAccountInfo_Async_0200
    * @tc.name    Test getSimAccountInfo async callback interface slotid exception,
    *             enter parameter 2, and check the callback value
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimAccountInfo_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimAccountInfo_Async_0200';
    sim.getSimAccountInfo(env.SLOTID2, (err) => {
      if (err) {
        console.log(`${CASE_NAME} expect error: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getSimAccountInfo_Promise_0200
    * @tc.name    Test getSimAccountInfo promise interface slotid exception,
    *             enter parameter 2, and check the callback value
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getSimAccountInfo_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimAccountInfo_Promise_0200';
    try {
      await sim.getSimAccountInfo(env.SLOTID2);
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
    } catch (err) {
      console.log(`${CASE_NAME} expect error: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
    }
    done();
  });

  /**
    * @tc.number  Telephony_Sim_setDefaultVoiceSlotId_Async_0100
    * @tc.name    Test the setDefaultVoiceLotid interface when the input SIM ID parameter is 2.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_setDefaultVoiceSlotId_Async_0100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setDefaultVoiceSlotId_Async_0100';
    sim.setDefaultVoiceSlotId(env.SLOTID2, (err) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_setDefaultVoiceSlotId_Promise_0100
    * @tc.name    Test the setDefaultVoiceLotid interface when the input SIM ID parameter is 2.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_setDefaultVoiceSlotId_Promise_0100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setDefaultVoiceSlotId_Promise_0100';
    try {
      await sim.setDefaultVoiceSlotId(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getDefaultVoiceSlotId_Async_0200
    * @tc.name    The test first sets the default card number and then calls
    *             the getDefaultVoiceLotid interface to get the SIM ID.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getDefaultVoiceSlotId_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getDefaultVoiceSlotId_Async_0200';
    sim.getDefaultVoiceSlotId((err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getDefaultVoiceSlotId_Promise_0200
    * @tc.name    The test first sets the default card number and then calls
    *             the getDefaultVoiceLotid interface to get the default SIM ID.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getDefaultVoiceSlotId_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getDefaultVoiceSlotId_Promise_0200';
    try {
      let data = await sim.getDefaultVoiceSlotId();
      console.log(`${CASE_NAME}, data = ${data}`);
      expect().assertFail();
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
    }
    done();
  });

  /**
    * @tc.number  Telephony_Sim_unlockPin_Async_0400
    * @tc.name    The status of the card is ready state, the unlockPin interface is tested, and slotId is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_unlockPin_Async_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPin_Async_0400';
    sim.unlockPin(env.SLOTID2, env.CORRECT_PIN, err => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_unlockPin_Promise_0400
    * @tc.name    The status of the card is ready state, the unlockPin interface is tested, and slotId is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_unlockPin_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPin_Promise_0400';
    try {
      await sim.unlockPin(env.SLOTID2, env.CORRECT_PIN);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_unlockPuk_Async_0400
    * @tc.name    The card status is ready state, and the unlockPuk interface is tested.
    *             The slotId parameter is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_unlockPuk_Async_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPuk_Async_0400';
    sim.unlockPuk(env.SLOTID2, env.CORRECT_PIN, env.CORRECT_PUK, err => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_unlockPuk_Promise_0400
    * @tc.name    The card status is ready state, and the unlockPuk interface is tested.
    *             The slotId parameter is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_unlockPuk_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPuk_Promise_0400';
    try {
      await sim.unlockPuk(env.SLOTID2, env.CORRECT_PIN, env.CORRECT_PUK);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    console.log(`${CASE_NAME} fail`);
    done();
  });

  /**
    * @tc.number  Telephony_Sim_alterPin_Async_0400
    * @tc.name    The status of the card is ready state, and the alterPin interface. SlotId is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_alterPin_Async_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_alterPin_Async_0400';
    sim.alterPin(env.SLOTID2, env.CORRECT_PIN, env.ALTER_PIN, err => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_alterPin_Promise_0400
    * @tc.name    The status of the card is ready state, and the alterPin interface is tested. SlotId is abnormal.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_alterPin_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_alterPin_Promise_0400';
    try {
      await sim.alterPin(env.SLOTID2, env.CORRECT_PIN, env.ALTER_PIN);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail`);
    done();
  });

  /**
   * @tc.number Telephony_Sim_SetLockState_Async_1000
   * @tc.name   The card is in the ready state, test the SetLockState async callback interface
   *            slotId exception into the parameter 2, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SetLockState_Async_1000', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_SetLockState_Async_1000';
    const lockInfo = { lockType: sim.PIN_LOCK, state: sim.LOCK_OFF, password: env.CORRECT_PIN };
    sim.setLockState(env.SLOTID2, lockInfo, error => {
      if (error) {
        console.log(`${CASE_NAME} setLockState expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_SetLockState_Promise_1000
   * @tc.name   The card is in the ready state, test the SetLockState promise interface
   *            slotId exception into the parameter 2, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SetLockState_Promise_1000', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_SetLockState_Promise_1000';
    const lockInfo = { lockType: sim.PIN_LOCK, state: sim.LOCK_OFF, password: env.CORRECT_PIN };
    try {
      await sim.setLockState(env.SLOTID2, lockInfo);
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} setLockState expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_GetSimIccId_Async_0600
   * @tc.name    CDMA file, getSimIccId async callback interface slotId
   *             exception enters parameter -1 test
   * @tc.desc    Function test
   */
  it('Telephony_Sim_GetSimIccId_Async_0600', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_GetSimIccId_Async_0600';
    sim.getSimIccId(env.SLOTID2, error => {
      if (error) {
        console.log(`${CASE_NAME} getSimIccId expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish.`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_GetSimIccId_Promise_0600
   * @tc.name    CDMA file, getSimIccId promise interface slotId
   *             exception enters parameter -1 test
   * @tc.desc    Function test
   */
  it('Telephony_Sim_GetSimIccId_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_GetSimIccId_Promise_0600';
    try {
      await sim.getSimIccId(env.SLOTID2);
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
    } catch (error) {
      console.log(`${CASE_NAME} getSimIccId expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish.`);
    }
    done();
  });

  /**
    * @tc.number  Telephony_Sim_getIMSI_Async_0200
    * @tc.name    To test the getIMSI interface, enter slotid as 4 and expect the result to enter err.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getIMSI_Async_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getIMSI_Async_0200';
    sim.getIMSI(env.SLOTID2, err => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} fail`);
      done();
    });
  });

  /**
    * @tc.number  Telephony_Sim_getIMSI_Promise_0200
    * @tc.name    To test the getIMSI interface, enter slotid as 4 and expect the result to enter err.
    * @tc.desc    Function test
    */
  it('Telephony_Sim_getIMSI_Promise_0200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getIMSI_Async_0200';
    try {
      await sim.getIMSI(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    expect().assertFail();
    console.log(`${CASE_NAME} fail`);
    done();
  });

  /**
   * @tc.number  Telephony_Sim_isSimActive_Async_0700
   * @tc.name    Test the slotId exception input parameter 2 during isSimActive interface card activation,
   *             check the callback value, and expect the callback result flase
   * @tc.desc    Function test
   */
  it('Telephony_Sim_isSimActive_Async_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_isSimActive_Async_0700';
    sim.isSimActive(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} fail, err: ${err.message}`);
        expect().assertFail();
        done();
        return;
      }
      expect(data).assertFalse();
      console.log(`${CASE_NAME} finish`);
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_isSimActive_Promise_0700
   * @tc.name    Test the slotId exception input parameter 2 during isSimActive interface card activation,
   *             check the callback value, and expect the callback result flase
   * @tc.desc    Function test
   */
  it('Telephony_Sim_isSimActive_Promise_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_isSimActive_Promise_0700';
    try {
      let data = await sim.isSimActive(env.SLOTID2);
      expect(data).assertFalse();
    } catch (err) {
      console.log(`${CASE_NAME} isSimActive fail, err: ${err.message}`);
      expect().assertFail();
      done();
      return;
    }
    console.log(`${CASE_NAME} finish`);
    done();
  });

  /**
     * @tc.number  Telephony_Sim_activateSim_Async_0600
     * @tc.name    Test the slotId exception input parameter 2 of activateSim interface,
     *             check the callback value, and expect to enter ERR
     * @tc.desc    Function test
     */
  it('Telephony_Sim_activateSim_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_activateSim_Async_0600';
    sim.activateSim(env.SLOTID2, (err) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} fail`);
      expect().assertFail();
      done();
    });
  });

  /**
     * @tc.number  Telephony_Sim_activateSim_Promise_0600
     * @tc.name    Test the slotId exception input parameter 2 of activateSim interface,
     *             check the callback value, and expect to enter ERR
     * @tc.desc    Function test
     */
  it('Telephony_Sim_activateSim_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_activateSim_Promise_0600';
    try {
      await sim.activateSim(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
      return;
    }
    console.log(`${CASE_NAME} fail`);
    expect().assertFail();
    done();
  });

  /**
     * @tc.number  Telephony_Sim_deactivateSim_Async_0600
     * @tc.name    Test the deactivateSim interface slotId exception input parameter 2
     *             and check the callback value, expecting to enter ERR
     * @tc.desc    Function test
     */
  it('Telephony_Sim_deactivateSim_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_deactivateSim_Async_0600';
    sim.deactivateSim(env.SLOTID2, (err) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} fail`);
      expect().assertFail();
      done();
    });
  });

  /**
     * @tc.number  Telephony_Sim_deactivateSim_Promise_0600
     * @tc.name    Test the deactivateSim interface slotId exception input parameter
     *             2 and check the callback value, expecting to enter ERR
     * @tc.desc    Function test
     */
  it('Telephony_Sim_deactivateSim_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_deactivateSim_Promise_0600';
    try {
      await sim.deactivateSim(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
      return;
    }
    console.log(`${CASE_NAME} fail`);
    expect().assertFail();
    done();
  });

  /**
     * @tc.number  Telephony_Sim_hasSimCard_Async_0600
     * @tc.name    Test the slotId exception input parameter 2 of the hasSimCard interface,
     *             check the callback value, and expect the callback result flase
     * @tc.desc    Function test
     */
  it('Telephony_Sim_hasSimCard_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_hasSimCard_Async_0600';
    sim.hasSimCard(env.SLOTID2, (err, data) => {
      if (err) {
        expect().assertFail();
        console.log(`${CASE_NAME} fail, err: ${err.message}`);
        done();
        return;
      }
      expect(data).assertFalse();
      console.log(`${CASE_NAME} finish`);
      done();
    });
  });

  /**
     * @tc.number  Telephony_Sim_hasSimCard_Promise_0600
     * @tc.name    Test the slotId exception input parameter 2 of the hasSimCard interface,
     *             check the callback value, and expect the callback result flase
     * @tc.desc    Function test
     */
  it('Telephony_Sim_hasSimCard_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_hasSimCard_Promise_0600';
    try {
      let data = await sim.hasSimCard(env.SLOTID2);
      expect(data).assertFalse();
    } catch (err) {
      expect().assertFail();
      console.log(`${CASE_NAME} fail, err: ${err.message}`);
      done();
      return;
    }
    console.log(`${CASE_NAME} finish`);
    done();
  });

  /**
   * @tc.number  Telephony_Sim_setShowName_Async_0700
   * @tc.name    Test setShowName slotId exception input parameter 2 to check the
   *             callback value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_setShowName_Async_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setShowName_Async_0700';
    sim.setShowName(env.SLOTID2, env.CARD_NAME, (err) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_setShowName_Promise_0700
   * @tc.name    Test setShowName promise interface slotId exception into parameter 2, check the return value
   * @tc.desc    Function test
   */
  it('Telephony_Sim_setShowName_Promise_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setShowName_Promise_0700';
    try {
      await sim.setShowName(env.SLOTID2, env.CARD_NAME);
    } catch (err) {
      console.log(`${CASE_NAME} finish, error : ${err.message}`);
      done();
      return;
    }
    console.log(`${CASE_NAME} fail`);
    expect().assertFail();
    done();
  });

  /**
   * @tc.number  Telephony_Sim_getShowName_Async_0700
   * @tc.name    Test the slotId exception input parameter 2 of getShowName and
   *             check the return value, expecting to enter ERR
   * @tc.desc   Function test
   */
  it('Telephony_Sim_getShowName_Async_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getShowName_Async_0700';
    sim.getShowName(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getShowName_Promise_0700
   * @tc.name    Test the slotId exception input parameter 2 of getShowName and check the
   *             return value, expecting to enter ERR
   * @tc.desc    Function test
   */

  it('Telephony_Sim_getShowName_Promise_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getShowName_Promise_0700';
    try {
      let data = await sim.getShowName(env.SLOTID2);
      console.log(`${CASE_NAME} fail, data : ${data}`);
      expect().assertFail();
      done();
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
    }
  });

  /**
   * @tc.number  Telephony_Sim_setShowNumber_Async_0700
   * @tc.name    Test setShowNumber slotId exception input parameter 2, check the callback value,
   *             and expect to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_setShowNumber_Async_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setShowNumber_Async_0700';
    sim.setShowNumber(env.SLOTID2, env.CARD_NUMBER, (err) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_setShowNumber_Promise_0700
   * @tc.name    Test setShowNumber slotId exception input parameter 2 and check
   *             the return value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_setShowNumber_Promise_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setShowNumber_Promise_0700';
    try {
      await sim.setShowNumber(env.SLOTID2, env.CARD_NUMBER);
      console.log(`${CASE_NAME} fail`);
      expect().assertFail();
      done();
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
    }
  });

  /**
   * @tc.number  Telephony_Sim_getShowNumber_Async_0600
   * @tc.name    Test the slotId exception of the getShowNumber async callback interface into parameter 2,
   *             check the callback value
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getShowNumber_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getShowNumber_Async_0600';
    sim.getShowNumber(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} fail, data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getShowNumber_Promise_0600
   * @tc.name    Test the slotId exception of the getShowNumber interface into parameter 2, check the callback value
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getShowNumber_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getShowNumber_Promise_0600';
    try {
      let data = await sim.getShowNumber(env.SLOTID2);
      console.log(`${CASE_NAME} fail, data : ${data}`);
      expect().assertFail();
      done();
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
    }
  });

  /**
   * @tc.number  Telephony_Sim_getOperatorConfigs_Async_0700
   * @tc.name    Test the getOperatorConfigs slotId exception input parameter 2 and check the
   *             callback value, expecting to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getOperatorConfigs_Async_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getOperatorConfigs_Async_0700';
    sim.getOperatorConfigs(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish, err: ${err.message}`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getOperatorConfigs_Promise_0700
   * @tc.name    Test the getOperatorConfigs slotId exception input parameter 2 and check
   *             the callback value, expecting to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getOperatorConfigs_Promise_0700', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getOperatorConfigs_Promise_0700';
    try {
      let data = await sim.getOperatorConfigs(env.SLOTID2);
      console.log(`${CASE_NAME} fail, data : ${data}`);
      expect().assertFail();
      done();
    } catch (err) {
      console.log(`${CASE_NAME} finish, err: ${err.message}`);
      done();
    }
  });

  /**
   * @tc.number  Telephony_Sim_getActiveSimAccountInfoList_Async_0500
   * @tc.name    When there is no card, test the getActiveSimAccountInfoList interface and check the return value
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getActiveSimAccountInfoList_Async_0500', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_getActiveSimAccountInfoList_Async_0500';
    sim.getActiveSimAccountInfoList(error => {
      if (error) {
        console.log(`${CASE_NAME} expect error: ${error.message}`);
      } else {
        console.log(`${CASE_NAME} test fail.`);
        expect().assertFail();
      }
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getActiveSimAccountInfoList_Promise_0500
   * @tc.name    When there is no card, test the getActiveSimAccountInfoList interface and check the return value
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getActiveSimAccountInfoList_Promise_0500', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getActiveSimAccountInfoList_Async_0500';
    try {
      await sim.getActiveSimAccountInfoList();
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_unlockPin2_Async_1000
   * @tc.name    Test slotid exception input parameter 2 of unlockPin2 and check the callback
   *             value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_unlockPin2_Async_1000', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPin2_Async_1000';
    sim.unlockPin2(env.SLOTID2, env.INCORRECT_PIN2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data: ${JSON.stringify(data)}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_unlockPin2_Promise_1000
   * @tc.name    Test slotid exception input parameter 2 of unlockPin2 and check the callback
   *             value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_unlockPin2_Promise_1000', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPin2_Promise_1000';
    try {
      await sim.unlockPin2(env.SLOTID2, env.INCORRECT_PIN2);
      expect().assertFail();
    } catch (err) {
      console.log(`${CASE_NAME} expect err: ${err.message}`);
      console.log(`${CASE_NAME} finish`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_unlockPuk2_Async_1100
   * @tc.name    Test the slotId exception input parameter 2 of the unlockPuk2
   *             interface and check the callback value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_unlockPuk2_Async_1100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPuk2_Async_1100';
    sim.unlockPuk2(env.SLOTID2, env.INCORRECT_PIN2, env.INCORRECT_PUK2_LEN8, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect err: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME}, data: ${JSON.stringify(data)}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_unlockPuk2_Promise_1100
   * @tc.name    Test the slotId exception input parameter 2 of the unlockPuk2 interface
   *             and check the callback value, expecting to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_unlockPuk2_Promise_1100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockPuk2_Async_1100';
    try {
      const data = await sim.unlockPuk2(env.SLOTID2, env.INCORRECT_PIN2, env.INCORRECT_PUK2_LEN8);
      console.log(`${CASE_NAME}, data: ${JSON.stringify(data)}`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} expect err: ${error.message}`);
      console.log(`${CASE_NAME} finish`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_alterPin2_Async_1100
   * @tc.name    Test alterPin2 interface exception slotId input parameter 2, check the callback
   *             value, expecting to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_alterPin2_Async_1100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_alterPin2_Async_1100';
    sim.alterPin2(env.SLOTID2, env.INCORRECT_PIN2, env.INCORRECT_PIN2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} expect error: ${err.message}`);
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} data: ${JSON.stringify(data)}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_alterPin2_Promise_1100
   * @tc.name    Test alterPin2 interface exception slotId input parameter 2, check the callback
   *             value, expecting to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_alterPin2_Promise_1100', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_alterPin2_Promise_1100';
    try {
      const data = await sim.alterPin2(env.SLOTID2, env.INCORRECT_PIN2, env.INCORRECT_PIN2);
      console.log(`${CASE_NAME} data: ${JSON.stringify(data)}`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
      console.log(`${CASE_NAME} finish`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_queryIccDiallingNumbers_Async_0800
   * @tc.name    Test sim.queryIccDiallingNumbers(soltId: 2, type: 1, callback:
   *             AsyncCallback<Array<DiallingNumbersInfo>>), expect return an empty array.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_queryIccDiallingNumbers_Async_0800', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_queryIccDiallingNumbers_Async_0800';
    sim.queryIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, (error, data) => {
      if (error) {
        console.log(`${CASE_NAME} query error: ${error.message}`);
        expect().assertFail();
        done();
        return;
      }
      expect(data.length === 0).assertTrue();
      console.log(`${CASE_NAME} finish`);
      done();
    });
  });

  /**
   * @tc.number  elephony_Sim_queryIccDiallingNumbers_Promise_0800
   * @tc.name    Test sim.queryIccDiallingNumbers(soltId: 2, type: 1):Promise<Array<DiallingNumbersInfo>>
   *             promise interface, expect result is an empty array.
   * @tc.desc    Function test
   */
  it('telephony_Sim_queryIccDiallingNumbers_Promise_0800', 0, async function (done) {
    const CASE_NAME = 'telephony_Sim_queryIccDiallingNumbers_Promise_0800';
    try {
      const contacts = await sim.queryIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT);
      expect(contacts.length === 0).assertTrue();
      console.log(`${CASE_NAME} test finish.`);
    } catch (error) {
      console.log(`${CASE_NAME} query error:${error.message}`);
      expect().assertFail();
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_addIccDiallingNumbers_Async_0900
   * @tc.name    Test sim.addIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers:
   *             {recordNumber: 1, alphaTag: 'test', number: '12345678', pin2: '123@#ABCD'},
   *             callback: AsyncCallback<void>) async callback interface, expect enter error.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_addIccDiallingNumbers_Async_0900', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_addIccDiallingNumbers_Async_0900';
    const CONTACT_INFO = { 'recordNumber': 1, 'alphaTag': 'test', 'number': '12345678' };
    const GENERAL_CONTACT_INFO = Object.assign({}, { 'pin2': env.INCORRECT_PIN2 }, CONTACT_INFO);
    sim.addIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, GENERAL_CONTACT_INFO, error => {
      if (error) {
        console.log(`${CASE_NAME} expect error: ${error.message}`);
        console.log(`${CASE_NAME} finish.`);
      } else {
        console.log(`${CASE_NAME} test fail.`);
        expect().assertFail();
      }
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_addIccDiallingNumbers_Promise_0900
   * @tc.name    Test sim.addIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers:
   *             {recordNumber: 1, alphaTag: 'test', number: '12345678', pin2: '123@#ABCD'}): Promise<void>
   *             promise interface, expect enter error.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_addIccDiallingNumbers_Promise_0900', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_addIccDiallingNumbers_Promise_0900';
    const CONTACT_INFO = { 'recordNumber': 1, 'alphaTag': 'test', 'number': '12345678' };
    const GENERAL_CONTACT_INFO = Object.assign({}, { 'pin2': env.INCORRECT_PIN2 }, CONTACT_INFO);
    try {
      await sim.addIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, GENERAL_CONTACT_INFO);
      console.log(`${CASE_NAME} test fail.`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish.`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_delIccDiallingNumbers_Async_0900
   * @tc.name    Test sim.delIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers: DiallingNumbersInfo,
   *             callback: AsyncCallback<void>) async callback interface, expect enter error.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_delIccDiallingNumbers_Async_0900', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_delIccDiallingNumbers_Async_0900';
    const CONTACT_INFO = { 'recordNumber': 1, 'alphaTag': 'test', 'number': '12345678' };
    sim.delIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, CONTACT_INFO, error => {
      if (error) {
        console.log(`${CASE_NAME} expect error: ${error.message}`);
        console.log(`${CASE_NAME} finish.`);
      } else {
        console.log(`${CASE_NAME} test fail.`);
        expect().assertFail();
      }
      done();
    });
  });

  /**
   * @tc.number   Telephony_Sim_delIccDiallingNumbers_Promise_0900
   * @tc.name     Test sim.delIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers: DiallingNumbersInfo):
   *              Promise<void> promise interface to delete record, expect enter error.
   * @tc.desc     Function test
   */
  it('Telephony_Sim_delIccDiallingNumbers_Promise_0900', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_delIccDiallingNumbers_Promise_0900';
    const CONTACT_INFO = { 'recordNumber': 1, 'alphaTag': 'test', 'number': '12345678' };
    try {
      await sim.delIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, CONTACT_INFO);
      console.log(`${CASE_NAME} test fail.`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number   Telephony_Sim_updateIccDiallingNumbers_Async_1200
   * @tc.name     Test sim.updateIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers:
   *              {recordNumber: 1, alphaTag: 'test_Update', number: '87654321', pin2:'123@#ABCD'}, callback:
   *              AsyncCallback<void>) async callback interface, expect enter error.
   * @tc.desc     Function test
   */
  it('Telephony_Sim_updateIccDiallingNumbers_Async_1200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_updateIccDiallingNumbers_Async_1200';
    let updateInfo = { recordNumber: 1, alphaTag: 'test_Update', number: '87654321', pin2: '123@#ABCD' };
    sim.updateIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, updateInfo, error => {
      if (error) {
        console.log(`${CASE_NAME} expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish.`);
        done();
        return;
      }
      console.log(`${CASE_NAME} test fail.`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number   Telephony_Sim_updateIccDiallingNumbers_Promise_1200
   * @tc.name     Test sim.updateIccDiallingNumbers(soltId: 2, type: 1, diallingNumbers:
   *              {recordNumber: 1, alphaTag: 'test_Update', number: '87654321', pin2:'123@#ABCD'})
   *              promise interface, expect enter error.
   * @tc.desc     Function test
   */
  it('Telephony_Sim_updateIccDiallingNumbers_Promise_1200', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_updateIccDiallingNumbers_Promise_1200';
    let updateInfo = { recordNumber: 1, alphaTag: 'test_Update', number: '87654321', pin2: '123@#ABCD' };
    try {
      console.log(`${CASE_NAME} update`);
      await sim.updateIccDiallingNumbers(env.SLOTID2, sim.GENERAL_CONTACT, updateInfo);
      console.log(`${CASE_NAME} test fail.`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish.`);
    }
    done();
  });

  /**
   * @tc.number  Telephony_Sim_getSimTelephoneNumber_Async_0600
   * @tc.name    Test getSimTelephoneNumber slotId exception input parameter 2,
   *             check the callback value, and expect to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getSimTelephoneNumber_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimTelephoneNumber_Async_0600';
    sim.getSimTelephoneNumber(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getSimTelephoneNumber_Promise_0600
   * @tc.name    Test getSimTelephoneNumber slotId exception input parameter 2, check the callback
   *             value, and expect to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getSimTelephoneNumber_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getSimTelephoneNumber_Promise_0600';
    let data;
    try {
      data = await sim.getSimTelephoneNumber(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    console.log(`${CASE_NAME}, data : ${data}`);
    expect().assertFail();
    done();
  });

  /**
   * @tc.number  Telephony_Sim_getVoiceMailIdentifier_Async_0600
   * @tc.name    Test getVoiceMailIdentifier slotId exception input parameter 2, check the
   *             callback value, and expect to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getVoiceMailIdentifier_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getVoiceMailIdentifier_Async_0600';
    sim.getVoiceMailIdentifier(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getVoiceMailIdentifier_Promise_0600
   * @tc.name    Test getVoiceMailIdentifier slotId exception input parameter 2, check the callback
   *             value, and expect to enter err
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getVoiceMailIdentifier_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getVoiceMailIdentifier_Promise_0600';
    let data;
    try {
      data = await sim.getVoiceMailIdentifier(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    console.log(`${CASE_NAME}, data : ${data}`);
    expect().assertFail();
    done();
  });

  /**
   * @tc.number  Telephony_Sim_getVoiceMailNumber_Async_0600
   * @tc.name    Test getVoiceMailNumber slotId exception input parameter 2, check the callback
   *             value, and expect to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getVoiceMailNumber_Async_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getVoiceMailNumber_Async_0600';
    sim.getVoiceMailNumber(env.SLOTID2, (err, data) => {
      if (err) {
        console.log(`${CASE_NAME} finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} , data : ${data}`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getVoiceMailNumber_Promise_0600
   * @tc.name    Test getVoiceMailNumber slotId exception input parameter 2, check the callback
   *             value, and expect to enter ERR
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getVoiceMailNumber_Promise_0600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_getVoiceMailNumber_Promise_0600';
    let data;
    try {
      data = await sim.getVoiceMailNumber(env.SLOTID2);
    } catch (err) {
      console.log(`${CASE_NAME} finish`);
      done();
      return;
    }
    console.log(`${CASE_NAME}, data : ${data}`);
    expect().assertFail();
    done();
  });

  /**
   * @tc.number Telephony_Sim_setVoiceMailInfo_Async_0400
   * @tc.name   Test the setVoiceMailInfo async callback interface, slotId is 2, expect enter error.
   * @tc.desc   Function test
   */
  it('Telephony_Sim_setVoiceMailInfo_Async_0400', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_setVoiceMailInfo_Async_0400';
    sim.setVoiceMailInfo(env.SLOTID2, env.MAIL_NAME_LEN12, env.MAIL_NUMBER_LEN20, error => {
      if (error) {
        console.log(`${CASE_NAME} setVoiceMailInfo expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish`);
      } else {
        console.log(`${CASE_NAME} test fail.`);
        expect().assertFail();
      }
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_setVoiceMailInfo_Promise_0400
   * @tc.name   Test the setVoiceMailInfo promise interface, slotId is 2, expect enter error.
   * @tc.desc   Function test
   */
  it('Telephony_Sim_setVoiceMailInfo_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_setVoiceMailInfo_Promise_0400';
    try {
      await sim.setVoiceMailInfo(env.SLOTID2, env.MAIL_NAME_LEN12, env.MAIL_NUMBER_LEN20);
      console.log(`${CASE_NAME} test fail.`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} setVoiceMailInfo expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_getMaxSimCount_0100
   * @tc.name   Test the getMaxSimCount interface and view the callback result
   * @tc.desc   Function test
   */
  it('Telephony_Sim_getMaxSimCount_0100', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_getMaxSimCount_0100';
    let simCount = sim.getMaxSimCount();
    expect(simCount > env.SIM_MIN_COUNT).assertTrue();
    console.log(`${CASE_NAME} test finish.`);
    done();
  });

  /**
   * @tc.number Telephony_Sim_GetLockState_Async_0300
   * @tc.name   Test the GetLockState async callback interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_GetLockState_Async_0300', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_GetLockState_Async_0300';
    sim.getLockState(env.SLOTID2, sim.PIN_LOCK, error => {
      if (error) {
        console.log(`${CASE_NAME} getLockState expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_GetLockState_Promise_0300
   * @tc.name   Test the GetLockState promise interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_GetLockState_Promise_0300', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_GetLockState_Promise_0300';
    try {
      await sim.getLockState(env.SLOTID2, sim.PIN_LOCK);
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} getLockState expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_GetCardType_Async_0400
   * @tc.name   Verify that the slotId of the GetCardType async callback interface is abnormal
   *            and enter parameter 2, and check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_GetCardType_Async_0400', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_GetCardType_Async_0400';
    sim.getCardType(env.SLOTID2, (error, cardType) => {
      if (error) {
        console.log(`${CASE_NAME} GetCardType error: ${error.message}`);
        expect().assertFail();
        done();
        return;
      }
      expect(cardType === sim.UNKNOWN_CARD).assertTrue();
      console.log(`${CASE_NAME} test finish.`);
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_GetCardType_Promise_0400
   * @tc.name   Verify that the slotId of the GetCardType promise interface is abnormal
   *            and enter parameter 2, and check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_GetCardType_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_GetCardType_Promise_0400';
    try {
      const cardType = await sim.getCardType(env.SLOTID2);
      expect(cardType === sim.UNKNOWN_CARD).assertTrue();
      console.log(`${CASE_NAME} test finish.`);
    } catch (error) {
      console.log(`${CASE_NAME} GetCardType error: ${error.message}`);
      expect().assertFail();
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_SendEnvelopeCmd_Async_0300
   * @tc.name   Test SendEnvelopeCmd async callback interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SendEnvelopeCmd_Async_0300', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_SendEnvelopeCmd_Async_0300';
    sim.sendEnvelopeCmd(env.SLOTID2, env.STK_CMD, (error) => {
      if (error) {
        console.log(`${CASE_NAME} SendEnvelopeCmd expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_SendEnvelopeCmd_Promise_0300
   * @tc.name   Test SendEnvelopeCmd promise interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SendEnvelopeCmd_Promise_0300', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_SendEnvelopeCmd_Promise_0300';
    try {
      await sim.sendEnvelopeCmd(env.SLOTID2, env.STK_CMD);
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} SendEnvelopeCmd expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_SendTerminalResponseCmd_Async_0300
   * @tc.name   Test SendTerminalResponseCmd async callback interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SendTerminalResponseCmd_Async_0300', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_SendTerminalResponseCmd_Async_0300';
    sim.sendTerminalResponseCmd(env.SLOTID2, env.STK_CMD, (error) => {
      if (error) {
        console.log(`${CASE_NAME} SendTerminalResponseCmd expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish`);
        done();
        return;
      }
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_SendTerminalResponseCmd_Promise_0300
   * @tc.name   Test SendTerminalResponseCmd promise interface, enter parameter 2
   *            for slotId exception, check the callback value
   * @tc.desc   Function test
   */
  it('Telephony_Sim_SendTerminalResponseCmd_Promise_0300', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_SendTerminalResponseCmd_Promise_0300';
    try {
      await sim.sendTerminalResponseCmd(env.SLOTID2, env.STK_CMD);
      console.log(`${CASE_NAME} test fail`);
      expect().assertFail();
    } catch (error) {
      console.log(`${CASE_NAME} SendTerminalResponseCmd expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish`);
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_hasOperatorPrivileges_Async_0400
   * @tc.name   Test hasOperatorPrivileges interface slotId exception enter parameter 2,
   *            check the callback result
   * @tc.desc   Function test
   */
  it('Telephony_Sim_hasOperatorPrivileges_Async_0400', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_hasOperatorPrivileges_Async_0400';
    sim.hasOperatorPrivileges(env.SLOTID2, (error, result) => {
      if (error) {
        console.log(`${CASE_NAME} hasOperatorPrivileges error: ${error.message}`);
        expect().assertFail();
        done();
        return;
      }
      expect(result).assertFalse();
      console.log(`${CASE_NAME} test finish.`);
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_hasOperatorPrivileges_Promise_0400
   * @tc.name   Test hasOperatorPrivileges interface slotId exception enter parameter 2,
   *            check the returned result
   * @tc.desc   Function test
   */
  it('Telephony_Sim_hasOperatorPrivileges_Promise_0400', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_hasOperatorPrivileges_Promise_0400';
    try {
      const result = await sim.hasOperatorPrivileges(env.SLOTID2);
      expect(result).assertFalse();
      console.log(`${CASE_NAME} test finish.`);
    } catch (error) {
      console.log(`${CASE_NAME} hasOperatorPrivileges error: ${error.message}`);
      expect().assertFail();
    }
    done();
  });

  /**
   * @tc.number Telephony_Sim_unlockSimLock_Async_2600
   * @tc.name   Test the unlocksimLock interface slotId exception into parameter 2, view the callback result
   * @tc.desc   Function test
   */
  it('Telephony_Sim_unlockSimLock_Async_2600', 0, function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockSimLock_Async_2600';
    const lockInfo = { lockType: sim.PN_PIN_LOCK, password: env.SIM_PN_PIN_PASSWORD };
    sim.unlockSimLock(env.SLOTID2, lockInfo, async (error) => {
      if (error) {
        console.log(`${CASE_NAME} expect error: ${error.message}`);
        console.log(`${CASE_NAME} test finish.`);
        done();
        return;
      }
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
      done();
    });
  });

  /**
   * @tc.number Telephony_Sim_unlockSimLock_Promise_2600
   * @tc.name   Test the unlocksimLock interface slotId exception into parameter 2, view the return result
   * @tc.desc   Function test
   */
  it('Telephony_Sim_unlockSimLock_Promise_2600', 0, async function (done) {
    const CASE_NAME = 'Telephony_Sim_unlockSimLock_Promise_2600';
    const lockInfo = { lockType: sim.PN_PIN_LOCK, password: env.SIM_PN_PIN_PASSWORD };
    try {
      await sim.unlockSimLock(env.SLOTID2, lockInfo);
      expect().assertFail();
      console.log(`${CASE_NAME} test fail.`);
    } catch (error) {
      console.log(`${CASE_NAME} expect error: ${error.message}`);
      console.log(`${CASE_NAME} test finish.`);
    }
    done();
  });
});