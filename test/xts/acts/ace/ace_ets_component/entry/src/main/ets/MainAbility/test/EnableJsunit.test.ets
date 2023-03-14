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
import Utils from './Utils.ets';
export default function enableJsunit() {
  describe('enableTest', function () {
    beforeEach(async function (done) {
      console.info("enable beforeEach start");
      let options = {
        uri: 'pages/enable',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get enable state pages: " + JSON.stringify(pages));
        if (!("enable" == pages.name)) {
          console.info("get enable state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push enable page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push enable page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("enable after each called");
    });

    it('testEnable01', 0, async function (done) {
      console.info('[testEnable01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('enabled');
      let obj = JSON.parse(strJson);
      console.info("[testEnable01] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.enabled).assertEqual(true);
      console.info('[testEnable01] END');
      done();
    });

    it('testEnable02', 0, async function (done) {
      console.info('[testEnable02] START');
      try {
        var eventData = {
          data: {
            "enabled": false,
            "text": "disable"
          }
        }
        var innerEvent = {
          eventId: 93,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEnable02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEnable02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('enabled');
      let obj = JSON.parse(strJson);
      console.info("[testEnable02] obj is: " + JSON.stringify(obj.$attrs.enabled));
      expect(obj.$attrs.enabled).assertEqual(false);
      console.info('testEnable02 END');
      done();
    });

    it('testEnable03', 0, async function (done) {
      console.info('[testEnable03] START');
      try {
        var eventData = {
          data: {
            "enabled": -3
          }
        }
        var innerEvent = {
          eventId: 1111,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEnable03] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEnable03] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('enabled');
      let obj = JSON.parse(strJson);
      console.info("[testEnable03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.enabled).assertEqual(true);
      console.info('testEnable03 END');
      done();
    });

    it('testEnable04', 0, async function (done) {
      console.info('testEnable04 START');
      try {
        let eventData = {
          data: {
            "enabled": 33567444
          }
        }
        let indexEvent = {
          eventId: 1111,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testEnable04 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testEnable04 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('enabled');
      let objNew = JSON.parse(strJsonNew);
      console.info("testEnable04 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.enabled).assertEqual(true);
      done();
    });
  })
}
