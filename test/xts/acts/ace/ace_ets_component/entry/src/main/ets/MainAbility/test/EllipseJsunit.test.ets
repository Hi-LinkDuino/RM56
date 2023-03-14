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

export default function ellipseJsunit() {
  describe('ellipseTest', function () {
    beforeEach(async function (done) {
      console.info("ellipse beforeEach start");
      let options = {
        uri: 'pages/ellipse',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get ellipse state pages:" + JSON.stringify(pages));
        if (!("ellipse" == pages.name)) {
          console.info("get ellipse state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push ellipse page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ellipse page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("ellipse after each called");
    });

    it('testEllipse01', 0, async function (done) {
      console.info('[testEllipse01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('200.00px');
      expect(obj.$attrs.width).assertEqual('200.00px');
      console.info('testEllipse02 END');
      done();
    });

    it('testEllipse02', 0, async function (done) {
      console.info('[testEllipse02] START');
      try {
        var eventData = {
          data: {
            "width": '100px'
          }
        }
        var innerEvent = {
          eventId: 92,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse02] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('100.00px');
      console.info('testEllipse02 END');
      done();
    });

    it('testEllipse03', 0, async function (done) {
      console.info('[testEllipse03] START');
      try {
        var eventData = {
          data: {
            "height": '500px'
          }
        }
        var innerEvent = {
          eventId: 92,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse03] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse03] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('500.00px');
      console.info('testEllipse03 END');
      done();
    });

    it('testEllipse04', 0, async function (done) {
      console.info('[testEllipse04] START');
      try {
        var eventData = {
          data: {
            "width": '1111111111111'
          }
        }
        var innerEvent = {
          eventId: 907,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse04] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('1111111111111.00px');
      console.info('testEllipse04 END');
      done();
    });

    it('testEllipse05', 0, async function (done) {
      console.info('[testEllipse05] START');
      try {
        var eventData = {
          data: {
            "height": '12111111111111'
          }
        }
        var innerEvent = {
          eventId: 907,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse05] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse05] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse05] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('12111111111111.00px');
      console.info('testEllipse05 END');
      done();
    });

    it('testEllipse06', 0, async function (done) {
      console.info('[testEllipse06] START');
      try {
        var eventData = {
          data: {
            "height": '-40'
          }
        }
        var innerEvent = {
          eventId: 908,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse06] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse06] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse06] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('-40.00px');
      console.info('testEllipse06 END');
      done();
    });

    it('testEllipse07', 0, async function (done) {
      console.info('[testEllipse07] START');
      try {
        var eventData = {
          data: {
            "width": '-10'
          }
        }
        var innerEvent = {
          eventId: 908,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testEllipse07] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testEllipse07] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ellipse');
      let obj = JSON.parse(strJson);
      console.info("[testEllipse07] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('-10.00px');
      console.info('testEllipse07 END');
      done();
    });
  })
}
