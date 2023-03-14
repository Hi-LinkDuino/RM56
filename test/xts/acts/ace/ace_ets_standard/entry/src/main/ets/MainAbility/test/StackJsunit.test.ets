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

export default function StackJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('StackTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Stack',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Stack state success " + JSON.stringify(pages));
        if (!("Stack" == pages.name)) {
          console.info("get Stack state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Stack page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Stack page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Stack after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1110
     * @tc.name      testStackGetInspectorByKey010
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey010', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      let strJson = getInspectorByKey('stack');
      let obj = JSON.parse(strJson);
      console.info("cd stack component obj is: " + JSON.stringify(obj));
      console.log("cd stack type" + JSON.stringify(obj.$type));
      console.log("cd stack alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$type).assertEqual('Stack')
      expect(obj.$attrs.alignContent).assertEqual('Alignment.Bottom')
      console.info('cd stackGetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1120
     * @tc.name      testStackGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey020', 0, async function (done) {
      let strJson = getInspectorByKey('stack1');
      let obj = JSON.parse(strJson);
      console.info("cd stack1 component obj is: " + JSON.stringify(obj));
      console.log("cd stack1 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$type).assertEqual('Stack')
      expect(obj.$attrs.alignContent).assertEqual('Alignment.BottomStart')
      console.info('cd stack1GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1130
     * @tc.name      testStackGetInspectorByKey030
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey030', 0, async function (done) {
      let strJson = getInspectorByKey('stack2');
      let obj = JSON.parse(strJson);
      console.info("cd stack2 component obj is: " + JSON.stringify(obj));
      console.log("cd stack2 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.BottomEnd')
      console.info('cd stack2GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1140
     * @tc.name      testStackGetInspectorByKey040
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey040', 0, async function (done) {
      let strJson = getInspectorByKey('stack3');
      let obj = JSON.parse(strJson);
      console.info("cd stack3 component obj is: " + JSON.stringify(obj));
      console.log("cd stack3 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.Start')
      console.info('cd stack3GetInspectorByKey END');
      done();
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1150
     * @tc.name      testStackGetInspectorByKey050
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey050', 0, async function (done) {
      let strJson = getInspectorByKey('stack4');
      let obj = JSON.parse(strJson);
      console.info("cd stack4 component obj is: " + JSON.stringify(obj));
      console.log("cd stack4 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.Center')
      console.info('cd stack4GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1160
     * @tc.name      testStackGetInspectorByKey060
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey060', 0, async function (done) {
      let strJson = getInspectorByKey('stack5');
      let obj = JSON.parse(strJson);
      console.info("cd stack5 component obj is: " + JSON.stringify(obj));
      console.log("cd stack5 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.End')
      console.info('cd stack5GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1170
     * @tc.name      testStackGetInspectorByKey070
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey070', 0, async function (done) {
      let strJson = getInspectorByKey('stack6');
      let obj = JSON.parse(strJson);
      console.info("cd stack6 component obj is: " + JSON.stringify(obj));
      console.log("cd stack6 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.Top')
      console.info('cd stack6GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1180
     * @tc.name      testStackGetInspectorByKey080
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey080', 0, async function (done) {
      let strJson = getInspectorByKey('stack7');
      let obj = JSON.parse(strJson);
      console.info("cd stack7 component obj is: " + JSON.stringify(obj));
      console.log("cd stack7 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.TopStart')
      console.info('cd stack7GetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1190
     * @tc.name      testStackGetInspectorByKey090
     * @tc.desc      aceEtsTest
     */
    it('testStackGetInspectorByKey090', 0, async function (done) {
      let strJson = getInspectorByKey('stack8');
      let obj = JSON.parse(strJson);
      console.info("cd stack8 component obj is: " + JSON.stringify(obj));
      console.log("cd stack8 alignContent" + JSON.stringify(obj.$attrs.alignContent));
      expect(obj.$attrs.alignContent).assertEqual('Alignment.TopEnd')
      console.info('cd stack8GetInspectorByKey END');
      done();
    });
  })
}
