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

export default function dividerJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('dividerTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Divider',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get divider state success " + JSON.stringify(pages));
        if (!("Divider" == pages.name)) {
          console.info("get divider state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push divider page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push divider page error " + JSON.stringify(result));
      }
      await sleep(1)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Divider after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0750
     * @tc.name      testDividerGetInspectorByKey010
     * @tc.desc      aceEtsTest
     */
    it('testDividerGetInspectorByKey010', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      let strJson = getInspectorByKey('divider');
      let obj = JSON.parse(strJson);
      console.info("cd divider component obj is: " + JSON.stringify(obj));
      console.log("cd divider type" + JSON.stringify(obj.$type));
      console.log("cd divider color" + JSON.stringify(obj.$attrs.color));
      expect(obj.$type).assertEqual('Divider')
      expect(obj.$attrs.color).assertEqual('#FFFF0000')
      console.info(" divider component obj is: " + JSON.stringify(obj));
      console.log("cd divider strokeWidth" + JSON.stringify(obj.$attrs.strokeWidth));
      console.log("cd divider vertical" + JSON.stringify(obj.$attrs.vertical));
      console.log("cd divider lineCap" + JSON.stringify(obj.$attrs.lineCap));
      expect(obj.$attrs.strokeWidth).assertEqual("7.000000")
      expect(obj.$attrs.vertical).assertEqual('true')
      expect(obj.$attrs.lineCap == LineCapStyle.Square)
      console.info('cd dividerInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0760
     * @tc.name      testDividerGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('testDividerGetInspectorByKey020', 0, async function (done) {
      let strJson = getInspectorByKey('divider1');
      let obj = JSON.parse(strJson);
      console.info(" divider1 component obj is: " + JSON.stringify(obj));
      console.log("cd divider1 vertical" + JSON.stringify(obj.$attrs.vertical));
      console.log("cd divider1 lineCap" + JSON.stringify(obj.$attrs.lineCap));
      expect(obj.$attrs.vertical).assertEqual('false')
      expect(obj.$attrs.lineCap == LineCapStyle.Round)
      console.info('cd divider1InspectorByKey END');
      done();
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0770
     * @tc.name      testDividerGetInspectorByKey030
     * @tc.desc      aceEtsTest
     */
    it('testDividerGetInspectorByKey030', 0, async function (done) {
      let strJson = getInspectorByKey('divider2');
      let obj = JSON.parse(strJson);
      console.info(" divider2 component obj is: " + JSON.stringify(obj));
      console.log("cd divider2 lineCap" + JSON.stringify(obj.$attrs.lineCap));
      expect(obj.$attrs.lineCap == LineCapStyle.Square)
      console.info('cd divider2InspectorByKey END');
      done();
    });
  })
}
