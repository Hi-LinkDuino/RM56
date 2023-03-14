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

export default function customPopupOptionJsunit(){
  describe('bindPopupTest', function (){
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/bindPopup',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get bindPopup state success " + JSON.stringify(pages));
        if (!("bindPopup" == pages.name)) {
          console.info("get bindPopup state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push bindPopup page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push bindPopup page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("bindPopup after each called");
    });

    it('bindPopupTest_0100', 0, async function (done) {
      console.info('bindPopupTest_0100 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("bindPopupTest_0100 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.enableArrow).assertEqual(true);
        expect(indexEvent.data.maskColor).assertEqual(0x33000000);
      }
      let indexEvent = {
        eventId: 67,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("bindPopupTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("bindPopupTest_0100 click result is: " + JSON.stringify(sendEventByKey('button',10,"")));
      await Utils.sleep(1000);
      console.info("bindPopupTest_0100 click Two result is: " + JSON.stringify(sendEventByKey('image',10,"")));
      await Utils.sleep(1000);
      console.info('bindPopupTest_0100 END');
      done();
    });

    it('bindPopupTest_0200', 0, async function (done) {
      console.info('bindPopupTest_0200 START');
      try {
        let eventData = {
          data: {
            "enableArrow": false,
            "maskColor": 0x33000009
          }
        }
        let indexEvent = {
          eventId: 68,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("bindPopupTest_0200 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("bindPopupTest_0200 change component data error: " + err.message);
      }
      await Utils.sleep(1500);
      let callbackOne = (indexEventOne) => {
        console.info("bindPopupTest_0200 get state result is: " + JSON.stringify(indexEventOne));
        expect(indexEventOne.data.enableArrow).assertEqual(false);
        expect(indexEventOne.data.maskColor).assertEqual(0x33000009);
      }
      let indexEventOne = {
        eventId: 66,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventOne, callbackOne);
      } catch (err) {
        console.info("bindPopupTest_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("bindPopupTest_0200 click result is: " + JSON.stringify(sendEventByKey('button1',10,"")));
      await Utils.sleep(1000);
      console.info('bindPopupTest_0200 END');
      done();
    });

    it('bindPopupTest_0300', 0, async function (done) {
      console.info('bindPopupTest_0300 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("bindPopupTest_0300 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.placementOnTop).assertEqual(false);
      }
      let indexEvent = {
        eventId: 65,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("bindPopupTest_0300 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("bindPopupTest_0300 click result is: " + JSON.stringify(sendEventByKey('button2',10,"")));
      await Utils.sleep(1000);
      console.info('bindPopupTest_0300 END');
      done();
    });
  })
}