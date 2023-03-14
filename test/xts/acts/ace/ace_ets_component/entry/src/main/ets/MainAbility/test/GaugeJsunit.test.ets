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
import Utils from './Utils.ets';
export default function gaugeJsunit() {
  describe('gaugeTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/gauge',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get gauge state success " + JSON.stringify(pages));
        if (!("gauge" == pages.name)) {
          console.info("get gauge state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push gauge page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push gauge page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("gauge after each called");
    });

    it('testGauge001', 0, async function (done) {
      console.info('[testGauge001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('gauge');
      let obj = JSON.parse(strJson);
      console.info("[testGauge001] component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Gauge');
      expect(obj.$attrs.strokeWidth).assertEqual('20.00vp');
      expect(obj.$attrs.width).assertEqual('200.00vp');
      expect(obj.$attrs.height).assertEqual('200.00vp');
      expect(obj.$attrs.value).assertEqual('50.00');
      done();
    });

    it('testGauge002', 0, async function (done) {
      console.info('[testGauge002] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "gaugeValue": "10"
          }
        }
        var innerEvent = {
          eventId: 102,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge002] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge002] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge002] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.value).assertEqual('10.00');
      done();
    });

    it('testGauge003', 0, async function (done) {
      console.info('[testGauge003] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "strokeWidthValue": "30",
          }
        }
        var innerEvent = {
          eventId: 102,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge003] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge003] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge003] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.strokeWidth).assertEqual('30.00vp');
      done();
    });

    it('testGauge004', 0, async function (done) {
      console.info('[testGauge004] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "startAngleValue": "200"
          }
        }
        var innerEvent = {
          eventId: 102,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge004] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge004] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.startAngle).assertEqual('200.00');
      done();
    });

    it('testGauge005', 0, async function (done) {
      console.info('[testGauge005] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "endAngleValue": "200"
          }
        }
        var innerEvent = {
          eventId: 102,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge005] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge005] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge005] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.endAngle).assertEqual('200.00');
      done();
    });

    it('testGauge006', 0, async function (done) {
      console.info('[testGauge006] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colorValues": JSON.stringify([[0x317AF7, 1], [0x5BA854, 1], [0xE08C3A, 1]])
          }
        }
        var innerEvent = {
          eventId: 103,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge006] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge006] change component data error: " + err.message);
      }
      done();
    });

    it('testGauge007', 0, async function (done) {
      console.info('[testGauge007] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "gaugeValue": "-10"
          }
        }
        var innerEvent = {
          eventId: 904,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge007] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge007] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge007] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.value).assertEqual('-10.00');
      done();
    });

    it('testGauge008', 0, async function (done) {
      console.info('[testGauge008] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "gaugeValue": 'nan'
          }
        }
        var innerEvent = {
          eventId: 904,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge008] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge008] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge008] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.value).assertEqual('nan');
      done();
    });

    it('testGauge009', 0, async function (done) {
      console.info('[testGauge009] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "strokeWidthValue": "-30",
          }
        }
        var innerEvent = {
          eventId: 904,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge009] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge009] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge009] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.strokeWidth).assertEqual('-30.00vp');
      done();
    });

    it('testGauge010', 0, async function (done) {
      console.info('[testGauge010] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "startAngleValue": "-10"
          }
        }
        var innerEvent = {
          eventId: 905,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge010] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge010] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge010] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.startAngle).assertEqual('-10.00');
      done();
    });

    it('testGauge011', 0, async function (done) {
      console.info('[testGauge011] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "startAngleValue": "nan"
          }
        }
        var innerEvent = {
          eventId: 905,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge011] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge011] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge011] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.startAngle).assertEqual('nan');
      done();
    });

    it('testGauge012', 0, async function (done) {
      console.info('[testGauge012] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "endAngleValue": "nan"
          }
        }
        var innerEvent = {
          eventId: 906,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge012] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge012] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge012] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.endAngle).assertEqual('nan');
      done();
    });

    it('testGauge013', 0, async function (done) {
      console.info('[testGauge013] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "endAngleValue": "-10"
          }
        }
        var innerEvent = {
          eventId: 906,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge013] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge013] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge013] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.endAngle).assertEqual('-10.00');
      done();
    });

    it('testGauge014', 0, async function (done) {
      console.info('[testGauge014] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "strokeWidthValue": "121111111"
          }
        }
        var innerEvent = {
          eventId: 905,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testGauge014] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testGauge014] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('gauge');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testGauge014] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.strokeWidth).assertEqual('121111111.00vp');
      done();
    });
  })
}
