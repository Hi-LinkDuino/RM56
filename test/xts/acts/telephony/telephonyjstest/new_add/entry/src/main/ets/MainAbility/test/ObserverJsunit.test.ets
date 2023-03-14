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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import observer from '@ohos.telephony.observer'
import cellular from '@ohos.telephony.data'
import radio from '@ohos.telephony.radio'
import utils from './Utils.ets'
import {SimStateData} from '@ohos.telephony.observer'
import {LockReason} from '@ohos.telephony.observer'
import sim from "@ohos.telephony.sim"


export default function observerJsunit() {
  describe('ObserverTest', function () {
    console.log("************* Observer Test start *************")

    const SLOT_0 = 0;
    const SLOT_2 = -1;

    /*
     * @tc.number    : Telephony_observer_observer_DataConnectState_0100
     * @tc.name      : DataConnectState
     * @tc.desc      : check the getCellularDataState callback to verify the call back data
     */
    it('Telephony_observer_observer_DataConnectState_0100', 0, function (done) {
      console.log("************* Telephony_observer_observer_DataConnectState_0100 Test start *************")
      cellular.getCellularDataState((err, data) => {
        if (!err) {
          var dataConnectStateList = [cellular.DataConnectState.DATA_STATE_UNKNOWN,
          cellular.DataConnectState.DATA_STATE_DISCONNECTED,
          cellular.DataConnectState.DATA_STATE_CONNECTING,
          cellular.DataConnectState.DATA_STATE_CONNECTED,
          cellular.DataConnectState.DATA_STATE_SUSPENDED]
          expect(dataConnectStateList.indexOf(data) >= 0).assertTrue()
          done();
          return;
        }
        console.log("Telephony_CellularData_getCellularDataState_0100 end");
        done()
      })
      console.log("************* Telephony_observer_observer_DataConnectState_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_observer_DataConnectState_0200
     * @tc.name      : DataConnectState
     * @tc.desc      : check the getCellularDataState callback to verify the call back data
     */
    it('Telephony_observer_observer_DataConnectState_0200', 0, function (done) {
      console.log("************* Telephony_observer_observer_DataConnectState_0200 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      console.log(observer.DataConnectState)
      console.log("************* Telephony_observer_observer_DataConnectState_0200 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_observer_RatType_0100
     * @tc.name      : RatType
     * @tc.desc      : Obtains the data connect state
     */
    it('Telephony_observer_observer_RatType_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_RatType_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      console.log(observer.RatType)
      console.log("************* Telephony_observer_observer_RatType_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_networkStateChange_0100
     * @tc.name      : on_networkStateChange
     * @tc.desc      : call the on method of networkStateChange
     */
    it('Telephony_observer_observer_on_networkStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_networkStateChange_0100 Test start *************")
      observer.on('networkStateChange', (networkState) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_networkStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_networkStateChange_0200
     * @tc.name      : on_networkStateChange
     * @tc.desc      : call the on method of networkStateChange
     */
    it('Telephony_observer_observer_on_networkStateChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_networkStateChange_0200 Test start *************")
      observer.on('networkStateChange', {
        slotId: SLOT_0
      }, (networkState) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_networkStateChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_networkStateChange_0100
     * @tc.name      : off_networkStateChange
     * @tc.desc      : call the off method of networkStateChange
     */
    it('Telephony_observer_observer_off_networkStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_networkStateChange_0100 Test start *************")
      observer.off('networkStateChange', (networkState) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_networkStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_signalInfoChange_0100
     * @tc.name      : on_signalInfoChange
     * @tc.desc      : call the on method of signalInfoChange
     */
    it('Telephony_observer_observer_on_signalInfoChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_signalInfoChange_0100 Test start *************")
      observer.on('signalInfoChange', (signalInformation) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_signalInfoChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_signalInfoChange_0200
     * @tc.name      : on_signalInfoChange
     * @tc.desc      : call the on method of signalInfoChange
     */
    it('Telephony_observer_observer_on_signalInfoChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_signalInfoChange_0200 Test start *************")
      observer.on('signalInfoChange', {
        slotId: SLOT_0
      }, (signalInformation) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_signalInfoChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_signalInfoChange_0100
     * @tc.name      : off_signalInfoChange
     * @tc.desc      : call the off method of signalInfoChange
     */
    it('Telephony_observer_observer_off_signalInfoChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_signalInfoChange_0100 Test start *************")
      observer.off('signalInfoChange', (networkState) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_signalInfoChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellInfoChange_0100
     * @tc.name      : on_cellInfoChange
     * @tc.desc      : call the on method of cellInfoChange
     */
    it('Telephony_observer_observer_on_cellInfoChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellInfoChange_0100 Test start *************")
      observer.on('cellInfoChange', (cellInformation) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellInfoChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellInfoChange_0200
     * @tc.name      : on_cellInfoChange
     * @tc.desc      : call the on method of cellInfoChange
     */
    it('Telephony_observer_observer_on_cellInfoChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellInfoChange_0200 Test start *************")
      observer.on('cellInfoChange', {
        slotId: SLOT_0
      }, (cellInformation) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellInfoChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_cellInfoChange_0100
     * @tc.name      : off_cellInfoChange
     * @tc.desc      : call the off method of cellInfoChange
     */
    it('Telephony_observer_observer_off_cellInfoChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_cellInfoChange_0100 Test start *************")
      observer.off('cellInfoChange', (cellInformation) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_cellInfoChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellularDataConnectionStateChange_0100
     * @tc.name      : on_cellularDataConnectionStateChange
     * @tc.desc      : call the on method of cellularDataConnectionStateChange
     */
    it('Telephony_observer_observer_on_cellularDataConnectionStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellularDataConnectionStateChange_0100 Test start *************")
      observer.on('cellularDataConnectionStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellularDataConnectionStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellularDataConnectionStateChange_0200
     * @tc.name      : on_cellularDataConnectionStateChange
     * @tc.desc      : call the on method of cellularDataConnectionStateChange
     */
    it('Telephony_observer_observer_on_cellularDataConnectionStateChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellularDataConnectionStateChange_0200 Test start *************")
      observer.on('cellularDataConnectionStateChange', {
        slotId: SLOT_0
      }, (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellularDataConnectionStateChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_cellularDataConnectionStateChange_0100
     * @tc.name      : off_cellularDataConnectionStateChange
     * @tc.desc      : call the off method of cellularDataConnectionStateChange
     */
    it('Telephony_observer_observer_off_cellularDataConnectionStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_cellularDataConnectionStateChange_0100 Test start *************")
      observer.off('cellularDataConnectionStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_cellularDataConnectionStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellularDataFlowChange_0100
     * @tc.name      : on_cellularDataFlowChange
     * @tc.desc      : call the on method of cellularDataFlowChange
     */
    it('Telephony_observer_observer_on_cellularDataFlowChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellularDataFlowChange_0100 Test start *************")
      observer.on('cellularDataFlowChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellularDataFlowChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_cellularDataFlowChange_0200
     * @tc.name      : on_cellularDataFlowChange
     * @tc.desc      : call the on method of cellularDataFlowChange
     */
    it('Telephony_observer_observer_on_cellularDataFlowChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_cellularDataFlowChange_0200 Test start *************")
      observer.on('cellularDataFlowChange', {
        slotId: SLOT_0
      }, (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_cellularDataFlowChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_cellularDataFlowChange_0100
     * @tc.name      : off_cellularDataFlowChange
     * @tc.desc      : call the off method of cellularDataFlowChange
     */
    it('Telephony_observer_observer_off_cellularDataFlowChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_cellularDataFlowChange_0100 Test start *************")
      observer.off('cellularDataFlowChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_cellularDataFlowChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_callStateChange_0100
     * @tc.name      : on_callStateChange
     * @tc.desc      : call the on method of callStateChange
     */
    it('Telephony_observer_observer_on_callStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_callStateChange_0100 Test start *************")
      observer.on('callStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_callStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_callStateChange_0200
     * @tc.name      : on_callStateChange
     * @tc.desc      : call the on method of callStateChange
     */
    it('Telephony_observer_observer_on_callStateChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_callStateChange_0200 Test start *************")
      observer.on('callStateChange', {
        slotId: SLOT_0
      }, (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_callStateChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_callStateChange_0100
     * @tc.name      : off_callStateChange
     * @tc.desc      : call the off method of callStateChange
     */
    it('Telephony_observer_observer_off_callStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_callStateChange_0100 Test start *************")
      observer.off('callStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_callStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_simStateChange_0100
     * @tc.name      : on_simStateChange
     * @tc.desc      : call the on method of simStateChange
     */
    it('Telephony_observer_observer_on_simStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_simStateChange_0100 Test start *************")
      observer.on('simStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_simStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_on_simStateChange_0200
     * @tc.name      : on_simStateChange
     * @tc.desc      : call the on method of simStateChange
     */
    it('Telephony_observer_observer_on_simStateChange_0200', 0, async function (done) {
      console.log("************* Telephony_observer_observer_on_simStateChange_0200 Test start *************")
      observer.on('simStateChange', {
        slotId: SLOT_0
      }, (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_on_simStateChange_0200 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_observer_off_simStateChange_0100
     * @tc.name      : off_simStateChange
     * @tc.desc      : call the off method of simStateChange
     */
    it('Telephony_observer_observer_off_simStateChange_0100', 0, async function (done) {
      console.log("************* Telephony_observer_observer_off_simStateChange_0100 Test start *************")
      observer.off('simStateChange', (data) => {
        expect().assertFail()
        done()
        return
      })
      setTimeout(timeout, 3000)
      console.log("************* Telephony_observer_observer_off_simStateChange_0100 Test end *************")
      done()
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_type_0100
     * @tc.name      : type
     * @tc.desc      : check the type value of SimStateData object
     */
    it('Telephony_observer_SimStateData_type_0100', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_type_0100 Test start *************")
      const unknown: SimStateData = {
        type: sim.CardType.UNKNOWN_CARD
      }
      expect(-1).assertEqual(unknown.type)

      const singleMode: SimStateData = {
        type: sim.CardType.SINGLE_MODE_SIM_CARD
      }
      expect(10).assertEqual(singleMode.type)

      const singleModeUSim: SimStateData = {
        type: sim.CardType.SINGLE_MODE_USIM_CARD
      }
      expect(20).assertEqual(singleModeUSim.type)
      done()
      console.log("************* Telephony_observer_SimStateData_type_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_type_0200
     * @tc.name      : type
     * @tc.desc      : check the type value of SimStateData object
     */
    it('Telephony_observer_SimStateData_type_0200', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_type_0200 Test start *************")
      const singleModeRUim: SimStateData = {
        type: sim.CardType.SINGLE_MODE_RUIM_CARD
      }
      expect(30).assertEqual(singleModeRUim.type)

      const dualModeCG: SimStateData = {
        type: sim.CardType.DUAL_MODE_CG_CARD
      }
      expect(40).assertEqual(dualModeCG.type)

      const ctNationalRoaming: SimStateData = {
        type: sim.CardType.CT_NATIONAL_ROAMING_CARD
      }
      expect(41).assertEqual(ctNationalRoaming.type)
      done()
      console.log("************* Telephony_observer_SimStateData_type_0200 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_type_0300
     * @tc.name      : type
     * @tc.desc      : check the type value of SimStateData object
     */
    it('Telephony_observer_SimStateData_type_0300', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_type_0300 Test start *************")
      const cuDualMode: SimStateData = {
        type: sim.CardType.CU_DUAL_MODE_CARD
      }
      expect(42).assertEqual(cuDualMode.type)

      const dualModeTelecomLte: SimStateData = {
        type: sim.CardType.DUAL_MODE_TELECOM_LTE_CARD
      }
      expect(43).assertEqual(dualModeTelecomLte.type)

      const dualModeUg: SimStateData = {
        type: sim.CardType.DUAL_MODE_UG_CARD
      }
      expect(50).assertEqual(dualModeUg.type)

      const singleModeIsim: SimStateData = {
        type: sim.CardType.SINGLE_MODE_ISIM_CARD
      }
      expect(60).assertEqual(singleModeIsim.type)
      done()
      console.log("************* Telephony_observer_SimStateData_type_0300 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_state_0100
     * @tc.name      : state
     * @tc.desc      : check the state value of SimStateData object
     */
    it('Telephony_observer_SimStateData_state_0100', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_state_0100 Test start *************")
      const unknown: SimStateData = {
        state: sim.SimState.SIM_STATE_UNKNOWN
      }
      expect(0).assertEqual(unknown.state)
      const notPresent: SimStateData = {
        state: sim.SimState.SIM_STATE_NOT_PRESENT
      }
      expect(1).assertEqual(notPresent.state)
      done()
      console.log("************* Telephony_observer_SimStateData_state_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_state_0200
     * @tc.name      : state
     * @tc.desc      : check the state value of SimStateData object
     */
    it('Telephony_observer_SimStateData_state_0200', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_state_0200 Test start *************")
      const locked: SimStateData = {
        state: sim.SimState.SIM_STATE_LOCKED
      }
      expect(2).assertEqual(locked.state)
      const notReady: SimStateData = {
        state: sim.SimState.SIM_STATE_NOT_READY
      }
      expect(3).assertEqual(notReady.state)
      done()
      console.log("************* Telephony_observer_SimStateData_state_0200 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_SimStateData_state_0300
     * @tc.name      : state
     * @tc.desc      : check the state value of SimStateData object
     */
    it('Telephony_observer_SimStateData_state_0300', 0, function (done) {
      console.log("************* Telephony_observer_SimStateData_state_0300 Test start *************")
      const ready: SimStateData = {
        state: sim.SimState.SIM_STATE_READY
      }
      expect(4).assertEqual(ready.state)
      const loaded: SimStateData = {
        state: sim.SimState.SIM_STATE_LOADED
      }
      expect(5).assertEqual(loaded.state)
      done()
      console.log("************* Telephony_observer_SimStateData_state_0300 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_NONE_0100
     * @tc.name      : SIM_NONE
     * @tc.desc      : check the SIM_NONE property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_NONE_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_NONE_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(0).assertEqual(LockReason.SIM_NONE)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_NONE_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PN_PUK_0100
     * @tc.name      : SIM_PN_PUK
     * @tc.desc      : check the SIM_PN_PUK property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PN_PUK_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PN_PUK_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(4).assertEqual(LockReason.SIM_PN_PUK)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PN_PUK_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PU_PIN_0100
     * @tc.name      : SIM_PU_PIN
     * @tc.desc      : check the SIM_PU_PIN property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PU_PIN_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PU_PIN_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(5).assertEqual(LockReason.SIM_PU_PIN)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PU_PIN_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PU_PUK_0100
     * @tc.name      : SIM_PU_PUK
     * @tc.desc      : check the SIM_PU_PUK property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PU_PUK_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PU_PUK_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(6).assertEqual(LockReason.SIM_PU_PUK)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PU_PUK_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PP_PIN_0100
     * @tc.name      : SIM_PP_PIN
     * @tc.desc      : check the SIM_PP_PIN property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PP_PIN_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PP_PIN_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(7).assertEqual(LockReason.SIM_PP_PIN)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PP_PIN_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PP_PUK_0100
     * @tc.name      : SIM_PP_PUK
     * @tc.desc      : check the SIM_PP_PUK property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PP_PUK_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PP_PUK_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(8).assertEqual(LockReason.SIM_PP_PUK)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PP_PUK_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PC_PIN_0100
     * @tc.name      : SIM_PC_PIN
     * @tc.desc      : check the SIM_PC_PIN property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PC_PIN_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PC_PIN_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(9).assertEqual(LockReason.SIM_PC_PIN)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PC_PIN_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_PC_PUK_0100
     * @tc.name      : SIM_PC_PUK
     * @tc.desc      : check the SIM_PC_PUK property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_PC_PUK_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_PC_PUK_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(10).assertEqual(LockReason.SIM_PC_PUK)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_PC_PUK_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_SIM_PIN_0100
     * @tc.name      : SIM_SIM_PIN
     * @tc.desc      : check the SIM_SIM_PIN property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_SIM_PIN_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_SIM_PIN_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(11).assertEqual(LockReason.SIM_SIM_PIN)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_SIM_PIN_0100 Test end *************")
    })

    /*
     * @tc.number    : Telephony_observer_LockReason_SIM_SIM_PUK_0100
     * @tc.name      : SIM_SIM_PUK
     * @tc.desc      : check the SIM_SIM_PUK property of LockReason
     */
    it('Telephony_observer_LockReason_SIM_SIM_PUK_0100', 0, function (done) {
      console.log("************* Telephony_observer_LockReason_SIM_SIM_PUK_0100 Test start *************")
      if (utils.notCheck) {
        expect(true).assertTrue()
        done()
      }
      //      expect(12).assertEqual(LockReason.SIM_SIM_PUK)
      done()
      console.log("************* Telephony_observer_LockReason_SIM_SIM_PUK_0100 Test end *************")
    })

    function timeout(done) {
      expect(true).assertTrue()
      console.debug('Observer Test=========timeout========');
      done()
    }

    /*
     * @tc.number    : Telephony_Observer_SimStateData_Reason
     * @tc.name      : on_simStateChange
     * @tc.desc      : call the on method of simStateChange
     */
    it('Telephony_Observer_SimStateData_Reason', 0, async function (done) {
      console.log("************* Telephony_Observer_SimStateData_Reason Test start *************")
      observer.on('simStateChange', (data:SimStateData) => {
        if((data === null || data === undefined)
        || (data.reason == null || data.reason === undefined)){
          expect(true).assertTrue()
          done()
        }
        return
      })
      console.log("************* Telephony_Observer_SimStateData_Reason Test end *************")
      done()
    })

    console.log("************* Observer Test end *************")
  })
}



