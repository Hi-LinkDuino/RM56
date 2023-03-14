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

export default function checkboxJsunit() {
  describe('checkboxTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/checkbox',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get checkbox state success " + JSON.stringify(pages));
        if (!("checkbox" == pages.name)) {
          console.info("get checkbox state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push checkbox page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push checkbox page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("checkbox after each called");
    });

    it('testCheckbox_100', 0, async function (done) {
      console.info('[testCheckbox_100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Checkbox1');
      let obj = JSON.parse(strJson);
      console.info("testCheckbox_100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.grayscale).assertEqual(0);
      console.info('testCheckbox_100 END');
      done();
    });

    it('testCheckbox_200', 0, async function (done) {
      console.info('[testCheckbox_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Checkbox1');
      let obj = JSON.parse(strJson);
      console.info("testCheckbox_200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.opacity).assertEqual(1);
      console.info('testCheckbox_200 END');
      done();
    });

    it('testCheckbox_300', 0, async function (done) {
      console.info('[testCheckbox_300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Checkbox1');
      let obj = JSON.parse(strJson);
      console.info("testCheckbox_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderRadius).assertEqual("0.0vp");
      console.info('testCheckbox_300 END');
      done();
    });

    it('testCheckbox_400', 0, async function (done) {
      console.info('[testCheckbox_400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Checkbox1');
      let obj = JSON.parse(strJson);
      console.info("testCheckbox_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.visibility).assertEqual("Visibility.Visible");
      console.info('testCheckbox_400 END');
      done();
    });
  })
}