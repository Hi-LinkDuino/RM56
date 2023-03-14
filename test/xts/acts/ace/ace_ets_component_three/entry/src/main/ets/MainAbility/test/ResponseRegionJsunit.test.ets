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

export default function ResponseRegionJsunit() {
  describe('responseRegionTest', function () {
    var x_value;
    var y_value;

    beforeEach(async function (done) {
      console.info("ResponseRegion beforeEach start");
      let options = {
        uri: 'pages/ResponseRegion',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get ResponseRegion state pages:" + JSON.stringify(pages));
        if (!("ResponseRegion" == pages.name)) {
          console.info("get ResponseRegion state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push ResponseRegion page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ResponseRegion page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("ResponseRegion after each called");
    });

    it('testResponseRegion01', 0, async function (done) {
      console.info('[testResponseRegion01] START');
      try {
        var eventData = {
          data: {
            "touchable": true,
          }
        }
        var innerEvent = {
          eventId: 30,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testResponseRegion01] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testResponseRegion01] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      console.info('[testResponseRegion01] END');
      done();
    });

    it('testResponseRegion02', 0, async function (done) {
      let rect = await Utils.getComponentRect('region')
      console.info("[testResponseRegion02] rectInfo is " + JSON.stringify(rect));
      x_value = rect.left + (rect.right - rect.left) / 10
      y_value = rect.top + (rect.bottom - rect.top) / 10
      console.info("[testResponseRegion02] onTouch location is: " + "[x]=== " + x_value + "   [y]===" + y_value);
      let point: TouchObject = {
        id: 1,
        x: x_value,
        y: y_value,
        type: TouchType.Down,
      }
      var callbackTwo = (eventData) => {
        console.info("[testResponseRegion02] get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.ACTION).assertEqual(true)
        expect(eventData.data.EVENT_TYPE).assertEqual('Down')
      }
      var innerEventTwo = {
        eventId: 31,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEventTwo, callbackTwo)
      } catch (err) {
        console.info("[testResponseRegion02] on events_emitter err : " + JSON.stringify(err));
      }
      console.info('[testResponseRegion02] testSendTouchEvent ' + sendTouchEvent(point));
      await Utils.sleep(1000)
      console.info('[testResponseRegion02] END');
      done();
    });
  })
}
