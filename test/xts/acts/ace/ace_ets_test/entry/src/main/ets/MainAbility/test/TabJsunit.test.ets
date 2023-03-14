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

export default function tabJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('tabTest', function () {

    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Tab',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get Tab state success " + JSON.stringify(pages));
        if (!("Tab" == pages.name)) {
          console.info("get Tab state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Tab page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Tab page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Tab after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0510
     * @tc.name      tabTest001
     * @tc.desc      aceEtsTest
     */
    it('tabTest001', 0, async function (done) {
      console.info('++++++++++++++++ Tab TestCase 1 START ++++++++++++++++');
      let strJson = getInspectorByKey('tab');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      console.log('obj.$attrs.scrollable' + typeof (obj.$attrs.scrollable))
      console.log('obj.$attrs.vertical' + typeof (obj.$attrs.vertical))
      expect(obj.$type).assertEqual('Tabs')
      expect(obj.$attrs.barPosition).assertEqual('BarPosition.Start')
      expect(obj.$attrs.index).assertEqual('1')
      expect(obj.$attrs.vertical).assertEqual('true')
      expect(obj.$attrs.scrollable).assertTrue()
      expect(obj.$attrs.barMode).assertEqual('BarMode.Fixed')
      expect(obj.$attrs.barWidth).assertEqual(70.000000.toFixed(6))
      expect(obj.$attrs.barHeight).assertEqual(150.000000.toFixed(6))
      console.info('++++++++++++++++ Tab TestCase 1 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0530
     * @tc.name      tabTest003
     * @tc.desc      aceEtsTest
     */
    it('tabTest003', 0, async function (done) {
      console.info('++++++++++++++++ Tab TestCase 3 START ++++++++++++++++');
      let strJson = getInspectorByKey('tab2');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Tabs')
      expect(obj.$attrs.barPosition).assertEqual('BarPosition.End')
      expect(obj.$attrs.barMode).assertEqual('BarMode.Scrollable')
      expect(obj.$attrs.vertical).assertEqual('false')
      expect(obj.$attrs.scrollable).assertFalse()
      console.info('++++++++++++++++ Tab TestCase 3 End ++++++++++++++++');
      done();
    });
  })
}
