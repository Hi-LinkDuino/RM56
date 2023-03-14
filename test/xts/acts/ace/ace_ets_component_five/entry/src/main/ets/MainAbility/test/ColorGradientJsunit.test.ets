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

export default function colorGradientJsunit() {
  describe('borderTest', function () {
    beforeEach(async function (done) {
      console.info("colorGradient beforeEach start");
      let options = {
        uri: 'pages/colorGradient',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get colorGradient state pages:" + JSON.stringify(pages));
        if (!("colorGradient" == pages.name)) {
          console.info("get colorGradient state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push colorGradient page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push colorGradient page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("colorGradient after each called");
    });

    it('testColorGradient001', 0, async function (done) {
      console.info('[testColorGradient001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('linearGradient');
      let obj = JSON.parse(strJson);
      console.info("[testColorGradient001] component obj is: " + JSON.stringify(obj.$attrs.linearGradient));
      expect(obj.$attrs.linearGradient.angle).assertEqual('90.00px');
      expect(obj.$attrs.linearGradient.direction).assertEqual('GradientDirection.Left');
      expect(obj.$attrs.linearGradient.repeating).assertEqual('true');
      console.info('[testColorGradient001] END');
      done();
    });

    it('testColorGradient002', 0, async function (done) {
      console.info('[testColorGradient002] START');
      await Utils.sleep(1000);
      let linearGradientValue = {
        angle: 80,
        direction: GradientDirection.Top,
        colors: [[0xAEE1E1, 0.0], [0xD3E0DC, 0.3], [0xFCD1D1, 1.0]],
        repeating: false
      };
      try {
        var eventData = {
          data: {
            "linearGradientValue": JSON.stringify(linearGradientValue)
          }
        }
        var innerEvent = {
          eventId: 5,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testColorGradient002] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testColorGradient002] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('linearGradient');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testColorGradient002] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.linearGradient.angle).assertEqual('80.00px');
      expect(objNew.$attrs.linearGradient.direction).assertEqual('GradientDirection.Top');
      expect(objNew.$attrs.linearGradient.repeating).assertEqual('false');
      console.info('[testColorGradient002] END');
      done();
    });

    it('testColorGradient003', 0, async function (done) {
      await Utils.sleep(1000)
      console.info('[testColorGradient003] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('sweepGradient');
      let obj = JSON.parse(strJson);
      console.info("[testColorGradient003] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.sweepGradient.end).assertEqual('359.00px');
      expect(obj.$attrs.sweepGradient.start).assertEqual('0.00px');
      expect(obj.$attrs.sweepGradient.repeating).assertEqual('true');
      console.info('[testColorGradient003] END');
      done();
    });

    it('testColorGradient004', 0, async function (done) {
      console.info('[testColorGradient004] START');
      await Utils.sleep(1000);
      let sweepGradientValue = {
        center: [50, 50],
        start: 10,
        end: 356,
        colors: [[0xAEE1E1, 0.0], [0xD3E0DC, 0.3], [0xFCD1D1, 1.0]],
        repeating: false
      };
      try {
        var eventData = {
          data: {
            "sweepGradientValue": JSON.stringify(sweepGradientValue)
          }
        }
        var innerEvent = {
          eventId: 6,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testColorGradient004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testColorGradient004] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('sweepGradient');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testColorGradient004] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.sweepGradient.end).assertEqual('356.00px');
      expect(objNew.$attrs.sweepGradient.start).assertEqual('10.00px');
      expect(objNew.$attrs.sweepGradient.repeating).assertEqual('false');
      console.info('[testColorGradient004] END');
      done();
    });

    it('testColorGradient005', 0, async function (done) {
      await Utils.sleep(1000)
      console.info('[testColorGradient005] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('radialGradient');
      let obj = JSON.parse(strJson);
      console.info("[testColorGradient005] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.radialGradient.radius).assertEqual('60.00vp');
      expect(obj.$attrs.radialGradient.center[0]).assertEqual('50.00vp');
      expect(obj.$attrs.radialGradient.center[1]).assertEqual('50.00vp');
      expect(obj.$attrs.radialGradient.repeating).assertEqual('true');
      console.info('[testColorGradient005] END');
      done();
    });

    it('testColorGradient006', 0, async function (done) {
      console.info('[testColorGradient006] START');
      await Utils.sleep(1000);
      let radialGradientValue = {
        center: [60, 60],
        radius: 70,
        colors: [[0xAEE1E1, 0.0], [0xD3E0DC, 0.3], [0xFCD1D1, 1.0]],
        repeating: false
      };
      try {
        var eventData = {
          data: {
            "radialGradientValue": JSON.stringify(radialGradientValue)
          }
        }
        var innerEvent = {
          eventId: 7,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testColorGradient006] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testColorGradient006] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('radialGradient');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testColorGradient006] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.radialGradient.radius).assertEqual('70.00vp');
      expect(objNew.$attrs.radialGradient.center[0]).assertEqual('60.00vp');
      expect(objNew.$attrs.radialGradient.center[1]).assertEqual('60.00vp');
      expect(objNew.$attrs.radialGradient.repeating).assertEqual('false');
      console.info('[testColorGradient006] END');
      done();
    });
  })
}