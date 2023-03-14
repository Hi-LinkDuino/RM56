// @ts-nocheck
/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import sim from '@ohos.telephony.sim';
import * as env from './lib/Const.js';
import utils from './Utils.ets'
export default function simJsunit() {
  describe('appInfoTest', function () {
    console.log("************* sim Test start*************");
    /**
   * @tc.number  Telephony_Sim_getOperatorConfigs_Async_0100
   * @tc.name    The slotId parameter is 0 and the OperatorConfig structure is not expected to be empty
   * @tc.desc    Function test
   */
    it('Telephony_Sim_getOperatorConfigs_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      console.log('Telephony_Sim_getOperatorConfigs_Async_0100 start');
      let containerIsEmpty = 0;
      sim.getOperatorConfigs(env.DEFAULT_SLOTID, (err, data) => {
        console.log('Telephony_Sim_getOperatorConfigs_Async_0100 result');
        if (err) {
          console.log(`Telephony_Sim_getOperatorConfigs_Async_0100 fail, err : ${err.message}`);
          done();
          return;
        }
        expect(Object.keys(data).length !== containerIsEmpty).assertTrue();
        expect(data.field !== containerIsEmpty).assertTrue();
        expect(data.value !== containerIsEmpty).assertTrue();
        console.log('Telephony_Sim_getOperatorConfigs_Async_0100 finish');
        done();
      });
    });

    /**
    * @tc.number  Telephony_Sim_getSimAccountInfo_Async_0100
    * @tc.name    Get the return value by calling the GetSimAccountInfo interface SIM ID input parameter 0.
    * @tc.desc    Function test
    */
    it('Telephony_Sim_getSimAccountInfo_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      const CASE_NAME = 'Telephony_Sim_getSimAccountInfo_Async_0100';
      let iccIdValue;
      sim.getSimAccountInfo(env.DEFAULT_SLOTID, (err, info) => {
        if (err) {
          console.log(`${CASE_NAME} fail ,err : ${err.message}`);
          done();
          return;
        }
        console.log(`${CASE_NAME} getSimAccountInfo: ${JSON.stringify(info)}`);
        expect(info.slotIndex === env.DEFAULT_SLOTID
        && info.isEsim === false
        && info.isActive === true
        && info.iccId === iccIdValue
        && info.showName === env.SIM_SLOT_NAME
        && info.showNumber === env.SIM_SLOT_NUMBER
        && info.simId === "simId"
        ).assertTrue();
        console.log(`${CASE_NAME} finish`);
        done();
      });
    });
    /**
    * @tc.number  Telephony_Sim_unlockPin_Async_0100
    * @tc.name    Test the unlockPin interface when the card status is ready state.
    * @tc.desc    Function test
    */
    it('Telephony_Sim_unlockPin_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let PIN = '1234';
      sim.unlockPin(env.DEFAULT_SLOTID, PIN, (err, data) => {
        if (err) {
          console.log(`Telephony_Sim_unlockPin_Async_0100 fail, err: ${err.message}`);
          done();
          return;
        }
        expect(data.result === env.OTHER_ABNORMAL).assertTrue();
        console.log('Telephony_Sim_unlockPin_Async_0100 finish');
        done();
      });
    });
    /**
   * @tc.number  Telephony_Sim_addIccDiallingNumbers_Async_0100
   * @tc.name    Test sim.addIccDiallingNumbers(soltId: -1, type: 1, diallingNumbers:
   *             {recordNumber: 1, alphaTag: 'test', number: '12345678', pin2: '123@#ABCD'},
   *             callback: AsyncCallback<void>) async callback interface, expect enter error.
   * @tc.desc    Function test
   */
    const CONTACT_INFO = { 'recordNumber': 1, 'alphaTag': 'test', 'number': '12345678' };
    const GENERAL_CONTACT_INFO = Object.assign({}, { 'pin2': env.INCORRECT_PIN2 }, CONTACT_INFO);
    it('Telephony_Sim_addIccDiallingNumbers_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      const CASE_NAME = 'Telephony_Sim_addIccDiallingNumbers_Async_0100';
      var number = env.SLOTID_MINUS1;
      sim.addIccDiallingNumbers(number,
        sim.GENERAL_CONTACT,
        GENERAL_CONTACT_INFO,
        error => {
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
   * @tc.number Telephony_Sim_SetLockState_Async_0100
   * @tc.name   Test the SetLockState async callback interface to open the PIN lock and check the callback value
   * @tc.desc   Function test
   */
    it('Telephony_Sim_SetLockState_Async_0100', 0, async function (done) {
      const CASE_NAME = 'Telephony_Sim_SetLockState_Async_0100';
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      const lockInfo = { lockType: sim.PIN_LOCK, state: sim.LOCK_ON, password: env.CORRECT_PIN };
      await setPinLockState(sim.LOCK_OFF);
      sim.setLockState(env.DEFAULT_SLOTID, lockInfo, (error, lockStatusResponse) => {
        if (error) {
          console.log(`${CASE_NAME} setLockState error: ${error.message}`);
          expect().assertFail();
          done();
          return;
        }
        if (lockStatusResponse.result !== env.LOCK_RESULT.SUCCESS) {
          console.log(`${CASE_NAME} setLockState failure, lock status response: ${lockStatusResponse.result}`);
          expect().assertFail();
          done();
          return;
        }
      });
    });
    console.log("************* sim Test end*************");
  })
}
