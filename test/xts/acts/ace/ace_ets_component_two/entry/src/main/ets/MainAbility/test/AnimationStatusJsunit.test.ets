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

export default function  animationStatusJsunit() {
  describe('animationStatus', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/animationStatus',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get animationStatus state success " + JSON.stringify(pages));
        if (!("animationStatus" == pages.name)) {
          console.info("get animationStatus state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push animationStatus page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push animationStatus page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("animationStatus after each called");
    });

    it('animationStatus01', 0, async function (done) {
      console.info('[animationStatus01] START');
      await Utils.sleep(1000);

      let strJson = getInspectorByKey('ImageAnimator');
      let obj = JSON.parse(strJson);
      console.info("[animationStatus01] obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('ImageAnimator');
      console.info('[animationStatus01] END');
      done();
    });

    it('animationStatus02', 0, async function (done) {
      console.info('[animationStatus02] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ImageAnimator');
      let obj = JSON.parse(strJson);
      console.info("[animationStatus02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.state).assertEqual('AnimationStatus.Initial');
      console.info('[animationStatus02] END');
      done();
    });

    it('animationStatus03', 0, async function (done) {
      console.info('animationStatus03 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("animationStatus03 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.animationStatus).assertEqual(AnimationStatus.Paused);
      }
      let indexEvent = {
        eventId: 56,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("animationStatus03 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("animationStatus03 click result is: " + JSON.stringify(sendEventByKey('button2',10,"")));
      await Utils.sleep(1000);
      console.info('animationStatus03 END');
      done();
    });

    it('animationStatus04', 0, async function (done) {
      console.info('animationStatus04 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("animationStatus04 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.animationStatus).assertEqual(AnimationStatus.Stopped);
      }
      let indexEvent = {
        eventId: 57,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("animationStatus04 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("animationStatus04 click result is: " + JSON.stringify(sendEventByKey('button3',10,"")));
      await Utils.sleep(1000);
      console.info('animationStatus04 END');
      done();
    });
  })
}