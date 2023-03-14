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

export default function imageAnimatorJsunit(){
  describe('imageAnimatorTest', function (){
    beforeEach(async function (done) {
      console.info("imageAnimator beforeEach start");
      let options = {
        uri: 'pages/imageAnimator',
      }
      let result;
      try{
        router.clear();
        let pages = router.getState();
        console.info("get imageAnimator state pages: " + JSON.stringify(pages));
        if(!("imageAnimator" == pages.name)){
          console.info("get imageAnimator state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push imageAnimator page result: " + JSON.stringify(result));
        }
      }catch(err){
        console.error("push imageAnimator page error: " + err);
      }
      done()
    });

    afterEach(async function(){
      await Utils.sleep(1000);
      console.info("imageAnimator after each called");
    })

    it('imageAnimatorTest_0100', 0, async function (done) {
      console.info('imageAnimatorTest_0100 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0100 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.state).assertEqual(AnimationStatus.Running);
      }
      let indexEvent = {
        eventId: 115,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0100 click result is: " + JSON.stringify(sendEventByKey('button',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0100 END');
      done();
    });

    it('imageAnimatorTest_0200', 0, async function (done) {
      console.info('imageAnimatorTest_0200 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0200 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.state).assertEqual(AnimationStatus.Paused);
        expect(indexEvent.data.fixedSize).assertEqual(false);
      }
      let indexEvent = {
        eventId: 116,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0200 click result is: " + JSON.stringify(sendEventByKey('button1',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0200 END');
      done();
    });

    it('imageAnimatorTest_0300', 0, async function (done) {
      console.info('imageAnimatorTest_0300 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0300 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.state).assertEqual(AnimationStatus.Stopped);
        expect(indexEvent.data.preDecode).assertEqual(0);
      }
      let indexEvent = {
        eventId: 117,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0300 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0300 click result is: " + JSON.stringify(sendEventByKey('button2',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0300 END');
      done();
    });

    it('imageAnimatorTest_0400', 0, async function (done) {
      console.info('imageAnimatorTest_0400 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0400 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.fillMode).assertEqual(Forwards);
      }
      let indexEvent = {
        eventId: 118,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0400 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0400 click result is: " + JSON.stringify(sendEventByKey('button3',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0400 END');
      done();
    });

    it('imageAnimatorTest_0500', 0, async function (done) {
      console.info('imageAnimatorTest_0500 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0500 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.iterations).assertEqual(1);
      }
      let indexEvent = {
        eventId: 119,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0500 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0500 click result is: " + JSON.stringify(sendEventByKey('button4',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0500 END');
      done();
    });

    it('imageAnimatorTest_0600', 0, async function (done) {
      console.info('imageAnimatorTest_0600 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("imageAnimatorTest_0600 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.string).assertEqual('repeat');
      }
      let indexEvent = {
        eventId: 114,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("imageAnimatorTest_0600 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0600 click result is: " + JSON.stringify(sendEventByKey('button',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0600 END');
      done();
    });

    it('imageAnimatorTest_0700', 0, async function (done) {
      console.info('imageAnimatorTest_0700 START');
      try {
        let eventData = {
          data: {
            "preDecode": 2,
          }
        }
        let indexEvent = {
          eventId: 121,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("imageAnimatorTest_0700 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("imageAnimatorTest_0700 change component data error: " + err.message);
      }
      await Utils.sleep(1500);
      let callbackOne = (indexEventOne) => {
        console.info("imageAnimatorTest_0700 get state result is: " + JSON.stringify(indexEventOne));
        expect(indexEventOne.data.preDecode).assertEqual(2);
      }
      let indexEventOne = {
        eventId: 120,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventOne, callbackOne);
      } catch (err) {
        console.info("imageAnimatorTest_0700 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("imageAnimatorTest_0700 click result is: " + JSON.stringify(sendEventByKey('button5',10,"")));
      await Utils.sleep(1000);
      console.info('imageAnimatorTest_0700 END');
      done();
    });
  })
}