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
import events_emitter from '@ohos.events.emitter'
import Utils from './Utils';

export default function badgeJsunit() {
  describe('badgeTest', function () {
    beforeEach(async function (done) {
      console.info("badge beforeEach start");
      let options = {
        uri: 'pages/badge',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get badge state pages: " + JSON.stringify(pages));
        if (!("badge" == pages.name)) {
          console.info("get badge state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push badge page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push badge page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("badge after each called");
    })

    it('testBadge01', 0, async function (done) {
      console.info('[testBadge01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('badge');
      let obj = JSON.parse(strJson);
      console.info("[testBadge01] component obj is: " + JSON.stringify(obj));
      let style = JSON.parse(obj.$attrs.style);
      expect(obj.$type).assertEqual('Badge');
      expect(style.fontSize).assertEqual('20.00fp');
      done();
    });

    it('testBadge02', 0, async function (done) {
      console.info('[testBadge02] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "fontSizeValue": "30",
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge02] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.fontSize).assertEqual('30.00fp');
      done();
    });

    it('testBadge03', 0, async function (done) {
      console.info('[testBadge03] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "fontSizeValue": "-1",
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge03] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge03] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge03] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.fontSize).assertEqual('-1.00fp');
      done();
    });

    it('testBadge04', 0, async function (done) {
      console.info('[testBadge04] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "fontSizeValue": "121212121",
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge04] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge04] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.fontSize).assertEqual('121212121.00fp');
      done();
    });

    it('testBadge05', 0, async function (done) {
      console.info('[testBadge05] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "fontSizeValue": "121212121vaas",
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge05] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge05] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge05] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      console.info("[testBadge05] component style is: " + JSON.stringify(style));
      expect(style.fontSize).assertEqual('121212121.00fp');
      done();
    });

    it('testBadge06', 0, async function (done) {
      console.info('[testBadge06] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('badge2');
      let obj = JSON.parse(strJson);
      console.info("[testBadge06] component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Badge');
      expect(obj.$attrs.value).assertEqual('new');
      done();
    });

    it('testBadge07', 0, async function (done) {
      console.info('[testBadge07] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('badge2');
      let obj = JSON.parse(strJson);
      console.info("[testBadge07] component obj is: " + JSON.stringify(obj));
      let style = JSON.parse(obj.$attrs.style);
      expect(obj.$type).assertEqual('Badge');
      expect(style.color).assertEqual('#FFFF0000');
      done();
    });

    it('testBadge08', 0, async function (done) {
      console.info('[testBadge08] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colorValue": Color.Blue,
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge08] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge08] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge2');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge08] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.color).assertEqual('#FF0000FF');
      done();
    });

    it('testBadge09', 0, async function (done) {
      console.info('[testBadge09] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "message": "test",
          }
        }
        var innerEvent = {
          eventId: 63,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge09] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge09] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge2');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge09] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.value).assertEqual('test');
      done();
    });

    it('testBadge10', 0, async function (done) {
      console.info('[testBadge10] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colorValue": ab88,
          }
        }
        var innerEvent = {
          eventId: 64,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge10] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge10] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge2');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge10] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.color).assertEqual('#FF0000FF');
      done();
    });

    it('testBadge11', 0, async function (done) {
      console.info('[testBadge11] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colorValue": -300,
          }
        }
        var innerEvent = {
          eventId: 65,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBadge11] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBadge11] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('badge2');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBadge11] component objNew is: " + JSON.stringify(objNew));
      let style = JSON.parse(objNew.$attrs.style);
      expect(style.color).assertEqual('#FFFFFED4');
      done();
    });
  })
}