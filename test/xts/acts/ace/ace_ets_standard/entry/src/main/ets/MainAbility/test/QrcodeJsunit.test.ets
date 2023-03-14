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

export default function qrcodeJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('QrcodeTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Qrcode',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Qrcode state success " + JSON.stringify(pages));
        if (!("Qrcode" == pages.name)) {
          console.info("get Qrcode state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Qrcode page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Qrcode page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Qrcode after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0980
     * @tc.name      testGetQrcodeAttr010
     * @tc.desc      aceEtsTest
     */
    it('testGetQrcodeAttr010', 0, async function (done) {
      console.info('testGetQrcodeAttr START');
      let strJson = getInspectorByKey('qrcode');
      let obj = JSON.parse(strJson);
      console.info("Qrcode component obj is: " + JSON.stringify(obj));
      console.info("qrcode backgroundColor" + JSON.stringify(obj.$attrs.backgroundColor))
      console.info("qrcode width" + JSON.stringify(obj.$attrs.width))
      console.info("qrcode height" + JSON.stringify(obj.$attrs.height))
      console.info("qrcode value" + JSON.stringify(obj.$attrs.value))
      expect(obj.$attrs.value).assertEqual("hello world")
      console.info('testGetQrcodeAttr END');
      done();
    });
  })
}
