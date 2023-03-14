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

export default function propJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('propTest', function () {

    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Prop',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Prop state success " + JSON.stringify(pages));
        if (!("Prop" == pages.name)) {
          console.info("get Prop state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Prop page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Prop page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Prop after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0470
     * @tc.name      propTest001
     * @tc.desc      aceEtsTest
     */
    it('propTest001', 0, async function (done) {
      console.info('propTest001 START');
      await sleep(1)
      let strJson = getInspectorByKey('Text001');
      let obj = JSON.parse(strJson);
      expect(obj.$type).assertEqual('Text');
      console.log(JSON.stringify(obj.$type))
      console.log("cd attrs.conten type1" + typeof ("10"))
      console.log("yr prop0 content-'10'" + obj.$attrs.content)
      expect(obj.$attrs.content).assertEqual("10");
      console.info('propTest001 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0480
     * @tc.name      propTest001
     * @tc.desc      aceEtsTest
     */
    it('propTest002', 0, async function (done) {
      console.info('propTest002 START');
      let strJson = getInspectorByKey('Text002');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("cd attrs.conten type" + typeof (obj.$attrs.content))
      console.log("cd attrs.conten type1 " + typeof ("10"))
      expect(obj.$attrs.content).assertEqual("10");
      console.log("cd prop0 content-'10'" + JSON.stringify(obj.$attrs.content))
      console.info('propTest002 END');
      done();
    });
  })
}
