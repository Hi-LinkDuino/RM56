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
export default function listJsunit() {
  describe('listTest', function () {
    beforeEach(async function (done) {
      console.info("list beforeEach start");
      let options = {
        uri: 'pages/list',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get list state pages:" + JSON.stringify(pages));
        if (!("list" == pages.name)) {
          console.info("get list state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push list page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push list page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("list after each called");
    });

    it('testList01', 0, async function (done) {
      console.info('[testList01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList01] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.listDirection).assertEqual('Axis.Vertical');
      expect(obj.$attrs.editMode).assertEqual('false');
      expect(obj.$attrs.edgeEffect).assertEqual('EdgeEffect.None');
      expect(obj.$attrs.chainAnimation).assertEqual('false');
      expect(obj.$attrs.divider.strokeWidth).assertEqual("2.00vp");
      expect(obj.$attrs.divider.color).assertEqual("#FFFFFFFF");
      expect(obj.$attrs.divider.startMargin).assertEqual("20.00vp");
      expect(obj.$attrs.divider.endMargin).assertEqual("20.00vp");
      console.info('[testList01] END');
      done();
    });

    it('testList02', 0, async function (done) {
      console.info('[testList02] START');
      try {
        var eventData = {
          data: {
            "listDirection": Axis.Horizontal
          }
        }
        var innerEvent = {
          eventId: 124,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList02] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList02] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.listDirection).assertEqual('Axis.Horizontal');
      console.info('testList02 END');
      done();
    });

    it('testList03', 0, async function (done) {
      console.info('[testList03] START');
      try {
        var eventData = {
          data: {
            "editMode": true
          }
        }
        var innerEvent = {
          eventId: 124,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList03] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList03] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.editMode).assertEqual('true');
      console.info('testList03 END');
      done();
    });

    it('testList04', 0, async function (done) {
      console.info('[testList04] START');
      try {
        var eventData = {
          data: {
            "edgeEffect": EdgeEffect.Spring
          }
        }
        var innerEvent = {
          eventId: 124,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList04] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList04] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.edgeEffect).assertEqual('EdgeEffect.Spring');
      console.info('testList04 END');
      done();
    });

    it('testList05', 0, async function (done) {
      console.info('[testList05] START');
      try {
        var eventData = {
          data: {
            "chainAnimation": true
          }
        }
        var innerEvent = {
          eventId: 124,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList05] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList05] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList05] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.chainAnimation).assertEqual('true');
      console.info('testList05 END');
      done();
    });

    it('testList06', 0, async function (done) {
      console.info('[testList06] START');
      try {
        var eventData = {
          data: {
            "strokeWidth": "3.000000vp"
          }
        }
        var innerEvent = {
          eventId: 125,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList06] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList06] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList06] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.divider.strokeWidth).assertEqual("3.00vp");
      console.info('testList06 END');
      done();
    });

    it('testList07', 0, async function (done) {
      console.info('[testList07] START');
      try {
        var eventData = {
          data: {
            "color": "#FF0000FF"
          }
        }
        var innerEvent = {
          eventId: 125,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList07] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList07] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList07] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.divider.color).assertEqual("#FF0000FF");
      console.info('testList07 END');
      done();
    });

    it('testList08', 0, async function (done) {
      console.info('[testList08] START');
      try {
        var eventData = {
          data: {
            "startMargin": "30.000000vp"
          }
        }
        var innerEvent = {
          eventId: 125,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList08] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList08] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList08] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.divider.startMargin).assertEqual("30.00vp");
      console.info('testList08 END');
      done();
    });

    it('testList09', 0, async function (done) {
      console.info('[testList09] START');
      try {
        var eventData = {
          data: {
            "endMargin": "30.000000vp"
          }
        }
        var innerEvent = {
          eventId: 125,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList09] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList09] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList09] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.divider.endMargin).assertEqual("30.00vp");
      console.info('testList09 END');
      done();
    });

    it('testList10', 0, async function (done) {
      console.info('[testList10] START');
      try {
        var eventData = {
          data: {
            "editMode": "true123"
          }
        }
        var innerEvent = {
          eventId: 901,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList10] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList10] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList10] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.editMode).assertEqual('true');
      console.info('testList10 END');
      done();
    });

    it('testList11', 0, async function (done) {
      console.info('[testList11] START');
      try {
        var eventData = {
          data: {
            "editMode": "39000000"
          }
        }
        var innerEvent = {
          eventId: 902,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList11] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList11] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList11] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.editMode).assertEqual('true');
      console.info('testList11 END');
      done();
    });

    it('testList12', 0, async function (done) {
      console.info('[testList12] START');
      try {
        var eventData = {
          data: {
            "chainAnimation": "true123"
          }
        }
        var innerEvent = {
          eventId: 903,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testList12] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testList12] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('list');
      let obj = JSON.parse(strJson);
      console.info("[testList12] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.chainAnimation).assertEqual('true');
      console.info('testList12 END');
      done();
    });
  })
}
