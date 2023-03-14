/*
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

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';

export default function datapanelJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('dataPanelTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/DataPanel',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get DataPanel state success " + JSON.stringify(pages));
        if (!("DataPanel" == pages.name)) {
          console.info("get DataPanel state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push DataPanel page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push DataPanel page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("DataPanel after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0730
     * @tc.name      testDataPanelGetInspectorByKey010
     * @tc.desc      aceEtsTest
     */
    it('testDataPanelGetInspectorByKey010', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      let strJson = getInspectorByKey('DataPanel');
      let obj = JSON.parse(strJson);
      console.info("cd DatePanel component obj is: " + JSON.stringify(obj));
      console.log("cd DataPanel type " + JSON.stringify(obj.$type));
      console.log("cd DataPanel values1 " + typeof ([20, 40, 20]));
      console.log("cd DataPanel values " + typeof (obj.$attrs.values));
      console.log("cd DataPanel values" + JSON.stringify(obj.$attrs.values));
      console.log("cd DataPanel dataPanelType" + JSON.stringify(obj.$attrs.DataPanelType));
      console.log("cd  DataPanel max" + JSON.stringify(obj.$attrs.max));
      console.log("panel closeEffect" + JSON.stringify(obj.$attrs.closeEffect))
      expect(obj.$attrs.closeEffect).assertEqual("false")
      expect(obj.$type).assertEqual("DataPanel")
      expect(JSON.stringify(obj.$attrs.values)).assertEqual('[20,40,20]')
      expect(obj.$attrs.max).assertEqual(100.000000.toFixed(6))
      console.info('cd dataPanelInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0740
     * @tc.name      testDataPanelGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('testDataPanelGetInspectorByKey020', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      let strJson = getInspectorByKey('DataPanel2');
      let obj = JSON.parse(strJson);
      console.info("cd DatePanel component obj is: " + JSON.stringify(obj));
      console.log("cd DataPanel dataPanelType" + JSON.stringify(obj.$attrs));
      console.log("cd DataPanel Type" + JSON.stringify(obj.$attrs.type));
      expect(obj.$attrs.type).assertEqual("DataPanelType.Circle")
      console.log("panel closeEffect" + JSON.stringify(obj.$attrs.closeEffect))
      expect(obj.$attrs.closeEffect).assertEqual("true")
      console.info('cd dataPanelInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0740
     * @tc.name      testDataPanelGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('testDataPanelGetInspectorByKey030', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      let strJson = getInspectorByKey('DataPanel3');
      let obj = JSON.parse(strJson);
      console.info("cd DatePanel component obj is: " + JSON.stringify(obj));
      console.log("cd DataPanel type " + JSON.stringify(obj.$type));
      console.log("cd DataPanel values1 " + typeof ([20, 40, 20]));
      console.log("cd DataPanel values " + typeof (obj.$attrs.values));
      console.log("cd DataPanel values" + JSON.stringify(obj.$attrs.values));
      console.log("cd  DataPanel max" + JSON.stringify(obj.$attrs.max));
      console.log("panel closeEffect" + JSON.stringify(obj.$attrs.closeEffect))
      expect(obj.$attrs.closeEffect).assertEqual("false")
      expect(obj.$type).assertEqual("DataPanel")
      expect(JSON.stringify(obj.$attrs.values)).assertEqual('[20,40,20]')
      expect(obj.$attrs.max).assertEqual(100.000000.toFixed(6))
      console.info('cd dataPanelInspectorByKey END');
      done();
    });
  })
}
