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

export default function layoutConstraintsJsunit() {
  describe('layoutConstraintsTest', function () {
    beforeEach(async function (done) {
      console.info("layoutConstraints beforeEach start");
      let options = {
        uri: 'pages/layoutConstraints',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get layoutConstraints state pages:" + JSON.stringify(pages));
        if (!("layoutConstraints" == pages.name)) {
          console.info("get layoutConstraints state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push layoutConstraints page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push layoutConstraints page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("layoutConstraints after each called");
    });

    it('testLayoutConstraints001', 0, async function (done) {
      console.info('[testLayoutConstraints001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('aspectRatio');
      console.info("[testLayoutConstraints001] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints001] aspectRatio: " + obj.$attrs.aspectRatio);
      expect(obj.$attrs.aspectRatio).assertEqual(1.5);
      done();
    });

    it('testLayoutConstraints002', 0, async function (done) {
      console.info('[testLayoutConstraints002] START');
      try {
        let eventData = {
          data: {
            "aspectRatio": 2
          }
        }
        let indexEvent = {
          eventId: 20,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testLayoutConstraints002] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testLayoutConstraints002] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('aspectRatio');
      console.info("[testLayoutConstraints002] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints002] aspectRatio: " + obj.$attrs.aspectRatio);
      expect(obj.$attrs.aspectRatio).assertEqual(2);
      done();
    });

    it('testLayoutConstraints003', 0, async function (done) {
      console.info('[testLayoutConstraints003] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('displayPriority');
      console.info("[testLayoutConstraints003] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints003] displayPriority: " + obj.$attrs.displayPriority);
      expect(obj.$attrs.displayPriority).assertEqual(1);
      done();
    });

    it('testLayoutConstraints004', 0, async function (done) {
      console.info('[testLayoutConstraints004] START');
      try {
        let eventData = {
          data: {
            "displayPriority": 5
          }
        }
        let indexEvent = {
          eventId: 21,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testLayoutConstraints004] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testLayoutConstraints004] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('displayPriority');
      console.info("[testLayoutConstraints004] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints004] displayPriority: " + obj.$attrs.displayPriority);
      expect(obj.$attrs.displayPriority).assertEqual(5);
      done();
    });

    it('testLayoutConstraints005', 0, async function (done) {
      console.info('[testLayoutConstraints005] START');
      try {
        let eventData = {
          data: {
            "aspectRatio": -4
          }
        }
        let indexEvent = {
          eventId: 1112,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testLayoutConstraints005] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testLayoutConstraints005] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('aspectRatio');
      console.info("[testLayoutConstraints005] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints005] aspectRatio: " + obj.$attrs.aspectRatio);
      expect(obj.$attrs.aspectRatio).assertEqual(-4);
      done();
    });

    it('testLayoutConstraints006', 0, async function (done) {
      console.info('[testLayoutConstraints006] START');
      try {
        let eventData = {
          data: {
            "aspectRatio": 9997077
          }
        }
        let indexEvent = {
          eventId: 1112,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testLayoutConstraints006] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testLayoutConstraints006] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('aspectRatio');
      console.info("[testLayoutConstraints006] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testLayoutConstraints006] aspectRatio: " + obj.$attrs.aspectRatio);
      expect(obj.$attrs.aspectRatio).assertEqual(9997077);
      done();
    });
  });
}