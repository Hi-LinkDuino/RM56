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
import Utils from './Utils.ets';
import events_emitter from '@ohos.events.emitter';

export default function marqueeJsunit() {
  describe('marqueeTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/marquee',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get marquee state success " + JSON.stringify(pages));
        if (!("marquee" == pages.name)) {
          console.info("get marquee state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push marquee page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push marquee page error " + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("marquee after each called");
    });


    it('testMarquee_0100', 0, async function (done) {
      console.info('testMarquee_0100 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("testMarquee_0100 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.start).assertEqual(true);
      }
      let indexEvent = {
        eventId: 130,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("testMarquee_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testMarquee_0100 click result is: " + JSON.stringify(sendEventByKey('button', 10, "")));
      await Utils.sleep(1000);
      var callback1 = (eventData) => {
        console.info("testMarquee_0100 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.fontColor).assertEqual(Color.Blue);
      }
      var innerEventOne = {
        eventId: 127,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEventOne, callback1);
      } catch (err) {
        console.info("testMarquee_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info('testMarquee_0100 END');
      done();
    });

    it('testMarquee_0200', 0, async function (done) {
      console.info('testMarquee_0200 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("testMarquee_0200 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.start).assertEqual(true);
      }
      let indexEvent = {
        eventId: 130,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("testMarquee_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testMarquee_0200 click result is: " + JSON.stringify(sendEventByKey('button', 10, "")));
      await Utils.sleep(1000);
      var callback1 = (eventData) => {
        console.info("testMarquee_0200 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.fontSize).assertEqual(50);
      }
      var innerEventOne = {
        eventId: 128,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEventOne, callback1)
      } catch (err) {
        console.info("testMarquee_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info('testMarquee_0200 END');
      done();
    });

    it('testMarquee_0300', 0, async function (done) {
      console.info('testMarquee_0300 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("testMarquee_0300 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.start).assertEqual(true);
      }
      let indexEvent = {
        eventId: 130,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("testMarquee_0300 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testMarquee_0300 click result is: " + JSON.stringify(sendEventByKey('button', 10, "")));
      await Utils.sleep(1000);
      var callback1 = (eventData) => {
        console.info("testMarquee_0300 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.ACTION).assertEqual(true);
      }
      var innerEventOne = {
        eventId: 129,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEventOne, callback1)
      } catch (err) {
        console.info("testMarquee_0300 on events_emitter err : " + JSON.stringify(err));
      }
      console.info('testMarquee_0300 END');
      done();
    });
  })
}