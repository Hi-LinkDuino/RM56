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
export default function zIndexJsunit() {
  describe('zIndexTest', function () {
    beforeEach(async function (done) {
      console.info("zIndex beforeEach start");
      let options = {
        uri: 'pages/zIndex',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get zIndex state pages: " + JSON.stringify(pages));
        if (!("zIndex" == pages.name)) {
          console.info("get zIndex state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push zIndex page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push zIndex page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("zIndex after each called");
    });

    it('testZIndex01', 0, async function (done) {
      console.info('[testZIndex01] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('zIndex');
      let obj = JSON.parse(strJson);
      console.info("[testZIndex01] obj is: " + JSON.stringify(obj.$attrs));
      expect(obj.$attrs.zIndex).assertEqual(1);
      console.info('[testZIndex01] END');
      done();
    });

    it('testZIndex02', 0, async function (done) {
      console.info('[testZIndex02] START');
      try {
        var eventData = {
          data: {
            "zIndex": 4
          }
        }
        var innerEvent = {
          eventId: 236,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testZIndex02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testZIndex02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('zIndex');
      let obj = JSON.parse(strJson);
      console.info("[testZIndex02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.zIndex).assertEqual(4);
      console.info('testZIndex02 END');
      done();
    });

    it('testZIndex03', 0, async function (done) {
      console.info('[testZIndex03] START');
      try {
        var eventData = {
          data: {
            "zIndex": -5
          }
        }
        var innerEvent = {
          eventId: 236,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testZIndex03] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testZIndex03] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('zIndex');
      let obj = JSON.parse(strJson);
      console.info("[testZIndex03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.zIndex).assertEqual(-5);
      console.info('testZIndex03 END');
      done();
    });

    it('testZIndex04', 0, async function (done) {
      console.info('[testZIndex04] START');
      try {
        var eventData = {
          data: {
            "zIndex": 889900444
          }
        }
        var innerEvent = {
          eventId: 1113,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testZIndex04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testZIndex04] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('zIndex');
      let obj = JSON.parse(strJson);
      console.info("[testZIndex04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.zIndex).assertEqual(889900444);
      console.info('testZIndex04 END');
      done();
    });
  })
}
