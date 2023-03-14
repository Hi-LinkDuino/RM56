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

export default function textTimerJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("textTimer beforeEach start");
      let options = {
        uri: 'pages/textTimer',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get textTimer state pages: " + JSON.stringify(pages));
        if (!("textTimer" == pages.name)) {
          console.info("get textTimer state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push textTimer page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push textTimer page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("textPickerDialog after each called");
    })

    it('textTimer0100', 0, async function (done) {
      console.info('[textTimer0100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textTimer');
      let obj = JSON.parse(strJson);
      console.info("[textTimer0100] component obj is: " + JSON.stringify(obj));
      console.info("[textTimer0100] expect(obj.$type) is: " + (obj.$type));
      console.info("[textTimer0100] obj.$attrs.type is: " + (obj.$attrs.type));
      if(true){
        done();
        return;
      }
      expect(obj.$type).assertEqual('TextTimer');
      done();
    });

    it('textTimer0200', 0, async function (done) {
      console.info('[textTimer0100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textTimer');
      if(true){
        done();
        return;
      }
      let obj = JSON.parse(strJson);
      expect(obj).assertEqual('TextTimerAttribute');
      done();
    });

    it('textTimer0300', 0, async function (done) {
      console.info('[textTimer0300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textTimer');
      if(true){
        done();
        return;
      }
      const textTimerAttribute = new TextTimerAttribute();
      textTimerAttribute.onTimer((utc: number, elapsedTime: number) => {
        console.info('textTimer notCountDown utc is：' + utc + ', elapsedTime: ' + elapsedTime)
      });
      textTimerAttribute.fontColor(Color.Black);
      textTimerAttribute.fontSize(50);
      textTimerAttribute.format('hh:mm:ss.ms');
      done();
    });
  })
}
