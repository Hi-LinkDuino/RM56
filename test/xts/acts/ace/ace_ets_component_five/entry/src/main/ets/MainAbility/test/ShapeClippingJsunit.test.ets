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

export default function shapeClippingJsunit() {
  describe('borderTest', function () {
    beforeEach(async function (done) {
      console.info("shapeClipping beforeEach start");
      let options = {
        uri: 'pages/shapeClipping',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get shapeClipping state pages:" + JSON.stringify(pages));
        if (!("shapeClipping" == pages.name)) {
          console.info("get shapeClipping state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push shapeClipping page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push shapeClipping page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("shapeClipping after each called");
    });

    it('testShapeClipping001', 0, async function (done) {
      console.info('[testShapeClipping001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('image_clip');
      let obj = JSON.parse(strJson);
      console.info("[testShapeClipping001] component obj is: " + JSON.stringify(obj));
      let clip = JSON.parse(obj.$attrs.clip);
      expect(clip.shape).assertEqual('Circle');
      let strJson1 = getInspectorByKey('image_clip_true');
      let obj1 = JSON.parse(strJson1);
      console.info("[testShapeClipping001] component obj1 is: " + JSON.stringify(obj1));
      expect(obj1.$attrs.clip).assertEqual('true');
      console.info('[testShapeClipping001] END');
      done();
    });

    it('testShapeClipping002', 0, async function (done) {
      console.info('[testShapeClipping002] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('image_mask');
      let obj = JSON.parse(strJson);
      console.info("[testShapeClipping002] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.mask.shape).assertEqual('Rect');
      expect(obj.$attrs.clip).assertEqual('false');
      console.info('[testShapeClipping002] END');
      done();
    });

    it('testShapeClipping003', 0, async function (done) {
      console.info('[testShapeClipping003] START');
      try {
        var eventData = {
          data: {
            "maskValue": false
          }
        }
        var innerEvent = {
          eventId: 32,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testShapeClipping003] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testShapeClipping003] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('image_mask');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testShapeClipping003] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.mask.shape).assertEqual('Circle');
      console.info('[testShapeClipping003] END');
      done();
    });

    it('testShapeClipping004', 0, async function (done) {
      console.info('[testShapeClipping004] START');
      try {
        var eventData = {
          data: {
            "clipValue": false
          }
        }
        var innerEvent = {
          eventId: 33,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testShapeClipping004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testShapeClipping004] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('image_clip');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testShapeClipping004] component objNew is: " + JSON.stringify(objNew));
      let clip = JSON.parse(objNew.$attrs.clip);
      expect(clip.shape).assertEqual('Rect');
      console.info('[testShapeClipping004] END');
      done();
    });
  })
}