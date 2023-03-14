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
import events_emitter from '@ohos.events.emitter'
import Utils from './Utils';

export default function videoJsunit() {
  describe('videoTest', function () {
    beforeEach(async function (done) {
      console.info("video beforeEach start");
      let options = {
        uri: 'pages/video',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get video state pages:" + JSON.stringify(pages));
        if (!("video" == pages.name)) {
          console.info("get video state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push video page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push video page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("video after each called");
    });

    it('testVideo01', 0, async function (done) {
      console.info('[testVideo01] START');
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      expect(objNew.$type).assertEqual('Video');
      console.info('[testVideo01] END');
      done();
    });

    it('testVideo02', 0, async function (done) {
      console.info('[testVideo02] START');
      try {
        var eventData = {
          data: {
            "progressRates": PlaybackSpeed.Speed_Forward_2_00_X,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo02] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo02] END');
      done();
    });

    it('testVideo03', 0, async function (done) {
      console.info('[testVideo03] START ');
      try {
        var eventData = {
          data: {
            "progressRates": PlaybackSpeed.Speed_Forward_1_75_X,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo03] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo03] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo03] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo03] END');
      done();
    });

    it('testVideo04', 0, async function (done) {
      console.info('[testVideo04] START');
      try {
        var eventData = {
          data: {
            "progressRates": PlaybackSpeed.Speed_Forward_1_25_X,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo04] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo04] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo04] END');
      done();
    });

    it('testVideo05', 0, async function (done) {
      console.info('[testVideo05] START');
      try {
        var eventData = {
          data: {
            "progressRates": PlaybackSpeed.Speed_Forward_1_00_X,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo05] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo05] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo05] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo05] END');
      done();
    });

    it('testVideo06', 0, async function (done) {
      console.info('[testVideo06] START');
      try {
        var eventData = {
          data: {
            "progressRates": PlaybackSpeed.Speed_Forward_0_75_X,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo06] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo06] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo06] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo06] END');
      done();
    });

    it('testVideo07', 0, async function (done) {
      console.info('[testVideo07] START');
      try {
        var eventData = {
          data: {
            "isStopped": true,
          }
        }
        var innerEvent = {
          eventId: 235,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVideo07] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVideo07] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('video');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testVideo07] component objNew is: " + JSON.stringify(objNew));
      console.info('[testVideo07] END');
      done();
    });

  });
}
