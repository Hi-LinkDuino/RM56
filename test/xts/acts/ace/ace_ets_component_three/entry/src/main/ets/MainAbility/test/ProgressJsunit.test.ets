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
import events_emitter from '@ohos.events.emitter';

export default function progressJsunit() {
  describe('progressTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/progress',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get progress state success " + JSON.stringify(pages));
        if (!("progress" == pages.name)) {
          console.info("get progress state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push progress page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push progress page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("progress after each called");
    });

    it('testProgress_100', 0, async function (done) {
      console.info('testProgress_100 START');
      await Utils.sleep(1000);
      let strJson1 = getInspectorByKey('LPValue');
      let objLP = JSON.parse(strJson1);
      console.info("testProgress_100 component objLP is: " + objLP.$attrs.value);
      try {
        let eventData = {
          data: {
            "value": 100
          }
        }
        var innerEvent = {
          eventId: 164,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_100 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_100 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_100 component obj is: " + JSON.stringify(obj.$attrs.value));
      expect(obj.$attrs.value).assertEqual("100.000000");
      console.info('testProgress_100 END');
      done();
    });

    it('testProgress_200', 0, async function (done) {
      console.info('[testProgress_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual("200.00vp");
      console.info('testProgress_200 END');
      done();
    });

    it('testProgress_300', 0, async function (done) {
      console.info('testProgress_300 START');
      let strJson1 = getInspectorByKey('EPValue');
      let objEP = JSON.parse(strJson1);
      console.info("testProgress_300 component objEP is: " + objEP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 100
          }
        }
        var innerEvent = {
          eventId: 165,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_300 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_300 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('EPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("100.000000");
      console.info('testProgress_300 END');
      done();
    });

    it('testProgress_400', 0, async function (done) {
      console.info('[testProgress_400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual("200.00vp");
      console.info('testProgress_400 END');
      done();
    });

    it('testProgress_500', 0, async function (done) {
      console.info('testProgress_500 START');
      let strJson1 = getInspectorByKey('CPValue');
      let objCP = JSON.parse(strJson1);
      console.info("testProgress_500 component objCP is: " + objCP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 100
          }
        }
        var innerEvent = {
          eventId: 166,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_500 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_500 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('EPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_500 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("100.000000");
      console.info('testProgress_500 END');
      done();
    });

    it('testProgress_600', 0, async function (done) {
      console.info('[testProgress_600] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('CPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual("100.00vp");
      console.info('testProgress_600 END');
      done();
    });

    it('testProgress_700', 0, async function (done) {
      console.info('testProgress_700 START');
      let strJson1 = getInspectorByKey('LPValue');
      let objLP = JSON.parse(strJson1);
      console.info("testProgress_700 component objLP is: " + objLP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 0
          }
        }
        var innerEvent = {
          eventId: 167,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_700 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_700 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_700 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("0.000000");
      console.info('testProgress_700 END');
      done();
    });

    it('testProgress_800', 0, async function (done) {
      console.info('testProgress_800 START');
      let strJson1 = getInspectorByKey('EPValue');
      let objEP = JSON.parse(strJson1);
      console.info("testProgress_800 component objEP is: " + objEP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 0
          }
        }
        var innerEvent = {
          eventId: 168,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_800 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_800 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('EPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_800 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("0.000000");
      console.info('testProgress_800 END');
      done();
    });

    it('testProgress_900', 0, async function (done) {
      console.info('testProgress_900 START');
      let strJson1 = getInspectorByKey('CPValue');
      let objCP = JSON.parse(strJson1);
      console.info("testProgress_900 component objCP is: " + objCP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 0
          }
        }
        var innerEvent = {
          eventId: 169,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_900 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_900 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('EPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_900 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("0.000000");
      console.info('testProgress_900 END');
      done();
    });

    it('testProgress_1000', 0, async function (done) {
      console.info('testProgress_1000 START');
      let strJson1 = getInspectorByKey('LPValue');
      let objLP = JSON.parse(strJson1);
      console.info("testProgress_1000 component objLP is: " + objLP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": -5
          }
        }
        var innerEvent = {
          eventId: 1021,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_1000 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_1000 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_1000 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("-5.000000");
      console.info('testProgress_1000 END');
      done();
    });

    it('testProgress_1100', 0, async function (done) {
      console.info('testProgress_1100 START');
      let strJson1 = getInspectorByKey('LPValue');
      let objLP = JSON.parse(strJson1);
      console.info("testProgress_1100 component objLP is: " + objLP.$attrs.value);
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "value": 121212121
          }
        }
        var innerEvent = {
          eventId: 1022,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testProgress_1100 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testProgress_1100 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('LPValue');
      let obj = JSON.parse(strJson);
      console.info("testProgress_1100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.value).assertEqual("121212121.000000");
      console.info('testProgress_1100 END');
      done();
    });
  })
}