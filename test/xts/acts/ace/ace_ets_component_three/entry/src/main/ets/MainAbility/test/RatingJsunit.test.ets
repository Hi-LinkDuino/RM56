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

export default function ratingJsunit() {
  describe('ratingTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/rating',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get rating state success " + JSON.stringify(pages));
        if (!("rating" == pages.name)) {
          console.info("get rating state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push rating page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push rating page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("rating after each called");
    });

    it('testRating_100', 0, async function (done) {
      console.info('testRating_100 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 2
          }
        }
        var innerEvent = {
          eventId: 173,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_100 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_100 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("2.000000");
      console.info('testRating_100 END');
      done();
    });

    it('testRating_200', 0, async function (done) {
      console.info('[testRating_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.stepSize).assertEqual("0.500000");
      console.info('testRating_200 END');
      done();
    });

    it('testRating_300', 0, async function (done) {
      console.info('testRating_300 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "rating": 3
          }
        }
        var innerEvent = {
          eventId: 174,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_300 start to publish emit");
        console.info("eventData.data.rating value" + eventData.data.rating);
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_300 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("3.000000");
      console.info('testRating_300 END');
      done();
    });

    it('testRating_400', 0, async function (done) {
      console.info('testRating_400 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 4
          }
        }
        var innerEvent = {
          eventId: 175,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_400 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_400 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("4.000000");
      console.info('testRating_400 END');
      done();
    });

    it('testRating_500', 0, async function (done) {
      console.info('testRating_500 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 5
          }
        }
        var innerEvent = {
          eventId: 176,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_500 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_500 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_500 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("5.000000");
      console.info('testRating_500 END');
      done();
    });

    it('testRating_600', 0, async function (done) {
      console.info('testRating_600 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 0
          }
        }
        var innerEvent = {
          eventId: 177,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_600 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_600 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("0.000000");
      console.info('testRating_600 END');
      done();
    });

    it('testRating_700', 0, async function (done) {
      console.info('testRating_700 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 1
          }
        }
        var innerEvent = {
          eventId: 178,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_700 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_700 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_700 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("1.000000");
      console.info('testRating_700 END');
      done();
    });

    it('testRating_800', 0, async function (done) {
      console.info('testRating_800 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": -1
          }
        }
        var innerEvent = {
          eventId: 1027,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_800 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_800 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_800 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("0.000000");
      console.info('testRating_800 END');
      done();
    });

    it('testRating_900', 0, async function (done) {
      console.info('testRating_900 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "rating": 'a'
          }
        }
        var innerEvent = {
          eventId: 1028,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testRating_900 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testRating_900 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Rating');
      let obj = JSON.parse(strJson);
      console.info("testRating_900 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.rating).assertEqual("0.000000");
      console.info('testRating_900 END');
      done();
    });
  })
}