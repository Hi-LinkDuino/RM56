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
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils.ets';

export default function textStyleJsunit() {
  describe('textStyleTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/textStyle',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get textStyle state pages:" + JSON.stringify(pages));
        if (!("textStyle" == pages.name)) {
          console.info("get textStyle state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push textStyle page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push textStyle page error:" + err);
      }
      done();
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("textStyle after each called");
    });

    it('testTextStyle001', 0, async function (done) {
      console.info('[testTextStyle001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testTextStyle001] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle001] fontColor: " + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FFFF0000');
      done();
    });

    it('testTextStyle002', 0, async function (done) {
      console.info('[testTextStyle002] START');
      try {
        let eventData = {
          data: {
            "fontColor": Color.Blue
          }
        }
        let indexEvent = {
          eventId: 37,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle002] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle002] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testTextStyle002] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle002] fontColor: " + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FF0000FF');
      done();
    });

    it('testTextStyle003', 0, async function (done) {
      console.info('[testTextStyle003] START');
      try {
        let eventData = {
          data: {
            "fontColor": 0x80000000
          }
        }
        let indexEvent = {
          eventId: 37,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle003] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle003] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testTextStyle003] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle003] fontColor: " + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#80000000');
      done();
    });

    it('testTextStyle004', 0, async function (done) {
      console.info('[testTextStyle004] START');
      try {
        let eventData = {
          data: {
            "fontColor": 0x7FFFFFFF
          }
        }
        let indexEvent = {
          eventId: 37,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle004] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle004] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testTextStyle004] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle004] fontColor: " + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#7FFFFFFF');
      done();
    });

    it('testTextStyle005', 0, async function (done) {
      console.info('[testTextStyle005] START');
      try {
        let eventData = {
          data: {
            "fontColor": 'error'
          }
        }
        let indexEvent = {
          eventId: 37,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle005] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle005] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testTextStyle005] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle005] fontColor: " + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FF000000');
      done();
    });

    it('testTextStyle006', 0, async function (done) {
      console.info('[testTextStyle006] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('fontSize');
      console.info("[testTextStyle006] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle006] fontSize: " + obj.$attrs.fontSize);
      expect(obj.$attrs.fontSize).assertEqual('20.00fp');
      done();
    });

    it('testTextStyle007', 0, async function (done) {
      console.info('[testTextStyle007] START');
      try {
        let eventData = {
          data: {
            "fontSize": 30
          }
        }
        let indexEvent = {
          eventId: 38,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle007] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle007] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontSize');
      console.info("[testTextStyle007] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle007] fontSize: " + obj.$attrs.fontSize);
      expect(obj.$attrs.fontSize).assertEqual('30.00fp');
      done();
    });

    it('testTextStyle008', 0, async function (done) {
      console.info('[testTextStyle008] START');
      try {
        let eventData = {
          data: {
            "fontSize": 0x80000000
          }
        }
        let indexEvent = {
          eventId: 38,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle008] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle008] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontSize');
      console.info("[testTextStyle008] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle008] fontSize: " + obj.$attrs.fontSize);
      expect(obj.$attrs.fontSize).assertEqual('-2147483648.00fp');
      done();
    });

    it('testTextStyle009', 0, async function (done) {
      console.info('[testTextStyle009] START');
      try {
        let eventData = {
          data: {
            "fontSize": 'error'
          }
        }
        let indexEvent = {
          eventId: 38,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle009] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle009] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontSize');
      console.info("[testTextStyle009] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle009] fontSize: " + obj.$attrs.fontSize);
      expect(obj.$attrs.fontSize).assertEqual('0.00fp');
      done();
    });

    it('testTextStyle010', 0, async function (done) {
      console.info('[testTextStyle010] START');
      try {
        let eventData = {
          data: {
            "fontSize": 0x7FFFFFFF
          }
        }
        let indexEvent = {
          eventId: 38,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle010] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle010] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontSize');
      console.info("[testTextStyle010] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle010] fontSize: " + obj.$attrs.fontSize);
      expect(obj.$attrs.fontSize).assertEqual('2147483647.00fp');
      done();
    });

    it('testTextStyle011', 0, async function (done) {
      console.info('[testTextStyle011] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('fontStyle');
      console.info("[testTextStyle011] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle011] fontStyle: " + obj.$attrs.fontStyle);
      expect(obj.$attrs.fontStyle).assertEqual('FontStyle.Italic');
      done();
    });

    it('testTextStyle012', 0, async function (done) {
      console.info('[testTextStyle012] START');
      try {
        let eventData = {
          data: {
            "fontStyle": FontStyle.Normal
          }
        }
        let indexEvent = {
          eventId: 39,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle012] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle012] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontStyle');
      console.info("[testTextStyle012] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle012] fontStyle: " + obj.$attrs.fontStyle);
      expect(obj.$attrs.fontStyle).assertEqual('FontStyle.Normal');
      done();
    });

    it('testTextStyle013', 0, async function (done) {
      console.info('[testTextStyle013] START');
      try {
        let eventData = {
          data: {
            "fontStyle": 0x7FFFFFFF
          }
        }
        let indexEvent = {
          eventId: 39,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle013] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle013] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontStyle');
      console.info("[testTextStyle013] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle013] fontStyle: " + obj.$attrs.fontStyle);
      expect(obj.$attrs.fontStyle).assertEqual('FontStyle.Normal');
      done();
    });

    it('testTextStyle014', 0, async function (done) {
      console.info('[testTextStyle014] START');
      try {
        let eventData = {
          data: {
            "fontStyle": 'error'
          }
        }
        let indexEvent = {
          eventId: 39,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle014] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle014] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontStyle');
      console.info("[testTextStyle014] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle014] fontStyle: " + obj.$attrs.fontStyle);
      expect(obj.$attrs.fontStyle).assertEqual('FontStyle.Normal');
      done();
    });

    it('testTextStyle015', 0, async function (done) {
      console.info('[testTextStyle015] START');
      try {
        let eventData = {
          data: {
            "fontStyle": 0x80000000
          }
        }
        let indexEvent = {
          eventId: 39,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle015] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle015] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontStyle');
      console.info("[testTextStyle015] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle015] fontStyle: " + obj.$attrs.fontStyle);
      expect(obj.$attrs.fontStyle).assertEqual('FontStyle.Normal');
      done();
    });

    it('testTextStyle016', 0, async function (done) {
      console.info('[testTextStyle016] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('fontWeight');
      console.info("[testTextStyle016] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle016] fontWeight: " + obj.$attrs.fontWeight);
      expect(obj.$attrs.fontWeight).assertEqual('FontWeight.700');
      done();
    });

    it('testTextStyle017', 0, async function (done) {
      console.info('[testTextStyle017] START');
      try {
        let eventData = {
          data: {
            "fontWeight": 400
          }
        }
        let indexEvent = {
          eventId: 40,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle017] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle017] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontWeight');
      console.info("[testTextStyle017] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle017] fontWeight: " + obj.$attrs.fontWeight);
      expect(obj.$attrs.fontWeight).assertEqual('FontWeight.400');
      done();
    });

    it('testTextStyle018', 0, async function (done) {
      console.info('[testTextStyle018] START');
      try {
        let eventData = {
          data: {
            "fontWeight": 'error'
          }
        }
        let indexEvent = {
          eventId: 40,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle018] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle018] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontWeight');
      console.info("[testTextStyle018] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle018] fontWeight: " + obj.$attrs.fontWeight);
      expect(obj.$attrs.fontWeight).assertEqual('FontWeight.Normal');
      done();
    });

    it('testTextStyle019', 0, async function (done) {
      console.info('[testTextStyle019] START');
      try {
        let eventData = {
          data: {
            "fontWeight": 0x80000000
          }
        }
        let indexEvent = {
          eventId: 40,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle019] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle019] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontWeight');
      console.info("[testTextStyle019] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle019] fontWeight: " + obj.$attrs.fontWeight);
      expect(obj.$attrs.fontWeight).assertEqual('FontWeight.Normal');
      done();
    });

    it('testTextStyle020', 0, async function (done) {
      console.info('[testTextStyle020] START');
      try {
        let eventData = {
          data: {
            "fontWeight": 0x7FFFFFFF
          }
        }
        let indexEvent = {
          eventId: 40,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle020] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle020] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontWeight');
      console.info("[testTextStyle020] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle020] fontWeight: " + obj.$attrs.fontWeight);
      expect(obj.$attrs.fontWeight).assertEqual('FontWeight.Normal');
      done();
    });

    it('testTextStyle021', 0, async function (done) {
      console.info('[testTextStyle021] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('fontFamily');
      console.info("[testTextStyle021] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle021] fontFamily: " + obj.$attrs.fontFamily);
      expect(obj.$attrs.fontFamily).assertEqual('sans-serif');
      done();
    });

    it('testTextStyle022', 0, async function (done) {
      console.info('[testTextStyle022] START');
      try {
        let eventData = {
          data: {
            "fontFamily": 'Arial'
          }
        }
        let indexEvent = {
          eventId: 41,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle022] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle022] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontFamily');
      console.info("[testTextStyle022] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle022] fontFamily: " + obj.$attrs.fontFamily);
      expect(obj.$attrs.fontFamily).assertEqual('Arial');
      done();
    });

    it('testTextStyle023', 0, async function (done) {
      console.info('[testTextStyle023] START');
      try {
        let eventData = {
          data: {
            "fontFamily": 'error'
          }
        }
        let indexEvent = {
          eventId: 41,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTextStyle023] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testTextStyle023] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('fontFamily');
      console.info("[testTextStyle023] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testTextStyle023] fontFamily: " + obj.$attrs.fontFamily);
      expect(obj.$attrs.fontFamily).assertEqual('error');
      done();
    });
  });
}