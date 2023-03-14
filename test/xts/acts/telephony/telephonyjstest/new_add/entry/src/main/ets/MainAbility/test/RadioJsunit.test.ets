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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import radio from '@ohos.telephony.radio';
import utils from './Utils.ets'

export default function radioJsunit() {
  describe('ActsNetworkSearchTest',function () {

    const SLOT_2 = -1;
    const SLOT_0 = 0;

    /**
     * @tc.number  Telephony_NetworkSearch_getMEID_Async_0100
     * @tc.name    Test getMEID(-1) to view the callback result
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_getMEID_Async_0100', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      radio.getMEID(SLOT_2, (err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_getMEID_Async_0100 fail: ${err}`);
          expect().assertFail();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_getMEID_Async_0100 end data: ${data}`);
        expect(data === '').assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_NetworkSearch_getMEID_Promise_0100
     * @tc.name    Test getMEID(-1) to check the callback result
     * @tc.desc    FunctigetMEIDon test
     */
    it('Telephony_NetworkSearch_getMEID_Promise_0100', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      radio.getMEID(SLOT_2).then(data => {
        console.log(`Telephony_NetworkSearch_getMEID_Promise_0100 end data: ${data}`);
        expect(data === '').assertTrue();
        done();
      }).catch(err => {
        console.log(`Telephony_NetworkSearch_getMEID_Promise_0100 fail: ${err}`);
        expect().assertFail();
        done();
      })
    });

    /**
     * @tc.number  Telephony_NetworkSearch_getPrimarySlotId_Async_0200
     * @tc.name    Test getPrimarySlotId(-1) to check the callback result
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_getPrimarySlotId_Async_0200', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      radio.getPrimarySlotId((err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_getPrimarySlotId_Async_0200 fail: ${err}`);
          expect(data === undefined).assertTrue();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_getPrimarySlotId_Async_0200 end data: ${data}`);
        expect(data === '').assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_NetworkSearch_setPrimarySlotId_Async_0300
     * @tc.name    Test The function setPrimarySlotId(0)
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_setPrimarySlotId_Async_0300', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      radio.setPrimarySlotId(SLOT_0, (err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_setPrimarySlotId_Async_0300 fail: ${err}`);
          expect(data === undefined).assertTrue();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_setPrimarySlotId_Async_0300 end data: ${data}`);
        expect(data === '').assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_NetworkSearch_setPrimarySlotId_Async_0300
     * @tc.name    Test The function setPrimarySlotId(0)
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_setPrimarySlotId_Promise_0300', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      try {
        await radio.setPrimarySlotId(SLOT_0);
        console.log('Telephony_NetworkSearch_setPrimarySlotId_Promise_0300 success');
        done();
      } catch (err) {
        console.log(`Telephony_NetworkSearch_setPrimarySlotId_Promiss_0300 fail ${err}`);
        done();
      }
    });

    /**
     * @tc.number  Telephony_NetworkSearch_isNrSupported_Promise_0400
     * @tc.name    Test The function setPrimarySlotId(0)
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_isNrSupported_Promise_0400', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      try {
        await radio.isNrSupported(SLOT_0);
        console.log('radio.isNrSupported(SLOT_0) = ' + radio.isNrSupported(SLOT_0));
        console.log('Telephony_NetworkSearch_isNrSupported_Promise_0400 success');
        done();
      } catch (err) {
        console.log(`Telephony_NetworkSearch_isNrSupported_Promise_0400 fail ${err}`);
        done();
      }
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_0500
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_0500', 0, async function (done) {
      console.info("Telephony_radio_PreferredNetworkMode_0500 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_WCDMA === 2).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE === 3).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_WCDMA === 4).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_WCDMA_GSM === 5).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_WCDMA_GSM === 6).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_0500 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_0600
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_0600', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_0600 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_CDMA === 7).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_EVDO === 8).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_EVDO_CDMA === 9).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_WCDMA_GSM_EVDO_CDMA === 10).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_EVDO_CDMA === 11).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_0600 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_0700
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_0700', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_0700 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_WCDMA_GSM_EVDO_CDMA === 12).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA === 13).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA_GSM === 14).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA === 15).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA_GSM === 16).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_0700 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_0800
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_0800', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_0800 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA === 17).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_GSM === 18).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA === 19).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM === 20).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA_GSM_EVDO_CDMA ===21).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_0800 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_0900
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_0900', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_0900 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA === 17).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_GSM === 18).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA === 19).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM === 20).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_TDSCDMA_WCDMA_GSM_EVDO_CDMA ===21).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_0900 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_1000
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_1000', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_1000 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA === 22).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR === 31).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE === 32).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_WCDMA === 33).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_WCDMA_GSM ===34).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_1000 end")
      done();
    });

    /**
     * @tc.number  Telephony_radio_PreferredNetworkMode_1100
     * @tc.name    Test The  PreferredNetworkMode enum
     * @tc.desc    Function test
     */
    it('Telephony_radio_PreferredNetworkMode_1100', 0, async function (done) {
      if(utils.notCheck){
        done();
        return;
      }
      console.info("Telephony_radio_PreferredNetworkMode_1100 start")
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_EVDO_CDMA === 35).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_WCDMA_GSM_EVDO_CDMA === 36).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA === 37).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_GSM === 38).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA ===39).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM === 40).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA === 41).assertTrue();
      expect(PreferredNetworkMode.PREFERRED_NETWORK_MODE_MAX_VALUE === 99).assertTrue();
      console.info("Telephony_radio_PreferredNetworkMode_1100 end")
      done();
    });

  })
}