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

export default function ToggleJsunit() {
  describe('toggleTest', function () {
    beforeEach(async function (done) {
      console.info("toggle beforeEach start");
      let options = {
        uri: 'pages/toggle',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get toggle state pages:" + JSON.stringify(pages));
        if (!("toggle" == pages.name)) {
          console.info("get toggle state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push toggle page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push toggle page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("toggle after each called");
    });

    it('testToggle01', 0, async function (done) {
      console.info('[testToggle01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('toggle');
      let obj = JSON.parse(strJson);
      console.info("[testToggle01] obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Toggle');
      expect(obj.$attrs.selectedColor).assertEqual('#330A59F7');
      console.info('[testToggle01] END');
      done();
    });

    it('testToggle02', 0, async function (done) {
      console.info('[testToggle02] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('toggle');
      let obj = JSON.parse(strJson);
      console.info("[testToggle02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.type).assertEqual('ToggleType.Button');
      console.info('[testToggle02] END');
      done();
    });

    it('testToggle03', 0, async function (done) {
      console.info('[testToggle03] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('toggle');
      let obj = JSON.parse(strJson);
      console.info("[testToggle03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.isOn).assertEqual('false');
      console.info('[testToggle03] END');
      done();
    });

    it('testToggle04', 0, async function (done) {
      console.info('testToggle04 START');
      try {
        let eventData = {
          data: {
            "isOn": true,
          }
        }
        let indexEvent = {
          eventId: 231,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle04 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle04 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle04 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.isOn).assertEqual('true');
      console.info('testToggle04 END');
      done();
    });

    it('testToggle05', 0, async function (done) {
      console.info('testToggle05 START');
      try {
        let eventData = {
          data: {
            "selectedColor": '#330A59F8',
          }
        }
        let indexEvent = {
          eventId: 232,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle05 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle05 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle05 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.selectedColor).assertEqual('#330A59F8');
      console.info('testToggle05 END');
      done();
    });

    it('testToggle06', 0, async function (done) {
      console.info('testToggle06 START');
      try {
        let eventData = {
          data: {
            "selectedColor": '-a',
          }
        }
        let indexEvent = {
          eventId: 1033,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle06 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle06 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle06 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.selectedColor).assertEqual('#FF000000');
      console.info('testToggle06 END');
      done();
    });

    it('testToggle07', 0, async function (done) {
      console.info('testToggle07 START');
      try {
        let eventData = {
          data: {
            "selectedColor": '-0.1',
          }
        }
        let indexEvent = {
          eventId: 1034,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle07 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle07 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle07 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.selectedColor).assertEqual('#FF000000');
      console.info('testToggle07 END');
      done();
    });

    it('testToggle08', 0, async function (done) {
      console.info('testToggle04 START');
      try {
        let eventData = {
          data: {
            "isOn": 'aaa',
          }
        }
        let indexEvent = {
          eventId: 1035,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle04 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle04 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle04 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.isOn).assertEqual('false');
      console.info('testToggle04 END');
      done();
    });

    it('testToggle09', 0, async function (done) {
      console.info('testToggle04 START');
      try {
        let eventData = {
          data: {
            "isOn": -1,
          }
        }
        let indexEvent = {
          eventId: 1036,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testToggle04 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testToggle04 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('toggle');
      let objNew = JSON.parse(strJsonNew);
      console.info("testToggle04 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.isOn).assertEqual('false');
      console.info('testToggle04 END');
      done();
    });
  })
}
