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

export default function textAreaJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("textArea beforeEach start");
      let options = {
        uri: 'pages/textArea',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get textArea state pages:" + JSON.stringify(pages));
        if (!("textArea" == pages.name)) {
          console.info("get textArea state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push textArea page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push textArea page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("textArea after each called");
    });

    it('textAreaTest01', 0, async function (done) {
      console.info('[textAreaTest01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest01] obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('TextArea');
      expect(obj.$attrs.fontSize).assertEqual('30.00fp');
      console.info('textAreaTest01 END');
      done();
    });

    it('textAreaTest02', 0, async function (done) {
      console.info('[textAreaTest02] START');
      try {
        var eventData = {
          data: {
            "fontSize": '50',
          }
        }
        var innerEvent = {
          eventId: 215,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textAreaTest02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textAreaTest02] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontSize).assertEqual('50.00fp');
      console.info('textAreaTest02 END');
      done();
    });

    it('textAreaTest03', 0, async function (done) {
      console.info('[textAreaTest03] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontColor).assertEqual('#FF000000');
      console.info('textAreaTest03 END');
      done();
    });

    it('textAreaTest04', 0, async function (done) {
      console.info('[textAreaTest04] START');
      try {
        var eventData = {
          data: {
            "fontColor": '#FFFF0000',
          }
        }
        var innerEvent = {
          eventId: 216,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textAreaTest04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textAreaTest04] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontColor).assertEqual('#FFFF0000');
      console.info('textAreaTest04 END');
      done();
    });

    it('textAreaTest05', 0, async function (done) {
      console.info('[testArea05] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest05] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Center');
      console.info('textAreaTest05 END');
      done();
    });

    it('textAreaTest06', 0, async function (done) {
      console.info('[textAreaTest06] START');
      try {
        var eventData = {
          data: {
            "textAlign": 'TextAlign.Start',
          }
        }
        var innerEvent = {
          eventId: 217,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textAreaTest06] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textAreaTest06] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest06] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      console.info('textAreaTest06 END');
      done();
    });

    it('textAreaTest07', 0, async function (done) {
      console.info('[textAreaTest07] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest07] obj is: " + JSON.stringify(obj.$attrs.constructor));
      expect(obj.$attrs.constructor.text).assertEqual(undefined);
      console.info('textAreaTest07 END');
      done();
    });

    it('textAreaTest08', 0, async function (done) {
      console.info('[textAreaTest08] START');
      try {
        var eventData = {
          data: {
            "text": 'TextArea_1',
          }
        }
        var innerEvent = {
          eventId: 218,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textAreaTest08] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textAreaTest08] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest08] obj is: " + JSON.stringify(obj.$attrs.constructor));
      expect(obj.$attrs.constructor.text).assertEqual(undefined);
      console.info('textAreaTest08 END');
      done();
    });

    it('textAreaTest09', 0, async function (done) {
      console.info('[textAreaTest09] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest09] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      console.info('textAreaTest09 END');
      done();
    });

    it('textAreaTest10', 0, async function (done) {
      console.info('[textAreaTest10] START');
      try {
        var eventData = {
          data: {
            "textAlign": TextAlign.Start,
          }
        }
        var innerEvent = {
          eventId: 218,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textAreaTest10] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textAreaTest10] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textArea');
      let obj = JSON.parse(strJson);
      console.info("[textAreaTest10] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      console.info('textAreaTest10 END');
      done();
    });
  })
}
