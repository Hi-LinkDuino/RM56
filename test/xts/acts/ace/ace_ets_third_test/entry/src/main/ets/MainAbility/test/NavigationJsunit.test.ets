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

export default function navigationJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('navigationTest', function () {

    /**
    * run before testcase
    */
    beforeEach(async function (done) {
      console.info('[navigationTest] before each called')
      let options = {
        uri: 'pages/Navigation',
      }
      try {
        let result = router.push(options)
        console.info("push navigation page success " + JSON.stringify(result));
      } catch (err) {
        console.info("push navigation page error " + JSON.stringify(result));
      }
      await sleep(1)
      done()
    });

    /**
    * run after testcase
    */
    afterEach(async function () {
      console.info('[navigationTest] after each called')
      router.clear();
      await sleep(1)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1310
     * @tc.name      testNavigation010
     * @tc.desc      aceEtsTest
     */
    it('testNavigation010', 0, async function (done) {
      console.info('testNavigation010 START');
      let strJson = getInspectorByKey('navigation1');
      let obj = JSON.parse(strJson);
      console.info("[navigation1] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Navigation')
      expect(obj.$attrs.title).assertEqual('title')
      expect(obj.$attrs.hideBackButton).assertFalse()
      expect(obj.$attrs.hideTitleBar).assertFalse()
      console.info('testNavigation010 End');
      done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1320
     * @tc.name      testNavigation020
     * @tc.desc      aceEtsTest
     */
    it('testNavigation020', 0, async function (done) {
      console.info('testNavigation020 START');
      let strJson = getInspectorByKey('navigation2');
      let obj = JSON.parse(strJson);
      console.info("[navigation2] get inspector value is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Navigation')
      expect(obj.$attrs.hideTitleBar).assertTrue()
      expect(obj.$attrs.hideBackButton).assertTrue()
      console.info('testNavigation020 End');
      done();
    });
  })
}
