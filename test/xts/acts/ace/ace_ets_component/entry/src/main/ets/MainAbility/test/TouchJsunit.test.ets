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

export default function touchJsunit() {
  describe('touchTest', function () {
    beforeEach(async function (done) {
      console.info("touch beforeEach start");
      let options = {
        uri: 'pages/touch',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get touch state pages: " + JSON.stringify(pages));
        if (!("touch" == pages.name)) {
          console.info("get touch state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push touch page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push touch page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("touch after each called");
    })

    it('testTouch01', 0, async function (done) {
      console.info('[testTouch01] START');
      await Utils.sleep(1000);
      try {
        let callBackTouch = (backData) => {
          console.info("testTouch01 get backEvent result is: " + JSON.stringify(backEvent));
          console.info("testTouch01 get flag result is: " + JSON.stringify(backData.data.ACTION));
          expect(backData.data.ACTION).assertEqual(true);
        }
        var backEvent = {
          eventId: 43,
          priority: events_emitter.EventPriority.LOW
        }
        events_emitter.on(backEvent, callBackTouch);
      } catch (err) {
        console.info("testTouch01 on touchEvent err : " + JSON.stringify(err));
      }
      console.info("testTouch01 click result is: " + JSON.stringify(sendEventByKey('touch', 10, "")));
      console.info('[testTouch01] END');
      done();
    });
  })
}
