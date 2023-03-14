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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import router from '@system.router';
import Utils from './Utils';

export default function checkboxGroupJsunit() {
  describe('checkboxGroupTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/checkboxGroup',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get checkboxGroup state success " + JSON.stringify(pages));
        if (!("checkboxGroup" == pages.name)) {
          console.info("get checkboxGroup state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push checkboxGroup page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push checkboxGroup page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("checkboxGroup after each called");
    });

    it('testCheckboxGroup_100', 0, async function (done) {
      console.info('[testCheckboxGroup_100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.opacity).assertEqual(1);
      console.info('testCheckboxGroup_100 END');
      done();
    });

    it('testCheckboxGroup_200', 0, async function (done) {
      console.info('[testCheckboxGroup_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Auto");
      console.info('testCheckboxGroup_200 END');
      done();
    });

    it('testCheckboxGroup_300', 0, async function (done) {
      console.info('[testCheckboxGroup_300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.scrollable).assertEqual(false);
      console.info('testCheckboxGroup_300 END');
      done();
    });

    it('testCheckboxGroup_400', 0, async function (done) {
      console.info('[testCheckboxGroup_400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderRadius).assertEqual("0.0vp");
      console.info('testCheckboxGroup_400 END');
      done();
    });

    it('testCheckboxGroup_500', 0, async function (done) {
      console.info('[testCheckboxGroup_500] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_500 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.flexBasis).assertEqual("auto");
      console.info('testCheckboxGroup_500 END');
      done();
    });

    it('testCheckboxGroup_600', 0, async function (done) {
      console.info('[testCheckboxGroup_600] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CheckboxGroup');
      let obj = JSON.parse(strJson);
      console.info("testCheckboxGroup_600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Solid");
      console.info('testCheckboxGroup_600 END');
      done();
    });
  })
}