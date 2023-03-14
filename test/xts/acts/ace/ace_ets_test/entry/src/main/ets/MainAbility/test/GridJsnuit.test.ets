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

export default function gridJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('gridTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Grid',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get Grid state success " + JSON.stringify(pages));
        if (!("Grid" == pages.name)) {
          console.info("get Grid state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Grid page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Grid page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Grid after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0350
     * @tc.name      gridItemTest001
     * @tc.desc      aceEtsTest
     */
    it('gridTest001', 0, async function (done) {
      console.info('gridTest001 START');
      await sleep(1)
      let strJson = getInspectorByKey('Grid');
      let obj = JSON.parse(strJson);
      console.info("属性值集合: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Grid')
      console.log(JSON.stringify(obj.$attrs.columnsTemplate))
      console.log(JSON.stringify(obj.$attrs.rowsTemplate))
      console.log(JSON.stringify(obj.$attrs.rowsTemplate))
      console.log(JSON.stringify(obj.$attrs.columnsGap))
      console.log(JSON.stringify(obj.$attrs.rowsGap))
      await sleep(1)
      expect(obj.$attrs.columnsTemplate).assertEqual('1fr 1fr 1fr 1fr 1fr')
      expect(obj.$attrs.rowsTemplate).assertEqual('1fr 1fr 1fr 1fr 1fr')
      console.info('gridTest001 END');
      done();
    });
  }

  )
}
