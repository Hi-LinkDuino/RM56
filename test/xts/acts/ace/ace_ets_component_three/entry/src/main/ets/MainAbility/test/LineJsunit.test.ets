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

export default function LineJsunit() {
  describe('LineTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/line',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get line state success " + JSON.stringify(pages));
        if (!("line" == pages.name)) {
          console.info("get line state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push line page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push line page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("line after each called");
    });

    it('lineTest_0100', 0, async function (done) {
      console.info('lineTest_0100 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('line');
      console.info("lineTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("lineTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.startPoint[0]).assertEqual(0);
      done();
    });

    it('lineTest_0200', 0, async function (done) {
      console.info('lineTest_0200 START');
      let strJson = getInspectorByKey('line');
      console.info("lineTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("lineTest_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.startPoint[1]).assertEqual(0);
      done();
    });

    it('lineTest_0300', 0, async function (done) {
      console.info('lineTest_0300 START');
      let strJson = getInspectorByKey('line');
      console.info("lineTest_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("lineTest_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.endPoint[0]).assertEqual(50);
      done();
    });

    it('lineTest_0400', 0, async function (done) {
      console.info('lineTest_0400 START');
      let strJson = getInspectorByKey('line');
      console.info("lineTest_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("lineTest_0400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.endPoint[1]).assertEqual(100);
      done();
    });

    it('lineTest_0500', 0, async function (done) {
      console.info('lineTest_0500 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "startPoint": 50,
            "startPointOne": 50,
          }
        }
        var innerEvent = {
          eventId: 131,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("lineTest_0500 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("lineTest_0500 change component error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('line');
      var obj = JSON.parse(strJson);
      console.info("lineTest_0500 component obj is: " + JSON.stringify(obj.$attrs.startPoint[0]));
      console.info("lineTest_0500 component objOne is: " + JSON.stringify(obj.$attrs.startPoint[1]));
      expect(obj.$attrs.startPoint[0]).assertEqual(50);
      expect(obj.$attrs.startPoint[1]).assertEqual(50);
      done();
    });

    it('lineTest_0600', 0, async function (done) {
      console.info('lineTest_0600 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "endPoint": 100,
            "endPointOne": 150,
          }
        }
        var innerEvent = {
          eventId: 131,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("lineTest_0600 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("lineTest_0600 change component error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('line');
      var obj = JSON.parse(strJson);
      console.info("lineTest_0600 component obj is: " + JSON.stringify(obj.$attrs.endPoint[0]));
      console.info("lineTest_0600 component objOne is: " + JSON.stringify(obj.$attrs.endPoint[1]));
      expect(obj.$attrs.endPoint[0]).assertEqual(100);
      expect(obj.$attrs.endPoint[1]).assertEqual(150);
      done();
    });

    it('lineTest_0700', 0, async function (done) {
      console.info('lineTest_0700 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "startPoint": 'A',
            "startPointOne": 'B',
          }
        }
        var innerEvent = {
          eventId: 131,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("lineTest_0700 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("lineTest_0700 change component error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('line');
      var obj = JSON.parse(strJson);
      console.info("lineTest_0700 component obj is: " + JSON.stringify(obj.$attrs.startPoint[0]));
      console.info("lineTest_0700 component objOne is: " + JSON.stringify(obj.$attrs.startPoint[1]));
      expect(obj.$attrs.startPoint[0]).assertEqual(0);
      expect(obj.$attrs.startPoint[1]).assertEqual(0);
      done();
    });

    it('lineTest_0800', 0, async function (done) {
      console.info('lineTest_0900 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "endPoint": 'A',
            "endPointOne": 'B',
          }
        }
        var innerEvent = {
          eventId: 131,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("lineTest_0800 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("lineTest_0800 change component error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('line');
      var obj = JSON.parse(strJson);
      console.info("lineTest_0800 component objOne is: " + JSON.stringify(obj.$attrs.endPoint[1]));
      console.info("lineTest_0800 component obj is: " + JSON.stringify(obj.$attrs.endPoint[0]));
      expect(obj.$attrs.endPoint[1]).assertEqual(0);
      expect(obj.$attrs.endPoint[0]).assertEqual(0);
      done();
    });
  })
}