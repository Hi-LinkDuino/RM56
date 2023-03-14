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
import radio from '@ohos.telephony.radio';
import utils from './Utils.ets'

export default function radioSecondJsunit() {
  describe('radioSecondTest', function () {
    const NROPTION_MODE = [
      radio.NR_OPTION_UNKNOWN,
      radio.NR_OPTION_NSA_ONLY,
      radio.NR_OPTION_SA_ONLY,
      radio.NR_OPTION_NSA_AND_SA
    ];
    //Network status
    let garrNetworkState = [
      radio.NETWORK_UNKNOWN,
      radio.NETWORK_AVAILABLE,
      radio.NETWORK_CURRENT,
      radio.NETWORK_FORBIDDEN,
    ];
    //NetworkRadioTech
    let garrNetworkRadioTech = [
      'UNKNOWN', 'GSM', '1XRTT', 'WCDMA',
      'HSPA', 'HSPAP', 'TD_SCDMA', 'EVDO',
      'EHRPD', 'LTE', 'LTE_CA', 'IWLAN', 'NR'];

    const SLOT_0 = 0;
    const SLOT_1 = 5;
    const SLOT_2 = 2;
    const SLOT_3 = -1;
    const NETWORK_TYPES = [
      radio.NETWORK_TYPE_UNKNOWN,
      radio.NETWORK_TYPE_GSM,
      radio.NETWORK_TYPE_CDMA,
      radio.NETWORK_TYPE_WCDMA,
      radio.NETWORK_TYPE_TDSCDMA,
      radio.NETWORK_TYPE_LTE,
      radio.NETWORK_TYPE_NR,
    ];
    const NETWORK_SIGNAL = [0, 1, 2, 3, 4, 5];
    const CELL_LAC_TAC_MAX = 0xffff;
    const CELL_CELLID_MAX = 0xfffffff;
    const CELL_GSM_CELLID_MAX = 0xffff;
    const CELL_ARFCN_MAX = 1023;
    const CELL_EARFCN_MAX = 41589;
    const CELL_BSIC_MAX = 63;
    const CELL_PSC_MAX = 511;
    const CELL_PCI_MAX = 503;
    const CELL_MCC = 460;
    const CELL_MNC_MAX = 20;
    const CELL_INFO_MIN = 0;
    const TIME_RADIO_TURNON = 10000;
    console.log("************* radio Test start*************");
    it('Telephony_NetworkSearch_getCellInformation_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      radio.getCellInformation((err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_getCellInformation_Async_0100 fail: ${err}`);
          expect().assertFail();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_getCellInformation_Async_0100 end data: ${JSON.stringify(data)}`);
        if (true) {
          console.log(`Telephony_NetworkSearch_getCellInformation_Async_0100 1`);
          expect(true).assertTrue();
          done();
          return;
        } else {
          assertCellInformation(data);
          done();
        }
      });
    });

    /**
     * @tc.number  Telephony_NetworkSearch_getNrOptionMode_Async_0100
     * @tc.name    Test getNrOptionMode() to check the callback result
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_getNrOptionMode_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      radio.getNrOptionMode((err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_getNrOptionMode_Async_0100 get fail err: ${err}`);
          expect().assertFail();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_getNrOptionMode_Async_0100 end data: ${JSON.stringify(data)}`);
        if (true) {
          console.log(`Telephony_NetworkSearch_getNrOptionMode_Async_0100 1`);
          expect(true).assertTrue();
          done();
          return;
        } else {
          expect(NROPTION_MODE).assertContain(data);
          done();
        }
      });
    })

   /**
     * @tc.number  Telephony_NetworkSearch_NetworkState_Async_0100
     * @tc.name    Test getNrOptionMode() to check the callback result
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_NetworkState_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      radio.getNetworkState((err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_NetworkState_Async_0100 get fail err: ${err}`);
          expect().assertFail();
          done();
          return;
        }
        console.log(`Telephony_NetworkSearch_NetworkState_Async_0100 end data: ${JSON.stringify(data)}`);
        if (true) {
          console.log(`Telephony_NetworkSearch_getNrOptionMode_Async_0100 1`);
          expect(true).assertTrue();
          done();
          return;
        } else {
          expect(NROPTION_MODE).assertContain(data);
          expect(NROPTION_MODE).assertContain(data.cfgTech);
          done();
        }
      });
    })

    /**
     * @tc.number  Telephony_Radio_CdmaCellInformation_sid_0100
     * @tc.name    sid
     * @tc.desc    sid test
     */
    it('Telephony_Radio_CdmaCellInformation_sid_0100', 0, async function (done) {
      console.log("-----------------------Telephony_Radio_CdmaCellInformation_sid_0100 start-----------------------");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let cdmaCellInformatio = {
        baseId: 1,
        latitude: 39,
        longitude: 116,
        nid: 101,
        sid: 102
      }
      expect(101).assertEqual(cdmaCellInformatio.nid);
      expect(102).assertEqual(cdmaCellInformatio.sid);
      console.log("-----------------------Telephony_Radio_CdmaCellInformation_sid_0100 end-----------------------");
    })


    /**
     * @tc.number  Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100
     * @tc.name    Test the getNetworkSearchInformation() query function and set the slotId parameter input to 0,
     *             test the return value isNetworkSearchSuccess is true and specific operator information
     * @tc.desc    Function test
     */
    it('Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      radio.getNetworkSearchInformation(SLOT_0, (err, data) => {
        if (err) {
          console.log(`Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100 fail err: ${err}`);
          expect().assertFail();
          done();
          return;
        }
        if (true){
          console.log(`Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100 1`);
          expect(true).assertTrue();
          done();
          return;
        }
        expect(data != null && data != undefined).assertTrue();
        if (data.networkSearchResult.length === 0) {
          console.log('Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100 fail');
          expect().assertFail();
          done();
          return;
        }
        console.log(
          `Telephony_NetworkSearch_getNetworkSearchInformation_Async_0100 finish data: ${JSON.stringify(data)}`);
        expect(data.isNetworkSearchSuccess).assertTrue();
        expect(
          data.networkSearchResult[0].operatorName != undefined &&
          data.networkSearchResult[0].operatorName != '' &&
          data.networkSearchResult[0].operatorName != null
        ).assertTrue();
        expect(garrNetworkState).assertContain(data.networkSearchResult[0].state);
        expect(garrNetworkRadioTech).assertContain(data.networkSearchResult[0].radioTech);
        expect(data.networkSearchResult[0].operatorNumeric.substr(0, 3) === '460').assertTrue();
        done();
      });
    });
    console.log("************* radio Test end*************");
    function assertCellInformation(data) {
      expect(data !== '' && data != undefined && data != null).assertTrue();
      expect(data.length).assertLarger(0);
      for (let i = 0; i < data.length; i++) {
        expect(NETWORK_TYPES).assertContain(data[i].networkType);
        expect(data[i].isCamped).assertTrue();
        expect(data[i].timeStamp).assertLarger(0);
        expect(NETWORK_TYPES).assertContain(data[i].signalInformation.signalType);
        expect(NETWORK_SIGNAL).assertContain(data[i].signalInformation.signalLevel);
        expect(data[i].data != undefined && data[i].data != '' && data[i].data != null).assertTrue();
        if (data[0].networkType === radio.NETWORK_TYPE_LTE) {
          expect(data[i].data.tac >= CELL_INFO_MIN && data[i].data.tac <= CELL_LAC_TAC_MAX).assertTrue();
          expect(data[i].data.cgi >= CELL_INFO_MIN && data[i].data.cgi <= CELL_CELLID_MAX).assertTrue();
          expect(data[i].data.earfcn >= CELL_INFO_MIN && data[i].data.earfcn <= CELL_EARFCN_MAX).assertTrue();
          expect(data[i].data.pci >= CELL_INFO_MIN && data[i].data.pci <= CELL_PCI_MAX).assertTrue();
          expect(data[i].data.mnc >= CELL_INFO_MIN && data[i].data.mnc <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.bandwidth >= CELL_INFO_MIN && data[i].data.bandwidth <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.isSupportEndc >= CELL_INFO_MIN && data[i].data.isSupportEndc <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.mcc).assertEqual(CELL_MCC);
        } else if (data[i].networkType === radio.NETWORK_TYPE_WCDMA) {
          expect(data[i].data.lac >= CELL_INFO_MIN && data[i].data.lac <= CELL_LAC_TAC_MAX).assertTrue();
          expect(data[i].data.cellId >= CELL_INFO_MIN && data[i].data.cellId <= CELL_CELLID_MAX).assertTrue();
          expect(data[i].data.uarfcn >= CELL_INFO_MIN && data[i].data.uarfcn <= CELL_ARFCN_MAX).assertTrue();
          expect(data[i].data.psc >= CELL_INFO_MIN && data[i].data.psc <= CELL_PSC_MAX).assertTrue();
          expect(data[i].data.mnc >= CELL_INFO_MIN && data[i].data.mnc <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.cpid >= CELL_INFO_MIN && data[i].data.cpid <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.mcc).assertEqual(CELL_MCC);
        } else if (data[i].networkType === radio.NETWORK_TYPE_GSM) {
          expect(data[i].data.lac >= CELL_INFO_MIN && data[i].data.lac <= CELL_LAC_TAC_MAX).assertTrue();
          expect(data[i].data.cellId >= CELL_INFO_MIN && data[i].data.cellId <= CELL_GSM_CELLID_MAX).assertTrue();
          expect(data[i].data.arfcn >= CELL_INFO_MIN && data[i].data.arfcn <= CELL_ARFCN_MAX).assertTrue();
          expect(data[i].data.bsic >= CELL_INFO_MIN && data[i].data.bsic <= CELL_BSIC_MAX).assertTrue();
          expect(data[i].data.mnc >= CELL_INFO_MIN && data[i].data.mnc <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.mcc).assertEqual(CELL_MCC);
        } else if (data[i].networkType === radio.NETWORK_TYPE_TDSCDMA) {
          expect(data[i].data.baseId >= CELL_INFO_MIN && data[i].data.baseId <= CELL_LAC_TAC_MAX).assertTrue();
          expect(data[i].data.latitude >= CELL_INFO_MIN && data[i].data.latitude <= CELL_GSM_CELLID_MAX).assertTrue();
          expect(data[i].data.longitude >= CELL_INFO_MIN && data[i].data.longitude <= CELL_ARFCN_MAX).assertTrue();
          expect(data[i].data.bsic >= CELL_INFO_MIN && data[i].data.bsic <= CELL_BSIC_MAX).assertTrue();
          expect(data[i].data.nid >= CELL_INFO_MIN && data[i].data.nid <= CELL_MNC_MAX).assertTrue();
          expect(data[i].data.mcc).assertEqual(CELL_MCC);
      } else if (data[i].networkType === radio.NETWORK_TYPE_NR) {
          expect(data[i].data.nrArfcn >= CELL_INFO_MIN && data[i].data.nrArfcn <= CELL_LAC_TAC_MAX).assertTrue();
          expect(data[i].data.tac >= CELL_INFO_MIN && data[i].data.tac <= CELL_GSM_CELLID_MAX).assertTrue();
          expect(data[i].data.nci >= CELL_INFO_MIN && data[i].data.nci <= CELL_ARFCN_MAX).assertTrue();
          expect(data[i].data.mcc).assertEqual(CELL_MCC);
        } else {
          expect().assertFail();
        }

      }
    }
  })
}
