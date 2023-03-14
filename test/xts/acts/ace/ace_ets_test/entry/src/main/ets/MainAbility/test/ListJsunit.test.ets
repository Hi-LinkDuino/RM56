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

export default function listJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('listTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/List',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get List state success " + JSON.stringify(pages));
        if (!("List" == pages.name)) {
          console.info("get List state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push List page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push List page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("List after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0410
     * @tc.name      listTest001
     * @tc.desc      aceEtsTest
     */
    it('listTest001', 0, async function (done) {
      console.info('++++++++++++++++ List TestCase 1 START ++++++++++++++++');
      await sleep(1)
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('List')
      console.log('List‘s space is ' + JSON.stringify(obj.$attrs.space))
      console.log('List‘s initialIndex is ' + JSON.stringify(obj.$attrs.initialIndex))
      console.log('List‘s listDirection is ' + JSON.stringify(obj.$attrs.listDirection))
      console.log('List‘s divider is ' + JSON.stringify(obj.$attrs.divider))
      console.log('List‘s edgeEffect is ' + JSON.stringify(obj.$attrs.edgeEffect))
      expect(obj.$attrs.initialIndex).assertEqual(0.000000.toFixed(6))
      expect(obj.$attrs.listDirection).assertEqual('Axis.Vertical')
      expect(obj.$attrs.edgeEffect).assertEqual('EdgeEffect.None')
      expect(obj.$attrs.editMode).assertEqual("false")
      console.info('++++++++++++++++ List TestCase 1 End ++++++++++++++++');
      done();
    });
  })
}
