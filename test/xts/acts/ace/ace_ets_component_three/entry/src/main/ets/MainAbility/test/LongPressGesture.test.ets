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

export default function longPressGestureJsunit() {
  describe('longPressGestureTest', function () {
    beforeEach(async function (done) {
      console.info("longPressGesture beforeEach start");
      let options = {
        uri: 'pages/longPressGesture',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get longPressGesture state pages:" + JSON.stringify(pages));
        if (!("longPressGesture" == pages.name)) {
          console.info("get longPressGesture state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push longPressGesture page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push longPressGesture page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("longPressGesture after each called");
    });

    it('testLongPressGesture01', 0, async function (done) {
      console.info('[testLongPressGesture01] START');
      await Utils.sleep(1000);
      let callback = (eventData) => {
        console.info("testLongPressGesture01 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.ACTION).assertEqual('onAction')
      }
      let innerEvent = {
        eventId: 125,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        console.info("testLongPressGesture01 on events_emitter on");
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("testLongPressGesture01 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testLongPressGesture01 sendEventByKey result: " + JSON.stringify(sendEventByKey('flex', 11, "")));
      await Utils.sleep(1000);
      console.info('testLongPressGesture01 END');
      done();
    });

    it('testLongPressGesture02', 0, async function (done) {
      console.info('[testLongPressGesture02] START');
      await Utils.sleep(1000);
      let callback = (eventData) => {
        console.info("testLongPressGesture02 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.ACTION).assertEqual('onActionEnd')
      }
      let innerEvent = {
        eventId: 126,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        console.info("testLongPressGesture02 on events_emitter on");
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("testLongPressGesture02 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testLongPressGesture02 sendEventByKey result: " + JSON.stringify(sendEventByKey('flex', 11, "")));
      await Utils.sleep(1000);
      console.info('testLongPressGesture02 END');
      done();
    });
  })
}
