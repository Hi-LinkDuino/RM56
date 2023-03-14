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
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils';

export default function textInputJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("textInput beforeEach start");
      let options = {
        uri: 'pages/textInput',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get textInput state pages:" + JSON.stringify(pages));
        if (!("textInput" == pages.name)) {
          console.info("get textInput state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push textInput page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push textInput page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("textInput after each called");
    });

    it('textInputTest01', 0, async function (done) {
      console.info('[textInputTest01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest01] obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('TextArea');
      expect(obj.$attrs.fontSize).assertEqual('16.00fp');
      console.info('textInputTest01 END');
      done();
    });

    it('textInputTest02', 0, async function (done) {
      console.info('[textInputTest02] START');
      try {
        var eventData = {
          data: {
            "fontSize": '16',
          }
        }
        var innerEvent = {
          eventId: 225,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textInputTest02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textInputTest02] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontSize).assertEqual('16.00fp');
      done();
    });

    it('textInputTest03', 0, async function (done) {
      console.info('[textInputTest03] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('TextInput');
      console.info("[textInputTest03] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[textInputTest03] fontColor:" + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#E6000000');
      done();
    });

    it('textInputTest04', 0, async function (done) {
      console.info('[textInputTest04] START');
      try {
        var eventData = {
          data: {
            "fontColor": Color.Red
          }
        }
        var innerEvent = {
          eventId: 225,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textInputTest04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textInputTest04] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontColor).assertEqual('#E6000000');
      done();
    });

    it('textInputTest05', 0, async function (done) {
      console.info('[textInputTest05] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest05] obj is: " + JSON.stringify(obj));
      console.info("[textInputTest05] obj is: " + JSON.stringify(obj.$attrs.inputType));
      expect(obj.$attrs.inputType).assertEqual(undefined);
      done();
    });

    it('textInputTest06', 0, async function (done) {
      console.info('[textInputTest06] START');
      try {
        var eventData = {
          data: {
            "type": 'InputType.Email',
          }
        }
        var innerEvent = {
          eventId: 225,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textInputTest06] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textInputTest06] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest06] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.inputType).assertEqual(undefined);
      done();
    });

    it('textInputTest07', 0, async function (done) {
      console.info('[textInputTest07] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('TextInput');
      console.info("[textInputTest07] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[textInputTest07] textAlign:" + obj.$attrs.textAlign);
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      done();
    });

    it('textInputTest08', 0, async function (done) {
      console.info('[textInputTest08] START');
      try {
        var eventData = {
          data: {
            "textAlign": 'TextAlign.Start'
          }
        }
        var innerEvent = {
          eventId: 225,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textInputTest08] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textInputTest08] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('TextInput');
      let obj = JSON.parse(strJson);
      console.info("[textInputTest08] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      done();
    });
  })
}
