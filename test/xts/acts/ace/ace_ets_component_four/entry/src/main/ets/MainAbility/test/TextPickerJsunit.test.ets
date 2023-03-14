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

export default function textPickerJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("button beforeEach start");
      let options = {
        uri: 'pages/textPickerDialog',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get textPickerDialog state pages: " + JSON.stringify(pages));
        if (!("textPickerDialog" == pages.name)) {
          console.info("get textPickerDialog state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push textPickerDialog page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push textPickerDialog page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("textPickerDialog after each called");
    })

    it('testTextPickerDialog0100', 0, async function (done) {
      console.info('[testTextPickerDialog0100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textPickerDialog');
      let obj = JSON.parse(strJson);
      console.info("[testTextPickerDialog0100] component obj is: " + JSON.stringify(obj));
      console.info("[testTextPickerDialog0100] expect(obj.$type) is: " + (obj.$type));
      console.info("[testTextPickerDialog0100] obj.$attrs.type is: " + (obj.$attrs.type));
      done();
    });

    it('testTextPickerDialog0200', 0, async function (done) {
      console.info('[testTextPickerDialog0200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textPickerDialog');
      let obj = JSON.parse(strJson);
      if(true){
        done();
        return;
      }
      const textPickerDialogOption = new textPickerDialogOption();
      textPickerDialogOption.defaultPickerItemHeight = 20;
      const textPickerAttribute = new TextPickerAttribute();
      textPickerAttribute.defaultPickerItemHeight = 20;
      const textPickerOption = new TextPickerOption();
      textPickerOption.value = "textPickerOptionTest";
      done();
    });
  })
}
