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

export default function sourceTypeJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("sourceType beforeEach start");
      let options = {
        uri: 'pages/sourceType',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get sourceType state pages:" + JSON.stringify(pages));
        if (!("sourceType" == pages.name)) {
          console.info("get sourceType state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push sourceType page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push sourceType page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("sourceType after each called");
    });

    it('sourceTypeTest_0100', 0, async function (done) {
      await Utils.sleep(1000)
      let rect = await Utils.getComponentRect('button')
      console.info("sourceTypeTest_0100 rectInfo is " + JSON.stringify(rect));
      let x_value = rect.left + (rect.right - rect.left) / 2
      let y_value = rect.top + (rect.bottom - rect.top) / 2
      console.info("sourceTypeTest_0100 onTouch location is: " + "[x]=== " + x_value + "   [y]===" + y_value);
      let point: TouchObject = {
        id: 1, x: x_value, y: y_value, type: TouchType.Down
      }
      var callback = (eventData) => {
        console.info("sourceTypeTest_0100 get event state result is: " + JSON.stringify(eventData));
        let event = JSON.parse(eventData.data.event);
        expect(event.source).assertEqual(0)
      }
      var innerEvent = {
        eventId: 198,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("sourceTypeTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info('sourceTypeTest_0100 sendTouchEvent :' + sendTouchEvent(point));
      await Utils.sleep(1000)
      console.info('sourceTypeTest_0100 testSendTouchEvent END');
      done();
    });
  })
}