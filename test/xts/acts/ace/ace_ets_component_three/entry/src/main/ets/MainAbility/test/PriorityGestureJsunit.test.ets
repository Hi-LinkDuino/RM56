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

export default function priorityGestureJsunit() {
  describe('priorityGestureTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/priorityGesture',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get priorityGesture state success " + JSON.stringify(pages));
        if (!("priorityGesture" == pages.name)) {
          console.info("get priorityGesture state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push priorityGesture page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push priorityGesture page error " + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("priorityGesture after each called");
    });

    it('priorityGestureTest_0100', 0, async function (done) {
      await Utils.sleep(1000)
      let rect = await Utils.getComponentRect('tapGesture')
      console.info("priorityGestureTest_0100 rectInfo is " + JSON.stringify(rect));
      let x_value = rect.left + (rect.right - rect.left) / 2;
      let y_value = rect.top + (rect.bottom - rect.top) / 2;
      console.info("priorityGestureTest_0100 onTouch location is: " + "[x]=== " + x_value + "   [y]===" + y_value);
      let point: TouchObject = {
        id: 11, x: x_value, y: y_value, type: TouchType.Move
      }
      var callback = (eventData) => {
        console.info("priorityGestureTest_0100 get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.value).assertEqual('gesture onAction')
      }
      var innerEvent = {
        eventId: 162,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("priorityGestureTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info('priorityGestureTest_0100 sendTouchEvent one:' + sendTouchEvent(point));
      await Utils.sleep(1000)
      console.info('priorityGestureTest_0100 testSendTouchEvent END');
      done();
    });
  })
}