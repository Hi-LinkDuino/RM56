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

export default function gridContainerJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('gridContainerTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/GridContainer',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get GridContainer state success " + JSON.stringify(pages));
        if (!("GridContainer" == pages.name)) {
          console.info("get GridContainer state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push GridContainer page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push GridContainer page error " + JSON.stringify(result));
      }
      await sleep(1)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("GridContainer after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0830
     * @tc.name      testGridContainer010
     * @tc.desc      aceEtsTest
     */
    it('testGridContainer010', 0, async function (done) {
      console.info('new GridContainer0 START');
      let strJson = getInspectorByKey('gridContainer');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('GridContainer');
      console.info("yr GridContainer0 type: " + JSON.stringify(obj.$type));
      console.log("yr gridContainer0 obj.$attrs.alignItems-'HorizontalAlign.Start':" + JSON.stringify(obj.$attrs.alignItems));
      console.info('new GridContainer0 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0840
     * @tc.name      testGridContainer020
     * @tc.desc      aceEtsTest
     */
    it('testGridContainer020', 0, async function (done) {
      console.info('new GridContainer1 START');
      let strJson = getInspectorByKey('gridContainer1');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignItems).assertEqual('HorizontalAlign.Center');
      console.log("yr gridContainer1 obj.$attrs.alignItems-'HorizontalAlign.Center':" + JSON.stringify(obj.$attrs.alignItems));
      console.info('new GridContainer1 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0850
     * @tc.name      testGridContainer030
     * @tc.desc      aceEtsTest
     */
    it('testGridContainer030', 0, async function (done) {
      console.info('new GridContainer2 START');
      let strJson = getInspectorByKey('gridContainer2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignItems).assertEqual('HorizontalAlign.End');
      console.log("yr gridContainer obj.$attrs.alignItems-'HorizontalAlign.End':" + JSON.stringify(obj.$attrs.alignItems));
      console.info('new GridContainer2 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0860
     * @tc.name      testGridContainer040
     * @tc.desc      aceEtsTest
     */
    it('testGridContainer040', 0, async function (done) {
      console.info('new GridContainer3 START');
      let strJson = getInspectorByKey('gridContainer3');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignItems).assertEqual('HorizontalAlign.End');
      console.log("yr gridContainer3 obj.$attrs.alignItems-'HorizontalAlign.End':" + JSON.stringify(obj.$attrs.alignItems));
      console.info('new GridContainer3  END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0870
     * @tc.name      testGridContainer050
     * @tc.desc      aceEtsTest
     */
    it('testGridContainer050', 0, async function (done) {
      console.info('new GridContainer4 START');
      let strJson = getInspectorByKey('gridContainer4');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("gridContainer obj.$attrs.sizeType:" + JSON.stringify(obj.$attrs.sizeType)); //undefinde
      console.info('new GridContainer4  END');
      done();
    });
  })
}
