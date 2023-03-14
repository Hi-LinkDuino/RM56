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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index.ets'
import data from '@ohos.telephony.data';


export function toString(data) {
  if (typeof data === 'object') {
    return JSON.stringify(data);
  } else {
    return data;
  }
}

export default function dataJsunit() {
  describe('dataTest', function () {
    console.log("==========> data Test start ==========>");


    /**
       * @tc.number  Telephony_Data_DataFlowType_DATA_FLOW_TYPE_NONE
       * @tc.name    Enum DataFlowType
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataFlowType_DATA_FLOW_TYPE_NONE", 0, async function (done) {
      let caseName = 'Telephony_Data_DataFlowType_DATA_FLOW_TYPE_NONE';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(0).assertEqual(data.DataFlowType.DATA_FLOW_TYPE_NONE);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DOWN
       * @tc.name    Enum DataFlowType
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DOWN", 0, async function (done) {
      let caseName = 'Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DOWN';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(1).assertEqual(data.DataFlowType.DATA_FLOW_TYPE_DOWN);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP
       * @tc.name    Enum DataFlowType
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP", 0, async function (done) {
      let caseName = 'Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(2).assertEqual(data.DataFlowType.DATA_FLOW_TYPE_UP);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP
       * @tc.name    Enum DataFlowType
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP_DOWN", 0, async function (done) {
      let caseName = 'Telephony_Data_DataFlowType_DATA_FLOW_TYPE_UP_DOWN';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(3).assertEqual(data.DataFlowType.DATA_FLOW_TYPE_UP_DOWN);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DORMANT
       * @tc.name    Enum DataFlowType
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DORMANT", 0, async function (done) {
      let caseName = 'Telephony_Data_DataFlowType_DATA_FLOW_TYPE_DORMANT';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(4).assertEqual(data.DataFlowType.DATA_FLOW_TYPE_DORMANT);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataConnectState_DATA_STATE_UNKNOWN
       * @tc.name    Enum DataConnectState
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataConnectState_DATA_STATE_UNKNOWN", 0, async function (done) {
      let caseName = 'Telephony_Data_DataConnectState_DATA_DATA_STATE_UNKNOWN';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(-1).assertEqual(data.DataConnectState.DATA_STATE_UNKNOWN);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataConnectState_DATA_STATE_DISCONNECTED
       * @tc.name    Enum DataConnectState
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataConnectState_DATA_STATE_DISCONNECTED", 0, async function (done) {
      let caseName = 'Telephony_Data_DataConnectState_DATA_DATA_STATE_DISCONNECTED';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(0).assertEqual(data.DataConnectState.DATA_STATE_DISCONNECTED);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataConnectState_DATA_STATE_CONNECTING
       * @tc.name    Enum DataConnectState
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataConnectState_DATA_STATE_CONNECTING", 0, async function (done) {
      let caseName = 'Telephony_Data_DataConnectState_DATA_STATE_CONNECTING';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(1).assertEqual(data.DataConnectState.DATA_STATE_CONNECTING);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataConnectState_DATA_STATE_CONNECTED
       * @tc.name    Enum DataConnectState
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataConnectState_DATA_STATE_CONNECTED", 0, async function (done) {
      let caseName = 'Telephony_Data_DataConnectState_DATA_STATE_CONNECTED';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(2).assertEqual(data.DataConnectState.DATA_STATE_CONNECTED);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  Telephony_Data_DataConnectState_DATA_STATE_SUSPENDED
       * @tc.name    Enum DataConnectState
       * @tc.desc    Enum value test
       */
    it("Telephony_Data_DataConnectState_DATA_STATE_SUSPENDED", 0, async function (done) {
      let caseName = 'Telephony_Data_DataConnectState_DATA_STATE_SUSPENDED';
      console.log(`==========> ${caseName} Test start ==========>`);
      expect(3).assertEqual(data.DataConnectState.DATA_STATE_SUSPENDED);
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });


  });
}


