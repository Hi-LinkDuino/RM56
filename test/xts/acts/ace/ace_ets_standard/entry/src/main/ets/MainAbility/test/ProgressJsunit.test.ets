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

export default function progressJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('progressTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Progress',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Progress state success " + JSON.stringify(pages));
        if (!("Progress" == pages.name)) {
          console.info("get Progress state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Progress page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Progress page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Progress after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0960
     * @tc.name      testGetProgressAttr010
     * @tc.desc      aceEtsTest
     */
    it('testGetProgressAttr010', 0, async function (done) {
      console.info('testGetProgressAttr START');
      let strJson = getInspectorByKey('progress');
      let obj = JSON.parse(strJson);
      let objConstructor = JSON.parse(obj.$attrs.constructor)
      console.log("progress constructor is " + objConstructor)
      expect(obj.$attrs.value).assertEqual(3.000000.toFixed(6))
      expect(obj.$type).assertEqual('Progress')
      expect(obj.$attrs.color).assertEqual("#FF0000FF")
      expect(obj.$attrs.width).assertEqual("50.00vp")
      expect(obj.$attrs.height).assertEqual("70.00vp")
      expect(objConstructor.total).assertEqual(40.000000.toFixed(6))
      expect(objConstructor.type).assertEqual("ProgressStyle.Linear")
      console.info('testGetProgressAttr END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0970
     * @tc.name      testGetProgressAttr020
     * @tc.desc      aceEtsTest
     */
    it('testGetProgressAttr020', 0, async function (done) {
      console.info('testGetProgressAttr2 START');
      let strJson = getInspectorByKey('progress2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      let objConstructor = JSON.parse(obj.$attrs.constructor)
      console.log("progress constructor is " + objConstructor)
      expect(objConstructor.type).assertEqual("ProgressStyle.Eclipse")
      console.info('testGetProgressAttr2 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0970
     * @tc.name      testGetProgressAttr030
     * @tc.desc      aceEtsTest
     */
    it('testGetProgressAttr030', 0, async function (done) {
      console.info('testGetProgressAttr3 START');
      let strJson = getInspectorByKey('progress3');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      let objConstructor = JSON.parse(obj.$attrs.constructor)
      console.log("progress constructor is " + objConstructor)
      expect(objConstructor.type).assertEqual("ProgressStyle.Ring")
      console.info('testGetProgressAttr3 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0970
     * @tc.name      testGetProgressAttr040
     * @tc.desc      aceEtsTest
     */
    it('testGetProgressAttr040', 0, async function (done) {
      console.info('testGetProgressAttr4 START');
      let strJson = getInspectorByKey('progress4');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      let objConstructor = JSON.parse(obj.$attrs.constructor)
      console.log("progress constructor is " + objConstructor)
      expect(objConstructor.type).assertEqual("ProgressStyle.ScaleRing")
      console.info('testGetProgressAttr4 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0970
     * @tc.name      testGetProgressAttr050
     * @tc.desc      aceEtsTest
     */
    it('testGetProgressAttr020', 0, async function (done) {
      console.info('testGetProgressAttr5 START');
      let strJson = getInspectorByKey('progress5');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      let objConstructor = JSON.parse(obj.$attrs.constructor)
      console.log("progress constructor is " + objConstructor)
      expect(objConstructor.type).assertEqual("ProgressStyle.Capsule")
      console.info('testGetProgressAttr5 END');
      done();
    });
  })
}
