// @ts-nocheck
/**
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';
import Utils from './Utils';

export default function radioJsunit() {
  describe('radioTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/radio',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get radio state success " + JSON.stringify(pages));
        if (!("radio" == pages.name)) {
          console.info("get radio state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(3000);
          console.info("push radio page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push radio page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("radio after each called");
    });

    it('testRadio_100', 0, async function (done) {
      console.info('[testRadio_100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('radio1');
      let obj = JSON.parse(strJson);
      console.info("testRadio_100_component obj is: " + JSON.stringify(obj));
      console.info("testRadio_100_obj.$attrs.value: " + obj.$attrs.value);
      expect(obj.$type).assertEqual('Radio');
      expect(obj.$attrs.checked).assertEqual('true');
      console.info('testRadio_100 END');
      done();
    });

    it('testRadio_200', 0, async function (done) {
      console.info('[testRadio_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('radio2');
      let obj = JSON.parse(strJson);
      console.info("testRadio_200_component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Radio');
      expect(obj.$attrs.checked).assertEqual('false');
      console.info('testRadio_200 END');
      done();
    });

    it('testRadio_300', 0, async function (done) {
      console.info('[testRadio_300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('radio3');
      let obj = JSON.parse(strJson);
      console.info("testRadio_300_component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Radio');
      expect(obj.$attrs.checked).assertEqual('false');
      console.info('testRadio_300 END');
      done();
    });
  })
}