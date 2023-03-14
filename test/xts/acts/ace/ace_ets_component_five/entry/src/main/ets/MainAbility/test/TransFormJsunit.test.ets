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
import Utils from './Utils';
export default function transFormJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("transForm beforeEach start");
      let options = {
        uri: 'pages/transForm',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get transForm state pages:" + JSON.stringify(pages));
        if (!("transForm " == pages.name)) {
          console.info("get transForm  state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push transForm  page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push transForm  page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("transFrom each called");
    });

    it('testTransForm001', 0, async function (done) {
      console.info('[testTransForm001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('rotate');
      let obj = JSON.parse(strJson);
      console.info("[testTransForm001] component strJson:" + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Row');
      expect(obj.$attrs.rotate.x).assertEqual('1.000000');
      expect(obj.$attrs.rotate.y).assertEqual('1.000000');
      expect(obj.$attrs.rotate.z).assertEqual('1.000000');
      expect(obj.$attrs.rotate.centerX).assertEqual('50.00%');
      expect(obj.$attrs.rotate.centerY).assertEqual('50.00%');
      expect(obj.$attrs.rotate.angle).assertEqual('300.000000');
      done();
    });

    it('testTransForm002', 0, async function (done) {
      console.info('[testTransForm002] START');
      await Utils.sleep(1000);
      let rotateValue = {x: 2, y: 2, z: 2, centerX: '60%', centerY: '60%', angle: 100}
      try {
        var eventData = {
          data: {
            "rotateValue": JSON.stringify(rotateValue)
          }
        }
        var innerEvent = {
          eventId: 45,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTransForm002] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testTransForm002] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('rotate');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testTransForm002] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.rotate.x).assertEqual('2.000000');
      expect(objNew.$attrs.rotate.y).assertEqual('2.000000');
      expect(objNew.$attrs.rotate.z).assertEqual('2.000000');
      expect(objNew.$attrs.rotate.centerX).assertEqual('60.00%');
      expect(objNew.$attrs.rotate.centerY).assertEqual('60.00%');
      expect(objNew.$attrs.rotate.angle).assertEqual('100.000000');
      done();
    });

    it('testTransForm003', 0, async function (done) {
      console.info('[testTransForm003] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('rotate3');
      let obj = JSON.parse(strJson);
      console.info("[testTransForm003] component strJson:" + JSON.stringify(obj));
      expect(obj.$attrs.scale.x).assertEqual('2.000000');
      expect(obj.$attrs.scale.y).assertEqual('0.500000');
      done();
    });

    it('testTransForm004', 0, async function (done) {
      console.info('[testTransForm004] START');
      await Utils.sleep(1000);
      let scaleValue = { x: 1, y: 1 }
      try {
        var eventData = {
          data: {
            "scaleValue": JSON.stringify(scaleValue)
          }
        }
        var innerEvent = {
          eventId: 46,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testTransForm004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testTransForm004] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('rotate3');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testTransForm004] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.scale.x).assertEqual('1.000000');
      expect(objNew.$attrs.scale.y).assertEqual('1.000000');
      done();
    });
  })
}




