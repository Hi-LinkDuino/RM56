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

export default function columnSplitJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('columnSplitTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/ColumnSplit',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get ColumnSplit state success " + JSON.stringify(pages));
        if (!("ColumnSplit" == pages.name)) {
          console.info("get ColumnSplit state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push ColumnSplit page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ColumnSplit page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("ColumnSplit after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0700
     * @tc.name      testColumnSplit010
     * @tc.desc      aceEtsTest
     */
    it('testColumnSplit010', 0, async function (done) {
      console.info('new columnSplit0 START');
      let strJson = getInspectorByKey('columnSplit')
      let obj = JSON.parse(strJson)
      console.info("component obj is: " + JSON.stringify(obj))
      expect(obj.$type).assertEqual('ColumnSplit')
      console.info("yr ColumnSplit0 type: " + JSON.stringify(obj.$type))
      console.log("yr columnSplit0 obj.$resizable-'true':" + JSON.stringify(obj.$attrs.resizable))
      expect(obj.$attrs.resizeable).assertEqual('true')
      console.info('new columnSplit0 END')
      done()
    })

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0710
     * @tc.name      testColumnSplit020
     * @tc.desc      aceEtsTest
     */
    it('testColumnSplit020', 0, async function (done) {
      console.info('new columnSplit1 START');
      let strJson = getInspectorByKey('columnSplit1');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("yr columnSplit obj.$resizable-'false':" + JSON.stringify(obj.$attrs.resizable));
      expect(obj.$attrs.resizeable).assertEqual('false')
      console.info('new columnSplit1 END');
      done();
    })
  })
}
