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

export default function textClockJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("text beforeEach start");
      let options = {
        uri: 'pages/textClock',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get text state pages:" + JSON.stringify(pages));
        if (!("text" == pages.name)) {
          console.info("get text state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push text page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push text page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("text after each called");
    });

    it('textClockTest01', 0, async function (done) {
      console.info('textClockTest01 START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textClock');
      let obj = JSON.parse(strJson);
      console.info("textClockTest01 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontSize).assertEqual('30.00fp');
      await Utils.sleep(1000);
      done();
    });

    it('textClockTest02', 0, async function (done) {
      console.info('[textClockTest02] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "fontSize": 30
          }
        }
        var innerEvent = {
          eventId: 219,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textClockTest02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textClockTest02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('textClock');
      let objNew = JSON.parse(strJsonNew);
      console.info("[textClockTest02] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.fontSize).assertEqual('30.00fp');
      done();
    });

    it('textClockTest03', 0, async function (done) {
      console.info('[textClockTest03] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('textClock');
      console.info("[textClockTest03] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[textClockTest03] fontColor:" + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FFFF0000');
      done();
    });

    it('textClockTest04', 0, async function (done) {
      console.info('[textClockTest04] START');
      try {
        let eventData = {
          data: {
            "fontColor": '#FF000990'
          }
        }
        let indexEvent = {
          eventId: 220,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textClockTest04] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[textClockTest04] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('textClock');
      console.info("[textClockTest04] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[textClockTest04] fontColor:" + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FF000990');
      done();
    });

    it('textClockTest05', 0, async function (done) {
      console.info('[textClockTest05] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textClock');
      let obj = JSON.parse(strJson);
      console.info("[textClockTest05] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.format).assertEqual('hhmmss');
      console.info('textClockTest05 END');
      done();
    });

    it('textClockTest06', 0, async function (done) {
      console.info('[textClockTest06] START');
      try {
        var eventData = {
          data: {
            "format": '20',
          }
        }
        var innerEvent = {
          eventId: 221,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[textClockTest06] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[textClockTest06] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('textClock');
      let obj = JSON.parse(strJson);
      console.info("[textClockTest06] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.format).assertEqual('20');
      console.info('textClockTest06 END');
      done();
    });
  })
}
