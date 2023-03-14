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

export default function gridItemJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('gridItemTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/GridItem',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get GridItem state success " + JSON.stringify(pages));
        if (!("GridItem" == pages.name)) {
          console.info("get GridItem state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push GridItem page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push GridItem page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("GridItem after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0340
     * @tc.name      gridItemTest001
     * @tc.desc      aceEtsTest
     */
    it('gridItemTest001', 0, async function (done) {
      console.info('gridItemTest001 START');
      await sleep(1)
      let strJson = getInspectorByKey('GridItem1');
      let obj = JSON.parse(strJson);
      console.info("属性值集合: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('GridItem')
      console.log('GridItem1‘s rowStart is ' + JSON.stringify(obj.$attrs.rowStart))
      console.log('GridItem1‘s rowEnd is ' + JSON.stringify(obj.$attrs.rowEnd))
      console.log('GridItem1‘s columnStart is ' + JSON.stringify(obj.$attrs.columnStart))
      console.log('GridItem1‘s columnEnd is ' + JSON.stringify(obj.$attrs.columnEnd))
      console.log('GridItem1‘s forceRebuild is ' + JSON.stringify(obj.$attrs.forceRebuild))
      await sleep(1)
      expect(obj.$attrs.rowStart).assertEqual('1')
      expect(obj.$attrs.rowEnd).assertEqual('4')
      expect(obj.$attrs.columnStart).assertEqual('1')
      expect(obj.$attrs.columnEnd).assertEqual('2')
      expect(obj.$attrs.forceRebuild).assertEqual('false')
      console.info('gridItemTest001 END');
      done();
    });
  })
}
