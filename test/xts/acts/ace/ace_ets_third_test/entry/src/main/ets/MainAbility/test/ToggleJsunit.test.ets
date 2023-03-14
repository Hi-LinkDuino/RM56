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

export default function toggleJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('toggleTest', function () {
    /**
    * run before testcase
    */
    beforeEach(async function (done) {
      console.info('[toggleTest] before each called')
      let options = {
        uri: 'pages/Toggle',
      }
      try {
        let pages = router.getState()
        console.info("[toggleTest] current page is: " + JSON.stringify(pages.name));
        if (!('Toggle' == pages.name)) {
          let result = await router.push(options)
          console.info("push toggle page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.info("push toggle page page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    /**
    * run after testcase
    */
    afterEach(async function () {
      console.info('[toggleTest] after each called')
      router.clear();
      await sleep(1)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1640
     * @tc.name      testToggle010
     * @tc.desc      aceEtsTest
     */
    it('testToggle010', 0, async function (done) {
      console.info('testToggle010 START');
      let strJson = getInspectorByKey('toggle1');
      let obj = JSON.parse(strJson);
      console.info("[toggle1] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Switch')
      expect(obj.$attrs.isOn).assertEqual('false')
      expect(obj.$attrs.selectedColor).assertEqual('#FFFF0000')
      expect(obj.$attrs.switchPointColor).assertEqual('#FF808080')
      console.info('testToggle010 End');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1650
     * @tc.name      testToggle020
     * @tc.desc      aceEtsTest
     */
    it('testToggle020', 0, async function (done) {
      console.info('testToggle020 START');
      let strJson = getInspectorByKey('toggle2');
      let obj = JSON.parse(strJson);
      console.info("[toggle2] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Switch')
      expect(obj.$attrs.isOn).assertEqual('true')
      console.info('testToggle020 End');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1660
     * @tc.name      testToggle030
     * @tc.desc      aceEtsTest
     */
    it('testToggle030', 0, async function (done) {
      console.info('testToggle030 START');
      let strJson = getInspectorByKey('toggle3');
      let obj = JSON.parse(strJson);
      console.info("[toggle3] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Checkbox')
      expect(obj.$attrs.isOn).assertEqual('false')
      console.info('testToggle030 End');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1670
     * @tc.name      testToggle040
     * @tc.desc      aceEtsTest
     */
    it('testToggle040', 0, async function (done) {
      console.info('testToggle040 START');
      let strJson = getInspectorByKey('toggle4');
      let obj = JSON.parse(strJson);
      console.info("[toggle4] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Checkbox')
      expect(obj.$attrs.isOn).assertEqual('true')
      console.info('testToggle040 End');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1680
     * @tc.name      testToggle050
     * @tc.desc      aceEtsTest
     */
    it('testToggle050', 0, async function (done) {
      console.info('testToggle050 START');
      let strJson = getInspectorByKey('toggle5');
      let obj = JSON.parse(strJson);
      console.info("[toggle5] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Button')
      expect(obj.$attrs.isOn).assertEqual('false')
      console.info('testToggle050 End');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1690
     * @tc.name      testToggle060
     * @tc.desc      aceEtsTest
     */
    it('testToggle060', 0, async function (done) {
      console.info('testToggle060 START');
      let strJson = getInspectorByKey('toggle6');
      console.info('sendEventByKey result is ' + sendEventByKey('toggle6', 10, ''))
      let obj = JSON.parse(strJson);
      console.info("[toggle6] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle')
      expect(obj.$attrs.type).assertEqual('ToggleType.Button')
      expect(obj.$attrs.isOn).assertEqual('true')
      console.info('testToggle060 End');
      done();
    });
  })
}
