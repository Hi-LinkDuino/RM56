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
import Utils from './Utils.ets';
import events_emitter from '@ohos.events.emitter';

export default function stepperJsunit() {
  describe('stepperTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/stepper',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get stepper state success " + JSON.stringify(pages));
        if (!("stepper" == pages.name)) {
          console.info("get stepper state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push stepper page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push stepper page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("stepper after each called");
    });

    it('stepperTest_0100', 0, async function (done) {
      console.info('stepperTest_0100 START');
      let strJson = getInspectorByKey('stepper');
      console.info("stepperTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("stepperTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Stepper');
      expect(obj.$attrs.index).assertEqual('0');
      console.info('stepperTest_0100 END');
      done();
    });

    it('stepperTest_0200', 0, async function (done) {
      console.info('stepperTest_0200 START');
      let strJson = getInspectorByKey('stepper');
      console.info("stepperTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("stepperTest_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.index).assertEqual('0');
      try {
        let eventData = {
          data: {
            "currentIndex": "1",
          }
        }
        let indexEvent = {
          eventId: 202,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("stepperTest_0200 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("stepperTest_0200 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('stepper');
      let objNew = JSON.parse(strJsonNew);
      console.info("stepperTest_0200 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.index).assertEqual('1');
      console.info('stepperTest_0200 END');
      done();
    });

    it('stepperTest_0300', 0, async function (done) {
      console.info('stepperTest_0300v START');
      let strJson = getInspectorByKey('stepper');
      console.info("stepperTest_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("stepperTest_0300 component obj is: " + JSON.stringify(obj));
      try {
        let eventData = {
          data: {
            "currentIndex": "11111111",
          }
        }
        let indexEvent = {
          eventId: 909,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("stepperTest_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("stepperTest_0300 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('stepper');
      let objNew = JSON.parse(strJsonNew);
      console.info("stepperTest_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.index).assertEqual('11111111');
      console.info('stepperTest_0300 END');
      done();
    });

    it('stepperTest_0400', 0, async function (done) {
      console.info('stepperTest_0400 START');
      let strJson = getInspectorByKey('stepper');
      console.info("stepperTest_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("stepperTest_0400 component obj is: " + JSON.stringify(obj));
      try {
        let eventData = {
          data: {
            "currentIndex": "-558038585",
          }
        }
        let indexEvent = {
          eventId: 909,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("stepperTest_0400 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("stepperTest_0400 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('stepper');
      let objNew = JSON.parse(strJsonNew);
      console.info("stepperTest_0400 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.index).assertEqual('-558038585');
      console.info('stepperTest_0400 END');
      done();
    });

    it('stepperTest_0500', 0, async function (done) {
      console.info('[stepperTest_0500] START');
      try {
        var eventData = {
          data: {
            "currentIndex": '-10'
          }
        }
        var innerEvent = {
          eventId: 910,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[stepperTest_0500] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[stepperTest_0500] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('stepper');
      let obj = JSON.parse(strJson);
      console.info("[stepperTest_0500] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.index).assertEqual('-10');
      console.info('stepperTest_0500 END');
      done();
    });
  })
}