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

export default function placementJsunit(){
  describe('placementTest', function (){
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/placement',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get placement state success " + JSON.stringify(pages));
        if (!("placement" == pages.name)) {
          console.info("get placement state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push placement page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push placement page error " + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("placement after each called");
    });

    it('placementTest_0100', 0, async function (done) {
      console.info('placementTest_0100 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("placementTest_0100 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.placement).assertEqual(Placement.TopLeft);
      }
      let indexEvent = {
        eventId: 157,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("placementTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("placementTest_0100 click result is: " + JSON.stringify(sendEventByKey('button',10,"")));
      await Utils.sleep(1500);
      console.info("placementTest_0100 clickOne result is: " + JSON.stringify(sendEventByKey('image',10,"")));
      console.info('placementTest_0100 END');
      done();
    });

    it('placementTest_0200', 0, async function (done) {
      console.info('placementTest_0200 START');
      await Utils.sleep(1500);
      let callbackOne = (indexEventOne) => {
        console.info("placementTest_0200 get state result is: " + JSON.stringify(indexEventOne));
        expect(indexEventOne.data.placement).assertEqual(Placement.TopRight);
      }
      let indexEventOne = {
        eventId: 158,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventOne, callbackOne);
      } catch (err) {
        console.info("placementTest_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("placementTest_0200 click result is: " + JSON.stringify(sendEventByKey('button1',10,"")));
      await Utils.sleep(1500);
      console.info("placementTest_0200 clickOne result is: " + JSON.stringify(sendEventByKey('image1',10,"")));
      console.info('placementTest_0200 END');
      done();
    });

    it('placementTest_0300', 0, async function (done) {
      console.info('placementTest_0300 START');
      await Utils.sleep(1500);
      let callbackTwo = (indexEventTwo) => {
        console.info("placementTest_0300 get state result is: " + JSON.stringify(indexEventTwo));
        expect(indexEventTwo.data.placement).assertEqual(Placement.BottomLeft);
      }
      let indexEventTwo = {
        eventId: 159,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventTwo, callbackTwo);
      } catch (err) {
        console.info("placementTest_0300 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("placementTest_0300 click result is: " + JSON.stringify(sendEventByKey('button2',10,"")));
      await Utils.sleep(1500);
      console.info("placementTest_0300 clickOne result is: " + JSON.stringify(sendEventByKey('image2',10,"")));
      console.info('placementTest_0300 END');
      done();
    });

    it('placementTest_0400', 0, async function (done) {
      console.info('placementTest_0400 START');
      await Utils.sleep(1500);
      let callbackThree = (indexEventThree) => {
        console.info("placementTest_0400 get state result is: " + JSON.stringify(indexEventThree));
        expect(indexEventThree.data.placement).assertEqual(Placement.BottomRight);
      }
      let indexEventThree = {
        eventId: 160,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventThree, callbackThree);
      } catch (err) {
        console.info("placementTest_0400 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("placementTest_0400 click result is: " + JSON.stringify(sendEventByKey('button3',10,"")));
      await Utils.sleep(1500);
      console.info("placementTest_0400 clickOne result is: " + JSON.stringify(sendEventByKey('image3',10,"")));
      console.info('placementTest_0400 END');
      done();
    });
  })
}